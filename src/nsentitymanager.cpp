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

nsstring NSEntityManager::getTypeString()
{
	return ENTITY_MANAGER_TYPESTRING;
}