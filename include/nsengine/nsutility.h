/*! 
	\file nsutility.h
	
	\brief Header file for nsutility class

	This file contains all of the neccessary declarations for the nsutility namespace.

	\author Daniel Randle
	\date February 17 2016
	\copywrite Earth Banana Games 2013
*/

#ifndef NSUTILITY_H
#define NSUTILITY_H

#include <nsstring.h>
#include <nsvector.h>

namespace util
{
void split(const nsstring &s, char delim, nsstring_vector & elems);
nsstring_vector split(const nsstring &s, char delim);
}

#endif
