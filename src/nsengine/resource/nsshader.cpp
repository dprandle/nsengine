/*--------------------------------------------- Noble Steed Engine--------------------------------------*
Authors : Daniel Randle, Alex Bourne
Date Created : Mar 5 2013

File:
	nsshader.cpp

Description:
	This file contains the definition for the nsshader class and any associated functions that help
	the ol' girl out
*-----------------------------------------------------------------------------------------------------*/
#include <nsmath.h>
#include <nsshader.h>
#include <nsmaterial.h>
#include <nstform_comp.h>
#include <nslight_comp.h>
#include <nsengine.h>
#include <nsengine.h>
#include <nsmaterial.h>
#include <exception>
#include <string.h>
#include <nsrender_system.h>
#include <nsopengl_driver.h>

nsshader::nsshader():
	nsresource(),
	m_error_sate(error_none),
	m_fragment(fragment_shader),
	m_vertex(vertex_shader),
	m_geometry(geometry_shader),
	m_uniform_locs(),
	m_xfb_mode(xfb_interleaved)
{
	set_ext(DEFAULT_SHADER_EXTENSION);
	init_gl();
}

nsshader::nsshader(const nsshader & copy_):
	nsresource(copy_),
	m_error_sate(copy_.m_error_sate),
	m_fragment(fragment_shader),
	m_vertex(vertex_shader),
	m_geometry(geometry_shader),
	m_uniform_locs(copy_.m_uniform_locs),
	m_xfb_mode(copy_.m_xfb_mode)
{
	init_gl();
}

nsshader::~nsshader()
{
	release();
}


nsshader & nsshader::operator=(nsshader rhs)
{
	nsresource::operator=(rhs);
	std::swap(m_error_sate, rhs.m_error_sate);
	std::swap(m_fragment, rhs.m_fragment);
	std::swap(m_vertex, rhs.m_vertex);
	std::swap(m_geometry, rhs.m_geometry);
	std::swap(m_uniform_locs, rhs.m_uniform_locs);
	std::swap(m_xfb_mode, rhs.m_xfb_mode);
	return *this;
}

bool nsshader::compile()
{
	bool ret = true;

	if (!m_fragment.m_source.empty())
		ret = compile(fragment_shader) && ret;
	if (!m_vertex.m_source.empty())
		ret = compile(vertex_shader) && ret;
	if (!m_geometry.m_source.empty())
		ret = compile(geometry_shader) && ret;

	if (ret)
	{
		dprint("nsshader::compile - Succesfully compiled shader " + m_name);
	}
	
	return ret;
}

bool nsshader::compile(shader_type type)
{
	if (_stage(type).m_stage_id != 0)
		glDeleteShader(_stage(type).m_stage_id);

	_stage(type).m_stage_id = glCreateShader(type);
	gl_err_check("nsshader::compile error creating shader");
	// create arguements for glShaderSource
	const char* c_str = _stage(type).m_source.c_str();

	glShaderSource(_stage(type).m_stage_id, 1, &c_str, NULL); // Attach source to shader
	gl_err_check("nsshader::compile error setting shader source");

	glCompileShader(_stage(type).m_stage_id);
	gl_err_check("nsshader::compile error compiling shader");

	GLint worked;
	glGetShaderiv(_stage(type).m_stage_id, GL_COMPILE_STATUS, &worked);
	if (!worked)
	{
		char infoLog[1024];
		glGetShaderInfoLog(_stage(type).m_stage_id, sizeof(infoLog), NULL, (GLchar*)infoLog);
		nsstring info(infoLog);
		m_error_sate = error_compile;

#ifdef NSDEBUG
		nsstringstream ss;
		ss << "nsshader::compile : Error compiling shader stage " << stage_name(type) << " from " << m_name << "\n";
			ss << info;
		dprint(ss.str());
#endif

		glDeleteShader(_stage(type).m_stage_id);
		gl_err_check("nsshader::compile error deleting shader");
		_stage(type).m_stage_id = 0;
		return false;
	}
	dprint("nsshader::compile - Succesfully compiled shader stage " + stage_name(type) + " from " + m_name);
	return true;
}

bool nsshader::compiled(shader_type type)
{
	return (_stage(type).m_stage_id != 0);
}

bool nsshader::linked()
{
	return (m_gl_name != 0);
}

void nsshader::set_source(shader_type type, const nsstring & source)
{
	_stage(type).m_source = source;
}

void nsshader::init_gl()
{
	m_gl_name = glCreateProgram();
	gl_err_check("nsshader::link Error creating program");
}

