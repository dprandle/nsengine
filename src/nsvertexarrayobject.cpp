/*!
\file nsvertexarrayobject.cpp

\brief Definition file for NSVertexArrayObject class

This file contains all of the neccessary definitions for the NSVertexArrayObject class.

\author Daniel Randle
\date November 2 2013
\copywrite Earth Banana Games 2013
*/


#include <nsvertexarrayobject.h>
#include <nsbufferobject.h>

NSVertexArrayObject::NSVertexArrayObject() :
NSGLObject()
{}

/*!
Add the buffer to the VAO - the buffer must be bound before adding
*/
void NSVertexArrayObject::add(NSBufferObject * mBuffer, nsuint pAttLoc)
{
	mOwnedBuffers[mBuffer->glid()].emplace(pAttLoc);
	enable(pAttLoc);
}

void NSVertexArrayObject::bind()
{
	glBindVertexArray(mGLName);
	GLError("NSVertexArrayObject::bind()");
}

bool NSVertexArrayObject::contains(NSBufferObject * pBuffer)
{
	return (mOwnedBuffers.find(pBuffer->glid()) != mOwnedBuffers.end());
}

void NSVertexArrayObject::enable(nsuint pAttLoc)
{
	glEnableVertexAttribArray(pAttLoc);
	GLError("NSVertexArrayObject::enableAttribute()");
}

void NSVertexArrayObject::disable(nsuint pAttLoc)
{
	glDisableVertexAttribArray(pAttLoc);
	GLError("NSVertexArrayObject::disableAttribute()");
}

void NSVertexArrayObject::initGL()
{
	glGenVertexArrays(1, &mGLName);
	GLError("NSVertexArrayObject::initGL()");
}

void NSVertexArrayObject::remove(NSBufferObject * mBuffer)
{
	auto itemSet = mOwnedBuffers.find(mBuffer->glid());
	if (itemSet == mOwnedBuffers.end())
		return;
	auto curItem = itemSet->second.begin();
	while (curItem != itemSet->second.end())
	{
		disable(*curItem);
		++curItem;
	}
	mOwnedBuffers.erase(mBuffer->glid());
}

/*!
Remove the buffer from the VAO - the buffer must be bound before removing
*/
void NSVertexArrayObject::remove(NSBufferObject * mBuffer, nsuint pAttLoc)
{
	auto item = mOwnedBuffers.find(mBuffer->glid());
	if (item != mOwnedBuffers.end())
	{
		size_t i = item->second.erase(pAttLoc);
		if (i != 0)
		{
			disable(pAttLoc);
			if (item->second.empty())
				mOwnedBuffers.erase(mBuffer->glid());
		}
	}
	
}

void NSVertexArrayObject::release()
{
	glDeleteVertexArrays(1, &mGLName);
	mGLName = 0;
	GLError("NSVertexArrayObject::release()");
}

void NSVertexArrayObject::unbind()
{
	glBindVertexArray(0);
	GLError("NSVertexArrayObject::unbind()");
}

void NSVertexArrayObject::vertexAttribDiv(nsuint pAttLoc, nsuint pDivisor)
{
	glVertexAttribDivisor(pAttLoc, pDivisor);
	GLError("NSVertexArrayObject::vertexAttribDiv()");
}

void NSVertexArrayObject::vertexAttribPtr(nsuint pAttLoc, nsuint pElementsPerAttribute, nsuint pGLElementType, nsbool pNormalized, nsuint pAttributeStride, nsuint pByteOffset)
{
	glVertexAttribPointer(pAttLoc, pElementsPerAttribute, pGLElementType, pNormalized, pAttributeStride, (const GLvoid*)pByteOffset);
	GLError("NSVertexArrayObject::vertexAttribPtr()");
}

void NSVertexArrayObject::vertexAttribIPtr(nsuint pAttLoc, nsuint pElementsPerAttribute, nsuint pGLElementType, nsuint pAttributeStride, nsuint pByteOffset)
{
	glVertexAttribIPointer(pAttLoc, pElementsPerAttribute, pGLElementType, pAttributeStride, (const GLvoid*)pByteOffset);
	GLError("NSVertexArrayObject::vertexAttribIPtr()");
}

void NSVertexArrayObject::vertexAttribLPtr(nsuint pAttLoc, nsuint pElementsPerAttribute, nsuint pGLElementType, nsuint pAttributeStride, nsuint pByteOffset)
{
	glVertexAttribLPointer(pAttLoc, pElementsPerAttribute, pGLElementType, pAttributeStride, (const GLvoid*)pByteOffset);
	GLError("NSVertexArrayObject::vertexAttribLPtr()");
}
