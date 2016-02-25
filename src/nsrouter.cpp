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

