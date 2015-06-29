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
#include <IL/ilut.h>
#include <IL/il.h>
#include <nsfileos.h>
using namespace nsfileio;

NSTexManager::NSTexManager(): NSResManager()
{
	setLocalDirectory(LOCAL_TEXTURE_DIR_DEFAULT);
}

NSTexManager::~NSTexManager()
{}

NSTexture * NSTexManager::load(const nsstring & resType, const nsstring & pFileName, bool pAppendDirectories)
{
	if (resType == NSTex2D::getTypeString())
		return loadImage(pFileName, pAppendDirectories);
	else if (resType == NSTexCubeMap::getTypeString())
	{
		nsstring texName(pFileName);
		nsstring texExtension;
		nsstring subDir;

		nsuint pos = texName.find_last_of("/\\");

		if (pos != nsstring::npos && (pos + 1) < texName.size())
		{
			if (pAppendDirectories)
				subDir = texName.substr(0, pos + 1);
			texName = texName.substr(pos + 1);
		}

		nsuint extPos = texName.find_last_of(".");

		if (extPos != nsstring::npos)
		{
			texExtension = texName.substr(extPos);
			texName = texName.substr(0, extPos);
		}

		if (texExtension == ".cube" || texExtension == ".CUBE")
		{
			return loadCubemap(
				texName + "Front.png",
				texName + "Back.png",
				texName + "Top.png",
				texName + "Bottom.png",
				texName + "Left.png",
				texName + "Right.png",
				subDir + texName,
				pAppendDirectories);
		}
		else
			return loadCubemap(pFileName, pAppendDirectories);
	}
	else
		return static_cast<NSTexture*>(NSResManager::load(resType, pFileName, pAppendDirectories));
}

