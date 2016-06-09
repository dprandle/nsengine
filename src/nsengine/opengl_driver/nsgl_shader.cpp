
#include <nsmath.h>
#include <nsgl_shader.h>
#include <nsgl_driver.h>

nsgl_shader::nsgl_shader():
	error_sate(error_none),
	xfb(xfb_interleaved),
	stages(),
	xfb_locs(),
	uniform_locs()
{
	stages[0].stage = vertex_shader;
	stages[1].stage = tess_control_shader;
	stages[2].stage = tess_evaluation_shader;
	stages[3].stage = geometry_shader;
	stages[4].stage = fragment_shader;
	stages[5].stage = compute_shader;
}

nsgl_shader::~nsgl_shader()
{}

bool nsgl_shader::compile()
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
		dprint("nsgl_shader::compile - Succesfully compiled shader " + m_name);
	}
	
	return ret;
}

bool nsgl_shader::compile(shader_stage stage, const nsstring & source)
{
	shader_stage_info * si = stage_info(stage);
	if (si->gl_id != 0)
		glDeleteShader(si->gl_id);

	si->gl_id = glCreateShader(si->stage);
	gl_err_check("nsgl_shader::compile Error creating shader");
	// create arguements for glShaderSource
	const char* c_str = _stage(stage).m_source.c_str();

	glShaderSource(gl_id(stage), 1, &c_str, NULL); // Attach source to shader
	gl_err_check("nsgl_shader::compile error setting shader source");

	glCompileShader(gl_id(stage));
	gl_err_check("nsgl_shader::compile error compiling shader");

	GLint worked;
	glGetShaderiv(gl_id(stage), GL_COMPILE_STATUS, &worked);
	if (!worked)
	{
		char infoLog[1024];
		glGetShaderInfoLog(gl_id(stage), sizeof(infoLog), NULL, (GLchar*)infoLog);
		nsstring info(infoLog);
		m_error_sate = error_compile;

#ifdef NSDEBUG
		nsstringstream ss;
		ss << "nsgl_shader::compile : Error compiling shader stage " << stage_name(stage) << " from " << m_name << "\n";
			ss << info;
		dprint(ss.str());
#endif

		glDeleteShader(gl_id(stage));
		gl_err_check("nsgl_shader::compile error deleting shader");
		set_gl_id(0, stage);
		return false;
	}
	dprint("nsgl_shader::compile - Succesfully compiled shader stage " + stage_name(stage) + " from " + m_name);
	return true;
}

bool nsgl_shader::compiled(shader_stage stage)
{
	return (_stage(stage).m_stage_id != 0);
}

bool nsgl_shader::linked()
{
	return (gl_id != 0);
}

void nsgl_shader::set_source(shader_stage stage, const nsstring & source)
{
	_stage(stage).m_source = source;
}

void nsgl_shader::video_init()
{
	if (gl_id != 0)
		return;
	nsgl_object::set_gl_id(glCreateProgram());
	gl_err_check("nsgl_shader::link Error creating program");
}

uint32 nsgl_shader::init_uniform_loc(const nsstring & var_name)
{
	uint32 loc = glGetUniformLocation(gl_id, var_name.c_str());
	gl_err_check("nsgl_shader::init_uniform_loc");
	m_uniform_locs[hash_id(var_name)] = loc;
	return loc;
}

