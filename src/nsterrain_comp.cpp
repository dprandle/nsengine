/*!
\file nsterraincomp.cpp

\brief Definition file for NSTerrainComp class

This file contains all of the neccessary definitions for the NSTerrainComp class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#include <nsterrain_comp.h>
#include <nsentity.h>

NSTerrainComp::NSTerrainComp() :minmax(0.0f,1.0f), NSComponent()
{}

NSTerrainComp::~NSTerrainComp()
{}

NSTerrainComp* NSTerrainComp::copy(const NSComponent * pToCopy)
{
	if (pToCopy == NULL)
		return NULL;
	const NSTerrainComp * comp = (const NSTerrainComp*)pToCopy;
	(*this) = (*comp);
	return this;
}

void NSTerrainComp::init()
{}

void NSTerrainComp::setHeightBounds(float min, float max)
{
	minmax.set(min, max);
}

void NSTerrainComp::setHeightBounds(const fvec2 & minmax_)
{
	minmax = minmax_;
}

const fvec2 & NSTerrainComp::heightBounds() const
{
	return minmax;
}


void NSTerrainComp::pup(NSFilePUPer * p)
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

NSTerrainComp & NSTerrainComp::operator=(const NSTerrainComp & pRHSComp)
{
	minmax = pRHSComp.minmax;
	postUpdate(true);
	return (*this);
}
