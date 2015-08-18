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
mOwner(NULL),
mUpdate(true),
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
	return mOwner;
}

bool NSComponent::updatePosted() const
{
	return mUpdate;
}

void NSComponent::postUpdate(bool pUpdate)
{
	mUpdate = pUpdate;
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

void NSComponent::setOwner(nsentity * owner)
{
	mOwner = owner;
}
