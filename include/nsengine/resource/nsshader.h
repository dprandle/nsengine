#ifndef NSSHADER_H
#define NSSHADER_H

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
#include <nsresource.h>

class nslight_comp;
class nstform_comp;

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

	nsshader(const nsshader & copy_);

	~nsshader();

	nsshader & operator=(nsshader rhs);

	bool compile();

	bool compile(shader_type type);

	void unbind() const;

	void bind() const;

	void set_xfb(xfb_mode pMode, nsstring_vector * pOutLocs);

	virtual void init() {}

	virtual void init_uniforms() {}

	uint32 attrib_loc(const nsstring & var_name) const;

	error_state error() const;

	void video_init();

	uint32 init_uniform_loc(const nsstring & pVariable);

	bool compiled(shader_type type);

	bool linked();

	bool link();

	virtual void pup(nsfile_pupper * p);

	virtual void pup(nsfile_pupper * p, shader_type type);

	void video_release();

	void reset_error();

	const nsstring & source(shader_type type);

	void set_gl_id(uint32 id, shader_type type);

	void set_name(shader_type type, const nsstring & name);

	void set_source(shader_type type, const nsstring & source);

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

	uint32 gl_id(shader_type type);

	nsstring stage_name(shader_type type);

	xfb_mode xfb();	

protected:
	struct shader_stage
	{
		shader_stage(shader_type pType);
		nsstring m_source;
		uint32 m_stage_id[MAX_CONTEXT_COUNT];
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
	nsstring_vector m_xfb_locs;
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
#endif
