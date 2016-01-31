/*! 
	\file nsrender_system.h
	
	\brief Header file for nsrender_system class

	This file contains all of the neccessary declarations for the nsrender_system class.

	\author Daniel Randle
	\date November 23 2013
	\copywrite Earth Banana Games 2013
*/

#ifndef NSRENDERSYSTEM_H
#define NSRENDERSYSTEM_H

// Default shaders
#define GBUFFER_SHADER "gbufferdefault"
#define GBUFFER_WF_SHADER "gbufferdefault_wireframe"
#define GBUFFER_TRANS_SHADER "gbufferdefault_translucent"
#define RENDER_PARTICLE_SHADER "renderparticle"
#define LIGHTSTENCIL_SHADER "lightstencil"
#define SPOT_LIGHT_SHADER "spotlight"
#define DIR_LIGHT_SHADER "directionlight"
#define FRAGMENT_SORT_SHADER "fragment_sort"
#define POINT_LIGHT_SHADER "pointlight"
#define POINT_SHADOWMAP_SHADER "pointshadowmap"
#define SPOT_SHADOWMAP_SHADER "spotshadowmap"
#define DIR_SHADOWMAP_SHADER "dirshadowmap"
#define SKYBOX_SHADER "skybox"
#define SELECTION_SHADER "selectionsolid"

// Default render targets
#define GBUFFER_TARGET "gbuffer_target"
#define ACCUM_TARGET "accum_target"
#define DIR_SHADOW2D_TARGET "dir_shadow2d_target"
#define SPOT_SHADOW2D_TARGET "spot_shadow2d_target"
#define POINT_SHADOW_TARGET "point_shadow_target"

// Default drawcall queues
#define SCENE_OPAQUE_QUEUE "scene_opaque_queue"
#define SCENE_TRANSLUCENT_QUEUE "scene_translucent_queue"
#define DIR_LIGHT_QUEUE "dir_light_queue"
#define SPOT_LIGHT_QUEUE "spot_light_queue"
#define POINT_LIGHT_QUEUE "point_light_queue"

// Default checkered material
#define DEFAULT_MATERIAL "default"

// Max draw_calls
#define MAX_INSTANCED_DRAW_CALLS 4096
#define MAX_LIGHTS_IN_SCENE 4096
#define MAX_GBUFFER_DRAWS 2048
#define MAX_OIT_DRAWS 2048
#define MAX_ACCUM_DRAWS 2048

// Light bounds, skydome, and tile meshes
#define MESH_FULL_TILE "fulltile"
#define MESH_HALF_TILE "halftile"
#define MESH_DIRLIGHT_BOUNDS "dirlightbounds"
#define MESH_SPOTLIGHT_BOUNDS "spotlightbounds"
#define MESH_POINTLIGHT_BOUNDS "pointlightbounds"
#define MESH_TERRAIN "terrain"
#define MESH_SKYDOME "skydome"

// Framebuffer sizes
#define DEFAULT_SPOTLIGHT_SHADOW_W 1024
#define DEFAULT_SPOTLIGHT_SHADOW_H 1024
#define DEFAULT_POINTLIGHT_SHADOW_W 1024
#define DEFAULT_POINTLIGHT_SHADOW_H 1024
#define DEFAULT_DIRLIGHT_SHADOW_W 2048
#define DEFAULT_DIRLIGHT_SHADOW_H 2048
#define DEFAULT_GBUFFER_RES_X 1920
#define DEFAULT_GBUFFER_RES_Y 1080
#define DEFAULT_ACCUM_BUFFER_RES_X 1920
#define DEFAULT_ACCUM_BUFFER_RES_Y 1080

// Some default fog settings
#define DEFAULT_FOG_FACTOR_NEAR 40
#define DEFAULT_FOG_FACTOR_FAR 80

// default light counts
#define MAX_DIR_LIGHT 64
#define MAX_SPOT_LIGHT 256
#define MAX_POINT_LIGHT 256

