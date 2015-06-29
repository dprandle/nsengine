/*! 
	\file nstformcomp.cpp
	
	\brief Definition file for NSTFormComp class

	This file contains all of the neccessary definitions for the NSTFormComp class.

	\author Daniel Randle
	\date November 23 2013
	\copywrite Earth Banana Games 2013
*/

#include <nstformcomp.h>
#include <nsrendercomp.h>
#include <nsentity.h>
#include <nstimer.h>
#include <nstilegrid.h>
#include <nsmeshmanager.h>
#include <nsmesh.h>
#include <nsengine.h>

NSTFormComp::NSTFormComp():
mTransformBuffer(NSBufferObject::Array, NSBufferObject::Mutable),
mTransformIDBuffer(NSBufferObject::Array, NSBufferObject::Mutable),
mXFBData(),
mBufferResize(false),
mTransformFB(false),
NSComponent(),
mVisibleCount(0)
{}

NSTFormComp::~NSTFormComp()
{
	release();
}

nsuint NSTFormComp::add()
{
	return add(InstTrans());
}

nsuint NSTFormComp::add(const InstTrans & trans)
{
	mTransforms.push_back(trans);
	postUpdate(true);
	mBufferResize = true;

	if (mTransformFB)
	{
		enableTransformFeedback(false);
		enableTransformFeedback(true);
	}

	return mTransforms.size() - 1;
}

nsuint NSTFormComp::add(nsuint pHowMany)
{
	// Get the transformID of the last transform before adding pHowMany and return it
	nsuint ret = mTransforms.size() - 1;
	mTransforms.resize(mTransforms.size() + pHowMany);
	postUpdate(true);
	mBufferResize = true;

	if (mTransformFB)
	{
		enableTransformFeedback(false);
		enableTransformFeedback(true);
	}

	return ret;
}

bool NSTFormComp::bufferResize() const
{
	return mBufferResize;
}

void NSTFormComp::changeScale(const fvec3 & pAmount, nsuint pTransformID)
{
	if (pTransformID >= mTransforms.size())
	{
		dprint("NSTFormComp::changeScale - Invalid TransformID: " + std::to_string(pTransformID));
		return;
	}
	mTransforms[pTransformID].mScaling += pAmount;
	mTransforms[pTransformID].mUpdate = true;
	postUpdate(true);
}

void NSTFormComp::changeScale(nsfloat pX, nsfloat pY, nsfloat pZ, nsuint pTransformID)
{
	changeScale(fvec3(pX, pY, pZ), pTransformID);
}

void NSTFormComp::pup(NSFilePUPer * p)
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

void NSTFormComp::computeTransform(nsuint pTransformID)
{
	if (pTransformID >= mTransforms.size())
	{
		dprint("NSTFormComp::computeTransform - Invalid TransformID: " + std::to_string(pTransformID));
		return;
	}
	mTransforms[pTransformID].compute();
}

NSTFormComp* NSTFormComp::copy(const NSComponent * pToCopy)
{
	if (pToCopy == NULL)
		return NULL;
	const NSTFormComp * comp = (const NSTFormComp*)pToCopy;
	(*this) = (*comp);
	return this;
}

void NSTFormComp::enableParent(nsbool pEnable, nsuint pTransformID)
{
	if (pTransformID >= mTransforms.size())
	{
		dprint("NSTFormComp::enableParent - Invalid TransformID: " + std::to_string(pTransformID));
		return;
	}

	mTransforms[pTransformID].mParentEnabled = pEnable;
	mTransforms[pTransformID].mUpdate = true;
	postUpdate(true);
}