uint32 nsshader::gl_id(shader_type type)
{
	return _stage(type).m_stage_id;
}

uint32 nsshader::init_uniform_loc(const nsstring & var_name)
{
	uint32 loc = glGetUniformLocation( m_gl_name, var_name.c_str() );
	gl_err_check("nsshader::initUniformLoc");
	m_uniform_locs[hash_id(var_name)] = loc;
	return loc;
}

bool nsshader::link()
{
	if (m_gl_name == 0)
	{
		m_error_sate = error_program;
		return false;
	}

	if (m_fragment.m_stage_id)
		glAttachShader(m_gl_name, m_fragment.m_stage_id);
	if (m_vertex.m_stage_id)
		glAttachShader(m_gl_name, m_vertex.m_stage_id);
	if (m_geometry.m_stage_id)
		glAttachShader(m_gl_name, m_geometry.m_stage_id);

	gl_err_check("nsshader::link Error attaching program");
	if (!m_xfb_locs.empty())
		_setup_xfb();

	glLinkProgram(m_gl_name);
	gl_err_check("nsshader::link Error linking program");
	if (m_fragment.m_stage_id)
		glDetachShader(m_gl_name, m_fragment.m_stage_id);
	if (m_vertex.m_stage_id)
		glDetachShader(m_gl_name, m_vertex.m_stage_id);
	if (m_geometry.m_stage_id)
		glDetachShader(m_gl_name, m_geometry.m_stage_id);

	gl_err_check("nsshader::link Error detaching program");
	GLint worked;
	glGetProgramiv(m_gl_name, GL_LINK_STATUS, &worked);
	if (!worked)
	{
		char infoLog[1024];
		glGetProgramInfoLog(m_gl_name, sizeof(infoLog), NULL,(GLchar*)infoLog);
		nsstring info(infoLog);
		m_error_sate = error_link;
		#ifdef NSDEBUG
		nsstringstream ss;
		ss << "Error linking shader with name " << m_name << "\n";
		ss << info;
		dprint(ss.str());
		#endif
		glDeleteProgram(m_gl_name);
		gl_err_check("nsshader::link Error deleting program");
		m_gl_name = 0;
		return false;
	}
	dprint("nsshader::link - Succesfully linked shader " + m_name);
	/*Validate may fail on older GPUS - add exceptions here!!*/
	return _validate();
}

void nsshader::pup(nsfile_pupper * p)
{
	if (p->type() == nsfile_pupper::pup_binary)
	{
		nsbinary_file_pupper * bf = static_cast<nsbinary_file_pupper *>(p);
		::pup(*bf, *this);
	}
	else
	{
		
		nstext_file_pupper * tf = static_cast<nstext_file_pupper *>(p);
		::pup(*tf, *this);
	}
}

nsstring nsshader::stage_name(shader_type type)
{
	nsstring typestr;
	switch (type)
	{
	case(vertex_shader) :
		typestr = "vertex";
		break;
	case(geometry_shader) :
		typestr = "geometry";
		break;
	case(fragment_shader) :
		typestr = "fragment";
		break;
	}
	return typestr;
}

void nsshader::pup(nsfile_pupper * p, shader_type type)
{
	nsstring typestr = stage_name(type);

	if (p->type() == nsfile_pupper::pup_binary)
	{
		nsbinary_file_pupper * bf = static_cast<nsbinary_file_pupper *>(p);
		::pup(*bf, _stage(type), typestr);
	}
	else
	{
		nstext_file_pupper * tf = static_cast<nstext_file_pupper *>(p);
		::pup(*tf, _stage(type), typestr);
	}
}

const nsstring & nsshader::source(shader_type type)
{
	return _stage(type).m_source;
}

nsshader::shader_stage & nsshader::_stage(shader_type type)
{
	switch (type)
	{
	case(vertex_shader) :
		return m_vertex;
	case(geometry_shader) :
		return m_geometry;
	case(fragment_shader) :
		return m_fragment;
	default:
		throw std::exception();
	}
}

void nsshader::_setup_xfb()
{
	GLchar ** varyings = new GLchar*[m_xfb_locs.size()];
	for (uint32 i = 0; i < m_xfb_locs.size(); ++i)
	{
		GLchar * str = new GLchar[m_xfb_locs[i].size() + 1];
		strcpy(str, m_xfb_locs[i].c_str());
		varyings[i] = str;
	}

 	glTransformFeedbackVaryings(m_gl_name, 4, varyings, m_xfb_mode);
	gl_err_check("nsshader::_setupTransformFeedback()");

	for (uint32 i = 0; i < m_xfb_locs.size(); ++i)
		delete[] varyings[i];
	delete[] varyings;
}