#include <nssystem.h>
#include <nsmesh.h>

class nsmaterial;
class nsfb_object;
class nsshadowbuf_object;
class nsgbuf_object;
class nsbuffer_object;
class nsentity;
class nsmaterial_shader;
class nsearlyz_shader;
class nslight_stencil_shader;
class nsdir_light_shader;
class nspoint_light_shader;
class nsspot_light_shader;
class nsshadow_cubemap_shader;
class nsspot_shadowmap_shader;
class nsdir_shadowmap_shader;
class nsxfb_shader;
class nsrender_xfb_shader;
class nsearlyz_xfb_shader;
class nsdir_shadowmap_xfb_shader;
class nspoint_shadowmap_xfb_shader;
class nsspot_shadowmap_xfb_shader;
class nsfragment_sort_shader;
class nslight_comp;
class nsshadow_2dmap_shader;
class nsshader;
class nsbuffer_object;
class nscam_comp;
class nsselection_shader;
class nsparticle_render_shader;
class nsrender_shader;

struct opengl_state
{
	opengl_state():
		depth_write(false),
		depth_test(false),
		stencil_test(false),
		blending(false),
		culling(false),
		cull_face(0),
		blend_func(),
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
	ivec2 blend_func; // renderer
	ivec3 stencil_func; // submesh
	ivec3 stencil_op_back; // submesh
	ivec3 stencil_op_front;
	ivec4 current_viewport;
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

typedef std::vector<nsentity*> light_vector;

struct light_vectors
{
	light_vectors();
	light_vector dir_lights;
	light_vector point_lights;
	light_vector spot_lights;
	void clear();
};

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

struct single_draw_call : public draw_call
{
	single_draw_call(): draw_call() {}
	~single_draw_call() {}

	void render() {}

	nsmesh::submesh * submesh;
	fmat4 transform;
	fmat4_vector * anim_transforms;
	fmat4 proj_cam;
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
	fmat4 proj_cam;
	fvec2 height_minmax;
	uint32 entity_id;
	uint32 plugin_id;
	uint32 transform_count;
	bool casts_shadows;
	bool transparent_picking;
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
	bool lighting_enabled;
	fvec4 bg_color;
	fvec3 direction;
	fvec3 cam_world_pos;
	uivec2 fog_factor;
	fvec4 fog_color;
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
typedef std::vector<draw_call*> drawcall_queue;
typedef std::map<nsmaterial*, drawcall_queue> mat_drawcall_map;
typedef std::map<nsrender_shader*, pmatset> shader_mat_map;
typedef std::map<nsstring, nsfb_object *> rt_map;

struct render_pass
{
	render_pass():
		ren_target(nullptr),
		draw_calls(nullptr),
		norm_viewport(0.0f,0.0f,1.0f,1.0f)
	{}
	
	virtual ~render_pass() {}

	virtual void setup_pass();

	virtual void render() {}

	drawcall_queue * draw_calls;
	nsfb_object * ren_target;
	fvec4 norm_viewport;
	opengl_state gl_state;
};

struct dir_light_shadow_pass : public render_pass
{
	virtual void render();
};

struct gbuffer_render_pass : public render_pass
{
	virtual void render();
};

struct oit_render_pass : public render_pass
{
	virtual void render();
};

struct dir_light_pass : public render_pass
{
	virtual void render();
	drawcall_queue * lit_items_draw_calls;
	nsfb_object * accum_buf;
	translucent_buffers * tbuffers;
};

struct final_render_pass : public render_pass
{
	virtual void render();
	nsfb_object * read_buffer;
	ivec4 read_vp;
};

typedef std::vector<render_pass*> render_pass_vector;
typedef std::map<nsstring, drawcall_queue*> queue_map;

class nsrender_system : public nssystem
{
public:
	nsrender_system();
	~nsrender_system();

	uint32 active_tex_unit();

	// system takes ownership and will delete on shutdown
	bool add_render_target(const nsstring & name, nsfb_object * rt);

