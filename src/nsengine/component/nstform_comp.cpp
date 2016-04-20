/*! 
	\file nstformcomp.cpp
	
	\brief Definition file for nstform_comp class

	This file contains all of the neccessary definitions for the nstform_comp class.

	\author Daniel Randle
	\date November 23 2013
	\copywrite Earth Banana Games 2013
*/

#include <nstform_comp.h>
#include <nsrender_comp.h>
#include <nsentity.h>
#include <nstile_grid.h>
#include <nsengine.h>
#include <nsxfb_object.h>
#include <nsvertex_array_object.h>
#include <nsshader.h>
#include <nsmesh.h>
#include <nsbuffer_object.h>

nstform_comp::nstform_comp():
	nscomponent(),
	m_scenes_info()
{}

nstform_comp::nstform_comp(const nstform_comp & copy):
	nscomponent(copy),
	m_scenes_info(copy.m_scenes_info)
{}

nstform_comp::~nstform_comp()
{
}

// uint32 nstform_comp::add()
// {
// 	return add(instance_tform());
// }

// uint32 nstform_comp::add(const instance_tform & trans)
// {
// 	m_tforms.push_back(trans);
// 	post_update(true);
// 	m_buffer_resized = true;
// 	return static_cast<uint32>(m_tforms.size()) - 1;
// }

// uint32 nstform_comp::add(uint32 cnt_)
// {
// 	// Get the transformID of the last transform before adding cnt_ and return it
// 	uint32 ret = static_cast<uint32>(m_tforms.size()) - 1;
// 	m_tforms.resize(m_tforms.size() + cnt_);
// 	post_update(true);
// 	m_buffer_resized = true;
// 	return ret;
// }

void nstform_comp::pup(nsfile_pupper * p)
{
	return; // not puppable
}

void nstform_comp::init()
{
}

nsbuffer_object * nstform_comp::transform_buffer(const nsscene * scn)
{
	auto fiter = m_scenes_info.find(scn);
	if (fiter == m_scenes_info.end())
		return nullptr;
	return fiter->second->m_tform_buffer;
}

nsbuffer_object * nstform_comp::transform_id_buffer(const nsscene * scn)
{
	auto fiter = m_scenes_info.find(scn);
	if (fiter == m_scenes_info.end())
		return nullptr;
	return fiter->second->m_tform_id_buffer;
}

void nstform_comp::release()
{
}

uint32 nstform_comp::instance_count(const nsscene * scn) const
{
	auto fiter = m_scenes_info.find(scn);
	if (fiter == m_scenes_info.end())
		return 0;
	return fiter->second->m_tforms.size();	
}

instance_tform * nstform_comp::instance_transform(const nsscene * scn, uint32 tform_id)
{
	auto fiter = m_scenes_info.find(scn);
	if (fiter == m_scenes_info.end())
		return 0;
	return &fiter->second->m_tforms[tform_id];
}

nstform_comp & nstform_comp::operator=(nstform_comp rhs_)
{
	nscomponent::operator=(rhs_);
	std::swap(m_scenes_info, rhs_.m_scenes_info);
	post_update(true);
	return (*this);
}

instance_tform & instance_tform::operator=(instance_tform rhs)
{
	std::swap(hidden_state, rhs.hidden_state);
	std::swap(orient, rhs.orient);
	std::swap(position, rhs.position);
	std::swap(scaling, rhs.scaling);
	std::swap(update, rhs.update);
	std::swap(snap_to_grid, rhs.snap_to_grid);
	return *this;
}

void instance_tform::add_child(instance_tform * child)
{
	if (m_scene != child->m_scene)
		return;
	
	if (has_child(child))
		return;
	
	if (child->m_parent != nullptr)
		child->m_parent->remove_child(child);
		
	m_children.push_back(child);
	child->m_parent = this;
}

void instance_tform::remove_child(instance_tform * child)
{
	auto child_iter = m_children.begin();
	while (child_iter != m_children.end())
	{
		if (*child_iter == child)
		{
			m_children.erase(child_iter);
			child->m_parent = nullptr;
			return;
		}
		++child_iter;
	}
}

void instance_tform::remove_children()
{
	auto child_iter = m_children.begin();
	while (child_iter != m_children.end())
	{
		(*child_iter)->m_parent = nullptr;
		++child_iter;
	}
	m_children.clear();
}

bool instance_tform::has_child(instance_tform * child)
{
	for (uint32 i = 0; i < m_children.size(); ++i)
	{
		if (m_children[i] == child || m_children[i]->has_child(child))
			return true;
	}
	return false;
}

void instance_tform::set_parent(instance_tform * parent)
{
	if (parent == nullptr)
	{
		if (m_parent != nullptr)
			m_parent->remove_child(this);
	}
	else
		parent->add_child(this);	
}

instance_tform * instance_tform::parent()
{
	return m_parent;	
}

nsscene * instance_tform::scene()
{
	return m_scene;
}

void instance_tform::recursive_compute()
{
//	if (update)
//	{
		m_local_tform.set(rotation_mat3(orient) % scaling);
		m_local_tform.set_column(3, position.x, position.y, position.z, 1);

		fvec4 col3(-position.x, -position.y, -position.z, 1.0f);
		m_local_inv_tform.rotation_from(orient).transpose();
		m_local_inv_tform.set_column(3, m_local_inv_tform[0] * col3, m_local_inv_tform[1] * col3, m_local_inv_tform[2] * col3, 1.0f);

		if (m_parent != nullptr)
		{
			m_world_tform = m_parent->m_world_tform * m_local_tform;
			m_world_inv_tform = inverse(m_world_tform);
			// would like to try - eventually - m_world_tform = m_local_inv_tform * m_parent->m_world_inv_tform
		}
//		update = false;
//	}
	for (uint32 i = 0; i < m_children.size(); ++i)
		m_children[i]->recursive_compute();
}

nstform_comp * instance_tform::owner()
{
	return m_owner;
}

const fmat4 & instance_tform::world_tf() const
{
	return m_world_tform;
}

const fmat4 & instance_tform::world_inv_tf() const
{
	return m_world_inv_tform;
}

const fmat4 & instance_tform::local_tf() const
{
	return m_local_tform;
}

const fmat4 & instance_tform::local_inv_tf() const
{
	return m_local_inv_tform;
}

fvec3 instance_tform::world_position() const
{
	if (m_parent != nullptr)
		return (m_parent->m_world_tform * position).xyz();
	return position;
}

void instance_tform::set_world_position(const fvec3 & pos)
{
	if (m_parent != nullptr)
		position = (m_parent->world_inv_tf() * pos).xyz();
	else
		position = pos;
}

instance_tform * instance_tform::child(uint32 index)
{
	return m_children[index];
}

uint32 instance_tform::child_count()
{
	return m_children.size();
}

nstform_comp::per_scene_info::per_scene_info():
	m_tforms(),
	m_tform_buffer(new nsbuffer_object(nsbuffer_object::array, nsbuffer_object::storage_mutable)),
	m_tform_id_buffer(new nsbuffer_object(nsbuffer_object::array, nsbuffer_object::storage_mutable)),
	m_buffer_resized(false),
	m_visible_count(0)
{
	m_tform_buffer->video_init();
	m_tform_id_buffer->video_init();
}

nstform_comp::per_scene_info::~per_scene_info()
{
	m_tform_buffer->video_release();
	m_tform_id_buffer->video_release();
	delete m_tform_buffer;
	delete m_tform_id_buffer;
}
