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

#include <nsres_manager.h>
#include <nsanim_set.h>

struct aiScene;

class nsanim_manager : public nsres_manager
{
public:
	nsanim_manager();
	~nsanim_manager();

	template <class res_type>
	res_type * create(const nsstring & res_name, nsresource * to_copy=nullptr)
	{
		return nsres_manager::create<res_type>(res_name, to_copy);
	}

	virtual nsanim_set * create(const nsstring & res_name, nsresource * to_copy=nullptr)
	{
		return create<nsanim_set>(res_name, to_copy); // Create 2d texture by default
	}

	template <class res_type, class T>
	res_type * get(const T & res_name)
	{
		return nsres_manager::get<res_type>(res_name);
	}
	
	template<class T>
	nsanim_set * get(const T & res_name)
	{
		return get<nsanim_set>(res_name);
	}

	template<class res_type>
	res_type * load(const nsstring & fname)
	{
		return nsres_manager::load<res_type>(fname);
	}

	nsanim_set * load(const nsstring & fname)
	{
		return load<nsanim_set>(fname);
	}
	
	template<class res_type, class T >
	res_type * remove(const T & res_name)
	{
		return nsres_manager::remove<res_type>(res_name);
	}

	template<class T >
	nsanim_set * remove(const T & res_name)
	{
		return remove<nsanim_set>(res_name);
	}

	nsanim_set * assimp_load_anim_set(const aiScene * assimp_scene, const nsstring & scene_name);
};

#endif
