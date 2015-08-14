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

uint32 NSSystem::type()
{
	return mHashedType;
}

int32 NSSystem::drawPriority()
{
	return NO_DRAW_PR;
}

void NSSystem::addTriggerHash(uint32 key, const nsstring & trigname)
{
	mHashedInputTriggers.emplace(key,hash_id(trigname));
}

void NSSystem::removeTriggerHash(uint32 key)
{
	mHashedInputTriggers.erase(key);
}

uint32 NSSystem::triggerHash(uint32 key)
{
	auto fiter = mHashedInputTriggers.find(key);
	if (fiter != mHashedInputTriggers.end())
		return fiter->second;
	return 0;
}
