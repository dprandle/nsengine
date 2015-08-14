/*!
\file nsevent.h

\brief Header file for NSEvent class

This file contains all of the neccessary declarations for the NSEvent class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#include <nsmath.h>
#include <nsinputmap.h>

#ifndef NSEVENT_H
#define NSEVENT_H

class NSEvent
{
public:
	NSEvent():
		refcount(0)
	{}
    virtual ~NSEvent() {}

	nsuint refcount;
};

class NSKeyEvent : public NSEvent
{
public:
	NSKeyEvent(NSInputMap::Key k, bool pressed) :
		mKey(k),
		mPressed(pressed),
		NSEvent()
	{}

	bool mPressed;
	NSInputMap::Key mKey;
};

class NSMouseButtonEvent : public NSEvent
{
public:
	NSMouseButtonEvent(NSInputMap::MouseButton btn, bool pressed, const fvec2 & normalized_mpos) :
		mb(btn),
		mPressed(pressed),
		mNormMousePos(normalized_mpos),
		NSEvent()
	{}

	fvec2 mNormMousePos;
	NSInputMap::MouseButton mb;
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
	NSMouseScrollEvent(nsfloat scrollDelta, const fvec2 & normalized_mpos) :
		mScroll(scrollDelta),
		mNormMousePos(normalized_mpos),
		NSEvent()
	{}
	fvec2 mNormMousePos;
	nsfloat mScroll;
};

class NSActionEvent : public NSEvent
{
public:
	NSActionEvent(nsuint trigger_hashid):
		mTriggerHashName(trigger_hashid),
		NSEvent()
	{}

	nsuint mTriggerHashName;
	NSInputMap::AxisMap axes;
};

class NSStateEvent : public NSActionEvent
{
public:
	NSStateEvent(nsuint trigger_hashid, bool toggle):
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
