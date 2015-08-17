/*!
\file nsvertex_array_object.h

\brief Header file for NSVertexArrayObject class

This file contains all of the neccessary declarations for the NSVertexArrayObject class.

\author Daniel Randle
\date November 2 2013
\copywrite Earth Banana Games 2013
*/

#ifndef NSVERTARRAY_H
#define NSVERTARRAY_H

#include <nsgl_object.h>
#include <myGL/glew.h>
#include <vector>
#include <nstexture.h>
#include <nsglobal.h>

class NSBufferObject;

class NSVertexArrayObject : public NSGLObject
{
public:
	typedef std::map<uint32,std::set<uint32>> BufferMap;
	NSVertexArrayObject();

	void add(NSBufferObject * mBuffer, uint32 pAttLoc);

	void bind();

	bool contains(NSBufferObject * mBuffer);

	void enable(uint32 pAttLoc);

	void disable(uint32 pAttLoc);

	void initGL();

	void remove(NSBufferObject * mBuffer);

	void remove(NSBufferObject * mBuffer, uint32 pAttLoc);

	void release();

	void unbind();

	void vertexAttribDiv(uint32 pAttLoc, uint32 pDivisor);

	void vertexAttribPtr(uint32 pAttLoc, uint32 pElementsPerAttribute, uint32 pGLElementType, bool pNormalized, uint32 pAttributeStride, uint32 pByteOffset = 0);

	void vertexAttribIPtr(uint32 pAttLoc, uint32 pElementsPerAttribute, uint32 pGLElementType, uint32 pAttributeStride, uint32 pByteOffset = 0);

	void vertexAttribLPtr(uint32 pAttLoc, uint32 pElementsPerAttribute, uint32 pGLElementType, uint32 pAttributeStride, uint32 pByteOffset = 0);

private:
	BufferMap mOwnedBuffers;
};

#endif
