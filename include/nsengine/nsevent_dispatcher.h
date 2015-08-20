/*!
\file nsevent_handler.h

\brief Header file for nsevent_dispatcher class

This file contains all of the neccessary declarations for the nsevent_dispatcher class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#ifndef NSEVENTDISPATCHER_H
#define NSEVENTDISPATCHER_H

#include <vector>
#include <nsevent.h>
#include <deque>
#include <typeindex>

class nsevent_handler;

class nsevent_dispatcher
{
public:
	typedef std::deque<nsevent*> event_queue;
	typedef std::unordered_map<nsevent_handler*, event_queue> listener_queue;

	typedef std::unordered_set<nsevent_handler *> listener_set;
    typedef std::unordered_map<std::type_index, listener_set> listener_map;

	nsevent_dispatcher();

	~nsevent_dispatcher();

	template<class event_type>
	void register_listener(nsevent_handler * handler_)
	{
		std::type_index eventT(typeid(event_type));
		auto empIter = m_listeners.emplace(eventT, listener_set());
		empIter.first->second.emplace(handler_);
		m_listener_events.emplace(handler_, event_queue());
	}

	void clear();

	void clear(nsevent_handler * handler_);

	nsevent * next(nsevent_handler * handler_);

	void pop(nsevent_handler * handler_);

	void pop_back(nsevent_handler * handler_);

	void process(nsevent_handler * handler_);
	
	template<class event_type, class ...arg_types>
	event_type * push(arg_types... fargs)
	{
		std::type_index eventT(typeid(event_type));
		auto listenerSetIter = m_listeners.find(eventT);
		if (listenerSetIter == m_listeners.end() || listenerSetIter->second.empty())
			return NULL;

		// Go through all of the registered listeners under this evnt ID and add this evnt to their queue
		event_type * evnt = new event_type(fargs...);
		listener_set::iterator currentListener = listenerSetIter->second.begin();
		while (currentListener != listenerSetIter->second.end())
		{
			m_listener_events[*currentListener].push_back(evnt);
			++evnt->ref_count;
			++currentListener;
		}
        nsevent * ev = evnt;
        std::type_index eventT1(typeid(*evnt));
        std::type_index eventT2(typeid(*ev));
		return evnt;
	}

    template<class event_type, class ...arg_types>
    event_type * push_front(arg_types... fargs)
	{
		std::type_index eventT(typeid(event_type));
		auto listenerSetIter = m_listeners.find(eventT);
		if (listenerSetIter == m_listeners.end() || listenerSetIter->second.empty())
			return NULL;

		// Go through all of the registered listeners under this evnt ID and add this evnt to their queue
		event_type * evnt = new event_type(fargs...);
		listener_set::iterator currentListener = listenerSetIter->second.begin();
		while (currentListener != listenerSetIter->second.end())
		{
			m_listener_events[*currentListener].push_front(evnt);
			++evnt->ref_count;
			++currentListener;
		}
		return evnt;
	}

	template<class event_type>
	bool unregister_listener(nsevent_handler * handler_)
	{
		std::type_index eventT(typeid(event_type));
		this->clear(handler_); // Remove all events for this system
		auto fiter = m_listeners.find(eventT);
		if (fiter != m_listeners.end())
			return fiter->second.erase(handler_) == 1;
		return false;
	}

	bool send(nsevent * pEvent);

private:
	listener_queue m_listener_events;
	listener_map m_listeners;
};


#endif
