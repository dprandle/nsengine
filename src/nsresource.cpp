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
m_owned(false)
{}

nsresource::nsresource(const nsresource & copy):
	m_icon_path(copy.m_icon_path),
	m_icon_tex_id(copy.m_icon_tex_id),
	m_name(copy.m_name),
	m_subdir(copy.m_subdir),
	m_id(copy.m_id),
	m_plugin_id(copy.m_plugin_id),
	m_hashed_type(copy.m_hashed_type),
	m_owned(false)
{	
}

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

nsresource * nsresource::copy(const nsresource * res)
{
	return NULL;
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

nsresource & nsresource::operator=(const nsresource & rhs)
{
	copy(&rhs);
	return *this;
}
