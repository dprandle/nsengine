/*!
\file nseventhandler.cpp

\brief Definition file for NSEventHandler class

This file contains all of the neccessary definitions for the NSEventHandler class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#include <nsevent_handler.h>

NSEventHandler::NSEventHandler()
{}

NSEventHandler::~NSEventHandler()
{
	auto iter = mHandlers.begin();
	while (iter != mHandlers.end())
	{
		delete iter->second;
		++iter;
	}
}
	
bool NSEventHandler::handleEvent(NSEvent * event)
{
    std::type_index ti(typeid(*event));
    auto fiter = mHandlers.find(ti);
	if (fiter != mHandlers.end())
		return fiter->second->exec(event);
	return false;
}
