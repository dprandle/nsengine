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

class nsshader_manager : public nsres_manager
{
public:

	nsshader_manager();
	~nsshader_manager();

	template <class res_type>
	res_type * create(const nsstring & res_name)
	{
		return nsres_manager::create<res_type>(res_name);
	}

	virtual nsshader * create(const nsstring & res_name)
	{
		return create<nsshader>(res_name); // Create 2d texture by default
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
	res_type * load(const nsstring & fname)
	{
		return nsres_manager::load<res_type>(fname);
	}

	nsshader * load(const nsstring & fname)
	{
		return load<nsshader>(fname);
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

	bool compile_all();

	template<class T>
	bool compile(const T & shader)
	{
		nsshader * sh = get(shader);
		return compile(sh);
	}

	bool compile(nsshader * sh);

	void init_uniforms_all();

	template<class T>
	void init_uniforms(const T & shader)
	{
		nsshader * sh = get(shader);
		init_uniforms(sh);
	}

	void init_uniforms(nsshader * sh);

	bool link_all();

	template<class T>
	bool link(const T & shader)
	{
		nsshader * sh = get(shader);
		return link(sh);
	}

	bool link(nsshader * sh);
	
	template<class T>
	bool load_stage(const T & shader, const nsstring & filename, nsshader::shader_type stagetype)
	{
		nsshader * sh = get(shader);
        return load_stage(shader, filename, stagetype);
	}

	bool load_stage(nsshader * shader, const nsstring & fname, nsshader::shader_type stagetype);

	template<class T>
	bool save_stage(const T & shader, const nsstring & filename, nsshader::shader_type stagetype)
	{
		nsshader * sh = get(shader);
        return save_stage(sh, filename, stagetype);
	}

	bool save_stage(nsshader * sh, const nsstring & filename, nsshader::shader_type stagetype);
};

#endif
