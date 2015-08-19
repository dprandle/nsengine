/*! 
	\file nstformcomp.cpp
	
	\brief Definition file for NSTFormComp class

	This file contains all of the neccessary definitions for the NSTFormComp class.

	\author Daniel Randle
	\date November 23 2013
	\copywrite Earth Banana Games 2013
*/

#include <nstform_comp.h>
#include <nsrender_comp.h>
#include <nsentity.h>
#include <nstimer.h>
#include <nstile_grid.h>
#include <nsmesh_manager.h>
#include <nsmesh.h>
#include <nsengine.h>

NSTFormComp::NSTFormComp():
mTransformBuffer(nsbuffer_object::array, nsbuffer_object::storage_mutable),
mTransformIDBuffer(nsbuffer_object::array, nsbuffer_object::storage_mutable),
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

uint32 NSTFormComp::add()
{
	return add(InstTrans());
}

uint32 NSTFormComp::add(const InstTrans & trans)
{
	mTransforms.push_back(trans);
	post_update(true);
	mBufferResize = true;

	if (mTransformFB)
	{
		enableTransformFeedback(false);
		enableTransformFeedback(true);
	}

	return static_cast<uint32>(mTransforms.size()) - 1;
}

uint32 NSTFormComp::add(uint32 pHowMany)
{
	// Get the transformID of the last transform before adding pHowMany and return it
	uint32 ret = static_cast<uint32>(mTransforms.size()) - 1;
	mTransforms.resize(mTransforms.size() + pHowMany);
	post_update(true);
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

void NSTFormComp::changeScale(const fvec3 & pAmount, uint32 pTransformID)
{
	if (pTransformID >= mTransforms.size())
	{
		dprint("NSTFormComp::changeScale - Invalid TransformID: " + std::to_string(pTransformID));
		return;
	}
	mTransforms[pTransformID].mScaling += pAmount;
	mTransforms[pTransformID].mUpdate = true;
	post_update(true);
}

void NSTFormComp::changeScale(float pX, float pY, float pZ, uint32 pTransformID)
{
	changeScale(fvec3(pX, pY, pZ), pTransformID);
}

void NSTFormComp::pup(nsfile_pupper * p)
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

void NSTFormComp::computeTransform(uint32 pTransformID)
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

void NSTFormComp::enableParent(bool pEnable, uint32 pTransformID)
{
	if (pTransformID >= mTransforms.size())
	{
		dprint("NSTFormComp::enableParent - Invalid TransformID: " + std::to_string(pTransformID));
		return;
	}

	mTransforms[pTransformID].mParentEnabled = pEnable;
	mTransforms[pTransformID].mUpdate = true;
	post_update(true);
}

/*!
Enable transform feedback for this entity. Pass in pVertCount to set the buffer size necessary to hold
*/
bool NSTFormComp::enableTransformFeedback(bool pEnable)
{
	mTransformFB = pEnable;

	if (mTransformFB)
	{
		// Figure out how much mem to allocate
		NSRenderComp * mRenComp = m_owner->get<NSRenderComp>();
		if (mRenComp == NULL || mTransforms.empty())
			return false;

		nsmesh * mesh = nsengine.resource<nsmesh>(mRenComp->meshID());
		if (mesh == NULL)
			return false;

		// Figure out how much memory to allocate in the buffers.. this should be number of tranforms times number of verts
		uint32 allocAmount = mesh->vert_count() * static_cast<uint32>(mTransforms.size());
		uint32 instancePerDraw = static_cast<uint32>(mTransforms.size());
		uint32 totalIntanceCount = static_cast<uint32>(mTransforms.size());

		// If the total is larger than 4mB, then find out how many 4 mB buffers are needed
		uint32 bufCount = 1;
		if (allocAmount > MAX_TF_BUFFER_SIZE)
		{
			bufCount = allocAmount / MAX_TF_BUFFER_SIZE + 1;
			allocAmount = MAX_TF_BUFFER_SIZE;
			instancePerDraw = MAX_TF_BUFFER_SIZE / mesh->vert_count();
		}

		if (bufCount > MAX_TF_BUFFER_COUNT)
			return false;



		mXFBData.mXFBBuffers.resize(bufCount);
		for (uint32 bufI = 0; bufI < bufCount; ++bufI)
		{
			XFBBuffer * buf = &mXFBData.mXFBBuffers[bufI];
			buf->mAllocAmount = allocAmount;

			buf->mTFFeedbackObj = new nsxfb_object();
			buf->mXFBVAO = new nsvertex_array_object();
			buf->mXBWorldPosBuf = new nsbuffer_object(nsbuffer_object::array, nsbuffer_object::storage_mutable);
			buf->mXBTexCoordBuf = new nsbuffer_object(nsbuffer_object::array, nsbuffer_object::storage_mutable);
			buf->mXBNormalBuf = new nsbuffer_object(nsbuffer_object::array, nsbuffer_object::storage_mutable);
			buf->mXBTangentBuf = new nsbuffer_object(nsbuffer_object::array, nsbuffer_object::storage_mutable);

			buf->mXFBVAO->init_gl();
			buf->mTFFeedbackObj->init_gl();
			buf->mXBWorldPosBuf->init_gl();
			buf->mXBTexCoordBuf->init_gl();
			buf->mXBNormalBuf->init_gl();
			buf->mXBTangentBuf->init_gl();

			// Set up VAO for draw on other stuff
			buf->mXFBVAO->bind();

			buf->mXBWorldPosBuf->bind();
			buf->mXBWorldPosBuf->allocate<fvec3>(nsbuffer_object::mutable_dynamic_copy, buf->mAllocAmount);
			buf->mXFBVAO->add(buf->mXBWorldPosBuf, nsshader::loc_position);
			buf->mXFBVAO->vertex_attrib_ptr(nsshader::loc_position, 4, GL_FLOAT, GL_FALSE, sizeof(fvec3), 0);

			buf->mXBTexCoordBuf->bind();
			buf->mXBTexCoordBuf->allocate<fvec3>(nsbuffer_object::mutable_dynamic_copy, buf->mAllocAmount);
			buf->mXFBVAO->add(buf->mXBTexCoordBuf, nsshader::loc_tex_coords);
			buf->mXFBVAO->vertex_attrib_ptr(nsshader::loc_tex_coords, 3, GL_FLOAT, GL_FALSE, sizeof(fvec3), 0);

			buf->mXBNormalBuf->bind();
			buf->mXBNormalBuf->allocate<fvec3>(nsbuffer_object::mutable_dynamic_copy, buf->mAllocAmount);
			buf->mXFBVAO->add(buf->mXBNormalBuf, nsshader::loc_normal);
			buf->mXFBVAO->vertex_attrib_ptr(nsshader::loc_normal, 3, GL_FLOAT, GL_FALSE, sizeof(fvec3), 0);

			buf->mXBTangentBuf->bind();
			buf->mXBTangentBuf->allocate<fvec3>(nsbuffer_object::mutable_dynamic_copy, buf->mAllocAmount);
			buf->mXFBVAO->add(buf->mXBTangentBuf, nsshader::loc_tangent);
			buf->mXFBVAO->vertex_attrib_ptr(nsshader::loc_tangent, 3, GL_FLOAT, GL_FALSE, sizeof(fvec3), 0);

			buf->mXFBVAO->unbind();

			buf->mXBWorldPosBuf->set_target(nsbuffer_object::transform_feedback);
			buf->mXBTexCoordBuf->set_target(nsbuffer_object::transform_feedback);
			buf->mXBNormalBuf->set_target(nsbuffer_object::transform_feedback);
			buf->mXBTangentBuf->set_target(nsbuffer_object::transform_feedback);
			buf->mTFFeedbackObj->bind();
			buf->mXBWorldPosBuf->bind(nsshader::loc_position);
			buf->mXBTexCoordBuf->bind(nsshader::loc_tex_coords);
			buf->mXBNormalBuf->bind(nsshader::loc_normal);
			buf->mXBTangentBuf->bind(nsshader::loc_tangent);
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
		for (uint32 bufI = 0; bufI < mXFBData.mXFBBuffers.size(); ++bufI)
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
	mTransformBuffer.init_gl();
	mTransformIDBuffer.init_gl();
}

const fvec3 NSTFormComp::dirVec(DirVec pDirection, uint32 pTransformID) const
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

NSTFormComp::InstTrans & NSTFormComp::instTrans(uint32 pTransformID)
{
	return mTransforms[pTransformID];
}

const NSTFormComp::InstanceVec & NSTFormComp::transformVec() const
{
	return mTransforms;
}

const fquat & NSTFormComp::orientation(uint32 pTransformID) const
{
	return mTransforms[pTransformID].mOrientation;
}

const fvec3 & NSTFormComp::lpos(uint32 pTransformID) const
{
	return mTransforms[pTransformID].mPosition;
}

const fmat4 & NSTFormComp::pov(uint32 pTransformID) const
{
	return mTransforms[pTransformID].mPOVTransform;
}

uint32 NSTFormComp::renderID(uint32 pTransformID) const
{
	return mTransforms[pTransformID].mRenderID;
}

const fvec3 & NSTFormComp::scaling(uint32 pTransformID) const
{
	return mTransforms[pTransformID].mScaling;
}

const uivec3 & NSTFormComp::parentid(uint32 pTransformID) const
{
	return mTransforms[pTransformID].mParentID;
}

NSTFormComp::XFBData * NSTFormComp::xfbData()
{
	return &mXFBData;
}

const fmat4 & NSTFormComp::parent(uint32 pTransformID) const
{
	return mTransforms[pTransformID].mParentTransform;
}

const fmat4 & NSTFormComp::transform(uint32 pTransformID) const
{
	return mTransforms[pTransformID].mTransform;
}

nsbuffer_object * NSTFormComp::transformBuffer()
{
	return &mTransformBuffer;
}

nsbuffer_object * NSTFormComp::transformIDBuffer()
{
	return &mTransformIDBuffer;
}

uint32 NSTFormComp::count() const
{
	return static_cast<uint32>(mTransforms.size());
}

uint32 NSTFormComp::visibleCount() const
{
	return mVisibleCount;
}

fvec3 NSTFormComp::wpos(uint32 pTransformID)
{
	if (mTransforms[pTransformID].mParentEnabled)
		return (mTransforms[pTransformID].mParentTransform * fvec4(mTransforms[pTransformID].mPosition, 1.0f)).xyz();

	return mTransforms[pTransformID].mPosition;
}

bool NSTFormComp::snapped(uint32 pTransformID) const
{
	return mTransforms[pTransformID].mSnapToGrid;
}

int32 NSTFormComp::hiddenState(uint32 pTransformID) const
{
	return mTransforms[pTransformID].mHiddenState;
}

bool NSTFormComp::parentEnabled(uint32 pTransformID) const
{
	return mTransforms[pTransformID].mParentEnabled;
}

bool NSTFormComp::transformFeedback()
{
	return mTransformFB;
}

const bool NSTFormComp::transUpdate(uint32 pTransformID) const
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

uint32 NSTFormComp::remove(uint32 pTransformID)
{
	if (pTransformID >= mTransforms.size())
	{
		dprint("NSTFormComp::removeTransform - Invalid TransformID or Invalid Operation; Transform ID: " + std::to_string(pTransformID));
		return count();
	}
	else if (mTransforms.size() == 1)
	{
		mTransforms.pop_back();
		return 0;
	}

	mTransforms[pTransformID] = mTransforms.back();
	mTransforms[pTransformID].mUpdate = true;
	mTransforms.pop_back();
	post_update(true);
	mBufferResize = true;

	if (mTransformFB)
	{
		enableTransformFeedback(false);
		enableTransformFeedback(true);
	}

	return count();
}

uint32 NSTFormComp::remove(uint32 pFirst, uint32 pLast)
{
	if (pFirst >= mTransforms.size() || pLast >= mTransforms.size() || pFirst >= pLast || (pLast - pFirst + 1) == mTransforms.size())
	{
		dprint("NSTFormComp::removeTransforms - Invalid TransformID or Invalid operation");
		return count();
	}
	uint32 resizeNum = count() - (pLast - pFirst + 1);

	// Move all the elements in the back of the last that arent part of the erase
	// range to the first index that the erase begins.. increasing the index and
	// popping an item off the back every iteration..
	for (uint32 i = pFirst; i <= pLast; ++i)
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
	post_update(true);

	if (mTransformFB)
	{
		enableTransformFeedback(false);
		enableTransformFeedback(true);
	}

	return count();
}

void NSTFormComp::rotate(const fvec4 & axisAngle, uint32 tformid)
{
	if (tformid >= mTransforms.size())
	{
		dprint("NSTFormComp::rotate - Invalid TransformID: " + std::to_string(tformid));
		return;
	}

	mTransforms[tformid].mOrientation = ::orientation(axisAngle) * mTransforms[tformid].mOrientation;
	mTransforms[tformid].mUpdate = true;
	post_update(true);
}

void NSTFormComp::rotate(const fvec4 & axisAngle)
{
	for (uint32 i = 0; i < mTransforms.size(); ++i)
		rotate(axisAngle, i);
}

void NSTFormComp::rotate(Axis pAxis, float pAngle, uint32 pTransformID)
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
	for (uint32 i = 0; i < mTransforms.size(); ++i)
		rotate(pAxis, pAngle, i);
}

void NSTFormComp::rotate(const fquat & pQuat, uint32 pTransformID)
{
	if (pTransformID >= mTransforms.size())
	{
		dprint("NSTFormComp::rotate - Invalid TransformID: " + std::to_string(pTransformID));
		return;
	}

	mTransforms[pTransformID].mOrientation = pQuat * mTransforms[pTransformID].mOrientation;
	mTransforms[pTransformID].mUpdate = true;
	post_update(true);
}

void NSTFormComp::rotate(const fquat & pQuat)
{
	for (uint32 i = 0; i < mTransforms.size(); ++i)
		rotate(pQuat, i);
}

void NSTFormComp::rotate(const fvec3 & euler)
{
	for (uint32 i = 0; i < mTransforms.size(); ++i)
		rotate(euler, i);
}

void NSTFormComp::rotate(const fvec3 & euler, uint32 pTransformID)
{
	rotate(::orientation(euler, fvec3::XYZ), pTransformID);
}

void NSTFormComp::rotate(DirVec dir, float angle, uint32 tformid)
{
	rotate(fvec4(dirVec(dir), angle), tformid);
}

void NSTFormComp::rotate(DirVec dir, float angle)
{
	for (uint32 i = 0; i < mTransforms.size(); ++i)
		rotate(dir, angle, i);
}

void NSTFormComp::scale(const fvec3 & pAmount, uint32 pTransformID)
{
	if (pTransformID >= mTransforms.size())
	{
		dprint("NSTFormComp::scale - Invalid TransformID: " + std::to_string(pTransformID));
		return;
	}

	mTransforms[pTransformID].mScaling %= pAmount;
	mTransforms[pTransformID].mUpdate = true;
	post_update(true);
}

void NSTFormComp::scale(float pX, float pY, float pZ, uint32 pTransformID)
{
	scale(fvec3(pX, pY, pZ), pTransformID);
}

void NSTFormComp::scale(const fvec3 & pAmount)
{
	for (uint32 i = 0; i < mTransforms.size(); ++i)
		scale(pAmount, i);
}

void NSTFormComp::scale(float pX, float pY, float pZ)
{
	for (uint32 i = 0; i < mTransforms.size(); ++i)
		scale(pX, pY, pZ, i);
}

void NSTFormComp::setBufferResize(bool pResize)
{
	mBufferResize = pResize;
}

void NSTFormComp::enableSnap(bool pSnap, uint32 pTransformID)
{
	if (pTransformID >= mTransforms.size())
	{
		dprint("NSTFormComp::setGridSnap - Invalid TransformID: " + std::to_string(pTransformID));
		return;
	}

	mTransforms[pTransformID].mSnapToGrid = pSnap;
	mTransforms[pTransformID].mUpdate = true;
	post_update(true);
}

void NSTFormComp::enableSnap(bool pSnap)
{
	for (uint32 i = 0; i < mTransforms.size(); ++i)
		enableSnap(pSnap, i);
}

void NSTFormComp::setHiddenState(HiddenState pState, bool pEnable, uint32 pTransformID)
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
	post_update(true);
}

void NSTFormComp::setHiddenState(HiddenState pState, bool pEnable)
{
	for (uint32 i = 0; i < mTransforms.size(); ++i)
		setHiddenState(pState, pEnable, i);
}

void NSTFormComp::setInstTrans(const InstTrans & pInsT)
{
	for (uint32 i = 0; i < mTransforms.size(); ++i)
		setInstTrans(pInsT, i);
}

void NSTFormComp::setInstTrans(const InstTrans & pInsT, uint32 pTransformID)
{
	if (pTransformID >= mTransforms.size())
	{
		dprint("NSTFormComp::setInstTrans - Invalid TransformID: " + std::to_string(pTransformID));
		return;
	}

	mTransforms[pTransformID] = pInsT;
	mTransforms[pTransformID].mUpdate = true;
	post_update(true);
}

void NSTFormComp::setOrientation(const fquat & pOrientation)
{
	for (uint32 i = 0; i < mTransforms.size(); ++i)
		setOrientation(pOrientation, i);
}

void NSTFormComp::setOrientation(const fquat & pOrientation, uint32 pTransformID)
{
	if (pTransformID >= mTransforms.size())
	{
		dprint("NSTFormComp::setOrientation - Invalid TransformID: " + std::to_string(pTransformID));
		return;
	}

	mTransforms[pTransformID].mOrientation = pOrientation;
	mTransforms[pTransformID].mUpdate = true;
	post_update(true);
}

void NSTFormComp::setParentID(const uivec3 & pParentID)
{
	for (uint32 i = 0; i < mTransforms.size(); ++i)
		setParentID(pParentID, i);
}

void NSTFormComp::setParentID(const uivec3 & pParentID, uint32 pTransformID)
{
	if (pTransformID >= mTransforms.size())
	{
		dprint("NSTFormComp::setParentID - Invalid TransformID: " + std::to_string(pTransformID));
		return;
	}

	mTransforms[pTransformID].mParentID = pParentID;
	mTransforms[pTransformID].mUpdate = true;
	post_update(true);
}

void NSTFormComp::setParent(const fmat4 & pTransform)
{
	for (uint32 i = 0; i < mTransforms.size(); ++i)
		setParent(pTransform, i);
}

void NSTFormComp::setParent(const fmat4 & pTransform, uint32 pTransformID)
{
	if (pTransformID >= mTransforms.size())
	{
		dprint("NSTFormComp::setParentTransform - Invalid TransformID: " + std::to_string(pTransformID));
		return;
	}

	mTransforms[pTransformID].mParentTransform = pTransform;
	mTransforms[pTransformID].mUpdate = true;
	post_update(true);
}

void NSTFormComp::setpos(const fvec3 & pPosition, uint32 pTransformID)
{
	if (pTransformID >= mTransforms.size())
	{
		dprint("NSTFormComp::setPosition - Invalid TransformID: " + std::to_string(pTransformID));
		return;
	}

	mTransforms[pTransformID].mPosition = pPosition;
	mTransforms[pTransformID].mUpdate = true;
	post_update(true);
}

void NSTFormComp::setpos(const fvec3 & pPosition)
{
	for (uint32 i = 0; i < mTransforms.size(); ++i)
		setpos(pPosition, i);
}

void NSTFormComp::setRenderID(uint32 pRenderID)
{
	for (uint32 i = 0; i < mTransforms.size(); ++i)
		setRenderID(pRenderID, i);
}

void NSTFormComp::setRenderID(uint32 pRenderID, uint32 pTransformID)
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
	for (uint32 i = 0; i < mTransforms.size(); ++i)
		setScale(pScaling, i);
}

