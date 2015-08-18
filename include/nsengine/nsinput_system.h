/*!
\file nsinput_system.h

\brief Header file for NSInputSystem class

This file contains all of the neccessary declarations for the NSInputSystem class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#ifndef NSCUSTOMSYSTEM_H
#define NSCUSTOMSYSTEM_H

#include <nssystem.h>
#include <nsinput_map.h>
#include <nsmath.h>

struct NSInputKeyEvent;
struct NSInputMouseButtonEvent;
struct NSInputMouseMoveEvent;
struct NSInputMouseScrollEvent;

class NSInputSystem : public NSSystem
{
public:

	typedef std::vector<nsinput_map::ctxt*> ContextStack;

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

	virtual int32 update_priority();

	void popContext();

	void pushContext(const nsstring & pName);

	void setCursorPos(const fvec2 & cursorPos);
	
private:

	void _keyPress(nsinput_map::key_val pKey);

	void _keyRelease(nsinput_map::key_val pKey);

	void _mouseMove(const fvec2 & cursorPos);

	void _mousePress(nsinput_map::mouse_button_val pButton, const fvec2 & mousePos);

	void _mouseRelease(nsinput_map::mouse_button_val pButton, const fvec2 & mousePos);

	void _mouseScroll(float pDelta, const fvec2 & mousePos);

	bool _checkTriggerModifiers(const nsinput_map::trigger & t);

	void _createActionEvent(nsinput_map::trigger & trigger);

	void _createStateEvent(nsinput_map::trigger & trigger, bool toggle);

	void _setAxesFromTrigger(nsinput_map::axis_map & am, const nsinput_map::trigger & t);
	
	ContextStack mContextStack;
	nsinput_map::key_modifier_set m_key_modifiers;
	nsinput_map::mouse_modifier_set m_mouse_modifiers;

	fvec2 mCurrentPos;
	fvec2 mLastPos;
	float mScrollDelta;
	uivec2 mInputMapID;
};


#endif
