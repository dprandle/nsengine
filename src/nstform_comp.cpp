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
	m_tform_buffer(new nsbuffer_object(nsbuffer_object::array, nsbuffer_object::storage_mutable)),
	m_tform_id_buffer(new nsbuffer_object(nsbuffer_object::array, nsbuffer_object::storage_mutable)),
	m_xfb_data(),
	m_buffer_resized(false),
	m_xfb(false),
	nscomponent(),
	m_visible_count(0)
{}

nstform_comp::~nstform_comp()
{
	release();
}

uint32 nstform_comp::add()
{
	return add(instance_tform());
}

uint32 nstform_comp::add(const instance_tform & trans)
{
	m_tforms.push_back(trans);
	post_update(true);
	m_buffer_resized = true;

	if (m_xfb)
	{
		enable_xfb(false);
		enable_xfb(true);
	}

	return static_cast<uint32>(m_tforms.size()) - 1;
}

uint32 nstform_comp::add(uint32 cnt_)
{
	// Get the transformID of the last transform before adding cnt_ and return it
	uint32 ret = static_cast<uint32>(m_tforms.size()) - 1;
	m_tforms.resize(m_tforms.size() + cnt_);
	post_update(true);
	m_buffer_resized = true;

	if (m_xfb)
	{
		enable_xfb(false);
		enable_xfb(true);
	}

	return ret;
}

bool nstform_comp::buffer_resized() const
{
	return m_buffer_resized;
}

void nstform_comp::change_scale(const fvec3 & amount_, uint32 tform_id_)
{
	if (tform_id_ >= m_tforms.size())
	{
		dprint("nstform_comp::changeScale - Invalid TransformID: " + std::to_string(tform_id_));
		return;
	}
	m_tforms[tform_id_].scaling += amount_;
	m_tforms[tform_id_].update = true;
	post_update(true);
}

void nstform_comp::change_scale(float x_, float y_, float z_, uint32 tform_id_)
{
	change_scale(fvec3(x_, y_, z_), tform_id_);
}

void nstform_comp::pup(nsfile_pupper * p)
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

void nstform_comp::compute_transform(uint32 tform_id_)
{
	if (tform_id_ >= m_tforms.size())
	{
		dprint("nstform_comp::computeTransform - Invalid TransformID: " + std::to_string(tform_id_));
		return;
	}
	m_tforms[tform_id_].compute();
}

nstform_comp* nstform_comp::copy(const nscomponent * pToCopy)
{
	if (pToCopy == NULL)
		return NULL;
	const nstform_comp * comp = (const nstform_comp*)pToCopy;
	(*this) = (*comp);
	return this;
}

void nstform_comp::enable_parent(bool enable_, uint32 tform_id_)
{
	if (tform_id_ >= m_tforms.size())
	{
		dprint("nstform_comp::enableParent - Invalid TransformID: " + std::to_string(tform_id_));
		return;
	}

	m_tforms[tform_id_].parent_enabled = enable_;
	m_tforms[tform_id_].update = true;
	post_update(true);
}

