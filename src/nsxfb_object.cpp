/*!
\file nstransformfeedbackobject.cpp

\brief Definition file for nsxfb_object class

This file contains all of the neccessary definitions for the nsxfb_object class.

\author Daniel Randle
\date November 2 2013
\copywrite Earth Banana Games 2013
*/


#include <nsxfb_object.h>
#include <nsgl_context.h>

nsxfb_object::nsxfb_object():
m_prim_mode(gl_triangles),
nsgl_object(),
m_update(true)
{}

void nsxfb_object::begin()
{
	glBeginTransformFeedback(m_prim_mode);
	gl_err_check("nsxfb_object::begin");
}

void nsxfb_object::bind()
{
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_gl_name);
	gl_err_check("nsxfb_object::bind");
}

void nsxfb_object::end()
{
	glEndTransformFeedback();
	gl_err_check("nsxfb_object::end");
}

nsxfb_object::primitive_mode nsxfb_object::primitive()
{
	return m_prim_mode;
}

void nsxfb_object::init_gl()
{
	glGenTransformFeedbacks(1, &m_gl_name);
	gl_err_check("nsxfb_object::initGL");
}

void nsxfb_object::release()
{
	glDeleteTransformFeedbacks(1, &m_gl_name);
	m_gl_name = 0;
	gl_err_check("nsxfb_object::release");
}

void nsxfb_object::set_primitive(primitive_mode pMode)
{
	m_prim_mode = pMode;
}

void nsxfb_object::set_update(bool pUpdate)
{
	m_update = pUpdate;
}

void nsxfb_object::unbind()
{
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
	gl_err_check("nsxfb_object::unbind");
}

bool nsxfb_object::update()
{
	return m_update;
}
