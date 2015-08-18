/*!
\file nstransformfeedbackobject.cpp

\brief Definition file for NSTransformFeedbackObject class

This file contains all of the neccessary definitions for the NSTransformFeedbackObject class.

\author Daniel Randle
\date November 2 2013
\copywrite Earth Banana Games 2013
*/


#include <nstransform_feedback_object.h>

NSTransformFeedbackObject::NSTransformFeedbackObject():
mPrimMode(Triangles),
nsgl_object(),
mUpdate(true)
{}

void NSTransformFeedbackObject::begin()
{
	glBeginTransformFeedback(mPrimMode);
	GLError("NSTransformFeedbackObject::begin");
}

void NSTransformFeedbackObject::bind()
{
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_gl_name);
	GLError("NSTransformFeedbackObject::bind");
}

void NSTransformFeedbackObject::end()
{
	glEndTransformFeedback();
	GLError("NSTransformFeedbackObject::end");
}

NSTransformFeedbackObject::PrimitiveMode NSTransformFeedbackObject::primitive()
{
	return mPrimMode;
}

void NSTransformFeedbackObject::init_gl()
{
	glGenTransformFeedbacks(1, &m_gl_name);
	GLError("NSTransformFeedbackObject::initGL");
}

void NSTransformFeedbackObject::release()
{
	glDeleteTransformFeedbacks(1, &m_gl_name);
	m_gl_name = 0;
	GLError("NSTransformFeedbackObject::release");
}

void NSTransformFeedbackObject::setPrimitive(PrimitiveMode pMode)
{
	mPrimMode = pMode;
}

void NSTransformFeedbackObject::setUpdate(bool pUpdate)
{
	mUpdate = pUpdate;
}

void NSTransformFeedbackObject::unbind()
{
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
	GLError("NSTransformFeedbackObject::unbind");
}

bool NSTransformFeedbackObject::update()
{
	return mUpdate;
}
