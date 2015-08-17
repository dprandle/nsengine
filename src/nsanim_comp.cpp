/*! 
	\file nsanimcomp.cpp
	
	\brief Definition file for NSAnimComp class

	This file contains all of the neccessary definitions for the NSAnimComp class.

	\author Daniel Randle
	\date November 23 2013
	\copywrite Earth Banana Games 2013
*/

#include <nsanim_comp.h>
#include <nstimer.h>
#include <nsengine.h>
#include <nsanim_manager.h>
#include <nsentity.h>

NSAnimComp::NSAnimComp():NSComponent(),
	mElapsedTime(0.0f),
	mAnimating(false),
	mLooping(false),
	mAnimSetID(0),
	mCurrentAnim(),
	mFinalTransforms()
{}

NSAnimComp::~NSAnimComp()
{}

NSAnimComp* NSAnimComp::copy(const NSComponent * pToCopy)
{
	if (pToCopy == NULL)
		return NULL;
	const NSAnimComp * cmp = (const NSAnimComp*)pToCopy;
	(*this) = (*cmp);
	return this;
}

void NSAnimComp::pup(NSFilePUPer * p)
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

float & NSAnimComp::elapsed()
{
	return mElapsedTime;
}

void NSAnimComp::fillBones(NSMesh::NodeTree * pNodeTree, NSAnimSet::AnimationData * pCurrentAnim)
{
	_fillBoneTransform(pNodeTree, pNodeTree->mRootNode, pCurrentAnim, pNodeTree->mRootNode->mWorldTransform);
}

void NSAnimComp::init()
{}

bool NSAnimComp::animating() const
{
	return mAnimating;
}

void NSAnimComp::nameChange(const uivec2 & oldid, const uivec2 newid)
{
	if (mAnimSetID.x == oldid.x)
	{
		mAnimSetID.x = newid.x;
		if (mAnimSetID.y == oldid.y)
			mAnimSetID.y = newid.y;
	}
}

bool NSAnimComp::looping() const
{
	return mLooping;
}

const uivec2 & NSAnimComp::animationSetID()
{
	return mAnimSetID;
}

const nsstring & NSAnimComp::currentAnimation()
{
	return mCurrentAnim;
}

fmat4array * NSAnimComp::finalTransforms()
{
	return &mFinalTransforms;
}

/*!
Get the resources that the component uses. For the animation component that is simply an AnimSet
*/
uivec2array NSAnimComp::resources()
{
	// Build map
	uivec2array ret;
	
	// only add if not 0
	if (mAnimSetID != 0)
		ret.push_back(mAnimSetID);

	return ret;
}

void NSAnimComp::setCurrentAnimation(const nsstring & pAnimationName)
{
	mCurrentAnim = pAnimationName;
}

void NSAnimComp::setAnimate(bool pAnimate)
{
	mAnimating = pAnimate;
	postUpdate(true);
}

void NSAnimComp::setLoop(bool pLoop)
{
	mLooping = pLoop;
	postUpdate(true);
}

void NSAnimComp::setAnimationSetID(const uivec2 & pID)
{
	mAnimSetID = pID;
	postUpdate(true);
}

NSAnimComp & NSAnimComp::operator=(const NSAnimComp & pRHSComp)
{
	mElapsedTime = pRHSComp.mElapsedTime;
	mAnimating = pRHSComp.mAnimating;
	mAnimSetID = pRHSComp.mAnimSetID;
	mCurrentAnim = pRHSComp.mCurrentAnim;
	mFinalTransforms.clear();
	postUpdate(true);
	return (*this);
}

void NSAnimComp::_fillBoneTransform(NSMesh::NodeTree * pNodeTree, NSMesh::Node * pNode, NSAnimSet::AnimationData * pCurrentAnim, fmat4 & pParentTransform)
{
	fmat4 globalTransform;
	nsstringstream ss;

	if (pNodeTree->boneNameMap.empty())
	{
		dprint("NSAnimComp::_fillBoneTransform Animation has no bones");
		return;
	}

	if (pCurrentAnim != NULL && pCurrentAnim->animationNode(pNode->mName) != NULL)
		globalTransform = pParentTransform * pCurrentAnim->boneTransform(pNode->mName, mElapsedTime);
	else
		globalTransform = pParentTransform * pNode->mNodeTransform;

	std::map<nsstring, NSMesh::Bone>::const_iterator iter = pNodeTree->boneNameMap.find(pNode->mName);
	if (iter != pNodeTree->boneNameMap.end())
		mFinalTransforms[iter->second.boneID] = globalTransform * iter->second.mOffsetTransform;

	for (uint32 i = 0; i < pNode->mChildNodes.size(); ++i)
		_fillBoneTransform(pNodeTree, pNode->mChildNodes[i], pCurrentAnim, globalTransform);
}

