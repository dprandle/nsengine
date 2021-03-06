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
class nsmap_area;
class nstform_ent_chunk;

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

class nstform_comp;

struct tform_per_scene_info : public nsvideo_object
{
	tform_per_scene_info();
	~tform_per_scene_info();

	void video_context_init();

	std::vector<nstform_comp*> shared_geom_tforms;
	uint32 visible_count;
	bool needs_update;
};

struct tform_info
{
	tform_info(
		uint32 parent=0,
		const fvec3 & pos=fvec3(),
		const fquat & ornt=fquat(),
		const fvec3 & scale=fvec3(1.0f),
		int32 hide_state=0,
		const ui_vector & children_=ui_vector()
		);

	nsstring to_string() const;
	
	int32 m_hidden_state;
	fquat m_orient;
	fvec3 m_position;
	fvec3 m_scaling;
	
	uint32 m_parent;
	ui_vector m_children;
};

struct packed_ent_tform
{
	packed_ent_tform(const uivec2 & ent_id_ = uivec2(),
				const tform_info & tf_inf=tform_info()):
		ent_id(ent_id_),
		tf_info(tf_inf)
	{}
	
	uivec2 ent_id;
	tform_info tf_info;
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

	template <class PUPer>
	friend void pup(PUPer & p, nstform_comp & iv, const nsstring & var_name);
	
	friend class nsmap_area;
	friend class nsrender_system;
	friend class nstform_ent_chunk;
	
	nstform_comp();

	nstform_comp(const nstform_comp & copy);

	~nstform_comp();

	nstform_comp & operator=(nstform_comp rhs_);

	void init();

	virtual void pup(nsfile_pupper * p);

	void release();
	
	void recursive_compute();
	
    void add_child(nstform_comp * child, bool keep_world_transform);
	
    void remove_child(nstform_comp * child, bool keep_world_transform);

    void remove_children(bool keep_world_transform);
	
	bool has_child(nstform_comp * child);
	
    void set_parent(nstform_comp * parent, bool keep_world_transform);
	
	nstform_comp * parent();

	nstform_comp * root();

	const nstform_comp * parent() const;

	const nstform_comp * root() const;

	nstform_comp * child(uint32 index);

	uint32 chunk_id();

	uint32 child_count();

    int32 hidden_state() const;

    void set_hidden_state(int32 state);

    void translate(const fvec3 & amount);

    void translate_world_space(const fvec3 & amount);

    void scale(const fvec3 & amount);

    void rotate(const fquat & rotation);

	const tform_info & tf_info() const;

	void set_tf_info(const tform_info & tfi_, bool preserve_world_tform);

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
	
	tform_per_scene_info * inst_obj; // shared among all instances

	void cache_tform();

	bool copyable();

	fmat4 & cached_tform();

  private:
	bool m_render_update;
	tform_info m_tfi;		
	fmat4 m_world_tform;
	fmat4 m_world_inv_tform;
	fmat4 m_local_tform;
	fmat4 m_local_inv_tform;

	fmat4 m_prev_tform;
	nstform_ent_chunk * m_owning_chunk;
};

template <class PUPer>
void pup(PUPer & p, tform_info & iv, const nsstring & var_name)
{
	pup(p, iv.m_parent, var_name + ".parent");
	pup(p, iv.m_children, var_name + ".children");
	pup(p, iv.m_hidden_state, var_name + ".hidden_state");
	pup(p, iv.m_orient, var_name + ".orient");
	pup(p, iv.m_position, var_name + ".position");
	pup(p, iv.m_scaling, var_name + ".scaling");
}

template <class PUPer>
void pup(PUPer & p, nstform_comp & iv, const nsstring & var_name)
{
	pup(p, iv.m_tfi, "");
}

#endif
