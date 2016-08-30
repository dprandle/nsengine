/*! 
	\file nstformcomp.cpp
	
	\brief Definition file for nstform_comp class

	This file contains all of the neccessary definitions for the nstform_comp class.

	\author Daniel Randle
	\date November 23 2013
	\copywrite Earth Banana Games 2013
*/

#include <nscube_grid.h>
#include <component/nstform_comp.h>
#include <component/nsrender_comp.h>
#include <asset/nsentity.h>
#include <nstile_grid.h>
#include <nsengine.h>
#include <opengl/nsgl_xfb.h>
#include <opengl/nsgl_vao.h>
#include <asset/nsshader.h>
#include <asset/nsmesh.h>
#include <opengl/nsgl_buffer.h>
#include <asset/nsscene.h>

tform_info::tform_info():
	m_parent(),
	m_hidden_state(0),
	m_orient(),
	m_position(),
	m_scaling(1.0f, 1.0f, 1.0f),
	m_children()
{}

nstform_comp::nstform_comp():
	nscomponent(type_to_hash(nstform_comp)),
	inst_obj(nullptr),
	inst_id(0),
	in_cube_grid(false),
	m_tfi(),
	m_render_update(true),
	m_world_tform(),
	m_world_inv_tform(),
	m_local_tform(),
	m_local_inv_tform()
{}

nstform_comp::nstform_comp(const nstform_comp & copy):
	nscomponent(copy),
	inst_obj(nullptr),
	inst_id(0),
	in_cube_grid(false),
	m_tfi(copy.m_tfi),
	m_render_update(true),
	m_world_tform(copy.m_world_tform),
	m_world_inv_tform(copy.m_world_inv_tform),
	m_local_tform(copy.m_local_tform),
	m_local_inv_tform(copy.m_local_inv_tform)
{}

nstform_comp::~nstform_comp()
{
	if (inst_obj != nullptr)
		release();
}

nstform_comp & nstform_comp::operator=(nstform_comp rhs)
{
	nscomponent::operator=(rhs);
	std::swap(m_owner, rhs.m_owner);
	std::swap(m_tfi, rhs.m_tfi);
	std::swap(m_render_update, rhs.m_render_update);
	std::swap(m_world_tform, rhs.m_world_tform);
	std::swap(m_world_inv_tform, rhs.m_world_inv_tform);
	std::swap(m_local_tform, rhs.m_local_tform);
	std::swap(m_local_inv_tform, rhs.m_local_inv_tform);
	post_update(true);
	return *this;
}


void nstform_comp::pup(nsfile_pupper * p)
{
	return; // not puppable
}

void nstform_comp::init()
{}

void nstform_comp::release()
{
	if (inst_obj != nullptr)
	{
		auto iter = inst_obj->shared_geom_tforms.begin();
		while (iter != inst_obj->shared_geom_tforms.end())
		{
			if (this == *iter)
			{
				iter = (*iter)->inst_obj->shared_geom_tforms.erase(iter);
				dprint("~tform_comp making myself no longer instanced");

#ifdef NSDEBUG
				nsstringstream ss;
				ss << "The following instanced ents remain:\n";
				for (uint32 i = 0; i < inst_obj->shared_geom_tforms.size(); ++i)
					ss << inst_obj->shared_geom_tforms[i]->owner()->name() << "\n";
				dprint(ss.str());
#endif
				inst_obj = nullptr;

				// If there is only one entity left - the auto update code should remove that and delete
				// the obj
				return;
			}
			else
			{
				++iter;
			}
		}
	}
}

void nstform_comp::translate_world_space(const fvec3 & amount)
{
	nsentity * ent = m_scene->find_entity(m_tfi.m_parent);
    if (ent != nullptr)
	{
		nstform_comp * itf = ent->get<nstform_comp>();
        m_tfi.m_position = (itf->world_inv_tf() * fvec4(world_position() + amount,1.0f)).xyz();
	}
    else
        m_tfi.m_position += amount;
    post_update(true);
}

void nstform_comp::add_child(nstform_comp * child, bool keep_world_transform)
{
	assert(child != nullptr);
	if (has_child(child))
		return;

	nsentity * parnt = m_scene->find_entity(child->m_tfi.m_parent);
	if (parnt != nullptr)
	{
		nstform_comp * cparent_itf = parnt->get<nstform_comp>();
        cparent_itf->remove_child(child, keep_world_transform);
	}
		
    if (update_posted())
        recursive_compute();

    m_tfi.m_children.push_back(child->owner()->full_id());

    if (child->update_posted())
        child->recursive_compute();

    fvec3 wpos = child->world_position();
    fquat worient = child->world_orientation();
	child->m_tfi.m_parent = owner()->full_id();
    child->post_update(true);
    if (keep_world_transform)
    {
        child->set_world_orientation(worient);
        child->set_world_position(wpos);
    }
}

