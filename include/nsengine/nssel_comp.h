/*!
\file nssel_comp.h

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
#include <nstform_comp.h>


class NSShader;
class NSTimer;

class NSSelComp : public NSComponent
{
public:
	template <class PUPer>
	friend void pup(PUPer & p, NSSelComp & sc);

	NSSelComp();
	~NSSelComp();

	bool add(uint32 pTransformID);

	uint32u_set::iterator begin();

	bool contains(uint32 pTransformID);

	NSSelComp * copy(const NSComponent* pToCopy);

	uint32 count();

	void clear();

	void enableDraw(bool pEnable);

	void enableMove(const bool & pEnable);

	uint32u_set::iterator end();

	bool empty();

	const fvec4 & defaultColor();

	const float & maskAlpha();

	const fvec4 & color();

	virtual void init();

	bool drawEnabled();

	const bool & moveEnabled();

	bool selected();

	bool remove(uint32 pTransformID);

	bool set(uint32 pTransformID);

	virtual void pup(NSFilePUPer * p);

	void setDefaultColor(const fvec4 & pColor);

	void setMaskAlpha(const float & pAlpha);

	void setSelected(bool pSelected);

	void setColor(const fvec4 & pColor);

	NSSelComp & operator=(const NSSelComp & pRHS);

private:
	fvec4 mDefaultSelColor;
	fvec4 mSelColor;
	float mMaskAlpha;
	bool mSelected;
	bool mDrawEnabled;
	bool mMoveWithInput;
	uint32u_set mSelection;
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
