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
	m_hashed_type(0)
{}

NSSystem::~NSSystem()
{}

void NSSystem::draw()
{
	// do nothing
}

uint32 NSSystem::type()
{
	return m_hashed_type;
}

int32 NSSystem::draw_priority()
{
	return NO_DRAW_PR;
}

void NSSystem::add_trigger_hash(uint32 key, const nsstring & trigname)
{
	m_hashed_input_triggers.emplace(key,hash_id(trigname));
}

void NSSystem::remove_trigger_hash(uint32 key)
{
	m_hashed_input_triggers.erase(key);
}

uint32 NSSystem::trigger_hash(uint32 key)
{
	auto fiter = m_hashed_input_triggers.find(key);
	if (fiter != m_hashed_input_triggers.end())
		return fiter->second;
	return 0;
}
