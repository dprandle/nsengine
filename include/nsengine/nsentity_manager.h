/*! 
	\file nsentity_manager.h
	
	\brief Header file for nsentity_manager class

	This file contains all of the neccessary declarations for the NSEnTempManager class.

	\author Daniel Randle
	\date November 23 2013
	\copywrite Earth Banana Games 2013
*/

#ifndef NSENTITY_MANAGER_H
#define NSENTITY_MANAGER_H

#include <nsres_manager.h>
#include <nsentity.h>

class nsentity_manager : public nsres_manager
{
public:

	nsentity_manager();
	~nsentity_manager();

	template <class res_type>
	res_type * create(const nsstring & res_name)
	{
		return nsres_manager::create<res_type>(res_name);
	}

	virtual nsentity * create(const nsstring & res_name)
	{
		return create<nsentity>(res_name); // Create 2d texture by default
	}

	template <class res_type, class T>
	res_type * get(const T & res_name)
	{
		return nsres_manager::get<res_type>(res_name);
	}
	
	template<class T>
	nsentity * get(const T & res_name)
	{
		return get<nsentity>(res_name);
	}

	template<class res_type>
	res_type * load(const nsstring & fname)
	{
		return nsres_manager::load<res_type>(fname);
	}

	nsentity * load(const nsstring & fname)
	{
		return load<nsentity>(fname);
	}
	
	template<class res_type, class T >
	res_type * remove(const T & res_name)
	{
		return nsres_manager::remove<res_type>(res_name);
	}

	template<class T >
	nsentity * remove(const T & res_name)
	{
		return remove<nsentity>(res_name);
	}

	template<class comp_type>
	nspentityset entities()
	{
		uint32 hashed_type = nsengine.type_id(std::type_index(typeid(comp_type)));
		return entities(hashed_type);
	}

	nspentityset entities(uint32 comp_type_id);

	nspentityset entities(const nsstring & comp_guid);
};

#endif



