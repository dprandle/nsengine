/*! 
	\file nsbufferobject.cpp
	
	\brief Definition file for nsbuffer_object class

	This file contains all of the neccessary definitions for the nsbuffer_object class.

	\author Daniel Randle
	\date November 2 2013
	\copywrite Earth Banana Games 2013
*/


#include <nsgl_buffer.h>
#include <nsgl_object.h>
#include <nsengine.h>

nsgl_buffer::nsgl_buffer():
	nsgl_obj()
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
		glBindBuffer(target, gl_id);
	}
	gl_err_check("nsgl_buffer::unbind");
}

void nsgl_buffer::init()
{
	if (gl_id != 0)
	{
		dprint("nsgl_buffer::init Error trying to initialize already initialized buffer");
		return;
	}
	glGenBuffers(1, &gl_id);
	gl_err_check("nsgl_buffer::init");	
}
	
void nsgl_buffer::release()
{
	glDeleteBuffers(1, &gl_id);
	gl_id = 0;
	gl_err_check("nsgl_buffer::release");	
}

void nsgl_buffer::allocate(uint32 data_byte_size, void * data, mutable_usage_flag usage)
{
	glBufferData(target, data_byte_size, data, usage);
	gl_err_check("nsgl_buffer::allocate");
}

void nsgl_buffer::allocate(uint32 data_byte_size, void * data, immutable_usage_flag usage)
{
	glBufferStorage(target, data_byte_size, data, usage);
	gl_err_check("nsgl_buffer::allocate");	
}

void nsgl_buffer::clear(int32 internal_format, int32 format, int32 type, const void * data)
{
	glClearBufferData(target,internal_format,format,type,data);
	gl_err_check("nsgl_buffer::clear");
}

void nsgl_buffer::download(uint32 offset_byte_size, uint32 data_byte_size, void * data)
{
	glGetBufferSubData(target, offset_byte_size, data_byte_size, data);
	gl_err_check("nsbuffer_object::download");
}

void nsgl_buffer::upload(uint32 offset_byte_size, uint32 data_byte_size, void * data)
{
	glBufferSubData(target, offset_byte_size, data_byte_size, data);
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
	glUnmapBuffer(target);
	gl_err_check("nsbuffer_object::unmap");
}
