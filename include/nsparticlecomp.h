/*!
\file nsparticlecomp.h

\brief Header file for NSParticleComp class

This file contains all of the neccessary declarations for the NSParticleComp class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#ifndef NSPARTICLECOMP_H
#define NSPARTICLECOMP_H

#include <nscomponent.h>
#include <nsmath.h>

class NSEntity;
class NSTransformFeedbackObject;
class NSVertexArrayObject;
class NSBufferObject;
struct NSEvent;

class NSParticleComp : public NSComponent
{
public:

	enum EmitterShape
	{
		Cube,
		Ellipse
	};

	enum MotionKeyType
	{
		Velocity,
		Acceleration
	};

	enum BlendMode
	{
		Mix,
		Add
	};

	struct Particle
	{
		fvec4 mPos;
		fvec4 mVel;
		fvec4 mScaleAndAngle;
		fvec4 mAgeTypeReserved;
	};

	typedef std::vector<Particle> ParticleArray;

	template <class PUPer>
	friend void pup(PUPer & p, NSParticleComp & pc);

	friend class NSParticleSystem;
	
	NSParticleComp();

	virtual ~NSParticleComp();

	void allocateBuffers();

	virtual NSParticleComp * copy(const NSComponent* pComp);

	virtual void init();

	nsuint motionKeyCount();

	nsuint visualKeyCount();

	nsfloat & elapsed();

	bool looping();

	bool simulating();

	bool motionKeyInterpolation();

	bool visualKeyInterpolation();

	bool visualGlobalTime();

	fvec3uimap::iterator beginMotionKey();
	fvec3uimap::iterator beginVisualKey();

	fvec3uimap::iterator endMotionKey();
	fvec3uimap::iterator endVisualKey();

	void enableLooping(bool pEnable);

	void enableSimulation(bool pEnable);

	void enableMotionGlobalTime(bool pEnable);

	void enableMotionKeyInterpolation(bool pEnable);

	void enableVisualGlobalTime(bool pEnable);

	void enableVisualKeyInterpolation(bool pEnable);

	void clearMotionKeys();

	void clearVisualKeys();

	virtual void nameChange(const uivec2 & oldid, const uivec2 newid);

	/*!
	Get the resources that the component uses. If no resources are used then leave this unimplemented - will return an empty map.
	/return Map of resource ID to resource type containing all used resources
	*/
	virtual uivec2array resources();

	void setMotionKeyType(const MotionKeyType & pType);

	const MotionKeyType & motionKeyType();

	void setEmitterShape(const EmitterShape & pShape);

	const EmitterShape & emitterShape();

	NSParticleComp & operator=(const NSParticleComp & pRHSComp);

	const uivec2 & shaderID();

	const uivec2 & materialID();

	nsuint maxParticles();

	const fvec3uimap & motionKeys();

	const fvec3uimap & visualKeys();

	const fvec3 & emitterSize();

	void setEmitterSize(const fvec3 & pSize);

	nsuint maxMotionKeys();

	nsuint maxVisualKeys();

	bool motionGlobalTime();

	fvec3 motionKeyAt(float pTime);

	fvec3 visualKeyAt(float pTime);

	bool hasMotionKey(float pTime);

	bool hasVisualKey(float pTime);

	void removeMotionKey(float pTime);

	void removeVisualKey(float pTime);

	void setMaxMotionKeys(nsuint pMax);

	void setMaxVisualKeys(nsuint pMax);

	void setMotionKey(float pTime, const fvec3 & pForce);

	void setVisualKey(float pTime, const fvec3 & pRenderKey);

	nsuint lifetime();

	nsuint emissionRate();

	void release();

	bool first();

	void setFirst(bool pSet);

	virtual void pup(NSFilePUPer * p);

	NSTransformFeedbackObject * transformFeedbackObject();

	NSVertexArrayObject * vertexArrayObject();

	nsuint transformFeedbackID();

	void reset();

	const fvec3 & initVelocityMult();

	void setInitVelocityMult(const fvec3 & pMult);

	const uivec2 & randomTextureID();

	void setEmissionRate(nsuint pRate);

	void setLifetime(nsuint pLifetime);

	void setMaxParticles(nsuint pMaxParticles);

	void setShaderID(nsuint plugid, nsuint resid)
	{
		mXFBShaderID.x = plugid; mXFBShaderID.y = resid;
		postUpdate(true);
	}

	void setShaderID(const uivec2 &);

	void setMaterialID(nsuint plugid, nsuint resid)
	{
		mMatID.x = plugid; mMatID.y = resid;
		postUpdate(true);
	}

	void setMaterialID(const uivec2 & pID);

	void setRandTextureID(const uivec2 & pID);

	void setRandTextureID(nsuint plugid, nsuint resid)
	{
		mRandTextID.x = plugid; mRandTextID.y = resid;
		postUpdate(true);
	}

	void setAngularVelocity(nsint pVel);

	void setStartingSize(const fvec2 & pSize);

	void setStartingSize(float pWidth, float pHeight);

	BlendMode blendMode();

	void setBlendMode(const BlendMode & pMode);

	fvec2 startingSize();

	int angularVelocity();

	void swap();

	static nsstring getTypeString();

	virtual nsstring typeString() { return getTypeString(); }

	NSBufferObject * mFrontBuf;
	NSBufferObject * mBackBuf;
