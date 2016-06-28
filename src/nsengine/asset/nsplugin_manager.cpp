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

nsplugin * get_plugin(uint32 id)
{
	return nsep.get(id);
}

nsplugin_manager::nsplugin_manager() :
	nsasset_manager(hash_id("nsplugin_manager"))
{
	set_local_dir(LOCAL_PLUGIN_DIR_DEFAULT);
	set_save_mode(text);
}

nsplugin_manager::~nsplugin_manager()
{}
