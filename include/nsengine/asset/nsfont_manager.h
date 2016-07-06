/*! 
	\file nsfont_manager.h
	
	\brief Header file for nsfont_manager class

	This file contains all of the neccessary declarations for the nsfont_manager class.

	\author Daniel Randle
	\date Feb 20 2016
	\copywrite Earth Banana Games 2013
*/

#ifndef NSFONT_MANAGER_H
#define NSFONT_MANAGER_H

#include <nsasset_manager.h>
#include <nsfont.h>

nsfont * get_font(const uivec2 & id);

class nsfont_manager : public nsasset_manager
{
public:
	nsfont_manager();
	~nsfont_manager();

	template <class res_type>
	res_type * create(const nsstring & res_name, nsasset * to_copy=nullptr)
	{
		return nsasset_manager::create<res_type>(res_name, to_copy);
	}

	virtual nsfont * create(const nsstring & res_name, nsasset * to_copy=nullptr)
	{
		return create<nsfont>(res_name, to_copy); // Create regular font by default
	}

	template <class res_type, class T>
	res_type * get(const T & res_name)
	{
		return nsasset_manager::get<res_type>(res_name);
	}
	
	template<class T>
	nsfont * get(const T & res_name)
	{
		return get<nsfont>(res_name);
	}

	template<class res_type>
	res_type * load(const nsstring & fname, bool finalize_)
	{
		return nsasset_manager::load<res_type>(fname, finalize_);
	}

	nsfont * load(const nsstring & fname, bool finalize_)
	{
		return load<nsfont>(fname, finalize_);
	}

	virtual nsfont * load(uint32 res_type_id, const nsstring & fname, bool finalize_);
	
	template<class res_type, class T >
	res_type * remove(const T & res_name)
	{
		return nsasset_manager::remove<res_type>(res_name);
	}

	template<class T >
	nsfont * remove(const T & res_name)
	{
		return remove<nsfont>(res_name);
	}

	nsfont * load_external(const nsstring & fname);
};

void parse_char_line(const nsstring & l, nsfont * fnt);
void parse_common_line(const nsstring & l, font_info & fi);
void parse_info_line(const nsstring & l, font_info & fi);
uint8 tex_name_from_line(const nsstring & l, nsstring & name);
uint8 parse_count_from_line(const nsstring & l);

#endif