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
#include <nsopengl_driver.h>

nsgl_object::nsgl_object()
{
	for (uint32 i = 0; i < MAX_CONTEXT_COUNT; ++i)
		m_gl_name[i] = 0;
}

nsgl_object::~nsgl_object()
{
	for (uint32 i = 0; i < MAX_CONTEXT_COUNT; ++i)
	{
		if (m_gl_name[i] != 0)
		{
			dprint("nsgl_object::~nsgl_object Warning - did not delete gl resource from context " + std::to_string(i));
		}
	}
}

bool nsgl_object::all_ids_released()
{
	for (uint32 i = 0; i < MAX_CONTEXT_COUNT; ++i)
	{
		if (m_gl_name[i] != 0)
		{
			return false;
		}
	}
	return true;
}

uint32 nsgl_object::gl_id() const
{
	nsopengl_driver * gl_driver = static_cast<nsopengl_driver*>(nse.video_driver());
	return m_gl_name[gl_driver->current_context()->context_id];
}

void nsgl_object::set_gl_id(uint32 id)
{
	nsopengl_driver * gl_driver = static_cast<nsopengl_driver*>(nse.video_driver());
	m_gl_name[gl_driver->current_context()->context_id] = id;
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