bool nsgl_shader::link()
{
	if (gl_id == 0)
	{
		m_error_sate = error_program;
		return false;
	}

	if (gl_id(fragment_shader) != 0)
		glAttachShader(gl_id, gl_id(fragment_shader));
	if (gl_id(vertex_shader) != 0)
		glAttachShader(gl_id, gl_id(vertex_shader));
	if (gl_id(geometry_shader) != 0)
		glAttachShader(gl_id, gl_id(geometry_shader));

	gl_err_check("nsgl_shader::link Error attaching program");
	if (!m_xfb_locs.empty())
		_setup_xfb();

	glLinkProgram(gl_id);
	gl_err_check("nsgl_shader::link Error linking program");
	if (gl_id(fragment_shader) != 0)
		glDetachShader(gl_id, gl_id(fragment_shader));
	if (gl_id(vertex_shader) != 0)
		glDetachShader(gl_id, gl_id(vertex_shader));
	if (gl_id(geometry_shader) != 0)
		glDetachShader(gl_id, gl_id(geometry_shader));

	gl_err_check("nsgl_shader::link Error detaching program");
	GLint worked;
	glGetProgramiv(gl_id, GL_LINK_STATUS, &worked);
	if (!worked)
	{
		char infoLog[1024];
		glGetProgramInfoLog(gl_id, sizeof(infoLog), NULL,(GLchar*)infoLog);
		nsstring info(infoLog);
		m_error_sate = error_link;
		#ifdef NSDEBUG
		nsstringstream ss;
		ss << "Error linking shader with name " << m_name << "\n";
		ss << info;
		dprint(ss.str());
		#endif
		glDeleteProgram(gl_id);
		gl_err_check("nsgl_shader::link Error deleting program");
		return false;
	}
	dprint("nsgl_shader::link - Succesfully linked shader " + m_name);
	/*Validate may fail on older GPUS - add exceptions here!!*/
	return _validate();
}

nsstring nsgl_shader::stage_name(shader_stage stage)
{
	switch (stage)
	{
	  case(vertex_shader) :
		  return "vertex";
	  case (tess_control_shader):
		  return "tess_control";
	  case (tess_evaluation_shader):
		  return "tess_evaluation";
	  case(geometry_shader) :
		  return "geometry";
	  case(fragment_shader) :
		  return "fragment";
	  case (compute_shader):
		  return "compute";
	}
}

nsgl_shader::shader_stage_info * nsgl_shader::stage_info(shader_stage stage)
{
	switch (stage)
	{
	  case(vertex_shader) :
		  return &stages[0];
	  case (tess_control_shader):
		  return &stages[1];
	  case (tess_evaluation_shader):
		  return &stages[2];
	  case(geometry_shader) :
		  return &stages[3];
	  case(fragment_shader) :
		  return &stages[4];
	  case (compute_shader):
		  return &stages[5];
	}
}

void nsgl_shader::_setup_xfb()
{
	GLchar ** varyings = new GLchar*[m_xfb_locs.size()];
	for (uint32 i = 0; i < m_xfb_locs.size(); ++i)
	{
		GLchar * str = new GLchar[m_xfb_locs[i].size() + 1];
		strcpy(str, m_xfb_locs[i].c_str());
		varyings[i] = str;
	}

 	glTransformFeedbackVaryings(gl_id, 4, varyings, xfb);
	gl_err_check("nsgl_shader::_setup_xfb()");

	for (uint32 i = 0; i < m_xfb_locs.size(); ++i)
		delete[] varyings[i];
	delete[] varyings;
}

void nsgl_shader::bind() const
{
	glUseProgram(gl_id);
	gl_err_check("nsgl_shader::bind");
}

void nsgl_shader::unbind() const
{
	glUseProgram(0);
	gl_err_check("nsgl_shader::bind");
}

void nsgl_shader::set_uniform(const nsstring & var_name, const fmat4 & data)
{
	fmat4 mat = data;
	uniform_loc_map::iterator iter = m_uniform_locs.find(hash_id(var_name));
	if (iter == m_uniform_locs.end())
		glUniformMatrix4fv(init_uniform_loc(var_name), 1, GL_TRUE, mat.data_ptr());
	else
		glUniformMatrix4fv(iter->second, 1, GL_TRUE, mat.data_ptr());
	gl_err_check("nsgl_shader::set_uniform");
}

void nsgl_shader::set_uniform(const nsstring & var_name, const fmat3 & data)
{
	fmat3 mat = data;
	uniform_loc_map::iterator iter = m_uniform_locs.find(hash_id(var_name));
	if (iter == m_uniform_locs.end())
		glUniformMatrix3fv(init_uniform_loc(var_name), 1, GL_TRUE, mat.data_ptr());
	else
		glUniformMatrix3fv(iter->second, 1, GL_TRUE, mat.data_ptr());
	gl_err_check("nsgl_shader::set_uniform");
}

