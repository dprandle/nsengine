

/*!
\file nslightcomp.cpp

\brief Definition file for NSLightComp class

This file contains all of the neccessary definitions for the NSLightComp class.

\author Daniel Randle
\date January 27 2013
\copywrite Earth Banana Games 2013
*/

#include <nslightcomp.h>
#include <nsmesh.h>
#include <nstimer.h>
#include <nsentity.h>
#include <nstformcomp.h>
#include <nsengine.h>
#include <nsmeshmanager.h>

NSLightComp::NSLightComp() : 
mLightType(Point),
mAttComp(),
mIntensityComp(),
mShadowDarkness(1.0f),
mAngle(),
mColor(1.0f, 1.0f, 1.0f),
mCastShadows(true),
mShadowSamples(0),
mBoundingMeshID(0),
mScaling(1.0f, 1.0f, 1.0f),
mshadowclip(DEFAULT_Z_NEAR, DEFAULT_Z_FAR),
NSComponent()
{}

NSLightComp::~NSLightComp()
{}

NSLightComp* NSLightComp::copy(const NSComponent * pToCopy)
{
	if (pToCopy == NULL)
		return NULL;
	const NSLightComp * comp = (const NSLightComp*)pToCopy;
	(*this) = (*comp);
	return this;
}

void NSLightComp::init()
{}

bool NSLightComp::castShadows() const
{
	return mCastShadows;
}

void NSLightComp::changeAtten(float constant, float lin, float exp)
{
	setAtten(mAttComp + fvec3(constant,lin,exp));
}

void NSLightComp::changeColor(float pRedAmount, float pGreenAmount, float pBlueAmount)
{
	mColor += fvec3(pRedAmount, pGreenAmount, pBlueAmount);
	postUpdate(true);
}

void NSLightComp::changeAngle(float pAmount)
{
	setAngle(mAngle + pAmount);
}

void NSLightComp::changeCutoff(float pAmount)
{
	float cut = cutoff();
	cut += pAmount;
	cut = clampf(cut, -1.0f, 1.0f);
	setCutoff(cut);
}

void NSLightComp::changeDistance(float pAmount, Adjustment pAdjustment)
{
	setDistance(mScaling.z + pAmount, pAdjustment);
}

void NSLightComp::changeIntensity(float diffuse, float ambient, Adjustment pAdjustment)
{
	setIntensity(mIntensityComp + fvec2(diffuse, ambient), pAdjustment);
}

void NSLightComp::changeRadius(float pAmount)
{
	setRadius(mScaling.x + pAmount);
}

void NSLightComp::changeShadowDarkness(float pAmount)
{
	if (pAmount+mShadowDarkness < SHADOW_LOWER_LIMIT)
		mShadowDarkness = 0.0f;
	else if (pAmount+mShadowDarkness > SHADOW_UPPER_LIMIT)
		mShadowDarkness = 1.0f;
	else
		mShadowDarkness += pAmount;
}

const fvec3 & NSLightComp::atten() const
{
	return mAttComp;
}

const uivec2 & NSLightComp::meshid() const
{
	return mBoundingMeshID;
}

void NSLightComp::nameChange(const uivec2 & oldid, const uivec2 newid)
{
	if (mBoundingMeshID.x == oldid.x)
	{
		mBoundingMeshID.x = newid.x;
		if (mBoundingMeshID.y == oldid.y)
			mBoundingMeshID.y = newid.y;
	}
}

const fvec3 & NSLightComp::color() const
{
	return mColor;
}

float NSLightComp::angle() const
{
	return mAngle;
}


float NSLightComp::cutoff() const
{
	return cosf(radians(mAngle));
}

float NSLightComp::distance()
{
	return mScaling.z;
}

NSLightComp::LightType NSLightComp::type() const
{
	return mLightType;
}

const fvec2 & NSLightComp::intensity() const
{
	return mIntensityComp;
}

float NSLightComp::radius() const
{
	return mScaling.x;
}

const fvec3 & NSLightComp::scaling() const
{
	return mScaling;
}

float NSLightComp::shadowDarkness() const
{
	return mShadowDarkness;
}

const fmat4 & NSLightComp::pov(uint32 pIndex) const
{
	return mOwner->get<NSTFormComp>()->pov(pIndex);
}

const int32 & NSLightComp::shadowSamples() const
{
	return mShadowSamples;
}

const fmat4 & NSLightComp::transform(uint32 pIndex)
{
	NSTFormComp * tc = mOwner->get<NSTFormComp>();
	if (tc == NULL)
		return tmpret.setIdentity();
	tmpret.set(rotationMat3(tc->orientation(pIndex)) % mScaling);
	tmpret.setColumn(3, tc->lpos(pIndex).x, tc->lpos(pIndex).y, tc->lpos(pIndex).z, 1);
	return tmpret;
}