/*!
Enable transform feedback for this entity. Pass in pVertCount to set the buffer size necessary to hold
*/
bool nstform_comp::enable_xfb(bool enable_)
{
	m_xfb = enable_;

	if (m_xfb)
	{
		// Figure out how much mem to allocate
		nsrender_comp * mRenComp = m_owner->get<nsrender_comp>();
		if (mRenComp == NULL || m_tforms.empty())
			return false;

		nsmesh * mesh = nse.resource<nsmesh>(mRenComp->mesh_id());
		if (mesh == NULL)
			return false;

		// Figure out how much memory to allocate in the buffers.. this should be number of tranforms times number of verts
		uint32 allocAmount = mesh->vert_count() * static_cast<uint32>(m_tforms.size());
		uint32 instancePerDraw = static_cast<uint32>(m_tforms.size());
		uint32 totalIntanceCount = static_cast<uint32>(m_tforms.size());

		// If the total is larger than 4mB, then find out how many 4 mB buffers are needed
		uint32 bufCount = 1;
		if (allocAmount > MAX_TF_BUFFER_SIZE)
		{
			bufCount = allocAmount / MAX_TF_BUFFER_SIZE + 1;
			allocAmount = MAX_TF_BUFFER_SIZE;
			instancePerDraw = MAX_TF_BUFFER_SIZE / mesh->vert_count();
		}

		if (bufCount > MAX_TF_BUFFER_COUNT)
			return false;



		m_xfb_data.xfb_buffers.resize(bufCount);
		for (uint32 bufI = 0; bufI < bufCount; ++bufI)
		{
			xfb_buffer * buf = &m_xfb_data.xfb_buffers[bufI];
			buf->alloc_amnt = allocAmount;

			buf->xfb_obj = new nsxfb_object();
			buf->xfb_vao = new nsvertex_array_object();
			buf->xfb_pos_buf = new nsbuffer_object(nsbuffer_object::array, nsbuffer_object::storage_mutable);
			buf->xfb_texcoord_buf = new nsbuffer_object(nsbuffer_object::array, nsbuffer_object::storage_mutable);
			buf->xfb_normal_buf = new nsbuffer_object(nsbuffer_object::array, nsbuffer_object::storage_mutable);
			buf->xfb_tangent_buf = new nsbuffer_object(nsbuffer_object::array, nsbuffer_object::storage_mutable);

			buf->xfb_vao->init_gl();
			buf->xfb_obj->init_gl();
			buf->xfb_pos_buf->init_gl();
			buf->xfb_texcoord_buf->init_gl();
			buf->xfb_normal_buf->init_gl();
			buf->xfb_tangent_buf->init_gl();

			// Set up VAO for draw on other stuff
			buf->xfb_vao->bind();

			buf->xfb_pos_buf->bind();
			buf->xfb_pos_buf->allocate<fvec3>(nsbuffer_object::mutable_dynamic_copy, buf->alloc_amnt);
			buf->xfb_vao->add(buf->xfb_pos_buf, nsshader::loc_position);
			buf->xfb_vao->vertex_attrib_ptr(nsshader::loc_position, 4, GL_FLOAT, GL_FALSE, sizeof(fvec3), 0);

			buf->xfb_texcoord_buf->bind();
			buf->xfb_texcoord_buf->allocate<fvec3>(nsbuffer_object::mutable_dynamic_copy, buf->alloc_amnt);
			buf->xfb_vao->add(buf->xfb_texcoord_buf, nsshader::loc_tex_coords);
			buf->xfb_vao->vertex_attrib_ptr(nsshader::loc_tex_coords, 3, GL_FLOAT, GL_FALSE, sizeof(fvec3), 0);

			buf->xfb_normal_buf->bind();
			buf->xfb_normal_buf->allocate<fvec3>(nsbuffer_object::mutable_dynamic_copy, buf->alloc_amnt);
			buf->xfb_vao->add(buf->xfb_normal_buf, nsshader::loc_normal);
			buf->xfb_vao->vertex_attrib_ptr(nsshader::loc_normal, 3, GL_FLOAT, GL_FALSE, sizeof(fvec3), 0);

			buf->xfb_tangent_buf->bind();
			buf->xfb_tangent_buf->allocate<fvec3>(nsbuffer_object::mutable_dynamic_copy, buf->alloc_amnt);
			buf->xfb_vao->add(buf->xfb_tangent_buf, nsshader::loc_tangent);
			buf->xfb_vao->vertex_attrib_ptr(nsshader::loc_tangent, 3, GL_FLOAT, GL_FALSE, sizeof(fvec3), 0);

			buf->xfb_vao->unbind();

			buf->xfb_pos_buf->set_target(nsbuffer_object::transform_feedback);
			buf->xfb_texcoord_buf->set_target(nsbuffer_object::transform_feedback);
			buf->xfb_normal_buf->set_target(nsbuffer_object::transform_feedback);
			buf->xfb_tangent_buf->set_target(nsbuffer_object::transform_feedback);
			buf->xfb_obj->bind();
			buf->xfb_pos_buf->bind(nsshader::loc_position);
			buf->xfb_texcoord_buf->bind(nsshader::loc_tex_coords);
			buf->xfb_normal_buf->bind(nsshader::loc_normal);
			buf->xfb_tangent_buf->bind(nsshader::loc_tangent);
			buf->xfb_obj->unbind();

			if (instancePerDraw <= totalIntanceCount)
			{
				buf->instance_count = instancePerDraw;
				totalIntanceCount -= instancePerDraw;
			}
			else
			{
				buf->instance_count = totalIntanceCount;
			}
		}
		m_xfb_data.update = true;
	}
	else
	{
		for (uint32 bufI = 0; bufI < m_xfb_data.xfb_buffers.size(); ++bufI)
		{
			xfb_buffer * buf = &m_xfb_data.xfb_buffers[bufI];

			buf->xfb_obj->release();
			delete buf->xfb_obj;
			buf->xfb_obj = NULL;

			buf->xfb_vao->release();
			delete buf->xfb_vao;
			buf->xfb_vao = NULL;

			buf->xfb_pos_buf->release();
			delete buf->xfb_pos_buf;
			buf->xfb_pos_buf = NULL;

			buf->xfb_texcoord_buf->release();
			delete buf->xfb_texcoord_buf;
			buf->xfb_texcoord_buf = NULL;

			buf->xfb_normal_buf->release();
			delete buf->xfb_normal_buf;
			buf->xfb_normal_buf = NULL;

			buf->xfb_tangent_buf->release();
			delete buf->xfb_tangent_buf;
			buf->xfb_tangent_buf = NULL;
		}
		m_xfb_data.xfb_buffers.clear();
		m_xfb_data.update = false;
	}
	return true;
}