	nsfb_object * render_target(const nsstring & name);

	nsfb_object * remove_render_target(const nsstring & name);

	void destroy_render_target(const nsstring & name);

	// system takes ownership and will delete on shutdown
	bool add_queue(const nsstring & name, drawcall_queue * rt);

	drawcall_queue * create_queue(const nsstring & name);

	void clear_render_queues();

	void clear_render_targets();

	void clear_render_passes();
	
	void create_default_render_queues();

	void create_default_render_targets(uint32 screen_fbo=0);

	void create_default_render_passes();

	drawcall_queue * queue(const nsstring & name);

	drawcall_queue * remove_queue(const nsstring & name);

	void destroy_queue(const nsstring & name);

	render_pass_vector * render_passes();

	void blend_dir_lights(nsentity * camera, const fvec4 & bg_color);

	void blend_spot_lights(nsentity * camera);

	void blend_point_lights(nsentity * camera);

	void blit_final_frame();

	void render_scene();

	void render_ui(nsscene * scene);

	void render_scene_opaque(nsentity * camera);

	void render_scene_particles(nsscene * scene, nsentity * camera);

	void render_scene_translucent(nsentity * camera);

	void render();

	void pre_lighting_pass();

	void render_scene_selection(nsscene * scene, nsentity * camera);

	void enable_debug_render(bool pDebDraw);

	void enable_lighting(bool pEnable);

	void setup_default_rendering(uint32 screen_fbo = 0);

	nsmaterial * default_mat();

	uivec3 pick(float mousex, float mousey);

	opengl_state current_gl_state();

	virtual void init();

	bool debug_draw();

	nsfb_object * default_fbo();

	void set_default_mat(nsmaterial * pDefMaterial);

	void set_fog_factor(const uivec2 & near_far);

	const uivec2 & fog_factor();

	void set_fog_color(const fvec4 & near_far);

	const fvec4 & fog_color();

	void toggle_debug_draw();

	void update();

	void add_draw_calls_from_scene(nsscene * scene);

	void add_lights_from_scene(nsscene * scene);

	virtual int32 update_priority();

	static void set_active_texture_unit(uint32 tex_unit);
	
	static uint32 bound_fbo();
	
	static void enable_depth_write(bool enable);

	static void enable_depth_test(bool enable);

	static void enable_stencil_test(bool enable);

	static void enable_blending(bool enable);

	static void enable_culling(bool enable);

	static void set_cull_face(int32 cull_face);

	static void set_blend_func(int32 sfactor, int32 dfactor);
	
	static void set_blend_func(const ivec2 & blend_func);

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

	bool _handle_window_resize(window_resize_event * evt);
	
	void _blend_dir_light(nslight_comp * pLight);
	
	void _blend_point_light(nslight_comp * pLight);
	void _stencil_point_light(nslight_comp * pLight);
	
	void _blend_spot_light(nslight_comp * pLight);
	void _stencil_spot_light(nslight_comp * pLight);
	
	void _draw_geometry(const shader_mat_map & sm_map, const mat_drawcall_map & mdc_map);
	void _draw_scene_to_depth(nsshadow_2dmap_shader * pShader);
	void _draw_call(const draw_call * dc);

	void _prepare_tforms(nsscene * scene);
	bool _valid_check();

	bool m_debug_draw;
	bool m_lighting_enabled;

	uivec2 m_fog_nf;
	fvec4 m_fog_color;

	mat_id_map m_mat_shader_ids;
	std::vector<instanced_draw_call> m_all_draw_calls;
	std::vector<light_draw_call> m_light_draw_calls;
	
	light_vectors m_lights;

	rt_map m_render_targets;
	render_pass_vector m_render_passes;
	queue_map m_render_queues;

	nsfb_object * m_default_fbo;
	nsmaterial * m_default_mat;
	render_shaders m_shaders;
	translucent_buffers m_tbuffers;
	nsbuffer_object * m_single_point;
	
	static opengl_state m_gl_state;
};

#endif
