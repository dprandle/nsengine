/*!
\file nsplugin_manager.h

\brief Header file for nsplugin_manager class

This file contains all of the neccessary declarations for the nsplugin_manager class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#ifndef NSPLUGIN_MANAGER_H
#define NSPLUGIN_MANAGER_H

#include <nsasset_manager.h>
#include <nsplugin.h>

nsplugin * get_plugin(uint32 id);

class nsplugin_manager : public nsasset_manager
{
public:

	using nsasset_manager::save;

	nsplugin_manager();
	~nsplugin_manager();
	
	template <class res_type>
	res_type * create(const nsstring & res_name, nsasset * to_copy=nullptr)
	{
		return nsasset_manager::create<res_type>(res_name, to_copy);
	}

	virtual nsplugin * create(const nsstring & res_name, nsasset * to_copy=nullptr)
	{
		return create<nsplugin>(res_name, to_copy); // Create 2d texture by default
	}

	template <class res_type, class T>
	res_type * get(const T & res_name)
	{
		return nsasset_manager::get<res_type>(res_name);
	}
	
	template<class T>
	nsplugin * get(const T & res_name)
	{
		return get<nsplugin>(res_name);
	}

	template<class res_type>
	res_type * load(const nsstring & fname, bool finalize_)
	{
		return nsasset_manager::load<res_type>(fname, finalize_);
	}

	nsplugin * load(const nsstring & fname, bool finalize_)
	{
		return load<nsplugin>(fname, finalize_);
	}
	
	template<class res_type, class T >
	res_type * remove(const T & res_name)
	{
		return nsasset_manager::remove<res_type>(res_name);
	}

	template<class T >
	nsplugin * remove(const T & res_name)
	{
		return remove<nsplugin>(res_name);
	}
};

#define nsep (*nsengine::inst().plugins())

#endif
