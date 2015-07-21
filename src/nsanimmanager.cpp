/*! 
	\file nsanimmanager.cpp
	
	\brief Header file for NSAnimManager class

	This file contains all of the neccessary definitions for the NSAnimManager class.

	\author Daniel Randle
	\date November 23 2013
	\copywrite Earth Banana Games 2013
*/

#include <nsanimmanager.h>
#include <nsanimset.h>
#include <assimp/scene.h>
#include <nsengine.h>

NSAnimManager::NSAnimManager(): NSResManager()
{
	setLocalDirectory(LOCAL_ANIMATION_DIR_DEFAULT);
}

NSAnimManager::~NSAnimManager()
{}

NSAnimSet * NSAnimManager::assimpLoadAnimationSet(const aiScene * pScene, const nsstring & pSceneName)
{
	NSAnimSet * animSet = create(pSceneName);
	for (nsuint i = 0; i < pScene->mNumAnimations; ++i)
	{
		nsstringstream ss;
		aiAnimation * currentAnim = pScene->mAnimations[i];
		ss << currentAnim->mName.C_Str();
		NSAnimSet::AnimationData * animData = animSet->createAnimationData(ss.str());

		animData->mTicksPerSecond = nsfloat(currentAnim->mTicksPerSecond);
		if (!animData->mTicksPerSecond)
			animData->mTicksPerSecond = DEFAULT_TICKS_PER_SECOND;
		animData->mDuration =  nsfloat(currentAnim->mDuration) / animData->mTicksPerSecond;

		for (nsuint j = 0; j < currentAnim->mNumChannels; ++j)
		{
			aiNodeAnim * aiAnimNode = currentAnim->mChannels[j];
			NSAnimSet::AnimationData::AnimationNode * animNode = animData->createAnimationNode(aiAnimNode->mNodeName.C_Str());
			
			for (nsuint transIndex = 0; transIndex < aiAnimNode->mNumPositionKeys; ++transIndex)
			{
				aiVectorKey * vPosKey = &aiAnimNode->mPositionKeys[transIndex];
				fvec3 trans(vPosKey->mValue.x, vPosKey->mValue.y, vPosKey->mValue.z);
				animNode->mTranslationKeys[nsfloat(vPosKey->mTime)] = trans;
			}

			for (nsuint scaleIndex = 0; scaleIndex < aiAnimNode->mNumScalingKeys; ++scaleIndex)
			{
				aiVectorKey * vScaleKey = &aiAnimNode->mScalingKeys[scaleIndex];
				fvec3 scale(vScaleKey->mValue.x, vScaleKey->mValue.y, vScaleKey->mValue.z);
				animNode->mScalingKeys[nsfloat(vScaleKey->mTime)] = scale;
			}

			for (nsuint rotIndex = 0; rotIndex < aiAnimNode->mNumRotationKeys; ++rotIndex)
			{
				fquat rot;
				aiQuatKey * vRotKey = &aiAnimNode->mRotationKeys[rotIndex];
				rot.set(vRotKey->mValue.x, vRotKey->mValue.y, vRotKey->mValue.z, vRotKey->mValue.w);
				animNode->mRotationKeys[nsfloat(vRotKey->mTime)] = rot;
			}
		}
	}
	add(animSet);
	return animSet;
}
