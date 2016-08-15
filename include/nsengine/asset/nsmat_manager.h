/*! 
	\file nsmat_manager.h
	
	\brief Header file for nsmat_manager class

	This file contains all of the neccessary declarations for the nsmat_manager class.

	\author Daniel Randle
	\date November 23 2013
	\copywrite Earth Banana Games 2013
*/

#ifndef NSMAT_MANAGER_H
#define NSMAT_MANAGER_H

#include <asset/nsasset_manager.h>
#include <asset/nsmaterial.h>

nsmaterial * get_material(const uivec2 & id);

struct aiMaterial;

class nsmat_manager : public nsasset_manager
{
public:

	MANAGER_TEMPLATES
	
	nsmat_manager();
	~nsmat_manager();

	nsmaterial * assimp_load_material(
		const nsstring & mat_name_,
		const aiMaterial * assimp_mat_,
		const nsstring & tex_dir_="");

};

#endif
