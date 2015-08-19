/*!
\file nstilecomp.cpp

\brief Definition file for NSTileComp class

This file contains all of the neccessary definitions for the NSTileComp class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#include <nstile_comp.h>
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


void NSTileComp::pup(nsfile_pupper * p)
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

NSTileComp & NSTileComp::operator=(const NSTileComp & pRHSComp)
{
	post_update(true);
	return (*this);
}
