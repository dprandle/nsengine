/*!
\file nsparticlecomp.cpp

\brief Definition file for NSParticleComp class

This file contains all of the neccessary definitions for the NSParticleComp class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/


#include <nsparticle_comp.h>
#include <nsxfb_object.h>
#include <nsvertex_array_object.h>
#include <nsbuffer_object.h>
#include <nstex_manager.h>
#include <nsshader_manager.h>
#include <nsentity.h>
#include <nsmat_manager.h>
#include <nsengine.h>

NSParticleComp::NSParticleComp() :
mFrontBuf(new nsbuffer_object(nsbuffer_object::array, nsbuffer_object::storage_mutable)),
mBackBuf(new nsbuffer_object(nsbuffer_object::array, nsbuffer_object::storage_mutable)),
mBufferIndex(0),
mSimulating(false),
mFirst(true),
mMatID(0),
mXFBShaderID(0),
mRandTextID(0),
mMaxParticleCount(DEFAULT_MAX_PARTICLES),
mLifetime(DEFAULT_PART_LIFETIME),
mEmissionRate(DEFAULT_PART_EMISSION),
mMaxMotionKeys(DEFAULT_MAX_FORCE_KEYS),
mMaxVisualKeys(DEFAULT_MAX_RENDER_KEYS),
mLoopingEnabled(false),
mMotionKeyType(Velocity),
mMotionKeyInterp(true),
mMotionGlobalTime(false),
mVisualGlobalTime(false),
mVisualKeyInterp(true),
mBlendMode(Mix),
mInitVelocityMult(1.0f, 1.0f, 1.0f),
mAngularVelocity(0),
mElapsedTime(0.0f),
mStartingSize(1.0f, 1.0f),
mEmitterShape(Cube),
mEmitterSize(1.0f, 1.0f, 1.0f),
NSComponent()
{
	mTFB[0] = new nsxfb_object();
	mTFB[1] = new nsxfb_object();
	mVAO[0] = new nsvertex_array_object();
	mVAO[1] = new nsvertex_array_object();
	mMotionKeys[0] = fvec3();
	mMotionKeys[mMaxMotionKeys] = fvec3();
	mVisualKeys[0] = fvec3(1.0f,1.0f,1.0f);
	mVisualKeys[mMaxVisualKeys] = fvec3(1.0f, 1.0f, 1.0f);
}

NSParticleComp::~NSParticleComp()
{
	release();
	delete mFrontBuf;
	delete mBackBuf;
	delete mTFB[0];
	delete mTFB[1];
	delete mVAO[0];
	delete mVAO[1];
}

NSParticleComp* NSParticleComp::copy(const NSComponent * pToCopy)
{
	if (pToCopy == NULL)
		return NULL;
	const NSParticleComp * comp = (const NSParticleComp*)pToCopy;
	(*this) = (*comp);
	return this;
}

void NSParticleComp::name_change(const uivec2 & oldid, const uivec2 newid)
{
	if (mRandTextID.x == oldid.x)
	{
		mRandTextID.x = newid.x;
		if (mRandTextID.y == oldid.y)
			mRandTextID.y = newid.y;
	}

	if (mMatID.x == oldid.x)
	{
		mMatID.x = newid.x;
		if (mMatID.y == oldid.y)
			mMatID.y = newid.y;
	}

	if (mXFBShaderID.x == oldid.x)
	{
		mXFBShaderID.x = newid.x;
		if (mXFBShaderID.y == oldid.y)
			mXFBShaderID.y = newid.y;
	}
}

/*!
Get the resources that the component uses. If no resources are used then leave this unimplemented - will return an empty map.
/return Map of resource ID to resource type containing all used resources
*/
uivec2array NSParticleComp::resources()
{
	uivec2array ret;

	if (mRandTextID != 0)
		ret.push_back(mRandTextID);

	if (mMatID != 0)
		ret.push_back(mMatID);

	if (mXFBShaderID != 0)
		ret.push_back(mXFBShaderID);

	return ret;
}

void NSParticleComp::clearMotionKeys()
{
	mMotionKeys.clear();
	mMotionKeys[0] = fvec3();
	mMotionKeys[mMaxMotionKeys] = fvec3();
}

