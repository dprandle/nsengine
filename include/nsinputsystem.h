/*!
\file nsinputsystem.h

\brief Header file for NSInputSystem class

This file contains all of the neccessary declarations for the NSInputSystem class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#ifndef NSCUSTOMSYSTEM_H
#define NSCUSTOMSYSTEM_H

#include <nssystem.h>

struct NSInputKeyEvent;
struct NSInputMouseButtonEvent;
struct NSInputMouseMoveEvent;
struct NSInputMouseScrollEvent;

class NSInputSystem : public NSSystem
{
public:
	NSInputSystem();

	~NSInputSystem();

	virtual bool handleEvent(NSEvent * pEvent);

	virtual void init();

	virtual void update();

	virtual float updatePriority();

	virtual nsstring typeString() { return getTypeString(); }

	static nsstring getTypeString();

private:
	void _eventKey(NSInputKeyEvent * pEvent);
	void _eventMouseButton(NSInputMouseButtonEvent * pEvent);
	void _eventMouseMove(NSInputMouseMoveEvent * pEvent);
	void _eventMouseScroll(NSInputMouseScrollEvent * pEvent);

};


#endif