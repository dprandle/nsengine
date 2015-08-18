/* ----------------------------- Noble Steed Engine----------------------------*
Authors : Daniel Randle, Alex Bourne
Date Created : Mar 5 2013

File:
	nsshader.h

Description:
	This file contains the shader class which can initialize/load/bind shaders
	from file or c style string
*---------------------------------------------------------------------------*/

#ifndef NSSHADER_H
#define NSSHADER_H


#include <myGL/glew.h>
#include <nsglobal.h>
#include <nsgl_object.h>
#include <nsmath.h>
#include <vector>
#include <map>
#include <nspupper.h>

#define MAX_BONE_TFORMS 100
#define PARTICLE_MAX_VISUAL_KEYS 50
#define PARTICLE_MAX_MOTION_KEYS 50

class NSLightComp;
class NSTFormComp;

#include <nsresource.h>

class nsshader : public nsresource, public nsgl_object
{
public:
	enum error_state { error_none, error_program, error_compile, error_link, error_validation };

	enum shader_type 
	{
		vertex_shader = GL_VERTEX_SHADER, 
		geometry_shader = GL_GEOMETRY_SHADER, 
		fragment_shader = GL_FRAGMENT_SHADER
	};

	enum xfb_mode {
		xfb_interleaved = GL_INTERLEAVED_ATTRIBS,
		xfb_separate = GL_SEPARATE_ATTRIBS
	};

	enum attrib_loc {
		loc_position = POS_ATT,
		loc_tex_coords = TEX_ATT,
		loc_normal = NORM_ATT,
		loc_tangent = TANG_ATT,
		loc_bone_id = BONEID_ATT,
		loc_joint = BONEWEIGHT_ATT,
		loc_instance_tform = TRANS_ATT,
		loc_ref_id = REFID_ATT
	};

	typedef std::unordered_map<uint32,uint32> uniform_loc_map;
	
	
	template <class PUPer>
	friend void pup(PUPer & p, nsshader & shader);

	nsshader();
	~nsshader();

	bool compile();

	bool compile(shader_type type);

	void unbind();

	void bind();

	void set_xfb(xfb_mode pMode, nsstringarray * pOutLocs);

	virtual void init() {}

	virtual void init_uniforms() {}

	uint32 attrib_loc(const nsstring & var_name) const;

	error_state error() const;

	void init_gl();

	uint32 init_uniform_loc(const nsstring & pVariable);

	bool compiled(shader_type type);

	bool linked();

	bool link();

	virtual void pup(NSFilePUPer * p);

	virtual void pup(NSFilePUPer * p, shader_type type);

	void release();

	void reset_error();

	const nsstring & source(shader_type type);

	void set_name(shader_type type, const nsstring & name);

	void set_source(shader_type type, const nsstring & source);

	void set_uniform(const nsstring & var_name, const fmat4 & data);

	void set_uniform(const nsstring & var_name, const fmat3 & data);

	void set_uniform(const nsstring & var_name, const fvec4 & data);

	void set_uniform(const nsstring & var_name, const fvec3 & data);

	void set_uniform(const nsstring & var_name, const fvec2 & data);

	void set_uniform(const nsstring & var_name, float data);

	void set_uniform(const nsstring & var_name, int32 data);

	void set_uniform(const nsstring & var_name, bool val);

	void set_uniform(const nsstring & var_name, uint32 data);

	uint32 gl_id(shader_type type);

	nsstring stage_name(shader_type type);

	xfb_mode xfb();

protected:
	struct shader_stage
	{
		shader_stage(shader_type pType);
		nsstring m_source;
		uint32 m_stage_id;
		shader_type m_type;
	};

	template <class PUPer>
	friend void pup(PUPer & p, nsshader::shader_stage & stage, const nsstring & var_name);

	void _setup_xfb();
	bool _validate();
	shader_stage & _stage(shader_type type);

	error_state m_error_sate;
	xfb_mode m_xfb_mode;
	shader_stage m_vertex;
	shader_stage m_geometry;
	shader_stage m_fragment;
	nsstringarray m_xfb_locs;
	uniform_loc_map m_uniform_locs;
};

