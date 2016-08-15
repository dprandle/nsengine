/*!
\file nsgl_xfb

\brief Header file for nsgl_xfb class

This file contains all of the neccessary declarations for the nsgl_xfb class.

\author Daniel Randle
\date November 2 2013
\copywrite Earth Banana Games 2013
*/

#ifndef NSGL_XFB_H
#define NSGL_XFB_H

#include <opengl/nsgl_object.h>
#include <opengl/glew.h>

struct nsgl_xfb : public nsgl_obj
{
	enum primitive_mode {
		gl_points = GL_POINTS,
		gl_lines = GL_LINES,
		gl_triangles = GL_TRIANGLES
	};

	nsgl_xfb();

	void begin();

	void bind();

	void end();

	void init();

	void release();

	void unbind();

	primitive_mode prim_mode;
	bool update;
};

#endif
