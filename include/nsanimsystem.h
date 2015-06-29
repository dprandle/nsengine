/*!
\file nsanimsystem.h

\brief Header file for NSAnimSystem class

This file contains all of the neccessary declarations for the NSAnimSystem class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#ifndef NSANIMSYSTEM_H
#define NSANIMSYSTEM_H

#include <nssystem.h>
#include <nsglobal.h>

class NSAnimSystem : public NSSystem
{
public:

	NSAnimSystem();

	~NSAnimSystem();

	virtual bool handleEvent(NSEvent * pEvent);

	virtual void init();

	virtual void update();

	virtual float updatePriority();

	virtual nsstring typeString() { return getTypeString(); }

	static nsstring getTypeString();
};


#endif