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

class NSEntity;
class NSEvent;
class NSTimer;

class NSComponent
{
public:

	friend class NSCompFactory;

	NSComponent();

	virtual ~NSComponent();

	virtual NSComponent * copy(const NSComponent * pComp);

	NSEntity * owner();

	bool updatePosted() const;

	virtual void init()=0;

	uint32 type();

	virtual void nameChange(const uivec2 &, const uivec2);

	/*!
	Get the resources that the component uses. If no resources are used then leave this unimplemented - will return an empty map.
	/return Map of resource ID to resource type containing all used resources
	*/
	virtual uivec2array resources();

	virtual void pup(NSFilePUPer * p) = 0;

	virtual void postUpdate(bool pUpdate);

	void setOwner(NSEntity * owner);

	NSComponent & operator=(const NSComponent & pRHSComp);

protected:
	NSEntity * mOwner;
	uint32 mHashedType;
	bool mUpdate;
};

#endif
