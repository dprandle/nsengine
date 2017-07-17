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
#include <nsentity.h>
#include <nstile_grid.h>
#include <nsengine.h>
#include <opengl/nsgl_xfb.h>
#include <opengl/nsgl_vao.h>
#include <asset/nsshader.h>
#include <asset/nsmesh.h>
#include <opengl/nsgl_buffer.h>
#include <nsworld_data.h>

tform_info::tform_info(
	uint32 parent,
	const fvec3 & pos,
	const fquat & ornt,
	const fvec3 & scale,
	int32 hide_state,
	const ui_vector & children_):
	m_parent(parent),
	m_hidden_state(hide_state),
	m_orient(ornt),
	m_position(pos),
	m_scaling(scale),
	m_children(children_)
{}

nsstring tform_info::to_string() const
{
	return ("pos: " + m_position.to_string() + "    orient: " + m_orient.to_string() + "    scaling: " + m_scaling.to_string());
}

nstform_comp::nstform_comp():
	nscomponent(type_to_hash(nstform_comp)),
	inst_obj(nullptr),
	m_tfi(),
	m_render_update(true),
	m_world_tform(),
	m_world_inv_tform(),
	m_local_tform(),
	m_local_inv_tform(),
	m_owning_chunk(nullptr)
{}

nstform_comp::nstform_comp(const nstform_comp & copy):
	nscomponent(copy),
	inst_obj(nullptr),
	m_tfi(copy.m_tfi),
	m_render_update(true),
	m_world_tform(copy.m_world_tform),
	m_world_inv_tform(copy.m_world_inv_tform),
	m_local_tform(copy.m_local_tform),
	m_local_inv_tform(copy.m_local_inv_tform),
	m_owning_chunk(nullptr)
{}

nstform_comp::~nstform_comp()
{
	if (inst_obj != nullptr)
		release();
}

nstform_comp & nstform_comp::operator=(nstform_comp rhs)
{
	nscomponent::operator=(rhs);
	std::swap(m_owning_chunk, rhs.m_owning_chunk);
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
	nstform_comp * prnt = parent();

    if (prnt != nullptr)
        m_tfi.m_position = (prnt->world_inv_tf() * fvec4(world_position() + amount,1.0f)).xyz();
    else
        m_tfi.m_position += amount;

	post_update(true);
}

void nstform_comp::add_child(nstform_comp * chld, bool keep_world_transform)
{
	assert(this != chld && child != nullptr);
	if (has_child(chld))
	{
		dprint("Cannot add " + chld->owner()->name() + " as child of " + owner()->name() + " - already a child");
		return;
	}

	nstform_comp * cparent_itf = chld->parent();
	if (cparent_itf != nullptr)
		cparent_itf->remove_child(chld, keep_world_transform);

	// This will update our transform in case our parent
    if (update_posted())
        recursive_compute();

    m_tfi.m_children.push_back(chld->owner()->id());

    if (chld->update_posted())
        chld->recursive_compute();

    fvec3 wpos = chld->world_position();
    fquat worient = chld->world_orientation();
	chld->m_tfi.m_parent = owner()->id();
    chld->post_update(true);
    if (keep_world_transform)
    {
        chld->set_world_orientation(worient);
        chld->set_world_position(wpos);
    }
}

void nstform_comp::remove_child(nstform_comp * child, bool keep_world_transform)
{
    root()->recursive_compute();

	nsentity * ent = child->owner();
	auto child_iter = m_tfi.m_children.begin();
	while (child_iter != m_tfi.m_children.end())
	{
		if (*child_iter == ent->id())
		{
			dprint("Removing child " + ent->name() + " from parent " + owner()->name());
			m_tfi.m_children.erase(child_iter);

			if (child->update_posted())
				child->recursive_compute();

			fvec3 wpos = child->world_position();
			fquat worient = child->world_orientation();
			child->m_tfi.m_parent = 0;
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
		remove_child(child(*citer), keep_world_transform);
		++citer;
	}
	m_tfi.m_children.clear();
}

void nstform_comp::cache_tform()
{
	m_prev_tform = m_world_tform;
}

fmat4 & nstform_comp::cached_tform()
{
	return m_prev_tform;
}

bool nstform_comp::has_child(nstform_comp * chld)
{
	for (uint32 i = 0; i < m_tfi.m_children.size(); ++i)
	{
		uint32 cur_id = m_tfi.m_children[i];
		nstform_comp * cur_chld = child(i);
		if (cur_id == chld->owner()->id() || cur_chld->has_child(chld))
			return true;
	}
	return false;
}

void nstform_comp::set_parent(nstform_comp * pnt, bool keep_world_transform)
{
	if (pnt == nullptr)
	{
		nstform_comp * cur_parent = parent();
		if (cur_parent != nullptr)
            cur_parent->remove_child(this, keep_world_transform);
	}
	else
        pnt->add_child(this, keep_world_transform);
}

nstform_comp * nstform_comp::parent()
{
	nsentity * par = m_owning_chunk->find_entity(m_tfi.m_parent);
	if (par != nullptr)
		return par->get<nstform_comp>();
	return nullptr;
}

nstform_comp * nstform_comp::root()
{
	nstform_comp * prnt = parent();
	if (prnt != nullptr)
		return prnt->root();
	else
		return this;
}

const nstform_comp * nstform_comp::parent() const
{
	nsentity * par = m_owning_chunk->find_entity(m_tfi.m_parent);
	if (par != nullptr)
		return par->get<nstform_comp>();
	return nullptr;
}

const nstform_comp * nstform_comp::root() const
{
	const nstform_comp * prnt = parent();
	if (prnt != nullptr)
		return prnt->root();
	else
		return this;
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
		nstform_comp * child_itf = child(i);
		child_itf->post_update(child_itf->m_update || m_render_update);
		child_itf->recursive_compute();
    }
	
	if (inst_obj == nullptr)
		m_render_update = false;
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
	const nstform_comp * par_itf = parent();
	if (par_itf != nullptr)
        return (par_itf->world_tf() * fvec4(m_tfi.m_position)).xyz();
	return m_tfi.m_position;
}

fquat nstform_comp::world_orientation() const
{
	const nstform_comp * par_itf = parent();
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
	nsentity * ent = m_owning_chunk->find_entity(m_tfi.m_children[index]);
	if (ent != nullptr)
		return ent->get<nstform_comp>();
	return nullptr;
}

uint32 nstform_comp::chunk_id()
{
	return m_owning_chunk->chunk_id();
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

void nstform_comp::set_tf_info(const tform_info & tfi_, bool preserve_world_tform)
{
	nsentity * prnt = nullptr;
	if (tfi_.m_parent != 0)
		prnt = m_owning_chunk->find_entity(tfi_.m_parent);
	
	if (prnt != nullptr)
		set_parent(prnt->get<nstform_comp>(), preserve_world_tform);

	remove_children(true);
	for (uint32 i = 0; i < tfi_.m_children.size(); ++i)
	{
		nsentity * chld = m_owning_chunk->find_entity(tfi_.m_children[i]);
		if (chld != nullptr)
			add_child(chld->get<nstform_comp>(), preserve_world_tform);
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
	nsvideo_object("tform_per_scene_info"),
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
