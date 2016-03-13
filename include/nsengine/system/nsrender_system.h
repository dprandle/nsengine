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

// Default checkered material
#define DEFAULT_MATERIAL "default"

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
#define SELECTION_SHADER "selectionsolid"
#define SKYBOX_SHADER "skybox"

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

#include <nssystem.h>
#include <nssignal.h>
#include <nsunordered_map.h>
#include <list>


class nsentity;
class nsmaterial;

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
		picking_enabled(selection) {}

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

	bool insert_viewport(const nsstring & vp_name, const nsrender::viewport & vp, const nsstring & insert_before="");

	bool remove_viewport(const nsstring & vp_name);

	nsrender::viewport * viewport(const nsstring & vp_name);

	void move_viewport_back(const nsstring & vp_name);

	void move_viewport_forward(const nsstring & vp_name);

	void move_viewport_to_back(const nsstring & vp_name);

	void move_viewport_to_front(const nsstring & vp_name);

	uivec3 pick(const fvec2 & screen_pos);

	nsrender::viewport * current_viewport();

	nsrender::viewport * front_viewport(const fvec2 & screen_pos);
	
	nsmaterial * default_mat();

	virtual void init();

	void set_default_mat(nsmaterial * pDefMaterial);

	void toggle_debug_draw();

	void update();

	void render();

	virtual int32 update_priority();
	
  private:

	bool _handle_window_resize(window_resize_event * evt);
	bool _handle_viewport_change(nsaction_event * evt);

	nsmaterial * m_default_mat;
	nsrender::viewport * m_current_vp;
	std::list<vp_node> vp_list;
};

#endif