private:
	fvec3uimap mMotionKeys;
	fvec3uimap mVisualKeys;

	uivec2 mMatID; //!< Material ID for the rendered quad
	uivec2 mXFBShaderID; //!< Shader used for transform feedback
	nsuint mMaxParticleCount; //!< Maximum particles allowed
	nsuint mLifetime; //!< How long should these particles stay alive
	nsuint mEmissionRate; //!< Particles emitted per second
	uivec2 mRandTextID; // Random texture ID
	EmitterShape mEmitterShape; // Cube or Ellipse
	MotionKeyType mMotionKeyType; // Velocity or Acceleration motion keyframes
	BlendMode mBlendMode; // Additive or Source minus destination bla bla
	nsint mAngularVelocity; // self explanitory
	bool mLoopingEnabled; // wrap elapsed time back to 0 at end of lifetime and restart
	bool mMotionKeyInterp; // Interpolate between motion key frames?
	bool mMotionGlobalTime; // Should the particles all be affected by motion keyframes in global elapsed time or per particle lifetime
	bool mVisualKeyInterp; // Interpolate between keys?
	bool mVisualGlobalTime; // Should the particles all be affected by visual keyframes in global elapsed time or per particle lifetime
	nsfloat mElapsedTime; // Amount of time elapsed since sim started
	fvec2 mStartingSize; // Startung size of screen facing quads in world space sizes (not screen space)
	fvec3 mEmitterSize; // x,y,z size of the emitter space where particles generate
	fvec3 mInitVelocityMult; // Initial velocity x,y,z multiplier - multiplies the random number by this
	nsuint mMaxMotionKeys; // Maximum number of motion key frames - should be set to match whatever is in shader
	nsuint mMaxVisualKeys; // Maximum number of visual key frames - also should match whatever is set in shader

	/* This stuff does not need to be saved*/
	bool mSimulating; // Are we simulating?
	bool mFirst; // Is it the first time running the simulation since being reset (need to render using glDrawElements rather than feedback draw)


	NSTransformFeedbackObject * mTFB[2]; // 2 transform feedback buffers (draw from last to first then swap)
	NSVertexArrayObject * mVAO[2]; // for rendering whats in the TF FB buffers
	nsuint mBufferIndex; //!< Current buffer index
	ParticleArray mParticles; //!< Sort of dummy array used to allocate VBOs
};

template<class PUPer>
void pup(PUPer & p, NSParticleComp::EmitterShape & en, const nsstring & pString)
{
	nsuint in = static_cast<nsuint>(en);
	pup(p, in, pString);
	en = static_cast<NSParticleComp::EmitterShape>(in);
}

template<class PUPer>
void pup(PUPer & p, NSParticleComp::MotionKeyType & en, const nsstring & pString)
{
	nsuint in = static_cast<nsuint>(en);
	pup(p, in, pString);
	en = static_cast<NSParticleComp::MotionKeyType>(in);
}

template<class PUPer>
void pup(PUPer & p, NSParticleComp::BlendMode & en, const nsstring & pString)
{
	nsuint in = static_cast<nsuint>(en);
	pup(p, in, pString);
	en = static_cast<NSParticleComp::BlendMode>(in);
}

template <class PUPer>
void pup(PUPer & p, NSParticleComp & pc)
{
	pup(p, pc.mMotionKeys, "motionKeys");
	pup(p, pc.mVisualKeys, "visualKeys");
	pup(p, pc.mMatID, "matID");
	pup(p, pc.mXFBShaderID, "xfbShaderID");
	pup(p, pc.mMaxParticleCount, "maxParticleCount");
	pup(p, pc.mLifetime, "lifetime");
	pup(p, pc.mEmissionRate, "emissionRate");
	pup(p, pc.mRandTextID, "randTextID");
	pup(p, pc.mEmitterShape, "emitterShape");
	pup(p, pc.mMotionKeyType, "motionKeyType");
	pup(p, pc.mBlendMode, "blendMode");
	pup(p, pc.mAngularVelocity, "angularVelocity");
	pup(p, pc.mLoopingEnabled, "loopingEnabled");
	pup(p, pc.mMotionKeyInterp, "motionKeyInterp");
	pup(p, pc.mMotionGlobalTime, "motionGlobalTime");
	pup(p, pc.mVisualKeyInterp, "visualKeyInterp");
	pup(p, pc.mVisualGlobalTime, "visualGlobalTime");
	pup(p, pc.mElapsedTime, "elapsedTime");
	pup(p, pc.mStartingSize, "startingSize");
	pup(p, pc.mEmitterSize, "emitterSize");
	pup(p, pc.mInitVelocityMult, "initVelocityMult");
	pup(p, pc.mMaxMotionKeys, "maxMotionKeys");
	pup(p, pc.mMaxVisualKeys, "maxVisualKeys");
	pc.postUpdate(true);
}

#endif