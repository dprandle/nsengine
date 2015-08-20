/*! 
	\file nsgl_object.h
	
	\brief Header file for NSGLObject class

	This file contains all of the neccessary declarations for the NSGLObject class.

	\author Daniel Randle
	\date November 2 2013
	\copywrite Earth Banana Games 2013
*/

#ifndef NSGL_OBJECT_H
#define NSGL_OBJECT_H

#include <myGL/glew.h>
#include <nsvector.h>

bool GLError(nsstring errorMessage);

class nsgl_object
{
public:
	nsgl_object();
	virtual ~nsgl_object();
	virtual void bind()=0;
	virtual void init_gl()=0;
	virtual void release()=0;
	virtual void unbind()=0;

	uint32 gl_id();

protected:
	uint32 m_gl_name;
};

#endif
