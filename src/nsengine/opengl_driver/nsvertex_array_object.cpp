/*!
\file nsvertexarrayobject.cpp

\brief Definition file for nsvertex_array_object class

This file contains all of the neccessary definitions for the nsvertex_array_object class.

\author Daniel Randle
\date November 2 2013
\copywrite Earth Banana Games 2013
*/


#include <nsvertex_array_object.h>
#include <nsbuffer_object.h>

nsvertex_array_object::nsvertex_array_object() :
nsgl_object()
{}

/*!
Add the buffer to the VAO - the buffer must be bound before adding
*/
void nsvertex_array_object::add(nsbuffer_object * mBuffer, uint32 pAttLoc)
{
	m_owned_buffers[mBuffer->gl_id()].emplace(pAttLoc);
	enable(pAttLoc);
}

void nsvertex_array_object::bind() const
{
	glBindVertexArray(gl_id());
	gl_err_check("nsvertex_array_object::bind()");
}

bool nsvertex_array_object::contains(nsbuffer_object * pBuffer)
{
	return (m_owned_buffers.find(pBuffer->gl_id()) != m_owned_buffers.end());
}

void nsvertex_array_object::enable(uint32 pAttLoc)
{
	glEnableVertexAttribArray(pAttLoc);
	gl_err_check("nsvertex_array_object::enableAttribute()");
}

void nsvertex_array_object::disable(uint32 pAttLoc)
{
	glDisableVertexAttribArray(pAttLoc);
	gl_err_check("nsvertex_array_object::disableAttribute()");
}

void nsvertex_array_object::video_init()
{
	uint32 glid;
	glGenVertexArrays(1, &glid);
	set_gl_id(glid);
	gl_err_check("nsvertex_array_object::initGL()");
}

void nsvertex_array_object::remove(nsbuffer_object * mBuffer)
{
	auto itemSet = m_owned_buffers.find(mBuffer->gl_id());
	if (itemSet == m_owned_buffers.end())
		return;
	auto curItem = itemSet->second.begin();
	while (curItem != itemSet->second.end())
	{
		disable(*curItem);
		++curItem;
	}
	m_owned_buffers.erase(mBuffer->gl_id());
}

/*!
Remove the buffer from the VAO - the buffer must be bound before removing
*/
void nsvertex_array_object::remove(nsbuffer_object * mBuffer, uint32 pAttLoc)
{
	auto item = m_owned_buffers.find(mBuffer->gl_id());
	if (item != m_owned_buffers.end())
	{
		size_t i = item->second.erase(pAttLoc);
		if (i != 0)
		{
			disable(pAttLoc);
			if (item->second.empty())
				m_owned_buffers.erase(mBuffer->gl_id());
		}
	}
	
}

void nsvertex_array_object::video_release()
{
	uint32 glid = gl_id();
	glDeleteVertexArrays(1, &glid);
	set_gl_id(0);
	gl_err_check("nsvertex_array_object::release()");
}

void nsvertex_array_object::unbind() const
{
	glBindVertexArray(0);
	if (gl_err_check("nsvertex_array_object::unbind()"))
		std::terminate();
}

void nsvertex_array_object::vertex_attrib_div(uint32 pAttLoc, uint32 pDivisor)
{
	glVertexAttribDivisor(pAttLoc, pDivisor);
	gl_err_check("nsvertex_array_object::vertexAttribDiv()");
}

void nsvertex_array_object::vertex_attrib_ptr(uint32 pAttLoc, uint32 pElementsPerAttribute, uint32 pGLElementType, bool pNormalized, uint32 pAttributeStride, uint32 pByteOffset)
{
	int64 cst = pByteOffset;
	glVertexAttribPointer(pAttLoc, pElementsPerAttribute, pGLElementType, pNormalized, pAttributeStride, (const GLvoid*)cst);
	gl_err_check("nsvertex_array_object::vertexAttribPtr()");
}

void nsvertex_array_object::vertex_attrib_I_ptr(uint32 pAttLoc, uint32 pElementsPerAttribute, uint32 pGLElementType, uint32 pAttributeStride, uint32 pByteOffset)
{
	int64 cst = pByteOffset;
	glVertexAttribIPointer(pAttLoc, pElementsPerAttribute, pGLElementType, pAttributeStride, (const GLvoid*)cst);
	gl_err_check("nsvertex_array_object::vertexAttribIPtr()");
}

void nsvertex_array_object::vertex_attrib_L_ptr(uint32 pAttLoc, uint32 pElementsPerAttribute, uint32 pGLElementType, uint32 pAttributeStride, uint32 pByteOffset)
{
	int64 cst = pByteOffset;
	glVertexAttribLPointer(pAttLoc, pElementsPerAttribute, pGLElementType, pAttributeStride, (const GLvoid*)cst);
	gl_err_check("nsvertex_array_object::vertexAttribLPtr()");
}
