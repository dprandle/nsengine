/*! 
	\file nstexmanager.cpp
	
	\brief Header file for NSTexManager class

	This file contains all of the neccessary definitions for the NSTexManager class.

	\author Daniel Randle
	\date November 23 2013
	\copywrite Earth Banana Games 2013
*/
#include <nstexmanager.h>
#include <nstexture.h>
#include <nsengine.h>
#include <IL/il.h>
#include <IL/ilu.h>
#include <soil/SOIL.h>
#include <nsfileos.h>

using namespace nsfileio;

NSTexManager::NSTexManager(): NSResManager()
{
	setLocalDirectory(LOCAL_TEXTURE_DIR_DEFAULT);
}

NSTexManager::~NSTexManager()
{}

NSTexture * NSTexManager::load(uint32 res_type_id, const nsstring & fname)
{
	if (res_type_id == type_to_hash(NSTex2D))
		return loadImage(fname);
	else if (res_type_id == type_to_hash(NSTexCubeMap))
	{
		nsstring texExtension = "";
		nsstring texName = nameFromFilename(fname);
		size_t extPos = fname.find_last_of(".");

		if (extPos != nsstring::npos)
			texExtension = fname.substr(extPos);
		else
			return NULL;

		if (texExtension == ".cube" || texExtension == ".CUBE")
		{
			return loadCubemap(
				texName + "_front.png",
				texName + "_back.png",
				texName + "_top.png",
				texName + "_bottom.png",
				texName + "_left.png",
				texName + "_right.png",
				fname);
		}
		else
			return loadCubemap(fname);
	}
	else
		return static_cast<NSTexture*>(NSResManager::load(res_type_id, fname));
}

NSTex2D * NSTexManager::loadImage(const nsstring & fname)
{
	nsstring resName(fname);
	nsstring resExtension;
	nsstring fName;
	nsstring subDir;
	bool shouldPrefix = false;
	
	nsstring prefixdirs = mResourceDirectory + mLocalDirectory;

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

	NSTex2D * tex = get<NSTex2D>(resName);
	if (tex == NULL)
		tex = create<NSTex2D>(resName); // possible else clear - will overwrite existing tex though
	else
		return NULL;

	tex->setSubDir(subDir); // should be "" for false appendDirectories
	tex->setExtension(resExtension);

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
			dprint("NSTexManager::loadImage Could not load NSTex2D from file " + fName);
			ilDeleteImages(1, &imageID);
			uint32 err = ilGetError();
			destroy(resName);
			return NULL;
		}
		dprint("NSTexManager::loadImage Could not convert NSTex2D with name " + tex->name() + " to specified format");
		ilDeleteImages(1, &imageID);
		destroy(resName);
		return NULL;
	}

	uivec2 dim;
	// Generate the texture and assign data from the data loaded by IL
	tex->setInternalFormat(GL_RGBA);
	tex->setFormat(GL_RGBA);
	dim.w = ilGetInteger(IL_IMAGE_WIDTH);
	dim.h = ilGetInteger(IL_IMAGE_HEIGHT);
	tex->setdim(dim);
	tex->setPixelDataType(GL_UNSIGNED_BYTE);

	tex->initGL();
	tex->bind();
	tex->allocate(ilGetData());
	tex->enableMipMaps(0);
	tex->setParameteri(NSTexture::MagFilter, GL_LINEAR);
	tex->setParameteri(NSTexture::MinFilter, GL_LINEAR_MIPMAP_LINEAR);
	tex->unbind();
	ilDeleteImages(1, &imageID);
	dprint("NSTexManager::loadImage Successfully loaded NSTex2D from file " + fName);
	return tex;
}