void nsgl_shader::set_uniform(const nsstring & var_name, const fmat2 & data)
{
	fmat2 mat(data);
	uniform_loc_map::iterator iter = m_uniform_locs.find(hash_id(var_name));
	if (iter == m_uniform_locs.end())
		glUniformMatrix2fv(init_uniform_loc(var_name), 1, GL_TRUE, mat.data_ptr());
	else
		glUniformMatrix2fv(iter->second, 1, GL_TRUE, mat.data_ptr());
	gl_err_check("nsgl_shader::set_uniform");	
}

void nsgl_shader::set_uniform(const nsstring & var_name, bool val)
{
	set_uniform(var_name, int32(val));
}

void nsgl_shader::set_uniform(const nsstring & var_name, const fvec4 & data)
{
	GLfloat vec[4];
	vec[0] = data.x; vec[1] = data.y; vec[2] = data.z; vec[3] = data.w;
	uniform_loc_map::iterator iter = m_uniform_locs.find(hash_id(var_name));
	if (iter == m_uniform_locs.end())
		glUniform4fv(init_uniform_loc(var_name), 1, vec);
	else
		glUniform4fv(iter->second, 1, vec);
	gl_err_check("nsgl_shader::set_uniform");
}

void nsgl_shader::set_uniform(const nsstring & var_name, const fvec3 & data)
{
	GLfloat vec[3];
	vec[0] = data.x; vec[1] = data.y; vec[2] = data.z;
	uniform_loc_map::iterator iter = m_uniform_locs.find(hash_id(var_name));
	if (iter == m_uniform_locs.end())
		glUniform3fv(init_uniform_loc(var_name), 1, vec);
	else
		glUniform3fv(iter->second, 1, vec);
	gl_err_check("nsgl_shader::set_uniform");
}

void nsgl_shader::set_uniform(const nsstring & var_name, const fvec2 & data)
{
	GLfloat vec[2];
	vec[0] = data.u; vec[1] = data.v;
	uniform_loc_map::iterator iter = m_uniform_locs.find(hash_id(var_name));
	if (iter == m_uniform_locs.end())
		glUniform2fv(init_uniform_loc(var_name), 1, vec);
	else
		glUniform2fv(iter->second, 1, vec);
	gl_err_check("nsgl_shader::set_uniform");
}

void nsgl_shader::set_uniform(const nsstring & var_name, const ivec4 & data)
{
	GLint vec[4];
	vec[0] = data.x; vec[1] = data.y; vec[2] = data.z; vec[3] = data.w;
	uniform_loc_map::iterator iter = m_uniform_locs.find(hash_id(var_name));
	if (iter == m_uniform_locs.end())
		glUniform4iv(init_uniform_loc(var_name), 1, vec);
	else
		glUniform4iv(iter->second, 1, vec);
	gl_err_check("nsgl_shader::set_uniform");		
}

void nsgl_shader::set_uniform(const nsstring & var_name, const ivec3 & data)
{
	GLint vec[3];
	vec[0] = data.x; vec[1] = data.y; vec[2] = data.z;
	uniform_loc_map::iterator iter = m_uniform_locs.find(hash_id(var_name));
	if (iter == m_uniform_locs.end())
		glUniform3iv(init_uniform_loc(var_name), 1, vec);
	else
		glUniform3iv(iter->second, 1, vec);
	gl_err_check("nsgl_shader::set_uniform");		
}

void nsgl_shader::set_uniform(const nsstring & var_name, const ivec2 & data)
{
	GLint vec[2];
	vec[0] = data.u; vec[1] = data.v;
	uniform_loc_map::iterator iter = m_uniform_locs.find(hash_id(var_name));
	if (iter == m_uniform_locs.end())
		glUniform2iv(init_uniform_loc(var_name), 1, vec);
	else
		glUniform2iv(iter->second, 1, vec);
	gl_err_check("nsgl_shader::set_uniform");		
}

