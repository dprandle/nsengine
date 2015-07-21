/*! 
	\file nsentitymanager.cpp
	
	\brief Definition file for NSEntityManager class

	This file contains all of the neccessary definitions for the NSEntityManager class.

	\author Daniel Randle
	\date November 23 2013
	\copywrite Earth Banana Games 2013
*/


#include <nsentitymanager.h>
#include <nsengine.h>
#include <nsscenemanager.h>
#include <nsscene.h>
#include <nsengine.h>

NSEntityManager::NSEntityManager() : NSResManager()
{
	setLocalDirectory(LOCAL_ENTITY_DIR_DEFAULT);
	setSaveMode(NSResManager::Text);
}

NSEntityManager::~NSEntityManager()
{}

nspentityset NSEntityManager::entities(nsuint comp_type_id)
{
	nspentityset ret;
	auto iter = mIDResourceMap.begin();
	while (iter != mIDResourceMap.end())
	{
		NSEntity * curEnt = get(iter->first);
		if (curEnt->has(comp_type_id))
			ret.emplace(curEnt);
		++iter;
	}
	return ret;	
}

nspentityset NSEntityManager::entities(const nsstring & comp_guid)
{
	return entities(hash_id(comp_guid));
}
