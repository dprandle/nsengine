/*! 
	\file nsmat_manager.h
	
	\brief Header file for NSMatManager class

	This file contains all of the neccessary declarations for the NSMatManager class.

	\author Daniel Randle
	\date November 23 2013
	\copywrite Earth Banana Games 2013
*/

#ifndef NSMATERIALMANAGER_H
#define NSMATERIALMANAGER_H

#include <nsres_manager.h>
#include <nsmaterial.h>

struct aiMaterial;

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
		return create<NSMaterial>(resName);
	}

	template <class ResType, class T>
	ResType * get(const T & rname)
	{
		return NSResManager::get<ResType>(rname);
	}
	
	template<class T>
	NSMaterial * get(const T & resname)
	{
		return get<NSMaterial>(resname);
	}

	template<class ResType>
	ResType * load(const nsstring & fname)
	{
		return NSResManager::load<ResType>(fname);
	}

	NSMaterial * load(const nsstring & fname)
	{
		return load<NSMaterial>(fname);
	}
	
	template<class ResType, class T >
	ResType * remove(const T & rname)
	{
		return NSResManager::remove<ResType>(rname);
	}

	template<class T >
	NSMaterial * remove(const T & rname)
	{
		return remove<NSMaterial>(rname);
	}

	NSMaterial* assimpLoadMaterial(const nsstring & pMaterialName, const aiMaterial * pAIMat, const nsstring & pTexDir="");

};

#endif
