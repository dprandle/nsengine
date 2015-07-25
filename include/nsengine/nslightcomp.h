/*!
\file nslightcomp.h

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
#include <nstformcomp.h>

class NSMesh;
class NSEntity;
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

	void changeAtten(nsfloat constant, nsfloat lin, nsfloat exp);

	void changeColor(nsfloat pRedAmount, nsfloat pGreenAmount, nsfloat pBlueAmount);

	void changeAngle(nsfloat pAmount);

	void changeCutoff(nsfloat pAmount);

	void changeDistance(nsfloat pAmount, Adjustment pAdjustment = Exponential);

	void changeIntensity(nsfloat diffuse, nsfloat ambient, Adjustment pAdjustment=Exponential);

	void changeShadowDarkness(nsfloat pAmount);

	void changeRadius(nsfloat pAmount);

	NSLightComp* copy(const NSComponent* pToCopy);

	const fvec3 & atten() const;

	const uivec2 & meshid() const;

	const fvec3 & color() const;

	nsfloat angle() const;

	nsfloat cutoff() const;

	nsfloat distance();

	LightType type() const;

	const fvec2 & intensity() const;

	const fmat4 & pov(nsuint pIndex) const;

	nsfloat radius() const;

	const fvec3 & scaling() const;

	virtual void pup(NSFilePUPer * p);

	nsfloat shadowDarkness() const;

	const nsint & shadowSamples() const;

	const fvec2 & shadowClipping() const { return mshadowclip; }

	const fmat4 & transform(nsuint pIndex);

	fmat4array & transforms();

	virtual void nameChange(const uivec2 & oldid, const uivec2 newid);

	/*!
	Get the resources that the component uses. The light comp uses a bounding mesh.
	/return Map of resource ID to resource type containing bounding mesh ID.
	*/
	virtual uivec2array resources();

	void setAtten(const fvec3 & pAttComp);

	void setAtten(nsfloat pConst, nsfloat pLin, nsfloat pExp);

	void setMeshID(nsuint plugid, nsuint resid)
	{
		mBoundingMeshID.x = plugid; mBoundingMeshID.y = resid;
		postUpdate(true);
	}

	void setShadowClipping(const fvec2 & shadowclip) { mshadowclip = shadowclip; postUpdate(true); }

	void setMeshID(const uivec2 & pID);

	void setCastShadows(bool pCastShadows);

	void setColor(const fvec3 & pColor);

	void setColor(nsfloat pRed, nsfloat pGreen, nsfloat pBlue);

	void setCutoff(nsfloat cutoff);

	void setAngle(nsfloat angle);

	void setDistance(nsfloat pDistance, Adjustment pAdjustment = Exponential);

	void setType(LightType pType);

	void setIntensity(const fvec2 & intensity, Adjustment pAdjustment = Exponential);

	void setIntensity(nsfloat pDiffuse, nsfloat pAmbient, Adjustment pAdjustment = Exponential);

	void setRadius(nsfloat pRandius);

	void setShadowDarkness(nsfloat pVal);

	void setShadowSamples(nsint pNumSamples);

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
	nsfloat mAngle;
	fvec3 mColor;
	bool mCastShadows;
	nsint mShadowSamples;
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
	nsuint in = static_cast<nsuint>(en);
	pup(p, in, pString);
	en = static_cast<NSLightComp::LightType>(in);
}

#endif // !NSLIGHTCOMPONENT_H