void nsshader::set_xfb(xfb_mode pMode, nsstring_vector * pOutLocs)
{
	// Need to link after doing this - if shader has been linked already then just re-link (ie call link again)
	m_xfb_mode = pMode;
	m_xfb_locs.clear();
	nsstring_vector::iterator sIter = pOutLocs->begin();
	while (sIter != pOutLocs->end())
	{
		m_xfb_locs.push_back(*sIter);
		++sIter;
	}
}

void nsshader::bind() const
{
	glUseProgram(m_gl_name);
	gl_err_check("nsshader::bind");
}

void nsshader::unbind() const
{
	glUseProgram(0);
	gl_err_check("nsshader::bind");
}

void nsshader::set_uniform(const nsstring & var_name, const fmat4 & data)
{
	fmat4 mat = data;
	uniform_loc_map::iterator iter = m_uniform_locs.find(hash_id(var_name));
	if (iter == m_uniform_locs.end())
		glUniformMatrix4fv(init_uniform_loc(var_name), 1, GL_TRUE, mat.data_ptr());
	else
		glUniformMatrix4fv(iter->second, 1, GL_TRUE, mat.data_ptr());
	gl_err_check("nsshader::set_uniform");
}

void nsshader::set_uniform(const nsstring & var_name, const fmat3 & data)
{
	fmat3 mat = data;
	uniform_loc_map::iterator iter = m_uniform_locs.find(hash_id(var_name));
	if (iter == m_uniform_locs.end())
		glUniformMatrix3fv(init_uniform_loc(var_name), 1, GL_TRUE, mat.data_ptr());
	else
		glUniformMatrix3fv(iter->second, 1, GL_TRUE, mat.data_ptr());
	gl_err_check("nsshader::set_uniform");
}

void nsshader::set_uniform(const nsstring & var_name, bool val)
{
	set_uniform(var_name, int32(val));
}

void nsshader::set_uniform(const nsstring & var_name, const fvec4 & data)
{
	GLfloat vec[4];
	vec[0] = data.x; vec[1] = data.y; vec[2] = data.z; vec[3] = data.w;
	uniform_loc_map::iterator iter = m_uniform_locs.find(hash_id(var_name));
	if (iter == m_uniform_locs.end())
		glUniform4fv(init_uniform_loc(var_name), 1, vec);
	else
		glUniform4fv(iter->second, 1, vec);
	gl_err_check("nsshader::set_uniform");
}

void nsshader::set_uniform(const nsstring & var_name, const fvec3 & data)
{
	GLfloat vec[3];
	vec[0] = data.x; vec[1] = data.y; vec[2] = data.z;
	uniform_loc_map::iterator iter = m_uniform_locs.find(hash_id(var_name));
	if (iter == m_uniform_locs.end())
		glUniform3fv(init_uniform_loc(var_name), 1, vec);
	else
		glUniform3fv(iter->second, 1, vec);
	gl_err_check("nsshader::set_uniform");
}

void nsshader::set_uniform(const nsstring & var_name, const fvec2 & data)
{
	GLfloat vec[2];
	vec[0] = data.u; vec[1] = data.v;
	uniform_loc_map::iterator iter = m_uniform_locs.find(hash_id(var_name));
	if (iter == m_uniform_locs.end())
		glUniform2fv(init_uniform_loc(var_name), 1, vec);
	else
		glUniform2fv(iter->second, 1, vec);
	gl_err_check("nsshader::set_uniform");
}

void nsshader::set_uniform(const nsstring & var_name, const ivec4 & data)
{
	GLint vec[4];
	vec[0] = data.x; vec[1] = data.y; vec[2] = data.z; vec[3] = data.w;
	uniform_loc_map::iterator iter = m_uniform_locs.find(hash_id(var_name));
	if (iter == m_uniform_locs.end())
		glUniform4iv(init_uniform_loc(var_name), 1, vec);
	else
		glUniform4iv(iter->second, 1, vec);
	gl_err_check("nsshader::set_uniform");		
}

void nsshader::set_uniform(const nsstring & var_name, const ivec3 & data)
{
	GLint vec[3];
	vec[0] = data.x; vec[1] = data.y; vec[2] = data.z;
	uniform_loc_map::iterator iter = m_uniform_locs.find(hash_id(var_name));
	if (iter == m_uniform_locs.end())
		glUniform3iv(init_uniform_loc(var_name), 1, vec);
	else
		glUniform3iv(iter->second, 1, vec);
	gl_err_check("nsshader::set_uniform");		
}

