/*! 
	\file nsanimmanager.h
	
	\brief Header file for NSAnimManager class

	This file contains all of the neccessary declarations for the NSAnimManager class.

	\author Daniel Randle
	\date December 11 2013
	\copywrite Earth Banana Games 2013
*/

#ifndef NSANIMMANAGER_H
#define NSANIMMANAGER_H

#include <nsanimset.h>
#include <nsresmanager.h>

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
		return NSResManager::create<NSAnimSet>(resName);
	}

	virtual NSAnimSet * create(const nsstring & resType, const nsstring & resName)
	{
		return static_cast<NSAnimSet*>(NSResManager::create(resType, resName));
	}

	template <class ResType, class T>
	ResType * get(const T & rname)
	{
		return NSResManager::get<ResType>(rname);
	}

	virtual NSAnimSet * get(nsuint resid)
	{
		return static_cast<NSAnimSet*>(NSResManager::get(resid));
	}

	virtual NSAnimSet * get(const nsstring & resName)
	{
		return static_cast<NSAnimSet*>(NSResManager::get(resName));
	}

	template<class ResType>
	ResType * load(const nsstring & pFileName, bool pAppendDirectories = true)
	{
		return NSResManager::load<ResType>(pFileName, pAppendDirectories);
	}

	virtual NSAnimSet * load(const nsstring & resType, const nsstring & pFileName, bool pAppendDirectories = true)
	{
		return static_cast<NSAnimSet*>(NSResManager::load(resType, pFileName, pAppendDirectories));
	}

	template<class ResType, class T >
	ResType * remove(const T & rname)
	{
		return NSResManager::remove<ResType>(rname);
	}

	virtual NSAnimSet * remove(const nsstring & name)
	{
		return static_cast<NSAnimSet*>(NSResManager::remove(name));
	}

	virtual NSAnimSet * remove(nsuint id)
	{
		return static_cast<NSAnimSet*>(NSResManager::remove(id));
	}

	virtual NSAnimSet * remove(NSResource * res)
	{
		return static_cast<NSAnimSet*>(NSResManager::remove(res));
	}

	NSAnimSet * assimpLoadAnimationSet(const aiScene * pScene, const nsstring & pSceneName);

	virtual nsstring typeString() { return getTypeString(); }

	static nsstring getTypeString();
};

#endif