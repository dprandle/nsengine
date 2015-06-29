/*! 
	\file nsanimset.h
	
	\brief Header file for NSAnimSet class

	This file contains all of the neccessary declarations for the NSAnimSet class.

	\author Daniel Randle
	\date December 11 2013
	\copywrite Earth Banana Games 2013
*/

#ifndef NSANIMSET_H
#define NSANIMSET_H

#define DEFAULT_TICKS_PER_SECOND 20

#include <nsresource.h>
#include <map>
#include <vector>
#include <nsmath.h>
#include <assimp/anim.h>
#include <nsmath.h>
#include <nspupper.h>

class NSAnimSet : public NSResource
{
public:
	struct AnimationData
	{
		struct AnimationNode
		{
			AnimationNode();
			~AnimationNode();

			typedef fquatfmap::iterator KeyRotIter;
			typedef fvec3fmap::iterator KeyIter;

			
			fquatfmap mRotationKeys;
			fvec3fmap mTranslationKeys;
			fvec3fmap mScalingKeys;

			fmat4 transform(float pTime,float pTicksPerSecond);

			nsstring mNodeName;
			fmat4 offsetMatrix;
		};
		typedef std::map<nsstring, AnimationNode*>::iterator AnimNodeMapIter;

		AnimationData();
		~AnimationData();

		AnimationNode * createAnimationNode(const nsstring & pNodeName);

		AnimationNode * animationNode(const nsstring & pNodeName);
		fmat4 boneTransform(const nsstring & pNodeName, float pTime);

		std::map<nsstring,AnimationNode*> mAnimNodesMap;
		nsstring mAnimationName;
		nsfloat mDuration;
		nsfloat mTicksPerSecond;
	};

	typedef std::map<nsstring, AnimationData*> AnimMap;
	typedef std::map<nsstring,AnimationData*>::iterator AnimMapIter;

	template<class PUPer>
	friend void pup(PUPer & p, NSAnimSet & aset);

	NSAnimSet();
	~NSAnimSet();

	void clear();

	void init();

	AnimationData * createAnimationData(const nsstring & pAnimationName);

	AnimationData * animationData(const nsstring & pAnimationName);

	fmat4 animationBoneTransform(const nsstring & pAnimationName,
		const nsstring & pNodeName, float pTime);

	virtual void pup(NSFilePUPer * p);

	virtual nsstring typeString() { return getTypeString(); }

	nsstring managerTypeString() { return getManagerTypeString(); }

	static nsstring getTypeString() { return ANIM_TYPESTRING; }

	static nsstring getManagerTypeString() { return ANIM_MANAGER_TYPESTRING; }

private:
	AnimMap mAnimationsMap;
};

template<class PUPer>
void pup(PUPer & p, NSAnimSet::AnimationData::AnimationNode & node, const nsstring & varName)
{
	pup(p, node.mNodeName, varName + ".mNodeName");
	pup(p, node.offsetMatrix, varName + ".offsetMatrix");
	pup(p, node.mScalingKeys, varName + ".mScalingKeys");
	pup(p, node.mRotationKeys, varName + ".mRotationKeys");
	pup(p, node.mTranslationKeys, varName + ".mTranslationKeys");
}

template<class PUPer>
void pup(PUPer & p, NSAnimSet::AnimationData::AnimationNode * & node, const nsstring & varName)
{
	if (p.mode() == PUP_IN) // If reading in then we need to allocate memory for the node
		node = new NSAnimSet::AnimationData::AnimationNode();
	pup(p, *node, varName);
}

template<class PUPer>
void pup(PUPer & p, NSAnimSet::AnimationData & data, const nsstring & varName)
{
	pup(p, data.mAnimationName, varName + ".mAnimationName");
	pup(p, data.mTicksPerSecond, varName + ".mTicksPerSecond");
	pup(p, data.mDuration, varName + ".mDuration");
	pup(p, data.mAnimNodesMap, varName + ".mAnimNodesMap");
}

template<class PUPer>
void pup(PUPer & p, NSAnimSet::AnimationData * & data, const nsstring & varName)
{
	if (p.mode() == PUP_IN)
		data = new NSAnimSet::AnimationData();
	pup(p, *data, varName);
}

template<class PUPer>
void pup(PUPer & p, NSAnimSet & aset)
{
	pup(p, aset.mAnimationsMap, "animationsMap");
}

#endif