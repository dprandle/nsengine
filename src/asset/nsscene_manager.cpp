/*!
\file nsscenemanager.cpp

\brief Definition file for nsscene_manager class

This file contains all of the neccessary definitions for the nsscene_manager class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#include <nsscene_manager.h>
#include <nsscene.h>
#include <nsentity.h>
#include <nsengine.h>
#include <nsentity_manager.h>
#include <nsplatform.h>
#include <nsplugin_manager.h>

nsscene * get_scene(const uivec2 & id)
{
	nsplugin * plg = get_plugin(id.x);
	if (plg == nullptr)
		return nullptr;
	return plg->get<nsscene>(id.y);
}

nsscene_manager::nsscene_manager() :
	nsasset_manager(type_to_hash(nsscene_manager))
{
	set_local_dir(LOCAL_SCENE_DIR_DEFAULT);
}

nsscene_manager::~nsscene_manager()
{}