void nstform_comp::init()
{
	m_tform_buffer->init_gl();
	m_tform_id_buffer->init_gl();
}

const fvec3 nstform_comp::dvec(dir_vec dir_, uint32 tform_id_) const
{
	switch (dir_)
	{
	case (dir_right):
		return m_tforms[tform_id_].orient.right();
	case (dir_up) :
		return m_tforms[tform_id_].orient.up();
	case (dir_target) :
		return m_tforms[tform_id_].orient.target();
	}
	return fvec3();
}

nstform_comp::instance_tform & nstform_comp::inst_transorm(uint32 tform_id_)
{
	return m_tforms[tform_id_];
}

const nstform_comp::instance_vec & nstform_comp::transform_array() const
{
	return m_tforms;
}

const fquat & nstform_comp::orientation(uint32 tform_id_) const
{
	return m_tforms[tform_id_].orient;
}

const fvec3 & nstform_comp::lpos(uint32 tform_id_) const
{
	return m_tforms[tform_id_].posistion;
}

const fmat4 & nstform_comp::pov(uint32 tform_id_) const
{
	return m_tforms[tform_id_].inv_tform;
}

uint32 nstform_comp::render_id(uint32 tform_id_) const
{
	return m_tforms[tform_id_].render_id;
}

const fvec3 & nstform_comp::scaling(uint32 tform_id_) const
{
	return m_tforms[tform_id_].scaling;
}

const uivec3 & nstform_comp::parent_id(uint32 tform_id_) const
{
	return m_tforms[tform_id_].parent_id;
}

nstform_comp::xfb_data * nstform_comp::xfb()
{
	return &m_xfb_data;
}

const fmat4 & nstform_comp::parent(uint32 tform_id_) const
{
	return m_tforms[tform_id_].parent_tform;
}

const fmat4 & nstform_comp::transform(uint32 tform_id_) const
{
	return m_tforms[tform_id_].tform;
}

nsbuffer_object * nstform_comp::transform_buffer()
{
	return m_tform_buffer;
}

nsbuffer_object * nstform_comp::transform_id_buffer()
{
	return m_tform_id_buffer;
}

uint32 nstform_comp::count() const
{
	return static_cast<uint32>(m_tforms.size());
}

uint32 nstform_comp::visible_count() const
{
	return m_visible_count;
}

