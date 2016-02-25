/*!
  \file nsrouter.h

  \brief Definition file for nsrouter class

  This file contains all of the neccessary definitions for the nsrouter class.

  \author Daniel Randle
  \date Feb 23 2016
  \copywrite Earth Banana Games 2013
*/

#ifndef NSROUTER_H
#define NSROUTER_H

#include <nssignal.h>

class nsrouter
{
  public:
	nsrouter();
    ~nsrouter();

	void disconnect_all();

	template <class T,class... Args>
	void connect(T * inst, void(T::*mf)(Args...), ns::signal<Args...> & sig)
	{
		ns::slot_concrete<T, Args...> * slot_ptr = new ns::slot<T, Args...>(inst, mf);
		slot_ptr->connected_signal = &sig;
		sig.slots.push_back(slot_ptr);
		slots.push_back(slot_ptr);
	}

	template <class... Args>
	void disconnect(ns::signal<Args...> & sig)
	{
		auto iter = slots.begin();
		while (iter != slots.end())
		{
			ns::slot<Args...> * cast_down = dynamic_cast<ns::slot<Args...>*>(*iter);
			if (cast_down != nullptr && cast_down->connected_signal == &sig)
			{
				delete *iter;
				iter = slots.erase(iter);
			}
			else
				++iter;
		}
	}

  private:
	
	std::vector<ns::slot_base*> slots;
};
#endif
