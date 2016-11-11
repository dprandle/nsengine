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

#define SLOT_OBJECT nsrouter m_router;
#define sig_connect(signal, slot) m_router.connect(this, &slot, signal)
#define sig_disconnect(signal) m_router.disconnect(signal)


class nsrouter
{
  public:
	nsrouter();
    ~nsrouter();

	void disconnect_all();

	template <class T,class... Args>
	void connect(T * inst, void(T::*mf)(Args...), ns::signal<Args...> & sig)
	{
		ns::slot_concrete<T, Args...> * slot_ptr = new ns::slot_concrete<T, Args...>(inst, mf);
		slot_ptr->connected_signal = &sig;
		slot_ptr->router = this;
                sig.con_slots.push_back(slot_ptr);
                con_slots.push_back(slot_ptr);
	}

	template <class... Args>
	void disconnect(ns::signal<Args...> & sig)
	{
                auto iter = con_slots.begin();
                while (iter != con_slots.end())
		{
			ns::slot<Args...> * cast_down = dynamic_cast<ns::slot<Args...>*>(*iter);
			if (cast_down != nullptr && cast_down->connected_signal == &sig)
			{
				delete *iter;
                                iter = con_slots.erase(iter);
			}
			else
				++iter;
		}
	}

	void remove_slot(ns::slot_base * slot);

  private:
	
        std::vector<ns::slot_base*> con_slots;
};

#endif
