/*!
\file nsscenemanager.cpp

\brief Definition file for nsscene_manager class

This file contains all of the neccessary definitions for the nsscene_manager class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#include <asset/nsmap_area_manager.h>
#include <asset/nsmap_area.h>
#include <asset/nsentity.h>
#include <nsengine.h>
#include <asset/nsentity_manager.h>
#include <nsplatform.h>
#include <asset/nsplugin_manager.h>

nsmap_area * get_scene(const uivec2 & id)
{
	nsplugin * plg = get_plugin(id.x);
	if (plg == nullptr)
		return nullptr;
	return plg->get<nsmap_area>(id.y);
}

nsscene_manager::nsscene_manager() :
	nsasset_manager(type_to_hash(nsscene_manager))
{
	set_local_dir(LOCAL_SCENE_DIR_DEFAULT);
}

nsscene_manager::~nsscene_manager()
{}
