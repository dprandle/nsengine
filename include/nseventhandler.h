/*!
\file nseventhandler.h

\brief Header file for NSEventHandler class

This file contains all of the neccessary declarations for the NSEventHandler class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#ifndef NSEVENTSYSTEM_H
#define NSEVENTSYSTEM_H

#include <vector>
#include <nsglobal.h>
#include <nsevent.h>

class NSSystem;

class NSEventHandler
{
public:
	typedef std::vector<NSEvent*> EventQueue;
	typedef std::map<NSSystem*, EventQueue> ListenerQueue;

	typedef std::set<NSSystem *> ListenerSet;
	typedef std::map<NSEvent::ID, ListenerSet> ListenerMap;

	NSEventHandler();

	~NSEventHandler();

	void addListener(NSSystem * pSys, NSEvent::ID pEventID);

	void clear();

	bool process(NSSystem * pSystem);

	/*!
	\return bool indicating whether event was pushed on to a listener stack
	*/
	template<class EventType>
	bool push(EventType * pEvent)
	{
		auto listenerSetIter = mListeners.find(pEvent->mID);
		if (listenerSetIter == mListeners.end())
			return false;

		// Go through all of the registered listeners under this event ID and add this event to their queue
		ListenerSet::iterator currentListener = listenerSetIter->second.begin();
		while (currentListener != listenerSetIter->second.end())
		{
			EventType * ev = new EventType(*pEvent);
			mListenerEvents[*currentListener].push_back(ev);
			++currentListener;
		}
		delete pEvent;
		return true;
	}

	bool removeListener(NSSystem * pSys, NSEvent::ID pEventID);

	bool send(NSEvent * pEvent);

private:
	ListenerQueue mListenerEvents;
	ListenerMap mListeners;
};


#endif