/*!
\file nsxfb_object

\brief Header file for nsxfb_object class

This file contains all of the neccessary declarations for the nsxfb_object class.

\author Daniel Randle
\date November 2 2013
\copywrite Earth Banana Games 2013
*/

#ifndef NSTFORMFB_H
#define NSTFORMFB_H

#include <nsgl_object.h>

class nsxfb_object : public nsgl_object
{
public:

	enum primitive_mode {
		gl_points = GL_POINTS,
		gl_lines = GL_LINES,
		gl_triangles = GL_TRIANGLES
	};

	nsxfb_object();
	void begin();
	void bind();
	void end();
	primitive_mode primitive();
	void init_gl();
	void release();
	void set_primitive(primitive_mode mode_);
	void set_update(bool update_);
	void unbind();
	bool update();
private:
	primitive_mode m_prim_mode;
	bool m_update;
};

#endif
