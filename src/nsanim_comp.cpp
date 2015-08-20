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

nsanim_comp::nsanim_comp():nscomponent(),
	m_elapsed_time(0.0f),
	m_animation(false),
	m_looping(false),
	m_anim_set_id(0),
	m_current_anim(),
	m_final_transforms()
{}

nsanim_comp::~nsanim_comp()
{}

nsanim_comp* nsanim_comp::copy(const nscomponent * pToCopy)
{
	if (pToCopy == NULL)
		return NULL;
	const nsanim_comp * cmp = (const nsanim_comp*)pToCopy;
	(*this) = (*cmp);
	return this;
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

void nsanim_comp::fill_bones(nsmesh::node_tree * pNodeTree, nsanim_set::animation_data * pCurrentAnim)
{
	_fill_bone_transforms(pNodeTree, pNodeTree->root_node, pCurrentAnim, pNodeTree->root_node->world_transform);
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
uivec2_vector nsanim_comp::resources()
{
	// Build map
	uivec2_vector ret;
	
	// only add if not 0
	if (m_anim_set_id != 0)
		ret.push_back(m_anim_set_id);

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

nsanim_comp & nsanim_comp::operator=(const nsanim_comp & pRHSComp)
{
	m_elapsed_time = pRHSComp.m_elapsed_time;
	m_animation = pRHSComp.m_animation;
	m_anim_set_id = pRHSComp.m_anim_set_id;
	m_current_anim = pRHSComp.m_current_anim;
	m_final_transforms.clear();
	post_update(true);
	return (*this);
}

void nsanim_comp::_fill_bone_transforms(nsmesh::node_tree * pNodeTree, nsmesh::node * pNode, nsanim_set::animation_data * pCurrentAnim, fmat4 & pParentTransform)
{
	fmat4 globalTransform;
	nsstringstream ss;

	if (pNodeTree->bone_name_map.empty())
	{
		dprint("nsanim_comp::_fillBoneTransform Animation has no bones");
		return;
	}

	if (pCurrentAnim != NULL && pCurrentAnim->anim_node(pNode->name) != NULL)
		globalTransform = pParentTransform * pCurrentAnim->bone_transform(pNode->name, m_elapsed_time);
	else
		globalTransform = pParentTransform * pNode->node_transform;

	std::map<nsstring, nsmesh::bone>::const_iterator iter = pNodeTree->bone_name_map.find(pNode->name);
	if (iter != pNodeTree->bone_name_map.end())
		m_final_transforms[iter->second.boneID] = globalTransform * iter->second.mOffsetTransform;

	for (uint32 i = 0; i < pNode->child_nodes.size(); ++i)
		_fill_bone_transforms(pNodeTree, pNode->child_nodes[i], pCurrentAnim, globalTransform);
}

