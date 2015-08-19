/*!
\file nsoccupycomp.cpp

\brief Definition file for nsoccupy_comp class

This file contains all of the neccessary definitions for the nsoccupy_comp class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/


#include <nsoccupy_comp.h>
#include <nsentity.h>
#include <nsplugin.h>
#include <cmath>

nsoccupy_comp::nsoccupy_comp() :
m_draw_enabled(false),
m_mesh_id(),
m_mat_id(),
NSComponent()
{
	add(0, 0, 0);
	nsmesh * occ = nsengine.core()->get<nsmesh>(MESH_FULL_TILE);
	if (occ == NULL)
	{
		dprint("nsoccupy_comp::nsoccupy_comp Could not get occupy mesh");
	}
	else
	{
		m_mesh_id = occ->full_id();
	}
}

nsoccupy_comp::~nsoccupy_comp()
{}

bool nsoccupy_comp::add(int32 x, int32 y, int32 z)
{
	//if (contains(x, y, z))
		//return false;

	return add(ivec3(x, y, z));
}

bool nsoccupy_comp::add(const ivec3 & pGridPos)
{
	m_spaces.push_back(pGridPos);
	return true;
}

ivec3array::iterator nsoccupy_comp::begin()
{
	return m_spaces.begin();
}

/*!
This function builds the occupied spaces from the bounding box around the object, assuming that the
origin is the center of the object. This should be true for any object imported in to the engine.
This calculation is very basic and the actual tiles occupied should be refined - That is the algorithm
uses the bounding box of the entire mesh.
*/
void nsoccupy_comp::build(const nsbounding_box & pBox)
{
	int32 zCountUp = int32(abs(pBox.mMax.z) / Z_GRID);
	if (fmod(abs(pBox.mMax.z), Z_GRID) > 0.01)
		++zCountUp;

	int32 zCountDown = int32(abs(pBox.mMin.z) / Z_GRID);
	if (fmod(abs(pBox.mMin.z), Z_GRID) > 0.01)
		++zCountDown;

	int32 xCountRight = int32(abs(pBox.mMax.x) / X_GRID);
	if (fmod(abs(pBox.mMax.x), X_GRID) > 0.01)
		++xCountRight;

	int32 xCountLeft = int32(abs(pBox.mMin.x) / X_GRID);
	if (fmod(abs(pBox.mMin.x), X_GRID) > 0.01)
		++xCountLeft;

	int32 yCountForward = int32(abs(pBox.mMax.y) / Y_GRID);
	if (fmod(abs(pBox.mMax.y), Y_GRID) > 0.01)
		++yCountForward;

	int32 yCountBackward = int32(abs(pBox.mMin.y) / Y_GRID);
	if (fmod(abs(pBox.mMin.z), Y_GRID) > 0.01)
		++yCountBackward;



	for (int32 i = 0; i < zCountUp; ++i)
	{
		for (int32 j = 0; j < yCountForward; ++j)
		{
			int32 mod = j % 2;
			for (int32 k = 0; k < xCountRight; ++k)
			{
				int32 mod2 = k % 2;
				if (!mod) // if mod == 0 - basically if j is even
				{
					if (!mod2) // if k is even
						add(k/2, j, i);
				}
				else // if j is odd
				{
					if (mod2) // if k is odd
						add((k)/2, j, i);
				}
			}
			for (int32 k = 1; k < xCountLeft; ++k)
			{
				int32 mod2 = k % 2;
				if (!mod) // if mod == 0 - basically if j is even
				{
					if (!mod2) // if k is even
						add(-k/2, j, i);
				}
				else // if j is odd
				{
					if (mod2) // if k is odd
						add((-k-1)/2, j, i);
				}
			}
		}
		for (int32 j = 1; j < yCountBackward; ++j)
		{
			int32 mod = j % 2;
			for (int32 k = 0; k < xCountRight; ++k)
			{
				int32 mod2 = k % 2;
				if (!mod) // if mod == 0 - basically if j is even
				{
					if (!mod2) // if k is even
						add(k/2, -j, i);
				}
				else // if j is odd
				{
					if (mod2) // if k is odd
						add((k)/2, -j, i);
				}
			}
			for (int32 k = 1; k < xCountLeft; ++k)
			{
				int32 mod2 = k % 2;
				if (!mod) // if mod == 0 - basically if j is even
				{
					if (!mod2) // if k is even
						add(-k/2, -j, i);
				}
				else // if j is odd
				{
					if (mod2) // if k is odd
						add((-k-1)/2, -j, i);
				}
			}
		}
	}

	for (int32 i = 1; i < zCountDown; ++i)
	{
		for (int32 j = 0; j < yCountForward; ++j)
		{
			int32 mod = j % 2;
			for (int32 k = 0; k < xCountRight; ++k)
			{
				int32 mod2 = k % 2;
				if (!mod) // if mod == 0 - basically if j is even
				{
					if (!mod2) // if k is even
						add(k/2, j, -i);
				}
				else // if j is odd
				{
					if (mod2) // if k is odd
						add((k)/2, j, -i);
				}
			}
			for (int32 k = 1; k < xCountLeft; ++k)
			{
				int32 mod2 = k % 2;
				if (!mod) // if mod == 0 - basically if j is even
				{
					if (!mod2) // if k is even
						add(-k/2, j, -i);
				}
				else // if j is odd
				{
					if (mod2) // if k is odd
						add((-k-1)/2, j, -i);
				}
			}
		}
		for (int32 j = 1; j < yCountBackward; ++j)
		{
			int32 mod = j % 2;
			for (int32 k = 0; k < xCountRight; ++k)
			{
				int32 mod2 = k % 2;
				if (!mod) // if mod == 0 - basically if j is even
				{
					if (!mod2) // if k is even
						add(k/2, -j, -i);
				}
				else // if j is odd
				{
					if (mod2) // if k is odd
						add((k)/2, -j, -i);
				}
			}
			for (int32 k = 1; k < xCountLeft; ++k)
			{
				int32 mod2 = k % 2;
				if (!mod) // if mod == 0 - basically if j is even
				{
					if (!mod2) // if k is even
						add(-k/2, -j, -i);
				}
				else // if j is odd
				{
					if (mod2) // if k is odd
						add((-k-1)/2, -j, -i);
				}
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

nsoccupy_comp* nsoccupy_comp::copy(const NSComponent * pToCopy)
{
	if (pToCopy == NULL)
		return NULL;
	const nsoccupy_comp * comp = (const nsoccupy_comp*)pToCopy;
	(*this) = (*comp);
	return this;
}

void nsoccupy_comp::enable_draw(bool pEnable)
{
	m_draw_enabled = pEnable;
}

ivec3array::iterator nsoccupy_comp::end()
{
	return m_spaces.end();
}

const ivec3array & nsoccupy_comp::spaces() const
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

uivec2array nsoccupy_comp::resources()
{
	uivec2array ret;
	if (m_mat_id != 0)
		ret.push_back(m_mat_id);
	if (m_mesh_id != 0)
		ret.push_back(m_mesh_id);
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

nsoccupy_comp & nsoccupy_comp::operator=(const nsoccupy_comp & pRHSComp)
{
	m_spaces.resize(pRHSComp.m_spaces.size());
	for (uint32 i = 0; i < m_spaces.size(); ++i)
		m_spaces[i] = pRHSComp.m_spaces[i];
	m_mesh_id = pRHSComp.m_mesh_id;
	m_mat_id = pRHSComp.m_mat_id;
	m_draw_enabled = pRHSComp.m_draw_enabled;
	post_update(true);
	return (*this);
}