fvec3 nstform_comp::wpos(uint32 tform_id_)
{
	if (m_tforms[tform_id_].parent_enabled)
		return (m_tforms[tform_id_].parent_tform * fvec4(m_tforms[tform_id_].posistion, 1.0f)).xyz();

	return m_tforms[tform_id_].posistion;
}

bool nstform_comp::snapped(uint32 tform_id_) const
{
	return m_tforms[tform_id_].snap_to_grid;
}

int32 nstform_comp::hidden_state(uint32 tform_id_) const
{
	return m_tforms[tform_id_].hidden_state;
}

bool nstform_comp::parent_enabled(uint32 tform_id_) const
{
	return m_tforms[tform_id_].parent_enabled;
}

bool nstform_comp::xfb_enabled()
{
	return m_xfb;
}

bool nstform_comp::transform_update(uint32 tform_id_) const
{
	return m_tforms[tform_id_].update;
}

void nstform_comp::release()
{
	m_tform_buffer->release();
	m_tform_id_buffer->release();
	if (m_xfb)
		enable_xfb(false);
}

uint32 nstform_comp::remove(uint32 tform_id_)
{
	if (tform_id_ >= m_tforms.size())
	{
		dprint("nstform_comp::removeTransform - Invalid TransformID or Invalid Operation; Transform ID: " + std::to_string(tform_id_));
		return count();
	}
	else if (m_tforms.size() == 1)
	{
		m_tforms.pop_back();
		return 0;
	}

	m_tforms[tform_id_] = m_tforms.back();
	m_tforms[tform_id_].update = true;
	m_tforms.pop_back();
	post_update(true);
	m_buffer_resized = true;

	if (m_xfb)
	{
		enable_xfb(false);
		enable_xfb(true);
	}

	return count();
}

uint32 nstform_comp::remove(uint32 first_, uint32 last_)
{
	if (first_ >= m_tforms.size() || last_ >= m_tforms.size() || first_ >= last_ || (last_ - first_ + 1) == m_tforms.size())
	{
		dprint("nstform_comp::removeTransforms - Invalid TransformID or Invalid operation");
		return count();
	}
	uint32 resizeNum = count() - (last_ - first_ + 1);

	// Move all the elements in the back of the last that arent part of the erase
	// range to the first index that the erase begins.. increasing the index and
	// popping an item off the back every iteration..
	for (uint32 i = first_; i <= last_; ++i)
	{
		if (last_ < m_tforms.size() - 1)
		{
			m_tforms[i] = m_tforms.back();
			m_tforms[i].update = true;
			m_tforms.pop_back();
		}
		else
			break;
	}

	m_tforms.resize(resizeNum);
	m_buffer_resized = true;
	post_update(true);

	if (m_xfb)
	{
		enable_xfb(false);
		enable_xfb(true);
	}

	return count();
}

void nstform_comp::rotate(const fvec4 & axis_angle_, uint32 tform_id_)
{
	if (tform_id_ >= m_tforms.size())
	{
		dprint("nstform_comp::rotate - Invalid TransformID: " + std::to_string(tform_id_));
		return;
	}

	m_tforms[tform_id_].orient = ::orientation(axis_angle_) * m_tforms[tform_id_].orient;
	m_tforms[tform_id_].update = true;
	post_update(true);
}

void nstform_comp::rotate(const fvec4 & axis_angle_)
{
	for (uint32 i = 0; i < m_tforms.size(); ++i)
		rotate(axis_angle_, i);
}

void nstform_comp::rotate(world_axis axis_, float angle_, uint32 tform_id_)
{
	fvec4 axAng;
	switch (axis_)
	{
	case(axis_x) :
		axAng.set(1, 0, 0, angle_);
		break;
	case(axis_y) :
		axAng.set(0, 1, 0, angle_);
		break;
	case(axis_z) :
		axAng.set(0, 0, 1, angle_);
		break;
	}
	rotate(axAng, tform_id_);
}

void nstform_comp::rotate(world_axis axis_, float angle_)
{
	for (uint32 i = 0; i < m_tforms.size(); ++i)
		rotate(axis_, angle_, i);
}

