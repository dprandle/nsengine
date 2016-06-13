#ifndef NSSHADER_H
#define NSSHADER_H

#include <nsresource.h>
#include <nsvideo_driver.h>
#include <myGL/glew.h>

struct shader_stage_info
{
	shader_stage_info(const nsstring & name_="", const nsstring & source_=""):
		name(name_),
		source(source_)
	{}
		
	nsstring name;
	nsstring source;
};

class nsshader : public nsresource, public nsvideo_object
{
public:

	template <class PUPer>
	friend void pup(PUPer & p, nsshader & shader);

	nsshader();

	nsshader(const nsshader & copy_);

	~nsshader();

	nsshader & operator=(nsshader rhs);

	void init() {}

	void video_context_init();

	virtual void pup(nsfile_pupper * p);

	std::vector<shader_stage_info> shader_stages;

	enum shader_type 
	{
		vertex_shader = GL_VERTEX_SHADER, 
		geometry_shader = GL_GEOMETRY_SHADER, 
		fragment_shader = GL_FRAGMENT_SHADER
	};
	
	struct shader_stage
	{
		shader_stage(shader_type pType):
			m_type(pType)
		{}
		
		nsstring m_source;
		uint32 m_stage_id[MAX_CONTEXT_COUNT];
		shader_type m_type;
	};

	shader_stage m_vertex;
	shader_stage m_geometry;
	shader_stage m_fragment;
};

template <class PUPer>
void pup(PUPer & p, shader_stage_info & stage, const nsstring & var_name)
{
	pup(p, stage.name, var_name + ".name");
	pup(p, stage.source, var_name + ".source");
}

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
	if (p.mode() == PUP_OUT)
	{
		pup(p, shader.shader_stages, "stage");
	}
	else
	{
		pup(p, shader.m_vertex, "vertex");
		pup(p, shader.m_geometry, "geometry");
		pup(p, shader.m_fragment, "fragment");
		shader.shader_stages.resize(3);
		shader.shader_stages[0].name = "vertex"; shader.shader_stages[0].source = shader.m_vertex.m_source;
		shader.shader_stages[1].name = "fragment"; shader.shader_stages[1].source = shader.m_fragment.m_source;
		shader.shader_stages[2].name = "geometry"; shader.shader_stages[2].source = shader.m_geometry.m_source;
	}
}


#endif
