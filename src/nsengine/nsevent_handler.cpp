/*!
\file nseventhandler.cpp

\brief Definition file for NSEventHandler class

This file contains all of the neccessary definitions for the NSEventHandler class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#include <nsevent_handler.h>

nsevent_handler::nsevent_handler()
{}

nsevent_handler::~nsevent_handler()
{
	auto iter = m_handlers.begin();
	while (iter != m_handlers.end())
	{
		delete iter->second;
		++iter;
	}
}
	
bool nsevent_handler::handle_event(nsevent * event)
{
    std::type_index ti(typeid(*event));
    auto fiter = m_handlers.find(ti);
	if (fiter != m_handlers.end())
		return fiter->second->exec(event);
	return false;
}
