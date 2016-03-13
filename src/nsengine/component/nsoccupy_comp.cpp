/*!
\file nsoccupycomp.cpp

\brief Definition file for nsoccupy_comp class

This file contains all of the neccessary definitions for the nsoccupy_comp class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/


#include <nsoccupy_comp.h>
#include <nstile_grid.h>
#include <nsentity.h>
#include <nsplugin.h>
#include <cmath>
#include <nsmesh.h>
#include <nsmaterial.h>
#include <nsrender_system.h>
#include <nsengine.h>

nsoccupy_comp::nsoccupy_comp() :
	nscomponent(),
	m_spaces(),
	m_draw_enabled(false),
	m_mesh_id(),
	m_mat_id()
{}

nsoccupy_comp::nsoccupy_comp(const nsoccupy_comp & copy):
	nscomponent(copy),
	m_spaces(copy.m_spaces),
	m_draw_enabled(copy.m_draw_enabled),
	m_mesh_id(copy.m_mesh_id),
	m_mat_id(copy.m_mat_id)
{}

nsoccupy_comp::~nsoccupy_comp()
{}

bool nsoccupy_comp::add(int32 x, int32 y, int32 z)
{
	return add(ivec3(x, y, z));
}

bool nsoccupy_comp::add(const ivec3 & pGridPos)
{
	m_spaces.push_back(pGridPos);
	return true;
}

ivec3_vector::iterator nsoccupy_comp::begin()
{
	return m_spaces.begin();
}

void nsoccupy_comp::clear()
{
	m_spaces.clear();
}
/*!
This function builds the occupied spaces from the bounding box around the object, assuming that the
origin is the center of the object. This should be true for any object imported in to the engine.
This calculation is very basic and the actual tiles occupied should be refined - That is the algorithm
uses the bounding box of the entire mesh.
*/
void nsoccupy_comp::build(const nsbounding_box & pBox)
{
	clear();
	fvec4 plus, minus;
	ivec4 iplus, iminus;
	
	plus.x = (0.5f * (pBox.mMax.x + X_GRID)/X_GRID);
	minus.x = (0.5f * (pBox.mMin.x + X_GRID)/X_GRID);
	plus.w = (0.5f * (pBox.mMax.x/X_GRID));
	minus.w = (0.5f * (pBox.mMin.x/X_GRID));
	plus.y = ((pBox.mMax.y + 0.5f) / Y_GRID);
	minus.y = ((pBox.mMin.y + 1.0f) / Y_GRID);
	plus.z = (pBox.mMax.z/Z_GRID);
	minus.z = (pBox.mMin.z/Z_GRID);

	for (uint32 i = 0; i < 4; ++i)
	{
		if (plus[i] >= 0.0f)
			iplus[i] = std::floor(plus[i]);
		else
			iplus[i] = std::ceil(plus[i]);

		if (minus[i] >= 0.0f)
			iminus[i] = std::floor(minus[i]);
		else
			iminus[i] = std::ceil(minus[i]);

		if (std::abs(fract(plus[i])) > R_FACTOR)
			iplus[i] += 1;
		
		if (std::abs(fract(minus[i])) > R_FACTOR)
			iminus[i] -= 1;
	}

	for (int32 z = iplus.z; z > iminus.z; --z)
	{
		for (int32 y = iminus.y; y < iplus.y; ++y)
		{
			if ((y % 2) == 0)
			{
				for (int32 x = iminus.x; x < iplus.x; ++x)
					add(x,y,z);
			}
			else
			{
				for (int32 x = iminus.w; x < iplus.w; ++x)
					add(x,y,z);
			}
		}
	}
}

void nsoccupy_comp::pup(nsfile_pupper * p)
{
	if (p->type() == nsfile_pupper::pup_binary)
	{
		nsbinary_file_pupper * bf = static_cast<nsbinary_file_pupper *>(p);
		::pup(*bf, *this);
	}
	else
	{
		nstext_file_pupper * tf = static_cast<nstext_file_pupper *>(p);
		::pup(*tf, *this);
	}
}

bool nsoccupy_comp::contains(int32 x, int32 y, int32 z)
{
	return contains(ivec3(x, y, z));
}

bool nsoccupy_comp::contains(const ivec3 & pGridPos)
{
	auto iter = m_spaces.begin();
	while (iter != m_spaces.end())
	{
		if (*iter == pGridPos)
			return true;
		++iter;
	}
	return false;
}

void nsoccupy_comp::enable_draw(bool pEnable)
{
	m_draw_enabled = pEnable;
}

ivec3_vector::iterator nsoccupy_comp::end()
{
	return m_spaces.end();
}

const ivec3_vector & nsoccupy_comp::spaces() const
{
	return m_spaces;
}

void nsoccupy_comp::init()
{
}

bool nsoccupy_comp::draw_enabled()
{
	return m_draw_enabled;
}

const uivec2 & nsoccupy_comp::mesh_id()
{
	return m_mesh_id;
}

const uivec2 & nsoccupy_comp::material_id()
{
	return m_mesh_id;
}

void nsoccupy_comp::name_change(uint32 plugID, uint32 oldID, uint32 newID)
{
	if (m_mesh_id == uivec2(plugID, oldID))
		m_mesh_id.y = newID;
	if (m_mat_id == uivec2(plugID, oldID))
		m_mat_id.y = newID;
}

bool nsoccupy_comp::remove(int32 x, int32 y, int32 z)
{
	return remove(ivec3(x,y,z));
}

bool nsoccupy_comp::remove(const ivec3 & pGridPos)
{
	auto iter = m_spaces.begin();
	while (iter != m_spaces.end())
	{
		if (*iter == pGridPos)
		{
			iter = m_spaces.erase(iter);
			return true;
		}
		++iter;
	}
	return false;
}

uivec3_vector nsoccupy_comp::resources()
{
	uivec3_vector ret;

	nsmaterial * _mat_ = get_resource<nsmaterial>(m_mat_id);
	if (_mat_ != NULL)
	{
		uivec3_vector tmp = _mat_->resources();
		ret.insert(ret.end(), tmp.begin(), tmp.end());
		ret.push_back(uivec3(_mat_->full_id(), type_to_hash(nsmaterial)));
	}
	
	nsmesh * _mesh_ = get_resource<nsmesh>(m_mesh_id);
	if (_mesh_ != NULL)
	{
		uivec3_vector tmp = _mesh_->resources();
		ret.insert(ret.end(), tmp.begin(), tmp.end());
		ret.push_back(uivec3(_mesh_->full_id(), type_to_hash(nsmesh)));
	}
	return ret;
}

void nsoccupy_comp::set_mesh_id(const uivec2 & mesh)
{
	m_mesh_id = mesh;
	post_update(true);
}

void nsoccupy_comp::set_material_id(const uivec2 & mat)
{
	m_mat_id = mat;
	post_update(true);
}

nsoccupy_comp & nsoccupy_comp::operator=(nsoccupy_comp rhs_)
{
	nscomponent::operator=(rhs_);
	std::swap(m_spaces, rhs_.m_spaces);
	std::swap(m_mesh_id, rhs_.m_mesh_id);
	std::swap(m_mat_id, rhs_.m_mat_id);
	std::swap(m_draw_enabled, rhs_.m_draw_enabled);
	post_update(true);
	return (*this);
}
