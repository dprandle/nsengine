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

#include <asset/nsasset_manager.h>
#include <asset/nstexture.h>

nstexture * get_texture(const uivec2 & id);

class nstex_manager : public nsasset_manager
{
public:

	MANAGER_TEMPLATES
	
	nstex_manager();
	~nstex_manager();

	nstexture * load(uint32 res_type_id, const nsstring & fname, bool finalize_);
	
	bool del(nsasset * res);

	bool save(nsasset * res, const nsstring & path);
	
	nstex_cubemap * load_cubemap(const nsstring & fname, const nsstring & cube_all_ext);

	nstex_cubemap * load_cubemap(const nsstring & pXPlus,
								 const nsstring & pXMinus,
								 const nsstring & pYPlus,
								 const nsstring & pYMinus,
								 const nsstring & pZPlus,
								 const nsstring & pZMinus,
								 const nsstring & fname);

	nstex2d * load_image(const nsstring & fname);

	bool save_image(nstex2d * image, const nsstring & path);

	bool save_cubemap(nstex_cubemap * cubemap, const nsstring & path);

	bool vid_update_on_load;

	bool load_with_mipmaps_enabled;
};

int set_tex_format(int bpp, nstexture * tex);
#endif
