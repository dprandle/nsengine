/*! 
	\file nstexmanager.cpp
	
	\brief Header file for nstex_manager class

	This file contains all of the neccessary definitions for the nstex_manager class.

	\author Daniel Randle
	\date November 23 2013
	\copywrite Earth Banana Games 2013
*/
#include <iostream>

#include <asset/stb_image_write.h>
#include <asset/stb_image.h>

#include <asset/nstex_manager.h>
#include <nsplatform.h>
#include <asset/nstexture.h>
#include <nsengine.h>
#include <asset/nsplugin_manager.h>

nstexture * get_texture(const uivec2 & id)
{
	nsplugin * plg = get_plugin(id.x);
	if (plg == nullptr)
		return nullptr;
	return plg->get<nstexture>(id.y);
}

nstex_manager::nstex_manager():
	nsasset_manager(type_to_hash(nstex_manager)),
	vid_update_on_load(true),
	load_with_mipmaps_enabled(true)
{
	set_local_dir(LOCAL_TEXTURE_DIR_DEFAULT);
}

nstex_manager::~nstex_manager()
{}

nstexture * nstex_manager::load(uint32 res_type_id, const nsstring & fname, bool finalize_)
{
	if (fname.empty())
		return NULL;
	nstexture * ret = nullptr;
	if (res_type_id == type_to_hash(nstex2d))
	{
		ret = load_image(fname);
		if (ret != nullptr && finalize_)
			ret->finalize();
	}
	else if (res_type_id == type_to_hash(nstex_cubemap))
	{
		nsstring texExtension = "";
		nsstring texName = name_from_filename(fname);
		size_t extPos = fname.find_last_of(".");

		if (extPos != nsstring::npos)
			texExtension = fname.substr(extPos);
		else
			return NULL;

		if (texExtension == ".cube" || texExtension == ".CUBE")
			ret = load_cubemap(fname,".png");
		if (ret != nullptr && finalize_)
			ret->finalize();
	}
	else
	{
		ret = static_cast<nstexture*>(nsasset_manager::load(res_type_id, fname, finalize_));
	}	
	return ret;
}

nstex2d * nstex_manager::load_image(const nsstring & fname)
{
	if (fname.empty())
		return NULL;
	
	nsstring resName(fname);
	nsstring res_ext;
	nsstring fName;
	nsstring sub_dir;
	bool shouldPrefix = false;
	
	nsstring prefixdirs = m_res_dir + m_local_dir;

	size_t pos = resName.find_last_of("/\\");
	if (pos != nsstring::npos)
	{
		if (resName[0] != '/' && resName[0] != '.' && resName.find(":") == nsstring::npos) // then subdir
		{
			sub_dir = resName.substr(0, pos + 1);
			shouldPrefix = true;
		}
		resName = resName.substr(pos + 1);
	}
	else
		shouldPrefix = true;

	size_t extPos = resName.find_last_of(".");
	res_ext = resName.substr(extPos);
	resName = resName.substr(0, extPos);

	if (shouldPrefix)
		fName = prefixdirs + sub_dir + resName + res_ext;
	else
		fName = fname;

	nstex2d * tex = get<nstex2d>(resName);
	if (tex == NULL)
		tex = create<nstex2d>(resName); // possible else clear - will overwrite existing tex though
	else
		return NULL;


	ivec2 sz; int32 bpp;
	uint8 * data = stbi_load(fName.c_str(), &sz.x, &sz.y, &bpp, 0);
	if (data == nullptr)
	{
		dprint("nstex_manager::load_image Error loading image data from " + fName);
		destroy(resName);
		return nullptr;
	}

	if (!set_tex_format(bpp,tex))
	{
		dprint("nstex_manager::load_image Error setting format for texture " + fName);
		destroy(resName);
		return nullptr;		
	}

	tex->set_subdir(sub_dir);
	tex->set_ext(res_ext);
	tex->resize(sz);
	tex->copy_data(data, 0);
	stbi_image_free(data);
	tex->flip_horizontal();
	
	if (load_with_mipmaps_enabled)
		tex->enable_mipmap_autogen(true);
	if (vid_update_on_load)
		tex->video_update();

	dprint("nstex_manager::load_image Successfully loaded nstex2d from file " + fName);
	return tex;
}

