/*!
\file nstile_brush_comp.h

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

	bool add(int32 x, int32 y);

	bool add(const ivec2 & pGridSpace);

	ivec2array::iterator begin();

	void changeHeight(const int32 & pAmount);

	bool contains(int32 x, int32 y);

	bool contains(const ivec2 & pGridSpace);

	virtual NSTileBrushComp * copy(const NSComponent* pComp);

	virtual void pup(nsfile_pupper * p);

	ivec2array::iterator end();

	int32 height() const;

	virtual void init();

	bool remove(int32 x, int32 y);

	bool remove(const ivec2 & pGridPos);

	void setHeight(const int32 & pHeight);

	NSTileBrushComp & operator=(const NSTileBrushComp & pRHSComp);

private:
	ivec2array mBrush;
	int32 mHeight;
};

template <class PUPer>
void pup(PUPer & p, NSTileBrushComp & tbc)
{
	pup(p, tbc.mBrush, "brush");
	pup(p, tbc.mHeight, "height");
}

#endif
