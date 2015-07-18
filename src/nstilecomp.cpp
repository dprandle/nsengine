/*!
\file nstilecomp.cpp

\brief Definition file for NSTileComp class

This file contains all of the neccessary definitions for the NSTileComp class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#include <nstilecomp.h>
#include <nsentity.h>

NSTileComp::NSTileComp() :NSComponent()
{}

NSTileComp::~NSTileComp()
{}

NSTileComp* NSTileComp::copy(const NSComponent * pToCopy)
{
	if (pToCopy == NULL)
		return NULL;
	const NSTileComp * comp = (const NSTileComp*)pToCopy;
	(*this) = (*comp);
	return this;
}

void NSTileComp::init()
{}


void NSTileComp::pup(NSFilePUPer * p)
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

NSTileComp & NSTileComp::operator=(const NSTileComp & pRHSComp)
{
	postUpdate(true);
	return (*this);
}