/*!
Enable transform feedback for this entity. Pass in pVertCount to set the buffer size necessary to hold
*/
bool NSTFormComp::enableTransformFeedback(nsbool pEnable)
{
	mTransformFB = pEnable;

	if (mTransformFB)
	{
		// Figure out how much mem to allocate
		NSRenderComp * mRenComp = mOwner->get<NSRenderComp>();
		if (mRenComp == NULL || mTransforms.empty())
			return false;

		NSMesh * mesh = nsengine.resource<NSMesh>(mRenComp->meshID());
		if (mesh == NULL)
			return false;

		// Figure out how much memory to allocate in the buffers.. this should be number of tranforms times number of verts
		nsuint allocAmount = mesh->vertcount() * mTransforms.size();
		nsuint instancePerDraw = mTransforms.size();
		nsuint totalIntanceCount = mTransforms.size();

		// If the total is larger than 4mB, then find out how many 4 mB buffers are needed
		nsuint bufCount = 1;
		if (allocAmount > MAX_TF_BUFFER_SIZE)
		{
			bufCount = allocAmount / MAX_TF_BUFFER_SIZE + 1;
			allocAmount = MAX_TF_BUFFER_SIZE;
			instancePerDraw = MAX_TF_BUFFER_SIZE / mesh->vertcount();
		}

		if (bufCount > MAX_TF_BUFFER_COUNT)
			return false;



		mXFBData.mXFBBuffers.resize(bufCount);
		for (nsuint bufI = 0; bufI < bufCount; ++bufI)
		{
			XFBBuffer * buf = &mXFBData.mXFBBuffers[bufI];
			buf->mAllocAmount = allocAmount;

			buf->mTFFeedbackObj = new NSTransformFeedbackObject();
			buf->mXFBVAO = new NSVertexArrayObject();
			buf->mXBWorldPosBuf = new NSBufferObject(NSBufferObject::Array, NSBufferObject::Mutable);
			buf->mXBTexCoordBuf = new NSBufferObject(NSBufferObject::Array, NSBufferObject::Mutable);
			buf->mXBNormalBuf = new NSBufferObject(NSBufferObject::Array, NSBufferObject::Mutable);
			buf->mXBTangentBuf = new NSBufferObject(NSBufferObject::Array, NSBufferObject::Mutable);

			buf->mXFBVAO->initGL();
			buf->mTFFeedbackObj->initGL();
			buf->mXBWorldPosBuf->initGL();
			buf->mXBTexCoordBuf->initGL();
			buf->mXBNormalBuf->initGL();
			buf->mXBTangentBuf->initGL();

			// Set up VAO for draw on other stuff
			buf->mXFBVAO->bind();

			buf->mXBWorldPosBuf->bind();
			buf->mXBWorldPosBuf->allocate<fvec3>(NSBufferObject::MutableDynamicCopy, buf->mAllocAmount);
			buf->mXFBVAO->add(buf->mXBWorldPosBuf, NSShader::Position);
			buf->mXFBVAO->vertexAttribPtr(NSShader::Position, 4, GL_FLOAT, GL_FALSE, sizeof(fvec3), 0);

			buf->mXBTexCoordBuf->bind();
			buf->mXBTexCoordBuf->allocate<fvec3>(NSBufferObject::MutableDynamicCopy, buf->mAllocAmount);
			buf->mXFBVAO->add(buf->mXBTexCoordBuf, NSShader::TexCoords);
			buf->mXFBVAO->vertexAttribPtr(NSShader::TexCoords, 3, GL_FLOAT, GL_FALSE, sizeof(fvec3), 0);

			buf->mXBNormalBuf->bind();
			buf->mXBNormalBuf->allocate<fvec3>(NSBufferObject::MutableDynamicCopy, buf->mAllocAmount);
			buf->mXFBVAO->add(buf->mXBNormalBuf, NSShader::Normal);
			buf->mXFBVAO->vertexAttribPtr(NSShader::Normal, 3, GL_FLOAT, GL_FALSE, sizeof(fvec3), 0);

			buf->mXBTangentBuf->bind();
			buf->mXBTangentBuf->allocate<fvec3>(NSBufferObject::MutableDynamicCopy, buf->mAllocAmount);
			buf->mXFBVAO->add(buf->mXBTangentBuf, NSShader::Tangent);
			buf->mXFBVAO->vertexAttribPtr(NSShader::Tangent, 3, GL_FLOAT, GL_FALSE, sizeof(fvec3), 0);

			buf->mXFBVAO->unbind();

			buf->mXBWorldPosBuf->setTarget(NSBufferObject::TransformFeedback);
			buf->mXBTexCoordBuf->setTarget(NSBufferObject::TransformFeedback);
			buf->mXBNormalBuf->setTarget(NSBufferObject::TransformFeedback);
			buf->mXBTangentBuf->setTarget(NSBufferObject::TransformFeedback);
			buf->mTFFeedbackObj->bind();
			buf->mXBWorldPosBuf->bind(NSShader::Position);
			buf->mXBTexCoordBuf->bind(NSShader::TexCoords);
			buf->mXBNormalBuf->bind(NSShader::Normal);
			buf->mXBTangentBuf->bind(NSShader::Tangent);
			buf->mTFFeedbackObj->unbind();

			if (instancePerDraw <= totalIntanceCount)
			{
				buf->mInstanceCount = instancePerDraw;
				totalIntanceCount -= instancePerDraw;
			}
			else
			{
				buf->mInstanceCount = totalIntanceCount;
			}
		}
		mXFBData.mUpdate = true;
	}
	else
	{
		for (nsuint bufI = 0; bufI < mXFBData.mXFBBuffers.size(); ++bufI)
		{
			XFBBuffer * buf = &mXFBData.mXFBBuffers[bufI];

			buf->mTFFeedbackObj->release();
			delete buf->mTFFeedbackObj;
			buf->mTFFeedbackObj = NULL;

			buf->mXFBVAO->release();
			delete buf->mXFBVAO;
			buf->mXFBVAO = NULL;

			buf->mXBWorldPosBuf->release();
			delete buf->mXBWorldPosBuf;
			buf->mXBWorldPosBuf = NULL;

			buf->mXBTexCoordBuf->release();
			delete buf->mXBTexCoordBuf;
			buf->mXBTexCoordBuf = NULL;

			buf->mXBNormalBuf->release();
			delete buf->mXBNormalBuf;
			buf->mXBNormalBuf = NULL;

			buf->mXBTangentBuf->release();
			delete buf->mXBTangentBuf;
			buf->mXBTangentBuf = NULL;
		}
		mXFBData.mXFBBuffers.clear();
		mXFBData.mUpdate = false;
	}
	return true;
}

