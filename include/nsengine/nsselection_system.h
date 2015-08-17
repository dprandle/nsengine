/*!
\file nsselection_system.h

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
#include <nstform_comp.h>
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

	bool add(NSEntity * ent, uint32 tformid);

	bool addToGrid();

	void changeLayer(int32 pChange);

	bool collision();

	void del();

	void clear();

	bool contains(const uivec3 & itemid);

	virtual void draw();

	bool empty();

	void enableLayerMode(const bool & pMode);

	int32 layer() const;

	const uivec3 & center();

	virtual int32 drawPriority();

	virtual int32 updatePriority();

//	virtual bool handleEvent(NSEvent * pEvent);

	uivec3 pick(float mousex, float mousey);

	uivec3 pick(const fvec2 & mpos);

	virtual void init();

	bool layerMode() const;

	bool brushValid();

	bool tileSwapValid();

	void remove(NSEntity * ent, uint32 pTFormID);

	void removeFromGrid();

	void resetColor();

	void rotate(NSEntity * ent, const fvec4 & axisangle);

	void rotate(const fvec4 & axisangle);

	void rotate(NSEntity * ent, NSTFormComp::DirVec axis, float angle);

	void rotate(NSTFormComp::DirVec axis, float angle);

	void rotate(NSEntity * ent, NSTFormComp::Axis axis, float angle);

	void rotate(NSTFormComp::Axis axis, float angle);

	void rotate(NSEntity * ent, const fvec3 & euler);

	void rotate(const fvec3 & euler);

	void rotate(NSEntity * ent, const fquat & orientation);

	void rotate(const fquat & orientation);

	void scale(NSEntity * ent, const fvec3 & pAmount);

	void scale(const fvec3 & pAmount);

	void scale(NSEntity * ent, float x, float y, float z);

	void scale(float x, float y, float z);

	bool set(NSEntity * ent, uint32 tformid);

	void setColor(const fvec4 & pColor);

	void setPickfbo(uint32 fbo);

	void setFinalfbo(uint32 fbo);

	void setHiddenState(NSTFormComp::HiddenState pState, bool pSet);

	void setLayer(int32 pLayer);

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

	void translate(float x, float y, float z);

	void translate(NSEntity * ent, float x, float y, float z);

	void translate(NSEntity * ent, NSTFormComp::DirVec pDir, float pAmount);

	void translate(NSTFormComp::DirVec pDir, float pAmount);

	void translate(NSEntity * ent, NSTFormComp::Axis pDir, float pAmount);

	void translate(NSTFormComp::Axis pDir, float pAmount);

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
		bool pPressed,
		const uivec3 & pID,
		bool pSnapZOnly = false
		);

	virtual void _onMultiSelect(
		NSEntity * ent,
		bool pPressed,
		const uivec3 & pID
		);

	virtual void _onPaintSelect(
		NSEntity * ent,
		const fvec2 & pPos
		);

	virtual void _onDragObject(
		NSEntity * ent,
		const fvec2 & pDelta,
		uint16 _axis
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
	int32 mLayer;
	fvec2 mCachedLPoint;
	uint32 finalBuf;
	uint32 pickBuf;
	fmat4 trans;
};

#endif
