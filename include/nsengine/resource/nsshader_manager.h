/*! 
	\file nsshader__manager.h
	
	\brief Header file for nsshader_manager
	!class

	This file contains all of the neccessary declarations for the nsshader_manager class.

	\author Daniel Randle
	\date November 23 2013
	\copywrite Earth Banana Games 2013
*/

#ifndef NSSHADER_MANAGER_H
#define NSSHADER_MANAGER_H

#include <nsres_manager.h>
#include <nsshader.h>

nsshader * get_shader(const uivec2 & id);

class nsshader_manager : public nsres_manager
{
	
  public:

	nsshader_manager();
	~nsshader_manager();

	template <class res_type>
	res_type * create(const nsstring & res_name, nsresource * to_copy)
	{
		return nsres_manager::create<res_type>(res_name, to_copy);
	}

	virtual nsshader * create(const nsstring & res_name, nsresource * to_copy)
	{
		return create<nsshader>(res_name, to_copy);
	}

	template <class res_type, class T>
	res_type * get(const T & res_name)
	{
		return nsres_manager::get<res_type>(res_name);
	}
	
	template<class T>
	nsshader * get(const T & res_name)
	{
		return get<nsshader>(res_name);
	}

	template<class res_type>
	res_type * load(const nsstring & fname, bool finalize_)
	{
		return nsres_manager::load<res_type>(fname, finalize_);
	}

	nsshader * load(const nsstring & fname, bool finalize_)
	{
		return load<nsshader>(fname, finalize_);
	}
	
	template<class res_type, class T >
	res_type * remove(const T & res_name)
	{
		return nsres_manager::remove<res_type>(res_name);
	}

	template<class T >
	nsshader * remove(const T & res_name)
	{
		return remove<nsshader>(res_name);
	}
};

#endif
