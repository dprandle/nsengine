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

nsentity * get_entity(const uivec2 & id);

typedef std::set<nsentity*> entity_ptr_set;
class nsentity_manager : public nsres_manager
{
public:
	
	nsentity_manager();
	~nsentity_manager();

	template <class res_type>
	res_type * create(const nsstring & res_name, nsresource * to_copy=nullptr)
	{
		return nsres_manager::create<res_type>(res_name, to_copy);
	}

	virtual nsentity * create(const nsstring & res_name, nsresource * to_copy=nullptr)
	{
		return create<nsentity>(res_name, to_copy);
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
	res_type * load(const nsstring & fname, bool finalize_)
	{
		return nsres_manager::load<res_type>(fname, finalize_);
	}

	nsentity * load(const nsstring & fname, bool finalize_)
	{
		return load<nsentity>(fname, finalize_);
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
	entity_ptr_set entities()
	{
		uint32 hashed_type = nse.type_id(std::type_index(typeid(comp_type)));
		return entities(hashed_type);
	}

	entity_ptr_set entities(uint32 comp_type_id);

	entity_ptr_set entities(const nsstring & comp_guid);
};

#endif



