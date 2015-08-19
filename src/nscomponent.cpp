/*! 
	\file nscomponent.cpp
	
	\brief Definition file for NSComponent class

	This file contains all of the neccessary definitions for the NSComponent class.

	\author Daniel Randle
	\date November 23 2013
	\copywrite Earth Banana Games 2013
*/

#include <nscomponent.h>

NSComponent::NSComponent(): 
m_owner(NULL),
m_update(true),
m_hashed_type(0)
{}

NSComponent::~NSComponent()
{}

NSComponent * NSComponent::copy(const NSComponent * pComp)
{
	if (pComp == NULL)
		return NULL;
	return this;
}

nsentity * NSComponent::owner()
{
	return m_owner;
}

bool NSComponent::update_posted() const
{
	return m_update;
}

void NSComponent::post_update(bool pUpdate)
{
	m_update = pUpdate;
}

uint32 NSComponent::type()
{
	return m_hashed_type;
}

void NSComponent::name_change(const uivec2 & oldid, const uivec2 newid)
{
	// do nothing
}

NSComponent & NSComponent::operator=(const NSComponent & pRHSComp)
{
	copy(&pRHSComp);
	return *this;
}

/*!
Get the resources that the component uses. If no resources are used then leave this unimplemented - will return an empty map.
*/
uivec2array NSComponent::resources()
{
	return uivec2array();
}

void NSComponent::set_owner(nsentity * owner)
{
	m_owner = owner;
}
