/*!
\file nslight_comp.h

\brief Header file for NSLightComp class

This file contains all of the neccessary declarations for the NSLightComp class.

\author Daniel Randle
\date January 27 2013
\copywrite Earth Banana Games 2013
*/

#ifndef NSLIGHTCOMPONENT_H
#define NSLIGHTCOMPONENT_H

#define SHADOW_LOWER_LIMIT 0.0
#define SHADOW_UPPER_LIMIT 1.0
#define LIGHT_CUTOFF 0.1f

#include <nscomponent.h>
#include <nsglobal.h>
#include <nsmath.h>
#include <nstform_comp.h>

class nsmesh;
class nsentity;
class NSTimer;

class NSLightComp : public NSComponent
{
public:
	template <class PUPer>
	friend void pup(PUPer & p, NSLightComp & lc);

	enum LightType {
		Directional,
		Point,
		Spot
	};

	enum Adjustment {
		Linear,
		Exponential
	};

	NSLightComp();
	~NSLightComp();

	void init();

	bool castShadows() const;

	void changeAtten(float constant, float lin, float exp);

	void changeColor(float pRedAmount, float pGreenAmount, float pBlueAmount);

	void changeAngle(float pAmount);

	void changeCutoff(float pAmount);

	void changeDistance(float pAmount, Adjustment pAdjustment = Exponential);

	void changeIntensity(float diffuse, float ambient, Adjustment pAdjustment=Exponential);

	void changeShadowDarkness(float pAmount);

	void changeRadius(float pAmount);

	NSLightComp* copy(const NSComponent* pToCopy);

	const fvec3 & atten() const;

	const uivec2 & meshid() const;

	const fvec3 & color() const;

	float angle() const;

	float cutoff() const;

	float distance();

	LightType type() const;

	const fvec2 & intensity() const;

	const fmat4 & pov(uint32 pIndex) const;

	float radius() const;

	const fvec3 & scaling() const;

	virtual void pup(NSFilePUPer * p);

	float shadowDarkness() const;

	const int32 & shadowSamples() const;

	const fvec2 & shadowClipping() const { return mshadowclip; }

	const fmat4 & transform(uint32 pIndex);

	fmat4array & transforms();

	virtual void name_change(const uivec2 & oldid, const uivec2 newid);

	/*!
	Get the resources that the component uses. The light comp uses a bounding mesh.
	/return Map of resource ID to resource type containing bounding mesh ID.
	*/
	virtual uivec2array resources();

	void setAtten(const fvec3 & pAttComp);

	void setAtten(float pConst, float pLin, float pExp);

	void setMeshID(uint32 plugid, uint32 resid)
	{
		mBoundingMeshID.x = plugid; mBoundingMeshID.y = resid;
		postUpdate(true);
	}

	void setShadowClipping(const fvec2 & shadowclip) { mshadowclip = shadowclip; postUpdate(true); }

	void setMeshID(const uivec2 & pID);

	void setCastShadows(bool pCastShadows);

	void setColor(const fvec3 & pColor);

	void setColor(float pRed, float pGreen, float pBlue);

	void setCutoff(float cutoff);

	void setAngle(float angle);

	void setDistance(float pDistance, Adjustment pAdjustment = Exponential);

	void setType(LightType pType);

	void setIntensity(const fvec2 & intensity, Adjustment pAdjustment = Exponential);

	void setIntensity(float pDiffuse, float pAmbient, Adjustment pAdjustment = Exponential);

	void setRadius(float pRandius);

	void setShadowDarkness(float pVal);

	void setShadowSamples(int32 pNumSamples);

	NSLightComp & operator=(const NSLightComp & pRHSComp);

private:
	void _updateMeshRadius();
	void _updateMeshLength();
	void _updateAttComp(Adjustment pAdjustment);
	fmat4 tmpret;
	LightType mLightType;
	fvec3 mAttComp;
	fvec2 mIntensityComp;
	float mShadowDarkness;
	float mAngle;
	fvec3 mColor;
	bool mCastShadows;
	int32 mShadowSamples;
	uivec2 mBoundingMeshID;
	fvec3 mScaling;
	fvec2 mshadowclip;
};

template <class PUPer>
void pup(PUPer & p, NSLightComp & lc)
{
	pup(p, lc.mLightType, "lighttype");
	pup(p, lc.mAttComp, "attComp");
	pup(p, lc.mIntensityComp, "intensityComp");
	pup(p, lc.mShadowDarkness, "shadowDarkness");
	pup(p, lc.mAngle, "angle");
	pup(p, lc.mColor, "color");
	pup(p, lc.mCastShadows, "castShadows");
	pup(p, lc.mShadowSamples, "shadowSamples");
	pup(p, lc.mBoundingMeshID, "boundingMeshID");
	pup(p, lc.mScaling, "scaling");
	lc.postUpdate(true);
}

template<class PUPer>
void pup(PUPer & p, NSLightComp::LightType & en, const nsstring & pString)
{
	uint32 in = static_cast<uint32>(en);
	pup(p, in, pString);
	en = static_cast<NSLightComp::LightType>(in);
}

#endif // !NSLIGHTCOMPONENT_H
