#ifndef NSGL_SHADER_H
#define NSGL_SHADER_H

#define MAX_BONE_TFORMS 100
#define PARTICLE_MAX_VISUAL_KEYS 50
#define PARTICLE_MAX_MOTION_KEYS 50

#define DEFAULT_SHADOW_EPSILON 0.00005f

#define POS_ATT 0
#define TEX_ATT 1
#define NORM_ATT 2
#define TANG_ATT 3
#define BONEID_ATT 4
#define BONEWEIGHT_ATT 5
#define TRANS_ATT 6
#define REFID_ATT 10

#define DIFFUSE_TEX_UNIT 0
#define NORMAL_TEX_UNIT 1
#define HEIGHT_TEX_UNIT 2
#define SPECULAR_TEX_UNIT 3
#define DISPLACEMENT_TEX_UNIT 4
#define AMBIENT_TEX_UNIT 5
#define EMISSIVE_TEX_UNIT 6
#define SHININESS_TEX_UNIT 7
#define OPACITY_TEX_UNIT 8
#define LIGHT_TEX_UNIT 9
#define REFLECTION_TEX_UNIT 10
#define G_DIFFUSE_TEX_UNIT 11
#define G_WORLD_POS_TEX_UNIT 12
#define G_NORMAL_TEX_UNIT 13
#define G_PICKING_TEX_UNIT 14
#define FINAL_TEX_UNIT 15
#define SKYBOX_TEX_UNIT 16
#define SHADOW_TEX_UNIT 17
#define RAND_TEX_UNIT 18

#include <nsgl_object.h>
#include <nsunordered_map.h>

struct nsgl_shader : public nsgl_obj
{
	enum error_state { error_none, error_program, error_compile, error_link, error_validation };

	enum shader_stage 
	{
		no_stage,
		vertex_shader = GL_VERTEX_SHADER,
		tess_control_shader = GL_TESS_CONTROL_SHADER,
		tess_evaluation_shader = GL_TESS_EVALUATION_SHADER,
		geometry_shader = GL_GEOMETRY_SHADER,
		fragment_shader = GL_FRAGMENT_SHADER,
		compute_shader = GL_COMPUTE_SHADER
	};

	struct shader_stage_info
	{
		shader_stage_info():
			gl_id(0),
			stage(no_stage)
		{}
		
		uint32 gl_id;
		shader_stage stage;
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

	nsgl_shader();

	~nsgl_shader();

	bool compile();

	bool compile(shader_stage stage, const nsstring & source);

	void unbind() const;

	void bind() const;

	void init();

	virtual void init_uniforms() {}

	uint32 attrib_loc(const nsstring & var_name) const;

	uint32 init_uniform_loc(const nsstring & pVariable);

	bool compiled(shader_stage stage);

	bool linked();

	bool link();

	void release();

	void reset_error();

	const nsstring & source(shader_stage stage);

	void set_gl_id(uint32 id, shader_stage stage);

	void set_name(shader_stage stage, const nsstring & name);

	void set_source(shader_stage stage, const nsstring & source);

	void set_uniform(const nsstring & var_name, const fmat4 & data);

	void set_uniform(const nsstring & var_name, const fmat3 & data);

	void set_uniform(const nsstring & var_name, const fmat2 & data);

	void set_uniform(const nsstring & var_name, const fvec4 & data);

	void set_uniform(const nsstring & var_name, const fvec3 & data);

	void set_uniform(const nsstring & var_name, const fvec2 & data);

	void set_uniform(const nsstring & var_name, const ivec4 & data);

	void set_uniform(const nsstring & var_name, const ivec3 & data);

	void set_uniform(const nsstring & var_name, const ivec2 & data);

	void set_uniform(const nsstring & var_name, const uivec4 & data);

	void set_uniform(const nsstring & var_name, const uivec3 & data);

	void set_uniform(const nsstring & var_name, const uivec2 & data);

	void set_uniform(const nsstring & var_name, float data);

	void set_uniform(const nsstring & var_name, int32 data);

	void set_uniform(const nsstring & var_name, uint32 data);

	void set_uniform(const nsstring & var_name, bool val);

	nsstring stage_name(shader_stage stage);

	shader_stage_info * stage_info(shader_stage stage);


	void _setup_xfb();
	bool _validate();

	error_state error_sate;
	xfb_mode xfb;
	shader_stage_info stages[6];
	nsstring_vector xfb_locs;
	ui_ui_umap uniform_locs;
};
#endif