void nstform_comp::rotate(const fquat & quat_, uint32 tform_id_)
{
	if (tform_id_ >= m_tforms.size())
	{
		dprint("nstform_comp::rotate - Invalid TransformID: " + std::to_string(tform_id_));
		return;
	}

	m_tforms[tform_id_].orient = quat_ * m_tforms[tform_id_].orient;
	m_tforms[tform_id_].update = true;
	post_update(true);
}

void nstform_comp::rotate(const fquat & quat_)
{
	for (uint32 i = 0; i < m_tforms.size(); ++i)
		rotate(quat_, i);
}

void nstform_comp::rotate(const fvec3 & euler_)
{
	for (uint32 i = 0; i < m_tforms.size(); ++i)
		rotate(euler_, i);
}

void nstform_comp::rotate(const fvec3 & euler_, uint32 tform_id_)
{
	rotate(::orientation(euler_, fvec3::XYZ), tform_id_);
}

void nstform_comp::rotate(dir_vec dir_, float angle_, uint32 tform_id_)
{
	rotate(fvec4(dvec(dir_), angle_), tform_id_);
}

void nstform_comp::rotate(dir_vec dir_, float angle_)
{
	for (uint32 i = 0; i < m_tforms.size(); ++i)
		rotate(dir_, angle_, i);
}

void nstform_comp::scale(const fvec3 & amount_, uint32 tform_id_)
{
	if (tform_id_ >= m_tforms.size())
	{
		dprint("nstform_comp::scale - Invalid TransformID: " + std::to_string(tform_id_));
		return;
	}

	m_tforms[tform_id_].scaling %= amount_;
	m_tforms[tform_id_].update = true;
	post_update(true);
}

void nstform_comp::scale(float x_, float y_, float z_, uint32 tform_id_)
{
	scale(fvec3(x_, y_, z_), tform_id_);
}

void nstform_comp::scale(const fvec3 & amount_)
{
	for (uint32 i = 0; i < m_tforms.size(); ++i)
		scale(amount_, i);
}

void nstform_comp::scale(float x_, float y_, float z_)
{
	for (uint32 i = 0; i < m_tforms.size(); ++i)
		scale(x_, y_, z_, i);
}

void nstform_comp::set_buffer_resize(bool resize_)
{
	m_buffer_resized = resize_;
}

void nstform_comp::enable_snap(bool snap_, uint32 tform_id_)
{
	if (tform_id_ >= m_tforms.size())
	{
		dprint("nstform_comp::setGridSnap - Invalid TransformID: " + std::to_string(tform_id_));
		return;
	}

	m_tforms[tform_id_].snap_to_grid = snap_;
	m_tforms[tform_id_].update = true;
	post_update(true);
}

void nstform_comp::enable_snap(bool snap_)
{
	for (uint32 i = 0; i < m_tforms.size(); ++i)
		enable_snap(snap_, i);
}

void nstform_comp::set_hidden_state(h_state state_, bool enable_, uint32 tform_id_)
{
	if (tform_id_ >= m_tforms.size())
	{
		dprint("nstform_comp::setHidden - Invalid TransformID: " + std::to_string(tform_id_));
		return;
	}

	if (enable_)
		m_tforms[tform_id_].hidden_state |= state_;
	else
		m_tforms[tform_id_].hidden_state &= ~state_;
	
	m_tforms[tform_id_].update = true;
	post_update(true);
}

void nstform_comp::set_hidden_state(h_state state_, bool enable_)
{
	for (uint32 i = 0; i < m_tforms.size(); ++i)
		set_hidden_state(state_, enable_, i);
}

void nstform_comp::set_instance_tform(const instance_tform & instt_)
{
	for (uint32 i = 0; i < m_tforms.size(); ++i)
		set_instance_tform(instt_, i);
}

void nstform_comp::set_instance_tform(const instance_tform & instt_, uint32 tform_id_)
{
	if (tform_id_ >= m_tforms.size())
	{
		dprint("nstform_comp::setInstTrans - Invalid TransformID: " + std::to_string(tform_id_));
		return;
	}

	m_tforms[tform_id_] = instt_;
	m_tforms[tform_id_].update = true;
	post_update(true);
}

