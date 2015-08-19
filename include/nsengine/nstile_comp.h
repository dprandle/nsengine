/*!
\file nstile_comp.h

\brief Header file for nstile_comp class

This file contains all of the neccessary declarations for the nstile_comp class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#ifndef NSTILE_COMP_H
#define NSTILE_COMP_H

#include <nscomponent.h>
#include <nspupper.h>

class nstile_comp : public NSComponent
{
public:
	template <class PUPer>
	friend void pup(PUPer & p, nstile_comp & tc);

	nstile_comp();

	virtual ~nstile_comp();

	virtual nstile_comp * copy(const NSComponent * comp_);

	virtual void init();

	virtual void pup(nsfile_pupper * p);

	nstile_comp & operator=(const nstile_comp & rhs_);

};

template <class PUPer>
void pup(PUPer & p, nstile_comp & tc)
{
	// do nothing for now
}

#endif