NSTexCubeMap * NSTexManager::loadCubemap(const nsstring & pXPlus,
	const nsstring & pXMinus,
	const nsstring & pYPlus,
	const nsstring & pYMinus,
	const nsstring & pZPlus,
	const nsstring & pZMinus,
	const nsstring & fname)
{
	nsstring resName(fname);
	nsstring resExtension;
	nsstring fName;
	nsstring subDir;
	bool shouldPrefix = false;
	
	nsstring prefixdirs = mResourceDirectory + mLocalDirectory;

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
		fName = pathFromFilename(fname);
	
	NSTexCubeMap * tex = get<NSTexCubeMap>(resName);
	if (tex == NULL)
		tex = create<NSTexCubeMap>(resName);
	else
		return NULL;
	
	tex->setSubDir(subDir);
	tex->setExtension(resExtension);

	std::vector<nsstring> fNames;
	fNames.resize(6);

	fNames[0] = fName + pXPlus; fNames[1] = fName + pXMinus;
	fNames[2] = fName + pYPlus; fNames[3] = fName + pYMinus;
	fNames[4] = fName + pZPlus; fNames[5] = fName + pZMinus;

	tex->initGL();
	tex->bind();

	for (uint32 i = 0; i < fNames.size(); ++i)
	{
		ILuint imageID;
		ilGenImages(1, &imageID);
		ilBindImage(imageID);

		int32 worked = ilLoadImage((const ILstring)fNames[i].c_str());
		int32 converted = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
		if (!worked || !converted)
		{
			if (!worked)
			{
				dprint("NSTexManager::loadCubemap Could not load NSTexCubemap from file " + fNames[i]);
				ilDeleteImages(1, &imageID);
				destroy(resName);
				return NULL;
			}

			dprint("NSTexManager::loadCubemap Could not convert NSTexCubemap with name " + tex->name() + " to specified format");
			ilDeleteImages(1, &imageID);
			destroy(resName);
			return NULL;
		}

		tex->setInternalFormat(GL_RGBA);
		tex->setFormat(GL_RGBA);
		tex->setdim(uivec2(ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT)));
		tex->setPixelDataType(GL_UNSIGNED_BYTE);
		tex->setAllocated(false);
		tex->allocate(NSTexCubeMap::CubeFace(BASE_CUBEMAP_FACE + i), (const void*)ilGetData());
		dprint("NSTexManager::loadCubemap Successfully loaded NSTexCubemap from file " + fNames[i]);
		ilDeleteImages(1, &imageID);
	}

	// Generate mipmap levels if mMimMapLevel is greater than 0
	tex->enableMipMaps();
	tex->setParameteri(NSTexture::MagFilter, GL_LINEAR);
	tex->setParameteri(NSTexture::MinFilter, GL_LINEAR_MIPMAP_LINEAR);
	tex->setParameteri(NSTexture::WrapS, GL_CLAMP_TO_EDGE);
	tex->setParameteri(NSTexture::WrapT, GL_CLAMP_TO_EDGE);
	tex->setParameteri(NSTexture::WrapR, GL_CLAMP_TO_EDGE);
	tex->unbind();
	dprint("NSTexManager::loadCubemap Successfully loaded NSTexCubemap with name " + tex->name());
	return tex;
}

NSTexCubeMap * NSTexManager::loadCubemap(const nsstring & fname)
{
	nsstring resName(fname);
	nsstring resExtension;
	nsstring fName;
	nsstring subDir;
	bool shouldPrefix = false;
	
	nsstring prefixdirs = mResourceDirectory + mLocalDirectory;

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
		fName = pathFromFilename(fname);

	NSTexCubeMap * tex = get<NSTexCubeMap>(resName);
	if (tex == NULL)
		tex = create<NSTexCubeMap>(resName); // possible else clear - will overwrite existing tex though
	else
		return NULL;

	tex->setSubDir(subDir); // should be "" for false appendDirectories
	tex->setExtension(resExtension);

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
		dprint("NSTexManager::loadCubemap Error loading NSTexCubemap from file " + fName);
		destroy(resName);
		return NULL;
	}

	tex->setGLName(glid);
	tex->bind();
	tex->setFormat(GL_RGBA);
	uivec2 dim;
	dim.w = tex->getParameteri(NSTexture::Width);
	dim.h = tex->getParameteri(NSTexture::Height);
	tex->setInternalFormat(tex->getParameteri(NSTexture::InternalFormat));
	tex->setdim(dim);
	tex->setPixelDataType(GL_UNSIGNED_BYTE);
	tex->enableMipMaps();
	tex->setAllocated(true);
	tex->unbind();
	dprint("NSTexManager::loadCubemap Successfully loaded NSTexCubemap from file " + fName);
	return tex;
}

bool NSTexManager::save(NSResource * res, const nsstring & path)
{
	NSTex2D * casted2d = dynamic_cast<NSTex2D*>(res);
	NSTexCubeMap * castedCube = dynamic_cast<NSTexCubeMap*>(res);
	
	if (castedCube != NULL)
		return save(castedCube, path);
	else if (casted2d != NULL)
		return save(casted2d, path);
	else
		return NSResManager::save(res, path);
}

