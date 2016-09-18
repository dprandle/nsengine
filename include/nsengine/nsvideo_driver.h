/*!
  \file nsvideo_driver.h

  \brief Definition file for nsvideo_driver class

  This file contains all of the neccessary definitions for the nsvideo_driver class.

  \author Daniel Randle
  \date Feb 23 2016
  \copywrite Earth Banana Games 2013
*/

#ifndef NSVIDEO_DRIVER_H
#define NSVIDEO_DRIVER_H

#include <nsstring.h>
#include <math/nsmath.h>
#include <nsunordered_map.h>
#include <nsset.h>
#include <list>
#include <nsrouter.h>

class nsparticle_comp;
struct tform_per_scene_info;
struct sel_per_scene_info;
class nstexture;
//class nsshader;
class nsentity;
class nsscene;

#define SHADER_VID_OBJ_GUID "nsshader_vid_obj"
#define TEXTURE_VID_OBJ_GUID "nstexture_vid_obj"
#define MESH_VID_OBJ_GUID "nssubmesh_vid_obj"
#define TFORM_VID_OBJ_GUID "nstform_per_scene_info_vid_obj"
#define SEL_VID_OBJ_GUID "nssel_per_scene_info_vid_obj"
#define PARTICLE_VID_OBJ_GUID "nsparticle_comp_vid_obj"
#define MAX_CONTEXT_COUNT 16

// Light bounds, skydome, and tile meshes
#define MESH_FULL_TILE "fulltile"
#define MESH_HALF_TILE "halftile"
#define MESH_DIRLIGHT_BOUNDS "dirlightbounds"
#define MESH_SPOTLIGHT_BOUNDS "spotlightbounds"
#define MESH_POINTLIGHT_BOUNDS "pointlightbounds"
#define MESH_TERRAIN "terrain"
#define MESH_SKYDOME "skydome"

struct viewport
{
	viewport( const fvec4 & norm_bounds = fvec4(),
			  nsentity * cam=nullptr,
			  uint32 window_tag_=0,
			  const ivec4 & bounds_=ivec4(),
			  const fvec4 & fog_color=fvec4(1,1,1,1),
			  const uivec2 & fog_near_far=uivec2(200,300),
			  bool lighting=true,
			  bool shadows=true,
			  bool oit=true,
			  bool selection=true,
			  bool debug_draw=false):
		normalized_bounds(norm_bounds),
		camera(cam),
		window_tag(window_tag_),
		bounds(bounds_),
		m_fog_nf(fog_near_far),
		m_fog_color(fog_color),
		spot_lights(lighting),
		spot_light_shadows(shadows),
		point_lights(lighting),
		point_light_shadows(shadows),
		dir_lights(true),
		dir_light_shadows(shadows),
		order_independent_transparency(oit),
		picking_enabled(selection)
	{}

	fvec4 normalized_bounds;
	uint32 window_tag;
	ivec4 bounds;
	bool spot_lights;
	bool spot_light_shadows;
	bool point_lights;
	bool point_light_shadows;
	bool dir_lights;
	bool dir_light_shadows;
	bool order_independent_transparency;
	bool picking_enabled;
	bool debug_draw;
	uivec2 m_fog_nf;
	fvec4 m_fog_color;
	nsentity * camera;
	std::vector<nsentity*> ui_canvases;
};

struct vp_node
{
	vp_node(const nsstring & vp_name_, viewport * vp_);
	~vp_node();
	nsstring vp_name;
	viewport * vp;
};

struct draw_call
{
	virtual ~draw_call() {}
};

typedef std::vector<draw_call*> render_queue;
typedef std::unordered_map<nsstring, render_queue*> queue_map;

struct render_pass
{
	render_pass():
		rq(nullptr),
		vp(nullptr),
		enabled(true)
	{}
	
	virtual ~render_pass() {};
	
	virtual void setup() = 0;
	virtual void render() = 0;
	virtual void finish() = 0;

	render_queue * rq;
	viewport * vp;
	bool enabled;
};

