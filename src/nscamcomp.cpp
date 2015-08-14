/*!
\file nscamcomp.cpp

\brief Definition file for NSCamComp class

This file contains all of the neccessary definitions for the NSCamComp class.

\author Daniel Randle
\date December 17 2013
\copywrite Earth Banana Games 2013
*/

#include <nscamcomp.h>
#include <nsentity.h>
#include <nstimer.h>
#include <nstformcomp.h>

/*
class NSCamComp

This class is implemented by keeping track of the camera. It uses the transform component
of the reference owner to manipulate its position and look angle etc.
The camera component will only work correctly after the base entity has been added to the scene - if
most of these functions are called without the entity being added to the scene (or without a reference owner assigned
to the component) they will not work.

Sensitivity effects the mouse movement speed of the camera and speed effects
the animation speed of movement.

Every frame (or update call on camera) the camera's timer has a certain value
that is compared to the timers value of the previous frame. This time difference
dt is multiplied by the speed to get the movement for that frame. Therefor,
speed units are simply units per second.
*/

NSCamComp::NSCamComp():
mFlying(),
mStrafing(),
mElevating(),
mFocPoint(),
mFocRot(),
mSpeed(DEFAULT_CAM_SPEED),
mFovAngle(0.0f),
mPersNFClip(),
mProjMode(Persp),
mDim(),
mFocTForm(),
NSComponent()
{}

NSCamComp::~NSCamComp()
{}

void NSCamComp::changeSpeed(float pAmount)
{
	mSpeed += pAmount;
	postUpdate(true);
}

NSCamComp* NSCamComp::copy(const NSComponent * pToCopy)
{
	// TODO: Fix the bla bla bla
	if (pToCopy == NULL)
		return NULL;
	const NSCamComp * comp = (const NSCamComp*)pToCopy;
	(*this) = (*comp);
	return this;
}

const fvec2 & NSCamComp::orthoTBClip()
{
	return mTBClip;
}

const fvec2 & NSCamComp::orthoLRClip()
{
	return mLRClip;
}

const fvec2 & NSCamComp::orthoNFClip()
{
	return mNFClip;
}

const fvec2 & NSCamComp::perspNFClip()
{
	return mPersNFClip;
}

void NSCamComp::setPerspNFClip(const fvec2 & nf)
{
	mPersNFClip = nf;
	_updateProj();
}

const fmat4 & NSCamComp::proj()
{
	return mProjMat;
}

float NSCamComp::fov()
{
	return mFovAngle;
}

void NSCamComp::setfov(float angledeg)
{
	mFovAngle = angledeg;
	_updateProj();
}

float NSCamComp::aspectRatio()
{
	return float(mDim.w) / float(mDim.h);
}

const fmat4 & NSCamComp::invproj()
{
	return mInverseProjMat;
}

NSCamComp::ProjectionMode NSCamComp::projectionMode()
{
	return mProjMode;
}

void NSCamComp::setOrthoTBClip(const fvec2 & tb)
{
	mTBClip = tb;
	_updateProj();
}

void NSCamComp::setOrthoLRClip(const fvec2 & lr)
{
	mLRClip = lr;
	_updateProj();
}

void NSCamComp::setOrthoNFClip(const fvec2 & nf)
{
	mNFClip = nf;
	_updateProj();
}

void NSCamComp::setProjectionMode(ProjectionMode mode)
{
	mProjMode = mode;
	_updateProj();
}

const fquat & NSCamComp::focusOrientation()
{
	return mFocRot;
}

const fvec3 & NSCamComp::focusPoint()
{
	return mFocPoint;
}

void NSCamComp::toggleProjectionMode()
{
	if (mProjMode == Persp)
		setProjectionMode(Ortho);
	else
		setProjectionMode(Persp);
}

void NSCamComp::rotateFocus(const fquat & pQuat)
{
	mFocRot *= pQuat;
	computeFocusTransform();
}

void NSCamComp::rotateFocus(float pX, float pY, float pZ, float pAngle)
{
	rotateFocus(fvec3(pX, pY, pZ), pAngle);
}

void NSCamComp::rotateFocus(const fvec3 & pAxis, float pAngle)
{
	mFocRot = orientation(fvec4(pAxis, pAngle)) * mFocRot;
	computeFocusTransform();
}

