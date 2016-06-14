/*! 
	\file nsanimset.cpp
	
	\brief Header file for nsanim_set class

	This file contains all of the neccessary definitions for the nsanim_set class.

	\author Daniel Randle
	\date December 11 2013
	\copywrite Earth Banana Games 2013
*/

#include <nsanim_set.h>
#include <nsengine.h>

nsanim_set::nsanim_set():
	nsresource(type_to_hash(nsanim_set)),
	m_animmap()
{
	set_ext(DEFAULT_ANIM_EXTENSION);
}

nsanim_set::nsanim_set(const nsanim_set & copy_):
	nsresource(copy_)
{
	auto iter = copy_.m_animmap.begin();
	while (iter != copy_.m_animmap.end())
	{
		animation_data * nd = create_anim_data(iter->first);
		(*nd) = (*iter->second);
		++iter;
	}
}

nsanim_set::~nsanim_set()
{
	animmap_iter iter = m_animmap.begin();
	while (iter != m_animmap.end())
	{
		delete iter->second;
		++iter;
	}
	m_animmap.clear();
}

nsanim_set & nsanim_set::operator=(nsanim_set rhs)
{
	nsresource::operator=(rhs);
	return *this;
}

void nsanim_set::init()
{

}

void nsanim_set::clear()
{
	animmap_iter iter = m_animmap.begin();
	while (iter != m_animmap.end())
	{
		delete iter->second;
		++iter;
	}
	m_animmap.clear();
}

void nsanim_set::pup(nsfile_pupper * p)
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

animation_data * nsanim_set::create_anim_data(const nsstring & pAnimationName)
{
	animation_data * animData = new animation_data();
	animData->animation_name = pAnimationName;
	m_animmap[pAnimationName] = animData;
	return animData;
}

animation_data * nsanim_set::anim_data(const nsstring & pAnimationName)
{
	animmap_iter iter = m_animmap.find(pAnimationName);
	if (iter != m_animmap.end())
		return m_animmap.at(pAnimationName);
	return NULL;
}

nsanim_set::animmap::iterator nsanim_set::begin()
{
	return m_animmap.begin();
}

nsanim_set::animmap::iterator nsanim_set::end()
{
	return m_animmap.end();
}

fmat4 nsanim_set::anim_joint_transform(const nsstring & pAnimationName,
	const nsstring & pNodeName, float pTime)
{
	return anim_data(pAnimationName)->joint_transform(pNodeName,pTime);
}

animation_data::animation_data(): anim_node_map(),
	animation_name(),
	duration(0.0f)
{}

animation_data::animation_data(const animation_data & copy_):
	animation_name(copy_.animation_name),
	duration(copy_.duration),
	ticks_per_second_(copy_.ticks_per_second_)
{
	auto iter = copy_.anim_node_map.begin();
	while (iter != copy_.anim_node_map.end())
	{
		animation_node * nd = create_node(iter->first);
		(*nd) = (*iter->second);
		++iter;
	}
}

animation_data::~animation_data()
{
	anim_nodemap_iter iter = anim_node_map.begin();
	while (iter != anim_node_map.end())
	{
		delete iter->second;
		iter->second = NULL;
		++iter;
	}
	anim_node_map.clear();
}

animation_data & animation_data::operator=(animation_data rhs)
{
	std::swap(anim_node_map, rhs.anim_node_map);
	std::swap(animation_name, rhs.animation_name);
	std::swap(duration, rhs.duration);
	std::swap(ticks_per_second_, rhs.ticks_per_second_);
	return *this;
}

animation_node * animation_data::create_node(const nsstring & pNodeName)
{
	animation_node * animNode = new animation_node();
	animNode->name = pNodeName;
	anim_node_map[pNodeName] = animNode;
	return animNode;
}

animation_node * animation_data::anim_node(const nsstring & pNodeName)
{
	anim_nodemap_iter iter = anim_node_map.find(pNodeName);
	if (iter != anim_node_map.end())
		return iter->second;
	return NULL;
}

fmat4 animation_data::joint_transform(const nsstring & pNodeName, float pTime)
{
	anim_nodemap_iter iter = anim_node_map.find(pNodeName);
	if (iter != anim_node_map.end())
		return iter->second->transform(pTime,ticks_per_second_);
	return fmat4();
}

animation_node::animation_node()
{}

animation_node::~animation_node()
{}

fmat4 animation_node::transform(float pTime, float pTicksPerSecond)
{
	float tick = pTime * pTicksPerSecond;

	key_rotation_iter lowerRot = rotation_keys.upper_bound(tick);
	key_rotation_iter upperRot = lowerRot;

	key_iter lowerScale = scaling_keys.upper_bound(tick);
	key_iter upperScale = lowerScale;

	key_iter lowerTrans = translation_keys.upper_bound(tick);
	key_iter upperTrans = lowerTrans;

	fquat interQ;
	if (upperRot != rotation_keys.end())
	{
		if (lowerRot != rotation_keys.begin())
		{
			--lowerRot;
			float scaling = (tick - lowerRot->first) / (upperRot->first - lowerRot->first);
			interQ = slerp(lowerRot->second, upperRot->second, scaling);
		}
		else
			interQ = lowerRot->second;
	}
	else
		interQ = rotation_keys.rbegin()->second;

	fvec3 transInter;
	if (upperTrans != translation_keys.end())
	{
		if (lowerTrans != translation_keys.begin())
		{
			--lowerTrans;
			float scaling = (tick - lowerTrans->first) / (upperTrans->first - lowerTrans->first);
			transInter = lerp(lowerTrans->second, upperTrans->second, scaling);
		}
		else
			transInter = lowerTrans->second;
	}
	else
		transInter = translation_keys.rbegin()->second;

	fvec3 scaleInter;
	if (upperScale != scaling_keys.end())
	{
		if (lowerScale != scaling_keys.begin())
		{
			--lowerScale;
			float scaling = (tick - lowerScale->first) / (upperScale->first - lowerScale->first);
			scaleInter = lerp(lowerScale->second, upperScale->second, scaling);
		}
		else
			scaleInter = lowerScale->second;
	}
	else
		scaleInter = scaling_keys.rbegin()->second;

	return ( fmat4(rotation_mat3(interQ) % scaleInter).set_column(3, transInter.x, transInter.y, transInter.z, 1) );
}
