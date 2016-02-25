/*! 
	\file nsresource.cpp
	
	\brief Definition file for nsresource class

	This file contains all of the neccessary definitions for the nsresource class.

	\author Daniel Randle
	\date November 23 2013
	\copywrite Earth Banana Games 2013
*/

#include <nsresource.h>
#ifdef NOTIFY_ENGINE_NAME_CHANGE
#include <nsengine.h>
#endif

nsresource::nsresource() :
	m_icon_path(),
	m_icon_tex_id(),
	m_name(),
	m_subdir(),
	m_id(0),
	m_plugin_id(0),
	m_hashed_type(0),
	m_owned(false),
	m_ctxt_id(-1)
{}

nsresource::nsresource(const nsresource & copy):
	m_hashed_type(copy.m_hashed_type),
	m_icon_path(copy.m_icon_path),
	m_icon_tex_id(copy.m_icon_tex_id),
	m_name(copy.m_name),
	m_subdir(copy.m_subdir),
	m_ext(copy.m_ext),
	m_id(copy.m_id),
	m_plugin_id(copy.m_plugin_id),
	m_owned(false)
{}

nsresource::~nsresource()
{}

const nsstring & nsresource::extension() const
{
	return m_ext;
}

uivec2 nsresource::full_id()
{
	return uivec2(m_plugin_id, m_id);
}

void nsresource::set_icon_path(const nsstring & pIconPath)
{
	m_icon_path = pIconPath;
}

void nsresource::set_icon_tex_id(const uivec2 & texID)
{
	m_icon_tex_id = texID;
}

const nsstring & nsresource::name() const
{
	return m_name;
}

uint32 nsresource::plugin_id() const
{
	return m_plugin_id;
}

uint32 nsresource::id() const
{
	return m_id;
}

uint32 nsresource::context_id()
{
	return m_ctxt_id;
}

void nsresource::set_context_id(uint32 id)
{
	m_ctxt_id = id;
}

const nsstring & nsresource::subdir() const
{
	return m_subdir;
}

const nsstring & nsresource::icon_path() const
{
	return m_icon_path;
}

const uivec2 & nsresource::icon_tex_id() const
{
	return m_icon_tex_id;
}

/*!
  Get the other resources that this resource uses. If no other resources are used then leave this unimplemented - will return an empty map.
*/
uivec3_vector  nsresource::resources()
{
	return uivec3_vector();
}

/*!
  This should be called if there was a name change to a resource - will check if the resource is used by this component and if is
  is then it will update the handle
*/
void nsresource::name_change(const uivec2 & oldid, const uivec2 newid)
{
	// do nothing
}

uint32 nsresource::type() const
{
	return m_hashed_type;
}

void nsresource::set_ext(const nsstring & pExt)
{
	m_ext = pExt;
}

void nsresource::rename(const nsstring & pRefName)
{
	uint32 tmp = m_id;

	m_name = pRefName;
	m_id = hash_id(pRefName);

#ifdef NOTIFY_ENGINE_NAME_CHANGE
	if (m_owned) // if a manager owns this resource - otherwise we dont care
		nse.name_change(uivec2(m_plugin_id, tmp),uivec2(m_plugin_id, m_id));
#endif
}

void nsresource::set_subdir(const nsstring & pDir)
{
	m_subdir = pDir;
}

nsresource & nsresource::operator=(nsresource_inst rhs)
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
