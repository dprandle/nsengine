/*!
\file nstilebrushcomp.h

\brief Header file for NSTileBrushComp class

This file contains all of the neccessary declarations for the NSTileBrushComp class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#ifndef NSTILEBRUSHCOMP_H
#define NSTILEBRUSHCOMP_H

#include <nscomponent.h>
#include <nsmath.h>

class NSTileBrushComp : public NSComponent
{
public:
	template <class PUPer>
	friend void pup(PUPer & p, NSTileBrushComp & tbc);

	NSTileBrushComp();

	virtual ~NSTileBrushComp();

	bool add(int x, int y);

	bool add(const ivec2 & pGridSpace);

	ivec2array::iterator begin();

	void changeHeight(const nsint & pAmount);

	bool contains(int x, int y);

	bool contains(const ivec2 & pGridSpace);

	virtual NSTileBrushComp * copy(const NSComponent* pComp);

	virtual void pup(NSFilePUPer * p);

	ivec2array::iterator end();

	nsint height() const;

	virtual void init();

	bool remove(int x, int y);

	bool remove(const ivec2 & pGridPos);

	void setHeight(const nsint & pHeight);

	NSTileBrushComp & operator=(const NSTileBrushComp & pRHSComp);

private:
	ivec2array mBrush;
	nsint mHeight;
};

template <class PUPer>
void pup(PUPer & p, NSTileBrushComp & tbc)
{
	pup(p, tbc.mBrush, "brush");
	pup(p, tbc.mHeight, "height");
}

#endif
