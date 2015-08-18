/*!
\file nscam_comp.h

\brief Header file for NSCamComp class

This file contains all of the neccessary declarations for the NSCamComp class.

\author Daniel Randle
\date December 17 2013
\copywrite Earth Banana Games 2013
*/

#ifndef NSCAMCOMP_H
#define NSCAMCOMP_H


#include <nscomponent.h>
#include <nsmath.h>
#include <nspupper.h>

class nsentity;
class NSTimer;

/*!
\bug No known bugs.
*/
class NSCamComp : public NSComponent
{
public:
	friend class NSCameraSystem;
	/*!
	Assigns integers to different travel directions

	Basically the amount to move either needs to be positive
	or negative depending on the direction - this enum takes care of that.
	*/
	enum Direction {
		DirNone = 0,
		DirPos = 1, /*!< Positive movement */
		DirNeg = -1 /*!< Negative movement */
	};

	struct Movement
	{
		Movement() :mDir(DirNone), mAnimate(false){}
		int32 mDir;
		bool mAnimate;
	};

	enum CameraView {
		Normal,
		Top, /*!< Top down camera view */
		Iso, /*!< Isometric camera view */
		Front /*!< Front on camera view */
	};

	enum ProjectionMode {
		Persp,
		Ortho
	};

	template <class PUPer>
	friend void pup(PUPer & p, NSCamComp & cc);

	NSCamComp();
	~NSCamComp();

	/*!
	Change camera speed in units per second
	\param
	pAmount The amount to change the speed by (can be positive or negative)
	\return
	void
	*/
	void changeSpeed(float pAmount);

	void computeFocusTransform();

	virtual NSCamComp* copy(const NSComponent* pToCopy);

	/*!
	Gets camera elevating direction

	\param
	NONE
	\return
	Camera elevating direction
	*/
	const Movement & elevate() const;

	/*!
	Gets camera flying direction

	\param
	NONE
	\return
	Camera flying direction
	*/
	const Movement & fly() const;

	/*!
	Gets the current focus point of the camera - the point the camera will set as its parent transform

	\param
	NONE
	\return
	The focus point of the camer for Focus mode
	*/
	const fvec3 & focusPoint();

	fmat4 camFocusTForm();

	const fquat & focusOrientation();



	/*!
	Get the current speed of the camera in units per second.

	\param
	NONE
	\return
	A floating point number representing the camera's speed.
	*/
	float speed() const;

	void toggleProjectionMode();

	/*!
	Gets camera strafing direction

	\param
	NONE
	\return
	Camera strafing direction
	*/
	const Movement & strafe() const;


	/*!
	Component initializer

	\param
	None
	\return
	void
	*/
	void init();

	float aspectRatio();

	const fvec2 & orthoTBClip();

	const fvec2 & orthoLRClip();

	const fvec2 & orthoNFClip();

	const fvec2 & perspNFClip();

	ProjectionMode projectionMode();

	const ivec2 & dim();
	
	virtual void pup(NSFilePUPer * p);

	/*!
	When this function is called the camera will begin elevating or dropping
	at the speed given by getSpeed()

	\param pDir Determines camera movement direction

	\return
	void
	*/
	void setElevate(Direction pDir, bool pAnimate);

	void setOrthoTBClip(const fvec2 & tb);

	void setOrthoLRClip(const fvec2 & lr);

	void setOrthoNFClip(const fvec2 & nf);

	void setPerspNFClip(const fvec2 & nf);

	void setProjectionMode(ProjectionMode mode);

	/*!
	On calling this function the camera will continue moving in the pDir
	direction until called with pDir = None

	\param pDir Determines camera movement direction

	\return
	void
	*/
	void setFly(Direction pDir, bool pAnimate);

	void setfov(float angledeg);

	/*!
	Sets the focus point that the camera will rotate around in focus mode. Call this
	function using the center point of an object you wish the camera to be focused on.
	Only effects the camera in focus mode.

	\param
	pFocPoint The point used to set the camera's focus point.
	\return
	void
	*/
	void setFocusPoint(const fvec3 & pFocPoint);

	void setFocusRot(const fquat & pFocRot);

	const fmat4 & proj();

	const fmat4 & projCam();

	const fmat4 & invProjCam();

	const fmat4 & invproj();

	float fov();


	/*!
	Sets the speed of the camera in units per second.

	\param
	pUnitsPerSecond The number of units per second the camera should travel in the forward direction.
	\return
	void
	*/
	void setSpeed(float pUnitsPerSecond);

	/*!
	Begins the camera strafing - this means translating to the left or right
	according to the sensitivity - call with pDir = None to stop strafing

	\param
	pDir Specify which direction the camera should strafe
	\return
	void
	*/
	void setStrafe(Direction pDir, bool pAnimate);

	void resize(int32 w, int32 h);

	void resize(const ivec2 & dimen);

	void rotateFocus(const fquat & pQuat);

	void rotateFocus(float pX, float pY, float pZ, float pAngle);

	void rotateFocus(const fvec3 & pAxis, float pAngle);

	void translateFocus(const fvec3 & pAmount);

	NSCamComp & operator=(const NSCamComp & pRHSComp);

private:
	void _updateProj();

	Movement mFlying;		        // which direction the camera is currently traveling
	Movement mStrafing;		    // if any of these are non-zero it means the camera is
	Movement mElevating;			// moving in a direction

	float mSpeed;				    // speed in units per second used in animation

	fvec3 mFocPoint;               // Basically the camera's parent position - camera has a special type of parent though
	fquat mFocRot;		    // Has the parent orientation for focusing

	float mFovAngle;
	fvec2 mPersNFClip;
	ivec2 mDim;

	fvec2 mTBClip;
	fvec2 mLRClip;
	fvec2 mNFClip;

	ProjectionMode mProjMode;

	fmat4 mProjMat;
	fmat4 mInverseProjMat;
	fmat4 mProjCam;
	fmat4 mInvProjCam;
	fmat4 mFocTForm;
};

template <class PUPer>
void pup(PUPer & p, NSCamComp & cc)
{
	pup(p, cc.mFlying, "flying");
	pup(p, cc.mStrafing, "strafing");
	pup(p, cc.mElevating, "elevating");
	pup(p, cc.mSpeed, "speed");
	pup(p, cc.mFocPoint, "focPoint");
	pup(p, cc.mFocRot, "focRot");
	pup(p, cc.mFocTForm, "focTForm");

	pup(p, cc.mFovAngle, "fovAngle");
	pup(p, cc.mPersNFClip, "PpersNFClip");
	pup(p, cc.mDim, "dim");

	pup(p, cc.mTBClip, "TBClip");
	pup(p, cc.mLRClip, "LRClip");
	pup(p, cc.mNFClip, "NFClip");
	pup(p, cc.mProjMode, "NFClip");

	cc._updateProj();
}

template <class PUPer>
void pup(PUPer & p, NSCamComp::Movement & cc, const nsstring & varName)
{
	pup(p, cc.mDir, "Dir");
	pup(p, cc.mAnimate, "Animate");
}

template<class PUPer>
void pup(PUPer & p, NSCamComp::ProjectionMode & en, const nsstring & pString)
{
	uint32 in = static_cast<uint32>(en);
	pup(p, in, pString);
	en = static_cast<NSCamComp::ProjectionMode>(in);
}

#endif