void nsshader::set_uniform(const nsstring & var_name, const ivec2 & data)
{
	GLint vec[2];
	vec[0] = data.u; vec[1] = data.v;
	uniform_loc_map::iterator iter = m_uniform_locs.find(hash_id(var_name));
	if (iter == m_uniform_locs.end())
		glUniform2iv(init_uniform_loc(var_name), 1, vec);
	else
		glUniform2iv(iter->second, 1, vec);
	gl_err_check("nsshader::set_uniform");		
}

void nsshader::set_uniform(const nsstring & var_name, const uivec4 & data)
{
	GLuint vec[4];
	vec[0] = data.x; vec[1] = data.y; vec[2] = data.z; vec[3] = data.w;
	uniform_loc_map::iterator iter = m_uniform_locs.find(hash_id(var_name));
	if (iter == m_uniform_locs.end())
		glUniform4uiv(init_uniform_loc(var_name), 1, vec);
	else
		glUniform4uiv(iter->second, 1, vec);
	gl_err_check("nsshader::set_uniform");		
}

void nsshader::set_uniform(const nsstring & var_name, const uivec3 & data)
{
	GLuint vec[3];
	vec[0] = data.x; vec[1] = data.y; vec[2] = data.z;
	uniform_loc_map::iterator iter = m_uniform_locs.find(hash_id(var_name));
	if (iter == m_uniform_locs.end())
		glUniform3uiv(init_uniform_loc(var_name), 1, vec);
	else
		glUniform3uiv(iter->second, 1, vec);
	gl_err_check("nsshader::set_uniform");				
}

void nsshader::set_uniform(const nsstring & var_name, const uivec2 & data)
{
	GLuint vec[2];
	vec[0] = data.u; vec[1] = data.v;
	uniform_loc_map::iterator iter = m_uniform_locs.find(hash_id(var_name));
	if (iter == m_uniform_locs.end())
		glUniform2uiv(init_uniform_loc(var_name), 1, vec);
	else
		glUniform2uiv(iter->second, 1, vec);
	gl_err_check("nsshader::set_uniform");	
}

void nsshader::set_uniform(const nsstring & var_name, float data)
{
	uniform_loc_map::iterator iter = m_uniform_locs.find(hash_id(var_name));
	if (iter == m_uniform_locs.end())
		glUniform1f(init_uniform_loc(var_name), data);
	else
		glUniform1f(iter->second, data);
	gl_err_check("nsshader::set_uniform");
}

void nsshader::set_uniform(const nsstring & var_name, int32 data)
{
	uniform_loc_map::iterator iter = m_uniform_locs.find(hash_id(var_name));
	if (iter == m_uniform_locs.end())
		glUniform1i(init_uniform_loc(var_name), data);
	else
		glUniform1i(iter->second, data);
	gl_err_check("nsshader::set_uniform");
}

void nsshader::set_uniform(const nsstring & var_name, uint32 data)
{
	uniform_loc_map::iterator iter = m_uniform_locs.find(hash_id(var_name));
	if (iter == m_uniform_locs.end())
		glUniform1ui(init_uniform_loc(var_name), data);
	else
		glUniform1ui(iter->second, data);
	gl_err_check("nsshader::set_uniform");
}

void nsshader::release()
{
	if (m_gl_name)
	{
		glDeleteProgram(m_gl_name);
		m_gl_name = 0;
	}
	if (m_fragment.m_stage_id)
	{
		glDeleteShader(m_fragment.m_stage_id);
		m_fragment.m_stage_id = 0;
	}
	if (m_vertex.m_stage_id)
	{
		glDeleteShader(m_vertex.m_stage_id);
		m_vertex.m_stage_id = 0;
	}
	if (m_geometry.m_stage_id)
	{
		glDeleteShader(m_geometry.m_stage_id);
		m_geometry.m_stage_id = 0;
	}
	gl_err_check("nsshader::release");
}

void nsshader::reset_error()
{
	m_error_sate = error_none;
}

nsshader::xfb_mode nsshader::xfb()
{
	return m_xfb_mode;
}

nsshader::error_state nsshader::error() const
{
	return m_error_sate;
}

uint32 nsshader::attrib_loc(const nsstring & var_name) const
{
	return glGetAttribLocation(m_gl_name, var_name.c_str());
	gl_err_check("nsshader::attrib");
}