void NSTFormComp::init()
{
	mTransformBuffer.initGL();
	mTransformIDBuffer.initGL();
}

const fvec3 NSTFormComp::dirVec(DirVec pDirection, nsuint pTransformID) const
{
	switch (pDirection)
	{
	case (Right):
		return mTransforms[pTransformID].mOrientation.right();
	case (Up) :
		return mTransforms[pTransformID].mOrientation.up();
	case (Target) :
		return mTransforms[pTransformID].mOrientation.target();
	}
	return fvec3();
}

NSTFormComp::InstTrans & NSTFormComp::instTrans(nsuint pTransformID)
{
	return mTransforms[pTransformID];
}

const NSTFormComp::InstanceVec & NSTFormComp::transformVec() const
{
	return mTransforms;
}

const fquat & NSTFormComp::orientation(nsuint pTransformID) const
{
	return mTransforms[pTransformID].mOrientation;
}

const fvec3 & NSTFormComp::lpos(nsuint pTransformID) const
{
	return mTransforms[pTransformID].mPosition;
}

const fmat4 & NSTFormComp::pov(nsuint pTransformID) const
{
	return mTransforms[pTransformID].mPOVTransform;
}

nsuint NSTFormComp::renderID(nsuint pTransformID) const
{
	return mTransforms[pTransformID].mRenderID;
}

const fvec3 & NSTFormComp::scaling(nsuint pTransformID) const
{
	return mTransforms[pTransformID].mScaling;
}

const uivec3 & NSTFormComp::parentid(nsuint pTransformID) const
{
	return mTransforms[pTransformID].mParentID;
}

NSTFormComp::XFBData * NSTFormComp::xfbData()
{
	return &mXFBData;
}

const fmat4 & NSTFormComp::parent(nsuint pTransformID) const
{
	return mTransforms[pTransformID].mParentTransform;
}

const fmat4 & NSTFormComp::transform(nsuint pTransformID) const
{
	return mTransforms[pTransformID].mTransform;
}

NSBufferObject * NSTFormComp::transformBuffer()
{
	return &mTransformBuffer;
}

NSBufferObject * NSTFormComp::transformIDBuffer()
{
	return &mTransformIDBuffer;
}

nsuint NSTFormComp::count() const
{
	return mTransforms.size();
}

nsuint NSTFormComp::visibleCount() const
{
	return mVisibleCount;
}

fvec3 NSTFormComp::wpos(nsuint pTransformID)
{
	if (mTransforms[pTransformID].mParentEnabled)
		return (mTransforms[pTransformID].mParentTransform * fvec4(mTransforms[pTransformID].mPosition, 1.0f)).xyz();

	return mTransforms[pTransformID].mPosition;
}

