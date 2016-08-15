/*!
\file system.cpp

\brief Definition file for nssystem class

This file contains all of the neccessary definitions for the nssystem class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#include <system/nssystem.h>
#include <asset/nsscene.h>

nssystem::nssystem(uint32 hashed_type):
	m_scene_null_error(false),
	m_scene_disabled_error(false),
	m_active_scene(nullptr),
	m_hashed_type(hashed_type)
{}

nssystem::~nssystem()
{}

nsscene * nssystem::active_scene()
{
	return m_active_scene;
}

bool nssystem::scene_error_check()
{
	if (m_active_scene == nullptr)
	{
		if (!m_scene_null_error)
		{
			dprint(hash_to_guid(m_hashed_type) + "::scene_valid_check - Active scene is null - must assign active scene");
			m_scene_null_error = true;
		}
	}
	else if (!m_active_scene->is_enabled())
	{
		if (!m_scene_disabled_error)
		{
			dprint(hash_to_guid(m_hashed_type) + "::scene_valid_check - Active scene is disabled - must enable active scene");
			m_scene_disabled_error = true;
		}		
	}
	else
	{
		m_scene_null_error = false;
		m_scene_disabled_error = false;
	}
	return m_scene_null_error || m_scene_disabled_error;
}

void nssystem::set_active_scene(nsscene * active_scene)
{
	m_active_scene = active_scene;
}

uint32 nssystem::type()
{
	return m_hashed_type;
}
