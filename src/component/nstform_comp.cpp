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
#include <nsscene.h>

instance_handle::instance_handle(tform_per_scene_info * tfc_, uint32 tf_ind):
	tfc(tfc_),
	ind(tf_ind)
{}

bool instance_handle::is_valid() const
{
	return tfc != nullptr && ind < tfc->m_tforms.size();
}

void instance_handle::invalidate()
{
	tfc = nullptr; ind = -1;
}

bool instance_handle::operator==(const instance_handle & rhs)
{
	return tfc == rhs.tfc && ind == rhs.ind;
}

nstform_comp::nstform_comp():
	nscomponent(type_to_hash(nstform_comp)),
	save_with_scene(true),
	m_scenes_info()
{}

nstform_comp::nstform_comp(const nstform_comp & copy):
	nscomponent(copy),
	m_scenes_info(copy.m_scenes_info)
{}

nstform_comp::~nstform_comp()
{
}

void nstform_comp::pup(nsfile_pupper * p)
{
	return; // not puppable
}

tform_per_scene_info * nstform_comp::per_scene_info(const nsscene * scn)
{
	auto fiter = m_scenes_info.find(scn);
	if (fiter == m_scenes_info.end())
		return nullptr;
	return fiter->second;	
}

void nstform_comp::init()
{
}

void nstform_comp::release()
{
}

void instance_tform::translate_world_space(const fvec3 & amount)
{
    if (m_parent.is_valid())
	{
		instance_tform & itf = m_parent.tfc->m_tforms[m_parent.ind];
        m_position = (itf.world_inv_tf() * fvec4(world_position() + amount,1.0f)).xyz();
	}
    else
        m_position += amount;
    update = true;
    m_owner->owner->post_update(true);
}

nstform_comp & nstform_comp::operator=(nstform_comp rhs_)
{
	nscomponent::operator=(rhs_);
	std::swap(m_scenes_info, rhs_.m_scenes_info);
	post_update(true);
	return (*this);
}

instance_tform::instance_tform():
	update(true),
	snap_to_grid(true),
	m_owner(nullptr),
	m_parent(),
	m_render_update(true),
	m_hidden_state(0),
	m_orient(),
	m_position(),
	m_scaling(1.0f, 1.0f, 1.0f),
	m_children(),
	m_world_tform(),
	m_world_inv_tform(),
	m_local_tform(),
	m_local_inv_tform()
{}

instance_tform::instance_tform(const instance_tform & copy):
	update(true),
	snap_to_grid(copy.snap_to_grid),
	m_owner(copy.m_owner),
	m_parent(copy.m_parent),
	m_render_update(true),
	m_hidden_state(copy.m_hidden_state),
	m_orient(copy.m_orient),
	m_position(copy.m_position),
	m_scaling(copy.m_scaling),
	m_children(copy.m_children),
	m_world_tform(copy.m_world_tform),
	m_world_inv_tform(copy.m_world_inv_tform),
	m_local_tform(copy.m_local_tform),
	m_local_inv_tform(copy.m_local_inv_tform)
{}

instance_tform & instance_tform::operator=(instance_tform rhs)
{
	std::swap(m_owner, rhs.m_owner);
	std::swap(m_parent, rhs.m_parent);
	std::swap(m_render_update, rhs.m_render_update);
	std::swap(m_hidden_state, rhs.m_hidden_state);
	std::swap(m_orient, rhs.m_orient);
	std::swap(m_position, rhs.m_position);
	std::swap(m_scaling, rhs.m_scaling);
	std::swap(m_children, rhs.m_children);
	std::swap(m_world_tform, rhs.m_world_tform);
	std::swap(m_world_inv_tform, rhs.m_world_inv_tform);
	std::swap(m_local_tform, rhs.m_local_tform);
	std::swap(m_local_inv_tform, rhs.m_local_inv_tform);
	std::swap(update, rhs.update);
	std::swap(snap_to_grid, rhs.snap_to_grid);
	return *this;
}

void instance_tform::add_child(instance_tform * child, bool keep_world_transform)
{
	if (m_owner != child->m_owner)
		return;
	
	if (has_child(child))
		return;
	
	if (child->m_parent.is_valid())
	{
		instance_tform & cparent_itf = child->m_parent.tfc->m_tforms[child->m_parent.ind];
        cparent_itf.remove_child(child, keep_world_transform);
	}
		
    if (update)
        recursive_compute();

    m_children.push_back(instance_handle(child->owner(),child->current_tform_id()));

    if (child->update)
        child->recursive_compute();

    fvec3 wpos = child->world_position();
    fquat worient = child->world_orientation();
	child->m_parent = instance_handle(owner(),current_tform_id());
    child->update = true;
    child->owner()->owner->post_update(true);
    if (keep_world_transform)
    {
        child->set_world_orientation(worient);
        child->set_world_position(wpos);
    }
}

uint32 instance_tform::current_tform_id()
{
	if (m_owner->m_tforms.empty())
		return -1;
    return (this - &(m_owner->m_tforms[0]));
}

