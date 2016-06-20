/*! 
	\file nsanimcomp.cpp
	
	\brief Definition file for nsanim_comp class

	This file contains all of the neccessary definitions for the nsanim_comp class.

	\author Daniel Randle
	\date November 23 2013
	\copywrite Earth Banana Games 2013
*/

#include <nsanim_comp.h>
#include <nstimer.h>
#include <nsengine.h>
#include <nsanim_manager.h>
#include <nsentity.h>

nsanim_comp::nsanim_comp():
	nscomponent(type_to_hash(nsanim_comp)),
	m_elapsed_time(0.0f),
	m_animation(false),
	m_looping(false),
	m_anim_set_id(0),
	m_current_anim(),
	m_final_transforms()
{}

nsanim_comp::nsanim_comp(const nsanim_comp & cpy):
	nscomponent(cpy),
	m_elapsed_time(0.0f), // Lets not copy the animation time elapseds
	m_animation(cpy.m_animation),
	m_looping(cpy.m_looping),
	m_anim_set_id(cpy.m_anim_set_id),
	m_current_anim(cpy.m_current_anim)
{		
	m_final_transforms.clear();
}

nsanim_comp::~nsanim_comp()
{}

nsanim_comp & nsanim_comp::operator=(nsanim_comp rhs)
{
	nscomponent::operator=(rhs);
	std::swap(m_elapsed_time, rhs.m_elapsed_time);
	std::swap(m_animation, rhs.m_animation);
	std::swap(m_anim_set_id, rhs.m_anim_set_id);
	std::swap(m_looping, rhs.m_looping);
	std::swap(m_current_anim, rhs.m_current_anim);
	m_final_transforms.clear();
	post_update(true);
	return *this;
}

void nsanim_comp::pup(nsfile_pupper * p)
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

float & nsanim_comp::elapsed()
{
	return m_elapsed_time;
}

void nsanim_comp::fill_joints(nsmesh::node_tree * pNodeTree, animation_data * pCurrentAnim)
{
	_fill_joint_transforms(pNodeTree, pNodeTree->m_root, pCurrentAnim, pNodeTree->m_root->m_world_tform);
}

void nsanim_comp::init()
{}

bool nsanim_comp::animating() const
{
	return m_animation;
}

void nsanim_comp::name_change(const uivec2 & oldid, const uivec2 newid)
{
	if (m_anim_set_id.x == oldid.x)
	{
		m_anim_set_id.x = newid.x;
		if (m_anim_set_id.y == oldid.y)
			m_anim_set_id.y = newid.y;
	}
}

bool nsanim_comp::looping() const
{
	return m_looping;
}

const uivec2 & nsanim_comp::anim_set_id()
{
	return m_anim_set_id;
}

const nsstring & nsanim_comp::current_anim_name()
{
	return m_current_anim;
}

fmat4_vector * nsanim_comp::final_transforms()
{
	return &m_final_transforms;
}

/*!
Get the resources that the component uses. For the animation component that is simply an AnimSet
*/
uivec3_vector nsanim_comp::resources()
{
	// Build map
	uivec3_vector ret;
	
	nsanim_set * _anim_set_ = get_asset<nsanim_set>(m_anim_set_id);
	if (_anim_set_ != NULL)
	{
		uivec3_vector tmp = _anim_set_->resources();
		ret.insert(ret.end(), tmp.begin(), tmp.end());
		ret.push_back(uivec3(_anim_set_->full_id(), type_to_hash(nsanim_set)));
	}
	return ret;
}

void nsanim_comp::set_current_animation(const nsstring & pAnimationName)
{
	m_current_anim = pAnimationName;
}

void nsanim_comp::set_animate(bool pAnimate)
{
	m_animation = pAnimate;
	post_update(true);
}

void nsanim_comp::set_loop(bool pLoop)
{
	m_looping = pLoop;
	post_update(true);
}

void nsanim_comp::set_anim_set_id(const uivec2 & pID)
{
	m_anim_set_id = pID;
	post_update(true);
}

void nsanim_comp::_fill_joint_transforms(nsmesh::node_tree * pNodeTree, nsmesh::node * pNode, animation_data * pCurrentAnim, fmat4 & pParentTransform)
{
	fmat4 globalTransform;
	nsstringstream ss;

	if (pNodeTree->m_name_joint_map.empty())
	{
		dprint("nsanim_comp::_fillBoneTransform Animation has no bones");
		return;
	}

	if (pCurrentAnim != NULL && pCurrentAnim->anim_node(pNode->m_name) != NULL)
		globalTransform = pParentTransform * pCurrentAnim->joint_transform(pNode->m_name, m_elapsed_time);
	else
		globalTransform = pParentTransform * pNode->m_node_tform;

	std::map<nsstring, nsmesh::joint>::const_iterator iter = pNodeTree->m_name_joint_map.find(pNode->m_name);
	if (iter != pNodeTree->m_name_joint_map.end())
		m_final_transforms[iter->second.m_joint_id] = globalTransform * iter->second.m_offset_tform;

	for (uint32 i = 0; i < pNode->m_child_nodes.size(); ++i)
		_fill_joint_transforms(pNodeTree, pNode->m_child_nodes[i], pCurrentAnim, globalTransform);
}

