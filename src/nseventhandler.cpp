/*!
\file nseventhandler.cpp

\brief Definition file for NSEventHandler class

This file contains all of the neccessary definitions for the NSEventHandler class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#include <nseventhandler.h>
#include <nsselcomp.h>
#include <nssystem.h>
#include <nsentity.h>

NSEventHandler::NSEventHandler() : mListenerEvents(), mListeners()
{}

NSEventHandler::~NSEventHandler()
{
	clear();
}

void NSEventHandler::addListener(NSSystem * pSys, NSEvent::ID pEventID)
{
	// Should insert no matter what
	mListeners[pEventID].emplace(pSys);
}

void NSEventHandler::clear()
{
	auto iter = mListenerEvents.begin();
	while (iter != mListenerEvents.end())
	{
		while (iter->second.begin() != iter->second.end())
		{
			delete iter->second.back();
			iter->second.pop_back();
		}
		++iter;
	}
	mListenerEvents.clear();
}

bool NSEventHandler::process(NSSystem * pSystem)
{
	auto events = mListenerEvents.find(pSystem);
	if (events == mListenerEvents.end())
		return false;

	while (events->second.begin() != events->second.end())
	{
		pSystem->handleEvent(events->second.back());
		delete events->second.back();
		events->second.pop_back();
	}
	return true;
}

bool NSEventHandler::removeListener(NSSystem * pSys, NSEvent::ID pEventID)
{
	auto listenerSetIter = mListeners.find(pEventID);
	if (listenerSetIter == mListeners.end())
		return false;

	nsuint erased = listenerSetIter->second.erase(pSys);
	return erased && true;
}

bool NSEventHandler::send(NSEvent * pEvent)
{
	auto listenerSetIter = mListeners.find(pEvent->mID);
	if (listenerSetIter == mListeners.end())
		return false;

	auto currentListener = listenerSetIter->second.begin();
	while (currentListener != listenerSetIter->second.end())
	{
		(*currentListener)->handleEvent(pEvent);
		++currentListener;
	}
	delete pEvent;
	return true;
}