void nstform_comp::set_orientation(const fquat & ornt_)
{
	for (uint32 i = 0; i < m_tforms.size(); ++i)
		set_orientation(ornt_, i);
}

void nstform_comp::set_orientation(const fquat & ornt_, uint32 tform_id_)
{
	if (tform_id_ >= m_tforms.size())
	{
		dprint("nstform_comp::setOrientation - Invalid TransformID: " + std::to_string(tform_id_));
		return;
	}

	m_tforms[tform_id_].orient = ornt_;
	m_tforms[tform_id_].update = true;
	post_update(true);
}

void nstform_comp::set_parent_id(const uivec3 & parent_id_)
{
	for (uint32 i = 0; i < m_tforms.size(); ++i)
		set_parent_id(parent_id_, i);
}

void nstform_comp::set_parent_id(const uivec3 & parent_id_, uint32 tform_id_)
{
	if (tform_id_ >= m_tforms.size())
	{
		dprint("nstform_comp::setParentID - Invalid TransformID: " + std::to_string(tform_id_));
		return;
	}

	m_tforms[tform_id_].parent_id = parent_id_;
	m_tforms[tform_id_].update = true;
	post_update(true);
}

void nstform_comp::set_parent(const fmat4 & tform_)
{
	for (uint32 i = 0; i < m_tforms.size(); ++i)
		set_parent(tform_, i);
}

void nstform_comp::set_parent(const fmat4 & tform_, uint32 tform_id_)
{
	if (tform_id_ >= m_tforms.size())
	{
		dprint("nstform_comp::setParentTransform - Invalid TransformID: " + std::to_string(tform_id_));
		return;
	}

	m_tforms[tform_id_].parent_tform = tform_;
	m_tforms[tform_id_].update = true;
	post_update(true);
}

void nstform_comp::set_pos(const fvec3 & pos_, uint32 tform_id_)
{
	if (tform_id_ >= m_tforms.size())
	{
		dprint("nstform_comp::setPosition - Invalid TransformID: " + std::to_string(tform_id_));
		return;
	}

	m_tforms[tform_id_].posistion = pos_;
	m_tforms[tform_id_].update = true;
	post_update(true);
}

void nstform_comp::set_pos(const fvec3 & pos_)
{
	for (uint32 i = 0; i < m_tforms.size(); ++i)
		set_pos(pos_, i);
}

void nstform_comp::set_render_id(uint32 render_id_)
{
	for (uint32 i = 0; i < m_tforms.size(); ++i)
		set_render_id(render_id_, i);
}

void nstform_comp::set_render_id(uint32 render_id_, uint32 tform_id_)
{
	if (tform_id_ >= m_tforms.size())
	{
		dprint("nstform_comp::setPosition - Invalid TransformID: " + std::to_string(tform_id_));
		return;
	}

	m_tforms[tform_id_].render_id = render_id_;
}

void nstform_comp::set_scale(const fvec3 & scaling_)
{
	for (uint32 i = 0; i < m_tforms.size(); ++i)
		set_scale(scaling_, i);
}

void nstform_comp::set_scale(const fvec3 & scaling_, uint32 tform_id_)
{
	if (tform_id_ >= m_tforms.size())
	{
		dprint("nstform_comp::setScaling - Invalid TransformID: " + std::to_string(tform_id_));
		return;
	}

	m_tforms[tform_id_].scaling = scaling_;
	m_tforms[tform_id_].update = true;
	post_update(true);
}

void nstform_comp::set_instance_update(bool update_)
{
	for (uint32 i = 0; i < m_tforms.size(); ++i)
		set_instance_update(update_, i);
}

void nstform_comp::set_instance_update(bool update_, uint32 tform_id_)
{
	if (tform_id_ >= m_tforms.size())
	{
		dprint("nstform_comp::setTransUpdate - Invalid TransformID: " + std::to_string(tform_id_));
		return;
	}

	m_tforms[tform_id_].update = update_;
}

void nstform_comp::post_update(bool update_)
{
	if (update_)
		m_xfb_data.update = update_;

	nscomponent::post_update(update_);
}

