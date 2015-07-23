/*!
\file nseventhandler.cpp

\brief Definition file for NSEventDispatcher class

This file contains all of the neccessary definitions for the NSEventDispatcher class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#include <nseventdispatcher.h>
#include <nsselcomp.h>
#include <nssystem.h>
#include <nsentity.h>

NSEventDispatcher::NSEventDispatcher() : mListenerEvents(), mListeners()
{}

NSEventDispatcher::~NSEventDispatcher()
{
	clear();
}

void NSEventDispatcher::clear()
{
	auto iter = mListenerEvents.begin();
	while (iter != mListenerEvents.end())
	{
		clear(iter->first);
		++iter;
	}
	mListenerEvents.clear();
}

void NSEventDispatcher::clear(NSEventHandler * handler)
{
	auto fiter = mListenerEvents.find(handler);
	if (fiter != mListenerEvents.end())
	{
		while (fiter->second.begin() != fiter->second.end())
		{
			--(fiter->second.front()->refcount);
			if (fiter->second.front()->refcount == 0)
				delete fiter->second.front();
			fiter->second.pop_front();
		}
	}
}


NSEvent * NSEventDispatcher::next(NSEventHandler * handler)
{
	auto fiter = mListenerEvents.find(handler);
	if (fiter != mListenerEvents.end() && !fiter->second.empty())
		return fiter->second.front();
	return NULL;
}

void NSEventDispatcher::pop(NSEventHandler * handler)
{
	auto fiter = mListenerEvents.find(handler);
	if (fiter != mListenerEvents.end() && !fiter->second.empty())
	{
		--(fiter->second.front()->refcount); // decrease refcount
		if (fiter->second.front()->refcount == 0)
			delete fiter->second.front();
		fiter->second.pop_front();
	}
}

void NSEventDispatcher::pop_back(NSEventHandler * handler)
{
	auto fiter = mListenerEvents.find(handler);
	if (fiter != mListenerEvents.end() && !fiter->second.empty())
	{
		--(fiter->second.back()->refcount); // decrease refcount
		if (fiter->second.back()->refcount == 0)
			delete fiter->second.back();
		fiter->second.pop_back();
	}
}

void NSEventDispatcher::process(NSEventHandler * handler)
{
	auto events = mListenerEvents.find(handler);
	if (events == mListenerEvents.end())
		return;

	auto iter = events->second.begin();
	while (iter != events->second.end())
	{
		if (events->first->handleEvent(*iter))
		{
			--((*iter)->refcount);
			if ((*iter)->refcount == 0)
				delete (*iter);
			iter = events->second.erase(iter);
		}
		else
			++iter;
	}
}

bool NSEventDispatcher::send(NSEvent * pEvent)
{
	// do nothing right now... until observer stuff realized then remove function
	return true;
}
