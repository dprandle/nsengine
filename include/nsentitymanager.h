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

#include <nsresmanager.h>
#include <nsentity.h>

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
		return create<NSEntity>(resName); // Create 2d texture by default
	}

	template <class ResType, class T>
	ResType * get(const T & rname)
	{
		return NSResManager::get<ResType>(rname);
	}
	
	template<class T>
	NSEntity * get(const T & resname)
	{
		return get<NSEntity>(resname);
	}

	template<class ResType>
	ResType * load(const nsstring & fname)
	{
		return NSResManager::load<ResType>(fname);
	}

	NSEntity * load(const nsstring & fname)
	{
		return load<NSEntity>(fname);
	}
	
	template<class ResType, class T >
	ResType * remove(const T & rname)
	{
		return NSResManager::remove<ResType>(rname);
	}

	template<class T >
	NSEntity * remove(const T & rname)
	{
		return remove<NSEntity>(rname);
	}

	template<class CompType>
	nspentityset entities()
	{
		nsuint hashed_type = nsengine.typeID(std::type_index(typeid(CompType)));
		return entities(hashed_type);
	}

	nspentityset entities(nsuint comp_type_id);

	nspentityset entities(const nsstring & comp_guid);
	
};

#endif



