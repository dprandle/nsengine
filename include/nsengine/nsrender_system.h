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
#define LIGHTSTENCIL_SHADER "lightstencil"
#define SPOT_LIGHT_SHADER "spotlight"
#define DIR_LIGHT_SHADER "directionlight"
#define FRAGMENT_SORT_SHADER "fragment_sort"
#define POINT_LIGHT_SHADER "pointlight"
#define POINT_SHADOWMAP_SHADER "pointshadowmap"
#define SPOT_SHADOWMAP_SHADER "spotshadowmap"
#define DIR_SHADOWMAP_SHADER "dirshadowmap"
#define SKYBOX_SHADER "skybox"

// Default checkered material
#define DEFAULT_MATERIAL "default"

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
#define DEFAULT_FB_RES_X 1920
#define DEFAULT_FB_RES_Y 1080

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
class nspoint_shadowmap_shader;
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
class nsdepth_shader;
class nsshader;
class nsbuffer_object;
class nscam_comp;

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

struct draw_call
{
	draw_call(nsmesh::submesh * submesh_,
			  fmat4_vector * anim_transforms_,
			  nsbuffer_object * transform_buffer_,
			  nsbuffer_object * transform_id_buffer_,
			  const fvec2 & height_minmax_,
			  uint32 ent_id,
			  uint32 plug_id,
			  uint32 transform_count_,
			  bool casts_shadows_,
			  bool selectable_
		);
	~draw_call();

	nsmesh::submesh * submesh;
	fmat4_vector * anim_transforms;
	nsbuffer_object * transform_buffer;
	nsbuffer_object * transform_id_buffer;
	fvec2 height_minmax;
	uint32 entity_id;
	uint32 plugin_id;
	uint32 transform_count;
	bool casts_shadows;
	bool transparent_picking;

	bool operator<(const draw_call & rhs) const;
	bool operator<=(const draw_call & rhs) const;
	bool operator>(const draw_call & rhs) const;
	bool operator>=(const draw_call & rhs) const;
	bool operator!=(const draw_call & rhs) const;
	bool operator==(const draw_call & rhs) const;
};

struct render_shaders
{
	render_shaders();
	bool error();
	bool valid();
	
	nsmaterial_shader * deflt;
	nsmaterial_shader * deflt_wireframe;
	nsmaterial_shader * deflt_translucent;
	nslight_stencil_shader * light_stencil;
	nsfragment_sort_shader * frag_sort;
	nsdir_light_shader * dir_light;
	nspoint_light_shader * point_light;
	nsspot_light_shader * spot_light;
	nspoint_shadowmap_shader * point_shadow;
	nsspot_shadowmap_shader * spot_shadow;
	nsdir_shadowmap_shader * dir_shadow;
};

typedef std::set<nsmaterial*> pmatset;
typedef std::set<draw_call> drawcall_set;
typedef std::map<nsmaterial*, drawcall_set> mat_drawcall_map;
typedef std::map<nsmaterial_shader*, pmatset> shader_mat_map;
typedef std::map<nsmaterial*, uint32> mat_id_map;

class nsrender_system : public nssystem
{
public:
	nsrender_system();
	~nsrender_system();

	uint32 active_tex_unit();

	void blend_dir_lights(nsentity * camera, const fvec4 & bg_color);

	void blend_spot_lights(nsentity * camera);

	void blend_point_lights(nsentity * camera);

	void blit_final_frame();

	void draw();

	void draw_opaque(nsentity * camera);

	void draw_translucent(nsentity * camera);

	void draw_pre_lighting_pass();

	void enable_debug_draw(bool pDebDraw);

	void enable_lighting(bool pEnable);

	void set_active_texture_unit(uint32 tex_unit);

	nsmaterial * default_mat();

	uivec3 pick(float mousex, float mousey);

	virtual void init();

	void resize_screen(const ivec2 & size);

	const ivec2 & screen_size();

	uivec3 shadow_fbo();

	uint32 final_fbo();

	uint32 gbuffer_fbo();

	uint32 bound_fbo();

	uint32 screen_fbo();

	void set_screen_fbo(uint32 fbo);

	bool debug_draw();

	void set_default_mat(nsmaterial * pDefMaterial);

	void set_shaders(const render_shaders & pShaders);

	void set_gbuffer_fbo(uint32 fbo);

	void set_final_fbo(uint32 fbo);

	void set_shadow_fbo(uint32 fbo1, uint32 fbo2, uint32 fbo3);

	void set_fog_factor(const uivec2 & near_far);

	const uivec2 & fog_factor();

	void set_fog_color(const fvec4 & near_far);

	const fvec4 & fog_color();

	void toggle_debug_draw();

	void update();

	void add_draw_calls_from_scene(nsscene * scene);

	void add_lights_from_scene(nsscene * scene);

	void update_material_ids();

	virtual int32 draw_priority();

	virtual int32 update_priority();
	
private:
	void _blend_dir_light(nslight_comp * pLight);
	
	void _blend_point_light(nslight_comp * pLight);
	void _stencil_point_light(nslight_comp * pLight);
	
	void _blend_spot_light(nslight_comp * pLight);
	void _stencil_spot_light(nslight_comp * pLight);
	
	void _draw_geometry(const shader_mat_map & sm_map, const mat_drawcall_map & mdc_map, nsentity * camera);
	void _draw_scene_to_depth(nsdepth_shader * pShader);
	void _draw_call(drawcall_set::iterator pDCIter);
	bool _valid_check();
	void _bind_gbuffer_textures();

	bool m_debug_draw;
	bool m_lighting_enabled;

	ivec2 m_screen_size;
	uivec2 m_fog_nf;
	fvec4 m_fog_color;

	mat_drawcall_map m_drawcall_map;
	shader_mat_map m_shader_mat_map;
	mat_drawcall_map m_tdrawcall_map;
	shader_mat_map m_tshader_mat_map;
	mat_id_map m_mat_shader_ids;

	light_vectors m_lights;
	
	uint32 m_screen_fbo;

	nsgbuf_object * m_gbuffer;
	nsshadowbuf_object * m_shadow_buf;
	nsfb_object * m_final_buf;
	nsmaterial * m_default_mat;
	render_shaders m_shaders;
	translucent_buffers m_tbuffers;
	nsbuffer_object * m_single_point;
};

#endif
