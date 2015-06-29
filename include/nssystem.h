/*!
\file nssystem.h

\brief Header file for NSSystem class

This file contains all of the neccessary declartations for the NSSystem class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#ifndef NSSYSTEM_H
#define NSSYSTEM_H

#include <nsglobal.h>
class NSTimer;
class NSScene;
struct NSEvent;

class NSSystem
{
public:

	NSSystem();

	virtual ~NSSystem();

	virtual bool handleEvent(NSEvent * pEvent)=0;

	virtual void draw();

	virtual void init()=0;

	virtual void update() = 0;

	virtual float drawPriority();

	virtual float updatePriority() = 0;

	virtual nsstring typeString() = 0;
};

#endif