void instance_tform::remove_child(instance_tform * child, bool keep_world_transform)
{
    if (update)
        recursive_compute();

	instance_handle hnd(child->owner(),child->current_tform_id());
	
	auto child_iter = m_children.begin();
	while (child_iter != m_children.end())
	{
		if (*child_iter == hnd)
		{
            m_children.erase(child_iter);

            if (child->update)
                child->recursive_compute();

            fvec3 wpos = child->world_position();
            fquat worient = child->world_orientation();
			child->m_parent.invalidate();
            child->update = true;

            child->owner()->owner->post_update(true);
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
	auto citer = m_children.begin();
    while (citer != m_children.end())
	{
		if (citer->is_valid())
		{
			instance_tform & chld = citer->tfc->m_tforms[citer->ind];
			remove_child(&chld, keep_world_transform);
		}
		++citer;
	}
	m_children.clear();
}

bool instance_tform::has_child(instance_tform * child)
{
	instance_handle hnd(child->m_owner, child->current_tform_id());
	for (uint32 i = 0; i < m_children.size(); ++i)
	{
		instance_tform & chld = m_children[i].tfc->m_tforms[m_children[i].ind];
		if (m_children[i] == hnd || chld.has_child(child))
			return true;
	}
	return false;
}

void instance_tform::set_parent(instance_tform * parent, bool keep_world_transform)
{
	if (parent == nullptr)
	{
		if (m_parent.is_valid())
		{
			instance_tform & par_itf = m_parent.tfc->m_tforms[m_parent.ind];
            par_itf.remove_child(this, keep_world_transform);
		}
	}
	else
        parent->add_child(this,keep_world_transform);
}

instance_tform * instance_tform::parent() const
{
	if (m_parent.is_valid())
		return &m_parent.tfc->m_tforms[m_parent.ind];
	return nullptr;
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
		m_owner->owner->post_update(true);
		m_render_update = true;
	}
	if (m_parent.is_valid())
	{
		instance_tform * par_itf = parent();
		if (update || par_itf->update)
		{
			m_world_tform = par_itf->m_world_tform * m_local_tform;
			m_world_inv_tform = inverse(m_world_tform);
			//m_world_inv_tform =  m_world_inv_tform * m_parent->m_world_inv_tform;
			m_owner->owner->post_update(true);
			m_render_update = true;
		}
	}
	for (uint32 i = 0; i < m_children.size(); ++i)
    {
		instance_tform & child_itf = m_children[i].tfc->m_tforms[m_children[i].ind];
        child_itf.update = true;
		child_itf.recursive_compute();
    }
	update = false;
}

tform_per_scene_info * instance_tform::owner()
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
    m_owner->owner->post_update(true);
}

void instance_tform::scale(const fvec3 & amount)
{
    m_scaling %= amount;
    update = true;
    m_owner->owner->post_update(true);
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
    m_owner->owner->post_update(true);
}

fvec3 instance_tform::world_position() const
{
	instance_tform * par_itf = parent();
	if (par_itf != nullptr)
        return (par_itf->world_tf() * fvec4(m_position)).xyz();
	return m_position;
}

fquat instance_tform::world_orientation() const
{
	instance_tform * par_itf = parent();
    if (par_itf != nullptr)
        return par_itf->world_orientation() * m_orient;
    return m_orient;
}

void instance_tform::set_world_position(const fvec3 & pos)
{
	instance_tform * par_itf = parent();

	if (par_itf != nullptr)
        m_position = (par_itf->world_inv_tf() * fvec4(pos,1.0f)).xyz();
	else
		m_position = pos;

    update = true;
    m_owner->owner->post_update(true);
}

void instance_tform::set_world_orientation(const fquat & orient_)
{
	instance_tform * par_itf = parent();

	if (m_parent.is_valid())
        m_orient = par_itf->world_orientation().invert() * orient_;
    else
        m_orient = orient_;

    update = true;
    m_owner->owner->post_update(true);
}

void instance_tform::set_local_position(const fvec3 & pos)
{
    m_position = pos;
    update = true;
    m_owner->owner->post_update(true);
}

void instance_tform::set_local_orientation(const fquat & orient_)
{
    m_orient = orient_;
    update = true;
    m_owner->owner->post_update(true);
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
    m_owner->owner->post_update(true);
}

const fvec3 & instance_tform::scaling() const
{
    return m_scaling;
}

instance_tform * instance_tform::child(uint32 index)
{
    if (index >= m_children.size())
        return nullptr;

	if (m_children[index].is_valid())
		return &m_children[index].tfc->m_tforms[m_children[index].ind];
	
	return nullptr;
}

uint32 instance_tform::child_count()
{
	return m_children.size();
}

void instance_tform::set_render_update(bool val)
{
	m_render_update = val;
}

bool instance_tform::render_update() const
{
	return m_render_update;
}

tform_per_scene_info::tform_per_scene_info(nstform_comp * owner_, nsscene * scn):
	nsvideo_object(),
	owner(owner_),
	scene(scn),
	m_tforms(),
	m_buffer_resized(false),
	m_visible_count(0)
{
	video_context_init();
}

void tform_per_scene_info::video_context_init()
{
	vid_ctxt * vc = nse.video_driver()->current_context();
	if (vc != nullptr)
	{
		if (ctxt_objs[vc->context_id] == nullptr)
		{
			ctxt_objs[vc->context_id] = nse.factory<nsvid_obj_factory>(TFORM_VID_OBJ_GUID)->create(this);
		}
		else
		{
			dprint("nstform_per_scene_info::video_context_init - Context has already been initialized for tform_psi " + owner->owner()->name() + " in scene " + scene->name() + " in ctxtid " + std::to_string(vc->context_id));
		}
	}
}

tform_per_scene_info::~tform_per_scene_info()
{}