bool nsshader::_validate()
{
	glValidateProgram(m_gl_name);
	GLint worked;
    glGetProgramiv(m_gl_name, GL_VALIDATE_STATUS, &worked);
    if (!worked) 
	{
		char infoLog[1024];
        glGetProgramInfoLog(m_gl_name, sizeof(infoLog), NULL, infoLog);
		nsstring info(infoLog);
		m_error_sate = error_validation;

		#ifdef NSDEBUG
		nsstringstream ss;
		ss << "nsshader::_validate : Error validating shader with name " << m_name << "\n";
		ss << info;
		dprint(ss.str());
		#endif
        return false;
    }
	return true;
}

nsshader::shader_stage::shader_stage(shader_type pType):
	m_source(),
	m_stage_id(0),
	m_type(pType)
{}


void nsrender_shader::set_viewport(const ivec4 & vp)
{
	set_uniform("viewport", fvec4(vp.x, vp.y, vp.z, vp.w));
}

void nsrender_shader::set_proj_cam_mat(const fmat4 & pcmat)
{
	set_uniform("projCamMat", pcmat);	
}

nsmaterial_shader::nsmaterial_shader() : nsrender_shader() {}
nsmaterial_shader::~nsmaterial_shader(){}

void nsmaterial_shader::init_uniforms()
{
	set_uniform("diffuseMap", DIFFUSE_TEX_UNIT);
	set_uniform("normalMap", NORMAL_TEX_UNIT);
	set_uniform("opacityMap", OPACITY_TEX_UNIT);
	set_uniform("heightMap", HEIGHT_TEX_UNIT);
	nsrender_shader::init_uniforms();
}

void nsmaterial_shader::set_for_draw_call(draw_call * drawc)
{
	instanced_draw_call * dc = static_cast<instanced_draw_call*>(drawc);

	set_uniform("hasHeightMap", drawc->mat->contains(nsmaterial::height));
	set_uniform("hasDiffuseMap", drawc->mat->contains(nsmaterial::diffuse));
	set_uniform("hasOpacityMap", drawc->mat->contains(nsmaterial::opacity));
	set_uniform("hasNormalMap", drawc->mat->contains(nsmaterial::normal));
	set_uniform("colorMode", drawc->mat->color_mode());
	set_uniform("fragColOut", drawc->mat->color());
	set_uniform("force_alpha", drawc->mat->using_alpha_from_color());
	set_uniform("material_id",  drawc->mat_index);

	uint32 ent_id = dc->entity_id;
	if (dc->transparent_picking)
		ent_id = 0;

	set_uniform("hminmax", dc->height_minmax);
	set_uniform("entityID", ent_id);
	set_uniform("pluginID", dc->plugin_id);

	if (dc->submesh->m_node != NULL)
		set_uniform("nodeTransform", dc->submesh->m_node->m_world_tform);
	else
		set_uniform("nodeTransform", fmat4());

	if (!dc->submesh->m_has_tex_coords)
		set_uniform("colorMode", true);

//	set_uniform("projCamMat", dc->proj_cam);

	if (dc->anim_transforms != NULL)
	{
		set_uniform("hasBones", true);
		for (uint32 i = 0; i < dc->anim_transforms->size(); ++i)
			set_uniform("boneTransforms[" + std::to_string(i) + "]", (*dc->anim_transforms)[i]);
	}
	else
		set_uniform("hasBones", false);	
}

nslight_shader::nslight_shader():nsrender_shader() {}
nslight_shader::~nslight_shader() {}

void nslight_shader::init_uniforms()
{
	set_uniform("shadowMap", SHADOW_TEX_UNIT);
	set_uniform("gDiffuseMap", G_DIFFUSE_TEX_UNIT);
	set_uniform("gNormalMap", G_NORMAL_TEX_UNIT);
	set_uniform("gWorldPosMap", G_WORLD_POS_TEX_UNIT);
	set_uniform("gMatMap", G_PICKING_TEX_UNIT);
	set_uniform("epsilon", DEFAULT_SHADOW_EPSILON);
	nsrender_shader::init_uniforms();
}

void nslight_shader::set_for_draw_call(draw_call *dc)
{
	light_draw_call * ldc = (light_draw_call*)dc;

	set_uniform("light.diffuseIntensity", ldc->diffuse_intensity);
	set_uniform("light.ambientIntensity", ldc->ambient_intensity);
	set_uniform("castShadows", ldc->cast_shadows);
	set_uniform("light.color", ldc->light_color);
	set_uniform("shadowSamples", ldc->shadow_samples);
	set_uniform("light.shadowDarkness", ldc->shadow_darkness);
	set_uniform("shadowTexSize", fvec2(ldc->shadow_tex_size.x, ldc->shadow_tex_size.y));

	nsstring id;
	id.reserve(64);
	auto iter = ldc->material_ids->begin();
	while (iter != ldc->material_ids->end())
	{
		id = "materials[" + std::to_string(iter->second) + "].spec_";
		set_uniform(id+"color",iter->first->specular_color());
		set_uniform(id+"power",iter->first->specular_power());
		set_uniform(id+"intensity",iter->first->specular_intensity());
		++iter;
	}
}