bool NSTexManager::save(NSTexCubeMap * cubemap, const nsstring & path)
{
	ILuint imageID = 0;
	int32 savetype = 0;

	if (cubemap == NULL)
	{
		dprint("NSTexManager::save : Cannot save NULL valued cubemap");
		return false;
	}

	nsstring fName(cubemap->name() + cubemap->extension());

	if (path == "")
		fName = mResourceDirectory + mLocalDirectory + cubemap->subDir() + fName;
	else
		fName = path + fName;

	bool fret = create_dir(fName);
	if (fret)
		dprint("NSTexManager::save Created directory " + fName);
	

	if (cubemap->extension() == ".cube" || cubemap->extension() == ".CUBE")
	{
		nsstring faceName;
	    fName = pathFromFilename(fName);
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
			localFname = fName + faceName + nsstring(DEFAULT_TEX_EXTENSION);

			cubemap->bind();
			cubemap->lock(NSTexCubeMap::CubeFace(BASE_CUBEMAP_FACE + curFace));
			cubemap->unbind();
			if (cubemap->data().data == NULL)
			{
				ret = false;
				continue;
			}

			ilEnable(IL_FILE_OVERWRITE);
			ilGenImages(1, &imageID);
			ilBindImage(imageID);
			ilTexImage(cubemap->dim().w,
					   cubemap->dim().h,
					   1,
					   cubemap->channels(),
					   cubemap->format(),
					   cubemap->pixelDataType(),
					   cubemap->data().data);
			ret = true && ilSaveImage((const ILstring)localFname.c_str());
			ilDeleteImages(1, &imageID);
			imageID = 0;
			cubemap->bind();
			cubemap->unlock(NSTexCubeMap::CubeFace(BASE_CUBEMAP_FACE + curFace));
			cubemap->unbind();
			ILenum err = ilGetError();
			while (err != IL_NO_ERROR)
			{
				dprint("NSTexManager::saveCubemap : Error saving NSTexCubemap to file " + localFname);
				err = ilGetError();
			}
		}
		if (ret)
			dprint("NSTexManager::loadCubemap Successfully saved NSTexCubemap to file " + fName);
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
		dprint("NSTexManager::saveCubemap Unsupported cubemap format " + cubemap->extension());
		return false;
	}
	cubemap->bind();
	cubemap->lock();
	cubemap->unbind();

	if (cubemap->data().data == NULL)
		return false;

	uint32 ret = SOIL_save_image(fName.c_str(), savetype, cubemap->dim().w, cubemap->dim().h*6, cubemap->channels(), (uint8*)cubemap->data().data);
	cubemap->bind();
	cubemap->unlock();
	cubemap->unbind();

	if (ret == 0)
	{
		dprint("NSTexManager::loadCubemap Could not save NSTexCubemap to file " + fName + " Error: " + nsstring(SOIL_last_result()));
	}
	else
	{
		dprint("NSTexManager::loadCubemap Successfully saved NSTexCubemap to file " + fName);
	}

	return (ret == 0);
}

bool NSTexManager::save(NSTex2D * image, const nsstring & path)
{
	if (image == NULL)
	{
		dprint("NSTexManager::save : Cannot save NULL valued image");
		return false;
	}

	nsstring fName(image->name() + image->extension());

	if (path == "")
		fName = mResourceDirectory + mLocalDirectory + image->subDir() + fName;
	else
		fName = path + fName;

	bool fret = create_dir(fName);
	if (fret)
		dprint("NSTexManager::save Created directory " + fName);

	image->bind();
	image->lock();
	image->unbind();

	if (image->data().data == NULL)
		return false;

	ILuint imageID = 0;
	ilEnable(IL_FILE_OVERWRITE);
	ilGenImages(1, &imageID);
	ilBindImage(imageID);
	ilTexImage(image->dim().w,
			   image->dim().h,
			   1,
			   image->channels(),
			   image->format(),
			   image->pixelDataType(),
			   image->data().data);

	bool ret = ilSaveImage((const ILstring)fName.c_str()) && true;
	ilDeleteImages(1, &imageID);
	image->bind();
	image->unlock();
	image->unbind();

	ILenum err = ilGetError();
	while (err != IL_NO_ERROR)
	{
		dprint("NSTexManager::saveImage : Error saving NSTex2D to file " + fName);
		err = ilGetError();
	}

	if (ret)
	{
		dprint("NSTexManager::saveImage Successfully saved NSTex2D to file " + fName);
	}
	else
	{
		dprint("NSTexManager::saveImage Could not save NSTex2D to file " + fName);
	}

	return ret;
}
