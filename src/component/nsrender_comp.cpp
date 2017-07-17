/*! 
	\file nsrendercomp.cpp
	
	\brief Definition file for nsrender_comp class

	This file contains all of the neccessary definitions for the nsrender_comp class.

	\author Daniel Randle
	\date December 17 2013
	\copywrite Earth Banana Games 2013
*/

#include <component/nsrender_comp.h>
#include <asset/nsmesh.h>
#include <nsentity.h>
#include <nstimer.h>
#include <asset/nsmaterial.h>
#include <asset/nsmat_manager.h>
#include <nsengine.h>
#include <asset/nsmesh_manager.h>


nsrender_comp::nsrender_comp() : 
	nscomponent(type_to_hash(nsrender_comp)),
	m_cast_shadow(true),
	m_mesh_id(0),
	transparent_picking(false)
{}

nsrender_comp::~nsrender_comp()
{}

nsrender_comp::nsrender_comp(const nsrender_comp & rend_comp):
	nscomponent(rend_comp),
	m_cast_shadow(rend_comp.m_cast_shadow),
	m_mesh_id(rend_comp.m_mesh_id),
	m_mats(rend_comp.m_mats),
	transparent_picking(rend_comp.transparent_picking)
{}
	
void nsrender_comp::clear_mats()
{
	m_mats.clear();
}

bool nsrender_comp::cast_shadow()
{
	return m_cast_shadow;
}

uivec2 nsrender_comp::material_id(uint32 subMeshIndex)
{
	auto fiter = m_mats.find(subMeshIndex);
	if (fiter != m_mats.end())
		return fiter->second;
	return uivec2();
}

const uivec2 & nsrender_comp::mesh_id()
{
	return m_mesh_id;
}

void nsrender_comp::name_change(const uivec2 & oldid, const uivec2 newid)
{
	if (m_mesh_id.x == oldid.x)
	{
		m_mesh_id.x = newid.x;
		if (m_mesh_id.y == oldid.y)
			m_mesh_id.y = newid.y;
	}

	// Add all materials
	auto iter = begin_mat();
	while (iter != end_mat())
	{
		if (iter->second.x == oldid.x)
		{
			iter->second.x = newid.x;
			if (iter->second.y == oldid.y)
				iter->second.y = newid.y;
		}
		++iter;
	}
}

/*!
Get the resources that the component uses. The render comp uses a mesh and possibly multiple materials.
*/
uivec3_vector nsrender_comp::resources()
{
	// Build map
	uivec3_vector ret;

	nsmesh * _mesh_ = get_asset<nsmesh>(m_mesh_id);
	if (_mesh_ != NULL)
	{
		uivec3_vector tmp = _mesh_->resources();
		ret.insert(ret.end(), tmp.begin(), tmp.end());
		ret.push_back(uivec3(_mesh_->full_id(), type_to_hash(nsmesh)));
	}

	// Add all materials
	auto iter = begin_mat();
	while (iter != end_mat())
	{
		nsmaterial * mat = get_asset<nsmaterial>(iter->second);
		if (mat != NULL)
		{
			uivec3_vector tmp = mat->resources();
			ret.insert(ret.end(), tmp.begin(), tmp.end());
			ret.push_back(uivec3(mat->full_id(), type_to_hash(nsmaterial)));
		}
		++iter;
	}

	return ret;
}

void nsrender_comp::pup(nsfile_pupper * p)
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

bool nsrender_comp::has_material(uint32 pSubMeshIndex)
{
	return (m_mats.find(pSubMeshIndex) != m_mats.end());
}


void nsrender_comp::init()
{}

ui_uivec2_map::iterator nsrender_comp::begin_mat()
{
	return m_mats.begin();
}

ui_uivec2_map::const_iterator nsrender_comp::begin_mat() const
{
	return m_mats.begin();
}

ui_uivec2_map::iterator nsrender_comp::end_mat()
{
	return m_mats.end();
}

ui_uivec2_map::const_iterator nsrender_comp::end_mat() const
{
	return m_mats.end();
}

bool nsrender_comp::remove_material(uint32 pSubMeshIndex)
{
	if (!has_material(pSubMeshIndex))
		return false;
	m_mats.erase(pSubMeshIndex);
	return true;
}


bool nsrender_comp::remove_all_materials(const uivec2 & toremove)
{
	ui_uivec2_map::iterator iter = m_mats.begin();
	bool ret = false;
	while (iter != m_mats.end())
	{
		if (iter->second == toremove)
		{
			iter = m_mats.erase(iter);
			ret = true;
		}
		else
			++iter;
	}
	return ret;
}


bool nsrender_comp::replace_material(const uivec2 & oldid, const uivec2 & newid)
{
	ui_uivec2_map::iterator iter = m_mats.begin();
	bool ret = false;
	while (iter != m_mats.end())
	{
		if (iter->second == oldid)
		{
			iter->second = newid;
			ret = true;
		}
		++iter;
	}
	return ret;
}


bool nsrender_comp::set_material(uint32 pSubMeshIndex, const uivec2 & pMatID, bool pReplace)
{
	if (has_material(pSubMeshIndex))
	{
		if (!pReplace)
			return false;
		m_mats.at(pSubMeshIndex) = pMatID;
		return true;
	}
	m_mats[pSubMeshIndex] = pMatID;
	return true;
}


void nsrender_comp::set_cast_shadow(bool pShadow)
{
	m_cast_shadow = pShadow;
}

void nsrender_comp::set_mesh_id(const uivec2 & pMeshID)
{
	m_mesh_id = pMeshID;
}

bool nsrender_comp::operator==(const nsrender_comp & rhs_)
{
	return (m_mesh_id == rhs_.m_mesh_id && m_mats == rhs_.m_mats && transparent_picking == rhs_.transparent_picking);
}

nsrender_comp & nsrender_comp::operator=(nsrender_comp rhs_)
{
	nscomponent::operator=(rhs_);
	std::swap(transparent_picking, rhs_.transparent_picking);
	std::swap(m_cast_shadow, rhs_.m_cast_shadow);
	std::swap(m_mesh_id, rhs_.m_mesh_id);
	std::swap(m_mats, rhs_.m_mats);
	post_update(true);
	return (*this);
}
