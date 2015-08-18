/*!
\file nsvertexarrayobject.cpp

\brief Definition file for NSVertexArrayObject class

This file contains all of the neccessary definitions for the NSVertexArrayObject class.

\author Daniel Randle
\date November 2 2013
\copywrite Earth Banana Games 2013
*/


#include <nsvertex_array_object.h>
#include <nsbuffer_object.h>

NSVertexArrayObject::NSVertexArrayObject() :
nsgl_object()
{}

/*!
Add the buffer to the VAO - the buffer must be bound before adding
*/
void NSVertexArrayObject::add(NSBufferObject * mBuffer, uint32 pAttLoc)
{
	mOwnedBuffers[mBuffer->gl_id()].emplace(pAttLoc);
	enable(pAttLoc);
}

void NSVertexArrayObject::bind()
{
	glBindVertexArray(m_gl_name);
	GLError("NSVertexArrayObject::bind()");
}

bool NSVertexArrayObject::contains(NSBufferObject * pBuffer)
{
	return (mOwnedBuffers.find(pBuffer->gl_id()) != mOwnedBuffers.end());
}

void NSVertexArrayObject::enable(uint32 pAttLoc)
{
	glEnableVertexAttribArray(pAttLoc);
	GLError("NSVertexArrayObject::enableAttribute()");
}

void NSVertexArrayObject::disable(uint32 pAttLoc)
{
	glDisableVertexAttribArray(pAttLoc);
	GLError("NSVertexArrayObject::disableAttribute()");
}

void NSVertexArrayObject::init_gl()
{
	glGenVertexArrays(1, &m_gl_name);
	GLError("NSVertexArrayObject::initGL()");
}

void NSVertexArrayObject::remove(NSBufferObject * mBuffer)
{
	auto itemSet = mOwnedBuffers.find(mBuffer->gl_id());
	if (itemSet == mOwnedBuffers.end())
		return;
	auto curItem = itemSet->second.begin();
	while (curItem != itemSet->second.end())
	{
		disable(*curItem);
		++curItem;
	}
	mOwnedBuffers.erase(mBuffer->gl_id());
}

/*!
Remove the buffer from the VAO - the buffer must be bound before removing
*/
void NSVertexArrayObject::remove(NSBufferObject * mBuffer, uint32 pAttLoc)
{
	auto item = mOwnedBuffers.find(mBuffer->gl_id());
	if (item != mOwnedBuffers.end())
	{
		size_t i = item->second.erase(pAttLoc);
		if (i != 0)
		{
			disable(pAttLoc);
			if (item->second.empty())
				mOwnedBuffers.erase(mBuffer->gl_id());
		}
	}
	
}

void NSVertexArrayObject::release()
{
	glDeleteVertexArrays(1, &m_gl_name);
	m_gl_name = 0;
	GLError("NSVertexArrayObject::release()");
}

void NSVertexArrayObject::unbind()
{
	glBindVertexArray(0);
	GLError("NSVertexArrayObject::unbind()");
}

void NSVertexArrayObject::vertexAttribDiv(uint32 pAttLoc, uint32 pDivisor)
{
	glVertexAttribDivisor(pAttLoc, pDivisor);
	GLError("NSVertexArrayObject::vertexAttribDiv()");
}

void NSVertexArrayObject::vertexAttribPtr(uint32 pAttLoc, uint32 pElementsPerAttribute, uint32 pGLElementType, bool pNormalized, uint32 pAttributeStride, uint32 pByteOffset)
{
	int64 cst = pByteOffset;
	glVertexAttribPointer(pAttLoc, pElementsPerAttribute, pGLElementType, pNormalized, pAttributeStride, (const GLvoid*)cst);
	GLError("NSVertexArrayObject::vertexAttribPtr()");
}

void NSVertexArrayObject::vertexAttribIPtr(uint32 pAttLoc, uint32 pElementsPerAttribute, uint32 pGLElementType, uint32 pAttributeStride, uint32 pByteOffset)
{
	int64 cst = pByteOffset;
	glVertexAttribIPointer(pAttLoc, pElementsPerAttribute, pGLElementType, pAttributeStride, (const GLvoid*)cst);
	GLError("NSVertexArrayObject::vertexAttribIPtr()");
}

void NSVertexArrayObject::vertexAttribLPtr(uint32 pAttLoc, uint32 pElementsPerAttribute, uint32 pGLElementType, uint32 pAttributeStride, uint32 pByteOffset)
{
	int64 cst = pByteOffset;
	glVertexAttribLPointer(pAttLoc, pElementsPerAttribute, pGLElementType, pAttributeStride, (const GLvoid*)cst);
	GLError("NSVertexArrayObject::vertexAttribLPtr()");
}
