/*! 
	\file nstexmanager.cpp
	
	\brief Header file for nstex_manager class

	This file contains all of the neccessary definitions for the nstex_manager class.

	\author Daniel Randle
	\date November 23 2013
	\copywrite Earth Banana Games 2013
*/
#include <iostream>

#include <IL/il.h>
#include <soil/SOIL.h>

#include <nstex_manager.h>
#include <nsfile_os.h>
#include <nstexture.h>
#include <nsengine.h>

nstex_manager::nstex_manager(): nsres_manager()
{
	set_local_dir(LOCAL_TEXTURE_DIR_DEFAULT);
}

nstex_manager::~nstex_manager()
{}

nstexture * nstex_manager::load(uint32 res_type_id, const nsstring & fname)
{
	if (fname.empty())
		return NULL;
	
	if (res_type_id == type_to_hash(nstex2d))
		return load_image(fname);
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
			return load_cubemap(fname,".png");
		else
			return load_cubemap(fname);
	}
	else
		return static_cast<nstexture*>(nsres_manager::load(res_type_id, fname));
}

nstex2d * nstex_manager::load_image(const nsstring & fname)
{
	if (fname.empty())
		return NULL;
	
	nsstring resName(fname);
	nsstring resExtension;
	nsstring fName;
	nsstring subDir;
	bool shouldPrefix = false;
	
	nsstring prefixdirs = m_res_dir + m_local_dir;

	size_t pos = resName.find_last_of("/\\");
	if (pos != nsstring::npos)
	{
		if (resName[0] != '/' && resName[0] != '.' && resName.find(":") == nsstring::npos) // then subdir
		{
			subDir = resName.substr(0, pos + 1);
			shouldPrefix = true;
		}
		resName = resName.substr(pos + 1);
	}
	else
		shouldPrefix = true;

	size_t extPos = resName.find_last_of(".");
	resExtension = resName.substr(extPos);
	resName = resName.substr(0, extPos);

	if (shouldPrefix)
		fName = prefixdirs + subDir + resName + resExtension;
	else
		fName = fname;

	nstex2d * tex = get<nstex2d>(resName);
	if (tex == NULL)
		tex = create<nstex2d>(resName); // possible else clear - will overwrite existing tex though
	else
		return NULL;

	tex->set_subdir(subDir); // should be "" for false appendDirectories
	tex->set_ext(resExtension);

	// Load the image using the IL loading library
	ILuint imageID;
	ilGenImages(1, &imageID);
	ilBindImage(imageID);

	// Make sure worked - if not send error message to log file
	
	int32 worked = ilLoadImage(fName.c_str());
	int32 converted = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
	if (!worked || !converted)
	{
		if (!worked)
		{
			dprint("nstex_manager::load_image Could not load nstex2d from file " + fName);
			ilDeleteImages(1, &imageID);
			uint32 err = ilGetError();
			destroy(resName);
			return NULL;
		}
		dprint("nstex_manager::load_image Could not convert nstex2d with name " + tex->name() + " to specified format");
		ilDeleteImages(1, &imageID);
		destroy(resName);
		return NULL;
	}

	uivec2 dim;
	// Generate the texture and assign data from the data loaded by IL
	tex->set_internal_format(GL_RGBA);
	tex->set_format(GL_RGBA);
	dim.w = ilGetInteger(IL_IMAGE_WIDTH);
	dim.h = ilGetInteger(IL_IMAGE_HEIGHT);
	tex->resize(dim);
	tex->set_pixel_data_type(GL_UNSIGNED_BYTE);

	tex->bind();
	tex->allocate(ilGetData());
	tex->enable_mipmaps(0);
	tex->set_parameter_i(nstexture::mag_filter, GL_LINEAR);
	tex->set_parameter_i(nstexture::min_filter, GL_LINEAR_MIPMAP_LINEAR);
	tex->unbind();
	ilDeleteImages(1, &imageID);
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
	nsstring resExtension;
	nsstring fName(fname);
	nsstring subDir;
	bool shouldPrefix = false;
	
	nsstring prefixdirs = m_res_dir + m_local_dir;

	size_t pos = resName.find_last_of("/\\");
	if (pos != nsstring::npos)
	{
		if (resName[0] != '/' && resName[0] != '.' && resName.find(":") == nsstring::npos) // then subdir
		{
			subDir = resName.substr(0, pos + 1);
			shouldPrefix = true;
		}
		resName = resName.substr(pos + 1);
	}
	else
		fName = "";

	size_t extPos = resName.find_last_of(".");
	resExtension = resName.substr(extPos);
	resName = resName.substr(0, extPos);

	if (shouldPrefix)
		fName = prefixdirs + subDir;
	else
		fName = path_from_filename(fName);
	
	nstex_cubemap * tex = get<nstex_cubemap>(resName);
	if (tex == NULL)
		tex = create<nstex_cubemap>(resName);
	else
		return NULL;
	
	tex->set_subdir(subDir);
	tex->set_ext(resExtension);

	std::vector<nsstring> fNames;
	fNames.resize(6);

	fNames[0] = fName + pXPlus; fNames[1] = fName + pXMinus;
	fNames[2] = fName + pYPlus; fNames[3] = fName + pYMinus;
	fNames[4] = fName + pZPlus; fNames[5] = fName + pZMinus;

	tex->bind();

	for (uint32 i = 0; i < fNames.size(); ++i)
	{
		ILuint imageID;
		ilGenImages(1, &imageID);
		ilBindImage(imageID);

		int32 worked = ilLoadImage(fNames[i].c_str());
		int32 converted = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
		if (!worked || !converted)
		{
			if (!worked)
			{
				dprint("nstex_manager::load_cubemap Could not load nstex_cubemap from file " + fNames[i]);
				ilDeleteImages(1, &imageID);
				destroy(resName);
				return NULL;
			}

			dprint("nstex_manager::load_cubemap Could not convert nstex_cubemap with name " + tex->name() + " to specified format");
			ilDeleteImages(1, &imageID);
			destroy(resName);
			return NULL;
		}
		tex->set_allocated(false);
		tex->set_internal_format(GL_RGBA);
		tex->set_format(GL_RGBA);
		tex->set_pixel_data_type(GL_UNSIGNED_BYTE);
		tex->resize(uivec2(ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT)));
		tex->allocate(nstex_cubemap::cube_face(BASE_CUBEMAP_FACE + i), (const void*)ilGetData());
		dprint("nstex_manager::load_cubemap Successfully loaded face " + std::to_string(i) + " of cubemap from file " + fNames[i]);
		ilDeleteImages(1, &imageID);
	}

	// Generate mipmap levels if mMimMapLevel is greater than 0
	tex->enable_mipmaps();
	tex->set_parameter_i(nstexture::mag_filter, GL_LINEAR);
	tex->set_parameter_i(nstexture::min_filter, GL_LINEAR_MIPMAP_LINEAR);
	tex->set_parameter_i(nstexture::wrap_s, GL_CLAMP_TO_EDGE);
	tex->set_parameter_i(nstexture::wrap_t, GL_CLAMP_TO_EDGE);
	tex->set_parameter_i(nstexture::wrap_r, GL_CLAMP_TO_EDGE);
	tex->unbind();
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

