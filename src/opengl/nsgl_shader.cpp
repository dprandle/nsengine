#include <math/nsmath.h>
#include <opengl/nsgl_shader.h>
#include <opengl/nsgl_driver.h>
#include <string.h>
#include <nsengine.h>

nsgl_shader::nsgl_shader():
	error_state(error_none),
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

bool nsgl_shader::compile(shader_stage stage, const nsstring & source)
{
	shader_stage_info * si = stage_info(stage);
	if (si->gl_id != 0)
		glDeleteShader(si->gl_id);

	si->gl_id = glCreateShader(si->stage);
	gl_err_check("nsgl_shader::compile Error creating shader");
	// create arguements for glShaderSource
	const char* c_str = source.c_str();

	glShaderSource(si->gl_id, 1, &c_str, NULL); // Attach source to shader
	gl_err_check("nsgl_shader::compile Error setting shader source");

	glCompileShader(si->gl_id);
	gl_err_check("nsgl_shader::compile Error compiling shader");

	GLint worked;
	glGetShaderiv(si->gl_id, GL_COMPILE_STATUS, &worked);
	if (!worked)
	{
		char infoLog[1024];
		glGetShaderInfoLog(si->gl_id, sizeof(infoLog), NULL, (GLchar*)infoLog);
		nsstring info(infoLog);
		info = "----Error compiline shader stage " + stage_name(stage) + " ----\n" + info;
		error_state = error_compile;
		compile_error_msg(info,si->stage);
		dprint(info);
		glDeleteShader(si->gl_id);
		gl_err_check("nsgl_shader::compile Error deleting shader");
		si->gl_id = 0;
		return false;
	}
	dprint("nsgl_shader::compile - Succesfully compiled shader stage " + stage_name(stage));
	return true;
}

void nsgl_shader::init()
{}

uint32 nsgl_shader::init_uniform_loc(const nsstring & var_name)
{
	uint32 loc = glGetUniformLocation(gl_id, var_name.c_str());
	gl_err_check("nsgl_shader::init_uniform_loc");
	uniform_locs[hash_id(var_name)] = loc;
	return loc;
}

bool nsgl_shader::link()
{
	if (gl_id != 0)
	{
		glDeleteProgram(gl_id);
		gl_err_check("nsgl_shader::link Error deleting shader program");
	}

	gl_id = glCreateProgram();
	gl_err_check("nsgl_shader::link Error creating shader program");
		
	if (gl_id == 0)
	{
		release();
		error_state = error_program;
		return false;
	}

	for (uint8 i = 0; i < 6; ++i)
	{
		if (stages[i].gl_id != 0)
		{
			glAttachShader(gl_id, stages[i].gl_id);
			gl_err_check("nsgl_shader::link Error attaching " + stage_name(stages[i].stage) + " shader");	
		}
	}

	if (!xfb_locs.empty())
		_setup_xfb();

	glLinkProgram(gl_id);
	gl_err_check("nsgl_shader::link Error linking program");

	for (uint8 i = 0; i < 6; ++i)
	{
		if (stages[i].gl_id != 0)
		{
			glDetachShader(gl_id, stages[i].gl_id);
			gl_err_check("nsgl_shader::link Error detaching " + stage_name(stages[i].stage) + " shader");	
		}
	}

	GLint worked;
	glGetProgramiv(gl_id, GL_LINK_STATUS, &worked);
	if (!worked)
	{
		char infoLog[1024];
		glGetProgramInfoLog(gl_id, sizeof(infoLog), NULL,(GLchar*)infoLog);
		nsstring info(infoLog);
		info = "----Error linking shader----\n" + info;
		link_error_msg(info);
		dprint(info);
		error_state = error_link;
		glDeleteProgram(gl_id);
		gl_err_check("nsgl_shader::link Error deleting program");
		return false;
	}
	dprint("nsgl_shader::link - Succesfully linked shader");
    return true;
}

nsstring nsgl_shader::stage_name(shader_stage stage)
{
	switch (stage)
	{
	  case (no_stage):
		  return "no stage";
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
	  case (no_stage):
		  return nullptr;
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
    default:
        return nullptr;
	}
}

void nsgl_shader::_setup_xfb()
{
	GLchar ** varyings = new GLchar*[xfb_locs.size()];
	for (uint32 i = 0; i < xfb_locs.size(); ++i)
	{
		GLchar * str = new GLchar[xfb_locs[i].size() + 1];
        strcpy(str, xfb_locs[i].c_str());
		varyings[i] = str;
	}

 	glTransformFeedbackVaryings(gl_id, 4, varyings, xfb);
	gl_err_check("nsgl_shader::_setup_xfb()");

	for (uint32 i = 0; i < xfb_locs.size(); ++i)
		delete[] varyings[i];
	delete[] varyings;
}

