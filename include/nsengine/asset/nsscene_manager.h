/*!
\file nsscene_manager.h

\brief Header file for nsscene_manager class

This file contains all of the neccessary declarations for the nsscene_manager class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#ifndef NSSCENE_MANAGER_H
#define NSSCENE_MANAGER_H

#include "nsasset_manager.h"
#include "nsscene.h"

nsscene * get_scene(const uivec2 & id);

class nsscene_manager : public nsasset_manager
{
public:
	nsscene_manager();
	~nsscene_manager();
	
	template <class res_type>
	res_type * create(const nsstring & res_name, nsasset * to_copy=nullptr)
	{
		return nsasset_manager::create<res_type>(res_name, to_copy);
	}

	virtual nsscene * create(const nsstring & res_name, nsasset * to_copy=nullptr)
	{
		return create<nsscene>(res_name, to_copy);
	}

	template <class res_type, class T>
	res_type * get(const T & res_name)
	{
		return nsasset_manager::get<res_type>(res_name);
	}
	
	template<class T>
	nsscene * get(const T & res_name)
	{
		return get<nsscene>(res_name);
	}

	template<class res_type>
	res_type * load(const nsstring & fname, bool finalize_)
	{
		return nsasset_manager::load<res_type>(fname, finalize_);
	}

	nsscene * load(const nsstring & fname, bool finalize_)
	{
		return load<nsscene>(fname, finalize_);
	}
	
	template<class res_type, class T >
	res_type * remove(const T & res_name)
	{
		return nsasset_manager::remove<res_type>(res_name);
	}

	template<class T >
	nsscene * remove(const T & res_name)
	{
		return remove<nsscene>(res_name);
	}

	template<class T>
	bool save(const T & res_name, nsstring path="")
	{
		nsasset * res = get(res_name);
		return save(res, path);
	}
};

#endif
