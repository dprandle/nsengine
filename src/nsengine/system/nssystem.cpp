/*!
\file system.cpp

\brief Definition file for nssystem class

This file contains all of the neccessary definitions for the nssystem class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#include <nssystem.h>

nssystem::nssystem():
	m_hashed_type(0)
{}

nssystem::~nssystem()
{}

uint32 nssystem::type()
{
	return m_hashed_type;
}
