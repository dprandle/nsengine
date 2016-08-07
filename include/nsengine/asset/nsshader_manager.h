/*! 
	\file nsshader__manager.h
	
	\brief Header file for nsshader_manager
	!class

	This file contains all of the neccessary declarations for the nsshader_manager class.

	\author Daniel Randle
	\date November 23 2013
	\copywrite Earth Banana Games 2013
*/

#ifndef NSSHADER_MANAGER_H
#define NSSHADER_MANAGER_H

#include "nsasset_manager.h"
#include "nsshader.h"

nsshader * get_shader(const uivec2 & id);

class nsshader_manager : public nsasset_manager
{
	
  public:

	MANAGER_TEMPLATES
	
	nsshader_manager();
	~nsshader_manager();

	nsshader * load(uint32 res_type_id, const nsstring & fname, bool finalize);
	
	bool vid_update_on_load;
};

#endif
