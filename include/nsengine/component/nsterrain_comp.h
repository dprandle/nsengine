/*!
\file nsterrain_comp.h

\brief Header file for nsterrain_comp class

This file contains all of the neccessary declarations for the nsterrain_comp class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#ifndef NSTERRAIN_COMP_H
#define NSTERRAIN_COMP_H

#include "nscomponent.h"

class nsterrain_comp : public nscomponent
{
public:
	template <class PUPer>
	friend void pup(PUPer & p, nsterrain_comp & tc);

	nsterrain_comp();

	nsterrain_comp(const nsterrain_comp & copy);

	virtual ~nsterrain_comp();

	virtual void init();

	void set_height_bounds(float min_, float max_);

	void set_height_bounds(const fvec2 & minmax_);

	const fvec2 & height_bounds() const;

	virtual void pup(nsfile_pupper * p);

	nsterrain_comp & operator=(nsterrain_comp rhs_);

private:
	fvec2 m_minmax;
};

template <class PUPer>
void pup(PUPer & p, nsterrain_comp & tc)
{
	pup(p, tc.m_minmax, "minmax");
}

#endif
