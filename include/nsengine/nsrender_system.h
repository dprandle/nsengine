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
	
	nsmaterial * default_mat();

	virtual void init();

	void set_default_mat(nsmaterial * pDefMaterial);

	void toggle_debug_draw();

	void update();

	void render();

	virtual int32 update_priority();
	
  private:

	bool _handle_window_resize(window_resize_event * evt);

	nsmaterial * m_default_mat;
	std::list<vp_node> vp_list;
};

#endif