void NSCamComp::translateFocus(const fvec3 & pAmount)
{
	mFocPoint += pAmount;
	computeFocusTransform();
}

void NSCamComp::setFocusRot(const fquat & pFocRot)
{
	mFocRot = pFocRot;
}

fmat4 NSCamComp::camFocusTForm()
{
	return mFocTForm;
}

void NSCamComp::resize(int32 w, int32 h)
{
	mDim.set(w,h);
	_updateProj();
}

void NSCamComp::resize(const ivec2 & dim)
{
	mDim = dim;
	_updateProj();
}

void NSCamComp::computeFocusTransform()
{
	mFocTForm.set(rotationMat3(mFocRot));
	mFocTForm.setColumn(3, mFocPoint.x, mFocPoint.y, mFocPoint.z, 1);
	mFocTForm[3].x = 0; mFocTForm[3].y = 0; mFocTForm[3].z = 0;
}

const ivec2 & NSCamComp::dim()
{
	return mDim;
}

const NSCamComp::Movement & NSCamComp::elevate() const
{
	return mElevating;
}

const NSCamComp::Movement & NSCamComp::fly() const
{
	return mFlying;
}

float NSCamComp::speed() const
{
	return mSpeed;
}

const NSCamComp::Movement & NSCamComp::strafe() const
{
	return mStrafing;
}

void NSCamComp::init()
{}

const fmat4 & NSCamComp::projCam()
{
	return mProjCam;
}

const fmat4 & NSCamComp::invProjCam()
{
	return mInvProjCam;
}

void NSCamComp::setElevate(Direction pDir, bool pAnimate)
{
	if (mElevating.mDir != pDir)
	{
		if (!pAnimate)
			return;
		mElevating.mDir = pDir;
		mElevating.mAnimate = pAnimate;
	}
	else
		mElevating.mAnimate = pAnimate;
	postUpdate(true);
}

void NSCamComp::pup(NSFilePUPer * p)
{
	if (p->type() == NSFilePUPer::Binary)
	{
		NSBinFilePUPer * bf = static_cast<NSBinFilePUPer *>(p);
		::pup(*bf, *this);
	}
	else
	{
		NSTextFilePUPer * tf = static_cast<NSTextFilePUPer *>(p);
		::pup(*tf, *this);
	}
}

void NSCamComp::setFly(Direction pDir, bool pAnimate)
{
	if (mFlying.mDir != pDir)
	{
		if (!pAnimate)
			return;
		mFlying.mDir = pDir;
		mFlying.mAnimate = pAnimate;
	}
	else
		mFlying.mAnimate = pAnimate;
	postUpdate(true);
}

void NSCamComp::setFocusPoint(const fvec3 & pFocPoint)
{
	mFocPoint = pFocPoint;
	computeFocusTransform();
	postUpdate(true);
}

void NSCamComp::setSpeed(float pUnitsPerSecond)
{
	mSpeed = pUnitsPerSecond;
	postUpdate(true);
}

void NSCamComp::setStrafe(Direction pDir, bool pAnimate)
{
	if (mStrafing.mDir != pDir)
	{
		if (!pAnimate)
			return;
		mStrafing.mDir = pDir;
		mStrafing.mAnimate = pAnimate;
	}
	else
		mStrafing.mAnimate = pAnimate;

	postUpdate(true);
}

NSCamComp & NSCamComp::operator=(const NSCamComp & pRHSComp)
{
	mFlying = pRHSComp.mFlying;
	mElevating = pRHSComp.mElevating;
	mStrafing = pRHSComp.mStrafing;
	mSpeed = pRHSComp.mSpeed;
	mFocPoint = pRHSComp.mFocPoint;
	mFocRot = pRHSComp.mFocRot;
	postUpdate(true);
	return (*this);
}

void NSCamComp::_updateProj()
{
	if (mProjMode == Persp)
		mProjMat = perspective(mFovAngle, aspectRatio(), mPersNFClip.x, mPersNFClip.y);
	else
		mProjMat = ortho(mLRClip.x, mLRClip.y, mTBClip.x, mTBClip.y, mNFClip.x, mNFClip.y);
	mInverseProjMat = inverse(mProjMat);
	postUpdate(true);
}