/*!
Get the resources that the component uses. The light comp uses a bounding mesh.
*/
uivec2array NSLightComp::resources()
{
	// Build map
	uivec2array ret;

	// only add if not 0
	if (mBoundingMeshID != 0)
		ret.push_back(mBoundingMeshID);

	return ret;
}

void NSLightComp::setAtten(const fvec3 & pAttComp)
{
	mAttComp = pAttComp;
	_updateMeshLength();
	_updateMeshRadius();
	postUpdate(true);
}

void NSLightComp::pup(NSFilePUPer * p)
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

void NSLightComp::setAtten(float pConst, float pLin, float pExp)
{
	mAttComp.set(pConst, pLin, pExp);
	_updateMeshLength();
	_updateMeshRadius();
	postUpdate(true);
}

void NSLightComp::setMeshID(const uivec2 & pID)
{
	mBoundingMeshID = pID;
	postUpdate(true);
}

void NSLightComp::setCastShadows(bool pCastShadows)
{
	mCastShadows = pCastShadows;
	postUpdate(true);
}

void NSLightComp::setColor(const fvec3 & pColor)
{
	mColor = pColor;
	postUpdate(true);
}

void NSLightComp::setColor(float pRed, float pGreen, float pBlue)
{
	setColor(fvec3(pRed, pGreen, pBlue));
}

void NSLightComp::setAngle(float angle)
{
	mAngle = clampf(angle,0.0f,180.0f);
	_updateMeshRadius();
	postUpdate(true);
}

void NSLightComp::setCutoff(float cutoff)
{
	cutoff = clampf(cutoff, -1.0f, 1.0f);
	setAngle(degrees(acosf(cutoff)));
}

void NSLightComp::setDistance(float pDistance, Adjustment pAdjustment)
{
	if (mLightType == Spot)
	{
		mScaling.z = pDistance;
		_updateMeshRadius();
	}
	else if (mLightType == Point)
		mScaling.set(pDistance, pDistance, pDistance);

	_updateAttComp(pAdjustment);
	postUpdate(true);
}

void NSLightComp::setType(LightType pType)
{
	mLightType = pType;
	postUpdate(true);
}

void NSLightComp::setIntensity(const fvec2 & intensity, Adjustment pAdjustment)
{
	mIntensityComp = intensity;
	_updateAttComp(pAdjustment);
	postUpdate(true);
}

void NSLightComp::setIntensity(float pDiffuse, float pAmbient, Adjustment pAdjustment)
{
	setIntensity(fvec2(pDiffuse, pAmbient),pAdjustment);
}

void NSLightComp::setRadius(float pRadius)
{
	if (mLightType == Spot)
	{
		float pAngle = degrees(atan2f(pRadius, mScaling.z));
		setAngle(pAngle);
	}
	else if (mLightType == Point)
		setDistance(pRadius);
}

void NSLightComp::setShadowDarkness(float pVal)
{
	if (pVal < SHADOW_LOWER_LIMIT)
		mShadowDarkness = 0.0f;
	else if (pVal > SHADOW_UPPER_LIMIT)
		mShadowDarkness = 1.0f;
	else
		mShadowDarkness = pVal;
}

void NSLightComp::setShadowSamples(int32 pNumSamples)
{
	mShadowSamples = pNumSamples;
	postUpdate(true);
}

NSLightComp & NSLightComp::operator=(const NSLightComp & pRHSComp)
{
	mLightType = pRHSComp.mLightType;
	mAttComp = pRHSComp.mAttComp;
	mIntensityComp = pRHSComp.mIntensityComp;
	mAngle = pRHSComp.mAngle;
	mColor = pRHSComp.mColor;
	mBoundingMeshID = pRHSComp.mBoundingMeshID;
	mShadowSamples = pRHSComp.mShadowSamples;
	mShadowDarkness = pRHSComp.mShadowDarkness;
	postUpdate(true);
	return (*this);
}

void NSLightComp::_updateMeshRadius()
{
	if (mLightType == Spot)
	{
		float scaleXY = tanf(radians(mAngle)) * mScaling.z;
		mScaling.x = mScaling.y = scaleXY;
	}
}

void NSLightComp::_updateMeshLength()
{
	float c = mAttComp.x - mIntensityComp.x / (LIGHT_CUTOFF);
	float b = mAttComp.y;
	float a = mAttComp.z;
	float ans = (-b* + sqrt(b*b - 4*a*c)) / (2 * a);
	if (mLightType == Spot)
		mScaling.z = ans;
	else if (mLightType == Point)
		mScaling.set(ans, ans, ans);
}

void NSLightComp::_updateAttComp(Adjustment pAdjustment)
{
	if (pAdjustment == Linear)
		mAttComp.y = 1.0f / (LIGHT_CUTOFF * mScaling.z) - mAttComp.z * mScaling.z - mAttComp.x / mScaling.z;
	else
		mAttComp.z = 1.0f / (LIGHT_CUTOFF * mScaling.z * mScaling.z) - mAttComp.y / mScaling.z - mAttComp.x / (mScaling.z * mScaling.z);
}
