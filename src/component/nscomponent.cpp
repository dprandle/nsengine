/*! 
	\file nscomponent.cpp
	
	\brief Definition file for nscomponent class

	This file contains all of the neccessary definitions for the nscomponent class.

	\author Daniel Randle
	\date November 23 2013
	\copywrite Earth Banana Games 2013
*/

#include <component/nscomponent.h>

nscomponent::nscomponent(uint32 hashed_type): 
	m_owner(NULL),
	m_update(true),
	m_hashed_type(hashed_type)
{}

nscomponent::nscomponent(const nscomponent & copy):
	m_owner(NULL),
	m_update(true),
	m_hashed_type(copy.m_hashed_type)
{}

nscomponent::~nscomponent()
{}

bool nscomponent::copyable()
{
	return true;
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
	if (m_update)
		emit_sig comp_edit(this);
}

uint32 nscomponent::type()
{
	return m_hashed_type;
}

void nscomponent::name_change(const uivec2 & oldid, const uivec2 newid)
{
	// do nothing
}

nscomponent & nscomponent::operator=(nscomponent_inst rhs)
{
	std::swap(m_hashed_type, rhs.m_hashed_type);
	m_update = true;
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