template<class PUPer>
void pup(PUPer & p, nsshader::shader_type & en, const nsstring & var_name)
{
	uint32 in = static_cast<uint32>(en);
	pup(p, in, var_name);
	en = static_cast<nsshader::shader_type>(in);
}

template <class PUPer>
void pup(PUPer & p, nsshader::shader_stage & stage, const nsstring & var_name)
{
	pup(p, stage.m_type, var_name + ".type");
	pup(p, stage.m_source, var_name + ".source");
}

template <class PUPer>
void pup(PUPer & p, nsshader & shader)
{
	pup(p, shader.m_vertex, "vertex");
	pup(p, shader.m_geometry, "geometry");
	pup(p, shader.m_fragment, "fragment");

}

class nslight_shader : public nsshader
{
public:
	nslight_shader();
	~nslight_shader();

	virtual void init_uniforms();

	void set_ambient_intensity(float intensity);

	void set_cast_shadows(bool cast);

	void set_diffuse_intensity(float intensity);

	void set_light_color(const fvec3 & col);

	void set_shadow_samples(int32 samples);

	void set_shadow_darkness(float darkness);

	void set_screen_size(const fvec2 & size);

	void set_cam_world_pos(const fvec3 & camPos);

	void set_epsilon(float epsilon);

	void set_shadow_tex_size(const fvec2 & size);

	void set_world_pos_sampler(int32 sampler);

	void set_diffuse_sampler(int32 sampler);

	void set_normal_sampler(int32 sampler);

	void set_material_sampler(int32 sampler);

	void set_shadow_sampler(int32 sampler);

};

class nsdir_light_shader : public nslight_shader
{
public:
	nsdir_light_shader();
	~nsdir_light_shader();

	virtual void init_uniforms();

	virtual void init();

	void set_proj_light_mat(const fmat4 & projLightMat);

	void set_lighting_enabled(bool enable);

	void set_bg_color(const fvec3 & col);

	void set_direction(const fvec3 & dir);

};

class nspoint_light_shader : public nslight_shader
{
public:
	nspoint_light_shader();
	~nspoint_light_shader();

	virtual void init_uniforms();

	virtual void init();

	void set_const_atten(float att);

	void set_lin_atten(float lin);

	void set_exp_atten(float exp);

	void set_position(const fvec3 & pos);

	void set_max_depth(float maxd);

	void set_transform(const fmat4 & mat) { set_uniform("transform", mat); }

	void set_proj_cam_mat(const fmat4 & mat) { set_uniform("projCamMat", mat); }

	void set_node_transform(const fmat4 & mat) { set_uniform("nodeTransform", mat); }

};

class nsspot_light_shader : public nspoint_light_shader
{
public:
	nsspot_light_shader();
	~nsspot_light_shader();

	virtual void init_uniforms();

	void set_proj_light_mat(const fmat4 & projLightMat);

	void set_direction(const fvec3 & dir);

	void set_cutoff(float cutoff);

};

class nsmaterial_shader : public nsshader
{
public:
	nsmaterial_shader();
	~nsmaterial_shader();

	virtual void init_uniforms();

	virtual void init();

	void set_diffuse_sampler(int32 sampler);

	void set_opacity_sampler(int32 sampler);

	void set_normal_sampler(int32 sampler);

	void set_height_sampler(int32 sampler) { set_uniform("heightMap", sampler); }

	void set_heightmap_enabled(bool enabled) { set_uniform("hasHeightMap", enabled); }

	void set_height_minmax(const fvec2 & hu) { set_uniform("hminmax", hu); }

	void set_specular_power(float power);

	void set_specular_intensity(float intensity);

	void set_specular_color(const fvec3 & col);

	void set_entity_id(uint32 id);

	void set_plugin_id(uint32 id);

	void set_color_mode(bool enable);

	void set_frag_color_out(const fvec4 & fragcol);