nsfragment_sort_shader::nsfragment_sort_shader(): nsrender_shader()
{}

nsfragment_sort_shader::~nsfragment_sort_shader()
{}

void nsfragment_sort_shader::init_uniforms()
{
	set_uniform("gMatMap", int32(G_PICKING_TEX_UNIT));
	nsrender_shader::init_uniforms();
}

nsdir_light_shader::nsdir_light_shader() :nslight_shader() {}

nsdir_light_shader::~nsdir_light_shader() {}

void nsdir_light_shader::init_uniforms()
{
	nslight_shader::init_uniforms();
}

void nsdir_light_shader::set_for_draw_call(draw_call * dc)
{
	light_draw_call * ldc = (light_draw_call*)dc;
	set_uniform("projLightMat", ldc->proj_light_mat);
	set_uniform("bgColor", ldc->bg_color);
	set_uniform("light.direction", ldc->direction);
	nslight_shader::set_for_draw_call(dc);
}

nspoint_light_shader::nspoint_light_shader() :nslight_shader() {}

nspoint_light_shader::~nspoint_light_shader() {}

void nspoint_light_shader::init_uniforms()
{
	nslight_shader::init_uniforms();
}

void nspoint_light_shader::set_for_draw_call(draw_call * dc)
{
	light_draw_call * ldc = (light_draw_call*)dc;
	set_uniform("light.attConstant", ldc->spot_atten.x);
	set_uniform("light.attLinear", ldc->spot_atten.y);
	set_uniform("light.attExp", ldc->spot_atten.z);
	set_uniform("light.position", ldc->light_pos);
	set_uniform("maxDepth", ldc->max_depth);
	set_uniform("transform", ldc->light_transform);
//	set_uniform("projCamMat", ldc->proj_cam_mat);
	if (ldc->submesh->m_node != nullptr)
		set_uniform("nodeTransform", ldc->submesh->m_node->m_world_tform);
	else
		set_uniform("nodeTransform", fmat4());
	nslight_shader::set_for_draw_call(dc);
}

nsspot_light_shader::nsspot_light_shader() :nspoint_light_shader() {}

nsspot_light_shader::~nsspot_light_shader() {}

void nsspot_light_shader::init_uniforms()
{
	nspoint_light_shader::init_uniforms();
}

void nsspot_light_shader::set_for_draw_call(draw_call * dc)
{
	light_draw_call * ldc = (light_draw_call*)dc;
	set_uniform("projLightMat", ldc->proj_light_mat);
	set_uniform("light.direction", ldc->direction);
	set_uniform("light.cutoff", ldc->cutoff);
	nspoint_light_shader::set_for_draw_call(dc);
}

nsparticle_process_shader::nsparticle_process_shader() : nsshader() {}
nsparticle_process_shader::~nsparticle_process_shader() {}

void nsparticle_process_shader::init_uniforms()
{
	set_random_sampler(RAND_TEX_UNIT);
	set_dt(0);
	set_elapsed(0);
	set_lifetime(0);
	set_launch_freq(0);
	set_angular_vel(0);
	set_motion_key_global(false);
	set_visual_key_global(false);
	set_interpolated_motion_keys(false);
	set_interpolated_visual_keys(false);
	set_starting_size(fvec2());
	set_emitter_size(fvec3());
	set_emitter_shape(0);
	set_initial_vel_mult(fvec3());
}

void nsparticle_process_shader::init() 
{
	std::vector<nsstring> outLocs2;
	outLocs2.push_back("gPosOut");
	outLocs2.push_back("gVelOut");
	outLocs2.push_back("gScaleAndAngleOut");
	outLocs2.push_back("gAgeOut");
	set_xfb(nsshader::xfb_interleaved, &outLocs2);
}

void nsparticle_process_shader::set_random_sampler(int32 sampler)
{
	set_uniform("randomTex", sampler);
}

void nsparticle_process_shader::set_dt(float dt)
{
	set_uniform("dt", dt);
}

