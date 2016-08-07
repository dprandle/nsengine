/*! 
	\file nsanim_manager.h
	
	\brief Header file for nsanim_manager class

	This file contains all of the neccessary declarations for the nsanim_manager class.

	\author Daniel Randle
	\date December 11 2013
	\copywrite Earth Banana Games 2013
*/

#ifndef NSANIM_MANAGER_H
#define NSANIM_MANAGER_H

#include "nsasset_manager.h"
#include "nsanim_set.h"

struct aiScene;

nsanim_set * get_anim_set(const uivec2 & id);

class nsanim_manager : public nsasset_manager
{

  public:

	MANAGER_TEMPLATES
	
	nsanim_manager();
	~nsanim_manager();

	nsanim_set * assimp_load_anim_set(
		const aiScene * assimp_scene,
		const nsstring & scene_name);
};

#endif
