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
#include <nsglobal.h>
#include <nssystem.h>
#include <nsrender_comp.h>
#include <nsanim_comp.h>
#include <nstform_comp.h>
#include <nsbuffer_object.h>
#include <map>

#include <nsframebuffer.h>


class nsrender_system : public NSSystem
{
public:
	struct draw_call
	{
		draw_call(nsmesh::submesh * submesh_,
				  fmat4array * anim_transforms_,
				  NSBufferObject * transform_buffer_,
				  NSBufferObject * transform_id_buffer_,
				  const fvec2 & height_minmax_,
				  uint32 ent_id,
				  uint32 plug_id,
				  uint32 transform_count_,
				  bool casts_shadows_);
		~draw_call();

		nsmesh::submesh * submesh;
		fmat4array * anim_transforms;
		NSBufferObject * transform_buffer;
		NSBufferObject * transform_id_buffer;
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

		bool error()
		{
			return (
				deflt->error() != nsshader::error_none ||
				early_z->error() != nsshader::error_none ||
				light_stencil->error() != nsshader::error_none ||
				dir_light->error() != nsshader::error_none ||
				point_light->error() != nsshader::error_none ||
				spot_light->error() != nsshader::error_none ||
				point_shadow->error() != nsshader::error_none ||
				spot_shadow->error() != nsshader::error_none ||
				dir_shadow->error() != nsshader::error_none ||
				xfb_default->error() != nsshader::error_none ||
				xfb_earlyz->error() != nsshader::error_none ||
				xfb_dir_shadow->error() != nsshader::error_none ||
				xfb_point_shadow->error() != nsshader::error_none ||
				xfb_spot_shadow->error() != nsshader::error_none ||
				xfb_render->error() != nsshader::error_none);
		}

		bool valid()
		{
			return (
				deflt != NULL &&
				early_z != NULL &&
				light_stencil != NULL &&
				dir_light != NULL &&
				point_light != NULL &&
				spot_light != NULL &&
				point_shadow != NULL &&
				spot_shadow != NULL &&
				dir_shadow != NULL &&
				xfb_default != NULL &&
				xfb_earlyz != NULL &&
				xfb_dir_shadow != NULL &&
				xfb_point_shadow != NULL &&
				xfb_spot_shadow != NULL &&
				xfb_render != NULL);
		}
	};

	typedef std::set<draw_call> drawcall_set;
	typedef std::map<nsmaterial*, drawcall_set> mat_drawcall_map;
	typedef std::map<nsmaterial_shader*, nspmatset> shader_mat_map;
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

	//virtual bool handleEvent(NSEvent * pEvent);

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
	void _blend_dir_light(NSLightComp * pLight);
	void _blend_point_light(NSLightComp * pLight);
	void _blend_spot_light(NSLightComp * pLight);
	void _stencil_point_light(NSLightComp * pLight);
	void _stencil_spot_light(NSLightComp * pLight);
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

	NSGBuffer * m_gbuffer;
	NSShadowBuffer * m_shadow_buf;
	NSFrameBuffer * m_final_buf;
	nsmaterial * m_default_mat;
	RenderShaders m_shaders;
};

#endif