void NSParticleComp::clearVisualKeys()
{
	mVisualKeys.clear();
	mVisualKeys[0] = fvec3(1.0f, 1.0f, 1.0f);
	mVisualKeys[mMaxVisualKeys] = fvec3(1.0f, 1.0f, 1.0f);
}

void NSParticleComp::setAngularVelocity(int32 pVel)
{
	mAngularVelocity = pVel;
}

fvec2 NSParticleComp::startingSize()
{
	return mStartingSize;
}

int32 NSParticleComp::angularVelocity()
{
	return mAngularVelocity;
}

void NSParticleComp::enableMotionGlobalTime(bool pEnable)
{
	mMotionGlobalTime = pEnable;
}

bool NSParticleComp::motionGlobalTime()
{
	return mMotionGlobalTime;
}

void NSParticleComp::setEmitterShape(const EmitterShape & pShape)
{
	mEmitterShape = pShape;
}

void NSParticleComp::setMotionKeyType(const MotionKeyType & pType)
{
	mMotionKeyType = pType;
}

const NSParticleComp::MotionKeyType & NSParticleComp::motionKeyType()
{
	return mMotionKeyType;
}

bool NSParticleComp::motionKeyInterpolation()
{
	return mMotionKeyInterp;
}

void NSParticleComp::enableMotionKeyInterpolation(bool pEnable)
{
	mMotionKeyInterp = pEnable;
}

const NSParticleComp::EmitterShape & NSParticleComp::emitterShape()
{
	return mEmitterShape;
}

const fvec3 & NSParticleComp::emitterSize()
{
	return mEmitterSize;
}

NSParticleComp::BlendMode NSParticleComp::blendMode()
{
	return mBlendMode;
}

void NSParticleComp::setBlendMode(const BlendMode & pMode)
{
	mBlendMode = pMode;
}

void NSParticleComp::setEmitterSize(const fvec3 & pSize)
{
	mEmitterSize = pSize;
}

fvec3 NSParticleComp::motionKeyAt(float pTime)
{
	uint32 index = static_cast<uint32>(pTime * mMaxMotionKeys);
	fvec3uimap::iterator fIter = mMotionKeys.find(index);
	if (fIter != mMotionKeys.end())
		return fIter->second;
	return fvec3();
}

fvec3 NSParticleComp::visualKeyAt(float pTime)
{
	uint32 index = static_cast<uint32>(pTime * mMaxVisualKeys);
	fvec3uimap::iterator rIter = mVisualKeys.find(index);
	if (rIter != mVisualKeys.end())
		return rIter->second;
	return fvec3();
}

void NSParticleComp::init()
{
	mTFB[0]->init_gl();
	mTFB[1]->init_gl();
	mVAO[0]->init_gl();
	mVAO[1]->init_gl();
	mFrontBuf->init_gl();
	mBackBuf->init_gl();

	mParticles.resize(mMaxParticleCount);
	mParticles[0].mAgeTypeReserved.y = 1.0f;

	mVAO[0]->bind();
	mBackBuf->bind();
	mBackBuf->allocate(mParticles,
					   nsbuffer_object::mutable_dynamic_draw,
					   static_cast<uint32>(mParticles.size()));
	mVAO[0]->enable(0);
	mVAO[0]->vertex_attrib_ptr(0, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), 0);
	mVAO[0]->enable(1);
	mVAO[0]->vertex_attrib_ptr(1, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), sizeof(fvec4));
	mVAO[0]->enable(2);
	mVAO[0]->vertex_attrib_ptr(2, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), sizeof(fvec4) * 2);
	mVAO[0]->enable(3);
	mVAO[0]->vertex_attrib_ptr(3, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), sizeof(fvec4) * 3);
	mVAO[0]->unbind();

	mVAO[1]->bind();
	mFrontBuf->bind();
	mFrontBuf->allocate(mParticles,
						nsbuffer_object::mutable_dynamic_draw,
						static_cast<uint32>(mParticles.size()));
	mVAO[1]->enable(0);
	mVAO[1]->vertex_attrib_ptr(0, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), 0);
	mVAO[1]->enable(1);
	mVAO[1]->vertex_attrib_ptr(1, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), sizeof(fvec4));
	mVAO[1]->enable(2);
	mVAO[1]->vertex_attrib_ptr(2, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), sizeof(fvec4) * 2);
	mVAO[1]->enable(3);
	mVAO[1]->vertex_attrib_ptr(3, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), sizeof(fvec4) * 3);
	mVAO[1]->unbind();

	mFrontBuf->set_target(nsbuffer_object::transform_feedback);
	mBackBuf->set_target(nsbuffer_object::transform_feedback);
	mTFB[0]->set_primitive(nsxfb_object::gl_points);
	mTFB[1]->set_primitive(nsxfb_object::gl_points);

	mTFB[0]->bind();
	mFrontBuf->bind();
	mFrontBuf->bind(0);
	mTFB[0]->unbind();

	mTFB[1]->bind();
	mBackBuf->bind();
	mBackBuf->bind(0);
	mTFB[1]->unbind();
}