void NSTFormComp::setScale(const fvec3 & pScaling, uint32 pTransformID)
{
	if (pTransformID >= mTransforms.size())
	{
		dprint("NSTFormComp::setScaling - Invalid TransformID: " + std::to_string(pTransformID));
		return;
	}

	mTransforms[pTransformID].mScaling = pScaling;
	mTransforms[pTransformID].mUpdate = true;
	post_update(true);
}

void NSTFormComp::setTransUpdate(bool pUpdate)
{
	for (uint32 i = 0; i < mTransforms.size(); ++i)
		setTransUpdate(pUpdate, i);
}

void NSTFormComp::setTransUpdate(bool pUpdate, uint32 pTransformID)
{
	if (pTransformID >= mTransforms.size())
	{
		dprint("NSTFormComp::setTransUpdate - Invalid TransformID: " + std::to_string(pTransformID));
		return;
	}

	mTransforms[pTransformID].mUpdate = pUpdate;
}

void NSTFormComp::post_update(bool pUpdate)
{
	if (pUpdate)
		mXFBData.mUpdate = pUpdate;

	NSComponent::post_update(pUpdate);
}

void NSTFormComp::setVisibleTransformCount(uint32 pCount)
{
	if (pCount > mTransforms.size())
	{
		dprint("NSTFormComp::setVisibleTransformCount - visible count larger than transform count");
		return;
	}

	mVisibleCount = pCount;
}

