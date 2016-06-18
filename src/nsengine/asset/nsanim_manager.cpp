/*! 
	\file nsanimmanager.cpp
	
	\brief Header file for nsanim_manager class

	This file contains all of the neccessary definitions for the nsanim_manager class.

	\author Daniel Randle
	\date November 23 2013
	\copywrite Earth Banana Games 2013
*/

#include <nsengine.h>
#include <nsanim_set.h>
#include <assimp/scene.h>
#include <nsanim_manager.h>
#include <nsplugin.h>
#include <vector>
#include <map>
#include <nsplugin_manager.h>

nsanim_set * get_anim_set(const uivec2 & id)
{
	nsplugin * plg = nsep.get(id.x);
	if (plg == nullptr)
		return nullptr;
	return plg->get<nsanim_set>(id.y);
}

nsanim_manager::nsanim_manager():
	nsasset_manager(type_to_hash(nsanim_manager))
{
	set_local_dir(LOCAL_ANIMATION_DIR_DEFAULT);
}

nsanim_manager::~nsanim_manager()
{}

nsanim_set * nsanim_manager::assimp_load_anim_set(const aiScene * pScene, const nsstring & pSceneName)
{
	nsanim_set * animSet = create(pSceneName);
	for (uint32 i = 0; i < pScene->mNumAnimations; ++i)
	{
		nsstringstream ss;
		aiAnimation * currentAnim = pScene->mAnimations[i];
		ss << currentAnim->mName.C_Str();
		animation_data * animData = animSet->create_anim_data(ss.str());


		animData->ticks_per_second_ = float(currentAnim->mTicksPerSecond);
		if (!animData->ticks_per_second_)
			animData->ticks_per_second_ = DEFAULT_TICKS_PER_SECOND;
		animData->duration =  float(currentAnim->mDuration) / animData->ticks_per_second_;

		for (uint32 j = 0; j < currentAnim->mNumChannels; ++j)
		{
			aiNodeAnim * aiAnimNode = currentAnim->mChannels[j];
			animation_node * animNode = animData->create_node(aiAnimNode->mNodeName.C_Str());
			
			for (uint32 transIndex = 0; transIndex < aiAnimNode->mNumPositionKeys; ++transIndex)
			{
				aiVectorKey * vPosKey = &aiAnimNode->mPositionKeys[transIndex];
				fvec3 trans(vPosKey->mValue.x, vPosKey->mValue.y, vPosKey->mValue.z);
				animNode->translation_keys[float(vPosKey->mTime)] = trans;
			}

			for (uint32 scaleIndex = 0; scaleIndex < aiAnimNode->mNumScalingKeys; ++scaleIndex)
			{
				aiVectorKey * vScaleKey = &aiAnimNode->mScalingKeys[scaleIndex];
				fvec3 scale(vScaleKey->mValue.x, vScaleKey->mValue.y, vScaleKey->mValue.z);
				animNode->scaling_keys[float(vScaleKey->mTime)] = scale;
			}

			for (uint32 rotIndex = 0; rotIndex < aiAnimNode->mNumRotationKeys; ++rotIndex)
			{
				fquat rot;
				aiQuatKey * vRotKey = &aiAnimNode->mRotationKeys[rotIndex];
				rot.set(vRotKey->mValue.x, vRotKey->mValue.y, vRotKey->mValue.z, vRotKey->mValue.w);
				animNode->rotation_keys[float(vRotKey->mTime)] = rot;
			}
		}
	}
	add(animSet);
	return animSet;
}
