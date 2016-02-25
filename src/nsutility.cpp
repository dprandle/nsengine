/*! 
	\file nsutility.cpp
	
	\brief Header file for nsutility class

	This file contains all of the neccessary definitions for the nsutility namespace.

	\author Daniel Randle
	\date Feb 17 2016
	\copywrite Earth Banana Games 2013
*/

#include <nsutility.h>

namespace util
{

void split(const nsstring &s, char delim, nsstring_vector &elems)
{
    nsstringstream ss(s);
    nsstring item;
    while (std::getline(ss, item, delim))
	{
		if (!item.empty())
			elems.push_back(item);
    }
}

nsstring_vector split(const nsstring &s, char delim)
{
    nsstring_vector elems;
    split(s, delim, elems);
    return elems;
}

}