void nsgl_shader::bind()
{
	glUseProgram(gl_id);
	gl_err_check("nsgl_shader::bind");
}

void nsgl_shader::unbind()
{
	glUseProgram(0);
	gl_err_check("nsgl_shader::unbind");
}

void nsgl_shader::set_uniform(const nsstring & var_name, const fmat4 & data)
{
	fmat4 mat = data;
	ui_ui_umap::iterator iter = uniform_locs.find(hash_id(var_name));
	if (iter == uniform_locs.end())
		glUniformMatrix4fv(init_uniform_loc(var_name), 1, GL_TRUE, mat.data_ptr());
	else
		glUniformMatrix4fv(iter->second, 1, GL_TRUE, mat.data_ptr());
	gl_err_check("nsgl_shader::set_uniform(fmat4)");
}

void nsgl_shader::set_uniform(const nsstring & var_name, const fmat3 & data)
{
	fmat3 mat = data;
	ui_ui_umap::iterator iter = uniform_locs.find(hash_id(var_name));
	if (iter == uniform_locs.end())
		glUniformMatrix3fv(init_uniform_loc(var_name), 1, GL_TRUE, mat.data_ptr());
	else
		glUniformMatrix3fv(iter->second, 1, GL_TRUE, mat.data_ptr());
	gl_err_check("nsgl_shader::set_uniform(fmat3)");
}

void nsgl_shader::set_uniform(const nsstring & var_name, const fmat2 & data)
{
	fmat2 mat(data);
	ui_ui_umap::iterator iter = uniform_locs.find(hash_id(var_name));
	if (iter == uniform_locs.end())
		glUniformMatrix2fv(init_uniform_loc(var_name), 1, GL_TRUE, mat.data_ptr());
	else
		glUniformMatrix2fv(iter->second, 1, GL_TRUE, mat.data_ptr());
	gl_err_check("nsgl_shader::set_uniform(fmat2)");	
}

void nsgl_shader::set_uniform(const nsstring & var_name, bool val)
{
	set_uniform(var_name, int32(val));
}

void nsgl_shader::set_uniform(const nsstring & var_name, const fvec4 & data)
{
	GLfloat vec[4];
	vec[0] = data.x; vec[1] = data.y; vec[2] = data.z; vec[3] = data.w;
	ui_ui_umap::iterator iter = uniform_locs.find(hash_id(var_name));
	if (iter == uniform_locs.end())
		glUniform4fv(init_uniform_loc(var_name), 1, vec);
	else
		glUniform4fv(iter->second, 1, vec);
	gl_err_check("nsgl_shader::set_uniform(fvec4)");
}

void nsgl_shader::set_uniform(const nsstring & var_name, const fvec3 & data)
{
	GLfloat vec[3];
	vec[0] = data.x; vec[1] = data.y; vec[2] = data.z;
	ui_ui_umap::iterator iter = uniform_locs.find(hash_id(var_name));
	if (iter == uniform_locs.end())
		glUniform3fv(init_uniform_loc(var_name), 1, vec);
	else
		glUniform3fv(iter->second, 1, vec);
	gl_err_check("nsgl_shader::set_uniform(fvec3)");
}

void nsgl_shader::set_uniform(const nsstring & var_name, const fvec2 & data)
{
	GLfloat vec[2];
	vec[0] = data.u; vec[1] = data.v;
	ui_ui_umap::iterator iter = uniform_locs.find(hash_id(var_name));
	if (iter == uniform_locs.end())
		glUniform2fv(init_uniform_loc(var_name), 1, vec);
	else
		glUniform2fv(iter->second, 1, vec);
	gl_err_check("nsgl_shader::set_uniform(fvec2)");
}

void nsgl_shader::set_uniform(const nsstring & var_name, const ivec4 & data)
{
	GLint vec[4];
	vec[0] = data.x; vec[1] = data.y; vec[2] = data.z; vec[3] = data.w;
	ui_ui_umap::iterator iter = uniform_locs.find(hash_id(var_name));
	if (iter == uniform_locs.end())
		glUniform4iv(init_uniform_loc(var_name), 1, vec);
	else
		glUniform4iv(iter->second, 1, vec);
	gl_err_check("nsgl_shader::set_uniform(ivec4)");		
}

void nsgl_shader::set_uniform(const nsstring & var_name, const ivec3 & data)
{
	GLint vec[3];
	vec[0] = data.x; vec[1] = data.y; vec[2] = data.z;
	ui_ui_umap::iterator iter = uniform_locs.find(hash_id(var_name));
	if (iter == uniform_locs.end())
		glUniform3iv(init_uniform_loc(var_name), 1, vec);
	else
		glUniform3iv(iter->second, 1, vec);
	gl_err_check("nsgl_shader::set_uniform(ivec3)");		
}