void nsparticle_process_shader::set_elapsed(float elapsed)
{
	set_uniform("timeElapsed", elapsed);
}

void nsparticle_process_shader::set_lifetime(uint32 lifetime)
{
	set_uniform("lifetime", lifetime);
}

void nsparticle_process_shader::set_launch_freq(float freq)
{
	set_uniform("launchFrequency", freq);
}

void nsparticle_process_shader::set_angular_vel(int32 angVelocity)
{
	set_uniform("angVelocity", angVelocity);
}

void nsparticle_process_shader::set_motion_key_global(bool global)
{
	set_uniform("motionGlobal", global);
}

void nsparticle_process_shader::set_visual_key_global(bool global)
{
	set_uniform("visualGlobal", global);
}

void nsparticle_process_shader::set_interpolated_motion_keys(bool interp)
{
	set_uniform("interpolateMotion", interp);
}

void nsparticle_process_shader::set_interpolated_visual_keys(bool interp)
{
	set_uniform("interpolateVisual", interp);
}

void nsparticle_process_shader::set_starting_size(const fvec2 & size)
{
	set_uniform("startingSize", size);
}

void nsparticle_process_shader::set_emitter_size(const fvec3 & size)
{
	set_uniform("emitterSize", size);
}

void nsparticle_process_shader::set_emitter_shape(uint32 shape)
{
	set_uniform("emitterShape", shape);
}

void nsparticle_process_shader::set_initial_vel_mult(const fvec3 & mult)
{
	set_uniform("initVelocityMult", mult);
}

void nsparticle_process_shader::set_motion_keys(const ui_fvec3_map & keys, uint32 maxKeys, uint32 lifetime)
{
	uint32 index = 0;
	ui_fvec3_map::const_iterator keyIter = keys.begin();
	while (keyIter != keys.end())
	{
		set_uniform("forceKeys[" + std::to_string(index) + "].time", float(keyIter->first) / float(maxKeys * 1000) * float(lifetime));
		set_uniform("forceKeys[" + std::to_string(index) + "].force", keyIter->second);
		++index;
		++keyIter;
	}
}

void nsparticle_process_shader::set_visual_keys(const ui_fvec3_map & keys, uint32 maxKeys, uint32 lifetime)
{
	uint32 index = 0;
	ui_fvec3_map::const_iterator keyIter = keys.begin();
	while (keyIter != keys.end())
	{
		set_uniform("sizeKeys[" + std::to_string(index) + "].time", float(keyIter->first) / float(maxKeys * 1000) * float(lifetime));
		set_uniform("sizeKeys[" + std::to_string(index) + "].sizeVel", fvec2(keyIter->second.x, keyIter->second.y));
		set_uniform("sizeKeys[" + std::to_string(index) + "].alpha", keyIter->second.z);
		++index;
		++keyIter;
	}
}

nsparticle_render_shader::nsparticle_render_shader(): nsshader() {}
nsparticle_render_shader::~nsparticle_render_shader() {}

void nsparticle_render_shader::init_uniforms()
{
	set_diffuse_sampler(DIFFUSE_TEX_UNIT);
	set_color_mode(false);
	set_frag_color_out(fvec4());
	set_diffusemap_enabled(false);
	set_lifetime(0.0f);
	set_blend_mode(0);
	set_proj_cam_mat(fmat4());
	set_cam_up(fvec3());
	set_cam_right(fvec3());
	set_cam_target(fvec3());
	set_world_up(fvec3());
}

void nsparticle_render_shader::init() {}

void nsparticle_render_shader::set_diffuse_sampler(int32 sampler)
{
	set_uniform("diffuseMap", sampler);
}

void nsparticle_render_shader::set_color_mode(bool enable)
{
	set_uniform("colorMode", enable);
}

void nsparticle_render_shader::set_frag_color_out(const fvec4 & col)
{
	set_uniform("fragColOut", col);
}

void nsparticle_render_shader::set_diffusemap_enabled(bool enable)
{
	set_uniform("hasDiffuseMap", enable);
}

void nsparticle_render_shader::set_lifetime(float seconds)
{
	set_uniform("lifetimeSecs", seconds);
}

void nsparticle_render_shader::set_blend_mode(uint32 mode)
{
	set_uniform("blendMode", mode);
}

void nsparticle_render_shader::set_proj_cam_mat(const fmat4 & mat)
{
	set_uniform("projCamMat", mat);
}

void nsparticle_render_shader::set_cam_up(const fvec3 & vec)
{
	set_uniform("camUp", vec);
}

