/*!
\file nsselcomp.cpp

\brief Definition file for NSSelComp class

This file contains all of the neccessary definitions for the NSSelComp class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/


#include <nssel_comp.h>
#include <nsentity.h>
#include <nstform_comp.h>
#include <nstimer.h>

NSSelComp::NSSelComp() :
mDefaultSelColor(DEFAULT_SEL_R, DEFAULT_SEL_G, DEFAULT_SEL_B, DEFAULT_SEL_A),
mSelColor(mDefaultSelColor),
mMaskAlpha(DEFAULT_SEL_MASK_A),
mSelected(false),
mDrawEnabled(true),
mSelection(),
mMoveWithInput(true),
NSComponent()
{}

NSSelComp::~NSSelComp()
{}

bool NSSelComp::add(uint32 pTransformID)
{

	if (pTransformID >= mOwner->get<NSTFormComp>()->count())
	{
		dprint("NSSelComp::add - TransformID out of bounds");
		return false;
	}
	if (mSelection.find(pTransformID) != mSelection.end())
		return false;

	mSelection.insert(pTransformID);
	setSelected(true);
	return true;
}

uint32u_set::iterator NSSelComp::begin()
{
	return mSelection.begin();
}

void NSSelComp::clear()
{
	mSelection.clear();
	setSelected(false);
}

NSSelComp* NSSelComp::copy(const NSComponent * pToCopy)
{
	if (pToCopy == NULL)
		return NULL;
	const NSSelComp * comp = (const NSSelComp*)pToCopy;
	(*this) = (*comp);
	return this;
}

bool NSSelComp::contains(uint32 pTransformID)
{
	return (mSelection.find(pTransformID) != mSelection.end());
}

void NSSelComp::enableDraw(bool pEnable)
{
	mDrawEnabled = pEnable;
}

void NSSelComp::pup(NSFilePUPer * p)
{
	if (p->type() == NSFilePUPer::Binary)
	{
		NSBinFilePUPer * bf = static_cast<NSBinFilePUPer *>(p);
		::pup(*bf, *this);
	}
	else
	{
		NSTextFilePUPer * tf = static_cast<NSTextFilePUPer *>(p);
		::pup(*tf, *this);
	}
}

void NSSelComp::enableMove(const bool & pEnable)
{
	mMoveWithInput = pEnable;
}

uint32u_set::iterator NSSelComp::end()
{
	return mSelection.end();
}

const fvec4 & NSSelComp::defaultColor()
{
	return mDefaultSelColor;
}

const float & NSSelComp::maskAlpha()
{
	return mMaskAlpha;
}

const fvec4 & NSSelComp::color()
{
	return mSelColor;
}

void NSSelComp::init()
{}

bool NSSelComp::drawEnabled()
{
	return mDrawEnabled;
}

const bool & NSSelComp::moveEnabled()
{
	return mMoveWithInput;
}

bool NSSelComp::selected()
{
	return mSelected;
}

bool NSSelComp::remove(uint32 pTransformID)
{
	if (contains(pTransformID))
	{
		mSelection.erase(pTransformID);
		return true;
	}
	return false;
}

bool NSSelComp::set(uint32 pTransformID)
{
	clear();
	return add(pTransformID);
}

void NSSelComp::setDefaultColor(const fvec4 & pColor)
{
	mDefaultSelColor = pColor;
	mSelColor = pColor;
}


void NSSelComp::setMaskAlpha(const float & pAlpha)
{
	mMaskAlpha = pAlpha;
}

void NSSelComp::setSelected(bool pSelected)
{
	mSelected = pSelected;
}

void NSSelComp::setColor(const fvec4 & pColor)
{
	mSelColor = pColor;
}

uint32 NSSelComp::count()
{
	return static_cast<uint32>(mSelection.size());
}

bool NSSelComp::empty()
{
	return mSelection.empty();
}

NSSelComp & NSSelComp::operator=(const NSSelComp & pRHSComp)
{
	mSelColor = pRHSComp.mSelColor;
	mDefaultSelColor = pRHSComp.mSelColor;
	mMaskAlpha = pRHSComp.mMaskAlpha;
	mDrawEnabled = pRHSComp.mDrawEnabled;
	postUpdate(true);
	return (*this);
}