void NSTFormComp::snap(uint32 pTransformID)
{
	if (pTransformID >= mTransforms.size())
	{
		dprint("NSTFormComp::snap - Invalid TransformID: " + std::to_string(pTransformID));
		return;
	}
	if (mTransforms[pTransformID].mSnapToGrid)
	{
		nstile_grid::snap(mTransforms[pTransformID].mPosition);
		mTransforms[pTransformID].mUpdate = true;
		post_update(true);
	}
}

void NSTFormComp::snapX(uint32 pTransformID)
{
	if (pTransformID >= mTransforms.size())
	{
		dprint("NSTFormComp::snap - Invalid TransformID: " + std::to_string(pTransformID));
		return;
	}
	if (mTransforms[pTransformID].mSnapToGrid)
	{
		fvec3 pos = mTransforms[pTransformID].mPosition;
		nstile_grid::snap(pos);
		mTransforms[pTransformID].mPosition.x = pos.x;
		mTransforms[pTransformID].mUpdate = true;
		post_update(true);
	}
}

void NSTFormComp::snapY(uint32 pTransformID)
{
	if (pTransformID >= mTransforms.size())
	{
		dprint("NSTFormComp::snap - Invalid TransformID: " + std::to_string(pTransformID));
		return;
	}
	if (mTransforms[pTransformID].mSnapToGrid)
	{
		fvec3 pos = mTransforms[pTransformID].mPosition;
		nstile_grid::snap(pos);
		mTransforms[pTransformID].mPosition.y = pos.y;
		mTransforms[pTransformID].mUpdate = true;
		post_update(true);
	}
}

