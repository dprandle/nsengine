/*!
\file nsselectionsystem.h

\brief Header file for NSSelectionSystem class

This file contains all of the neccessary declarations for the NSSelectionSystem class.

\author Daniel Randle
\date March 8 2014
\copywrite Earth Banana Games 2013
*/

#ifndef NSSELECTIONSYSTEM_H
#define NSSELECTIONSYSTEM_H

#include <nssystem.h>
#include <nsglobal.h>
#include <nsevent.h>
#include <nstformcomp.h>
#include <nsmath.h>

#define NSSEL_SELECT "SelectEntity"
#define NSSEL_MULTISELECT "MultiSelect"
#define NSSEL_SHIFTSELECT "ShiftSelect"
#define NSSEL_MOVE "MoveSelection"
#define NSSEL_MOVE_XY "MoveSelectionXY"
#define NSSEL_MOVE_ZY "MoveSelectionZY"
#define NSSEL_MOVE_ZX "MoveSelectionZX"
#define NSSEL_MOVE_X "MoveSelectionX"
#define NSSEL_MOVE_Y "MoveSelectionY"
#define NSSEL_MOVE_Z "MoveSelectionZ"
#define NSSEL_MOVE_TOGGLE "MoveSelectionToggle"

class NSScene;
class NSSelComp;
class NSSelectionShader;
class NSActionEvent;
class NSStateEvent;

class NSSelectionSystem : public NSSystem
{
public:
	
	enum Axis
	{
		XAxis=0x0001,
		YAxis=0x0010,
		ZAxis=0x0100
	};

	NSSelectionSystem();

	~NSSelectionSystem();

	bool add(NSEntity * ent, nsuint tformid);

	bool addToGrid();

	void changeLayer(int pChange);

	bool collision();

	void del();

	void clear();

	bool contains(const uivec3 & itemid);

	virtual void draw();

	bool empty();

	void enableLayerMode(const nsbool & pMode);

	nsint layer() const;

	const uivec3 & center();

	virtual nsint drawPriority();

	virtual nsint updatePriority();

//	virtual bool handleEvent(NSEvent * pEvent);

	uivec3 pick(nsfloat mousex, nsfloat mousey);

	uivec3 pick(const fvec2 & mpos);

	virtual void init();

	nsbool layerMode() const;

	bool brushValid();

	bool tileSwapValid();

	void remove(NSEntity * ent, nsuint pTFormID);

	void removeFromGrid();

	void resetColor();

	void rotate(NSEntity * ent, const fvec4 & axisangle);

	void rotate(const fvec4 & axisangle);

	void rotate(NSEntity * ent, NSTFormComp::DirVec axis, nsfloat angle);

	void rotate(NSTFormComp::DirVec axis, nsfloat angle);

	void rotate(NSEntity * ent, NSTFormComp::Axis axis, nsfloat angle);

	void rotate(NSTFormComp::Axis axis, nsfloat angle);

	void rotate(NSEntity * ent, const fvec3 & euler);

	void rotate(const fvec3 & euler);

	void rotate(NSEntity * ent, const fquat & orientation);

	void rotate(const fquat & orientation);

	void scale(NSEntity * ent, const fvec3 & pAmount);

	void scale(const fvec3 & pAmount);

	void scale(NSEntity * ent, nsfloat x, nsfloat y, nsfloat z);

	void scale(nsfloat x, nsfloat y, nsfloat z);

	bool set(NSEntity * ent, nsuint tformid);

	void setColor(const fvec4 & pColor);

	void setPickfbo(nsuint fbo);

	void setFinalfbo(nsuint fbo);

	void setHiddenState(NSTFormComp::HiddenState pState, nsbool pSet);

	void setLayer(nsint pLayer);

	void setShader(NSSelectionShader * selShader);

	void showOccupiedSpaces(bool show);

	void snap(NSEntity * ent);

	void snap();

	void snapX(NSEntity * ent);

	void snapX();

	void snapY(NSEntity * ent);

	void snapY();

	void snapZ(NSEntity * ent);

	void snapZ();

	void tileswap(NSEntity * newtile);

	void translate(NSEntity * ent, const fvec3 & pAmount);

	void translate(const fvec3 & pAmount);

	void translate(nsfloat x, nsfloat y, nsfloat z);

	void translate(NSEntity * ent, nsfloat x, nsfloat y, nsfloat z);

	void translate(NSEntity * ent, NSTFormComp::DirVec pDir, nsfloat pAmount);

	void translate(NSTFormComp::DirVec pDir, nsfloat pAmount);

	void translate(NSEntity * ent, NSTFormComp::Axis pDir, nsfloat pAmount);

	void translate(NSTFormComp::Axis pDir, nsfloat pAmount);

	virtual void update();

protected:

	enum InputTriggers
	{
		SelectEntity,
		MultiSelect,
		ShiftSelect,
		MoveSelection,
		MoveSelectionXY,
		MoveSelectionZY,
		MoveSelectionZX,
		MoveSelectionX,
		MoveSelectionY,
		MoveSelectionZ,
		MoveSelectionToggle
	};
   	
	bool _handleActionEvent(NSActionEvent * evnt);
	bool _handleStateEvent(NSStateEvent * evnt);

	void _reset_focus(const uivec3 & pickid);

	virtual void _onRotateX(
		NSEntity * ent,
		bool pPressed
		);

	virtual void _onRotateY(
		NSEntity * ent,
		bool pPressed
		);

	virtual void _onRotateZ(
		NSEntity * ent,
		bool pPressed
		);

	virtual void _onSelect(
		NSEntity * ent,
		nsbool pPressed,
		const uivec3 & pID,
		bool pSnapZOnly = false
		);

	virtual void _onMultiSelect(
		NSEntity * ent,
		nsbool pPressed,
		const uivec3 & pID
		);

	virtual void _onPaintSelect(
		NSEntity * ent,
		const fvec2 & pPos
		);

	virtual void _onDragObject(
		NSEntity * ent,
		const fvec2 & pDelta,
		nsusint _axis
		);

	void _drawOcc();
	void _drawEntOcc(NSEntity * ent);
	void _drawHidden();

	uivec3 mFocusEnt; //!< The entity/tform ID that the selection is focused on (the center of rotation)
	nspentityset mSelectedEnts;
	NSSelectionShader * selShader;
	fvec3 mTotalFrameTranslation;
	bool mMoving;
	bool mToggleMove;
	bool mSendFocEvent;
	
	fvec2 mPickPos;
	fvec3 mCachedPoint;
	bool mLayerMode;
	bool drawOcc;
	nsint mLayer;
	fvec2 mCachedLPoint;
	nsuint finalBuf;
	nsuint pickBuf;
	fmat4 trans;
};

#endif
