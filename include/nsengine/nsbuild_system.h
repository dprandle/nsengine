/*!
\file nsbuild_system.h

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

	void changeLayer(const int32 & pAmount);

	void enable(const bool & pEnable, const fvec2 & pMousePos);

	void enableOverwrite(bool pEnable);

	void enableMirror(bool pEnable);

	void erase();

	const fvec4 activeBrushColor() const;

	const BrushMode & brushMode();

	NSEntity * buildent();

	const fvec3 & center() const;

	NSEntity * tilebrush();

	NSEntity * objectbrush();

	const int32 & layer() const;

	const Mode & mode() const;

	//virtual bool handleEvent(NSEvent * pEvent);

	virtual void init();

	bool enabled() const;

	bool overwrite() const;

	bool mirror() const;

	void toCursor(const fvec2 & pCursorPos, bool pUpdateCamFirst=false);

	void paint();

	void setBrushMode(const BrushMode & pBrushMode);

	void setActiveBrushColor(const fvec4 & pColor);

	void setMode(const Mode & pMode);

	void setObjectBrush(NSEntity * pBrush);

	void setTileBrush(NSEntity * pBrush);

	void setBuildEnt(NSEntity * pBuildEnt);

	void setLayer(const int32 & pLayer);

	void setCenter(const fvec3 & pMirrorCenter);

	void toggle(const fvec2 & pMousePos);

	virtual void update();

	virtual int32 updatePriority();

private:
	NSEntity * mTileBrush;
	NSEntity * mObjectBrush;
	NSEntity * mMirrorBrush;
	NSEntity * mBuildEnt;
	fvec3 mMirrorCenter;
	uint32 mTBCenterTFormID;
	int32 mLayer;
	Mode mCurrentMode;
	BrushMode mCurrentBrushMode;
	bool mOverwrite;
	bool mEnabled;
	bool mMirrorMode;
};


#endif
