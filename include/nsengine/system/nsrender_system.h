/*! 
	\file nsrender_system.h
	
	\brief Header file for nsrender_system class

	This file contains all of the neccessary declarations for the nsrender_system class.

	\author Daniel Randle
	\date November 23 2013
	\copywrite Earth Banana Games 2013
*/

#define OPENGL

#ifndef NSRENDERSYSTEM_H
#define NSRENDERSYSTEM_H

// Default checkered material
#define DEFAULT_MATERIAL "default"

// Default drawcall queues
#define SCENE_OPAQUE_QUEUE "scene_opaque_queue"
#define SCENE_SELECTION_QUEUE "scene_selection_queue"
#define SCENE_TRANSLUCENT_QUEUE "scene_translucent_queue"
#define DIR_LIGHT_QUEUE "dir_light_queue"
#define SPOT_LIGHT_QUEUE "spot_light_queue"
#define POINT_LIGHT_QUEUE "point_light_queue"
#define UI_RENDER_QUEUE "ui_render_queue"

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
#define UI_BORDER_SHADER "ui_render_border"
#define UI_TEXT_SHADER "ui_render_text"

// Light bounds, skydome, and tile meshes
#define MESH_FULL_TILE "fulltile"
#define MESH_HALF_TILE "halftile"
#define MESH_DIRLIGHT_BOUNDS "dirlightbounds"
#define MESH_SPOTLIGHT_BOUNDS "spotlightbounds"
#define MESH_POINTLIGHT_BOUNDS "pointlightbounds"
#define MESH_TERRAIN "terrain"
#define MESH_SKYDOME "skydome"

// Input Events
#define VIEWPORT_CHANGE "mouse_pressed_in_viewport"

// Some default fog settings
#define DEFAULT_FOG_FACTOR_NEAR 40
#define DEFAULT_FOG_FACTOR_FAR 80

// Max draw_calls
#define MAX_INSTANCED_DRAW_CALLS 4096
#define MAX_LIGHTS_IN_SCENE 4096
#define MAX_GBUFFER_DRAWS 2048
#define MAX_UI_DRAW_CALLS 1024

#include <nssystem.h>
#include <nssignal.h>
#include <nsunordered_map.h>
#include <list>
#include <nsmesh.h>

class nsentity;
class nsmaterial;
class nsbuffer_object;
class nsshader;

class nsshader;

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

struct draw_call
{
	draw_call():
		shdr(nullptr),
		mat(nullptr),
		mat_index(0)
	{}
	virtual ~draw_call() {}
	
	nsshader * shdr;
	nsmaterial * mat;
	uint32 mat_index;
};

typedef std::vector<draw_call*> drawcall_queue;
typedef std::unordered_map<nsstring, drawcall_queue*> queue_map;

struct single_draw_call : public draw_call
{
	single_draw_call(): draw_call() {}
	~single_draw_call() {}

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
	instanced_draw_call():
		submesh(nullptr),
		transform_buffer(nullptr),
		transform_id_buffer(nullptr),
		anim_transforms(nullptr),
		height_minmax(),
		entity_id(0),
		plugin_id(0),
		transform_count(0),
		casts_shadows(false),
		transparent_picking(false)
	{}
	
	~instanced_draw_call() {}

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
	float max_depth;
	float cutoff;
	uint32 light_type;
	nsmesh::submesh * submesh;
};

class nsui_shader;
class nsui_border_shader;
class nsfont;

struct ui_draw_call : public draw_call
{
	ui_draw_call():
		draw_call(),
		border_shader(nullptr),
		text_shader(nullptr),
		entity_id(),
		content_tform(),
		content_wscale(1.0f),
		border_pix(0.0f),
		text(),
		fnt(nullptr),
		text_editable(false),
		cursor_pixel_width(0),
		cursor_color(1.0f,0.0f,0.0f,1.0f),
		cursor_offset(),
		text_line_sizes(),
		text_margins(),
		alignment(0)
	{}

	~ui_draw_call() {}

	// ui_rect_tform info
	uivec3 entity_id;	
	fmat3 content_tform;
	fvec2 content_wscale;

	// ui_material_comp along with shdr and mat
	nsshader * border_shader;
	nsmaterial * border_mat;
	fvec4 border_pix;

	// ui_text_comp info
	nsshader * text_shader;
	nsstring text;
	nsfont * fnt;
	std::vector<uint32> text_line_sizes;
	uivec4 text_margins;
	uint8 alignment;

	// ui_text_input_comp info
	bool text_editable;
	uint32 cursor_pixel_width;
	fvec4 cursor_color;
	uivec2 cursor_offset;
};

typedef std::set<nsmaterial*> pmatset;
typedef std::map<nsmaterial*, drawcall_queue> mat_drawcall_map;
typedef std::map<nsshader*, pmatset> shader_mat_map;

namespace nsrender
{

struct viewport
{
	viewport(
		const fvec4 & norm_bounds = fvec4(),
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

}

namespace nsrender
{
struct viewport;
}

struct vp_node
{
	vp_node(const nsstring & vp_name_, nsrender::viewport * vp_);
	~vp_node();
	nsstring vp_name;
	nsrender::viewport * vp;
};

class nsrender_system : public nssystem
{
	friend struct nsrender::viewport;
	
  public:
	
	nsrender_system();
	~nsrender_system();

	bool add_queue(const nsstring & name, drawcall_queue * rt);

	void clear_render_queues();

	void create_default_render_queues();
	
	drawcall_queue * create_queue(const nsstring & name);

	void destroy_queue(const nsstring & name);

	drawcall_queue * queue(const nsstring & name);

	drawcall_queue * remove_queue(const nsstring & name);

	nsrender::viewport * insert_viewport(const nsstring & vp_name, const nsrender::viewport & vp, const nsstring & insert_before="");

	bool remove_viewport(const nsstring & vp_name);

	nsrender::viewport * viewport(const nsstring & vp_name);

	void move_viewport_back(const nsstring & vp_name);

	void move_viewport_forward(const nsstring & vp_name);

	void move_viewport_to_back(const nsstring & vp_name);

	void move_viewport_to_front(const nsstring & vp_name);

	uivec3 pick(const fvec2 & screen_pos);

	void push_scene(nsscene * scn);

	nsrender::viewport * current_viewport();

	nsrender::viewport * front_viewport(const fvec2 & screen_pos);
	
	nsmaterial * default_mat();

	virtual void init();

	void set_default_mat(nsmaterial * pDefMaterial);

	void toggle_debug_draw();

	void update();

	void render();

	std::list<vp_node> * viewports();

	virtual int32 update_priority();

	const render_shaders & get_render_shaders();

	void set_render_shaders(const render_shaders & rs);
	
  private:
	void _add_draw_calls_from_scene(nsscene * scene);
	void _add_lights_from_scene(nsscene * scene);
	void _prepare_tforms(nsscene * scene);

	bool _handle_window_resize(window_resize_event * evt);
	bool _handle_viewport_change(nsaction_event * evt);

	nsmaterial * m_default_mat;
	nsrender::viewport * m_current_vp;
	std::list<vp_node> vp_list;

	std::vector<instanced_draw_call> m_all_draw_calls;
	std::vector<light_draw_call> m_light_draw_calls;

	render_shaders m_shaders;
	queue_map m_render_queues;
	mat_id_map m_mat_shader_ids;
};

#endif