nsbool NSTFormComp::snapped(nsuint pTransformID) const
{
	return mTransforms[pTransformID].mSnapToGrid;
}

int NSTFormComp::hiddenState(nsuint pTransformID) const
{
	return mTransforms[pTransformID].mHiddenState;
}

nsbool NSTFormComp::parentEnabled(nsuint pTransformID) const
{
	return mTransforms[pTransformID].mParentEnabled;
}

nsbool NSTFormComp::transformFeedback()
{
	return mTransformFB;
}

const nsbool NSTFormComp::transUpdate(nsuint pTransformID) const
{
	return mTransforms[pTransformID].mUpdate;
}

void NSTFormComp::release()
{
	mTransformBuffer.release();
	mTransformIDBuffer.release();
	if (mTransformFB)
		enableTransformFeedback(false);
}

nsuint NSTFormComp::remove(nsuint pTransformID)
{
	if (pTransformID >= mTransforms.size())
	{
		dprint("NSTFormComp::removeTransform - Invalid TransformID or Invalid Operation; Transform ID: " + std::to_string(pTransformID));
		return mTransforms.size();
	}
	else if (mTransforms.size() == 1)
	{
		mTransforms.pop_back();
		return 0;
	}

	mTransforms[pTransformID] = mTransforms.back();
	mTransforms[pTransformID].mUpdate = true;
	mTransforms.pop_back();
	postUpdate(true);
	mBufferResize = true;

	if (mTransformFB)
	{
		enableTransformFeedback(false);
		enableTransformFeedback(true);
	}

	return mTransforms.size();
}

nsuint NSTFormComp::remove(nsuint pFirst, nsuint pLast)
{
	if (pFirst >= mTransforms.size() || pLast >= mTransforms.size() || pFirst >= pLast || (pLast - pFirst + 1) == mTransforms.size())
	{
		dprint("NSTFormComp::removeTransforms - Invalid TransformID or Invalid operation");
		return mTransforms.size();
	}
	nsuint resizeNum = mTransforms.size() - (pLast - pFirst + 1);

	// Move all the elements in the back of the last that arent part of the erase
	// range to the first index that the erase begins.. increasing the index and
	// popping an item off the back every iteration..
	for (nsuint i = pFirst; i <= pLast; ++i)
	{
		if (pLast < mTransforms.size() - 1)
		{
			mTransforms[i] = mTransforms.back();
			mTransforms[i].mUpdate = true;
			mTransforms.pop_back();
		}
		else
			break;
	}

	mTransforms.resize(resizeNum);
	mBufferResize = true;
	postUpdate(true);

	if (mTransformFB)
	{
		enableTransformFeedback(false);
		enableTransformFeedback(true);
	}

	return mTransforms.size();
}

void NSTFormComp::rotate(const fvec4 & axisAngle, nsuint tformid)
{
	if (tformid >= mTransforms.size())
	{
		dprint("NSTFormComp::rotate - Invalid TransformID: " + std::to_string(tformid));
		return;
	}

	mTransforms[tformid].mOrientation = ::orientation(axisAngle) * mTransforms[tformid].mOrientation;
	mTransforms[tformid].mUpdate = true;
	postUpdate(true);
}

void NSTFormComp::rotate(const fvec4 & axisAngle)
{
	for (nsuint i = 0; i < mTransforms.size(); ++i)
		rotate(axisAngle, i);
}

void NSTFormComp::rotate(Axis pAxis, float pAngle, nsuint pTransformID)
{
	fvec4 axAng;
	switch (pAxis)
	{
	case(XAxis) :
		axAng.set(1, 0, 0, pAngle);
		break;
	case(YAxis) :
		axAng.set(0, 1, 0, pAngle);
		break;
	case(ZAxis) :
		axAng.set(0, 0, 1, pAngle);
		break;
	}
	rotate(axAng, pTransformID);
}

void NSTFormComp::rotate(Axis pAxis, float pAngle)
{
	for (nsuint i = 0; i < mTransforms.size(); ++i)
		rotate(pAxis, pAngle, i);
}

void NSTFormComp::rotate(const fquat & pQuat, nsuint pTransformID)
{
	if (pTransformID >= mTransforms.size())
	{
		dprint("NSTFormComp::rotate - Invalid TransformID: " + std::to_string(pTransformID));
		return;
	}

	mTransforms[pTransformID].mOrientation = pQuat * mTransforms[pTransformID].mOrientation;
	mTransforms[pTransformID].mUpdate = true;
	postUpdate(true);
}

