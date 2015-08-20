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
class nslight_comp;
class nsdepth_shader;
class nsshader;

class nsrender_system : public nssystem
{
public:
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
				  bool casts_shadows_);
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

		bool operator<(const draw_call & rhs) const;
		bool operator<=(const draw_call & rhs) const;
		bool operator>(const draw_call & rhs) const;
		bool operator>=(const draw_call & rhs) const;
		bool operator!=(const draw_call & rhs) const;
		bool operator==(const draw_call & rhs) const;
	};

	struct RenderShaders
	{
		RenderShaders() :
			deflt(NULL),
			early_z(NULL),
			light_stencil(NULL),
			dir_light(NULL),
			point_light(NULL),
			spot_light(NULL),
			point_shadow(NULL),
			spot_shadow(NULL),
			dir_shadow(NULL),
			xfb_default(NULL),
			xfb_render(NULL),
			xfb_earlyz(NULL),
			xfb_dir_shadow(NULL),
			xfb_point_shadow(NULL),
			xfb_spot_shadow(NULL)
		{}

		nsmaterial_shader * deflt;
		nsearlyz_shader * early_z;
		nslight_stencil_shader * light_stencil;
		nsdir_light_shader * dir_light;
		nspoint_light_shader * point_light;
		nsspot_light_shader * spot_light;
		nspoint_shadowmap_shader * point_shadow;
		nsspot_shadowmap_shader * spot_shadow;
		nsdir_shadowmap_shader * dir_shadow;
		nsxfb_shader * xfb_default;
		nsrender_xfb_shader * xfb_render;
		nsearlyz_xfb_shader * xfb_earlyz;
		nsdir_shadowmap_xfb_shader * xfb_dir_shadow;
		nspoint_shadowmap_xfb_shader * xfb_point_shadow;
		nsspot_shadowmap_xfb_shader * xfb_spot_shadow;

		bool error();
		bool valid();
	};

	typedef std::set<nsmaterial*> pmatset;
	typedef std::set<draw_call> drawcall_set;
	typedef std::map<nsmaterial*, drawcall_set> mat_drawcall_map;
	typedef std::map<nsmaterial_shader*, pmatset> shader_mat_map;
	typedef std::set<nsentity*> xfb_draw_set;

	nsrender_system();
	~nsrender_system();

	void blit_final_frame();

	void draw();

	void enable_earlyz(bool pEnable);

	void enable_debug_draw(bool pDebDraw);

	void enable_lighting(bool pEnable);

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

	void set_shaders(const RenderShaders & pShaders);

	void set_gbuffer_fbo(uint32 fbo);

	void set_final_fbo(uint32 fbo);

	void set_shadow_fbo(uint32 fbo1, uint32 fbo2, uint32 fbo3);

	void toggle_debug_draw();

	void update();

	virtual int32 draw_priority();

	virtual int32 update_priority();

private:
	void _blend_dir_light(nslight_comp * pLight);
	void _blend_point_light(nslight_comp * pLight);
	void _blend_spot_light(nslight_comp * pLight);
	void _stencil_point_light(nslight_comp * pLight);
	void _stencil_spot_light(nslight_comp * pLight);
	void _draw_geometry();
	void _draw_xfbs();
	void _draw_scene_to_depth(nsdepth_shader * pShader);
	void _draw_scene_to_depth_xfb(nsshader * pShader);
	void _draw_call(drawcall_set::iterator pDCIter);
	bool _valid_check();

	bool m_debug_draw;
	bool m_earlyz_enabled;
	bool m_lighting_enabled;

	ivec2 m_screen_size;


	mat_drawcall_map m_drawcall_map;
	//MatDCMap mTransparentDrawCallMap;
	shader_mat_map m_shader_mat_map;
	//ShaderMaterialMap mTransparentShaderMatMap;
	xfb_draw_set m_xfb_draws;
	uint32 m_screen_fbo;

	nsgbuf_object * m_gbuffer;
	nsshadowbuf_object * m_shadow_buf;
	nsfb_object * m_final_buf;
	nsmaterial * m_default_mat;
	RenderShaders m_shaders;
};

#endif