bool nstex_manager::del(nsresource * res)
{
	if (res == NULL)
		return false;
	if (res->extension() != ".cube" && res->extension() != ".CUBE")
		return nsres_manager::del(res);


	nsstring dir = m_res_dir + m_local_dir + res->subdir();
	nsstring fName = dir + res->name();
    bool ret = nsfile_os::remove(fName + "_front" + DEFAULT_TEX_EXTENSION);
    ret |= nsfile_os::remove(fName + "_back" + DEFAULT_TEX_EXTENSION);
    ret |= nsfile_os::remove(fName + "_top" + DEFAULT_TEX_EXTENSION);
    ret |= nsfile_os::remove(fName + "_bottom" + DEFAULT_TEX_EXTENSION);
    ret |= nsfile_os::remove(fName + "_left" + DEFAULT_TEX_EXTENSION);
    ret |= nsfile_os::remove(fName + "_right" + DEFAULT_TEX_EXTENSION);

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

nstex_cubemap * nstex_manager::load_cubemap(const nsstring & fname)
{
	nsstring resName(fname);
	nsstring resExtension;
	nsstring fName;
	nsstring subDir;
	bool shouldPrefix = false;
	
	nsstring prefixdirs = m_res_dir + m_local_dir;

	size_t pos = resName.find_last_of("/\\");
	if (pos != nsstring::npos)
	{
		if (resName[0] != '/' && resName[0] != '.' && resName.find(":") == nsstring::npos) // then subdir
		{
			subDir = resName.substr(0, pos + 1);
			shouldPrefix = true;
		}
		resName = resName.substr(pos + 1);
	}
	else
		shouldPrefix = true;

	size_t extPos = resName.find_last_of(".");
	resExtension = resName.substr(extPos);
	resName = resName.substr(0, extPos);

	if (shouldPrefix)
		fName = prefixdirs + subDir;
	else
		fName = path_from_filename(fname);

	nstex_cubemap * tex = get<nstex_cubemap>(resName);
	if (tex == NULL)
		tex = create<nstex_cubemap>(resName); // possible else clear - will overwrite existing tex though
	else
		return NULL;

	tex->set_subdir(subDir); // should be "" for false appendDirectories
	tex->set_ext(resExtension);

	uint32 glid = 0;

	if (tex->extension() == ".dds")
		glid = SOIL_load_OGL_single_cubemap(
			fName.c_str(),
			SOIL_DDS_CUBEMAP_FACE_ORDER,
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_MIPMAPS);
	else
		glid = SOIL_load_OGL_single_cubemap(
			fName.c_str(),
			"EWUDNS",
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_MIPMAPS);

	if (glid == 0)
	{
		dprint("nstex_manager::load_cubemap Error loading nstex_cubemap from file " + fName);
		destroy(resName);
		return NULL;
	}

	tex->set_gl_name(glid);
	tex->bind();
	tex->set_format(GL_RGBA);
	uivec2 dim;
	dim.w = tex->parameter_i(nstexture::tex_width);
	dim.h = tex->parameter_i(nstexture::tex_height);
	tex->set_internal_format(tex->parameter_i(nstexture::tex_internal_format));
	tex->resize(dim);
	tex->set_pixel_data_type(GL_UNSIGNED_BYTE);
	tex->enable_mipmaps();
	tex->set_allocated(true);
	tex->unbind();
	dprint("nstex_manager::load_cubemap Successfully loaded nstex_cubemap from file " + fName);
	return tex;
}

bool nstex_manager::save(nsresource * res, const nsstring & path)
{
	nstex2d * casted2d = dynamic_cast<nstex2d*>(res);
	nstex_cubemap * castedCube = dynamic_cast<nstex_cubemap*>(res);
	
	if (castedCube != NULL)
		return save(castedCube, path);
	else if (casted2d != NULL)
		return save(casted2d, path);
	else
		return nsres_manager::save(res, path);
}

bool nstex_manager::save(nstex_cubemap * cubemap, const nsstring & path)
{
	ILuint imageID = 0;
	int32 savetype = 0;

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

    bool fret = nsfile_os::create_dir(fName);
	if (fret)
    {
		dprint("nstex_manager::save Created directory " + fName);
    }
	

	if (cubemap->extension() == ".cube" || cubemap->extension() == ".CUBE")
	{
		nsstring faceName;
	    fName = path_from_filename(fName);
		nsstring localFname;
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

			cubemap->bind();
			nstexture::image_data tex_data = cubemap->data(
				nstex_cubemap::cube_face(BASE_CUBEMAP_FACE + curFace));
			cubemap->unbind();
			if (tex_data.data == NULL)
			{
				ret = false;
				continue;
			}

			ilEnable(IL_FILE_OVERWRITE);
			ilGenImages(1, &imageID);
			ilBindImage(imageID);
			ilTexImage(cubemap->size().w,
					   cubemap->size().h,
					   1,
					   cubemap->channels(),
					   cubemap->format(),
					   cubemap->pixel_data_type(),
					   tex_data.data);
			ret = true && ilSaveImage((const ILstring)localFname.c_str());
			ilDeleteImages(1, &imageID);
			imageID = 0;
			ILenum err = ilGetError();
			while (err != IL_NO_ERROR)
			{
				dprint("nstex_manager::saveCubemap Error saving nstex_cubemap to file " + localFname);
				err = ilGetError();
			}
		}
		if (ret)
        {
			dprint("nstex_manager::load_cubemap Successfully saved nstex_cubemap to file " + fName);
        }
		return ret;
	}
	else if (cubemap->extension() == ".tga")
	{
		savetype = SOIL_SAVE_TYPE_TGA;
	}
	else if (cubemap->extension() == ".bmp")
	{
		savetype = SOIL_SAVE_TYPE_BMP;
	}
	else if (cubemap->extension() == ".dds")
	{
		savetype = SOIL_SAVE_TYPE_DDS;
	}
	else
	{
		dprint("nstex_manager::saveCubemap Unsupported cubemap format " + cubemap->extension());
		return false;
	}
	cubemap->bind();
	nstexture::image_data tex_data = cubemap->data();
	cubemap->unbind();

	if (tex_data.data == NULL)
		return false;

	uint32 ret = SOIL_save_image(
		fName.c_str(),
		savetype,
		cubemap->size().w,
		cubemap->size().h*6,
		cubemap->channels(),
		tex_data.data);
	
	if (ret == 0)
	{
		dprint("nstex_manager::load_cubemap Could not save nstex_cubemap to file " + fName + " Error: " + nsstring(SOIL_last_result()));
	}
	else
	{
		dprint("nstex_manager::load_cubemap Successfully saved nstex_cubemap to file " + fName);
	}

	return (ret == 0);
}

