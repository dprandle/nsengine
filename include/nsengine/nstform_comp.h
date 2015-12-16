/*! 
	\file nstform_comp.h
	
	\brief Header file for nstform_comp class

	This file contains all of the neccessary declarations for the nstform_comp class.

	\author Daniel Randle
	\date November 23 2013
	\copywrite Earth Banana Games 2013
*/

#ifndef NSTFORM_COMP_H
#define NSTFORM_COMP_H

#define MAX_TF_BUFFER_SIZE 10048576
#define MAX_TF_BUFFER_COUNT 32 // about 100 mb

#include <nscomponent.h>

class nsbuffer_object;
class nsvertex_array_object;
class nsxfb_object;
class nstimer;

class nstform_comp : public nscomponent
{
public:
	enum world_axis {
		axis_x,
		axis_y,
		axis_z
	};

	enum dir_vec {
		dir_right,
		dir_up,
		dir_target
	};

	enum h_state
	{
		hide_layer = 0x01,
		hide_object = 0x02,
		hide_none = 0x04,
		hide_all = 0x08
	};

	struct instance_tform
	{
		instance_tform() :
		hidden_state(0),
		orient(),
		posistion(),
		scaling(1.0f, 1.0f, 1.0f),
		parent_tform(),
		inv_tform(),
		tform(),
		update(true),
		render_id(0),
		snap_to_grid(true),
		parent_enabled(false),
		parent_id()
		{}

		void compute();

		int32 hidden_state;
		fquat orient;
		fvec3 posistion;
		fvec3 scaling;
		fmat4 parent_tform;
		fmat4 inv_tform;
		fmat4 tform;
		bool update;
		uint32 render_id;
		bool snap_to_grid;
		bool parent_enabled;
		uivec3 parent_id;
	};

	typedef std::vector<instance_tform> instance_vec;

	template <class PUPer>
	friend void pup(PUPer & p, nstform_comp & tc);

	nstform_comp();

	nstform_comp(const nstform_comp & copy);

	~nstform_comp();

	uint32 add();

	uint32 add(const instance_tform & trans);

	uint32 add(uint32 pHowMany);

	bool buffer_resized() const;

	void change_scale(const fvec3 & amount_, uint32 tform_id_=0);

	void change_scale(float x_, float y_, float z_, uint32 tform_id_ = 0);

	void compute_transform(uint32 tform_id_ = 0);

	void enable_parent(bool enable_, uint32 tform_id_ = 0);

	const fvec3 dvec(dir_vec dir_, uint32 tform_id_ = 0) const;

	nsbuffer_object * transform_buffer();

	nsbuffer_object * transform_id_buffer();

	const instance_vec & transform_array() const;

	const fquat & orientation(uint32 tform_id_ = 0) const;

	const uivec3 & parent_id(uint32 tform_id_ = 0) const;

	const fmat4 & parent(uint32 tform_id_ = 0) const;

	const fvec3 & lpos(uint32 tform_id_ = 0) const;

	const fmat4 & pov(uint32 tform_id_ = 0) const;

	uint32 render_id(uint32 tform_id_) const;

	const fvec3 & scaling(uint32 tform_id_ = 0) const;

	const fmat4 & transform(uint32 tform_id_ = 0) const;

	int32 hidden_state(uint32 tform_id_ = 0) const;

	instance_tform & inst_transorm(uint32 tform_id_ = 0);

	uint32 count() const;

	uint32 visible_count() const;

	fvec3 wpos(uint32 tform_id_ = 0);

	void init();

	bool snapped(uint32 tform_id_ = 0) const;

	bool parent_enabled(uint32 tform_id_ = 0) const;

	bool transform_update(uint32 tform_id_ = 0) const;

	virtual void pup(nsfile_pupper * p);

	void release();

	uint32 remove(uint32 tform_id_);

	uint32 remove(uint32 first_, uint32 last_);

	void rotate(world_axis axis_, float angle_, uint32 tform_id_);

	void rotate(world_axis axis_, float angle_);

	void rotate(const fvec4 & axis_angle_, uint32 tform_id_);

	void rotate(const fvec4 & axis_angle_);

	void rotate(const fvec3 & euler_);

	void rotate(const fvec3 & euler_, uint32 tform_id_);

	void rotate(const fquat & quat_, uint32 tform_id_);

