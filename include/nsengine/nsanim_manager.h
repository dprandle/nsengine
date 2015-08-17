/*! 
	\file nsanim_manager.h
	
	\brief Header file for NSAnimManager class

	This file contains all of the neccessary declarations for the NSAnimManager class.

	\author Daniel Randle
	\date December 11 2013
	\copywrite Earth Banana Games 2013
*/

#ifndef NSANIMMANAGER_H
#define NSANIMMANAGER_H

#include <nsres_manager.h>
#include <nsanim_set.h>

struct aiScene;

class NSAnimManager : public NSResManager
{
public:
	NSAnimManager();
	~NSAnimManager();

	template <class ResType>
	ResType * create(const nsstring & resName)
	{
		return NSResManager::create<ResType>(resName);
	}

	virtual NSAnimSet * create(const nsstring & resName)
	{
		return create<NSAnimSet>(resName); // Create 2d texture by default
	}

	template <class ResType, class T>
	ResType * get(const T & rname)
	{
		return NSResManager::get<ResType>(rname);
	}
	
	template<class T>
	NSAnimSet * get(const T & resname)
	{
		return get<NSAnimSet>(resname);
	}

	template<class ResType>
	ResType * load(const nsstring & fname)
	{
		return NSResManager::load<ResType>(fname);
	}

	NSAnimSet * load(const nsstring & fname)
	{
		return load<NSAnimSet>(fname);
	}
	
	template<class ResType, class T >
	ResType * remove(const T & rname)
	{
		return NSResManager::remove<ResType>(rname);
	}

	template<class T >
	NSAnimSet * remove(const T & rname)
	{
		return remove<NSAnimSet>(rname);
	}

	NSAnimSet * assimpLoadAnimationSet(const aiScene * pScene, const nsstring & pSceneName);
};

#endif