bool nstex_manager::save(nstex2d * image, const nsstring & path)
{
	if (image == NULL)
	{
		dprint("nstex_manager::save Cannot save NULL valued image");
		return false;
	}

	nsstring fName(image->name() + image->extension());

	if (path == "")
		fName = m_res_dir + m_local_dir + image->subdir() + fName;
	else
		fName = path + fName;

    bool fret = nsfile_os::create_dir(fName);
	if (fret)
    {
		dprint("nstex_manager::save Created directory " + fName);
    }

	image->bind();
	nstexture::image_data tex_data = image->data();
	image->unbind();

	if (tex_data.data == NULL)
		return false;

	ILuint imageID = 0;
	ilEnable(IL_FILE_OVERWRITE);
	ilGenImages(1, &imageID);
	ilBindImage(imageID);
	ilTexImage(image->size().w,
			   image->size().h,
			   1,
			   image->channels(),
			   image->format(),
			   image->pixel_data_type(),
			   tex_data.data);

	bool ret = ilSaveImage((const ILstring)fName.c_str()) && true;
	ilDeleteImages(1, &imageID);

	ILenum err = ilGetError();
	while (err != IL_NO_ERROR)
	{
		dprint("nstex_manager::saveImage Error saving nstex2d to file " + fName);
		err = ilGetError();
	}

	if (ret)
	{
		dprint("nstex_manager::saveImage Successfully saved nstex2d to file " + fName);
	}
	else
	{
		dprint("nstex_manager::saveImage Could not save nstex2d to file " + fName);
	}

	return ret;
}
