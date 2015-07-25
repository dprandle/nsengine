/*!
\file nsselcomp.h

\brief Header file for NSSelComp class

This file contains all of the neccessary declarations for the NSSelComp class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#ifndef NSSELCOMP_H
#define NSSELCOMP_H

#include <nscomponent.h>
#include <unordered_set>
#include <nstformcomp.h>


class NSShader;
class NSTimer;

class NSSelComp : public NSComponent
{
public:
	template <class PUPer>
	friend void pup(PUPer & p, NSSelComp & sc);

	NSSelComp();
	~NSSelComp();

	bool add(nsuint pTransformID);

	nsuintu_set::iterator begin();

	bool contains(nsuint pTransformID);

	NSSelComp * copy(const NSComponent* pToCopy);

	nsuint count();

	void clear();

	void enableDraw(bool pEnable);

	void enableMove(const bool & pEnable);

	nsuintu_set::iterator end();

	bool empty();

	const fvec4 & defaultColor();

	const nsfloat & maskAlpha();

	const fvec4 & color();

	virtual void init();

	bool drawEnabled();

	const bool & moveEnabled();

	bool selected();

	bool remove(nsuint pTransformID);

	bool set(nsuint pTransformID);

	virtual void pup(NSFilePUPer * p);

	void setDefaultColor(const fvec4 & pColor);

	void setMaskAlpha(const nsfloat & pAlpha);

	void setSelected(bool pSelected);

	void setColor(const fvec4 & pColor);

	NSSelComp & operator=(const NSSelComp & pRHS);

private:
	fvec4 mDefaultSelColor;
	fvec4 mSelColor;
	nsfloat mMaskAlpha;
	bool mSelected;
	bool mDrawEnabled;
	bool mMoveWithInput;
	nsuintu_set mSelection;
};

template <class PUPer>
void pup(PUPer & p, NSSelComp & sc)
{
	pup(p, sc.mDefaultSelColor, "defaultSelColor");
	pup(p, sc.mSelColor, "selColor");
	pup(p, sc.mMaskAlpha, "maskAlpha");
	pup(p, sc.mDrawEnabled, "drawEnabled");
}

#endif
