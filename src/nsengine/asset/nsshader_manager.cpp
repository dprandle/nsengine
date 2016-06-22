/*! 
	\file nsshader_manager.cpp
	
	\brief Definition file for nsshader_manager class

	This file contains all of the neccessary definitions for the nsshader_manager class.

	\author Daniel Randle
	\date November 23 2013
	\copywrite Earth Banana Games 2013
*/

#include <nsshader_manager.h>
#include <nsplatform.h>
#include <nsengine.h>
#include <nsplugin_manager.h>

nsshader * get_shader(const uivec2 & id)
{
	nsplugin * plg = nsep.get(id.x);
	if (plg == nullptr)
		return nullptr;
	return plg->get<nsshader>(id.y);
}

nsshader_manager::nsshader_manager():
	nsasset_manager(type_to_hash(nsshader_manager)),
	vid_update_on_load(true)
{
	set_local_dir(LOCAL_SHADER_DIR_DEFAULT);
	set_save_mode(text);
}

nsshader_manager::~nsshader_manager()
{}

nsshader * nsshader_manager::load(uint32 res_type_id, const nsstring & fname, bool finalize)
{
	nsshader * shdr = (nsshader*)nsasset_manager::load(res_type_id,fname,finalize);
	if (vid_update_on_load)
		shdr->video_update();
	return shdr;
}

nsshader * nsshader_manager::load(const nsstring & fname, bool finalize_)
{
	return load<nsshader>(fname, finalize_);
}
