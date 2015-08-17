/*!
\file nsmovement_system.h

\brief Header file for NSMovementSystem class

This file contains all of the neccessary declarations for the NSMovementSystem class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#ifndef NSMOVEMENTSYSTEM_H
#define NSMOVEMENTSYSTEM_H

#include <nssystem.h>

class NSMovementSystem : public NSSystem
{
public:

	NSMovementSystem();

	~NSMovementSystem();

//	virtual bool handleEvent(NSEvent * pEvent);

	virtual void init();

	virtual void update();

	virtual int32 updatePriority();

private:
};


#endif
