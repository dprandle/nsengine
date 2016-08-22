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

#include <component/nscomponent.h>
#include <nsvideo_driver.h>

struct nsgl_buffer;
struct nsgl_vao;
struct nsgl_xfb;
class nstimer;
class nsscene;

class nstform_comp;

struct instance_handle
{
	instance_handle(tform_per_scene_info * tfc_=nullptr, uint32 tf_ind = -1);

	bool is_valid() const;

	void invalidate();

	bool operator==(const instance_handle & rhs);
	
	tform_per_scene_info * tfc;
	uint32 ind;
};

struct accel_over_time
{
	accel_over_time(fvec3 accel_=fvec3(), float duration_=0.0f):
		accel(accel_),
        duration(duration_),
        elapsed(0.0f)
	{}
	
	fvec3 accel;
	float duration;
	float elapsed;
};

struct physic_instance
{
	physic_instance():
		velocity(),
		accels()
	{
		accels.reserve(32);
	}
	
	fvec3 velocity;
	std::vector<accel_over_time> accels;
	fbox aabb;
};

struct instance_tform
{
	template <class PUPer>
	friend void pup(PUPer & p, instance_tform & iv, const nsstring & var_name);

	friend class nsscene;
	friend class nsrender_system;
	
	instance_tform();

	instance_tform(const instance_tform & copy);

	instance_tform & operator=(instance_tform rhs);

	void recursive_compute();

	bool update;
	bool snap_to_grid;
	physic_instance phys;
	bool in_cube_grid;

    void add_child(instance_tform * child, bool keep_world_transform);
	
    void remove_child(instance_tform * child, bool keep_world_transform);

    void remove_children(bool keep_world_transform);

	uint32 current_tform_id();
	
	bool has_child(instance_tform * child);
	
    void set_parent(instance_tform * parent, bool keep_world_transform);
	
	instance_tform * parent() const;

	instance_tform * child(uint32 index);

	uint32 child_count();

    int32 hidden_state() const;

    void set_hidden_state(int32 state);

	tform_per_scene_info * owner();

    void translate(const fvec3 & amount);

    void translate_world_space(const fvec3 & amount);

    void scale(const fvec3 & amount);

    void rotate(const fquat & rotation);

	void set_world_position(const fvec3 & pos);

    void set_world_orientation(const fquat & orient_);

    void set_local_position(const fvec3 & pos);

    void set_local_orientation(const fquat & orient_);

    void set_scaling(const fvec3 & scale_);

    const fvec3 & local_position() const;

    const fquat & local_orientation() const;

	fvec3 world_position() const;

    fquat world_orientation() const;

	void set_render_update(bool val);

	bool render_update() const;

    const fvec3 & scaling() const;

	const fmat4 & world_tf() const;
	
	const fmat4 & world_inv_tf() const;
	
	const fmat4 & local_tf() const;
	
	const fmat4 & local_inv_tf() const;


  private:

	tform_per_scene_info * m_owner;
	instance_handle m_parent;
	bool m_render_update;
	int32 m_hidden_state;
	fquat m_orient;
	fvec3 m_position;
	fvec3 m_scaling;
	
	std::vector<instance_handle> m_children;
	fmat4 m_world_tform;
	fmat4 m_world_inv_tform;
	fmat4 m_local_tform;
	fmat4 m_local_inv_tform;
};

template <class PUPer>
void pup(PUPer & p, accel_over_time & iv, const nsstring & var_name)
{
	pup(p, iv.accel, var_name + ".accel");
	pup(p, iv.duration, var_name + ".duration");
	pup(p, iv.elapsed, var_name + ".elapsed");
}

template <class PUPer>
void pup(PUPer & p, physic_instance & iv, const nsstring & var_name)
{
	pup(p, iv.velocity, var_name + ".velocity");
	pup(p, iv.accels, var_name + ".accels");
	pup(p, iv.aabb, var_name + ".aabb");
}

template <class PUPer>
void pup(PUPer & p, instance_tform & iv, const nsstring & var_name)
{
	pup(p, iv.m_hidden_state, var_name + ".hidden_state");
	pup(p, iv.m_orient, var_name + ".orient");
	pup(p, iv.m_position, var_name + ".position");
	pup(p, iv.m_scaling, var_name + ".scaling");
	pup(p, iv.snap_to_grid, var_name + ".snap_to_grid");
	pup(p, iv.phys, var_name + ".phys");
}

typedef std::vector<instance_tform> instance_vec;

struct tform_per_scene_info : public nsvideo_object
{
	tform_per_scene_info();
	~tform_per_scene_info();
	
	void video_context_init();
	instance_vec m_tforms;
	bool m_buffer_resized;
	uint32 m_visible_count;
	nstform_comp * owner;

	nsscene * scene;

// New per scene info stuff - for migrating to new syste
	std::vector<nstform_comp*> shared_geom_tforms;
	std::vector<uint32> needs_update_ids;
	uint32 visible_count;
	bool needs_update;
};

struct shared_inst_obj
{
	nsvid_obj * vobj;
};

class nstform_comp : public nscomponent
{
  public:
	enum h_state
	{
		hide_layer = 0x01,
		hide_object = 0x02,
		hide_none = 0x04,
		hide_all = 0x08
	};

	friend class nsscene;
	friend class nsrender_system;
	
	nstform_comp();

	nstform_comp(const nstform_comp & copy);

	~nstform_comp();

	void init();

	virtual void pup(nsfile_pupper * p);

	void release();

	tform_per_scene_info * per_scene_info(const nsscene * scn);

	nstform_comp & operator=(nstform_comp rhs_);

	bool save_with_scene;

	tform_per_scene_info * inst_obj; // shared among all instances

	uint32 inst_id;
	
  private:
	
	std::unordered_map<const nsscene *, tform_per_scene_info *> m_scenes_info;
};

#endif
