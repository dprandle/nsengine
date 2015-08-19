/*! 
	\file nscomponent.h
	
	\brief Header file for NSComponent class

	This file contains all of the neccessary declarations for the NSComponent class.

	\author Daniel Randle
	\date November 23 2013
	\copywrite Earth Banana Games 2013
*/

#ifndef NSCOMPONENT_H
#define NSCOMPONENT_H

#include <nsglobal.h>
#include <nsmath.h>
#include <nspupper.h>

class nsentity;
class nsevent;
class nstimer;

class NSComponent
{
public:

	friend class nscomp_factory;

	NSComponent();

	virtual ~NSComponent();

	virtual NSComponent * copy(const NSComponent * comp_);

	nsentity * owner();

	bool update_posted() const;

	virtual void init()=0;

	uint32 type();

	virtual void name_change(const uivec2 &, const uivec2);

	/*!
	Get the resources that the component uses. If no resources are used then leave this unimplemented - will return an empty map.
	/return Map of resource ID to resource type containing all used resources
	*/
	virtual uivec2array resources();

	virtual void pup(nsfile_pupper * p) = 0;

	virtual void post_update(bool update_);

	void set_owner(nsentity * owner_);

	NSComponent & operator=(const NSComponent & rhs_);

protected:
	nsentity * m_owner;
	uint32 m_hashed_type;
	bool m_update;
};

#endif
