/*!
\file nstilecomp.h

\brief Header file for NSTileComp class

This file contains all of the neccessary declarations for the NSTileComp class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#ifndef NSTILECOMP_H
#define NSTILECOMP_H

#include <nscomponent.h>
#include <nspupper.h>

class NSTileComp : public NSComponent
{
public:
	template <class PUPer>
	friend void pup(PUPer & p, NSTileComp & tc);

	NSTileComp();

	virtual ~NSTileComp();

	virtual NSTileComp * copy(const NSComponent * pComp);

	virtual void init();

	virtual void pup(NSFilePUPer * p);

	NSTileComp & operator=(const NSTileComp & pRHSComp);

};

template <class PUPer>
void pup(PUPer & p, NSTileComp & tc)
{
	// do nothing for now
}

#endif
