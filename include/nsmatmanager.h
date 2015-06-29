/*! 
	\file nsmatmanager.h
	
	\brief Header file for NSMatManager class

	This file contains all of the neccessary declarations for the NSMatManager class.

	\author Daniel Randle
	\date November 23 2013
	\copywrite Earth Banana Games 2013
*/

#ifndef NSMATERIALMANAGER_H
#define NSMATERIALMANAGER_H

#include <nsmaterial.h>
#include <nsresmanager.h>

struct aiMaterial;
class NSTexManager;

class NSMatManager : public NSResManager
{
public:
	NSMatManager();
	~NSMatManager();

	template <class ResType>
	ResType * create(const nsstring & resName)
	{
		return NSResManager::create<ResType>(resName);
	}

	virtual NSMaterial * create(const nsstring & resName)
	{
		return NSResManager::create<NSMaterial>(resName);
	}

	virtual NSMaterial * create(const nsstring & resType, const nsstring & resName)
	{
		return static_cast<NSMaterial*>(NSResManager::create(resType, resName));
	}

	template <class ResType, class T>
	ResType * get(const T & rname)
	{
		return NSResManager::get<ResType>(rname);
	}

	virtual NSMaterial * get(nsuint resid)
	{
		return static_cast<NSMaterial*>(NSResManager::get(resid));
	}

	virtual NSMaterial * get(const nsstring & resName)
	{
		return static_cast<NSMaterial*>(NSResManager::get(resName));
	}

	template<class ResType>
	ResType * load(const nsstring & pFileName, bool pAppendDirectories = true)
	{
		return NSResManager::load<ResType>(pFileName, pAppendDirectories);
	}

	virtual NSMaterial * load(const nsstring & resType, const nsstring & pFileName, bool pAppendDirectories = true)
	{
		return static_cast<NSMaterial*>(NSResManager::load(resType, pFileName, pAppendDirectories));
	}

	template<class ResType, class T >
	ResType * remove(const T & rname)
	{
		return NSResManager::remove<ResType>(rname);
	}

	virtual NSMaterial * remove(const nsstring & name)
	{
		return static_cast<NSMaterial*>(NSResManager::remove(name));
	}

	virtual NSMaterial * remove(nsuint id)
	{
		return static_cast<NSMaterial*>(NSResManager::remove(id));
	}

	virtual NSMaterial * remove(NSResource * res)
	{
		return static_cast<NSMaterial*>(NSResManager::remove(res));
	}

	NSMaterial* assimpLoadMaterial(const nsstring & pMaterialName, const aiMaterial * pAIMat, const nsstring & pTexDir="");

	virtual nsstring typeString() { return getTypeString(); }

	static nsstring getTypeString();
};

#endif