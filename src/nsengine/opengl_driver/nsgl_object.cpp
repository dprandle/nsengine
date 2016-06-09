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
#include <nsgl_driver.h>

nsgl_obj::nsgl_obj():
	gl_id(0)
{}

nsgl_obj::~nsgl_obj()
{}

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

