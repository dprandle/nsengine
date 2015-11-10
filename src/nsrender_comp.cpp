/*! 
	\file nsrendercomp.cpp
	
	\brief Definition file for nsrender_comp class

	This file contains all of the neccessary definitions for the nsrender_comp class.

	\author Daniel Randle
	\date December 17 2013
	\copywrite Earth Banana Games 2013
*/

#include <nsrender_comp.h>
#include <nsmesh.h>
#include <nsentity.h>
#include <nstimer.h>
#include <nsmaterial.h>
#include <nsmat_manager.h>
#include <nsengine.h>
#include <nsmesh_manager.h>


nsrender_comp::nsrender_comp() : 
nscomponent(),
m_cast_shadow(true),
m_mesh_id(0)
{}

nsrender_comp::~nsrender_comp()
{}

void nsrender_comp::clear_mats()
{
	m_mats.clear();
}

nsrender_comp* nsrender_comp::copy(const nscomponent * pToCopy)
{
	if (pToCopy == NULL)
		return NULL;
	const nsrender_comp * comp = (const nsrender_comp*)pToCopy;
	(*this) = (*comp);
	return this;
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

	// only add resources if they are not 0
	if (m_mesh_id != 0)
		ret.push_back(uivec3(m_mesh_id, type_to_hash(nsmesh)));

	// Add all materials
	auto iter = begin_mat();
	while (iter != end_mat())
	{
		if (iter->second != 0) // only add if the submesh has a material assigned (it might not)
			ret.push_back(uivec3(iter->second, type_to_hash(nsmaterial)));
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

nsrender_comp & nsrender_comp::operator=(const nsrender_comp & pRHSComp)
{
	m_mesh_id = pRHSComp.m_mesh_id;
	m_cast_shadow = pRHSComp.m_cast_shadow;
	
	m_mats.clear();
	m_mats.insert(pRHSComp.m_mats.begin(), pRHSComp.m_mats.end());
	post_update(true);
	return (*this);
}
