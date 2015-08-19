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


void NSTerrainComp::pup(nsfile_pupper * p)
{
	if (p->type() == nsfile_pupper::pup_binary)
	{
		nsbinary_file_pupper * bf = static_cast<nsbinary_file_pupper *>(p);
		::pup(*bf, *this);
	}
	else
	{
		nstext_file_pupper * tf = static_cast<nstext_file_pupper *>(p);
		::pup(*tf, *this);
	}
}

NSTerrainComp & NSTerrainComp::operator=(const NSTerrainComp & pRHSComp)
{
	minmax = pRHSComp.minmax;
	post_update(true);
	return (*this);
}
