/*! 
	\file nstformcomp.h
	
	\brief Header file for NSTFormComp class

	This file contains all of the neccessary declarations for the NSTFormComp class.

	\author Daniel Randle
	\date November 23 2013
	\copywrite Earth Banana Games 2013
*/

#ifndef NSTFORMCOMP_H
#define NSTFORMCOMP_H

#include <nsmath.h>
#include <nscomponent.h>
#include <vector>
#include <nsbufferobject.h>
#include <nstransformfeedbackobject.h>
#include <nsvertexarrayobject.h>

class NSTimer;

class NSTFormComp : public NSComponent
{
public:
	enum Axis {
		XAxis,
		YAxis,
		ZAxis
	};

	enum DirVec {
		Right,
		Up,
		Target
	};

	enum HiddenState
	{
		LayerHide = 0x01,
		ObjectHide = 0x02,
		Show = 0x04,
		Hide = 0x08
	};

	struct XFBBuffer
	{
		XFBBuffer():
			mXFBVAO(NULL),
			mTFFeedbackObj(NULL),
			mXBWorldPosBuf(NULL),
			mXBTexCoordBuf(NULL),
			mXBNormalBuf(NULL),
			mXBTangentBuf(NULL),
			mAllocAmount(0),
			mInstanceCount(0)
		{}

		NSVertexArrayObject * mXFBVAO;
		NSTransformFeedbackObject * mTFFeedbackObj;
		NSBufferObject * mXBWorldPosBuf;
		NSBufferObject * mXBTexCoordBuf;
		NSBufferObject * mXBNormalBuf;
		NSBufferObject * mXBTangentBuf;
		nsuint mAllocAmount;
		nsuint mInstanceCount;
	};

	typedef std::vector<XFBBuffer> XFBBufferArray;

	struct XFBData
	{
		XFBData() :
			mXFBBuffers(),
			mUpdate(true)
		{}

		XFBBufferArray mXFBBuffers;
		nsbool mUpdate;
	};

	struct InstTrans
	{
		InstTrans() :
		mHiddenState(0),
		mOrientation(),
		mPosition(),
		mScaling(1.0f, 1.0f, 1.0f),
		mParentTransform(),
		mPOVTransform(),
		mTransform(),
		mUpdate(true),
		mRenderID(0),
		mSnapToGrid(true),
		mParentEnabled(false),
		mParentID()
		{}

		void compute();

		int mHiddenState;
		fquat mOrientation;
		fvec3 mPosition;
		fvec3 mScaling;
		fmat4 mParentTransform;
		fmat4 mPOVTransform;
		fmat4 mTransform;
		nsbool mUpdate;
		nsuint mRenderID;
		nsbool mSnapToGrid;
		nsbool mParentEnabled;
		uivec3 mParentID;
	};

	typedef std::vector<InstTrans> InstanceVec;

	template <class PUPer>
	friend void pup(PUPer & p, NSTFormComp & tc);

	NSTFormComp();

	~NSTFormComp();

	nsuint add();

	nsuint add(const InstTrans & trans);

	nsuint add(nsuint pHowMany);

	bool bufferResize() const;

	void changeScale(const fvec3 & pAmount, nsuint pTransformID=0);

	void changeScale(nsfloat pX, nsfloat pY, nsfloat pZ, nsuint pTransformID = 0);

	void computeTransform(nsuint pTransformID = 0);

	NSTFormComp * copy(const NSComponent* pToCopy);

	void enableParent(nsbool pEnable, nsuint pTransformID = 0);

	bool enableTransformFeedback(nsbool pEnable);

	const fvec3 dirVec(DirVec pDirection, nsuint pTransformID = 0) const;

	NSBufferObject * transformBuffer();

	NSBufferObject * transformIDBuffer();

	const InstanceVec & transformVec() const;

	const fquat & orientation(nsuint pTransformID = 0) const;

	const uivec3 & parentid(nsuint pTransformID = 0) const;

	const fmat4 & parent(nsuint pTransformID = 0) const;

	const fvec3 & lpos(nsuint pTransformID = 0) const;

	const fmat4 & pov(nsuint pTransformID = 0) const;

	nsuint renderID(nsuint pTransformID) const;

	const fvec3 & scaling(nsuint pTransformID = 0) const;

	const fmat4 & transform(nsuint pTransformID = 0) const;

	int hiddenState(nsuint pTransformID = 0) const;

	InstTrans & instTrans(nsuint pTransformID = 0);

	nsuint count() const;

	XFBData * xfbData();

	nsuint visibleCount() const;

	fvec3 wpos(nsuint pTransformID = 0);

	void init();

	nsbool snapped(nsuint pTransformID = 0) const;

	nsbool calcLocalTForm(nsuint pTransformID = 0) const;

	nsbool parentEnabled(nsuint pTransformID = 0) const;

	nsbool transformFeedback();

	const nsbool transUpdate(nsuint pTransformID = 0) const;

	virtual void pup(NSFilePUPer * p);

	void release();

	nsuint remove(nsuint pTransformID);

	nsuint remove(nsuint pFirst, nsuint pLast);

	void rotate(Axis pAxis, float pAngle, nsuint pTransformID);

	void rotate(Axis pAxis, float pAngle);

