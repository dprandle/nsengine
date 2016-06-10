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

class nsgl_buffer;
class nsgl_vao;
class nsgl_xfb;
class nstimer;
class nsscene;

class nstform_comp;

struct instance_tform
{
	friend class nsscene;
	friend class nsrender_system;
	
	instance_tform() :
		m_hidden_state(0),
		m_orient(),
		m_position(),
		m_scaling(1.0f, 1.0f, 1.0f),
		update(true),
		snap_to_grid(true),
		m_owner(nullptr),
		m_scene(nullptr),
		m_parent(nullptr),
		m_children(),
		m_world_tform(),
		m_world_inv_tform(),
		m_local_tform(),
		m_local_inv_tform()
	{}

	instance_tform(const instance_tform & copy):
		m_hidden_state(0),
		m_orient(copy.m_orient),
		m_position(copy.m_position),
		m_scaling(copy.m_scaling),
		update(true),
		snap_to_grid(copy.snap_to_grid),
		m_owner(nullptr),
		m_scene(nullptr),
		m_render_update(true),
		m_parent(nullptr),
		m_children(),
		m_world_tform(),
		m_world_inv_tform(),
		m_local_tform(),
		m_local_inv_tform()
	{}

	instance_tform & operator=(instance_tform rhs);

	void recursive_compute();

	bool update;
	bool snap_to_grid;

    void add_child(instance_tform * child, bool keep_world_transform);
	
    void remove_child(instance_tform * child, bool keep_world_transform);

    void remove_children(bool keep_world_transform);
	
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

    const fvec3 & scaling() const;

	const fmat4 & world_tf() const;
	
	const fmat4 & world_inv_tf() const;
	
	const fmat4 & local_tf() const;
	
	const fmat4 & local_inv_tf() const;

  private:

	nstform_comp * m_owner;
	instance_tform * m_parent;
	nsscene * m_scene;
	bool m_render_update;
	int32 m_hidden_state;
	fquat m_orient;
	fvec3 m_position;
	fvec3 m_scaling;

	
	std::vector<instance_tform*> m_children;
	fmat4 m_world_tform;
	fmat4 m_world_inv_tform;
	fmat4 m_local_tform;
	fmat4 m_local_inv_tform;
};

typedef std::vector<instance_tform> instance_vec;

struct tform_per_scene_info
{
	tform_per_scene_info();
	~tform_per_scene_info();
		
	instance_vec m_tforms;
	nsgl_buffer * m_tform_buffer;
	nsgl_buffer * m_tform_id_buffer;
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

	nsgl_buffer * transform_buffer(const nsscene * scn);

	nsgl_buffer * transform_id_buffer(const nsscene * scn);

	void init();

	virtual void pup(nsfile_pupper * p);

	void release();

	instance_tform * instance_transform(const nsscene * scn, uint32 tform_id);

	uint32 instance_count(const nsscene * scn) const;

	nstform_comp & operator=(nstform_comp rhs_);

	bool save_with_scene;
	
  private:
	
	std::unordered_map<const nsscene *, tform_per_scene_info *> m_scenes_info;
};


template <class PUPer>
void pup(PUPer & p, instance_tform & iv)
{
	// pup(p, iv.hidden_state, varName + ".hidden_state");
	// pup(p, iv.orient, varName + ".orient");
	// pup(p, iv.posistion, varName + ".position");
	// pup(p, iv.scaling, varName + ".scaling");
	// pup(p, iv.world_tform, varName + ".parent_tform");
	// pup(p, iv.local_inv_tform, varName + ".inv_tform");
	// pup(p, iv.local_tform, varName + ".tform");
	// pup(p, iv.render_id, varName + ".render_id");
	// pup(p, iv.snap_to_grid, varName + ".snap_to_grid");
	// pup(p, iv.parent_enabled, varName + ".parent_enabled");
	// pup(p, iv.parent_id, varName + ".parent_id");
}
#endif