	void rotate(const fquat & quat_);

	void rotate(dir_vec dir_, float angle_, uint32 tform_id_);

	void rotate(dir_vec dir_, float angle_);

	void scale(const fvec3 & amount_, uint32 tform_id_);

	void scale(const fvec3 & amount_);

	void scale(float x_, float y_, float z_, uint32 tform_id_);

	void scale(float x_, float y_, float z_);

	void set_buffer_resize(bool resize_);

	void enable_snap(bool snap_, uint32 tform_id_);

	void enable_snap(bool snap_);

	void set_hidden_state(h_state state_, bool enable_, uint32 tform_id_);

	void set_hidden_state(h_state state_, bool enable_);

	void set_instance_tform(const instance_tform & inst_tform_, uint32 tform_id_);

	void set_instance_tform(const instance_tform & inst_tform_);

	void set_orientation(const fquat & ornt_, uint32 tform_id_);

	void set_orientation(const fquat & ornt_);

	void set_parent(const fmat4 & tform_, uint32 tform_id_);

	void set_parent(const fmat4 & tform_);

	void set_parent_id(const uivec3 & parent_id_, uint32 tform_id_);

	void set_parent_id(const uivec3 & parent_id_);

	void set_pos(const fvec3 & pos_, uint32 tform_id_);

	void set_pos(const fvec3 & pos_);

	void set_render_id(uint32 render_id_, uint32 tform_id_);

	void set_render_id(uint32 render_id_);

	void set_scale(const fvec3 & scaling_, uint32 tform_id_);

	void set_scale(const fvec3 & scaling_);

	void set_instance_update(bool update_, uint32 tform_id_);

	void set_instance_update(bool update_);

	void post_update(bool update_);

	void set_visible_instance_count(uint32 count_);

	void snap(uint32 tform_id_);

	void snap();

	void snap_x(uint32 tform_id_);

	void snap_x();

	void snap_y(uint32 tform_id_);

	void snap_y();

	void snap_z(uint32 tform_id_);

	void snap_z();

	void toggle_snap(uint32 tform_id_);

	void toggle_snap();

	void toggle_hidden_state(h_state state_, uint32 tform_id_);

	void toggle_hidden_state(h_state state_);

	void translate(const fvec3 & amount_, uint32 tform_id_);

	void translate(const fvec3 & amount_);

	void translate(float x_, float y_, float z_, uint32 tform_id_);

	void translate(float x_, float y_, float z_);

	void translate(dir_vec dir_, float amount_, uint32 tform_id_);

	void translate(dir_vec dir_, float amount_);

	void translate(world_axis dir_, float amount_, uint32 tform_id_);

	void translate(world_axis dir_, float amount_);

	void translate_x(float amount_, uint32 tform_id_);

	void translate_x(float amount_);

	void translate_y(float amount_, uint32 tform_id_);

	void translate_y(float amount_);

	void translate_z(float amount_, uint32 tform_id_ );

	void translate_z(float amount_);

	nstform_comp & operator=(nstform_comp rhs_);

private:
	instance_vec m_tforms;
	nsbuffer_object * m_tform_buffer;
	nsbuffer_object * m_tform_id_buffer;
	bool m_buffer_resized;
	uint32 m_visible_count;
};

template <class PUPer>
void pup(PUPer & p, nstform_comp & tc)
{
	pup(p, tc.m_tforms, "tforms");
	tc.post_update(true);
	tc.m_buffer_resized = true;
}

template <class PUPer>
void pup(PUPer & p, nstform_comp::instance_tform & iv, const nsstring & varName)
{
	pup(p, iv.hidden_state, varName + ".hidden_state");
	pup(p, iv.orient, varName + ".orient");
	pup(p, iv.posistion, varName + ".position");
	pup(p, iv.scaling, varName + ".scaling");
	pup(p, iv.parent_tform, varName + ".parent_tform");
	pup(p, iv.inv_tform, varName + ".inv_tform");
	pup(p, iv.tform, varName + ".tform");
	pup(p, iv.render_id, varName + ".render_id");
	pup(p, iv.snap_to_grid, varName + ".snap_to_grid");
	pup(p, iv.parent_enabled, varName + ".parent_enabled");
	pup(p, iv.parent_id, varName + ".parent_id");
}
#endif
