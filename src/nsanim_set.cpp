/*! 
	\file nsanimset.cpp
	
	\brief Header file for NSAnimSet class

	This file contains all of the neccessary definitions for the NSAnimSet class.

	\author Daniel Randle
	\date December 11 2013
	\copywrite Earth Banana Games 2013
*/

#include <nsanim_set.h>

NSAnimSet::NSAnimSet(): mAnimationsMap()
{
	setExtension(DEFAULT_ANIM_EXTENSION);
}

NSAnimSet::~NSAnimSet()
{
	AnimMapIter iter = mAnimationsMap.begin();
	while (iter != mAnimationsMap.end())
	{
		delete iter->second;
		++iter;
	}
	mAnimationsMap.clear();
}

void NSAnimSet::init()
{

}

void NSAnimSet::clear()
{
	AnimMapIter iter = mAnimationsMap.begin();
	while (iter != mAnimationsMap.end())
	{
		delete iter->second;
		++iter;
	}
	mAnimationsMap.clear();
}

void NSAnimSet::pup(NSFilePUPer * p)
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

NSAnimSet::AnimationData * NSAnimSet::createAnimationData(const nsstring & pAnimationName)
{
	AnimationData * animData = new AnimationData();
	animData->mAnimationName = pAnimationName;
	mAnimationsMap[pAnimationName] = animData;
	return animData;
}

NSAnimSet::AnimationData * NSAnimSet::animationData(const nsstring & pAnimationName)
{
	AnimMapIter iter = mAnimationsMap.find(pAnimationName);
	if (iter != mAnimationsMap.end())
		return mAnimationsMap.at(pAnimationName);
	return NULL;
}

fmat4 NSAnimSet::animationBoneTransform(const nsstring & pAnimationName,
	const nsstring & pNodeName, float pTime)
{
	return animationData(pAnimationName)->boneTransform(pNodeName,pTime);
}

NSAnimSet::AnimationData::AnimationData(): mAnimNodesMap(),
	mAnimationName(),
	mDuration(0.0f)
{}

NSAnimSet::AnimationData::~AnimationData()
{
	AnimNodeMapIter iter = mAnimNodesMap.begin();
	while (iter != mAnimNodesMap.end())
	{
		delete iter->second;
		iter->second = NULL;
		++iter;
	}
	mAnimNodesMap.clear();
}

NSAnimSet::AnimationData::AnimationNode * NSAnimSet::AnimationData::createAnimationNode(const nsstring & pNodeName)
{
	AnimationNode * animNode = new AnimationNode();
	animNode->mNodeName = pNodeName;
	mAnimNodesMap[pNodeName] = animNode;
	return animNode;
}

NSAnimSet::AnimationData::AnimationNode * NSAnimSet::AnimationData::animationNode(const nsstring & pNodeName)
{
	AnimNodeMapIter iter = mAnimNodesMap.find(pNodeName);
	if (iter != mAnimNodesMap.end())
		return iter->second;
	return NULL;
}

fmat4 NSAnimSet::AnimationData::boneTransform(const nsstring & pNodeName, float pTime)
{
	AnimNodeMapIter iter = mAnimNodesMap.find(pNodeName);
	if (iter != mAnimNodesMap.end())
		return iter->second->transform(pTime,mTicksPerSecond);
	return fmat4();
}

NSAnimSet::AnimationData::AnimationNode::AnimationNode()
{}

NSAnimSet::AnimationData::AnimationNode::~AnimationNode()
{}

fmat4 NSAnimSet::AnimationData::AnimationNode::transform(float pTime, float pTicksPerSecond)
{
	float tick = pTime * pTicksPerSecond;

	KeyRotIter lowerRot = mRotationKeys.upper_bound(tick);
	KeyRotIter upperRot = lowerRot;

	KeyIter lowerScale = mScalingKeys.upper_bound(tick);
	KeyIter upperScale = lowerScale;

	KeyIter lowerTrans = mTranslationKeys.upper_bound(tick);
	KeyIter upperTrans = lowerTrans;

	fquat interQ;
	if (upperRot != mRotationKeys.end())
	{
		if (lowerRot != mRotationKeys.begin())
		{
			--lowerRot;
			float scaling = (tick - lowerRot->first) / (upperRot->first - lowerRot->first);
			interQ = slerp(lowerRot->second, upperRot->second, scaling);
		}
		else
			interQ = lowerRot->second;
	}
	else
		interQ = mRotationKeys.rbegin()->second;

	fvec3 transInter;
	if (upperTrans != mTranslationKeys.end())
	{
		if (lowerTrans != mTranslationKeys.begin())
		{
			--lowerTrans;
			float scaling = (tick - lowerTrans->first) / (upperTrans->first - lowerTrans->first);
			transInter = lerp(lowerTrans->second, upperTrans->second, scaling);
		}
		else
			transInter = lowerTrans->second;
	}
	else
		transInter = mTranslationKeys.rbegin()->second;

	fvec3 scaleInter;
	if (upperScale != mScalingKeys.end())
	{
		if (lowerScale != mScalingKeys.begin())
		{
			--lowerScale;
			float scaling = (tick - lowerScale->first) / (upperScale->first - lowerScale->first);
			scaleInter = lerp(lowerScale->second, upperScale->second, scaling);
		}
		else
			scaleInter = lowerScale->second;
	}
	else
		scaleInter = mScalingKeys.rbegin()->second;

	return ( fmat4(rotationMat3(interQ) % scaleInter).setColumn(3, transInter.x, transInter.y, transInter.z, 1) );
}