void NSTFormComp::snapZ(uint32 pTransformID)
{
	if (pTransformID >= mTransforms.size())
	{
		dprint("NSTFormComp::snap - Invalid TransformID: " + std::to_string(pTransformID));
		return;
	}
	if (mTransforms[pTransformID].mSnapToGrid)
	{
		fvec3 pos = mTransforms[pTransformID].mPosition;
		nstile_grid::snap(pos);
		mTransforms[pTransformID].mPosition.z = pos.z;
		mTransforms[pTransformID].mUpdate = true;
		post_update(true);
	}
}

void NSTFormComp::snap()
{
	for (uint32 i = 0; i < mTransforms.size(); ++i)
		snap(i);
}

void NSTFormComp::snapX()
{
	for (uint32 i = 0; i < mTransforms.size(); ++i)
		snapX(i);
}

void NSTFormComp::snapY()
{
	for (uint32 i = 0; i < mTransforms.size(); ++i)
		snapY(i);
}

void NSTFormComp::snapZ()
{
	for (uint32 i = 0; i < mTransforms.size(); ++i)
		snapZ(i);
}

void NSTFormComp::toggleGridSnap(uint32 pTransformID)
{
	if (pTransformID >= mTransforms.size())
	{
		dprint("NSTFormComp::toggleGridSnap - Invalid TransformID: " + std::to_string(pTransformID));
		return;
	}

	mTransforms[pTransformID].mSnapToGrid = !mTransforms[pTransformID].mSnapToGrid;
	mTransforms[pTransformID].mUpdate = true;
	post_update(true);
}

