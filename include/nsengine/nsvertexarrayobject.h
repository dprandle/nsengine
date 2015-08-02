/*!
\file nsvertexarrayobject.h

\brief Header file for NSVertexArrayObject class

This file contains all of the neccessary declarations for the NSVertexArrayObject class.

\author Daniel Randle
\date November 2 2013
\copywrite Earth Banana Games 2013
*/

#ifndef NSVERTARRAY_H
#define NSVERTARRAY_H

#include <nsglobject.h>
#include <myGL/glew.h>
#include <vector>
#include <nstexture.h>
#include <nsglobal.h>

class NSBufferObject;

class NSVertexArrayObject : public NSGLObject
{
public:
	typedef std::map<nsuint,std::set<nsuint>> BufferMap;
	NSVertexArrayObject();

	void add(NSBufferObject * mBuffer, nsuint pAttLoc);

	void bind();

	bool contains(NSBufferObject * mBuffer);

	void enable(nsuint pAttLoc);

	void disable(nsuint pAttLoc);

	void initGL();

	void remove(NSBufferObject * mBuffer);

	void remove(NSBufferObject * mBuffer, nsuint pAttLoc);

	void release();

	void unbind();

	void vertexAttribDiv(nsuint pAttLoc, nsuint pDivisor);

	void vertexAttribPtr(nsuint pAttLoc, nsuint pElementsPerAttribute, nsuint pGLElementType, nsbool pNormalized, nsuint pAttributeStride, nsuint pByteOffset = 0);

	void vertexAttribIPtr(nsuint pAttLoc, nsuint pElementsPerAttribute, nsuint pGLElementType, nsuint pAttributeStride, nsuint pByteOffset = 0);

	void vertexAttribLPtr(nsuint pAttLoc, nsuint pElementsPerAttribute, nsuint pGLElementType, nsuint pAttributeStride, nsuint pByteOffset = 0);

private:
	BufferMap mOwnedBuffers;
};

#endif
