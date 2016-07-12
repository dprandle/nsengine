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

#define OPENGL

// Default shaders
#define GBUFFER_SHADER "gbufferdefault"
#define GBUFFER_WF_SHADER "gbufferdefault_wireframe"
#define GBUFFER_TRANS_SHADER "gbufferdefault_translucent"
#define RENDER_PARTICLE_SHADER "renderparticle"
#define PARTICLE_PROCESS_SHADER "xfbparticle"
#define LIGHTSTENCIL_SHADER "lightstencil"
#define SPOT_LIGHT_SHADER "spotlight"
#define DIR_LIGHT_SHADER "directionlight"
#define FRAGMENT_SORT_SHADER "fragment_sort"
#define POINT_LIGHT_SHADER "pointlight"
#define POINT_SHADOWMAP_SHADER "pointshadowmap"
#define SPOT_SHADOWMAP_SHADER "spotshadowmap"
#define DIR_SHADOWMAP_SHADER "dirshadowmap"
#define SELECTION_SHADER "selectionsolid"
#define SKYBOX_SHADER "skybox"
#define UI_SHADER "render_ui"
#define UI_TEXT_SHADER "ui_render_text"

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

// Some default fog settings
#define DEFAULT_FOG_FACTOR_NEAR 40
#define DEFAULT_FOG_FACTOR_FAR 80

// Max draw_calls
#define MAX_INSTANCED_DRAW_CALLS 4096
#define MAX_LIGHTS_IN_SCENE 4096
#define MAX_GBUFFER_DRAWS 2048
#define MAX_UI_DRAW_CALLS 1024

#include <nsplatform.h>


#include <nsvideo_driver.h>
#include <myGL/glew.h>
#include <nsgl_draw_calls.h>
#include <nsgl_render_passes.h>

class nsmaterial;
class nsscene;

struct nsgl_framebuffer;
struct nsgl_buffer;
struct nsgl_shader;

typedef std::unordered_map<nsstring, nsgl_framebuffer*> rt_map;
typedef std::map<nsmaterial*, uint32> mat_id_map;

struct render_shaders
{
	render_shaders();
	bool error() const;
	bool valid() const;
	
	nsshader * deflt;
	nsshader * deflt_wireframe;
	nsshader * deflt_translucent;
	nsshader * dir_light;
	nsshader * point_light;
	nsshader * spot_light;
	nsshader * light_stencil;
	nsshader * shadow_cube;
	nsshader * shadow_2d;
	nsshader * frag_sort;
	nsshader * deflt_particle;
	nsshader * sel_shader;
};

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

#ifdef ORDER_INDEPENDENT_TRANSLUCENCY
struct translucent_buffers
{
	translucent_buffers();
	~translucent_buffers();

	void init();
	void release();
	

	void bind_buffers();
	void unbind_buffers();
	void reset_atomic_counter();
	
	nsgl_buffer * atomic_counter;
	nsgl_buffer * header;
	nsgl_buffer * fragments;
	ui_vector header_clr_data;
};
#endif

GLEWContext * glewGetContext();

class nsgl_driver;

struct gl_ctxt : public vid_ctxt
{
	gl_ctxt(uint32 id);
	~gl_ctxt();

	void init();

	void release();

	bool add_render_target(const nsstring & name, nsgl_framebuffer * rt);

	nsgl_framebuffer * render_target(const nsstring & name);

	nsgl_framebuffer * remove_render_target(const nsstring & name);

	void destroy_render_target(const nsstring & name);

	void destroy_all_render_targets();

	void setup_default_rendering();

	void clear_render_queues();

	void create_default_render_targets();

	void create_default_render_queues();

	void create_default_render_passes();

	void window_resized(const ivec2 & new_size);

	const ivec2 & window_size();

	uivec3 pick(const fvec2 & mouse_pos);

	void push_scene(nsscene * scn);

	void push_entity(nsentity * ent);

	void push_viewport_ui(viewport * vp);
	
	void render_to_viewport(viewport * vp);

	void render_to_all_viewports();

	void render(nsscene * scn);
	
	uint32 active_tex_unit();

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

	void set_gl_viewport(const ivec4 & val);

	void bind_textures(nsmaterial * material);

	void bind_gbuffer_textures(nsgl_framebuffer * fb);
	
    void render_instanced_dc(instanced_draw_call * idc, nsgl_shader * bound_shader);

    void render_light_dc(light_draw_call * idc, nsgl_shader * bound_shader);

    void render_ui_dc(ui_draw_call * idc, nsgl_shader * bound_shader);

	void _add_draw_calls_from_scene(nsscene * scene);

	void _add_lights_from_scene(nsscene * scene);

	GLEWContext * glew_context; // created in ctor

#ifdef ORDER_INDEPENDENT_TRANSLUCENCY
	translucent_buffers * m_tbuffers; // created in init
#endif

	nsgl_framebuffer * m_default_target; // created in init
	nsgl_buffer * m_single_point;

	rt_map render_targets; // created and removed by driver
	opengl_state m_gl_state;
	mat_id_map mat_shader_ids;

	nsgl_driver * driver;
	
	std::vector<instanced_draw_call> all_instanced_draw_calls;
	std::vector<light_draw_call> all_light_draw_calls;
	std::vector<ui_draw_call> all_ui_draw_calls;
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
	
	uint8 create_context();

	gl_ctxt * current_context();

	gl_ctxt * context(uint8 id);

	opengl_state current_gl_state();

	nsmaterial * default_mat();

	void set_default_mat(nsmaterial * pDefMaterial);

	void init();

	void release();

	bool shaders_are_valid();

	render_shaders rshaders;
	
  private:
	
	nsmaterial * m_default_mat;
};

int32 get_gl_prim_type(mesh_primitive_type pt);

#endif