struct nsvid_obj;

typedef std::vector<render_pass*> render_pass_vector;
typedef std::vector<nsvid_obj*> vid_obj_vector;

struct vid_ctxt
{
	vid_ctxt(uint32 cntxt_id);
	virtual ~vid_ctxt();

	viewport * insert_viewport(
		const nsstring & vp_name,
		const viewport & vp,
		const nsstring & insert_before="");

	bool remove_viewport(const nsstring & vp_name);

	viewport * find_viewport(const nsstring & vp_name);

	void move_viewport_back(const nsstring & vp_name);

	void move_viewport_forward(const nsstring & vp_name);

	void move_viewport_to_back(const nsstring & vp_name);

	void move_viewport_to_front(const nsstring & vp_name);

	viewport * front_viewport_at_screen_pos(const fvec2 & screen_pos);

	void destroy_all_render_queues();

	bool add_queue(const nsstring & name, render_queue * rt);
	
	render_queue * create_queue(const nsstring & name);

	void destroy_queue(const nsstring & name);

	render_queue * queue(const nsstring & name);

	render_queue * remove_queue(const nsstring & name);

	void destroy_all_render_passes();

	void cleanup_vid_objs();

	virtual void clear_render_queues();

	virtual void update_vid_objs();	

	virtual void window_resized(const ivec2 & new_size);

	virtual void init() = 0;

	virtual void release();

	virtual uivec3 pick(const fvec2 & mouse_pos) = 0;

	virtual void push_scene(nsscene * scn) = 0;

	virtual void push_entity(nsentity * ent) = 0;

	virtual void push_viewport_ui(viewport * vp) = 0;

	virtual void render_to_viewport(viewport * vp) = 0;

	virtual void render_to_all_viewports() = 0;

	virtual void render(nsscene * scn) = 0;

	virtual void setup_default_rendering() = 0;

	virtual const ivec2 & window_size() = 0;

	bool auto_cleanup;
	bool auto_update_vobjs;
	uint32 context_id;
	bool initialized;
	queue_map render_queues;
	render_pass_vector render_passes;
	vid_obj_vector need_release;
	viewport * focused_vp;
	std::list<vp_node> vp_list;
	std::set<nsvid_obj*> registered_vid_objs;

	uint32 top_mat_id;

	std::vector<tform_per_scene_info*> instance_objs;
};

class nsvideo_driver
{
  public:

	nsvideo_driver();
	
	virtual ~nsvideo_driver();

	virtual void init();

	virtual void release();

	virtual uint8 create_context() = 0;

	virtual bool destroy_context(uint8 c_id);

	virtual bool make_context_current(uint8 c_id);

	virtual vid_ctxt * current_context();

	virtual vid_ctxt * context(uint8 id);
	
	bool initialized();
	
	ns::signal<vid_ctxt*> context_switch;

  protected:
	
	vid_ctxt * m_current_context;
	vid_ctxt * m_contexts[MAX_CONTEXT_COUNT];
	bool m_initialized;
};

class nsvideo_object;

struct nsvid_obj
{
	nsvid_obj(nsvideo_object * parent_);
	
	virtual ~nsvid_obj();

	virtual void update() = 0;

	bool needs_update;

	nsvideo_object * parent;
};

class nsvideo_object
{
	SLOT_OBJECT
  public:
	nsvideo_object();

	virtual ~nsvideo_object();

	bool context_sharing();

	void enable_context_sharing(bool enable);
	
	virtual bool initialized(vid_ctxt * ctxt);

	virtual void video_context_init() = 0;
	
	virtual void video_context_release();
	
	virtual void video_update();

	virtual void on_context_switch(vid_ctxt * current_ctxt);
	
	template<class obj_type>
	obj_type * video_obj()
	{
		return static_cast<obj_type*>(video_obj());
	}

	nsvid_obj * video_obj();

  protected:
	bool share_between_contexts;
	nsvid_obj * ctxt_objs[16];
};


#endif
