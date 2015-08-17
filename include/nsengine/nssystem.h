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
#include <nsevent_handler.h>

class NSTimer;
class NSScene;
class NSEvent;

class NSSystem : public NSEventHandler
{
public:

	friend class NSSysFactory;
	
	NSSystem();

	virtual ~NSSystem();

	virtual void draw();

	virtual void init()=0;

	virtual void update() = 0;

	virtual int32 drawPriority();

	virtual int32 updatePriority() = 0;

	uint32 type();

  protected:
	
	void addTriggerHash(uint32 key, const nsstring & trigname);
	void removeTriggerHash(uint32 key);
	uint32 triggerHash(uint32 key);
	
  private:
	uint32uintu_map mHashedInputTriggers;
	uint32 mHashedType;
};

#endif