void NSParticleComp::removeMotionKey(float pTime)
{
	if (pTime >= 1.0f)
	{
		return;
	}
	else if (pTime <= 0.0f)
	{
		return;
	}
	uint32 index = static_cast<uint32>(pTime * mMaxMotionKeys);
	mMotionKeys.erase(index);
}

void NSParticleComp::enableVisualGlobalTime(bool pEnable)
{
	mVisualGlobalTime = pEnable;
}

void NSParticleComp::enableVisualKeyInterpolation(bool pEnable)
{
	mVisualKeyInterp = pEnable;
}

bool NSParticleComp::visualKeyInterpolation()
{
	return mVisualKeyInterp;
}

bool NSParticleComp::visualGlobalTime()
{
	return mVisualGlobalTime;
}

void NSParticleComp::removeVisualKey(float pTime)
{
	if (pTime >= 1.0f)
	{
		return;
	}
	else if (pTime <= 0.0f)
	{
		return;
	}
	uint32 index = static_cast<uint32>(pTime * mMaxVisualKeys);
	mVisualKeys.erase(index);
}


const uivec2 & NSParticleComp::shaderID()
{
	return mXFBShaderID;
}

uint32 NSParticleComp::maxMotionKeys()
{
	return mMaxMotionKeys;
}

uint32 NSParticleComp::maxVisualKeys()
{
	return mMaxVisualKeys;
}

void NSParticleComp::setMaxMotionKeys(uint32 pMax)
{
	if (pMax > PARTICLE_MAX_MOTION_KEYS)
		return;

	fvec3uimap tmpMap;
	fvec3uimap::iterator mapIter = mMotionKeys.begin();
	while (mapIter != mMotionKeys.end())
	{
		tmpMap[static_cast<uint32>(float(mapIter->first) / float(mMaxMotionKeys) * pMax)] = mapIter->second;
		++mapIter;
	}
	std::swap(tmpMap, mMotionKeys);
	mMaxMotionKeys = pMax;
}

const fvec3uimap & NSParticleComp::motionKeys()
{
	return mMotionKeys;
}

const fvec3uimap & NSParticleComp::visualKeys()
{
	return mVisualKeys;
}

void NSParticleComp::setMaxVisualKeys(uint32 pMax)
{
	if (pMax > PARTICLE_MAX_VISUAL_KEYS)
		return;

	fvec3uimap tmpMap;
	fvec3uimap::iterator mapIter = mVisualKeys.begin();
	while (mapIter != mVisualKeys.end())
	{
		tmpMap[static_cast<uint32>(float(mapIter->first) / float(mMaxVisualKeys) * pMax)] = mapIter->second;
		++mapIter;
	}
	std::swap(tmpMap, mVisualKeys);
	mMaxVisualKeys = pMax;
}

void NSParticleComp::setMotionKey(float pTime, const fvec3 & pForce)
{
	if (pTime > 1.0f)
	{
		dprint("NSParticleComp::setForceKey - Warning : setting time above 1.0 clamps the time to 1");
		pTime = 1.0f;
	}
	else if (pTime < 0.0f)
	{
		dprint("NSParticleComp::setForceKey - Warning : setting time below 0.0 clamps the time to 0");
		pTime = 0.0f;
	}
	uint32 index = static_cast<uint32>(pTime * mMaxMotionKeys);
	mMotionKeys[index] = pForce;
}