void nsgl_shader::set_uniform(const nsstring & var_name, const uivec4 & data)
{
	GLuint vec[4];
	vec[0] = data.x; vec[1] = data.y; vec[2] = data.z; vec[3] = data.w;
	uniform_loc_map::iterator iter = m_uniform_locs.find(hash_id(var_name));
	if (iter == m_uniform_locs.end())
		glUniform4uiv(init_uniform_loc(var_name), 1, vec);
	else
		glUniform4uiv(iter->second, 1, vec);
	gl_err_check("nsgl_shader::set_uniform");		
}

void nsgl_shader::set_uniform(const nsstring & var_name, const uivec3 & data)
{
	GLuint vec[3];
	vec[0] = data.x; vec[1] = data.y; vec[2] = data.z;
	uniform_loc_map::iterator iter = m_uniform_locs.find(hash_id(var_name));
	if (iter == m_uniform_locs.end())
		glUniform3uiv(init_uniform_loc(var_name), 1, vec);
	else
		glUniform3uiv(iter->second, 1, vec);
	gl_err_check("nsgl_shader::set_uniform");				
}

void nsgl_shader::set_uniform(const nsstring & var_name, const uivec2 & data)
{
	GLuint vec[2];
	vec[0] = data.u; vec[1] = data.v;
	uniform_loc_map::iterator iter = m_uniform_locs.find(hash_id(var_name));
	if (iter == m_uniform_locs.end())
		glUniform2uiv(init_uniform_loc(var_name), 1, vec);
	else
		glUniform2uiv(iter->second, 1, vec);
	gl_err_check("nsgl_shader::set_uniform");	
}

void nsgl_shader::set_uniform(const nsstring & var_name, float data)
{
	uniform_loc_map::iterator iter = m_uniform_locs.find(hash_id(var_name));
	if (iter == m_uniform_locs.end())
		glUniform1f(init_uniform_loc(var_name), data);
	else
		glUniform1f(iter->second, data);
	gl_err_check("nsgl_shader::set_uniform");
}

void nsgl_shader::set_uniform(const nsstring & var_name, int32 data)
{
	uniform_loc_map::iterator iter = m_uniform_locs.find(hash_id(var_name));
	if (iter == m_uniform_locs.end())
		glUniform1i(init_uniform_loc(var_name), data);
	else
		glUniform1i(iter->second, data);
	gl_err_check("nsgl_shader::set_uniform");
}

void nsgl_shader::set_uniform(const nsstring & var_name, uint32 data)
{
	uniform_loc_map::iterator iter = m_uniform_locs.find(hash_id(var_name));
	if (iter == m_uniform_locs.end())
		glUniform1ui(init_uniform_loc(var_name), data);
	else
		glUniform1ui(iter->second, data);
	gl_err_check("nsgl_shader::set_uniform");
}

void nsgl_shader::video_release()
{
	if (gl_id)
	{
		glDeleteProgram(gl_id);
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
	gl_err_check("nsgl_shader::release");
}

void nsgl_shader::reset_error()
{
	m_error_sate = error_none;
}

nsgl_shader::xfb_mode nsgl_shader::xfb()
{
	return m_xfb_mode;
}

nsgl_shader::error_state nsgl_shader::error() const
{
	return m_error_sate;
}

uint32 nsgl_shader::attrib_loc(const nsstring & var_name) const
{
	return glGetAttribLocation(gl_id, var_name.c_str());
	gl_err_check("nsgl_shader::attrib");
}

bool nsgl_shader::_validate()
{
	glValidateProgram(gl_id);
	GLint worked;
    glGetProgramiv(gl_id, GL_VALIDATE_STATUS, &worked);
    if (!worked) 
	{
		char infoLog[1024];
        glGetProgramInfoLog(gl_id, sizeof(infoLog), NULL, infoLog);
		nsstring info(infoLog);
		m_error_sate = error_validation;

		#ifdef NSDEBUG
		nsstringstream ss;
		ss << "nsgl_shader::_validate : Error validating shader with name " << m_name << "\n";
		ss << info;
		dprint(ss.str());
		#endif
        return false;
    }
	return true;
}