void nstform_comp::set_visible_instance_count(uint32 count_)
{
	if (count_ > m_tforms.size())
	{
		dprint("nstform_comp::setVisibleTransformCount - visible count larger than transform count");
		return;
	}

	m_visible_count = count_;
}

void nstform_comp::snap(uint32 tform_id_)
{
	if (tform_id_ >= m_tforms.size())
	{
		dprint("nstform_comp::snap - Invalid TransformID: " + std::to_string(tform_id_));
		return;
	}
	if (m_tforms[tform_id_].snap_to_grid)
	{
		nstile_grid::snap(m_tforms[tform_id_].posistion);
		m_tforms[tform_id_].update = true;
		post_update(true);
	}
}

void nstform_comp::snap_x(uint32 tform_id_)
{
	if (tform_id_ >= m_tforms.size())
	{
		dprint("nstform_comp::snap - Invalid TransformID: " + std::to_string(tform_id_));
		return;
	}
	if (m_tforms[tform_id_].snap_to_grid)
	{
		fvec3 pos = m_tforms[tform_id_].posistion;
		nstile_grid::snap(pos);
		m_tforms[tform_id_].posistion.x = pos.x;
		m_tforms[tform_id_].update = true;
		post_update(true);
	}
}

void nstform_comp::snap_y(uint32 tform_id_)
{
	if (tform_id_ >= m_tforms.size())
	{
		dprint("nstform_comp::snap - Invalid TransformID: " + std::to_string(tform_id_));
		return;
	}
	if (m_tforms[tform_id_].snap_to_grid)
	{
		fvec3 pos = m_tforms[tform_id_].posistion;
		nstile_grid::snap(pos);
		m_tforms[tform_id_].posistion.y = pos.y;
		m_tforms[tform_id_].update = true;
		post_update(true);
	}
}

void nstform_comp::snap_z(uint32 tform_id_)
{
	if (tform_id_ >= m_tforms.size())
	{
		dprint("nstform_comp::snap - Invalid TransformID: " + std::to_string(tform_id_));
		return;
	}
	if (m_tforms[tform_id_].snap_to_grid)
	{
		fvec3 pos = m_tforms[tform_id_].posistion;
		nstile_grid::snap(pos);
		m_tforms[tform_id_].posistion.z = pos.z;
		m_tforms[tform_id_].update = true;
		post_update(true);
	}
}

void nstform_comp::snap()
{
	for (uint32 i = 0; i < m_tforms.size(); ++i)
		snap(i);
}

void nstform_comp::snap_x()
{
	for (uint32 i = 0; i < m_tforms.size(); ++i)
		snap_x(i);
}

void nstform_comp::snap_y()
{
	for (uint32 i = 0; i < m_tforms.size(); ++i)
		snap_y(i);
}

void nstform_comp::snap_z()
{
	for (uint32 i = 0; i < m_tforms.size(); ++i)
		snap_z(i);
}

void nstform_comp::toggle_snap(uint32 tform_id_)
{
	if (tform_id_ >= m_tforms.size())
	{
		dprint("nstform_comp::toggleGridSnap - Invalid TransformID: " + std::to_string(tform_id_));
		return;
	}

	m_tforms[tform_id_].snap_to_grid = !m_tforms[tform_id_].snap_to_grid;
	m_tforms[tform_id_].update = true;
	post_update(true);
}

void nstform_comp::toggle_snap()
{
	for (uint32 i = 0; i < m_tforms.size(); ++i)
		toggle_snap(i);
}

void nstform_comp::toggle_hidden_state(h_state state_, uint32 tform_id_)
{
	if (tform_id_ >= m_tforms.size())
	{
		dprint("nstform_comp::toggleHidden - Invalid TransformID: " + std::to_string(tform_id_));
		return;
	}

	m_tforms[tform_id_].hidden_state ^= state_;
	m_tforms[tform_id_].update = true;
	post_update(true);
}

void nstform_comp::toggle_hidden_state(h_state state_)
{
	auto iter = m_tforms.begin();
	while (iter != m_tforms.end())
	{
		iter->hidden_state ^= state_;
		++iter;
	}
	post_update(true);
}