void NSTFormComp::rotate(const fquat & pQuat)
{
	for (nsuint i = 0; i < mTransforms.size(); ++i)
		rotate(pQuat, i);
}

void NSTFormComp::rotate(const fvec3 & euler)
{
	for (nsuint i = 0; i < mTransforms.size(); ++i)
		rotate(euler, i);
}

void NSTFormComp::rotate(const fvec3 & euler, nsuint pTransformID)
{
	rotate(::orientation(euler, fvec3::XYZ), pTransformID);
}

void NSTFormComp::rotate(DirVec dir, nsfloat angle, nsuint tformid)
{
	rotate(fvec4(dirVec(dir), angle), tformid);
}

void NSTFormComp::rotate(DirVec dir, nsfloat angle)
{
	for (nsuint i = 0; i < mTransforms.size(); ++i)
		rotate(dir, angle, i);
}

void NSTFormComp::scale(const fvec3 & pAmount, nsuint pTransformID)
{
	if (pTransformID >= mTransforms.size())
	{
		dprint("NSTFormComp::scale - Invalid TransformID: " + std::to_string(pTransformID));
		return;
	}

	mTransforms[pTransformID].mScaling %= pAmount;
	mTransforms[pTransformID].mUpdate = true;
	postUpdate(true);
}

void NSTFormComp::scale(nsfloat pX, nsfloat pY, nsfloat pZ, nsuint pTransformID)
{
	scale(fvec3(pX, pY, pZ), pTransformID);
}

void NSTFormComp::scale(const fvec3 & pAmount)
{
	for (nsuint i = 0; i < mTransforms.size(); ++i)
		scale(pAmount, i);
}

void NSTFormComp::scale(nsfloat pX, nsfloat pY, nsfloat pZ)
{
	for (nsuint i = 0; i < mTransforms.size(); ++i)
		scale(pX, pY, pZ, i);
}

void NSTFormComp::setBufferResize(nsbool pResize)
{
	mBufferResize = pResize;
}

void NSTFormComp::enableSnap(nsbool pSnap, nsuint pTransformID)
{
	if (pTransformID >= mTransforms.size())
	{
		dprint("NSTFormComp::setGridSnap - Invalid TransformID: " + std::to_string(pTransformID));
		return;
	}

	mTransforms[pTransformID].mSnapToGrid = pSnap;
	mTransforms[pTransformID].mUpdate = true;
	postUpdate(true);
}

void NSTFormComp::enableSnap(nsbool pSnap)
{
	for (nsuint i = 0; i < mTransforms.size(); ++i)
		enableSnap(pSnap, i);
}

void NSTFormComp::setHiddenState(HiddenState pState, nsbool pEnable, nsuint pTransformID)
{
	if (pTransformID >= mTransforms.size())
	{
		dprint("NSTFormComp::setHidden - Invalid TransformID: " + std::to_string(pTransformID));
		return;
	}

	if (pEnable)
		mTransforms[pTransformID].mHiddenState |= pState;
	else
		mTransforms[pTransformID].mHiddenState &= ~pState;
	
	mTransforms[pTransformID].mUpdate = true;
	postUpdate(true);
}

void NSTFormComp::setHiddenState(HiddenState pState, nsbool pEnable)
{
	for (nsuint i = 0; i < mTransforms.size(); ++i)
		setHiddenState(pState, pEnable, i);
}

void NSTFormComp::setInstTrans(const InstTrans & pInsT)
{
	for (nsuint i = 0; i < mTransforms.size(); ++i)
		setInstTrans(pInsT, i);
}

void NSTFormComp::setInstTrans(const InstTrans & pInsT, nsuint pTransformID)
{
	if (pTransformID >= mTransforms.size())
	{
		dprint("NSTFormComp::setInstTrans - Invalid TransformID: " + std::to_string(pTransformID));
		return;
	}

	mTransforms[pTransformID] = pInsT;
	mTransforms[pTransformID].mUpdate = true;
	postUpdate(true);
}

void NSTFormComp::setOrientation(const fquat & pOrientation)
{
	for (nsuint i = 0; i < mTransforms.size(); ++i)
		setOrientation(pOrientation, i);
}

