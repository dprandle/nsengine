/*!
\file nsvertexarrayobject.cpp

\brief Definition file for nsgl_vao class

This file contains all of the neccessary definitions for the nsgl_vao class.

\author Daniel Randle
\date November 2 2013
\copywrite Earth Banana Games 2013
*/


#include <nsgl_vao.h>
#include <nsgl_buffer.h>

nsgl_vao::nsgl_vao() :
nsgl_obj()
{}

void nsgl_vao::add(nsgl_buffer * buffer, uint32 att_loc)
{
	owned_buffers[buffer->gl_id].emplace(att_loc);
	enable(att_loc);
}

void nsgl_vao::bind()
{
	glBindVertexArray(gl_id);
	gl_err_check("nsgl_vao::bind()");
}

bool nsgl_vao::contains(nsgl_buffer * pBuffer)
{
	return (owned_buffers.find(pBuffer->gl_id) != owned_buffers.end());
}

void nsgl_vao::enable(uint32 att_loc)
{
	glEnableVertexAttribArray(att_loc);
	gl_err_check("nsgl_vao::enable");
}

void nsgl_vao::disable(uint32 att_loc)
{
	glDisableVertexAttribArray(att_loc);
	gl_err_check("nsgl_vao::disable");
}

void nsgl_vao::init()
{
	if (gl_id != 0)
	{
		dprint("nsgl_vao::init Error trying to initialize already initialized vao");
		return;
	}
	glGenVertexArrays(1, &gl_id);
	gl_err_check("nsgl_vao::init");
}

void nsgl_vao::remove(nsgl_buffer * buffer)
{
	auto itemSet = owned_buffers.find(buffer->gl_id);
	if (itemSet == owned_buffers.end())
		return;
	auto curItem = itemSet->second.begin();
	while (curItem != itemSet->second.end())
	{
		disable(*curItem);
		++curItem;
	}
	owned_buffers.erase(buffer->gl_id);
}

void nsgl_vao::remove(nsgl_buffer * buffer, uint32 att_loc)
{
	auto item = owned_buffers.find(buffer->gl_id);
	if (item != owned_buffers.end())
	{
		size_t i = item->second.erase(att_loc);
		if (i != 0)
		{
			disable(att_loc);
			if (item->second.empty())
				owned_buffers.erase(buffer->gl_id);
		}
	}
	
}

void nsgl_vao::release()
{
	glDeleteVertexArrays(1, &gl_id);
	gl_id = 0;
	gl_err_check("nsgl_vao::release");
}

void nsgl_vao::unbind()
{
	glBindVertexArray(0);
	gl_err_check("nsgl_vao::unbind");
}

void nsgl_vao::vertex_attrib_div(uint32 att_loc, uint32 divisor)
{
	glVertexAttribDivisor(att_loc, divisor);
	gl_err_check("nsgl_vao::vertex_attrib_div");
}

void nsgl_vao::vertex_attrib_ptr(
	uint32 att_loc,
	uint32 elements_per_attrib,
	uint32 elements_type,
	bool normalized,
	uint32 attrib_stride,
	uint32 byte_offset)
{
	int64 cst = byte_offset;
	glVertexAttribPointer(att_loc, elements_per_attrib, elements_type, normalized, attrib_stride, (const GLvoid*)cst);
	gl_err_check("nsgl_vao::vertexAttribPtr()");
}

void nsgl_vao::vertex_attrib_I_ptr(
	uint32 att_loc,
	uint32 elements_per_attrib,
	uint32 elements_type,
	uint32 attrib_stride,
	uint32 byte_offset)
{
	int64 cst = byte_offset;
	glVertexAttribIPointer(att_loc, elements_per_attrib, elements_type, attrib_stride, (const GLvoid*)cst);
	gl_err_check("nsgl_vao::vertexAttribIPtr()");
}

void nsgl_vao::vertex_attrib_L_ptr(
	uint32 att_loc,
	uint32 elements_per_attrib,
	uint32 elements_type,
	uint32 attrib_stride,
	uint32 byte_offset)
{
	int64 cst = byte_offset;
	glVertexAttribLPointer(att_loc, elements_per_attrib, elements_type, attrib_stride, (const GLvoid*)cst);
	gl_err_check("nsgl_vao::vertexAttribLPtr()");
}
