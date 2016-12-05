/*!
  \file nsrouter.cpp

  \brief Definition file for nsrouter class

  This file contains all of the neccessary definitions for the nsrouter class.

  \author Daniel Randle
  \date Feb 23 2016
  \copywrite Earth Banana Games 2013
*/

#include <nsrouter.h>

nsrouter::nsrouter()
{}

nsrouter::~nsrouter()
{
	disconnect_all();
}

void nsrouter::disconnect_all()
{
	while (con_slots.begin() != con_slots.end())
	{
		delete con_slots.back();
		con_slots.pop_back();
	}		
}

void nsrouter::remove_slot(ns::slot_base * slot)
{
	auto iter = con_slots.begin();
	while (iter != con_slots.end())
	{
		if (*iter == slot)
		{
			delete *iter;
			iter = con_slots.erase(iter);
		}
		else
			++iter;
	}	
}

void ns::assist_delete(ns::slot_base * del)
{
	del->router->remove_slot(del);
}