	void set_diffusemap_enabled(bool enabled);

	void set_opacitymap_enabled(bool enabled);

	void set_normalmap_enabled(bool enabled);

	void set_lighting_enabled(bool enabled);

	void set_node_transform(const fmat4 & tform);

	void set_proj_cam_mat(const fmat4 & projCam);

	void set_bone_transforms(const fmat4array & transforms);

	void set_has_bones(bool hasthem);


};

class nsparticle_process_shader : public nsshader
{
public:
	nsparticle_process_shader();
	~nsparticle_process_shader();

	virtual void init_uniforms();

	void init();

	void set_random_sampler(int32 sampler);

	void set_dt(float dt);

	void set_elapsed(float elapsed);

	void set_lifetime(uint32 lifetime);

	void set_launch_freq(float freq);

	void set_angular_vel(int32 angVelocity);

	void set_motion_key_global(bool global);

	void set_visual_key_global(bool global);

	void set_interpolated_motion_keys(bool interp);

	void set_interpolated_visual_keys(bool interp);

	void set_starting_size(const fvec2 & size);

	void set_emitter_size(const fvec3 & size);

	void set_emitter_shape(uint32 shape);

	void set_initial_vel_mult(const fvec3 & mult);

	void set_motion_keys(const fvec3uimap & keys, uint32 maxKeys, uint32 lifetime);

	void set_visual_keys(const fvec3uimap & keys, uint32 maxKeys, uint32 lifetime);

};

class nsparticle_render_shader : public nsshader
{
public:
	nsparticle_render_shader();
	~nsparticle_render_shader();

	virtual void init_uniforms();

	void init();

	void set_diffuse_sampler(int32 sampler);

	void set_color_mode(bool enable);

	void set_frag_color_out(const fvec4 & col);

	void set_diffusemap_enabled(bool enable);

	void set_lifetime(float seconds);

	void set_blend_mode(uint32 mode);

	void set_proj_cam_mat(const fmat4 & mat);

	void set_cam_up(const fvec3 & vec);

	void set_cam_right(const fvec3 & vec);

	void set_cam_target(const fvec3 & vec);

	void set_world_up(const fvec3 & vec);

};

class nsdepth_shader : public nsshader
{
public:
	nsdepth_shader();
	~nsdepth_shader();

	virtual void init_uniforms();

	void set_node_transform(const fmat4 & tform);

	void set_bone_transform(const fmat4array & transforms);

	void set_has_bones(bool hasthem);

	void set_proj_mat(const fmat4 & mat) { set_uniform("projMat", mat); }

	void set_height_sampler(int32 sampler) { set_uniform("heightMap", sampler); }

	void set_height_map_enabled(bool enabled) { set_uniform("hasHeightMap", enabled); }

	void set_height_minmax(const fvec2 & hu) { set_uniform("hminmax", hu); }

};

class nsdir_shadowmap_shader : public nsdepth_shader
{
public:
	nsdir_shadowmap_shader();
	~nsdir_shadowmap_shader();

	virtual void init_uniforms();

	void init();

};

class nspoint_shadowmap_shader : public nsdepth_shader
{
public:
	nspoint_shadowmap_shader();
	~nspoint_shadowmap_shader();

	virtual void init_uniforms();

	void init() {}

	void set_light_pos(const fvec3 & pos);

	void set_max_depth(float maxd);

	void set_inverse_trans_mat(const fmat4 & invt);

};

class nsspot_shadowmap_shader : public nsdepth_shader
{
public:
	nsspot_shadowmap_shader();
	~nsspot_shadowmap_shader();

	virtual void init_uniforms();

	void init();

};

class nsearlyz_shader : public nsdepth_shader
{
public:
	nsearlyz_shader();
	~nsearlyz_shader();

	virtual void init_uniforms();

	void init();

};

class nsdir_shadowmap_xfb_shader : public nsshader
{
public:
	nsdir_shadowmap_xfb_shader(){}
	~nsdir_shadowmap_xfb_shader(){}

