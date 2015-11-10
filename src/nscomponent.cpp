/*! 
	\file nscomponent.cpp
	
	\brief Definition file for nscomponent class

	This file contains all of the neccessary definitions for the nscomponent class.

	\author Daniel Randle
	\date November 23 2013
	\copywrite Earth Banana Games 2013
*/

#include <nscomponent.h>

nscomponent::nscomponent(): 
m_owner(NULL),
m_update(true),
m_hashed_type(0)
{}

nscomponent::~nscomponent()
{}

nscomponent * nscomponent::copy(const nscomponent * pComp)
{
	if (pComp == NULL)
		return NULL;
	return this;
}

nsentity * nscomponent::owner()
{
	return m_owner;
}

bool nscomponent::update_posted() const
{
	return m_update;
}

void nscomponent::post_update(bool pUpdate)
{
	m_update = pUpdate;
}

uint32 nscomponent::type()
{
	return m_hashed_type;
}

void nscomponent::name_change(const uivec2 & oldid, const uivec2 newid)
{
	// do nothing
}

nscomponent & nscomponent::operator=(const nscomponent & pRHSComp)
{
	copy(&pRHSComp);
	return *this;
}

/*!
Get the resources that the component uses. If no resources are used then leave this unimplemented - will return an empty map.
*/
uivec3_vector nscomponent::resources()
{
	return uivec3_vector();
}

void nscomponent::set_owner(nsentity * owner)
{
	m_owner = owner;
}
