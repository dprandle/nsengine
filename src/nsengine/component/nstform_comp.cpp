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
#include <nsgl_xfb.h>
#include <nsgl_vao.h>
#include <nsshader.h>
#include <nsmesh.h>
#include <nsgl_buffer.h>

nstform_comp::nstform_comp():
	nscomponent(),
	m_scenes_info(),
	save_with_scene(true)
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

nsgl_buffer * nstform_comp::transform_buffer(const nsscene * scn)
{
	auto fiter = m_scenes_info.find(scn);
	if (fiter == m_scenes_info.end())
		return nullptr;
	return fiter->second->m_tform_buffer;
}

nsgl_buffer * nstform_comp::transform_id_buffer(const nsscene * scn)
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
		return nullptr;
	return &fiter->second->m_tforms[tform_id];
}

void instance_tform::translate_world_space(const fvec3 & amount)
{
    if (m_parent != nullptr)
        m_position = (m_parent->world_inv_tf() * fvec4(world_position() + amount,1.0f)).xyz();
    else
        m_position += amount;
    update = true;
    m_owner->post_update(true);
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
	std::swap(m_hidden_state, rhs.m_hidden_state);
	std::swap(m_orient, rhs.m_orient);
	std::swap(m_position, rhs.m_position);
	std::swap(m_scaling, rhs.m_scaling);
	std::swap(update, rhs.update);
	std::swap(snap_to_grid, rhs.snap_to_grid);
	return *this;
}

void instance_tform::add_child(instance_tform * child, bool keep_world_transform)
{
	if (m_scene != child->m_scene)
		return;
	
	if (has_child(child))
		return;
	
	if (child->m_parent != nullptr)
        child->m_parent->remove_child(child, keep_world_transform);
		

    if (update)
        recursive_compute();

    m_children.push_back(child);

    if (child->update)
        child->recursive_compute();

    fvec3 wpos = child->world_position();
    fquat worient = child->world_orientation();
	child->m_parent = this;
    child->update = true;
    child->owner()->post_update(true);
    if (keep_world_transform)
    {
        child->set_world_orientation(worient);
        child->set_world_position(wpos);
    }
}

void instance_tform::remove_child(instance_tform * child, bool keep_world_transform)
{
    if (update)
        recursive_compute();

	auto child_iter = m_children.begin();
	while (child_iter != m_children.end())
	{
		if (*child_iter == child)
		{
            m_children.erase(child_iter);

            if (child->update)
                child->recursive_compute();

            fvec3 wpos = child->world_position();
            fquat worient = child->world_orientation();
			child->m_parent = nullptr;
            child->update = true;

            child->owner()->post_update(true);
            if (keep_world_transform)
            {
                child->set_world_orientation(worient);
                child->set_world_position(wpos);
            }
			return;
		}
		++child_iter;
	}
}

void instance_tform::remove_children(bool keep_world_transform)
{
    while (m_children.begin() != m_children.end())
        remove_child(*m_children.begin(),keep_world_transform);
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

void instance_tform::set_parent(instance_tform * parent, bool keep_world_transform)
{
	if (parent == nullptr)
	{
		if (m_parent != nullptr)
            m_parent->remove_child(this,keep_world_transform);
	}
	else
        parent->add_child(this,keep_world_transform);
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
	if (update)
	{
		m_local_tform.set(rotation_mat3(m_orient) % m_scaling);
		m_local_tform.set_column(3, m_position.x, m_position.y, m_position.z, 1);

		fvec4 col3(-m_position.x, -m_position.y, -m_position.z, 1.0f);
		m_local_inv_tform.rotation_from(m_orient).transpose();
		m_local_inv_tform.set_column(3, m_local_inv_tform[0] * col3, m_local_inv_tform[1] * col3, m_local_inv_tform[2] * col3, 1.0f);

		m_world_tform = m_local_tform;
		m_world_inv_tform = m_local_inv_tform;
		m_owner->post_update(true);
		m_render_update = true;
	}
	if ((m_parent != nullptr) && (update || m_parent->update))
	{
		m_world_tform = m_parent->m_world_tform * m_local_tform;
        m_world_inv_tform = inverse(m_world_tform);
        //m_world_inv_tform =  m_world_inv_tform * m_parent->m_world_inv_tform;
		m_owner->post_update(true);
		m_render_update = true;
	}
	for (uint32 i = 0; i < m_children.size(); ++i)
		m_children[i]->recursive_compute();
	update = false;
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

void instance_tform::translate(const fvec3 & amount)
{
    m_position += amount;
    update = true;
    m_owner->post_update(true);
}

void instance_tform::scale(const fvec3 & amount)
{
    m_scaling %= amount;
    update = true;
    m_owner->post_update(true);
}

int32 instance_tform::hidden_state() const
{
	return m_hidden_state;
}

void instance_tform::set_hidden_state(int32 state)
{
    m_hidden_state = state;
}

void instance_tform::rotate(const fquat & rotation)
{
    m_orient = rotation * m_orient;
    update = true;
    m_owner->post_update(true);
}

fvec3 instance_tform::world_position() const
{
	if (m_parent != nullptr)
        return (m_parent->world_tf() * fvec4(m_position)).xyz();
	return m_position;
}

fquat instance_tform::world_orientation() const
{
    if (m_parent != nullptr)
        return m_parent->world_orientation() * m_orient;
    return m_orient;
}

void instance_tform::set_world_position(const fvec3 & pos)
{
	if (m_parent != nullptr)
        m_position = (m_parent->world_inv_tf() * fvec4(pos,1.0f)).xyz();
	else
		m_position = pos;
    update = true;
    m_owner->post_update(true);
}

void instance_tform::set_world_orientation(const fquat & orient_)
{
    if (m_parent != nullptr)
    {
        m_orient = m_parent->world_orientation().invert() * orient_;
    }
    else
        m_orient = orient_;
    update = true;
    m_owner->post_update(true);
}

void instance_tform::set_local_position(const fvec3 & pos)
{
    m_position = pos;
    update = true;
    m_owner->post_update(true);
}

void instance_tform::set_local_orientation(const fquat & orient_)
{
    m_orient = orient_;
    update = true;
    m_owner->post_update(true);
}

const fvec3 & instance_tform::local_position() const
{
    return m_position;
}

const fquat & instance_tform::local_orientation() const
{
    return m_orient;
}

void instance_tform::set_scaling(const fvec3 & scale_)
{
    m_scaling = scale_;
    update = true;
    m_owner->post_update(true);
}

const fvec3 & instance_tform::scaling() const
{
    return m_scaling;
}

instance_tform * instance_tform::child(uint32 index)
{
    if (index >= m_children.size())
        return nullptr;
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
