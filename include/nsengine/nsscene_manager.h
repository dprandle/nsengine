/*!
\file nsscene_manager.h

\brief Header file for NSSceneManager class

This file contains all of the neccessary declarations for the NSSceneManager class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#ifndef NSSCENEMANAGER_H
#define NSSCENEMANAGER_H

#include <nsres_manager.h>
#include <nsscene.h>

class NSSceneManager : public NSResManager
{
public:
	NSSceneManager();
	~NSSceneManager();
	
	template <class ResType>
	ResType * create(const nsstring & resName)
	{
		return NSResManager::create<ResType>(resName);
	}

	virtual NSScene * create(const nsstring & resName)
	{
		return create<NSScene>(resName); // Create 2d texture by default
	}

	template <class ResType, class T>
	ResType * get(const T & rname)
	{
		return NSResManager::get<ResType>(rname);
	}
	
	template<class T>
	NSScene * get(const T & resname)
	{
		return get<NSScene>(resname);
	}

	template<class ResType>
	ResType * load(const nsstring & fname)
	{
		return NSResManager::load<ResType>(fname);
	}

	NSScene * load(const nsstring & fname)
	{
		return load<NSScene>(fname);
	}

	virtual NSScene * load(uint32 res_type_id, const nsstring & fname);
	
	template<class ResType, class T >
	ResType * remove(const T & rname)
	{
		return NSResManager::remove<ResType>(rname);
	}

	template<class T >
	NSScene * remove(const T & rname)
	{
		return remove<NSScene>(rname);
	}

	template<class T>
	bool save(const T & res_name, nsstring path="")
	{
		NSResource * res = get(res_name);
		return save(res, path);
	}

	virtual bool save(NSResource * res, const nsstring & path="");
	
	NSScene * current();
	
	template<class T>
	bool setCurrent(const T & scene, bool newScene = false, bool savePrevious = false)
	{
		NSScene * sc = get(scene);
		return setCurrent(sc, newScene, savePrevious);
	}
	
	bool setCurrent(NSScene * scene, bool newScene=false, bool savePrevious=false);

private:
	NSScene * mCurrentScene;
};

#endif
