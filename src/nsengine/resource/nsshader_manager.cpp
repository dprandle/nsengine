/*! 
	\file nsshader_manager.cpp
	
	\brief Definition file for nsshader_manager class

	This file contains all of the neccessary definitions for the nsshader_manager class.

	\author Daniel Randle
	\date November 23 2013
	\copywrite Earth Banana Games 2013
*/

#include <nsshader_manager.h>
#include <nsfile_os.h>
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
	nsres_manager()
{
	set_local_dir(LOCAL_SHADER_DIR_DEFAULT);
	set_save_mode(text);
}

nsshader_manager::~nsshader_manager()
{}
