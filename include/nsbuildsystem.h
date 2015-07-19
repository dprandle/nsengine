/*!
\file nsbuildsystem.h

\brief Header file for NSBuildSystem class

This file contains all of the neccessary declarations for the NSBuildSystem class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#ifndef NSBUILDSYSTEM_H
#define NSBUILDSYSTEM_H

#include <nssystem.h>
#include <nsmath.h>

class NSEntity;

class NSBuildSystem : public NSSystem
{
public:

	enum Mode {
		Build,
		Erase
	};

	enum BrushMode {
		None,
		Tile,
		Object
	};

	NSBuildSystem();

	~NSBuildSystem();

	void changeLayer(const nsint & pAmount);

	void enable(const bool & pEnable, const fvec2 & pMousePos);

	void enableOverwrite(nsbool pEnable);

	void enableMirror(nsbool pEnable);

	void erase();

	const fvec4 activeBrushColor() const;

	const BrushMode & brushMode();

	NSEntity * buildent();

	const fvec3 & center() const;

	NSEntity * tilebrush();

	NSEntity * objectbrush();

	const nsint & layer() const;

	const Mode & mode() const;

	virtual bool handleEvent(NSEvent * pEvent);

	virtual void init();

	nsbool enabled() const;

	nsbool overwrite() const;

	nsbool mirror() const;

	void toCursor(const fvec2 & pCursorPos, bool pUpdateCamFirst=false);

	void paint();

	void setBrushMode(const BrushMode & pBrushMode);

	void setActiveBrushColor(const fvec4 & pColor);

	void setMode(const Mode & pMode);

	void setObjectBrush(NSEntity * pBrush);

	void setTileBrush(NSEntity * pBrush);

	void setBuildEnt(NSEntity * pBuildEnt);

	void setLayer(const nsint & pLayer);

	void setCenter(const fvec3 & pMirrorCenter);

	void toggle(const fvec2 & pMousePos);

	virtual void update();

	virtual nsint updatePriority();

private:
	NSEntity * mTileBrush;
	NSEntity * mObjectBrush;
	NSEntity * mMirrorBrush;
	NSEntity * mBuildEnt;
	fvec3 mMirrorCenter;
	nsuint mTBCenterTFormID;
	nsint mLayer;
	Mode mCurrentMode;
	BrushMode mCurrentBrushMode;
	nsbool mOverwrite;
	nsbool mEnabled;
	nsbool mMirrorMode;
};


#endif
