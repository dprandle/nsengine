/*!
\file nspluginmanager.cpp

\brief Definition file for nsplugin_manager class

This file contains all of the neccessary definitions for the nsplugin_manager class.

\author Daniel Randle
\date August 23 2014
\copywrite Earth Banana Games 2013
*/

#include <nsplugin_manager.h>
#include <nsplugin.h>
#include <nsmesh_manager.h>
#include <nstex_manager.h>
#include <nsanim_manager.h>
#include <nsengine.h>
#include <nsmat_manager.h>
#include <nsscene_manager.h>
#include <nsentity_manager.h>

nsplugin_manager::nsplugin_manager() :
nsres_manager()
{
	set_save_mode(text);
}

nsplugin_manager::~nsplugin_manager()
{}

bool nsplugin_manager::add(nsresource * res)
{
	if (nsres_manager::add(res))
	{
		nsplugin * plug = static_cast<nsplugin*>(res);
		plug->set_res_dir(m_owned_plugins_res_dir);
		plug->set_import_dir(nsengine.importdir());
		return true;
	}
	return false;
}

bool nsplugin_manager::bind(nsplugin * plg)
{
	return plg->bind();
}

void nsplugin_manager::set_res_dir(const nsstring & pDirectory)
{
	m_owned_plugins_res_dir = pDirectory;
	auto iter = m_id_resmap.begin();
	while (iter != m_id_resmap.end())
	{
		nsplugin * plug = get(iter->first);
		plug->set_res_dir(m_owned_plugins_res_dir);
		++iter;
	}
}

void nsplugin_manager::set_plugin_dir(const nsstring & dir)
{
	m_res_dir = dir;
}

const nsstring & nsplugin_manager::plugin_dir()
{
	return m_res_dir;
}

const nsstring & nsplugin_manager::res_dir()
{
	return m_owned_plugins_res_dir;
}

nsplugin * nsplugin_manager::active()
{
	return get(m_active_plugin_id);
}

void nsplugin_manager::set_active(nsplugin * plg)
{
	if (plg != NULL)
	{
		if (!plg->bound())
			plg->bind();
		m_active_plugin_id = plg->id();
	}
	else
		m_active_plugin_id = 0;
}

bool nsplugin_manager::unbind(nsplugin * plg)
{
	return plg->unbind();
}
