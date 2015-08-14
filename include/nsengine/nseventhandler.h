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
#include <typeindex>
#include <nsengine.h>
#include <nseventdispatcher.h>

class NSHandlerFunc
{
  public:
	virtual ~NSHandlerFunc() {}
	virtual bool exec(NSEvent * evnt)=0;
};

template<class ClassType, class EventType>
class NSHandlerFuncType : public NSHandlerFunc
{
  public:
	typedef bool (ClassType::*MemberFunc)(EventType *);

	NSHandlerFuncType(ClassType * inst, MemberFunc mf):
		class_instance(inst),
		member(mf)
	{}

	bool exec(NSEvent * evnt)
	{
		EventType * cast_evnt = static_cast<EventType*>(evnt);
        return (class_instance->*member)(cast_evnt);
	}

  private:
	
	ClassType * class_instance;
	MemberFunc member;
};

class NSEventHandler
{
  public:
	
	typedef std::unordered_map<std::type_index, NSHandlerFunc*> HandlerMap;
	
	NSEventHandler();
	~NSEventHandler();
	
	bool handleEvent(NSEvent * event);

	template<class ClassType,class EventType>
	bool registerHandlerFunc(ClassType * inst, bool (ClassType::*memberFunc)(EventType*))
	{
		std::type_index ti(typeid(EventType));
		NSHandlerFunc * hf = new NSHandlerFuncType<ClassType,EventType>(inst,memberFunc);
		if (!mHandlers.emplace(ti, hf).second)
		{
			delete hf;
			return false;
		}
		nsengine.eventDispatch()->registerListener<EventType>(inst);
		return true;
	}

	template<class EventType>
	bool unregisterHandlerFunc()
	{
		std::type_index ti(typeid(EventType));
		auto fiter = mHandlers.find(ti);
		if (fiter != mHandlers.end())
		{
			delete fiter->second;
			mHandlers.erase(fiter);
			return true;
		}
		return false;
	}

  private:
	HandlerMap mHandlers;
};
#endif
