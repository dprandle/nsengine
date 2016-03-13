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

class nsopengl_driver;

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
	nsgl_framebuffer * ren_target;
	nsrender::viewport * vp;
	nsopengl_driver * driver;
	opengl_state gl_state;
};

typedef std::unordered_map<nsstring, nsgl_framebuffer*> rt_map;
typedef std::vector<render_pass*> render_pass_vector;

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
	fmat4 light_transform;
	fvec3 light_pos;
	fvec4 bg_color;
	fvec3 direction;
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
	uint32 light_type;
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
};

struct light_shadow_pass : public render_pass
{
	virtual void render();
	light_draw_call * ldc;
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
};

struct final_render_pass : public render_pass
{
	virtual void render();
	nsgl_framebuffer * read_buffer;
};

struct selection_render_pass : public render_pass
{
	virtual void render();
};

GLEWContext * glewGetContext();

struct gl_ctxt
{
	gl_ctxt(uint32 id);
	~gl_ctxt();
	
	void init();
	void release();
	
	uint32 context_id;
	bool initialized;
	
	GLEWContext * glew_context; // created in ctor
	translucent_buffers * m_tbuffers; // created in init
	nsbuffer_object * m_single_point; // created in init
	nsgl_framebuffer * m_default_target; // created in init
	
	queue_map m_render_queues; // created and removed by driver
	rt_map m_render_targets; // created and removed by driver
	render_pass_vector m_render_passes; // created and removed by driver
	opengl_state m_gl_state;
};

typedef std::map<uint32, gl_ctxt*> gl_context_map;

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

	bool add_queue(const nsstring & name, drawcall_queue * rt);
	
	bool add_render_target(const nsstring & name, nsgl_framebuffer * rt);

	nsgl_framebuffer * render_target(const nsstring & name);

	nsgl_framebuffer * remove_render_target(const nsstring & name);

	virtual nsgl_framebuffer * default_target();

	void destroy_render_target(const nsstring & name);

	void clear_render_targets();

	void clear_render_passes();
	
	void clear_render_queues();

	render_pass_vector * render_passes();
	
	drawcall_queue * create_queue(const nsstring & name);

	void destroy_queue(const nsstring & name);

	drawcall_queue * queue(const nsstring & name);

	drawcall_queue * remove_queue(const nsstring & name);

	void setup_default_rendering();

	void create_default_render_targets();

	void create_default_render_passes();

	void create_default_render_queues();

	uint32 create_context();

	bool destroy_context(uint32 c_id);

	bool make_context_current(uint32 c_id);

	gl_ctxt * current_context();

	opengl_state current_gl_state();

	uivec3 pick(const fvec2 & mouse_pos);

	virtual void init(bool setup_default_rend=true);

	virtual void release();

	virtual void resize_screen(const ivec2 & new_size);

	virtual void push_scene(nsscene * scn);

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

  private:
	void _add_draw_calls_from_scene(nsscene * scene);
	void _add_lights_from_scene(nsscene * scene);

	bool _handle_window_resize(window_resize_event * evt);	
	void _prepare_tforms(nsscene * scene);
	bool _valid_check();

	std::vector<instanced_draw_call> m_all_draw_calls;
	std::vector<light_draw_call> m_light_draw_calls;
	mat_id_map m_mat_shader_ids;
	
	gl_context_map m_contexts;
	gl_ctxt * m_current_context;
};

#endif
