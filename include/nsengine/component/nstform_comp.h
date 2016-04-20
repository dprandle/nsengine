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
class nsscene;

class nstform_comp;

struct instance_tform
{
	friend class nsscene;
	
	instance_tform() :
		hidden_state(0),
		orient(),
		position(),
		scaling(1.0f, 1.0f, 1.0f),
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
		hidden_state(0),
		orient(copy.orient),
		position(copy.position),
		scaling(copy.scaling),
		update(true),
		snap_to_grid(copy.snap_to_grid),
		m_owner(nullptr),
		m_scene(nullptr),
		m_parent(nullptr),
		m_children(),
		m_world_tform(),
		m_world_inv_tform(),
		m_local_tform(),
		m_local_inv_tform()
	{}

	instance_tform & operator=(instance_tform rhs);

	void recursive_compute();

	int32 hidden_state;
	fquat orient;
	fvec3 position;
	fvec3 scaling;
	bool update;
	bool snap_to_grid;

	void add_child(instance_tform * child);
	
	void remove_child(instance_tform * child);

	void remove_children();
	
	bool has_child(instance_tform * child);
	
	void set_parent(instance_tform * parent);
	
	instance_tform * parent();

	instance_tform * child(uint32 index);

	uint32 child_count();

	nstform_comp * owner();

	nsscene * scene();

	void set_world_position(const fvec3 & pos);

	fvec3 world_position() const;

	const fmat4 & world_tf() const;
	
	const fmat4 & world_inv_tf() const;
	
	const fmat4 & local_tf() const;
	
	const fmat4 & local_inv_tf() const;

  private:

	nstform_comp * m_owner;
	instance_tform * m_parent;
	nsscene * m_scene;
	
	std::vector<instance_tform*> m_children;
	fmat4 m_world_tform;
	fmat4 m_world_inv_tform;
	fmat4 m_local_tform;
	fmat4 m_local_inv_tform;
};

typedef std::vector<instance_tform> instance_vec;

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

	nsbuffer_object * transform_buffer(const nsscene * scn);

	nsbuffer_object * transform_id_buffer(const nsscene * scn);

	void init();

	virtual void pup(nsfile_pupper * p);

	void release();

	instance_tform * instance_transform(const nsscene * scn, uint32 tform_id);

	uint32 instance_count(const nsscene * scn) const;

	nstform_comp & operator=(nstform_comp rhs_);

  private:

	struct per_scene_info
	{
		per_scene_info();
		~per_scene_info();
		
		instance_vec m_tforms;
		nsbuffer_object * m_tform_buffer;
		nsbuffer_object * m_tform_id_buffer;
		bool m_buffer_resized;
		uint32 m_visible_count;
	};

	std::unordered_map<const nsscene *, per_scene_info *> m_scenes_info;
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
