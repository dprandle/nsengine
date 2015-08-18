/*!
\file nsevent.h

\brief Header file for NSEvent class

This file contains all of the neccessary declarations for the NSEvent class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#include <nsmath.h>
#include <nsinput_map.h>

#ifndef NSEVENT_H
#define NSEVENT_H

class NSEvent
{
public:
	NSEvent():
		refcount(0)
	{}
    virtual ~NSEvent() {}

	uint32 refcount;
};

class NSKeyEvent : public NSEvent
{
public:
	NSKeyEvent(nsinput_map::key_val k, bool pressed) :
		mKey(k),
		mPressed(pressed),
		NSEvent()
	{}

	bool mPressed;
	nsinput_map::key_val mKey;
};

class NSMouseButtonEvent : public NSEvent
{
public:
	NSMouseButtonEvent(nsinput_map::mouse_button_val btn, bool pressed, const fvec2 & normalized_mpos) :
		mb(btn),
		mPressed(pressed),
		mNormMousePos(normalized_mpos),
		NSEvent()
	{}

	fvec2 mNormMousePos;
	nsinput_map::mouse_button_val mb;
	bool mPressed;
};

class NSMouseMoveEvent : public NSEvent
{
public:
	NSMouseMoveEvent(const fvec2 & normalized_mpos) :
		mNormMousePos(normalized_mpos),
		NSEvent()
	{}

	fvec2 mNormMousePos;
};

class NSMouseScrollEvent : public NSEvent
{
public:
	NSMouseScrollEvent(float scrollDelta, const fvec2 & normalized_mpos) :
		mScroll(scrollDelta),
		mNormMousePos(normalized_mpos),
		NSEvent()
	{}
	fvec2 mNormMousePos;
	float mScroll;
};

class NSActionEvent : public NSEvent
{
public:
	NSActionEvent(uint32 trigger_hashid):
		mTriggerHashName(trigger_hashid),
		NSEvent()
	{}

	uint32 mTriggerHashName;
	nsinput_map::axis_map axes;
};

class NSStateEvent : public NSActionEvent
{
public:
	NSStateEvent(uint32 trigger_hashid, bool toggle):
		mToggle(toggle),
		NSActionEvent(trigger_hashid)
	{}

	bool mToggle;
};

class NSSelFocusEvent : public NSEvent
{
  public:
	
	NSSelFocusEvent(const uivec3 & focid):
		mFocID(focid)
	{}
	uivec3 mFocID;
};

#endif
