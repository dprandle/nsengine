/*! 
	\file nstex_manager.h
	
	\brief Header file for nstex_manager class

	This file contains all of the neccessary declarations for the nstex_manager class.

	\author Daniel Randle
	\date November 23 2013
	\copywrite Earth Banana Games 2013
*/


//Internal Format : Bytes per pixel(ie 3 or 4)
//Format : enum specifying order of those byes - ie RGBA or RBGA etc
//Pixel Data Type : the data type assigned to each component of the pixe(ie unsigned byte)


#ifndef NSTEX_MANAGER_H
#define NSTEX_MANAGER_H

#include "nsasset_manager.h"
#include "nstexture.h"

nstexture * get_texture(const uivec2 & id);

class nstex_manager : public nsasset_manager
{
public:

	nstex_manager();
	~nstex_manager();

	template <class res_type>
	res_type * create(const nsstring & resName, nsasset * to_copy=nullptr)
	{
		return nsasset_manager::create<res_type>(resName, to_copy);
	}

	virtual nstex2d * create(const nsstring & resName, nsasset * to_copy=nullptr)
	{
		return create<nstex2d>(resName, to_copy); // Create 2d texture by default
	}

	template <class res_type, class T>
	res_type * get(const T & rname)
	{
		return nsasset_manager::get<res_type>(rname);
	}
	
	template<class T>
	nstexture * get(const T & resname)
	{
		return get<nstexture>(resname);
	}

	template<class res_type>
	res_type * load(const nsstring & fname, bool finalize_)
	{
		return nsasset_manager::load<res_type>(fname, finalize_);
	}

	nstexture * load(const nsstring & fname, bool finalize_);

	nstexture * load(uint32 res_type_id, const nsstring & fname, bool finalize_);
	
	template<class res_type, class T >
	res_type * remove(const T & rname)
	{
		return nsasset_manager::remove<res_type>(rname);
	}

	template<class T >
	nstexture * remove(const T & rname)
	{
		return remove<nstexture>(rname);
	}

	template<class T>
	bool save(const T & res_name, nsstring path="")
	{
		nsasset * res = get(res_name);
		return save(res, path);
	}

	virtual bool del(nsasset * res);

	virtual bool save(nsasset * res, const nsstring & path);
	
	virtual nstex_cubemap * load_cubemap(const nsstring & fname, const nsstring & cube_all_ext);

	virtual nstex_cubemap * load_cubemap(const nsstring & pXPlus,
									   const nsstring & pXMinus,
									   const nsstring & pYPlus,
									   const nsstring & pYMinus,
									   const nsstring & pZPlus,
									   const nsstring & pZMinus,
									   const nsstring & fname);

	virtual nstex2d * load_image(const nsstring & fname);

	virtual bool save(nstex2d * image, const nsstring & path);

	virtual bool save(nstex_cubemap * cubemap, const nsstring & path);

	bool vid_update_on_load;

	bool load_with_mipmaps_enabled;
};

int set_tex_format(int bpp, nstexture * tex);
#endif