void NSTFormComp::toggleGridSnap()
{
	for (uint32 i = 0; i < mTransforms.size(); ++i)
		toggleGridSnap(i);
}

void NSTFormComp::toggleHiddenState(HiddenState pState, uint32 pTransformID)
{
	if (pTransformID >= mTransforms.size())
	{
		dprint("NSTFormComp::toggleHidden - Invalid TransformID: " + std::to_string(pTransformID));
		return;
	}

	mTransforms[pTransformID].mHiddenState ^= pState;
	mTransforms[pTransformID].mUpdate = true;
	post_update(true);
}

void NSTFormComp::toggleHiddenState(HiddenState pState)
{
	auto iter = mTransforms.begin();
	while (iter != mTransforms.end())
	{
		iter->mHiddenState ^= pState;
		++iter;
	}
	post_update(true);
}

void NSTFormComp::translate(const fvec3 & pAmount)
{
	for (uint32 i = 0; i < mTransforms.size(); ++i)
		translate(pAmount, i);
}

void NSTFormComp::translate(float pX, float pY, float pZ)
{
	for (uint32 i = 0; i < mTransforms.size(); ++i)
		translate(fvec3(pX,pY,pZ), i);
}

void NSTFormComp::translate(const fvec3 & pAmount, uint32 pTransformID)
{
	if (pTransformID >= mTransforms.size())
	{
		dprint("NSTFormComp::translate - Invalid TransformID: " + std::to_string(pTransformID));
		return;
	}

	mTransforms[pTransformID].mPosition += pAmount;
	mTransforms[pTransformID].mUpdate = true;
	post_update(true);
}