void nstform_comp::remove_child(nstform_comp * child, bool keep_world_transform)
{
    if (update_posted())
        recursive_compute();

	nsentity * ent = child->owner();
	auto child_iter = m_tfi.m_children.begin();
	while (child_iter != m_tfi.m_children.end())
	{
		if (*child_iter == ent->full_id())
		{
			dprint("nstform_comp::remove_child removing child " + ent->name() + " from parent " + owner()->name());
			m_tfi.m_children.erase(child_iter);

			if (child->update_posted())
				child->recursive_compute();

			fvec3 wpos = child->world_position();
			fquat worient = child->world_orientation();
			child->m_tfi.m_parent = uivec2(0);
			child->post_update(true);
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

void nstform_comp::remove_children(bool keep_world_transform)
{
	auto citer = m_tfi.m_children.begin();
    while (citer != m_tfi.m_children.end())
	{
		nsentity * ent = m_scene->find_entity(*citer);
		if (ent != nullptr)
		{
			nstform_comp * chld = ent->get<nstform_comp>();
			remove_child(chld, keep_world_transform);
		}
		else
		{
			dprint("nstform_comp::remove_children - child with id " + citer->to_string() + " could not be located in scene - will be removed anyway");
		}
		++citer;
	}
	m_tfi.m_children.clear();
}

bool nstform_comp::has_child(nstform_comp * child)
{
	for (uint32 i = 0; i < m_tfi.m_children.size(); ++i)
	{
		if (m_tfi.m_children[i] == child->owner()->full_id() || child->has_child(child))
			return true;
	}
	return false;
}

void nstform_comp::set_parent(nstform_comp * parent, bool keep_world_transform)
{
	if (parent == nullptr)
	{
		nsentity * cur_parent = m_scene->find_entity(m_tfi.m_parent);
		if (cur_parent != nullptr)
		{
			nstform_comp * par_itf = cur_parent->get<nstform_comp>();
            par_itf->remove_child(this, keep_world_transform);
		}
	}
	else
        parent->add_child(this,keep_world_transform);
}

nstform_comp * nstform_comp::parent() const
{
	nsentity * par = m_scene->find_entity(m_tfi.m_parent);
	if (par != nullptr)
		return par->get<nstform_comp>();
	return nullptr;
}

void nstform_comp::recursive_compute()
{
	if (update_posted())
	{
		m_local_tform.set(rotation_mat3(m_tfi.m_orient) % m_tfi.m_scaling);
		m_local_tform.set_column(3, m_tfi.m_position.x, m_tfi.m_position.y, m_tfi.m_position.z, 1);

		fvec4 col3(-m_tfi.m_position.x, -m_tfi.m_position.y, -m_tfi.m_position.z, 1.0f);
		m_local_inv_tform.rotation_from(m_tfi.m_orient).transpose();
		m_local_inv_tform.set_column(3, m_local_inv_tform[0] * col3, m_local_inv_tform[1] * col3, m_local_inv_tform[2] * col3, 1.0f);

		m_world_tform = m_local_tform;
		m_world_inv_tform = m_local_inv_tform;

		nstform_comp * par_itf = parent();
	
		if (par_itf != nullptr)
		{
			m_world_tform = par_itf->m_world_tform * m_local_tform;
			m_world_inv_tform = inverse(m_world_tform);
			//m_world_inv_tform =  m_world_inv_tform * m_tfi.m_parent->m_world_inv_tform;
		}
		m_render_update = true;
		post_update(false);
	}

	for (uint32 i = 0; i < m_tfi.m_children.size(); ++i)
    {
		nsentity * chld = m_scene->find_entity(m_tfi.m_children[i]);
		if (chld != nullptr)
		{
			nstform_comp * child_itf = chld->get<nstform_comp>();
			child_itf->post_update(child_itf->m_update || m_render_update);
			child_itf->recursive_compute();
		}
		else
		{
			dprint("nstform_comp::recursive_compute entity " + m_owner->name() + " has child that cannot be found in scene - id " + m_tfi.m_children[i].to_string());
		}
    }
}

const fmat4 & nstform_comp::world_tf() const
{
	return m_world_tform;
}

const fmat4 & nstform_comp::world_inv_tf() const
{
	return m_world_inv_tform;
}

const fmat4 & nstform_comp::local_tf() const
{
	return m_local_tform;
}

const fmat4 & nstform_comp::local_inv_tf() const
{
	return m_local_inv_tform;
}

void nstform_comp::translate(const fvec3 & amount)
{
    m_tfi.m_position += amount;
	post_update(true);
}

void nstform_comp::scale(const fvec3 & amount)
{
    m_tfi.m_scaling %= amount;
	post_update(true);
}

int32 nstform_comp::hidden_state() const
{
	return m_tfi.m_hidden_state;
}

void nstform_comp::set_hidden_state(int32 state)
{
    m_tfi.m_hidden_state = state;
}

void nstform_comp::rotate(const fquat & rotation)
{
    m_tfi.m_orient = rotation * m_tfi.m_orient;
	post_update(true);
}

fvec3 nstform_comp::world_position() const
{
	nstform_comp * par_itf = parent();
	if (par_itf != nullptr)
        return (par_itf->world_tf() * fvec4(m_tfi.m_position)).xyz();
	return m_tfi.m_position;
}

fquat nstform_comp::world_orientation() const
{
	nstform_comp * par_itf = parent();
    if (par_itf != nullptr)
        return par_itf->world_orientation() * m_tfi.m_orient;
    return m_tfi.m_orient;
}

void nstform_comp::set_world_position(const fvec3 & pos)
{
	nstform_comp * par_itf = parent();

	if (par_itf != nullptr)
        m_tfi.m_position = (par_itf->world_inv_tf() * fvec4(pos,1.0f)).xyz();
	else
		m_tfi.m_position = pos;
	
	post_update(true);
}

void nstform_comp::set_world_orientation(const fquat & orient_)
{
	nstform_comp * par_itf = parent();

	if (par_itf != nullptr)
        m_tfi.m_orient = par_itf->world_orientation().invert() * orient_;
    else
        m_tfi.m_orient = orient_;
	post_update(true);
}

void nstform_comp::set_local_position(const fvec3 & pos)
{
    m_tfi.m_position = pos;
	post_update(true);
}

void nstform_comp::set_local_orientation(const fquat & orient_)
{
    m_tfi.m_orient = orient_;
	post_update(true);
}

const fvec3 & nstform_comp::local_position() const
{
    return m_tfi.m_position;
}

const fquat & nstform_comp::local_orientation() const
{
    return m_tfi.m_orient;
}

void nstform_comp::set_scaling(const fvec3 & scale_)
{
    m_tfi.m_scaling = scale_;
	post_update(true);
}

const fvec3 & nstform_comp::scaling() const
{
    return m_tfi.m_scaling;
}

nstform_comp * nstform_comp::child(uint32 index)
{
    if (index >= m_tfi.m_children.size())
        return nullptr;
	nsentity * ent = m_scene->find_entity(m_tfi.m_children[index]);
	if (ent != nullptr)
		return ent->get<nstform_comp>();
	return nullptr;
}

uint32 nstform_comp::child_count()
{
	return m_tfi.m_children.size();
}

void nstform_comp::set_render_update(bool val)
{
	m_render_update = val;
}

const tform_info & nstform_comp::tf_info() const
{
	return m_tfi;
}

void nstform_comp::set_tf_info(const tform_info & tfi_)
{
	nsentity * prnt = m_scene->find_entity(tfi_.m_parent);
	if (prnt != nullptr)
		set_parent(prnt->get<nstform_comp>(), true);
	remove_children(true);
	for (uint32 i = 0; i < tfi_.m_children.size(); ++i)
	{
		nsentity * child = m_scene->find_entity(tfi_.m_children[i]);
		if (child != nullptr)
			add_child(child->get<nstform_comp>(), true);
	}
	set_local_position(tfi_.m_position);
	set_local_orientation(tfi_.m_orient);
	set_scaling(tfi_.m_scaling);
	set_hidden_state(tfi_.m_hidden_state);
	post_update(true);
}

bool nstform_comp::render_update() const
{
	return m_render_update;
}

tform_per_scene_info::tform_per_scene_info():
	nsvideo_object(),
	shared_geom_tforms(),
	visible_count(0),
	needs_update(true)
{}

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
			dprint("nstform_per_scene_info::video_context_init - Context has already been initialized for tform_psi in ctxtid " + std::to_string(vc->context_id));
		}
	}
}

tform_per_scene_info::~tform_per_scene_info()
{}
