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

#define MAX_CONTEXT_COUNT 16

#include <myGL/glew.h>
#include <nsvector.h>

bool gl_err_check(nsstring errorMessage);

struct nsgl_object
{
public:
	nsgl_object();
	virtual ~nsgl_object();
//	virtual void bind() const = 0;
//	virtual void video_init()=0;
//	virtual void video_release()=0;
//	virtual void unbind() const = 0;

	bool all_ids_released();
	uint32 gl_id() const;
	void set_gl_id(uint32 id);
	uint32 m_gl_name[MAX_CONTEXT_COUNT];
};

#endif
