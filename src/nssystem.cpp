/*!
\file system.cpp

\brief Definition file for nssystem class

This file contains all of the neccessary definitions for the nssystem class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#include <nssystem.h>

nssystem::nssystem():
	m_hashed_type(0)
{}

nssystem::~nssystem()
{}

void nssystem::draw()
{
	// do nothing
}

uint32 nssystem::type()
{
	return m_hashed_type;
}

int32 nssystem::draw_priority()
{
	return NO_DRAW_PR;
}

void nssystem::add_trigger_hash(uint32 key, const nsstring & trigname)
{
	m_hashed_input_triggers.emplace(key,hash_id(trigname));
}

void nssystem::remove_trigger_hash(uint32 key)
{
	m_hashed_input_triggers.erase(key);
}

uint32 nssystem::trigger_hash(uint32 key)
{
	auto fiter = m_hashed_input_triggers.find(key);
	if (fiter != m_hashed_input_triggers.end())
		return fiter->second;
	return 0;
}