void nstform_comp::translate(const fvec3 & amount_)
{
	for (uint32 i = 0; i < m_tforms.size(); ++i)
		translate(amount_, i);
}

void nstform_comp::translate(float x_, float y_, float z_)
{
	for (uint32 i = 0; i < m_tforms.size(); ++i)
		translate(fvec3(x_,y_,z_), i);
}

void nstform_comp::translate(const fvec3 & amount_, uint32 tform_id_)
{
	if (tform_id_ >= m_tforms.size())
	{
		dprint("nstform_comp::translate - Invalid TransformID: " + std::to_string(tform_id_));
		return;
	}

	m_tforms[tform_id_].posistion += amount_;
	m_tforms[tform_id_].update = true;
	post_update(true);
}

void nstform_comp::translate(float x_, float y_, float z_, uint32 tform_id_)
{
	translate(fvec3(x_, y_, z_), tform_id_);
}

void nstform_comp::translate(dir_vec dir_, float amount_)
{
	for (uint32 i = 0; i < m_tforms.size(); ++i)
		translate(dir_, amount_, i);
}

void nstform_comp::translate(world_axis dir_, float amount_, uint32 tform_id_)
{
	if (tform_id_ >= m_tforms.size())
	{
		dprint("nstform_comp::rotate - Invalid TransformID: " + std::to_string(tform_id_));
		return;
	}
	fvec3 trans;
	switch (dir_)
	{
	case(axis_x) :
		trans.set(amount_, 0, 0);
		break;
	case(axis_y) :
		trans.set(0, amount_, 0);
		break;
	case(axis_z) :
		trans.set(0, 0, amount_);
		break;
	}
	m_tforms[tform_id_].posistion += trans;
	m_tforms[tform_id_].update = true;
	post_update(true);
}

void nstform_comp::translate(world_axis dir_, float amount_)
{
	for (uint32 i = 0; i < m_tforms.size(); ++i)
		translate(dir_, amount_, i);
}

void nstform_comp::translate(dir_vec dir_, float amount_, uint32 tform_id_)
{
	translate(dvec(dir_)*amount_, tform_id_);
}

void nstform_comp::translate_x(float amount_)
{
	for (uint32 i = 0; i < m_tforms.size(); ++i)
		translate_x(amount_, i);
}

void nstform_comp::translate_y(float amount_)
{
	for (uint32 i = 0; i < m_tforms.size(); ++i)
		translate_y(amount_, i);
}

void nstform_comp::translate_z(float amount_)
{
	for (uint32 i = 0; i < m_tforms.size(); ++i)
		translate_z(amount_, i);
}

void nstform_comp::translate_x(float amount_, uint32 tform_id_)
{
	translate(amount_, 0.0f, 0.0f, tform_id_);
}

void nstform_comp::translate_y(float amount_, uint32 tform_id_)
{
	translate(0.0f, amount_, 0.0f, tform_id_);
}

void nstform_comp::translate_z(float amount_, uint32 tform_id_)
{
	translate(0.0f, 0.0f, amount_, tform_id_);
}

nstform_comp & nstform_comp::operator=(const nstform_comp & rhs_)
{
	m_tforms.resize(rhs_.m_tforms.size());
	for (uint32 i = 0; i < m_tforms.size(); ++i)
		m_tforms[i] = rhs_.m_tforms[i];
	post_update(true);
	return (*this);
}

void nstform_comp::instance_tform::compute()
{
	tform.set(rotation_mat3(orient) % scaling);
	tform.set_column(3, posistion.x, posistion.y, posistion.z, 1);

	fvec4 col3(-posistion.x, -posistion.y, -posistion.z, 1.0f);
	inv_tform.rotation_from(orient).transpose();
	inv_tform.set_column(3, inv_tform[0] * col3, inv_tform[1] * col3, inv_tform[2] * col3, 1.0f);

	if (parent_enabled)
	{
		tform = parent_tform * tform;
		inv_tform = inverse(tform);
	}
}