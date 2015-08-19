/*!
\file nsinputcomponent.h

\brief Header file for NSInputComp class

This file contains all of the neccessary declarations for the NSInputComp class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#ifndef NSINPUTCOMP_H
#define NSINPUTCOMP_H

#include <nscomponent.h>
#include <nsmath.h>
#include <map>

class NSInputComp : public NSComponent
{
public:

	template <class PUPer>
	friend void pup(PUPer & p, NSInputComp & in);

	struct Action
	{
		Action( 
		bool pPressed = false, 
		const fvec2 & pPos = fvec2(), 
		const fvec2 & pDelta = fvec2(),
		float pScroll=0.0f):
		mPressed(pPressed),
		mPos(pPos),
		mDelta(pDelta),
		mScroll(pScroll),
		mActivated(false)
		{}

		bool mActivated;
		bool mPressed;
		fvec2 mPos;
		fvec2 mDelta;
		float mScroll;
	};

	typedef std::map<nsstring, Action> ActionMap;

	NSInputComp();

	virtual ~NSInputComp();

	bool add(const nsstring & pTriggerName);

	void clear();

	virtual void pup(nsfile_pupper * p);

	bool contains(const nsstring & pTriggerName);

	virtual NSInputComp * copy(const NSComponent * pComp);

	virtual void init();

	Action * action(const nsstring & pTriggerName);

	bool remove(const nsstring & pTriggerName);

	void setActivated(const nsstring & pTriggerName, bool pActivate);

	void setActivated(bool pActivate);

	NSInputComp & operator=(const NSInputComp & pRHSComp);

private:
	ActionMap mActions;
};

template <class PUPer>
void pup(PUPer & p, NSInputComp & in)
{
	pup(p, in.mActions, "actions");
}

template <class PUPer>
void pup(PUPer & p, NSInputComp::Action & a, const nsstring & varName)
{
	pup(p, a.mActivated, varName + "mActivated");
	pup(p, a.mPressed, varName + "mPressed");
	pup(p, a.mPos, varName + "mPos");
	pup(p, a.mDelta, varName + "mDelta");
	pup(p, a.mScroll, varName + "mScroll");
}

#endif