void NSParticleComp::setVisualKey(float pTime, const fvec3 & pRenderKey)
{
	if (pTime > 1.0f)
	{
		dprint("NSParticleComp::setRenderKey - Warning : setting time above 1.0 clamps the time to 1");
		pTime = 1.0f;
	}
	else if (pTime < 0.0f)
	{
		dprint("NSParticleComp::setRenderKey - Warning : setting time below 0.0 clamps the time to 0");
		pTime = 0.0f;
	}
	uint32 index = static_cast<uint32>(pTime * mMaxVisualKeys);
	mVisualKeys[index] = pRenderKey;
}

void NSParticleComp::setShaderID(const uivec2 & pID)
{
	mXFBShaderID = pID;
}

const uivec2 & NSParticleComp::materialID()
{
	return mMatID;
}

uint32 NSParticleComp::maxParticles()
{
	return mMaxParticleCount;
}

float & NSParticleComp::elapsed()
{
	return mElapsedTime;
}

uint32 NSParticleComp::lifetime()
{
	return mLifetime;
}

uint32 NSParticleComp::emissionRate()
{
	return mEmissionRate;
}

const fvec3 & NSParticleComp::initVelocityMult()
{
	return mInitVelocityMult;
}

void NSParticleComp::setInitVelocityMult(const fvec3 & pMult)
{
	mInitVelocityMult = pMult;
}

uint32 NSParticleComp::motionKeyCount()
{
	return static_cast<uint32>(mMotionKeys.size());
}

uint32 NSParticleComp::visualKeyCount()
{
	return static_cast<uint32>(mVisualKeys.size());
}

fvec3uimap::iterator NSParticleComp::beginMotionKey()
{
	return mMotionKeys.begin();
}

fvec3uimap::iterator NSParticleComp::beginVisualKey()
{
	return mVisualKeys.begin();
}

fvec3uimap::iterator NSParticleComp::endMotionKey()
{
	return mMotionKeys.end();
}

fvec3uimap::iterator NSParticleComp::endVisualKey()
{
	return mVisualKeys.end();
}

void NSParticleComp::setEmissionRate(uint32 pRate)
{
	if (pRate * mLifetime >= 1000 * mMaxParticleCount)
	{
		dprint("NSParticleComp::setEmissionRate(uint32 pRate) - Emission rate limited by max particle count");
		return;
	}
	mEmissionRate = pRate;
}

bool NSParticleComp::hasMotionKey(float pTime)
{
	int32 index = static_cast<uint32>(pTime*mMaxMotionKeys);
	return (mMotionKeys.find(index) != mMotionKeys.end());
}

bool NSParticleComp::hasVisualKey(float pTime)
{
	int32 index = static_cast<uint32>(pTime*mMaxVisualKeys);
	return (mVisualKeys.find(index) != mVisualKeys.end());
}

void NSParticleComp::setStartingSize(const fvec2 & pSize)
{
	mStartingSize = pSize;
}

void NSParticleComp::setStartingSize(float pWidth, float pHeight)
{
	mStartingSize.u = pWidth;
	mStartingSize.v = pHeight;
}

void NSParticleComp::setLifetime(uint32 pLifetime)
{
	if (mEmissionRate * pLifetime >= 1000 * mMaxParticleCount)
	{
		dprint("NSParticleComp::setLoopTime(uint32 pRate) - Lifetime limited by max particle count");
		return;
	}
	mLifetime = pLifetime;
}

void NSParticleComp::allocateBuffers()
{
	mParticles.resize(mMaxParticleCount);
	mParticles[0].mAgeTypeReserved.y = 1.0f;

	mFrontBuf->set_target(nsbuffer_object::array);
	mBackBuf->set_target(nsbuffer_object::array);

	mBackBuf->bind();
	mBackBuf->allocate(mParticles,
					   nsbuffer_object::mutable_dynamic_draw,
					   static_cast<uint32>(mParticles.size()));
	mFrontBuf->bind();
	mBackBuf->allocate(mParticles,
					   nsbuffer_object::mutable_dynamic_draw,
					   static_cast<uint32>(mParticles.size()));
	mFrontBuf->unbind();
}

