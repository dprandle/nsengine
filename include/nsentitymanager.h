/*! 
	\file nsentitymanager.h
	
	\brief Header file for NSEntityManager class

	This file contains all of the neccessary declarations for the NSEnTempManager class.

	\author Daniel Randle
	\date November 23 2013
	\copywrite Earth Banana Games 2013
*/

#ifndef NSENTITYMANAGER_H
#define NSENTITYMANAGER_H

#include <nsentity.h>
#include <nsresmanager.h>

class NSEntityManager : public NSResManager
{
public:

	NSEntityManager();
	~NSEntityManager();

	template <class ResType>
	ResType * create(const nsstring & resName)
	{
		return NSResManager::create<ResType>(resName);
	}

	virtual NSEntity * create(const nsstring & resName)
	{
		return NSResManager::create<NSEntity>(resName);
	}

	virtual NSEntity * create(const nsstring & resType, const nsstring & resName)
	{
		return static_cast<NSEntity*>(NSResManager::create(resType, resName));
	}

	template <class ResType, class T>
	ResType * get(const T & rname)
	{
		return NSResManager::get<ResType>(rname);
	}

	virtual NSEntity * get(nsuint resid)
	{
		return static_cast<NSEntity*>(NSResManager::get(resid));
	}

	virtual NSEntity * get(const nsstring & resName)
	{
		return static_cast<NSEntity*>(NSResManager::get(resName));
	}

	template<class ResType>
	ResType * load(const nsstring & pFileName, bool pAppendDirectories = true)
	{
		return NSResManager::load<ResType>(pFileName, pAppendDirectories);
	}

	virtual NSEntity * load(const nsstring & resType, const nsstring & pFileName, bool pAppendDirectories = true)
	{
		return static_cast<NSEntity*>(NSResManager::load(resType, pFileName, pAppendDirectories));
	}

	template<class ResType, class T >
	ResType * remove(const T & rname)
	{
		return NSResManager::remove<ResType>(rname);
	}

	virtual NSEntity * remove(const nsstring & name)
	{
		return static_cast<NSEntity*>(NSResManager::remove(name));
	}

	virtual NSEntity * remove(nsuint id)
	{
		return static_cast<NSEntity*>(NSResManager::remove(id));
	}

	virtual NSEntity * remove(NSResource * res)
	{
		return static_cast<NSEntity*>(NSResManager::remove(res));
	}

	template<class CompType>
	nspentityset entities()
	{
		nspentityset ret;
		auto iter = mIDResourceMap.begin();
		while (iter != mIDResourceMap.end())
		{
			NSEntity * curEnt = get<NSEntity>(iter->first);
			if (curEnt->has<CompType>())
				ret.emplace(curEnt);
			++iter;
		}
		return ret;
	}

	virtual nsstring typeString() { return getTypeString(); }

	static nsstring getTypeString();
};

#endif



