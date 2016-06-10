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
#include <nsgl_driver.h>

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
	video_init();
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
	video_init();
}

nsshader::~nsshader()
{
	video_release();
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
		glDeleteShader(gl_id(type));

	set_gl_id(glCreateShader(type), type);
	gl_err_check("nsshader::compile error creating shader");
	// create arguements for glShaderSource
	const char* c_str = _stage(type).m_source.c_str();

	glShaderSource(gl_id(type), 1, &c_str, NULL); // Attach source to shader
	gl_err_check("nsshader::compile error setting shader source");

	glCompileShader(gl_id(type));
	gl_err_check("nsshader::compile error compiling shader");

	GLint worked;
	glGetShaderiv(gl_id(type), GL_COMPILE_STATUS, &worked);
	if (!worked)
	{
		char infoLog[1024];
		glGetShaderInfoLog(gl_id(type), sizeof(infoLog), NULL, (GLchar*)infoLog);
		nsstring info(infoLog);
		m_error_sate = error_compile;

#ifdef NSDEBUG
		nsstringstream ss;
		ss << "nsshader::compile : Error compiling shader stage " << stage_name(type) << " from " << m_name << "\n";
			ss << info;
		dprint(ss.str());
#endif

		glDeleteShader(gl_id(type));
		gl_err_check("nsshader::compile error deleting shader");
		set_gl_id(0, type);
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
	return (nsgl_object::gl_id() != 0);
}

void nsshader::set_source(shader_type type, const nsstring & source)
{
	_stage(type).m_source = source;
}

void nsshader::video_init()
{
	if (nsgl_object::gl_id() != 0)
		return;
	nsgl_object::set_gl_id(glCreateProgram());
	gl_err_check("nsshader::link Error creating program");
}

uint32 nsshader::gl_id(shader_type type)
{
	nsgl_driver * gl_driver = static_cast<nsgl_driver*>(nse.video_driver());
	return _stage(type).m_stage_id[gl_driver->current_context()->context_id];
}

void nsshader::set_gl_id(uint32 id, shader_type type)
{
	nsgl_driver * gl_driver = static_cast<nsgl_driver*>(nse.video_driver());
	_stage(type).m_stage_id[gl_driver->current_context()->context_id] = id;	
}

uint32 nsshader::init_uniform_loc(const nsstring & var_name)
{
	uint32 loc = glGetUniformLocation(nsgl_object::gl_id(), var_name.c_str() );
	gl_err_check("nsshader::initUniformLoc");
	m_uniform_locs[hash_id(var_name)] = loc;
	return loc;
}

bool nsshader::link()
{
	if (nsgl_object::gl_id() == 0)
	{
		m_error_sate = error_program;
		return false;
	}

	if (gl_id(fragment_shader) != 0)
		glAttachShader(nsgl_object::gl_id(), gl_id(fragment_shader));
	if (gl_id(vertex_shader) != 0)
		glAttachShader(nsgl_object::gl_id(), gl_id(vertex_shader));
	if (gl_id(geometry_shader) != 0)
		glAttachShader(nsgl_object::gl_id(), gl_id(geometry_shader));

	gl_err_check("nsshader::link Error attaching program");
	if (!m_xfb_locs.empty())
		_setup_xfb();

	glLinkProgram(nsgl_object::gl_id());
	gl_err_check("nsshader::link Error linking program");
	if (gl_id(fragment_shader) != 0)
		glDetachShader(nsgl_object::gl_id(), gl_id(fragment_shader));
	if (gl_id(vertex_shader) != 0)
		glDetachShader(nsgl_object::gl_id(), gl_id(vertex_shader));
	if (gl_id(geometry_shader) != 0)
		glDetachShader(nsgl_object::gl_id(), gl_id(geometry_shader));

	gl_err_check("nsshader::link Error detaching program");
	GLint worked;
	glGetProgramiv(nsgl_object::gl_id(), GL_LINK_STATUS, &worked);
	if (!worked)
	{
		char infoLog[1024];
		glGetProgramInfoLog(nsgl_object::gl_id(), sizeof(infoLog), NULL,(GLchar*)infoLog);
		nsstring info(infoLog);
		m_error_sate = error_link;
		#ifdef NSDEBUG
		nsstringstream ss;
		ss << "Error linking shader with name " << m_name << "\n";
		ss << info;
		dprint(ss.str());
		#endif
		glDeleteProgram(nsgl_object::gl_id());
		gl_err_check("nsshader::link Error deleting program");
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

 	glTransformFeedbackVaryings(nsgl_object::gl_id(), 4, varyings, m_xfb_mode);
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
	glUseProgram(nsgl_object::gl_id());
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

void nsshader::set_uniform(const nsstring & var_name, const fmat2 & data)
{
	fmat2 mat(data);
	uniform_loc_map::iterator iter = m_uniform_locs.find(hash_id(var_name));
	if (iter == m_uniform_locs.end())
		glUniformMatrix2fv(init_uniform_loc(var_name), 1, GL_TRUE, mat.data_ptr());
	else
		glUniformMatrix2fv(iter->second, 1, GL_TRUE, mat.data_ptr());
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

void nsshader::video_release()
{
	if (nsgl_object::gl_id())
	{
		glDeleteProgram(nsgl_object::gl_id());
		nsgl_object::set_gl_id(0);
	}
	if (m_fragment.m_stage_id)
	{
		glDeleteShader(gl_id(fragment_shader));
		set_gl_id(0, fragment_shader);
	}
	if (m_vertex.m_stage_id)
	{
		glDeleteShader(gl_id(vertex_shader));
		set_gl_id(0, vertex_shader);
	}
	if (m_geometry.m_stage_id)
	{
		glDeleteShader(gl_id(geometry_shader));
		set_gl_id(0, geometry_shader);
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
	return glGetAttribLocation(nsgl_object::gl_id(), var_name.c_str());
	gl_err_check("nsshader::attrib");
}

bool nsshader::_validate()
{
	glValidateProgram(nsgl_object::gl_id());
	GLint worked;
    glGetProgramiv(nsgl_object::gl_id(), GL_VALIDATE_STATUS, &worked);
    if (!worked) 
	{
		char infoLog[1024];
        glGetProgramInfoLog(nsgl_object::gl_id(), sizeof(infoLog), NULL, infoLog);
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
	m_type(pType)
{
	for (uint32 i = 0; i < MAX_CONTEXT_COUNT; ++i)
		m_stage_id[i] = 0;
}