NSTex2D * NSTexManager::loadImage(const nsstring & filename, bool pAppendDirectories)
{
	nsstring texName(filename);
	nsstring texExtension;
	nsstring fName(filename);
	nsstring subDir;

	nsuint pos = texName.find_last_of("/\\");

	if (pos != nsstring::npos && (pos + 1) < texName.size())
	{
		if (pAppendDirectories)
			subDir = texName.substr(0, pos + 1);
		texName = texName.substr(pos + 1);
	}

	nsuint extPos = texName.find_last_of(".");

	if (extPos != nsstring::npos)
	{
		texExtension = texName.substr(extPos);
		texName = texName.substr(0, extPos);
	}

	if (pAppendDirectories)
		fName = mResourceDirectory + mLocalDirectory + subDir + texName + texExtension;

	NSTex2D * tex = get<NSTex2D>(texName);
	if (tex == NULL)
		tex = create<NSTex2D>(texName); // possible else clear - will overwrite existing tex though
	else
		return NULL;

	tex->setSubDir(subDir); // should be "" for false appendDirectories
	tex->setExtension(texExtension);

	// Load the image using the IL loading library
	ILuint imageID;
	ilGenImages(1, &imageID);
	ilBindImage(imageID);

	// Make sure worked - if not send error message to log file
	int worked = ilLoadImage(fName.c_str());
	int converted = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
	if (!worked || !converted)
	{
		if (!worked)
		{
			dprint("NSTexManager::loadImage Could not load " + tex->typeString() + " from file " + fName);
			ilDeleteImages(1, &imageID);
			unload(texName);
			return NULL;
		}
		dprint("NSTexManager::loadImage Could not convert " + tex->typeString() + " with name " + tex->name() + " to specified format");
		ilDeleteImages(1, &imageID);
		unload(texName);
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
	dprint("NSTexManager::loadImage Successfully loaded " + tex->typeString() + " from file " + fName);
	return tex;
}

NSTexCubeMap * NSTexManager::loadCubemap(const nsstring & pXPlus,
	const nsstring & pXMinus,
	const nsstring & pYPlus,
	const nsstring & pYMinus,
	const nsstring & pZPlus,
	const nsstring & pZMinus,
	const nsstring & pResourceName,
	bool pAppendDirectories)
{
	nsstring texName(pResourceName);
	nsstring subDir;

	nsuint pos = texName.find_last_of("/\\");

	if (pos != nsstring::npos && (pos + 1) < texName.size())
	{
		if (pAppendDirectories)
			subDir = texName.substr(0, pos + 1);
		texName = texName.substr(pos + 1);
	}

	nsuint extPos = texName.find_last_of(".");
	if (extPos != nsstring::npos)
		texName = texName.substr(0, extPos);

	NSTexCubeMap * tex = get<NSTexCubeMap>(texName);
	if (tex == NULL)
		tex = create<NSTexCubeMap>(texName);
	else
		return NULL;

	tex->setSubDir(subDir);
	tex->setExtension(".cube");

	std::vector<nsstring> fNames;
	fNames.resize(6);
	if (pAppendDirectories == true)
	{
		nsstring preFix = mResourceDirectory + mLocalDirectory + tex->subDir();
		fNames[0] = preFix + pXPlus; fNames[1] = preFix + pXMinus;
		fNames[2] = preFix + pYPlus; fNames[3] = preFix + pYMinus;
		fNames[4] = preFix + pZPlus; fNames[5] = preFix + pZMinus;
	}
	else
	{
		fNames[0] = pXPlus; fNames[1] = pXMinus;
		fNames[2] = pYPlus; fNames[3] = pYMinus;
		fNames[4] = pZPlus; fNames[5] = pZMinus;
	}

	tex->initGL();
	tex->bind();

	for (nsuint i = 0; i < fNames.size(); ++i)
	{
		ILuint imageID;
		ilGenImages(1, &imageID);
		ilBindImage(imageID);

		int worked = ilLoadImage(fNames[i].c_str());
		int converted = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
		if (!worked || !converted)
		{
			if (!worked)
			{
				dprint("NSTexManager::loadCubemap Could not load "+tex->typeString()+" from file " + fNames[i]);
				ilDeleteImages(1, &imageID);
				unload(texName);
				return NULL;
			}

			dprint("NSTexManager::loadCubemap Could not convert "+tex->typeString()+" with name " + tex->name() + " to specified format");
			ilDeleteImages(1, &imageID);
			unload(texName);
			return NULL;
		}

		tex->setInternalFormat(GL_RGBA);
		tex->setFormat(GL_RGBA);
		tex->setdim(uivec2(ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT)));
		tex->setPixelDataType(GL_UNSIGNED_BYTE);
		tex->setAllocated(false);
		tex->allocate(NSTexCubeMap::CubeFace(BASE_CUBEMAP_FACE + i), (const void*)ilGetData());
		dprint("NSTexManager::loadCubemap Successfully loaded " + tex->typeString() + " from file " + fNames[i]);
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
	dprint("NSTexManager::loadCubemap Successfully loaded " + tex->typeString() + " with name " + tex->name());
	return tex;
}

NSTexCubeMap * NSTexManager::loadCubemap(const nsstring & filename, bool pAppendDirectories = true)
{
	nsstring texName(filename);
	nsstring texExtension;
	nsstring fName(filename);
	nsstring subDir;

	nsuint pos = texName.find_last_of("/\\");

	if (pos != nsstring::npos && (pos + 1) < texName.size())
	{
		if (pAppendDirectories)
			subDir = texName.substr(0, pos + 1);
		texName = texName.substr(pos + 1);
	}

	nsuint extPos = texName.find_last_of(".");

	if (extPos != nsstring::npos)
	{
		texExtension = texName.substr(extPos);
		texName = texName.substr(0, extPos);
	}

	if (pAppendDirectories)
		fName = mResourceDirectory + mLocalDirectory + subDir + texName + texExtension;

	NSTexCubeMap * tex = get<NSTexCubeMap>(texName);
	if (tex == NULL)
		tex = create<NSTexCubeMap>(texName); // possible else clear - will overwrite existing tex though
	else
		return NULL;

	tex->setSubDir(subDir); // should be "" for false appendDirectories
	tex->setExtension(texExtension);

	nsuint glid = 0;

	if (tex->extension() == ".dds")
		glid = SOIL_load_OGL_single_cubemap(fName.c_str(), SOIL_DDS_CUBEMAP_FACE_ORDER, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	else
		glid = SOIL_load_OGL_single_cubemap(fName.c_str(), "EWUDNS", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	if (glid == 0)
	{
		dprint("NSTexManager::loadCubemap Error loading " + tex->typeString() + " from file " + fName);
		unload(texName);
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
	dprint("NSTexManager::loadCubemap Successfully loaded " + tex->typeString() + " from file " + fName);
	return tex;
}

bool NSTexManager::save(const nsstring & resName, bool pAppendDirectories)
{
	NSTexture * tex = NULL;

	if (!pAppendDirectories)
		tex = get(nameFromFilename(resName));
	else
		tex = get(resName);

	if (tex == NULL)
	{
		dprint("NSTexManager::save : Cannot save NULL valued resource");
		return false;
	}

	nsstring fName(resName);

	if (pAppendDirectories)
		fName = mResourceDirectory + mLocalDirectory + tex->subDir() + tex->name();

	bool fret = create_dir(fName);
	if (fret)
		dprint("NSResManager::save Created directory " + fName);

	if (tex->textureType() == NSTexture::Tex2D)
		return saveImage(static_cast<NSTex2D*>(tex), fName);
	else if (tex->textureType() == NSTexture::TexCubeMap)
		return saveCubemap(static_cast<NSTexCubeMap*>(tex), fName);
	else
		return NSResManager::save(resName, pAppendDirectories);
}

bool NSTexManager::saveCubemap(NSTexCubeMap * cubemap, const nsstring & fname)
{
	ILuint imageID = 0;
	int savetype = 0;
	NSPlugin * plug = nsengine.plugin(mPlugID);

	if (cubemap->extension() == ".cube" || cubemap->extension() == ".CUBE")
	{
		nsstring faceName;
		nsstring fName;
		bool ret = true;
		for (nsuint curFace = 0; curFace < 6; ++curFace)
		{
			switch (curFace)
			{
			case(0) :
				faceName = "Front";
				break;
			case(1) :
				faceName = "Back";
				break;
			case(2) :
				faceName = "Top";
				break;
			case(3) :
				faceName = "Bottom";
				break;
			case(4) :
				faceName = "Left";
				break;
			case(5) :
				faceName = "Right";
				break;
			}
			fName = fname + faceName + nsstring(DEFAULT_TEX_EXTENSION);

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
			ilTexImage(cubemap->dim().w, cubemap->dim().h, 1, cubemap->channels(), cubemap->format(), cubemap->pixelDataType(), cubemap->data().data);

			ret = true && ilSaveImage(fName.c_str());
			ilDeleteImages(1, &imageID);
			imageID = 0;
			cubemap->bind();
			cubemap->unlock(NSTexCubeMap::CubeFace(BASE_CUBEMAP_FACE + curFace));
			cubemap->unbind();
			ILenum err = ilGetError();
			while (err != IL_NO_ERROR)
			{
				const char * errStr = iluErrorString(err);
				dprint("NSTexManager::saveCubemap : Error saving " + cubemap->typeString() + " to file " + fName + " Error: " + std::string(errStr));
				err = ilGetError();
			}
		}
		if (ret)
			dprint("NSTexManager::loadCubemap Successfully saved " + cubemap->typeString() + " to file " + fName);
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

	nsstring fName = fname + cubemap->extension();
	nsuint ret = SOIL_save_image(fName.c_str(), savetype, cubemap->dim().w, cubemap->dim().h*6, cubemap->channels(), (nsuchar*)cubemap->data().data);
	cubemap->bind();
	cubemap->unlock();
	cubemap->unbind();

	if (ret == 0)
		dprint("NSTexManager::loadCubemap Could not save " + cubemap->typeString() + " to file " + fName + " Error: " + nsstring(SOIL_last_result()));
	else
		dprint("NSTexManager::loadCubemap Successfully saved " + cubemap->typeString() + " to file " + fName);

	return (ret == 0);
}

bool NSTexManager::saveImage(NSTex2D * image, const nsstring & fname)
{
	if (image == NULL)
		return false;

	nsstring fName(fname + image->extension());

	image->bind();
	image->lock();
	image->unbind();

	if (image->data().data == NULL)
		return false;

	ILuint imageID = 0;
	ilEnable(IL_FILE_OVERWRITE);
	ilGenImages(1, &imageID);
	ilBindImage(imageID);
	ilTexImage(image->dim().w, image->dim().h, 1, image->channels(), image->format(), image->pixelDataType(), image->data().data);

	bool ret = ilSaveImage(fName.c_str()) && true;
	ilDeleteImages(1, &imageID);
	image->bind();
	image->unlock();
	image->unbind();

	ILenum err = ilGetError();
	while (err != IL_NO_ERROR)
	{
		const char * errStr = iluErrorString(err);
		dprint("NSTexManager::saveImage : Error saving " + image->typeString() + " to file " + fName + " Error: " + std::string(errStr));
		err = ilGetError();
	}

	if (ret)
		dprint("NSTexManager::saveImage Successfully saved " + image->typeString() + " to file " + fName);
	else
		dprint("NSTexManager::saveImage Could not save " + image->typeString() + " to file " + fName);

	return ret;
}

nsstring NSTexManager::getTypeString()
{
	return TEX_MANAGER_TYPESTRING;
}
