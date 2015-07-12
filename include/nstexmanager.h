/*! 
	\file nstexmanager.h
	
	\brief Header file for NSTexManager class

	This file contains all of the neccessary declarations for the NSTexManager class.

	\author Daniel Randle
	\date November 23 2013
	\copywrite Earth Banana Games 2013
*/


//Internal Format : Bytes per pixel(ie 3 or 4)
//Format : enum specifying order of those byes - ie RGBA or RBGA etc
//Pixel Data Type : the data type assigned to each component of the pixe(ie unsigned byte)


#ifndef NSTEXTUREMANAGER_H
#define NSTEXTUREMANAGER_H

#include <nsglobal.h>
#include <nstexture.h>
#include <nsresmanager.h>
#include <IL/il.h>
#include <soil/SOIL.h>
#include <IL/ilu.h>

class NSTexManager : public NSResManager
{
public:

	NSTexManager();
	~NSTexManager();

	template <class ResType>
	ResType * create(const nsstring & resName)
	{
		return NSResManager::create<ResType>(resName);
	}

	virtual NSTexture * create(const nsstring & resType, const nsstring & resName)
	{
		return static_cast<NSTexture*>(NSResManager::create(resType, resName));
	}

	template <class ResType, class T>
	ResType * get(const T & rname)
	{
		return NSResManager::get<ResType>(rname);
	}

	virtual NSTexture * get(nsuint resid)
	{
		return static_cast<NSTexture*>(NSResManager::get(resid));
	}

	virtual NSTexture * get(const nsstring & resName)
	{
		return static_cast<NSTexture*>(NSResManager::get(resName));
	}

	template<class ResType>
	ResType * load(const nsstring & pFileName, bool pAppendDirectories)
	{
		return NSResManager::load<ResType>(pFileName, pAppendDirectories);
	}

	virtual NSTexture * load(const nsstring & resType, const nsstring & pFileName, bool pAppendDirectories);

	virtual NSTexCubeMap * loadCubemap(const nsstring & pFileName, bool pAppendDirectories);

	virtual NSTexCubeMap * loadCubemap(const nsstring & pXPlus,
		const nsstring & pXMinus,
		const nsstring & pYPlus,
		const nsstring & pYMinus,
		const nsstring & pZPlus,
		const nsstring & pZMinus,
		const nsstring & pResourceName,
		bool pAppendDirectories);

	virtual NSTex2D * loadImage(const nsstring & filename, bool pAppendDirectories);

	template<class ResType, class T >
	ResType * remove(const T & rname)
	{
		return NSResManager::remove<ResType>(rname);
	}

	virtual NSTexture * remove(const nsstring & name)
	{
		return static_cast<NSTexture*>(NSResManager::remove(name));
	}

	virtual NSTexture * remove(nsuint id)
	{
		return static_cast<NSTexture*>(NSResManager::remove(id));
	}

	virtual NSTexture * remove(NSResource * res)
	{
		return static_cast<NSTexture*>(NSResManager::remove(res));
	}

	virtual bool save(const nsstring & resName, bool pAppendDirectories);

	virtual bool saveImage(NSTex2D * image, const nsstring & fname);

	virtual bool saveCubemap(NSTexCubeMap * cubemap, const nsstring & fname);

	static nsstring getTypeString();

	virtual nsstring typeString() { return getTypeString(); }
};

#endif
