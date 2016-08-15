/*!
\file nssignal.cpp

\brief Definition file for nssignal class

This file contains all of the neccessary definitions for the nssignal class.

\author Daniel Randle
\date Feb 23 2016
\copywrite Earth Banana Games 2013
*/

#ifndef NSSIGNAL_H
#define NSSIGNAL_H

#include <nsvector.h>

class nsrouter;

namespace ns
{

struct slot_base
{
	virtual ~slot_base() {}
	nsrouter * router;
};

template<class... Args>
struct signal;

template<class... Args>
struct slot : public slot_base
{
	~slot();
	
    virtual void call(Args...) = 0;
	signal<Args...> * connected_signal;
};

template<class T, class... Args>
struct slot_concrete : public slot<Args...>
{
    typedef void (T::*mem_func_t)(Args...);

    slot_concrete(T * inst_, mem_func_t func_):
        inst(inst_),
        func(func_)
    {}
    
    void call(Args... args)
    {
		(inst->*func)(args...);
    }

    T * inst;
    mem_func_t func;
};

class nsrouter;

void assist_delete(slot_base * del);

template<class... Args>
struct signal
{
	~signal()
	{
		while (slots.begin() != slots.end())
			assist_delete(slots.back());
	}
	
    void operator()(Args... args)
    {
		auto iter = slots.begin();
		while (iter != slots.end())
		{
			(*iter)->call(args...);
			++iter;
		}
    }
    
    std::vector<slot<Args...>*> slots;
};

template<class... Args>
slot<Args...>::~slot()
{
	auto iter = connected_signal->slots.begin();
	while (iter != connected_signal->slots.end())
	{
		if (this == *iter)
			iter = connected_signal->slots.erase(iter);
		else
			++iter;
	}
}
}

#endif