	void rotate(const fvec4 & axisAngle, nsuint pTransformID);

	void rotate(const fvec4 & axisAngle);

	void rotate(const fvec3 & euler);

	void rotate(const fvec3 & euler, nsuint pTransformID);

	void rotate(const fquat & pQuat, nsuint pTransformID);

	void rotate(const fquat & pQuat);

	void rotate(DirVec dir, nsfloat angle, nsuint tformid);

	void rotate(DirVec dir, nsfloat angle);

	void scale(const fvec3 & pAmount, nsuint pTransformID);

	void scale(const fvec3 & pAmount);

	void scale(nsfloat pX, nsfloat pY, nsfloat pZ, nsuint pTransformID);

	void scale(nsfloat pX, nsfloat pY, nsfloat pZ);

	void setBufferResize(nsbool pResize);

	void enableSnap(nsbool pSnap, nsuint pTransformID = 0);

	void enableSnap(nsbool pSnap);

	void setHiddenState(HiddenState pState, bool pEnable, nsuint pTransformID);

	void setHiddenState(HiddenState pState, nsbool pEnable);

	void setInstTrans(const InstTrans & pInsT, nsuint pTransformID);

	void setInstTrans(const InstTrans & pInsT);

	void setOrientation(const fquat & pOrientation, nsuint pTransformID);

	void setOrientation(const fquat & pOrientation);

	void setParent(const fmat4 & pTransform, nsuint pTransformID);

	void setParent(const fmat4 & pTransform);

	void setParentID(const uivec3 & pParentID, nsuint pTransformID);

	void setParentID(const uivec3 & pParentID);

	void setpos(const fvec3 & pPosition, nsuint pTransformID);

	void setpos(const fvec3 & pPosition);

	void setRenderID(nsuint pRenderID, nsuint pTransformID);

	void setRenderID(nsuint pRenderID);

	void setScale(const fvec3 & pScaling, nsuint pTransformID);

	void setScale(const fvec3 & pScaling);

	void setTransUpdate(bool pUpdate, nsuint pTransformID);

	void setTransUpdate(bool pUpdate);

	void postUpdate(bool pUpdate);

	void setVisibleTransformCount(nsuint pCount);

	void snap(nsuint pTransformID);

	void snap();

	void snapX(nsuint pTransformID);

	void snapX();

	void snapY(nsuint pTransformID);

	void snapY();

	void snapZ(nsuint pTransformID);

	void snapZ();

	void toggleGridSnap(nsuint pTransformID);

	void toggleGridSnap();

	void toggleHiddenState(HiddenState pState, nsuint pTransformID);

	void toggleHiddenState(HiddenState pState);

	void translate(const fvec3 & pAmount, nsuint pTransformID);

	void translate(const fvec3 & pAmount);

	void translate(nsfloat pX, nsfloat pY, nsfloat pZ, nsuint pTransformID);

	void translate(nsfloat pX, nsfloat pY, nsfloat pZ);

	void translate(DirVec pDirection, nsfloat pAmount, nsuint pTransformID);

	void translate(DirVec pDirection, nsfloat pAmount);

	void translate(Axis pDirection, nsfloat pAmount, nsuint pTransformID);

	void translate(Axis pDirection, nsfloat pAmount);

	void translateX(nsfloat pAmount, nsuint pTransformID);

	void translateX(nsfloat pAmount);

	void translateY(nsfloat pAmount, nsuint pTransformID);

	void translateY(nsfloat pAmount);

	void translateZ(nsfloat pAmount, nsuint pTransformID );

	void translateZ(nsfloat pAmount);

	NSTFormComp & operator=(const NSTFormComp & pRHSComp);

private:
	InstanceVec mTransforms;
	NSBufferObject mTransformBuffer;
	NSBufferObject mTransformIDBuffer;

	XFBData mXFBData;
	
	bool mBufferResize;
	bool mTransformFB;
	nsuint mVisibleCount;
};

template <class PUPer>
void pup(PUPer & p, NSTFormComp & tc)
{
	pup(p, tc.mTransforms, "transforms");
	pup(p, tc.mTransformFB, "transformFB");
	tc.postUpdate(true);
	tc.mBufferResize = true;
	if (tc.mTransformFB)
	{
		tc.enableTransformFeedback(false);
		tc.enableTransformFeedback(true);
	}
}

template <class PUPer>
void pup(PUPer & p, NSTFormComp::InstTrans & iv, const nsstring & varName)
{
	pup(p, iv.mHiddenState, varName + ".mHiddenState");
	pup(p, iv.mOrientation, varName + ".mOrientation");
	pup(p, iv.mPosition, varName + ".mPosition");
	pup(p, iv.mScaling, varName + ".mScaling");
	pup(p, iv.mParentTransform, varName + ".mParentTransform");
	pup(p, iv.mPOVTransform, varName + ".mPOVTransform");
	pup(p, iv.mTransform, varName + ".mTransform");
	pup(p, iv.mRenderID, varName + ".mRenderID");
	pup(p, iv.mSnapToGrid, varName + ".mSnapToGrid");
	pup(p, iv.mParentEnabled, varName + ".mParentEnabled");
	pup(p, iv.mParentID, varName + ".mParentID");
}
#endif