void nsgl_shader::set_uniform(const nsstring & var_name, const ivec2 & data)
{
	GLint vec[2];
	vec[0] = data.u; vec[1] = data.v;
	ui_ui_umap::iterator iter = uniform_locs.find(hash_id(var_name));
	if (iter == uniform_locs.end())
		glUniform2iv(init_uniform_loc(var_name), 1, vec);
	else
		glUniform2iv(iter->second, 1, vec);
	gl_err_check("nsgl_shader::set_uniform(ivec2)");		
}

void nsgl_shader::set_uniform(const nsstring & var_name, const uivec4 & data)
{
	GLuint vec[4];
	vec[0] = data.x; vec[1] = data.y; vec[2] = data.z; vec[3] = data.w;
	ui_ui_umap::iterator iter = uniform_locs.find(hash_id(var_name));
	if (iter == uniform_locs.end())
		glUniform4uiv(init_uniform_loc(var_name), 1, vec);
	else
		glUniform4uiv(iter->second, 1, vec);
	gl_err_check("nsgl_shader::set_uniform(uivec4)");		
}

void nsgl_shader::set_uniform(const nsstring & var_name, const uivec3 & data)
{
	GLuint vec[3];
	vec[0] = data.x; vec[1] = data.y; vec[2] = data.z;
	ui_ui_umap::iterator iter = uniform_locs.find(hash_id(var_name));
	if (iter == uniform_locs.end())
		glUniform3uiv(init_uniform_loc(var_name), 1, vec);
	else
		glUniform3uiv(iter->second, 1, vec);
	gl_err_check("nsgl_shader::set_uniform(uivec3)");				
}

void nsgl_shader::set_uniform(const nsstring & var_name, const uivec2 & data)
{
	GLuint vec[2];
	vec[0] = data.u; vec[1] = data.v;
	ui_ui_umap::iterator iter = uniform_locs.find(hash_id(var_name));
	if (iter == uniform_locs.end())
		glUniform2uiv(init_uniform_loc(var_name), 1, vec);
	else
		glUniform2uiv(iter->second, 1, vec);
	gl_err_check("nsgl_shader::set_uniform(uivec2)");	
}

void nsgl_shader::set_uniform(const nsstring & var_name, float data)
{
	ui_ui_umap::iterator iter = uniform_locs.find(hash_id(var_name));
	if (iter == uniform_locs.end())
		glUniform1f(init_uniform_loc(var_name), data);
	else
		glUniform1f(iter->second, data);
	gl_err_check("nsgl_shader::set_uniform(float)");
}

void nsgl_shader::set_uniform(const nsstring & var_name, int32 data)
{
	ui_ui_umap::iterator iter = uniform_locs.find(hash_id(var_name));
	if (iter == uniform_locs.end())
		glUniform1i(init_uniform_loc(var_name), data);
	else
		glUniform1i(iter->second, data);
	gl_err_check("nsgl_shader::set_uniform(int32)");
}

void nsgl_shader::set_uniform(const nsstring & var_name, uint32 data)
{
	ui_ui_umap::iterator iter = uniform_locs.find(hash_id(var_name));
	if (iter == uniform_locs.end())
		glUniform1ui(init_uniform_loc(var_name), data);
	else
		glUniform1ui(iter->second, data);
	gl_err_check("nsgl_shader::set_uniform(uint32)");
}

void nsgl_shader::release()
{
	if (gl_id != 0)
	{
		glDeleteProgram(gl_id);
		gl_id = 0;
		gl_err_check("nsgl_shader::release");
	}
	for (uint8 i = 0; i < 6; ++i)
	{
		glDeleteShader(stages[i].gl_id);
		stages[i].gl_id = 0;
		gl_err_check("nsgl_shader::release");
	}
}

uint32 nsgl_shader::attrib_loc(const nsstring & var_name) const
{
	return glGetAttribLocation(gl_id, var_name.c_str());
	gl_err_check("nsgl_shader::attrib_loc");
}

bool nsgl_shader::validate()
{
	glValidateProgram(gl_id);
	GLint worked;
    glGetProgramiv(gl_id, GL_VALIDATE_STATUS, &worked);
    if (!worked) 
	{
		char infoLog[1024];
        glGetProgramInfoLog(gl_id, sizeof(infoLog), NULL, infoLog);
		nsstring info(infoLog);
		info = "----Error validating shader----\n" + info;
		validation_error_msg(info);
		dprint(info);
        return false;
    }
	return true;
}
