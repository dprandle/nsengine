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

struct NSEvent
{
	NSEvent():
		refcount(0)
	{}

	nsuint refcount;
};

struct NSKeyEvent : public NSEvent
{
	NSKeyEvent(NSInputMap::Key k, bool pressed) :
		mKey(k),
		mPressed(pressed),
		NSEvent()
	{}

	bool mPressed;
	NSInputMap::Key mKey;
};

struct NSMouseButtonEvent : public NSEvent
{
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

struct NSMouseMoveEvent : public NSEvent
{
	NSMouseMoveEvent(const fvec2 & normalized_mpos) :
		mNormMousePos(normalized_mpos),
		NSEvent()
	{}

	fvec2 mNormMousePos;
};

struct NSMouseScrollEvent : NSEvent
{
	NSMouseScrollEvent(nsfloat scrollDelta, const fvec2 & normalized_mpos) :
		mScroll(scrollDelta),
		mNormMousePos(normalized_mpos),
		NSEvent()
	{}
	fvec2 mNormMousePos;
	nsfloat mScroll;
};

struct NSActionEvent : NSEvent
{
	NSActionEvent(const nsstring & triggerName):
		mTriggerName(triggerName),
		NSEvent()
	{}

	nsstring mTriggerName;
	NSInputMap::AxisMap axes;
};

struct NSStateEvent : NSActionEvent
{
	NSStateEvent(const nsstring & triggerName, bool toggle):
		mToggle(toggle),
		NSActionEvent(triggerName)
	{}

	bool mToggle;
};

#endif