void NSParticleComp::setMaxParticles(uint32 pMaxParticles)
{
	mMaxParticleCount = pMaxParticles;
	if (mEmissionRate * mLifetime >= 1000 * mMaxParticleCount)
	{
		mLifetime = 1000 * mMaxParticleCount / mEmissionRate;
	}
	allocateBuffers();
}

void NSParticleComp::enableLooping(bool pEnable)
{
	mLoopingEnabled = pEnable;
}

void NSParticleComp::setMaterialID(const uivec2 & pID)
{
	mMatID = pID;
}

bool NSParticleComp::looping()
{
	return mLoopingEnabled;
}

void NSParticleComp::release()
{
	mTFB[0]->release();
	mTFB[1]->release();
	mVAO[0]->release();
	mVAO[1]->release();
	mFrontBuf->release();
	mBackBuf->release();
}

void NSParticleComp::setRandTextureID(const uivec2 & pID)
{
	mRandTextID = pID;
}

const uivec2 & NSParticleComp::randomTextureID()
{
	return mRandTextID;
}

uint32 NSParticleComp::transformFeedbackID()
{
	return mTFB[1 - mBufferIndex]->gl_id();
}

nsxfb_object * NSParticleComp::transformFeedbackObject()
{
	return mTFB[mBufferIndex];
}

nsvertex_array_object * NSParticleComp::vertexArrayObject()
{
	return mVAO[mBufferIndex];
}

void NSParticleComp::swap()
{
	mBufferIndex = 1 - mBufferIndex;
}

void NSParticleComp::pup(nsfile_pupper * p)
{
	if (p->type() == nsfile_pupper::pup_binary)
	{
		nsbinary_file_pupper * bf = static_cast<nsbinary_file_pupper *>(p);
		::pup(*bf, *this);
	}
	else
	{
		nstext_file_pupper * tf = static_cast<nstext_file_pupper *>(p);
		::pup(*tf, *this);
	}
}

bool NSParticleComp::first()
{
	return mFirst;
}

void NSParticleComp::reset()
{
	mElapsedTime = 0.0f;
}

void NSParticleComp::setFirst(bool pSet)
{
	mFirst = pSet;
}

NSParticleComp & NSParticleComp::operator=(const NSParticleComp & rhs)
{
	// Check for self assignment
	if (this == &rhs)
		return *this;

	release();
	mMotionKeys.clear();
	mVisualKeys.clear();
	mMotionKeys.insert(rhs.mMotionKeys.begin(), rhs.mMotionKeys.end());
	mVisualKeys.insert(rhs.mVisualKeys.begin(), rhs.mVisualKeys.end());

	mMatID = rhs.mMatID;
	mXFBShaderID = rhs.mXFBShaderID;
	mMaxParticleCount = rhs.mMaxParticleCount;
	mLifetime = rhs.mLifetime;
	mEmissionRate = rhs.mEmissionRate;
	mRandTextID = rhs.mRandTextID;
	mEmitterShape = rhs.mEmitterShape;
	mMotionKeyType = rhs.mMotionKeyType;
	mBlendMode = rhs.mBlendMode;
	mAngularVelocity = rhs.mAngularVelocity;
	mLoopingEnabled = rhs.mLoopingEnabled;
	mMotionKeyInterp = rhs.mMotionKeyInterp;
	mMotionGlobalTime = rhs.mMotionGlobalTime;
	mVisualKeyInterp = rhs.mVisualKeyInterp;
	mVisualGlobalTime = rhs.mVisualGlobalTime;
	mElapsedTime = rhs.mElapsedTime;
	mStartingSize = rhs.mStartingSize;
	mEmitterSize = rhs.mEmitterSize;
	mInitVelocityMult = rhs.mInitVelocityMult;
	mMaxMotionKeys = rhs.mMaxMotionKeys;
	mMaxVisualKeys = rhs.mMaxVisualKeys;

	post_update(true);
	return (*this);
}

bool NSParticleComp::simulating()
{
	return mSimulating;
}

void NSParticleComp::enableSimulation(bool pEnable)
{
	mSimulating = pEnable;
}
