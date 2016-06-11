/*!
  \file nsgl_driver.h

  \brief Definition file for nsopengl_driver class

  This file contains all of the neccessary definitions for the nsopengl_driver class.

  \author Daniel Randle
  \date Feb 23 2016
  \copywrite Earth Banana Games 2013
*/

#ifndef NSGL_DRIVER_H
#define NSGL_DRIVER_H

// Default render targets
#define GBUFFER_TARGET "gbuffer_target"
#define ACCUM_TARGET "accum_target"
#define DIR_SHADOW2D_TARGET "dir_shadow2d_target"
#define SPOT_SHADOW2D_TARGET "spot_shadow2d_target"
#define POINT_SHADOW_TARGET "point_shadow_target"

// Default drawcall queues
#define SCENE_OPAQUE_QUEUE "scene_opaque_queue"
#define SCENE_SELECTION_QUEUE "scene_selection_queue"
#define SCENE_TRANSLUCENT_QUEUE "scene_translucent_queue"
#define DIR_LIGHT_QUEUE "dir_light_queue"
#define SPOT_LIGHT_QUEUE "spot_light_queue"
#define POINT_LIGHT_QUEUE "point_light_queue"
#define UI_RENDER_QUEUE "ui_render_queue"

// Default checkered material
#define DEFAULT_MATERIAL "default"

// Framebuffer sizes
#define DEFAULT_SPOT_LIGHT_SHADOW_W 1024
#define DEFAULT_SPOT_LIGHT_SHADOW_H 1024
#define DEFAULT_POINT_LIGHT_SHADOW_W 1024
#define DEFAULT_POINT_LIGHT_SHADOW_H 1024
#define DEFAULT_DIR_LIGHT_SHADOW_W 2048
#define DEFAULT_DIR_LIGHT_SHADOW_H 2048
#define DEFAULT_GBUFFER_RES_X 1920
#define DEFAULT_GBUFFER_RES_Y 1080
#define DEFAULT_ACCUM_BUFFER_RES_X 1920
#define DEFAULT_ACCUM_BUFFER_RES_Y 1080

#include <nsvideo_driver.h>
#include <nsmesh.h>
#include <nsrender_system.h>

class nsmaterial;
class nsgl_framebuffer;
struct window_resize_event;
class nsscene;

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

class nsgl_driver;

struct gl_render_pass : public render_pass
{
	gl_render_pass():
		render_pass(),
		ren_target(nullptr),
		use_vp_size(true)
	{}

	virtual ~gl_render_pass() {}

	virtual void setup();

	virtual void render() = 0;

	virtual void finish() {}

	bool use_vp_size;
	nsgl_framebuffer * ren_target;
	nsgl_driver * driver;
	opengl_state gl_state;
};

typedef std::unordered_map<nsstring, nsgl_framebuffer*> rt_map;

struct packed_fragment_data // 32 bytes total
{
	fvec3 wpos;
	uint32 color;
	uint32 normal;
	int32 next;
	float depth;
	uint32 mat_id;
	uivec4 ids;
};

struct translucent_buffers
{
	translucent_buffers();
	~translucent_buffers();

	void bind_buffers();
	void unbind_buffers();
	void reset_atomic_counter();
	
	nsgl_buffer * atomic_counter;
	nsgl_buffer * header;
	nsgl_buffer * fragments;
	ui_vector header_clr_data;
};

struct gbuffer_render_pass : public gl_render_pass
{
	virtual void render();
};

struct oit_render_pass : public gl_render_pass
{
	virtual void render();
	translucent_buffers * tbuffers;
};

struct light_shadow_pass : public gl_render_pass
{
	virtual void render();
	light_draw_call * ldc;
};

struct light_pass : public gl_render_pass
{
	virtual void render();
	light_shadow_pass * rpass;
	translucent_buffers * tbuffers;
};

struct culled_light_pass : public light_pass
{
	virtual void render();
};

struct final_render_pass : public gl_render_pass
{
	virtual void render();
	nsgl_framebuffer * read_buffer;
};

struct selection_render_pass : public gl_render_pass
{
	virtual void render();
};

struct ui_render_pass : public gl_render_pass
{
	virtual void render();
};

GLEWContext * glewGetContext();

class nstexture;
class nstex1d;
class nstex2d;
class nstex3d;
class nstex_cubemap;

struct gl_ctxt : public vid_ctxt
{
	gl_ctxt(uint32 id);
	~gl_ctxt();
	
	void init();
	void release();
	
	GLEWContext * glew_context; // created in ctor
	translucent_buffers * m_tbuffers; // created in init
	nsgl_framebuffer * m_default_target; // created in init
	nsgl_buffer * m_single_point;
	
	rt_map m_render_targets; // created and removed by driver
	opengl_state m_gl_state;
};

class nsgl_driver : public nsvideo_driver
{
  public:
	enum render_pass_t
	{
		geometry,
		oit,
		dir_shadow,
		dir_light,
		spot_shadow,
		spot_light,
		point_shadow,
		point_light,
		selection,
		final
	};
		
	nsgl_driver();
	~nsgl_driver();
	
	uint32 active_tex_unit();
	
	bool add_render_target(const nsstring & name, nsgl_framebuffer * rt);

	nsgl_framebuffer * render_target(const nsstring & name);

	nsgl_framebuffer * remove_render_target(const nsstring & name);

	nsgl_framebuffer * default_target();

	void destroy_render_target(const nsstring & name);

	void clear_render_targets();

	void setup_default_rendering();

	void create_default_render_targets();

	void create_default_render_queues();

	void create_default_render_passes();

	uint8 create_context();

	gl_ctxt * current_context();

	gl_ctxt * context(uint8 id);

	opengl_state current_gl_state();

	uivec3 pick(const fvec2 & mouse_pos);

	virtual void init(bool setup_default_rend=true);

	virtual void release();

	virtual void resize_screen(const ivec2 & new_size);

	virtual void render(nsrender::viewport * vp);

	void set_active_texture_unit(uint32 tex_unit);
	
	uint32 bound_fbo();

	void clear_framebuffer(uint32 clear_mask);
	
	void enable_depth_write(bool enable);

	void enable_depth_test(bool enable);

	void enable_stencil_test(bool enable);

	void enable_blending(bool enable);

	void enable_culling(bool enable);

	void set_cull_face(int32 cull_face);

	void set_blend_func(int32 sfactor, int32 dfactor);
	
	void set_blend_func(const ivec2 & blend_func);

	void set_blend_eqn(int32 eqn);

	void set_stencil_func(int32 func, int32 ref, int32 mask);
	
	void set_stencil_func(const ivec3 & stencil_func);

	void set_stencil_op(int32 sfail, int32 dpfail, int32 dppass);
	
	void set_stencil_op(const ivec3 & stencil_op);

	void set_stencil_op_back(int32 sfail, int32 dpfail, int32 dppass);
	
	void set_stencil_op_back(const ivec3 & stencil_op);

	void set_stencil_op_front(int32 sfail, int32 dpfail, int32 dppass);
	
	void set_stencil_op_front(const ivec3 & stencil_op);

	void set_gl_state(const opengl_state & state);

	void set_viewport(const ivec4 & val);

	void bind_textures(nsmaterial * material);

	void bind_gbuffer_textures(nsgl_framebuffer * fb);
	
	void render_instanced_dc(instanced_draw_call * idc);

	void render_light_dc(light_draw_call * idc);

	void render_ui_dc(ui_draw_call * idc);
	
  private:

	bool _handle_window_resize(window_resize_event * evt);	
	bool _valid_check();	
};

#endif
