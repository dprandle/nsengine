/*!
  \file nsgl_xfb.cpp

  \brief Definition file for nsgl_xfb class

  This file contains all of the neccessary definitions for the nsgl_xfb class.

  \author Daniel Randle
  \date November 2 2013
  \copywrite Earth Banana Games 2013
*/

#include <nsengine.h>
#include <opengl/nsgl_xfb.h>
#include <opengl/nsgl_driver.h>

nsgl_xfb::nsgl_xfb():
	prim_mode(gl_triangles),
	update(true),
	nsgl_obj()
{}

void nsgl_xfb::begin()
{
	glBeginTransformFeedback(prim_mode);
	gl_err_check("nsgl_xfb::begin");
}

void nsgl_xfb::bind()
{
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, gl_id);
	gl_err_check("nsgl_xfb::bind");
}

void nsgl_xfb::end()
{
	glEndTransformFeedback();
	gl_err_check("nsgl_xfb::end");
}

void nsgl_xfb::init()
{
	if (gl_id != 0)
	{
		dprint("nsgl_xfb::init Error trying to initialize already initialized xfb obj");
		return;
	}
	glGenTransformFeedbacks(1, &gl_id);
	gl_err_check("nsgl_xfb::init");
}

void nsgl_xfb::release()
{
	glDeleteTransformFeedbacks(1, &gl_id);
	gl_id = 0;
	gl_err_check("nsgl_xfb::release");
}

void nsgl_xfb::unbind()
{
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
	gl_err_check("nsgl_xfb::unbind");
}
