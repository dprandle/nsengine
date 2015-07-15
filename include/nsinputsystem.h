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
#include <nsinputmap.h>
#include <nsmath.h>

struct NSInputKeyEvent;
struct NSInputMouseButtonEvent;
struct NSInputMouseMoveEvent;
struct NSInputMouseScrollEvent;

class NSInputSystem : public NSSystem
{
public:

	typedef std::vector<NSInputMap::Context*> ContextStack;
	
	NSInputSystem();

	~NSInputSystem();

	virtual bool handleEvent(NSEvent * pEvent);

	virtual void init();

	void setInputMap(const uivec2 & resid);

	const uivec2 & inputMap();

	virtual void update();

	virtual float updatePriority();

	virtual nsstring typeString() { return getTypeString(); }

	static nsstring getTypeString();

	void keyPress(NSInputMap::Key pKey);

	void keyRelease(NSInputMap::Key pKey);

	void mouseMove(nsfloat pPosX, nsfloat pPosY);

	void mousePress(NSInputMap::MouseButton pButton, nsfloat pPosX, nsfloat pPosY);

	void mouseRelease(NSInputMap::MouseButton pButton, nsfloat pPosX, nsfloat pPosY);

	void mouseScroll(nsfloat pDelta, nsfloat pPosX, nsfloat pPosY);

	void popContext();

	void pushContext(const nsstring & pName);

	void setLastPos(const fvec2 & pLastPos);
	
private:

	bool _checkTriggerModifiers(const NSInputMap::Trigger & t);
	
	void _eventKey(NSInputKeyEvent * pEvent);
	void _eventMouseButton(NSInputMouseButtonEvent * pEvent);
	void _eventMouseMove(NSInputMouseMoveEvent * pEvent);
	void _eventMouseScroll(NSInputMouseScrollEvent * pEvent);
	
	ContextStack mContextStack;
	NSInputMap::Modifiers mMods;
	NSInputMap::MouseModifiers mMouseMods;
	fvec2 mMLastPos;
	uivec2 mInputMapID;
};


#endif