void NSTFormComp::setOrientation(const fquat & pOrientation, nsuint pTransformID)
{
	if (pTransformID >= mTransforms.size())
	{
		dprint("NSTFormComp::setOrientation - Invalid TransformID: " + std::to_string(pTransformID));
		return;
	}

	mTransforms[pTransformID].mOrientation = pOrientation;
	mTransforms[pTransformID].mUpdate = true;
	postUpdate(true);
}

void NSTFormComp::setParentID(const uivec3 & pParentID)
{
	for (nsuint i = 0; i < mTransforms.size(); ++i)
		setParentID(pParentID, i);
}

void NSTFormComp::setParentID(const uivec3 & pParentID, nsuint pTransformID)
{
	if (pTransformID >= mTransforms.size())
	{
		dprint("NSTFormComp::setParentID - Invalid TransformID: " + std::to_string(pTransformID));
		return;
	}

	mTransforms[pTransformID].mParentID = pParentID;
	mTransforms[pTransformID].mUpdate = true;
	postUpdate(true);
}

void NSTFormComp::setParent(const fmat4 & pTransform)
{
	for (nsuint i = 0; i < mTransforms.size(); ++i)
		setParent(pTransform, i);
}

void NSTFormComp::setParent(const fmat4 & pTransform, nsuint pTransformID)
{
	if (pTransformID >= mTransforms.size())
	{
		dprint("NSTFormComp::setParentTransform - Invalid TransformID: " + std::to_string(pTransformID));
		return;
	}

	mTransforms[pTransformID].mParentTransform = pTransform;
	mTransforms[pTransformID].mUpdate = true;
	postUpdate(true);
}

void NSTFormComp::setpos(const fvec3 & pPosition, nsuint pTransformID)
{
	if (pTransformID >= mTransforms.size())
	{
		dprint("NSTFormComp::setPosition - Invalid TransformID: " + std::to_string(pTransformID));
		return;
	}

	mTransforms[pTransformID].mPosition = pPosition;
	mTransforms[pTransformID].mUpdate = true;
	postUpdate(true);
}

void NSTFormComp::setpos(const fvec3 & pPosition)
{
	for (nsuint i = 0; i < mTransforms.size(); ++i)
		setpos(pPosition, i);
}

void NSTFormComp::setRenderID(nsuint pRenderID)
{
	for (nsuint i = 0; i < mTransforms.size(); ++i)
		setRenderID(pRenderID, i);
}

void NSTFormComp::setRenderID(nsuint pRenderID, nsuint pTransformID)
{
	if (pTransformID >= mTransforms.size())
	{
		dprint("NSTFormComp::setPosition - Invalid TransformID: " + std::to_string(pTransformID));
		return;
	}

	mTransforms[pTransformID].mRenderID = pRenderID;
}

void NSTFormComp::setScale(const fvec3 & pScaling)
{
	for (nsuint i = 0; i < mTransforms.size(); ++i)
		setScale(pScaling, i);
}

void NSTFormComp::setScale(const fvec3 & pScaling, nsuint pTransformID)
{
	if (pTransformID >= mTransforms.size())
	{
		dprint("NSTFormComp::setScaling - Invalid TransformID: " + std::to_string(pTransformID));
		return;
	}

	mTransforms[pTransformID].mScaling = pScaling;
	mTransforms[pTransformID].mUpdate = true;
	postUpdate(true);
}

void NSTFormComp::setTransUpdate(bool pUpdate)
{
	for (nsuint i = 0; i < mTransforms.size(); ++i)
		setTransUpdate(pUpdate, i);
}

void NSTFormComp::setTransUpdate(bool pUpdate, nsuint pTransformID)
{
	if (pTransformID >= mTransforms.size())
	{
		dprint("NSTFormComp::setTransUpdate - Invalid TransformID: " + std::to_string(pTransformID));
		return;
	}

	mTransforms[pTransformID].mUpdate = pUpdate;
}

void NSTFormComp::postUpdate(bool pUpdate)
{
	if (pUpdate)
		mXFBData.mUpdate = pUpdate;

	NSComponent::postUpdate(pUpdate);
}

void NSTFormComp::setVisibleTransformCount(nsuint pCount)
{
	if (pCount > mTransforms.size())
	{
		dprint("NSTFormComp::setVisibleTransformCount - visible count larger than transform count");
		return;
	}

	mVisibleCount = pCount;
}

