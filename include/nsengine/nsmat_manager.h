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
	res_type * create(const nsstring & resName)
	{
		return nsres_manager::create<res_type>(resName);
	}

	virtual nsmaterial * create(const nsstring & resName)
	{
		return create<nsmaterial>(resName);
	}

	template <class res_type, class T>
	res_type * get(const T & rname)
	{
		return nsres_manager::get<res_type>(rname);
	}
	
	template<class T>
	nsmaterial * get(const T & resname)
	{
		return get<nsmaterial>(resname);
	}

	template<class res_type>
	res_type * load(const nsstring & fname)
	{
		return nsres_manager::load<res_type>(fname);
	}

	nsmaterial * load(const nsstring & fname)
	{
		return load<nsmaterial>(fname);
	}
	
	template<class res_type, class T >
	res_type * remove(const T & rname)
	{
		return nsres_manager::remove<res_type>(rname);
	}

	template<class T >
	nsmaterial * remove(const T & rname)
	{
		return remove<nsmaterial>(rname);
	}

	nsmaterial * assimp_load_material(const nsstring & pMaterialName, const aiMaterial * pAIMat, const nsstring & pTexDir="");

};

#endif
