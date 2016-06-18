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

#include <nsasset_manager.h>
#include <nsanim_set.h>

struct aiScene;

nsanim_set * get_anim_set(const uivec2 & id);

class nsanim_manager : public nsasset_manager
{
public:
	nsanim_manager();
	~nsanim_manager();

	template <class res_type>
	res_type * create(const nsstring & res_name, nsasset * to_copy=nullptr)
	{
		return nsasset_manager::create<res_type>(res_name, to_copy);
	}

	virtual nsanim_set * create(const nsstring & res_name, nsasset * to_copy=nullptr)
	{
		return create<nsanim_set>(res_name, to_copy); // Create 2d texture by default
	}

	template <class res_type, class T>
	res_type * get(const T & res_name)
	{
		return nsasset_manager::get<res_type>(res_name);
	}
	
	template<class T>
	nsanim_set * get(const T & res_name)
	{
		return get<nsanim_set>(res_name);
	}

	template<class res_type>
	res_type * load(const nsstring & fname, bool finalize_)
	{
		return nsasset_manager::load<res_type>(fname, finalize_);
	}

	nsanim_set * load(const nsstring & fname, bool finalize_)
	{
		return load<nsanim_set>(fname, finalize_);
	}
	
	template<class res_type, class T >
	res_type * remove(const T & res_name)
	{
		return nsasset_manager::remove<res_type>(res_name);
	}

	template<class T >
	nsanim_set * remove(const T & res_name)
	{
		return remove<nsanim_set>(res_name);
	}

	nsanim_set * assimp_load_anim_set(const aiScene * assimp_scene, const nsstring & scene_name);
};

#endif
