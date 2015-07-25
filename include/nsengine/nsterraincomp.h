/*!
\file nsterraincomp.h

\brief Header file for NSTerrainComp class

This file contains all of the neccessary declarations for the NSTerrainComp class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#ifndef NSTERRAINCOMP_H
#define NSTERRAINCOMP_H

#include <nscomponent.h>
#include <nspupper.h>

class NSTerrainComp : public NSComponent
{
public:
	template <class PUPer>
	friend void pup(PUPer & p, NSTerrainComp & tc);

	NSTerrainComp();

	virtual ~NSTerrainComp();

	virtual NSTerrainComp * copy(const NSComponent * pComp);

	virtual void init();

	void setHeightBounds(float min, float max);

	void setHeightBounds(const fvec2 & minmax_);

	const fvec2 & heightBounds() const;

	virtual void pup(NSFilePUPer * p);

	NSTerrainComp & operator=(const NSTerrainComp & pRHSComp);

private:
	fvec2 minmax;
};

template <class PUPer>
void pup(PUPer & p, NSTerrainComp & tc)
{
	pup(p, tc.minmax, "minmax");
}

#endif
