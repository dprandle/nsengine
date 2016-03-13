/*! 
	\file nsglobject.cpp
	
	\brief Definition file for NSGLObject class

	This file contains all of the neccessary definitions for the NSGLObject class.

	\author Daniel Randle
	\date November 2 2013
	\copywrite Earth Banana Games 2013
*/

#include <myGL/glew.h>
#include <nsgl_object.h>
#include <nslog_file.h>
#include <nsengine.h>

nsgl_object::nsgl_object() :m_gl_name(0)
{}

nsgl_object::~nsgl_object()
{
	if (m_gl_name != 0)
		nslog_file("Warning : Failed to call \"release()\" on NSBuffer object - openGL resource was not released","enginecreatedestroy.log");
}

uint32 nsgl_object::gl_id() const
{
	return m_gl_name;
}

void nsgl_object::set_gl_id(uint32 id)
{
	if (m_gl_name != 0)
		release();
	m_gl_name = id;
}

bool gl_err_check(nsstring errorMessage)
{
	GLenum err = glGetError();
	if (err != GL_NO_ERROR)
	{
#ifdef NSDEBUG
		nsstringstream ss;
		ss << errorMessage << "\nOpenGL Error Code : " << err;
		dprint(ss.str());
#endif
		return true;
	}
	return false;
}

