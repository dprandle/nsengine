/*! 
	\file nstex_manager.h
	
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
#include <nsres_manager.h>
#include <nstexture.h>

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

	virtual NSTex2D * create(const nsstring & resName)
	{
		return create<NSTex2D>(resName); // Create 2d texture by default
	}

	template <class ResType, class T>
	ResType * get(const T & rname)
	{
		return NSResManager::get<ResType>(rname);
	}
	
	template<class T>
	NSTexture * get(const T & resname)
	{
		return get<NSTexture>(resname);
	}

	template<class ResType>
	ResType * load(const nsstring & fname)
	{
		return NSResManager::load<ResType>(fname);
	}

	NSTexture * load(const nsstring & fname)
	{
		return load<NSTexture>(fname);
	}

	virtual NSTexture * load(uint32 res_type_id, const nsstring & fname);
	
	template<class ResType, class T >
	ResType * remove(const T & rname)
	{
		return NSResManager::remove<ResType>(rname);
	}

	template<class T >
	NSTexture * remove(const T & rname)
	{
		return remove<NSTexture>(rname);
	}

	template<class T>
	bool save(const T & res_name, nsstring path="")
	{
		NSResource * res = get(res_name);
		return save(res, path);
	}

	virtual bool save(NSResource * res, const nsstring & path);
	
	virtual NSTexCubeMap * loadCubemap(const nsstring & fname);

	virtual NSTexCubeMap * loadCubemap(const nsstring & pXPlus,
									   const nsstring & pXMinus,
									   const nsstring & pYPlus,
									   const nsstring & pYMinus,
									   const nsstring & pZPlus,
									   const nsstring & pZMinus,
									   const nsstring & fname);

	virtual NSTex2D * loadImage(const nsstring & fname);

	virtual bool save(NSTex2D * image, const nsstring & path);

	virtual bool save(NSTexCubeMap * cubemap, const nsstring & path);
};

#endif
