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

#include <nsres_manager.h>
#include <nsscene.h>

class nsscene_manager : public nsres_manager
{
public:
	nsscene_manager();
	~nsscene_manager();
	
	template <class res_type>
	res_type * create(const nsstring & res_name)
	{
		return nsres_manager::create<res_type>(res_name);
	}

	virtual nsscene * create(const nsstring & res_name)
	{
		return create<nsscene>(res_name); // Create 2d texture by default
	}

	template <class res_type, class T>
	res_type * get(const T & res_name)
	{
		return nsres_manager::get<res_type>(res_name);
	}
	
	template<class T>
	nsscene * get(const T & res_name)
	{
		return get<nsscene>(res_name);
	}

	template<class res_type>
	res_type * load(const nsstring & fname)
	{
		return nsres_manager::load<res_type>(fname);
	}

	nsscene * load(const nsstring & fname)
	{
		return load<nsscene>(fname);
	}

	virtual nsscene * load(uint32 res_type_id, const nsstring & fname);
	
	template<class res_type, class T >
	res_type * remove(const T & res_name)
	{
		return nsres_manager::remove<res_type>(res_name);
	}

	template<class T >
	nsscene * remove(const T & res_name)
	{
		return remove<nsscene>(res_name);
	}

	template<class T>
	bool save(const T & res_name, nsstring path="")
	{
		nsresource * res = get(res_name);
		return save(res, path);
	}

	virtual bool save(nsresource * res, const nsstring & path="");
	
	nsscene * current();
	
	template<class T>
	bool set_current(const T & scene, bool new_scene = false, bool save_previous = false)
	{
		nsscene * sc = get(scene);
		return set_current(sc, new_scene, save_previous);
	}
	
	bool set_current(nsscene * scene, bool new_scene=false, bool save_previous=false);

private:
	nsscene * mCurrentScene;
};

#endif
