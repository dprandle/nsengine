/*!
\file nseventhandler.cpp

\brief Definition file for NSEventHandler class

This file contains all of the neccessary definitions for the NSEventHandler class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#include <nseventhandler.h>

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
	auto fiter = mHandlers.find(std::type_index(typeid(event)));
	if (fiter != mHandlers.end())
		return fiter->second->exec(event);
	return false;
}
