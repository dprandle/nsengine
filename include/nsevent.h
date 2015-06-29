/*!
\file nsevent.h

\brief Header file for NSEvent class

This file contains all of the neccessary declarations for the NSEvent class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#include <nsmath.h>

#ifndef NSEVENT_H
#define NSEVENT_H

struct NSEvent
{
	enum ID {
		None,
		InputKey,
		InputMouseButton,
		InputMouseMove,
		InputMouseScroll,
		SelPick,
		SelSet,
		SelAdd,
		ClearSelection,
		BuildModeEnable,
		SelFocusChangeEvent,
		Custom //! Used for extending events
	};

	NSEvent(ID pID, const nsstring & pName):
		mID(pID),
		mName(pName)
	{}

	nsstring mName;
	ID mID;
};

struct NSInputKeyEvent : NSEvent
{
	enum PressOrRelease { Released, Pressed };
	NSInputKeyEvent(const nsstring & pEventName, PressOrRelease pPorR, const fvec2 & pMousePos) :
	mPorR(pPorR),
	mMousePos(pMousePos),
	NSEvent(InputKey, pEventName)
	{}

	PressOrRelease mPorR;
	fvec2 mMousePos;
};

struct NSInputMouseEvent : NSEvent
{
	NSInputMouseEvent(ID pID, const nsstring & pEventName, const fvec2 & pPos) :
	mPos(pPos),
	NSEvent(pID, pEventName) {}

	fvec2 mPos;
};

struct NSInputMouseButtonEvent : NSInputMouseEvent
{
	enum PressOrRelease { Released, Pressed };
	NSInputMouseButtonEvent(const nsstring & pEventName, PressOrRelease pPorR, const fvec2 & pPos) :
	mPorR(pPorR),
	NSInputMouseEvent(InputMouseButton, pEventName, pPos) {}

	PressOrRelease mPorR;
};

struct NSInputMouseMoveEvent : NSInputMouseEvent
{
	NSInputMouseMoveEvent(const nsstring & pEventName, const fvec2 & pPos, const fvec2 & pDelta) :
	mDelta(pDelta),
	NSInputMouseEvent(InputMouseMove, pEventName, pPos) {}

	fvec2 mDelta;
};

struct NSInputMouseScrollEvent : NSInputMouseEvent
{
	NSInputMouseScrollEvent(const nsstring & pEventName, const fvec2 & pPos, nsfloat pScroll) :
	mScroll(pScroll),
	NSInputMouseEvent(InputMouseScroll, pEventName, pPos) {}

	nsfloat mScroll;
};

struct NSSelPickEvent : NSEvent
{
	NSSelPickEvent(const nsstring & pEventName, const fvec2 & pPickPos) :
	mPickPos(pPickPos),
	NSEvent(SelPick, pEventName)
	{}

	fvec2 mPickPos;
};

struct NSSelSetEvent : NSEvent
{
	NSSelSetEvent(const nsstring & pEventName, const uivec3 & pEntRefID) :
	mEntRefID(pEntRefID),
	NSEvent(SelSet, pEventName)
	{}

	uivec3 mEntRefID;
};

struct NSSelAddEvent : NSEvent
{
	NSSelAddEvent(const nsstring & pEventName, const uivec3 & pEntRefID) :
	mEntRefID(pEntRefID),
	NSEvent(SelAdd, pEventName)
	{}

	uivec3 mEntRefID;
};

struct NSSelFocusChangeEvent : NSEvent
{
	NSSelFocusChangeEvent(const nsstring & pEventName, const uivec3 & pNewSelCenter) :
	mNewSelCenter(pNewSelCenter),
	NSEvent(SelFocusChangeEvent,pEventName)
	{}
	uivec3 mNewSelCenter;
};

struct NSClearSelectionEvent : NSEvent
{
	NSClearSelectionEvent(const nsstring & pEventName) :
	NSEvent(ClearSelection, pEventName)
	{}
};

#endif