nstex_cubemap * nstex_manager::load_cubemap(const nsstring & pXPlus,
	const nsstring & pXMinus,
	const nsstring & pYPlus,
	const nsstring & pYMinus,
	const nsstring & pZPlus,
	const nsstring & pZMinus,
	const nsstring & fname)
{
	nsstring resName(fname);
	nsstring res_ext;
	nsstring fName(fname);
	nsstring sub_dir;
	bool shouldPrefix = false;
	
	nsstring prefixdirs = m_res_dir + m_local_dir;

	size_t pos = resName.find_last_of("/\\");
	if (pos != nsstring::npos)
	{
		if (resName[0] != '/' && resName[0] != '.' && resName.find(":") == nsstring::npos) // then subdir
		{
			sub_dir = resName.substr(0, pos + 1);
			shouldPrefix = true;
		}
		resName = resName.substr(pos + 1);
	}
	else
		fName = "";

	size_t extPos = resName.find_last_of(".");
	res_ext = resName.substr(extPos);
	resName = resName.substr(0, extPos);

	if (shouldPrefix)
		fName = prefixdirs + sub_dir;
	else
		fName = path_from_filename(fName);
	
	nstex_cubemap * tex = get<nstex_cubemap>(resName);
	if (tex == NULL)
		tex = create<nstex_cubemap>(resName);
	else
		return NULL;
	
	tex->set_subdir(sub_dir);
	tex->set_ext(res_ext);

	std::vector<nsstring> fNames;
	fNames.resize(6);

	fNames[0] = fName + pXPlus; fNames[1] = fName + pXMinus;
	fNames[2] = fName + pYMinus; fNames[3] = fName + pYPlus;
	fNames[4] = fName + pZPlus; fNames[5] = fName + pZMinus;

	tex_params tp;
	tp.edge_behavior.set(te_clamp,te_clamp,te_clamp);
	tex->set_parameters(tp);

	ivec2 first_sz, cur_sz; int32 first_bpp, cur_bpp;
	for (uint32 i = 0; i < fNames.size(); ++i)
	{
        uint8 * data = stbi_load(fNames[i].c_str(), &cur_sz.x, &cur_sz.y, &cur_bpp, 0);

		if (i == 0)
		{
			first_sz = cur_sz; first_bpp = cur_bpp;
			
            if (data == nullptr || !set_tex_format(first_bpp, tex))
			{
				dprint("nstex_manager::load_cubemap Error setting format for cubemap " + fName);
				destroy(resName);
				return nullptr;		
			}
			tex->resize(first_sz, true);
		}

		if (data == nullptr || cur_sz != first_sz || cur_bpp != first_bpp)
		{
			dprint("nstex_manager::load_cubemap Error loading image data from " + fNames[0]);
			uint8 dat = 0x80;
			tex->copy_data(&dat, 1, i, true); // set all face data to grey (0x80,0x80,0x80,0x80)
			stbi_image_free(data);
			continue;
		}

		tex->copy_data(data, i);
		dprint("nstex_manager::load_cubemap Successfully loaded face " + std::to_string(i) + " of cubemap from file " + fNames[i]);
		stbi_image_free(data);
		tex->flip_horizontal(i);
	}
	
	if (load_with_mipmaps_enabled)
		tex->enable_mipmap_autogen(true);
	if (vid_update_on_load)
		tex->video_update();

	dprint("nstex_manager::load_cubemap Successfully loaded nstex_cubemap with name " + tex->name());
	return tex;
}