void NSTFormComp::translate(float pX, float pY, float pZ, uint32 pTransformID)
{
	translate(fvec3(pX, pY, pZ), pTransformID);
}

void NSTFormComp::translate(DirVec pDirection, float pAmount)
{
	for (uint32 i = 0; i < mTransforms.size(); ++i)
		translate(pDirection, pAmount, i);
}

void NSTFormComp::translate(Axis pDirection, float pAmount, uint32 pTransformID)
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
	post_update(true);
}

void NSTFormComp::translate(Axis pDirection, float pAmount)
{
	for (uint32 i = 0; i < mTransforms.size(); ++i)
		translate(pDirection, pAmount, i);
}

void NSTFormComp::translate(DirVec pDirection, float pAmount, uint32 pTransformID)
{
	translate(dirVec(pDirection)*pAmount, pTransformID);
}

void NSTFormComp::translateX(float pAmount)
{
	for (uint32 i = 0; i < mTransforms.size(); ++i)
		translateX(pAmount, i);
}

void NSTFormComp::translateY(float pAmount)
{
	for (uint32 i = 0; i < mTransforms.size(); ++i)
		translateY(pAmount, i);
}

void NSTFormComp::translateZ(float pAmount)
{
	for (uint32 i = 0; i < mTransforms.size(); ++i)
		translateZ(pAmount, i);
}

