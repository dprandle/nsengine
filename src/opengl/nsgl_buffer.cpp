/*! 
	\file nsbufferobject.cpp
	
	\brief Definition file for nsbuffer_object class

	This file contains all of the neccessary definitions for the nsbuffer_object class.

	\author Daniel Randle
	\date November 2 2013
	\copywrite Earth Banana Games 2013
*/


#include <opengl/nsgl_buffer.h>
#include <opengl/nsgl_object.h>
#include <nsengine.h>
#include <opengl/nsgl_driver.h>

nsgl_buffer::nsgl_buffer():
    nsgl_obj(),
    target(0),
    target_index(0),
    indexed_target_offset(0),
    indexed_target_size(0)
{}

nsgl_buffer::~nsgl_buffer()
{}

void nsgl_buffer::bind()
{
	if (target == transform_feedback || target == uniform ||
		target == shader_storage || target == atomic_counter)
	{
		if (indexed_target_size == 0)
		{
			glBindBufferBase(target, target_index, gl_id);
		}
		else
		{
			glBindBufferRange(target, target_index, gl_id, indexed_target_offset, indexed_target_size);
		}
	}
	else
	{
		glBindBuffer(target, gl_id);
	}
	gl_err_check("nsgl_buffer::bind");
}

void nsgl_buffer::unbind()
{
	if (target == transform_feedback || target == uniform ||
		target == shader_storage || target == atomic_counter)
	{
		glBindBufferBase(target, target_index, 0);
	}
	else
	{
        glBindBuffer(target, 0);
	}
    if (gl_err_check("nsgl_buffer::unbind"))
    {
        dprint("Checking errors");
    }
}

void nsgl_buffer::init()
{
	if (gl_id != 0)
	{
		dprint("nsgl_buffer::init Error trying to initialize already initialized buffer");
		return;
	}
	glGenBuffers(1, &gl_id);
    if (gl_err_check("nsgl_buffer::init"))
    {
        dprint("tooty fruity");
    }
    return;
}
	
void nsgl_buffer::release()
{
	glDeleteBuffers(1, &gl_id);
	gl_id = 0;
	gl_err_check("nsgl_buffer::release");	
}

void nsgl_buffer::allocate(
	uint32 data_size,
	uint32 data_type_byte_size,
	void * data,
	mutable_usage_flag usage)
{
	glBufferData(target, data_size * data_type_byte_size, data, usage);
	gl_err_check("nsgl_buffer::allocate");
}

void nsgl_buffer::allocate(
	uint32 data_size,
	uint32 data_type_byte_size,
	void * data,
	immutable_usage_flag usage)
{
	glBufferStorage(target, data_size * data_type_byte_size, data, usage);
	gl_err_check("nsgl_buffer::allocate");	
}

void nsgl_buffer::clear(int32 internal_format, int32 format, int32 type, const void * data)
{
	glClearBufferData(target,internal_format,format,type,data);
	gl_err_check("nsgl_buffer::clear");
}

void nsgl_buffer::download(uint32 offset_size, uint32 data_size, uint32 data_type_byte_size, void * data)
{
	glGetBufferSubData(target, offset_size * data_type_byte_size, data_size * data_type_byte_size, data);
	gl_err_check("nsbuffer_object::download");
}

void nsgl_buffer::upload(uint32 offset, uint32 data_size, uint32 data_type_byte_size, void * data)
{
	glBufferSubData(target, offset * data_type_byte_size, data_size * data_type_byte_size, data);
	gl_err_check("nsgl_buffer::upload");
}
	
void * nsgl_buffer::map(access_map access)
{
	void * ret = glMapBuffer(target, access);
	gl_err_check("nsgl_buffer::map");
	return ret;
}
	
void * nsgl_buffer::map_range(uint32 byte_offset, uint32 byte_length, access_map_range access)
{
	void * ret = glMapBufferRange(target, byte_offset, byte_length, access);
	gl_err_check("nsgl_buffer::map_range");
	return ret;	
}

void nsgl_buffer::unmap()
{
    if (glUnmapBuffer(target) == GL_FALSE)
    {
        dprint("nsgl_buffer::unmap - Error unmapping buffer");
    }
	gl_err_check("nsbuffer_object::unmap");
}
