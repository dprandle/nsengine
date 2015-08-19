/*!
\file nseventhandler.cpp

\brief Definition file for nsevent_dispatcher class

This file contains all of the neccessary definitions for the nsevent_dispatcher class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/
#include <iostream>
#include <typeinfo>

#include <nsevent_dispatcher.h>
#include <nssel_comp.h>
#include <nssystem.h>
#include <nsentity.h>

nsevent_dispatcher::nsevent_dispatcher() : m_listener_events(), m_listeners()
{}

nsevent_dispatcher::~nsevent_dispatcher()
{
	clear();
}

void nsevent_dispatcher::clear()
{
	auto iter = m_listener_events.begin();
	while (iter != m_listener_events.end())
	{
		clear(iter->first);
		++iter;
	}
	m_listener_events.clear();
}

void nsevent_dispatcher::clear(nsevent_handler * handler)
{
	auto fiter = m_listener_events.find(handler);
	if (fiter != m_listener_events.end())
	{
		while (fiter->second.begin() != fiter->second.end())
		{
			--(fiter->second.front()->ref_count);
			if (fiter->second.front()->ref_count == 0)
				delete fiter->second.front();
			fiter->second.pop_front();
		}
	}
}


nsevent * nsevent_dispatcher::next(nsevent_handler * handler)
{
	auto fiter = m_listener_events.find(handler);
	if (fiter != m_listener_events.end() && !fiter->second.empty())
		return fiter->second.front();
	return NULL;
}

void nsevent_dispatcher::pop(nsevent_handler * handler)
{
	auto fiter = m_listener_events.find(handler);
	if (fiter != m_listener_events.end() && !fiter->second.empty())
	{
		--(fiter->second.front()->ref_count); // decrease refcount
		if (fiter->second.front()->ref_count == 0)
			delete fiter->second.front();
		fiter->second.pop_front();
	}
}

void nsevent_dispatcher::pop_back(nsevent_handler * handler)
{
	auto fiter = m_listener_events.find(handler);
	if (fiter != m_listener_events.end() && !fiter->second.empty())
	{
		--(fiter->second.back()->ref_count); // decrease refcount
		if (fiter->second.back()->ref_count == 0)
			delete fiter->second.back();
		fiter->second.pop_back();
	}
}

void nsevent_dispatcher::process(nsevent_handler * handler)
{
	auto events = m_listener_events.find(handler);
	if (events == m_listener_events.end())
		return;

	auto iter = events->second.begin();
	while (iter != events->second.end())
	{
        std::type_index ti(typeid(*(*iter)));
		if (events->first->handle_event(*iter))
		{
			--((*iter)->ref_count);
			if ((*iter)->ref_count == 0)
				delete (*iter);
			iter = events->second.erase(iter);
		}
		else
			++iter;
	}
}

bool nsevent_dispatcher::send(nsevent * pEvent)
{
	// do nothing right now... until observer stuff realized then remove function
	return true;
}
