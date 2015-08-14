/*!
\file system.cpp

\brief Definition file for NSSystem class

This file contains all of the neccessary definitions for the NSSystem class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#include <nssystem.h>

NSSystem::NSSystem():
	mHashedType(0)
{}

NSSystem::~NSSystem()
{}

void NSSystem::draw()
{
	// do nothing
}

nsuint NSSystem::type()
{
	return mHashedType;
}

nsint NSSystem::drawPriority()
{
	return NO_DRAW_PR;
}

void NSSystem::addTriggerHash(nsuint key, const nsstring & trigname)
{
	mHashedInputTriggers.emplace(key,hash_id(trigname));
}

void NSSystem::removeTriggerHash(nsuint key)
{
	mHashedInputTriggers.erase(key);
}

nsuint NSSystem::triggerHash(nsuint key)
{
	auto fiter = mHashedInputTriggers.find(key);
	if (fiter != mHashedInputTriggers.end())
		return fiter->second;
	return 0;
}
