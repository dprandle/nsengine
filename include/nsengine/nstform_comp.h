/*! 
	\file nstform_comp.h
	
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
#include <nsbuffer_object.h>
#include <nstransform_feedback_object.h>
#include <nsvertex_array_object.h>

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
		uint32 mAllocAmount;
		uint32 mInstanceCount;
	};

	typedef std::vector<XFBBuffer> XFBBufferArray;

	struct XFBData
	{
		XFBData() :
			mXFBBuffers(),
			mUpdate(true)
		{}

		XFBBufferArray mXFBBuffers;
		bool mUpdate;
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

		int32 mHiddenState;
		fquat mOrientation;
		fvec3 mPosition;
		fvec3 mScaling;
		fmat4 mParentTransform;
		fmat4 mPOVTransform;
		fmat4 mTransform;
		bool mUpdate;
		uint32 mRenderID;
		bool mSnapToGrid;
		bool mParentEnabled;
		uivec3 mParentID;
	};

	typedef std::vector<InstTrans> InstanceVec;

	template <class PUPer>
	friend void pup(PUPer & p, NSTFormComp & tc);

	NSTFormComp();

	~NSTFormComp();

	uint32 add();

	uint32 add(const InstTrans & trans);

	uint32 add(uint32 pHowMany);

	bool bufferResize() const;

	void changeScale(const fvec3 & pAmount, uint32 pTransformID=0);

	void changeScale(float pX, float pY, float pZ, uint32 pTransformID = 0);

	void computeTransform(uint32 pTransformID = 0);

	NSTFormComp * copy(const NSComponent* pToCopy);

	void enableParent(bool pEnable, uint32 pTransformID = 0);

	bool enableTransformFeedback(bool pEnable);

	const fvec3 dirVec(DirVec pDirection, uint32 pTransformID = 0) const;

	NSBufferObject * transformBuffer();

	NSBufferObject * transformIDBuffer();

	const InstanceVec & transformVec() const;

	const fquat & orientation(uint32 pTransformID = 0) const;

	const uivec3 & parentid(uint32 pTransformID = 0) const;

	const fmat4 & parent(uint32 pTransformID = 0) const;

	const fvec3 & lpos(uint32 pTransformID = 0) const;

	const fmat4 & pov(uint32 pTransformID = 0) const;

	uint32 renderID(uint32 pTransformID) const;

	const fvec3 & scaling(uint32 pTransformID = 0) const;

	const fmat4 & transform(uint32 pTransformID = 0) const;

	int32 hiddenState(uint32 pTransformID = 0) const;

	InstTrans & instTrans(uint32 pTransformID = 0);

	uint32 count() const;

	XFBData * xfbData();

	uint32 visibleCount() const;

	fvec3 wpos(uint32 pTransformID = 0);

	void init();

	bool snapped(uint32 pTransformID = 0) const;

	bool calcLocalTForm(uint32 pTransformID = 0) const;

	bool parentEnabled(uint32 pTransformID = 0) const;

	bool transformFeedback();

	const bool transUpdate(uint32 pTransformID = 0) const;

	virtual void pup(NSFilePUPer * p);

	void release();

	uint32 remove(uint32 pTransformID);

	uint32 remove(uint32 pFirst, uint32 pLast);

	void rotate(Axis pAxis, float pAngle, uint32 pTransformID);

	void rotate(Axis pAxis, float pAngle);

	void rotate(const fvec4 & axisAngle, uint32 pTransformID);

	void rotate(const fvec4 & axisAngle);

	void rotate(const fvec3 & euler);

	void rotate(const fvec3 & euler, uint32 pTransformID);

	void rotate(const fquat & pQuat, uint32 pTransformID);

	void rotate(const fquat & pQuat);

	void rotate(DirVec dir, float angle, uint32 tformid);

	void rotate(DirVec dir, float angle);

	void scale(const fvec3 & pAmount, uint32 pTransformID);

	void scale(const fvec3 & pAmount);

	void scale(float pX, float pY, float pZ, uint32 pTransformID);

	void scale(float pX, float pY, float pZ);

	void setBufferResize(bool pResize);

	void enableSnap(bool pSnap, uint32 pTransformID = 0);

	void enableSnap(bool pSnap);

	void setHiddenState(HiddenState pState, bool pEnable, uint32 pTransformID);

	void setHiddenState(HiddenState pState, bool pEnable);

	void setInstTrans(const InstTrans & pInsT, uint32 pTransformID);

	void setInstTrans(const InstTrans & pInsT);

	void setOrientation(const fquat & pOrientation, uint32 pTransformID);

	void setOrientation(const fquat & pOrientation);

	void setParent(const fmat4 & pTransform, uint32 pTransformID);

	void setParent(const fmat4 & pTransform);

	void setParentID(const uivec3 & pParentID, uint32 pTransformID);

	void setParentID(const uivec3 & pParentID);

	void setpos(const fvec3 & pPosition, uint32 pTransformID);

	void setpos(const fvec3 & pPosition);

	void setRenderID(uint32 pRenderID, uint32 pTransformID);

	void setRenderID(uint32 pRenderID);

	void setScale(const fvec3 & pScaling, uint32 pTransformID);

	void setScale(const fvec3 & pScaling);

	void setTransUpdate(bool pUpdate, uint32 pTransformID);

	void setTransUpdate(bool pUpdate);

	void postUpdate(bool pUpdate);

	void setVisibleTransformCount(uint32 pCount);

	void snap(uint32 pTransformID);

	void snap();

	void snapX(uint32 pTransformID);

	void snapX();

	void snapY(uint32 pTransformID);

	void snapY();

	void snapZ(uint32 pTransformID);

	void snapZ();

	void toggleGridSnap(uint32 pTransformID);

	void toggleGridSnap();

	void toggleHiddenState(HiddenState pState, uint32 pTransformID);

	void toggleHiddenState(HiddenState pState);

	void translate(const fvec3 & pAmount, uint32 pTransformID);

	void translate(const fvec3 & pAmount);

	void translate(float pX, float pY, float pZ, uint32 pTransformID);

	void translate(float pX, float pY, float pZ);

	void translate(DirVec pDirection, float pAmount, uint32 pTransformID);

	void translate(DirVec pDirection, float pAmount);

	void translate(Axis pDirection, float pAmount, uint32 pTransformID);

	void translate(Axis pDirection, float pAmount);

	void translateX(float pAmount, uint32 pTransformID);

	void translateX(float pAmount);

	void translateY(float pAmount, uint32 pTransformID);

	void translateY(float pAmount);

	void translateZ(float pAmount, uint32 pTransformID );

	void translateZ(float pAmount);

	NSTFormComp & operator=(const NSTFormComp & pRHSComp);

private:
	InstanceVec mTransforms;
	NSBufferObject mTransformBuffer;
	NSBufferObject mTransformIDBuffer;

	XFBData mXFBData;
	
	bool mBufferResize;
	bool mTransformFB;
	uint32 mVisibleCount;
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