nstex_cubemap * nstex_manager::load_cubemap(const nsstring & fname, const nsstring & cube_all_ext)
{
	nsstring cube_name = name_from_filename(fname);		
	if (cube_all_ext.find_first_of(".") == nsstring::npos)
	{
		return load_cubemap(
			cube_name + "_front." + cube_all_ext,
			cube_name + "_back." + cube_all_ext,
			cube_name + "_bottom." + cube_all_ext,
			cube_name + "_top." + cube_all_ext,
			cube_name + "_left." + cube_all_ext,
			cube_name + "_right." + cube_all_ext,
			fname);
	}
	else
	{
		return load_cubemap(
			cube_name + "_front" + cube_all_ext,
			cube_name + "_back" + cube_all_ext,
			cube_name + "_bottom" + cube_all_ext,
			cube_name + "_top" + cube_all_ext,
			cube_name + "_left" + cube_all_ext,
			cube_name + "_right" + cube_all_ext,
			fname);		
	}
}

bool nstex_manager::del(nsasset * res)
{
	if (res == NULL)
		return false;
	if (res->extension() != ".cube" && res->extension() != ".CUBE")
		return nsasset_manager::del(res);


	nsstring dir = m_res_dir + m_local_dir + res->subdir();
	nsstring fName = dir + res->name();
    bool ret = platform::remove(fName + "_front" + DEFAULT_TEX_EXTENSION);
    ret |= platform::remove(fName + "_back" + DEFAULT_TEX_EXTENSION);
    ret |= platform::remove(fName + "_top" + DEFAULT_TEX_EXTENSION);
    ret |= platform::remove(fName + "_bottom" + DEFAULT_TEX_EXTENSION);
    ret |= platform::remove(fName + "_left" + DEFAULT_TEX_EXTENSION);
    ret |= platform::remove(fName + "_right" + DEFAULT_TEX_EXTENSION);

	if (ret)
	{
		dprint("nsres_manager::del - Succesfully deleted file with name: " + fName);
		destroy(res);
	}
	else
	{
		dprint("nsres_manager::del - Could not delete file with name: " + fName);
	}
	
	return ret;
}

bool nstex_manager::save(nsasset * res, const nsstring & path)
{
	nstex2d * casted2d = dynamic_cast<nstex2d*>(res);
	nstex_cubemap * castedCube = dynamic_cast<nstex_cubemap*>(res);
	
	if (castedCube != NULL)
		return save_cubemap(castedCube, path);
	else if (casted2d != NULL)
		return save_image(casted2d, path);
	else
		return nsasset_manager::save(res, path);
}

bool nstex_manager::save_cubemap(nstex_cubemap * cubemap, const nsstring & path)
{
	if (cubemap == NULL)
	{
		dprint("nstex_manager::save Cannot save NULL valued cubemap");
		return false;
	}

	nsstring fName(cubemap->name() + cubemap->extension());

	if (path == "")
		fName = m_res_dir + m_local_dir + cubemap->subdir() + fName;
	else
		fName = path + fName;

    bool fret = platform::create_dir(fName);
	if (fret)
    {
		dprint("nstex_manager::save Created directory " + fName);
    }	

	if (cubemap->extension() == ".cube" || cubemap->extension() == ".CUBE")
	{
		nsstring faceName;
		nsstring localFname;
	    fName = path_from_filename(fName);
		bool ret = true;
		for (uint32 curFace = 0; curFace < 6; ++curFace)
		{
			switch (curFace)
			{
			case(0) :
				faceName = "_front";
				break;
			case(1) :
				faceName = "_back";
				break;
			case(2) :
				faceName = "_top";
				break;
			case(3) :
				faceName = "_bottom";
				break;
			case(4) :
				faceName = "_left";
				break;
			case(5) :
				faceName = "_right";
				break;
			}
            localFname = fName + cubemap->name() + faceName + nsstring(DEFAULT_TEX_EXTENSION);

			uint8 * cube_dat = cubemap->data(curFace);
			if (cube_dat == nullptr)
			{
				ret = false;
				continue;
			}

			ivec2 sz = cubemap->size();
			if (DEFAULT_TEX_EXTENSION == ".png" || DEFAULT_TEX_EXTENSION == ".jpg")
			{
				ret = stbi_write_png(
					localFname.c_str(),
					sz.w,
					sz.h,
					cubemap->bytes_per_pixel()/cubemap->bytes_per_channel(),
					cube_dat,
					sz.w*cubemap->bytes_per_pixel());
			}
			else if (DEFAULT_TEX_EXTENSION == ".tga")
			{
				ret = stbi_write_tga(
					localFname.c_str(),
					sz.w,
					sz.h,
					cubemap->bytes_per_pixel()/cubemap->bytes_per_channel(),
					cube_dat);
			}
			else if (DEFAULT_TEX_EXTENSION == ".hdr")
			{
				ret = stbi_write_hdr(
					localFname.c_str(),
					sz.w,
					sz.h,
					cubemap->bytes_per_pixel()/cubemap->bytes_per_channel(),
					(const float*)cube_dat);
			}
			else
			{
				ret = stbi_write_bmp(
					localFname.c_str(),
					sz.w,
					sz.h,
					cubemap->bytes_per_pixel()/cubemap->bytes_per_channel(),
					cube_dat);
			}
			if (ret)
			{
				dprint("nstex_manager::save Successfully saved cubemap face " + std::to_string(curFace) +
					   " from " + cubemap->name() + " to " + localFname);
				return true;
			}
			dprint("nstex_manager::save Could not save cubemap face " + std::to_string(curFace) +
				   " from " + cubemap->name() + " to " + fName);
		}
		return true;
	}
	else
	{
		dprint("nstex_manager::save Cannot save cubemap with extension differing from .cube or .CUBE");
		return false;
	}
}

