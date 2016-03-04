/*!
  \file nsopengl_driver.h

  \brief Definition file for nsopengl_driver class

  This file contains all of the neccessary definitions for the nsopengl_driver class.

  \author Daniel Randle
  \date Feb 23 2016
  \copywrite Earth Banana Games 2013
*/

#ifndef NSOPENGL_DRIVER
#define NSOPENGL_DRIVER

// Default drawcall queues
#define SCENE_OPAQUE_QUEUE "scene_opaque_queue"
#define SCENE_SELECTION_QUEUE "scene_selection_queue"
#define SCENE_TRANSLUCENT_QUEUE "scene_translucent_queue"
#define DIR_LIGHT_QUEUE "dir_light_queue"
#define SPOT_LIGHT_QUEUE "spot_light_queue"
#define POINT_LIGHT_QUEUE "point_light_queue"

// Default render targets
#define GBUFFER_TARGET "gbuffer_target"
#define ACCUM_TARGET "accum_target"
#define DIR_SHADOW2D_TARGET "dir_shadow2d_target"
#define SPOT_SHADOW2D_TARGET "spot_shadow2d_target"
#define POINT_SHADOW_TARGET "point_shadow_target"

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

class nsmaterial;
class nsfb_object;
struct window_resize_event;

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
	
	nsbuffer_object * atomic_counter;
	nsbuffer_object * header;
	nsbuffer_object * fragments;
	ui_vector header_clr_data;
};

struct single_draw_call : public draw_call
{
	single_draw_call(): draw_call() {}
	~single_draw_call() {}

	void render() {}

	nsmesh::submesh * submesh;
	fmat4 transform;
	fmat4_vector * anim_transforms;
//	fmat4 proj_cam;
	fvec2 height_minmax;
	uint32 entity_id;
	uint32 plugin_id;
	uint32 transform_count;
	bool casts_shadows;
	bool transparent_picking;
};

struct instanced_draw_call : public draw_call
{
	instanced_draw_call();
	~instanced_draw_call();

	void render();

	nsmesh::submesh * submesh;
	nsbuffer_object * transform_buffer;
	nsbuffer_object * transform_id_buffer;
	fmat4_vector * anim_transforms;
//	fmat4 proj_cam;
	fvec2 height_minmax;
	uint32 entity_id;
	uint32 plugin_id;
	uint32 transform_count;
	bool casts_shadows;
	bool transparent_picking;
	fvec4 sel_color;
};

typedef std::map<nsmaterial*, uint32> mat_id_map;

struct light_draw_call : public draw_call
{
	light_draw_call(): draw_call() {}
	~light_draw_call() {}
	
	void render();

	nsbuffer_object * draw_point;
	fmat4 proj_light_mat;
	fmat4 proj_cam_mat;
	fmat4 light_transform;
	fvec3 light_pos;
	fvec4 bg_color;
	fvec3 direction;
	fvec3 cam_world_pos;
	bool cast_shadows;
	fvec3 light_color;
	fvec3 spot_atten;
	int32 shadow_samples;
	float shadow_darkness;
	float diffuse_intensity;
	float ambient_intensity;
	mat_id_map * material_ids;
	ivec2 shadow_tex_size;
	float max_depth;
	float cutoff;
	nsmesh::submesh * submesh;
};

typedef std::set<nsmaterial*> pmatset;
typedef std::map<nsmaterial*, drawcall_queue> mat_drawcall_map;
typedef std::map<nsrender_shader*, pmatset> shader_mat_map;

struct gbuffer_render_pass : public render_pass
{
	virtual void render();
};

struct oit_render_pass : public render_pass
{
	virtual void render();
	translucent_buffers * tbuffers;
	nsbuffer_object * single_point;
	nsfragment_sort_shader * sort_shader;
};

struct light_shadow_pass : public render_pass
{
	virtual void render();
	light_draw_call * ldc;
	nsshadow_2dmap_shader * shdr;
};

struct light_pass : public render_pass
{
	virtual void render();
	light_shadow_pass * rpass;
	translucent_buffers * tbuffers;
};

struct culled_light_pass : public light_pass
{
	virtual void render();
	nslight_stencil_shader * stencil_shdr;
};

struct final_render_pass : public render_pass
{
	virtual void render();
	nsfb_object * read_buffer;
};

struct selection_render_pass : public render_pass
{
	virtual void render();
};

class nsopengl_driver : public nsvideo_driver
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
		
	nsopengl_driver();
	~nsopengl_driver();
	
	uint32 active_tex_unit();

	void create_default_render_targets();

	void create_default_render_passes();

	void create_default_render_queues();

	opengl_state current_gl_state();

	uivec3 pick(const fvec2 & mouse_pos);

	virtual void init();

	virtual void release();

	virtual void push_scene(nsscene * scn);

	virtual void render(nsrender::viewport * vp);

	static void set_active_texture_unit(uint32 tex_unit);
	
	static uint32 bound_fbo();

	static void clear_framebuffer(uint32 clear_mask);
	
	static void enable_depth_write(bool enable);

	static void enable_depth_test(bool enable);

	static void enable_stencil_test(bool enable);

	static void enable_blending(bool enable);

	static void enable_culling(bool enable);

	static void set_cull_face(int32 cull_face);

	static void set_blend_func(int32 sfactor, int32 dfactor);
	
	static void set_blend_func(const ivec2 & blend_func);

	static void set_blend_eqn(int32 eqn);

	static void set_stencil_func(int32 func, int32 ref, int32 mask);
	
	static void set_stencil_func(const ivec3 & stencil_func);

	static void set_stencil_op(int32 sfail, int32 dpfail, int32 dppass);
	
	static void set_stencil_op(const ivec3 & stencil_op);

	static void set_stencil_op_back(int32 sfail, int32 dpfail, int32 dppass);
	
	static void set_stencil_op_back(const ivec3 & stencil_op);

	static void set_stencil_op_front(int32 sfail, int32 dpfail, int32 dppass);
	
	static void set_stencil_op_front(const ivec3 & stencil_op);

	static void set_gl_state(const opengl_state & state);

	static void set_viewport(const ivec4 & val);

	static void bind_textures(nsmaterial * material);

	static void bind_gbuffer_textures(nsfb_object * fb);

  private:
	void _add_draw_calls_from_scene(nsscene * scene);
	void _add_lights_from_scene(nsscene * scene);

	bool _handle_window_resize(window_resize_event * evt);	
	void _prepare_tforms(nsscene * scene);
	bool _valid_check();

	translucent_buffers * m_tbuffers;
	nsbuffer_object * m_single_point;

	std::vector<instanced_draw_call> m_all_draw_calls;
	std::vector<light_draw_call> m_light_draw_calls;

	mat_id_map m_mat_shader_ids;	
	static opengl_state m_gl_state;
};

#endif