void NSTFormComp::snap(nsuint pTransformID)
{
	if (pTransformID >= mTransforms.size())
	{
		dprint("NSTFormComp::snap - Invalid TransformID: " + std::to_string(pTransformID));
		return;
	}
	if (mTransforms[pTransformID].mSnapToGrid)
	{
		NSTileGrid::snap(mTransforms[pTransformID].mPosition);
		mTransforms[pTransformID].mUpdate = true;
		postUpdate(true);
	}
}

void NSTFormComp::snapX(nsuint pTransformID)
{
	if (pTransformID >= mTransforms.size())
	{
		dprint("NSTFormComp::snap - Invalid TransformID: " + std::to_string(pTransformID));
		return;
	}
	if (mTransforms[pTransformID].mSnapToGrid)
	{
		fvec3 pos = mTransforms[pTransformID].mPosition;
		NSTileGrid::snap(pos);
		mTransforms[pTransformID].mPosition.x = pos.x;
		mTransforms[pTransformID].mUpdate = true;
		postUpdate(true);
	}
}

void NSTFormComp::snapY(nsuint pTransformID)
{
	if (pTransformID >= mTransforms.size())
	{
		dprint("NSTFormComp::snap - Invalid TransformID: " + std::to_string(pTransformID));
		return;
	}
	if (mTransforms[pTransformID].mSnapToGrid)
	{
		fvec3 pos = mTransforms[pTransformID].mPosition;
		NSTileGrid::snap(pos);
		mTransforms[pTransformID].mPosition.y = pos.y;
		mTransforms[pTransformID].mUpdate = true;
		postUpdate(true);
	}
}

void NSTFormComp::snapZ(nsuint pTransformID)
{
	if (pTransformID >= mTransforms.size())
	{
		dprint("NSTFormComp::snap - Invalid TransformID: " + std::to_string(pTransformID));
		return;
	}
	if (mTransforms[pTransformID].mSnapToGrid)
	{
		fvec3 pos = mTransforms[pTransformID].mPosition;
		NSTileGrid::snap(pos);
		mTransforms[pTransformID].mPosition.z = pos.z;
		mTransforms[pTransformID].mUpdate = true;
		postUpdate(true);
	}
}

void NSTFormComp::snap()
{
	for (nsuint i = 0; i < mTransforms.size(); ++i)
		snap(i);
}

void NSTFormComp::snapX()
{
	for (nsuint i = 0; i < mTransforms.size(); ++i)
		snapX(i);
}

void NSTFormComp::snapY()
{
	for (nsuint i = 0; i < mTransforms.size(); ++i)
		snapY(i);
}

void NSTFormComp::snapZ()
{
	for (nsuint i = 0; i < mTransforms.size(); ++i)
		snapZ(i);
}

void NSTFormComp::toggleGridSnap(nsuint pTransformID)
{
	if (pTransformID >= mTransforms.size())
	{
		dprint("NSTFormComp::toggleGridSnap - Invalid TransformID: " + std::to_string(pTransformID));
		return;
	}

	mTransforms[pTransformID].mSnapToGrid = !mTransforms[pTransformID].mSnapToGrid;
	mTransforms[pTransformID].mUpdate = true;
	postUpdate(true);
}

void NSTFormComp::toggleGridSnap()
{
	for (nsuint i = 0; i < mTransforms.size(); ++i)
		toggleGridSnap(i);
}

void NSTFormComp::toggleHiddenState(HiddenState pState, nsuint pTransformID)
{
	if (pTransformID >= mTransforms.size())
	{
		dprint("NSTFormComp::toggleHidden - Invalid TransformID: " + std::to_string(pTransformID));
		return;
	}

	mTransforms[pTransformID].mHiddenState ^= pState;
	mTransforms[pTransformID].mUpdate = true;
	postUpdate(true);
}

void NSTFormComp::toggleHiddenState(HiddenState pState)
{
	auto iter = mTransforms.begin();
	while (iter != mTransforms.end())
	{
		iter->mHiddenState ^= pState;
		++iter;
	}
	postUpdate(true);
}

void NSTFormComp::translate(const fvec3 & pAmount)
{
	for (nsuint i = 0; i < mTransforms.size(); ++i)
		translate(pAmount, i);
}

