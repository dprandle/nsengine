/*!
\file nseventhandler.h

\brief Header file for NSEventDispatcher class

This file contains all of the neccessary declarations for the NSEventDispatcher class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#ifndef NSEVENTDISPATCHER_H
#define NSEVENTDISPTACHER_H

#include <vector>
#include <nsglobal.h>
#include <nsevent.h>
#include <deque>
#include <typeindex>

class NSEventHandler;

class NSEventDispatcher
{
public:
	typedef std::deque<NSEvent*> EventQueue;
	typedef std::unordered_map<NSEventHandler*, EventQueue> ListenerQueue;

	typedef std::unordered_set<NSEventHandler *> ListenerSet;
	typedef std::unordered_map<std::type_index, ListenerSet> ListenerMap;

	NSEventDispatcher();

	~NSEventDispatcher();

	template<class EventType>
	void registerListener(NSEventHandler * handler)
	{
		std::type_index eventT(typeid(EventType));
		auto empIter = mListeners.emplace(eventT, ListenerSet());
		empIter.first->second.emplace(handler);
		mListenerEvents.emplace(handler, EventQueue());
	}

	void clear();

	void clear(NSEventHandler * handler);

	NSEvent * next(NSEventHandler * handler);

	void pop(NSEventHandler * handler);

	void pop_back(NSEventHandler * handler);

	void process(NSEventHandler * handler);
	
	template<class EventType, class ...Types>
	EventType * push(Types... fargs)
	{
		std::type_index eventT(typeid(EventType));
		auto listenerSetIter = mListeners.find(eventT);
		if (listenerSetIter == mListeners.end() || listenerSetIter->second.empty())
			return NULL;

		// Go through all of the registered listeners under this evnt ID and add this evnt to their queue
		EventType * evnt = new EventType(fargs...);
		ListenerSet::iterator currentListener = listenerSetIter->second.begin();
		while (currentListener != listenerSetIter->second.end())
		{
			mListenerEvents[*currentListener].push_back(evnt);
			++evnt->refcount;
			++currentListener;
		}
		return evnt;
	}

	template<class EventType, class... U>
	EventType * push_front(U&&... u)
	{
		std::type_index eventT(typeid(EventType));
		auto listenerSetIter = mListeners.find(eventT);
		if (listenerSetIter == mListeners.end() || listenerSetIter->second.empty())
			return NULL;

		// Go through all of the registered listeners under this evnt ID and add this evnt to their queue
		EventType * evnt = new EventType(std::forward<U>(u));
		ListenerSet::iterator currentListener = listenerSetIter->second.begin();
		while (currentListener != listenerSetIter->second.end())
		{
			mListenerEvents[*currentListener].push_front(evnt);
			++evnt->refcount;
			++currentListener;
		}
		return evnt;
	}

	template<class EventType>
	bool unregisterListener(NSEventHandler * handler)
	{
		std::type_index eventT(typeid(EventType));
		mListenerEvents.clear(handler); // Remove all events for this system
		auto fiter = mListeners.find(eventT);
		if (fiter != mListeners.end())
			return fiter->second.erase(handler) == 1;
		return false;
	}

	bool send(NSEvent * pEvent);

private:
	ListenerQueue mListenerEvents;
	ListenerMap mListeners;
};


#endif
