#ifndef NSINPUT_MAP_MANAGER_H
#define NSINPUT_MAP_MANAGER_H

#include <nsasset_manager.h>
#include <nsinput_map.h>

class nsinput_map_manager : public nsasset_manager
{
public:

	nsinput_map_manager();
	~nsinput_map_manager();
	
	template <class res_type>
	res_type * create(const nsstring & res_name, nsasset * to_copy=nullptr)
	{
		return nsasset_manager::create<res_type>(res_name, to_copy);
	}

	virtual nsinput_map * create(const nsstring & res_name, nsasset * to_copy=nullptr)
	{
		return create<nsinput_map>(res_name, to_copy);
	}

	template <class res_type, class T>
	res_type * get(const T & res_name)
	{
		return nsasset_manager::get<res_type>(res_name);
	}
	
	template<class T>
	nsinput_map * get(const T & res_name)
	{
		return get<nsinput_map>(res_name);
	}

	template<class res_type>
	res_type * load(const nsstring & fname, bool finalize_)
	{
		return nsasset_manager::load<res_type>(fname, finalize_);
	}

	nsinput_map * load(const nsstring & fname, bool finalize_)
	{
		return load<nsinput_map>(fname, finalize_);
	}
	
	template<class res_type, class T >
	res_type * remove(const T & res_name)
	{
		return nsasset_manager::remove<res_type>(res_name);
	}

	template<class T >
	nsinput_map * remove(const T & res_name)
	{
		return remove<nsinput_map>(res_name);
	}

};

#endif
