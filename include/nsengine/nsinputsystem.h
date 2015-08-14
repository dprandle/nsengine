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

	bool keyEvent(NSKeyEvent * evnt);

	bool mouseButtonEvent(NSMouseButtonEvent * evnt);

	bool mouseMoveEvent(NSMouseMoveEvent * evnt);

	bool mouseScrollEvent(NSMouseScrollEvent * evnt);
	
	virtual void init();

	void setInputMap(const uivec2 & resid);

	const uivec2 & inputMap();

	virtual void update();

	virtual int32 updatePriority();

	void popContext();

	void pushContext(const nsstring & pName);

	void setCursorPos(const fvec2 & cursorPos);
	
private:

	void _keyPress(NSInputMap::Key pKey);

	void _keyRelease(NSInputMap::Key pKey);

	void _mouseMove(const fvec2 & cursorPos);

	void _mousePress(NSInputMap::MouseButton pButton, const fvec2 & mousePos);

	void _mouseRelease(NSInputMap::MouseButton pButton, const fvec2 & mousePos);

	void _mouseScroll(float pDelta, const fvec2 & mousePos);

	bool _checkTriggerModifiers(const NSInputMap::Trigger & t);

	void _createActionEvent(NSInputMap::Trigger & trigger);

	void _createStateEvent(NSInputMap::Trigger & trigger, bool toggle);

	void _setAxesFromTrigger(NSInputMap::AxisMap & am, const NSInputMap::Trigger & t);
	
	ContextStack mContextStack;
	NSInputMap::KeyModifiers mKeyMods;
	NSInputMap::MouseModifiers mMouseMods;

	fvec2 mCurrentPos;
	fvec2 mLastPos;
	float mScrollDelta;
	uivec2 mInputMapID;
};


#endif
