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

#include <nssystem.h>
#include <nsglobal.h>
#include <nsmath.h>

class NSCamComp;
class NSTFormComp;

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
		Normal,
		Top, /*!< Top down camera view */
		Iso, /*!< Isometric camera view */
		Front /*!< Front on camera view */
	};

	NSCameraSystem();
	~NSCameraSystem();

	void changeSensitivity(float pAmount, const Sensitivity & pWhich);

	virtual bool handleEvent(NSEvent * pEvent);

	void init();

	virtual void onCamBackward(NSCamComp * pCam, nsbool pAnimate);

	virtual void onCamForward(NSCamComp * pCam, nsbool pAnimate);

	virtual void onCamLeft(NSCamComp * pCam, nsbool pAnimate);

	virtual void onCamMove(NSCamComp * pCam, NSTFormComp * tComp, const fvec2 & pDelta, const fvec2 & pPos);

	virtual void onCamRight(NSCamComp * pCam, nsbool pAnimate);

	virtual void onCamTurn(NSCamComp * pCam, NSTFormComp * tComp, const fvec2 & pDelta, const fvec2 & pPos);

	virtual void onCamZoom(NSCamComp * pCam, NSTFormComp * tComp, nsfloat pScroll);

	virtual void update();

	const CameraMode & mode() const;

	const nsfloat & sensitivity(const Sensitivity & pWhich) const;

	const CameraView & view() const;

	nsfloat zoom() const;

	void setSensitivity(float pSensitivity, const Sensitivity & pWhich);

	void setMode(CameraMode pMode);

	void setView(CameraView pView);

	bool xinv(const CameraMode & pMode = Free);

	bool yinv(const CameraMode & pMode = Free);

	void setxinv(bool pInvert, const CameraMode & pMode=Free);

	void setyinv(bool pInvert, const CameraMode & pMode = Free);

	void setZoom(nsfloat pZFactor);

	void toggleMode();

	virtual float updatePriority();

	virtual nsstring typeString() { return getTypeString(); }

	static nsstring getTypeString();

private:
	nsfloat mZoomFactor;
	nsfloat mTurnSensitivity;
	nsfloat mStrafeSensitivity;

	ivec2 mFreeModeInverted;
	ivec2 mFocusModeInverted;

	CameraMode mMode;
	CameraView mView;
};

#endif