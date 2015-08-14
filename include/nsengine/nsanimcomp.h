/*! 
	\file nsanimcomp.h
	
	\brief Header file for NSAnimComp class

	This file contains all of the neccessary declarations for the NSAnimComp class.

	\author Daniel Randle
	\date November 23 2013
	\copywrite Earth Banana Games 2013
*/

#ifndef NSANIMCOMP_H
#define NSANIMCOMP_H

#include <vector>
#include <nsmath.h>
#include <nscomponent.h>
#include <nspupper.h>
#include <nsanimset.h>
#include <nsmesh.h>

class NSTimer;


class NSAnimComp : public NSComponent
{
public:
	NSAnimComp();
	~NSAnimComp();

	template <class PUPer>
	friend void pup(PUPer & p, NSAnimComp & anim);

	virtual NSAnimComp* copy(const NSComponent* pToCopy);

	float & elapsed();

	void fillBones(NSMesh::NodeTree * pNodeTree, NSAnimSet::AnimationData * pCurrentAnim);

	const uivec2 & animationSetID();

	const nsstring & currentAnimation();

	fmat4array * finalTransforms();

	virtual void nameChange(const uivec2 & oldid, const uivec2 newid);

	/*!
	Get the resources that the component uses. For the animation component that is simply an AnimSet
	/return Map of resource ID to resource type containing AnimSet used
	*/
	virtual uivec2array resources();

	void init();

	bool animating() const;

	bool looping() const;

	virtual void pup(NSFilePUPer * p);

	void setAnimationSetID(uint32 plugid, uint32 resid)
	{
		mAnimSetID.x = plugid; mAnimSetID.y = resid;
		postUpdate(true);
	}

	void setAnimationSetID(const uivec2 & pID);

	void setAnimate(bool pAnimate);

	void setLoop(bool pLoop);

	void setCurrentAnimation(const nsstring & pAnimationName);

	NSAnimComp & operator=(const NSAnimComp & pRHSComp);

private:
	void _fillBoneTransform(NSMesh::NodeTree * pNodeTree, NSMesh::Node * pNode, NSAnimSet::AnimationData * pCurrentAnim, fmat4 & pParentTransform);

	float mElapsedTime;
	
	bool mAnimating;
	bool mLooping;

	uivec2 mAnimSetID; //! AnimSet resource used
	nsstring mCurrentAnim; //! Current animation within the animation set
	fmat4array mFinalTransforms;
};

template <class PUPer>
void pup(PUPer & p, NSAnimComp & anim)
{
	pup(p, anim.mElapsedTime, "elapsedTime");
	pup(p, anim.mAnimating, "animating");
	pup(p, anim.mLooping, "looping");
	pup(p, anim.mAnimSetID, "animSetID");
	pup(p, anim.mCurrentAnim, "currentAnim");
}

#endif