void NSTFormComp::translateX(float pAmount, uint32 pTransformID)
{
	translate(pAmount, 0.0f, 0.0f, pTransformID);
}

void NSTFormComp::translateY(float pAmount, uint32 pTransformID)
{
	translate(0.0f, pAmount, 0.0f, pTransformID);
}

void NSTFormComp::translateZ(float pAmount, uint32 pTransformID)
{
	translate(0.0f, 0.0f, pAmount, pTransformID);
}

NSTFormComp & NSTFormComp::operator=(const NSTFormComp & pRHSComp)
{
	mTransforms.resize(pRHSComp.mTransforms.size());
	for (uint32 i = 0; i < mTransforms.size(); ++i)
		mTransforms[i] = pRHSComp.mTransforms[i];
	post_update(true);
	return (*this);
}

void NSTFormComp::InstTrans::compute()
{
	mTransform.set(rotation_mat3(mOrientation) % mScaling);
	mTransform.set_column(3, mPosition.x, mPosition.y, mPosition.z, 1);

	fvec4 col3(-mPosition.x, -mPosition.y, -mPosition.z, 1.0f);
	mPOVTransform.rotation_from(mOrientation).transpose();
	mPOVTransform.set_column(3, mPOVTransform[0] * col3, mPOVTransform[1] * col3, mPOVTransform[2] * col3, 1.0f);

	if (mParentEnabled)
	{
		mTransform = mParentTransform * mTransform;
		mPOVTransform = inverse(mTransform);
	}
}
