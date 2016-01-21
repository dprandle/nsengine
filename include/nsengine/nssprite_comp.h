/*! 
	\file nssprite_comp.h
	
	\brief Header file for nssprite_comp class

	This file contains all of the neccessary declarations for the nssprite_comp class.

	\author Daniel Randle
	\date December 11 2013
	\copywrite Earth Banana Games 2013
*/

#ifndef NSSPRITE_COMP_H
#define NSSPRITE_COMP_H

#include <nscomponent.h>

class nssprite_sheet_comp : public nscomponent
{
public:
	template <class PUPer>
	friend void pup(PUPer & p, nssprite_sheet_comp & tc);

	nssprite_sheet_comp();
	nssprite_sheet_comp(const nssprite_sheet_comp & rhs);

	virtual ~nssprite_sheet_comp();

	virtual nssprite_sheet_comp * copy(const nscomponent * comp_);

	virtual void init();

	virtual void pup(nsfile_pupper * p);

	nssprite_sheet_comp & operator=(nssprite_sheet_comp rhs_);

};

template <class PUPer>
void pup(PUPer & p, nssprite_sheet_comp & tc)
{
	// do nothing for now
}

#endif
