/*!
  \file nsvideo_driver.h

  \brief Definition file for nsvideo_driver class

  This file contains all of the neccessary definitions for the nsvideo_driver class.

  \author Daniel Randle
  \date Feb 23 2016
  \copywrite Earth Banana Games 2013
*/

#ifndef NSVIDEO_DRIVER
#define NSVIDEO_DRIVER

#include <nsrender_system.h>
#include <nsstring.h>
#include <nsmath.h>
#include <nsunordered_map.h>
#include <nsmesh.h>

class nsscene;
class nsentity;
class nsrender_shader;
class nsmaterial;
class nsrender_target;
class nsmaterial_shader;
class nslight_stencil_shader;
class nsdir_light_shader;
class nspoint_light_shader;
class nsspot_light_shader;
class nsshadow_cubemap_shader;
class nsfragment_sort_shader;
class nsshadow_2dmap_shader;
class nsbuffer_object;
class nsselection_shader;
class nsparticle_render_shader;
class nsrender_shader;
class nsrender_target;

struct render_shaders
{
	render_shaders();
	bool error();
	bool valid();
	
	nsmaterial_shader * deflt;
	nsmaterial_shader * deflt_wireframe;
	nsmaterial_shader * deflt_translucent;
	nsdir_light_shader * dir_light;
	nspoint_light_shader * point_light;
	nsspot_light_shader * spot_light;
	nslight_stencil_shader * light_stencil;
	nsshadow_cubemap_shader * shadow_cube;
	nsshadow_2dmap_shader * shadow_2d;
	nsfragment_sort_shader * frag_sort;
	nsparticle_render_shader * deflt_particle;
	nsselection_shader * sel_shader;
};

struct opengl_state
{
	opengl_state():
		depth_write(false),
		depth_test(false),
		stencil_test(false),
		blending(false),
		culling(false),
		cull_face(0),
		clear_mask(0),
		blend_func(),
		blend_eqn(0),
		stencil_func(),
		stencil_op_back(),
		stencil_op_front()
	{}
	
	bool depth_write; // material
	bool depth_test; // material
	bool stencil_test; // material
	bool blending; // renderer
	bool culling; // material
	int32 cull_face; // material
	int32 clear_mask;
	ivec2 blend_func; // renderer
	int32 blend_eqn;
	ivec3 stencil_func; // submesh
	ivec3 stencil_op_back; // submesh
	ivec3 stencil_op_front;
	ivec4 current_viewport;
};

struct draw_call
{
	draw_call():
		shdr(nullptr),
		mat(nullptr)
	{}
	
	virtual ~draw_call() {}
	virtual void render() = 0;
	
	nsrender_shader * shdr;
	nsmaterial * mat;
	uint32 mat_index;
};

typedef std::vector<draw_call*> drawcall_queue;
typedef std::unordered_map<nsstring, drawcall_queue*> queue_map;

struct render_pass
{
	render_pass():
		enabled(true),
		draw_calls(nullptr),
		ren_target(nullptr),
		use_vp_size(true),
		vp(nullptr)
	{}
	
	virtual ~render_pass() {}

	virtual void setup_pass();

	virtual void render();

	bool enabled;
	bool use_vp_size;
	drawcall_queue * draw_calls;
	nsrender_target * ren_target;
	nsrender::viewport * vp;
	opengl_state gl_state;
};

typedef std::unordered_map<nsstring, nsrender_target*> rt_map;
typedef std::vector<render_pass*> render_pass_vector;

class nsvideo_driver
{
  public:

	nsvideo_driver():
		m_default_target(nullptr),
		m_initialized(false)
	{}
	
	virtual ~nsvideo_driver() {}

	bool add_queue(const nsstring & name, drawcall_queue * rt);
	
	// system takes ownership and will delete on shutdown
	bool add_render_target(const nsstring & name, nsrender_target * rt);

	nsrender_target * render_target(const nsstring & name);

	nsrender_target * remove_render_target(const nsstring & name);

	virtual nsrender_target * default_target();

	void destroy_render_target(const nsstring & name);

	void clear_render_targets();

	void clear_render_passes();
	
	void clear_render_queues();

	virtual void create_default_render_targets() = 0;

	virtual void create_default_render_passes() = 0;

	virtual void create_default_render_queues() = 0;
	
	render_pass_vector * render_passes();
	
	drawcall_queue * create_queue(const nsstring & name);

	void destroy_queue(const nsstring & name);

	drawcall_queue * queue(const nsstring & name);

	drawcall_queue * remove_queue(const nsstring & name);

	virtual uivec3 pick(const fvec2 & mouse_pos) = 0;
	
	virtual void init();

	bool initialized();

	virtual void release();

	void set_render_shaders(render_shaders rs);

	virtual void push_scene(nsscene * scn) = 0;

	virtual void render(nsrender::viewport * vp);

  protected:
	nsrender_target * m_default_target;
	render_shaders m_shaders;
	queue_map m_render_queues;
	rt_map m_render_targets;
	render_pass_vector m_render_passes;	
	bool m_initialized;
};

#endif
