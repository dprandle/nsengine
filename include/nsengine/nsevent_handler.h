/*!
\file nsevent_handler.h

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
#include <typeindex>
#include <nsengine.h>
#include <nsevent_dispatcher.h>

class nshandler_func
{
  public:
	virtual ~nshandler_func() {}
	virtual bool exec(nsevent * evnt)=0;
};

template<class class_type, class event_type>
class nshandler_func_type : public nshandler_func
{
  public:
	typedef bool (class_type::*member_func_t)(event_type *);

	nshandler_func_type(class_type * class_inst_, member_func_t member_func_):
		m_cls_instance(class_inst_),
		m_member_func(member_func_)
	{}

	bool exec(nsevent * evnt)
	{
		event_type * cast_evnt = static_cast<event_type*>(evnt);
        return (m_cls_instance->*m_member_func)(cast_evnt);
	}

  private:
	
	class_type * m_cls_instance;
	member_func_t m_member_func;
};

class nsevent_handler
{
  public:
	
	typedef std::unordered_map<std::type_index, nshandler_func*> handler_map;
	
	nsevent_handler();
	~nsevent_handler();
	
	bool handle_event(nsevent * event);

	template<class class_type,class event_type>
	bool register_handler_func(class_type * class_inst_, bool (class_type::*memberFunc)(event_type*))
	{
		std::type_index ti(typeid(event_type));
		nshandler_func * hf = new nshandler_func_type<class_type,event_type>(class_inst_,memberFunc);
		if (!m_handlers.emplace(ti, hf).second)
		{
			delete hf;
			return false;
		}
		nsengine.event_dispatch()->register_listener<event_type>(class_inst_);
		return true;
	}

	template<class event_type>
	bool unregister_handler_func()
	{
		std::type_index ti(typeid(event_type));
		auto fiter = m_handlers.find(ti);
		if (fiter != m_handlers.end())
		{
			delete fiter->second;
			m_handlers.erase(fiter);
			return true;
		}
		return false;
	}

  private:
	handler_map m_handlers;
};
#endif
