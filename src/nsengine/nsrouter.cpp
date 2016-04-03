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
	while (slots.begin() != slots.end())
	{
		delete slots.back();
		slots.pop_back();
	}		
}

void nsrouter::remove_slot(ns::slot_base * slot)
{
	auto iter = slots.begin();
	while (iter != slots.end())
	{
		if (*iter == slot)
		{
			delete *iter;
			iter = slots.erase(iter);
		}
		else
			++iter;
	}	
}

void ns::assist_delete(ns::slot_base * del)
{
	del->router->remove_slot(del);
}
