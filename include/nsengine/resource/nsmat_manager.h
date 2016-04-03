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

#include <nsres_manager.h>
#include <nsmaterial.h>

nsmaterial * get_material(const uivec2 & id);

struct aiMaterial;

class nsmat_manager : public nsres_manager
{
public:
	nsmat_manager();
	~nsmat_manager();

	template <class res_type>
	res_type * create(const nsstring & res_name_, nsresource * to_copy=nullptr)
	{
		return nsres_manager::create<res_type>(res_name_, to_copy);
	}

	virtual nsmaterial * create(const nsstring & res_name_, nsresource * to_copy=nullptr)
	{
		return create<nsmaterial>(res_name_, to_copy);
	}

	template <class res_type, class T>
	res_type * get(const T & res_name_)
	{
		return nsres_manager::get<res_type>(res_name_);
	}
	
	template<class T>
	nsmaterial * get(const T & res_name)
	{
		return get<nsmaterial>(res_name);
	}

	template<class res_type>
	res_type * load(const nsstring & fname_, bool finalize_)
	{
		return nsres_manager::load<res_type>(fname_, finalize_);
	}

	nsmaterial * load(const nsstring & fname_, bool finalize_)
	{
		return load<nsmaterial>(fname_, finalize_);
	}
	
	template<class res_type, class T >
	res_type * remove(const T & res_name_)
	{
		return nsres_manager::remove<res_type>(res_name_);
	}

	template<class T >
	nsmaterial * remove(const T & res_name_)
	{
		return remove<nsmaterial>(res_name_);
	}

	nsmaterial * assimp_load_material(const nsstring & mat_name_, const aiMaterial * assimp_mat_, const nsstring & tex_dir_="");

};

#endif