void NSTFormComp::translate(nsfloat pX, nsfloat pY, nsfloat pZ)
{
	for (nsuint i = 0; i < mTransforms.size(); ++i)
		translate(fvec3(pX,pY,pZ), i);
}

void NSTFormComp::translate(const fvec3 & pAmount, nsuint pTransformID)
{
	if (pTransformID >= mTransforms.size())
	{
		dprint("NSTFormComp::translate - Invalid TransformID: " + std::to_string(pTransformID));
		return;
	}

	mTransforms[pTransformID].mPosition += pAmount;
	mTransforms[pTransformID].mUpdate = true;
	postUpdate(true);
}

void NSTFormComp::translate(nsfloat pX, nsfloat pY, nsfloat pZ, nsuint pTransformID)
{
	translate(fvec3(pX, pY, pZ), pTransformID);
}

void NSTFormComp::translate(DirVec pDirection, nsfloat pAmount)
{
	for (nsuint i = 0; i < mTransforms.size(); ++i)
		translate(pDirection, pAmount, i);
}

void NSTFormComp::translate(Axis pDirection, nsfloat pAmount, nsuint pTransformID)
{
	if (pTransformID >= mTransforms.size())
	{
		dprint("NSTFormComp::rotate - Invalid TransformID: " + std::to_string(pTransformID));
		return;
	}
	fvec3 trans;
	switch (pDirection)
	{
	case(XAxis) :
		trans.set(pAmount, 0, 0);
		break;
	case(YAxis) :
		trans.set(0, pAmount, 0);
		break;
	case(ZAxis) :
		trans.set(0, 0, pAmount);
		break;
	}
	mTransforms[pTransformID].mPosition += trans;
	mTransforms[pTransformID].mUpdate = true;
	postUpdate(true);
}

void NSTFormComp::translate(Axis pDirection, nsfloat pAmount)
{
	for (nsuint i = 0; i < mTransforms.size(); ++i)
		translate(pDirection, pAmount, i);
}

void NSTFormComp::translate(DirVec pDirection, nsfloat pAmount, nsuint pTransformID)
{
	translate(dirVec(pDirection)*pAmount, pTransformID);
}

void NSTFormComp::translateX(nsfloat pAmount)
{
	for (nsuint i = 0; i < mTransforms.size(); ++i)
		translateX(pAmount, i);
}

void NSTFormComp::translateY(nsfloat pAmount)
{
	for (nsuint i = 0; i < mTransforms.size(); ++i)
		translateY(pAmount, i);
}

void NSTFormComp::translateZ(nsfloat pAmount)
{
	for (nsuint i = 0; i < mTransforms.size(); ++i)
		translateZ(pAmount, i);
}

void NSTFormComp::translateX(nsfloat pAmount, nsuint pTransformID)
{
	translate(pAmount, 0.0f, 0.0f, pTransformID);
}

void NSTFormComp::translateY(nsfloat pAmount, nsuint pTransformID)
{
	translate(0.0f, pAmount, 0.0f, pTransformID);
}

void NSTFormComp::translateZ(nsfloat pAmount, nsuint pTransformID)
{
	translate(0.0f, 0.0f, pAmount, pTransformID);
}

NSTFormComp & NSTFormComp::operator=(const NSTFormComp & pRHSComp)
{
	mTransforms.resize(pRHSComp.mTransforms.size());
	for (nsuint i = 0; i < mTransforms.size(); ++i)
		mTransforms[i] = pRHSComp.mTransforms[i];
	postUpdate(true);
	return (*this);
}

nsstring NSTFormComp::getTypeString()
{
	return TFORM_COMP_TYPESTRING;
}

void NSTFormComp::InstTrans::compute()
{
	mTransform.set(rotationMat3(mOrientation) % mScaling);
	mTransform.setColumn(3, mPosition.x, mPosition.y, mPosition.z, 1);

	fvec4 col3(-mPosition.x, -mPosition.y, -mPosition.z, 1.0f);
	mPOVTransform.rotationFrom(mOrientation).transpose();
	mPOVTransform.setColumn(3, mPOVTransform[0] * col3, mPOVTransform[1] * col3, mPOVTransform[2] * col3, 1.0f);

	if (mParentEnabled)
	{
		mTransform = mParentTransform * mTransform;
		mPOVTransform = mPOVTransform *inverse(mParentTransform);
	}
}