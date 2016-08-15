/*! 
	\file nsasset.cpp
	
	\brief Definition file for nsasset class

	This file contains all of the neccessary definitions for the nsasset class.

	\author Daniel Randle
	\date November 23 2013
	\copywrite Earth Banana Games 2013
*/

#include <asset/nsasset.h>
#ifdef NOTIFY_ENGINE_NAME_CHANGE
#include <nsengine.h>
#endif

nsasset::nsasset(uint32 type_hash) :
	m_icon_path(),
	m_icon_tex_id(),
	m_name(),
	m_subdir(),
	m_id(0),
	m_plugin_id(0),
	m_hashed_type(type_hash),
	m_owned(false),
	m_save_with_group(true)
{}

nsasset::nsasset(const nsasset & copy):
	m_hashed_type(copy.m_hashed_type),
	m_icon_path(copy.m_icon_path),
	m_icon_tex_id(copy.m_icon_tex_id),
	m_name(copy.m_name),
	m_subdir(copy.m_subdir),
	m_ext(copy.m_ext),
	m_id(copy.m_id),
	m_plugin_id(copy.m_plugin_id),
	m_owned(false),
	m_save_with_group(true)
{}

nsasset::~nsasset()
{
	resource_destroyed(type(), full_id());
}

const nsstring & nsasset::extension() const
{
	return m_ext;
}

uivec2 nsasset::full_id()
{
	return uivec2(m_plugin_id, m_id);
}

void nsasset::set_icon_path(const nsstring & pIconPath)
{
	m_icon_path = pIconPath;
}

void nsasset::set_icon_tex_id(const uivec2 & texID)
{
	m_icon_tex_id = texID;
}

const nsstring & nsasset::name() const
{
	return m_name;
}

uint32 nsasset::plugin_id() const
{
	return m_plugin_id;
}

uint32 nsasset::id() const
{
	return m_id;
}

const nsstring & nsasset::subdir() const
{
	return m_subdir;
}

const nsstring & nsasset::icon_path() const
{
	return m_icon_path;
}

void nsasset::enable_group_save(bool enable)
{
	m_save_with_group = enable;
}

bool nsasset::group_save_enabled()
{
	return m_save_with_group;
}

const uivec2 & nsasset::icon_tex_id() const
{
	return m_icon_tex_id;
}

/*!
  Get the other resources that this resource uses. If no other resources are used then leave this unimplemented - will return an empty map.
*/
uivec3_vector  nsasset::resources()
{
	return uivec3_vector();
}

/*!
  This should be called if there was a name change to a resource - will check if the resource is used by this component and if is
  is then it will update the handle
*/
void nsasset::name_change(const uivec2 & oldid, const uivec2 newid)
{
	// do nothing
}

uint32 nsasset::type() const
{
	return m_hashed_type;
}

void nsasset::set_ext(const nsstring & pExt)
{
	m_ext = pExt;
}

void nsasset::rename(const nsstring & pRefName)
{
	uint32 tmp = m_id;

	m_name = pRefName;
	m_id = hash_id(pRefName);

#ifdef NOTIFY_ENGINE_NAME_CHANGE
	if (m_owned) // if a manager owns this resource - otherwise we dont care
		nse.name_change(uivec2(m_plugin_id, tmp),uivec2(m_plugin_id, m_id));
#endif
}

void nsasset::set_subdir(const nsstring & pDir)
{
	m_subdir = pDir;
}

nsasset & nsasset::operator=(nsasset_inst rhs)
{
	std::swap(m_hashed_type, rhs.m_hashed_type);
	std::swap(m_icon_path, rhs.m_icon_path);
	std::swap(m_icon_tex_id, rhs.m_icon_tex_id);
	std::swap(m_name, rhs.m_name);
	std::swap(m_subdir, rhs.m_subdir);
	std::swap(m_ext, rhs.m_ext);
	std::swap(m_id, rhs.m_id);
	std::swap(m_plugin_id, rhs.m_plugin_id);
	std::swap(m_owned, rhs.m_owned);
	return *this;
}
