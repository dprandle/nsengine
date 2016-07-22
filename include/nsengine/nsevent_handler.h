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
#include <typeindex>
#include "nsevent.h"
#include "nsengine.h"
#include "nsevent_dispatcher.h"

//1) Make it so that events have one more parameter - uint32 id - use this for action events

//2) Remove state events all together

#define register_handler(func) register_handler_func(this, &func, 0)
#define unregister_handler(func) unregister_handler_func(this, &func, 0)
#define register_id_handler(func,id) register_handler_func(this, &func, id)
#define unregister_id_handler(func,id) unregister_handler_func(this, &func, id)
#define register_action_handler(func,str) register_handler_func(this, &func, hash_id(str))
#define unregister_action_handler(func,str) unregister_handler_func(this, &func, hash_id(str))

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
	
	typedef std::unordered_map<uint32, nshandler_func*> handler_func_map;
	typedef std::unordered_map<std::type_index, handler_func_map> handler_map;

	nsevent_handler();
	~nsevent_handler();
	
	bool handle_event(nsevent * event);

	template<class class_type,class event_type>
	bool register_handler_func(
		class_type * class_inst_,
		bool (class_type::*memberFunc)(event_type*),
		uint32 event_id)
	{
		std::type_index ti(typeid(event_type));
		nshandler_func * hf = new nshandler_func_type<class_type,event_type>(class_inst_,memberFunc);
		auto handler_iter = m_handlers.emplace(ti, handler_func_map());
		if (!handler_iter.first->second.emplace(event_id, hf).second)
		{
			delete hf;
			dprint("nsevent_handler::register_handler_func Cannot add handler function for event id " + std::to_string(event_id) + " as there is one already");
			return false;
		}
		nse.event_dispatch()->register_listener<event_type>(class_inst_);
		return true;
	}

	template<class class_type, class event_type>
	bool unregister_handler_func(
		class_type * class_inst_,
		bool (class_type::*memberFunc)(event_type*),
		uint32 event_id)
	{
		std::type_index ti(typeid(event_type));
		auto fiter = m_handlers.find(ti);
		if (fiter != m_handlers.end())
		{
			auto func_iter = fiter->second.find(event_id);
			if (func_iter != fiter->second.end())
			{
				delete func_iter->second;
				fiter->second.erase(func_iter);
				if (fiter->second.empty())
				{
					m_handlers.erase(fiter);
					return nse.event_dispatch()->unregister_listener<event_type>(class_inst_);
				}
				return true;
			}
		}
		return false;
	}

  private:
	handler_map m_handlers;
};
#endif
