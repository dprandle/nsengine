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

struct aiMaterial;

class nsmat_manager : public nsres_manager
{
public:
	nsmat_manager();
	~nsmat_manager();

	template <class res_type>
	res_type * create(const nsstring & res_name_)
	{
		return nsres_manager::create<res_type>(res_name_);
	}

	virtual nsmaterial * create(const nsstring & res_name_)
	{
		return create<nsmaterial>(res_name_);
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
	res_type * load(const nsstring & fname_)
	{
		return nsres_manager::load<res_type>(fname_);
	}

	nsmaterial * load(const nsstring & fname_)
	{
		return load<nsmaterial>(fname_);
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
