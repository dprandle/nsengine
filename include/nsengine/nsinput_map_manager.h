#ifndef NSINPUT_MAP_MANAGER_H
#define NSINPUT_MAP_MANAGER_H

#include <nsres_manager.h>
#include <nsinput_map.h>

class nsinput_map_manager : public nsres_manager
{
public:

	nsinput_map_manager();
	~nsinput_map_manager();
	
	template <class res_type>
	res_type * create(const nsstring & res_name)
	{
		return nsres_manager::create<res_type>(res_name);
	}

	virtual nsinput_map * create(const nsstring & res_name)
	{
		return create<nsinput_map>(res_name); // Create 2d texture by default
	}

	template <class res_type, class T>
	res_type * get(const T & res_name)
	{
		return nsres_manager::get<res_type>(res_name);
	}
	
	template<class T>
	nsinput_map * get(const T & res_name)
	{
		return get<nsinput_map>(res_name);
	}

	template<class res_type>
	res_type * load(const nsstring & fname)
	{
		return nsres_manager::load<res_type>(fname);
	}

	nsinput_map * load(const nsstring & fname)
	{
		return load<nsinput_map>(fname);
	}
	
	template<class res_type, class T >
	res_type * remove(const T & res_name)
	{
		return nsres_manager::remove<res_type>(res_name);
	}

	template<class T >
	nsinput_map * remove(const T & res_name)
	{
		return remove<nsinput_map>(res_name);
	}

};

#endif
