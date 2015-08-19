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

void NSAnimComp::pup(nsfile_pupper * p)
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

float & NSAnimComp::elapsed()
{
	return mElapsedTime;
}

void NSAnimComp::fillBones(nsmesh::node_tree * pNodeTree, nsanim_set::animation_data * pCurrentAnim)
{
	_fillBoneTransform(pNodeTree, pNodeTree->root_node, pCurrentAnim, pNodeTree->root_node->world_transform);
}

void NSAnimComp::init()
{}

bool NSAnimComp::animating() const
{
	return mAnimating;
}

void NSAnimComp::name_change(const uivec2 & oldid, const uivec2 newid)
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
	post_update(true);
}

void NSAnimComp::setLoop(bool pLoop)
{
	mLooping = pLoop;
	post_update(true);
}

void NSAnimComp::setAnimationSetID(const uivec2 & pID)
{
	mAnimSetID = pID;
	post_update(true);
}

NSAnimComp & NSAnimComp::operator=(const NSAnimComp & pRHSComp)
{
	mElapsedTime = pRHSComp.mElapsedTime;
	mAnimating = pRHSComp.mAnimating;
	mAnimSetID = pRHSComp.mAnimSetID;
	mCurrentAnim = pRHSComp.mCurrentAnim;
	mFinalTransforms.clear();
	post_update(true);
	return (*this);
}

void NSAnimComp::_fillBoneTransform(nsmesh::node_tree * pNodeTree, nsmesh::node * pNode, nsanim_set::animation_data * pCurrentAnim, fmat4 & pParentTransform)
{
	fmat4 globalTransform;
	nsstringstream ss;

	if (pNodeTree->bone_name_map.empty())
	{
		dprint("NSAnimComp::_fillBoneTransform Animation has no bones");
		return;
	}

	if (pCurrentAnim != NULL && pCurrentAnim->anim_node(pNode->name) != NULL)
		globalTransform = pParentTransform * pCurrentAnim->bone_transform(pNode->name, mElapsedTime);
	else
		globalTransform = pParentTransform * pNode->node_transform;

	std::map<nsstring, nsmesh::bone>::const_iterator iter = pNodeTree->bone_name_map.find(pNode->name);
	if (iter != pNodeTree->bone_name_map.end())
		mFinalTransforms[iter->second.boneID] = globalTransform * iter->second.mOffsetTransform;

	for (uint32 i = 0; i < pNode->child_nodes.size(); ++i)
		_fillBoneTransform(pNodeTree, pNode->child_nodes[i], pCurrentAnim, globalTransform);
}

