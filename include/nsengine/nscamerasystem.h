/*!
\file nscamerasystem.h

\brief Header file for NSCameraSystem class

This file contains all of the neccessary declarations for the NSCameraSystem class.

\author Daniel Randle
\date December 17 2013
\copywrite Earth Banana Games 2013
*/

#ifndef NSCAMERASYSTEM_H
#define NSCAMERASYSTEM_H

#define NSCAM_FORWARD "CameraForward"
#define NSCAM_BACKWARD "CameraBackward"
#define NSCAM_LEFT "CameraLeft"
#define NSCAM_RIGHT "CameraRight"
#define NSCAM_TILTPAN "CameraTiltPan"
#define NSCAM_MOVE "CameraMove"
#define NSCAM_ZOOM "CameraZoom"
#define NSCAM_TOPVIEW "CameraTopView"
#define NSCAM_ISOVIEW "CameraIsoView"
#define NSCAM_FRONTVIEW "CameraFrontView"
#define NSCAM_TOGGLEMODE "CameraToggleMode"

#include <nssystem.h>
#include <nsglobal.h>
#include <nsmath.h>

class NSCamComp;
class NSTFormComp;
class NSActionEvent;
class NSStateEvent;

class NSCameraSystem : public NSSystem
{
public:

	enum CameraMode {
		Free, /*!< Free mode - camera moves as fps */
		Focus /*!< Focus mode - camera focuses on single object */
	};

	enum Sensitivity
	{
		Turn,
		Strafe
	};

	/*!
	Enum holds 3 different pre-set camera view identifiers
	*/
	enum CameraView {
		Top, /*!< Top down camera view */
		Iso, /*!< Isometric camera view */
		Front /*!< Front on camera view */
	};

	NSCameraSystem();
	~NSCameraSystem();

	void changeSensitivity(float pAmount, const Sensitivity & pWhich);

	//virtual bool handleEvent(NSEvent * pEvent);

	void init();

	virtual void update();

	const CameraMode & mode() const;

	const float & sensitivity(const Sensitivity & pWhich) const;

	float zoom() const;

	void setSensitivity(float pSensitivity, const Sensitivity & pWhich);

	void setMode(CameraMode pMode);

	void setView(CameraView pView);

	bool xinv(const CameraMode & pMode = Free);

	bool yinv(const CameraMode & pMode = Free);

	void setxinv(bool pInvert, const CameraMode & pMode=Free);

	void setyinv(bool pInvert, const CameraMode & pMode = Free);

	void setZoom(float pZFactor);

	void toggleMode();

	virtual int32 updatePriority();

private:

	enum InputTriggers
	{
		CameraForward,
		CameraBackward,
		CameraLeft,
		CameraRight,
		CameraTiltPan,
		CameraMove,
		CameraZoom,
		CameraTopView,
		CameraIsoView,
		CameraFrontView,
		CameraToggleMode
	};

    void _onCamTurn(NSCamComp * pCam, NSTFormComp * tComp, const fvec2 & pDelta);
	void _onCamMove(NSCamComp * pCam, NSTFormComp * tComp, const fvec2 & pDelta);
	void _onCamZoom(NSCamComp * pCam, NSTFormComp * tComp, float pScroll);
	
	bool _handleActionEvent(NSActionEvent * evnt);
	bool _handleStateEvent(NSStateEvent * evnt);
	bool _handleSelFocusEvent(NSSelFocusEvent * evnt);
	
	float mZoomFactor;
	float mTurnSensitivity;
	float mStrafeSensitivity;

	float animTime;
	float anim_elapsed;
	fquat startOrient, finalOrient;
	fvec3 startPos, finalPos;
	bool anim_view, switch_back;

	ivec2 mFreeModeInverted;
	ivec2 mFocusModeInverted;

	uivec3 mFocusEnt;

	CameraMode mMode;
};

#endif
