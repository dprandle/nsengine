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

#include "nscomponent.h"
#include "nsvideo_driver.h"

class nsgl_buffer;
class nsgl_vao;
class nsgl_xfb;
class nstimer;
class nsscene;

class nstform_comp;

struct instance_handle
{
	instance_handle(nstform_comp * tfc_=nullptr, uint32 tf_ind = -1):
		tfc(tfc_),
		ind(tf_ind)
	{}

	bool is_valid() const
	{
		return tfc != nullptr && ind != -1;
	}

	void invalidate()
	{
		tfc = nullptr; ind = -1;
	}

	bool operator==(const instance_handle & rhs)
	{
		return tfc == rhs.tfc && ind == rhs.ind;
	}
	
	nstform_comp * tfc;
	uint32 ind;
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

    void add_child(instance_tform * child, bool keep_world_transform);
	
    void remove_child(instance_tform * child, bool keep_world_transform);

    void remove_children(bool keep_world_transform);

	uint32 current_tform_id();
	
	bool has_child(instance_tform * child);
	
    void set_parent(instance_tform * parent, bool keep_world_transform);
	
	instance_tform * parent();

	instance_tform * child(uint32 index);

	uint32 child_count();

    int32 hidden_state() const;

    void set_hidden_state(int32 state);

	nstform_comp * owner();

	nsscene * scene();

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

	nstform_comp * m_owner;
	instance_handle m_parent;
	nsscene * m_scene;
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
void pup(PUPer & p, instance_tform & iv, const nsstring & var_name)
{
	pup(p, iv.m_hidden_state, var_name + ".hidden_state");
	pup(p, iv.m_orient, var_name + ".orient");
	pup(p, iv.m_position, var_name + ".position");
	pup(p, iv.m_scaling, var_name + ".scaling");
	pup(p, iv.snap_to_grid, var_name + ".snap_to_grid");
}

typedef std::vector<instance_tform> instance_vec;

struct tform_per_scene_info : public nsvideo_object
{
	tform_per_scene_info(nstform_comp * owner_, nsscene * scn);
	~tform_per_scene_info();
	
	void video_context_init();
	nstform_comp * owner;
	nsscene * scene;
	instance_vec m_tforms;
	bool m_buffer_resized;
	uint32 m_visible_count;
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

	instance_tform * instance_transform(const nsscene * scn, uint32 tform_id);

	uint32 instance_count(const nsscene * scn) const;

	tform_per_scene_info * per_scene_info(nsscene * scn);

	nstform_comp & operator=(nstform_comp rhs_);

	bool save_with_scene;
	
  private:
	
	std::unordered_map<const nsscene *, tform_per_scene_info *> m_scenes_info;
};

#endif