	virtual void init_uniforms() { set_proj_light_mat(fmat4()); }

	void init(){ nsshader::init(); }

	void set_proj_light_mat(const fmat4 & mat){ set_uniform("projLightMat", mat); }
};

class nspoint_shadowmap_xfb_shader : public nsshader
{
public:
	nspoint_shadowmap_xfb_shader(){}
	~nspoint_shadowmap_xfb_shader(){}

	void init(){ nsshader::init(); }

	virtual void init_uniforms() {
		set_proj_mat(fmat4());
		set_inverse_trans_mat(fmat4());
		set_proj_light_mat(fmat4());
		set_light_pos(fvec3());
		set_max_depth(0.0f);
	}

	void set_proj_mat(const fmat4 & mat){ set_uniform("projMat", mat); }

	void set_inverse_trans_mat(const fmat4 & mat){ set_uniform("inverseTMat", mat); }

	void set_proj_light_mat(const fmat4 & mat){ set_uniform("projLightMat", mat); }

	void set_light_pos(fvec3 pos) { set_uniform("lightPos", pos); }

	void set_max_depth(float d) { set_uniform("maxDepth", d); }

};

class nsspot_shadowmap_xfb_shader : public nsshader
{
public:
	nsspot_shadowmap_xfb_shader() {}
	~nsspot_shadowmap_xfb_shader() {}

	virtual void init_uniforms() { set_proj_light_mat(fmat4()); }

	void init(){ nsshader::init(); }

	void set_proj_light_mat(const fmat4 & mat){ set_uniform("projLightMat", mat); }

};

class nsearlyz_xfb_shader : public nsspot_shadowmap_xfb_shader
{
public:
	nsearlyz_xfb_shader(){}
	~nsearlyz_xfb_shader(){}

	virtual void init_uniforms() { set_proj_cam_mat(fmat4()); }

	void init(){ nsshader::init(); }

	void set_proj_cam_mat(const fmat4 & mat){ set_uniform("projCamMat", mat); }

};

class nsrender_xfb_shader : public nsmaterial_shader
{
public:
	nsrender_xfb_shader();
	~nsrender_xfb_shader();
};

class nsxfb_shader : public nsshader
{
public:
	nsxfb_shader();
	~nsxfb_shader();

	virtual void init_uniforms();

	void init();

	void set_node_transform(const fmat4 & mat);

};

class nslight_stencil_shader : public nsshader
{
public:
	nslight_stencil_shader() {}
	~nslight_stencil_shader() {}

	virtual void init_uniforms() {
		set_transform(fmat4());
		set_proj_cam_mat(fmat4());
	}

	void init() {}

	void set_node_transform(const fmat4 & mat) { set_uniform("nodeTransform", mat); }

	void set_transform(const fmat4 & mat) { set_uniform("transform", mat); }

	void set_proj_cam_mat(const fmat4 & mat){ set_uniform("projCamMat", mat); }
};

class nsskybox_shader : public nsmaterial_shader
{
public:
	nsskybox_shader() : nsmaterial_shader() {}
	~nsskybox_shader() {}

};

class nstransparency_shader : public nsmaterial_shader
{
public:
	nstransparency_shader() : nsmaterial_shader() {}
	~nstransparency_shader() {}
};

class nsselection_shader : public nsshader
{
public:
	nsselection_shader();
	~nsselection_shader();

	virtual void init_uniforms();

	void init();

	void set_frag_color_out(const fvec4 & col);

	void set_node_transform(const fmat4 & tform);

	void set_proj_cam_mat(const fmat4 & projCam);

	void set_bone_transform(const fmat4array & transforms);

	void set_has_bones(bool hasthem);

	void set_transform(const fmat4 & mat);

	void set_height_sampler(int32 sampler) { set_uniform("heightMap", sampler); }

	void set_heightmap_enabled(bool enabled) { set_uniform("hasHeightMap", enabled); }

	void set_height_minmax(const fvec2 & hu) { set_uniform("hminmax", hu); }
};

#endif