bool nstex_manager::save_image(nstex2d * image, const nsstring & path)
{
	if (image == NULL)
		return false;

	nsstring fName(image->name() + image->extension());

	if (path == "")
		fName = m_res_dir + m_local_dir + image->subdir() + fName;
	else
		fName = path + fName;

    bool fret = platform::create_dir(fName);
	if (fret)
    {
		dprint("nstex_manager::save Created directory " + fName);
    }

	uint8 * image_data = image->data();
	ivec2 sz = image->size();
	if (image_data == NULL)
		return false;

	int ret = 0;
	if (image->extension() == ".png" || image->extension() == ".jpg")
	{
		image->set_ext(".png");
		ret = stbi_write_png(
			fName.c_str(),
			sz.w,
			sz.h,
			image->bytes_per_pixel()/image->bytes_per_channel(),
			image_data,
			sz.w*image->bytes_per_pixel());
	}
	else if (image->extension() == ".tga")
	{
		ret = stbi_write_tga(
			fName.c_str(),
			sz.w,
			sz.h,
			image->bytes_per_pixel()/image->bytes_per_channel(),
			image_data);
	}
	else if (image->extension() == ".hdr")
	{
		ret = stbi_write_hdr(
			fName.c_str(),
			sz.w,
			sz.h,
			image->bytes_per_pixel()/image->bytes_per_channel(),
			(float*)image_data);
	}
	else
	{
		ret = stbi_write_bmp(
			fName.c_str(),
			sz.w,
			sz.h,
			image->bytes_per_pixel()/image->bytes_per_channel(),
			image_data);
	}
	if (ret)
	{
		dprint("nstex_manager::save_image Successfully saved image " + image->name() + " to " + fName);
		return true;
	}
	dprint("nstex_manager::save_image Could not save image " + image->name() + " to " + fName);
	return false;
}

int set_tex_format(int bpp, nstexture * tex)
{
	if (bpp == 4)
	{
		dprint("set_tex_bpp - setting texture to rgba with single byte per comp and 4 bpp");
		tex->set_format(tex_rgba);
	}
	else if (bpp == 3)
	{
		dprint("set_tex_bpp - setting texture to rgb with single byte per comp and 3 bpp");
		tex->set_format(tex_rgb);
	}
	else if (bpp == 2)
	{
		dprint("set_tex_bpp - setting texture to rg with single byte per comp and 2 bpp");
		tex->set_format(tex_rg);
	}
	else if (bpp == 1)
	{
		dprint("set_tex_bpp - setting texture to red with single byte per comp/pixel");
		tex->set_format(tex_red);
	}
	else
	{
		return 0;
	}
	tex->set_component_data_type(tex_u8);
	return 1;
}