void nsparticle_render_shader::set_cam_right(const fvec3 & vec)
{
	set_uniform("rightVec", vec);
}

void nsparticle_render_shader::set_cam_target(const fvec3 & vec)
{
	set_uniform("camTarget", vec);
}

void nsparticle_render_shader::set_world_up(const fvec3 & vec)
{
	set_uniform("worldUp", vec);
}

nsshadow_2dmap_shader::nsshadow_2dmap_shader() : nsrender_shader(){}
nsshadow_2dmap_shader::~nsshadow_2dmap_shader() {}

void nsshadow_2dmap_shader::init_uniforms()
{
	set_uniform("heightMap", HEIGHT_TEX_UNIT);
	nsrender_shader::init_uniforms();
}

void nsshadow_2dmap_shader::set_for_draw_call(draw_call * dc)
{
	instanced_draw_call * idc = dynamic_cast<instanced_draw_call*>(dc);
	if (idc->submesh->m_node != nullptr)
		set_uniform("nodeTransform", idc->submesh->m_node->m_world_tform);
	else
		set_uniform("nodeTransform", fmat4());

	if (idc->anim_transforms != nullptr)
	{
		for (uint32 i = 0; i < idc->anim_transforms->size(); ++i)
			set_uniform("boneTransforms[" + std::to_string(i) + "]", (*idc->anim_transforms)[i]);
		set_uniform("hasBones", true);
	}
	else
	{
		set_uniform("hasBones", false);
	}
	set_uniform("hasHeightMap", idc->mat->contains(nsmaterial::height));
	set_uniform("hminmax", idc->height_minmax);
}

void nsshadow_2dmap_shader::set_for_light_draw_call(light_draw_call * dc)
{
	set_uniform("projMat", dc->proj_light_mat);	
}

nsshadow_cubemap_shader::nsshadow_cubemap_shader() :nsshadow_2dmap_shader() {}
nsshadow_cubemap_shader::~nsshadow_cubemap_shader() {}

void nsshadow_cubemap_shader::init_uniforms()
{
	nsshadow_2dmap_shader::init_uniforms();
}

void nsshadow_cubemap_shader::set_for_light_draw_call(light_draw_call * dc)
{
	fmat4 invt;
	set_uniform("lightPos", dc->light_pos);
	set_uniform("maxDepth", dc->max_depth);
	nsshadow_2dmap_shader::set_for_light_draw_call(dc);
}

nslight_stencil_shader::nslight_stencil_shader(): nsrender_shader()
{}
nslight_stencil_shader::~nslight_stencil_shader()
{}

void nslight_stencil_shader::init_uniforms()
{
	nsrender_shader::init_uniforms();
}

void nslight_stencil_shader::set_for_draw_call(draw_call * dc)
{
	light_draw_call * ldc = (light_draw_call*)dc;
	if (ldc->submesh->m_node != nullptr)
		set_uniform("nodeTransform", ldc->submesh->m_node->m_world_tform);
	else
		set_uniform("nodeTransform", fmat4());
	set_uniform("transform", ldc->light_transform);
//	set_uniform("projCamMat", ldc->proj_cam_mat);
}

nsselection_shader::nsselection_shader() : nsrender_shader() {}
nsselection_shader::~nsselection_shader() {}

void nsselection_shader::init_uniforms()
{
	set_uniform("heightMap", HEIGHT_TEX_UNIT);
	nsrender_shader::init_uniforms();	
}

void nsselection_shader::set_for_draw_call(draw_call * dc)
{
	instanced_draw_call * idc = (instanced_draw_call*)dc;
	if (idc->submesh->m_node != nullptr)
		set_uniform("nodeTransform", idc->submesh->m_node->m_world_tform);
	else
		set_uniform("nodeTransform", fmat4());

	if (idc->anim_transforms != nullptr)
	{
		for (uint32 i = 0; i < idc->anim_transforms->size(); ++i)
			set_uniform("boneTransforms[" + std::to_string(i) + "]", (*idc->anim_transforms)[i]);
		set_uniform("hasBones", true);
	}
	else
	{
		set_uniform("hasBones", false);
	}
//	set_uniform("projCamMat", idc->proj_cam);
	set_uniform("hasHeightMap", idc->mat->contains(nsmaterial::height));
	set_uniform("hminmax", idc->height_minmax);
}

void nsselection_shader::set_border_color(const fvec3 & rgb)
{
	set_uniform("fragColOut", fvec4(rgb,1.0f));	
}

void nsselection_shader::set_main_color(const fvec4 & rgba)
{
	set_uniform("fragColOut", rgba);		
}

