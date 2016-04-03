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

#include <nsres_manager.h>
#include <nsplugin.h>

nsplugin * get_plugin(uint32 id);

class nsplugin_manager : public nsres_manager
{
public:
	using nsres_manager::save;
	nsplugin_manager();
	~nsplugin_manager();

	virtual bool add(nsresource * res);
	
	template <class res_type>
	res_type * create(const nsstring & res_name, nsresource * to_copy=nullptr)
	{
		return nsres_manager::create<res_type>(res_name, to_copy);
	}

	virtual nsplugin * create(const nsstring & res_name, nsresource * to_copy=nullptr)
	{
		return create<nsplugin>(res_name, to_copy); // Create 2d texture by default
	}

	template <class res_type, class T>
	res_type * get(const T & res_name)
	{
		return nsres_manager::get<res_type>(res_name);
	}
	
	template<class T>
	nsplugin * get(const T & res_name)
	{
		return get<nsplugin>(res_name);
	}

	template<class res_type>
	res_type * load(const nsstring & fname, bool finalize_)
	{
		return nsres_manager::load<res_type>(fname, finalize_);
	}

	nsplugin * load(const nsstring & fname, bool finalize_)
	{
		return load<nsplugin>(fname, finalize_);
	}
	
	template<class res_type, class T >
	res_type * remove(const T & res_name)
	{
		return nsres_manager::remove<res_type>(res_name);
	}

	template<class T >
	nsplugin * remove(const T & res_name)
	{
		return remove<nsplugin>(res_name);
	}

	nsplugin * active();
	
	template<class T>
	bool bind(const T & name)
	{
		nsplugin * plug = get(name);
		return bind(plug);
	}

	bool bind(nsplugin * plg);
		
	void set_plugin_dir(const nsstring & dir);

	const nsstring & plugin_dir();

	void set_res_dir(const nsstring & pDirectory);

	const nsstring & res_dir();

	template<class T>
	bool unbind(const T & name)
	{
		nsplugin * plg = get(name);
		return unbind(plg);
	}

	bool unbind(nsplugin * plg);

	template<class T>
	void set_active(const T & name)
	{
		nsplugin * plg = get(name);
		set_active(plg);
	}

	void set_active(nsplugin * plg);
	
private:
	uint32 m_active_plugin_id;
	nsstring m_owned_plugins_res_dir;
};

#define nsep (*nsengine::inst().plugins())

#endif
