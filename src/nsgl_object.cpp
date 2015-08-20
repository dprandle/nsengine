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
		NSLogFile("Warning : Failed to call \"release()\" on NSBuffer object - openGL resource was not released","enginecreatedestroy.log");
}

uint32 nsgl_object::gl_id()
{
	return m_gl_name;
}

bool GLError(nsstring errorMessage)
{
	GLenum err = glGetError();
	if (err != GL_NO_ERROR)
	{
#ifdef NSDEBUG
		nsstringstream ss;
		ss << errorMessage << ": OpenGL Error Code : " << err;
		dprint(ss.str());
#endif
		return true;
	}
	return false;
}
