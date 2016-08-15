/*!
\file nspluginmanager.cpp

\brief Definition file for nsplugin_manager class

This file contains all of the neccessary definitions for the nsplugin_manager class.

\author Daniel Randle
\date August 23 2014
\copywrite Earth Banana Games 2013
*/

#include <asset/nsplugin_manager.h>
#include <asset/nsplugin.h>
#include <asset/nsmesh_manager.h>
#include <asset/nstex_manager.h>
#include <asset/nsanim_manager.h>
#include <nsengine.h>
#include <asset/nsmat_manager.h>
#include <asset/nsscene_manager.h>
#include <asset/nsentity_manager.h>

nsplugin * get_plugin(uint32 id)
{
	return nsep.get<nsplugin>(id);
}

nsplugin_manager::nsplugin_manager() :
	nsasset_manager(hash_id("nsplugin_manager"))
{
	set_local_dir(LOCAL_PLUGIN_DIR_DEFAULT);
	set_save_mode(text);
}

nsplugin_manager::~nsplugin_manager()
{}
