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

//#include "glew.h"
#include "nsvector.h"

bool gl_err_check(nsstring errorMessage);

struct nsgl_obj
{
	nsgl_obj();
	virtual ~nsgl_obj();

	virtual void bind()=0;
	virtual void init()=0;
	virtual void release()=0;
	virtual void unbind()=0;
	
	uint32 gl_id;
};

#endif
