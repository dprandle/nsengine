/*!
\file nstransformfeedbackobject.cpp

\brief Definition file for NSTransformFeedbackObject class

This file contains all of the neccessary definitions for the NSTransformFeedbackObject class.

\author Daniel Randle
\date November 2 2013
\copywrite Earth Banana Games 2013
*/


#include <nstransformfeedbackobject.h>

NSTransformFeedbackObject::NSTransformFeedbackObject():
mPrimMode(Triangles),
NSGLObject(),
mUpdate(true)
{}

void NSTransformFeedbackObject::begin()
{
	glBeginTransformFeedback(mPrimMode);
	GLError("NSTransformFeedbackObject::begin");
}

void NSTransformFeedbackObject::bind()
{
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, mGLName);
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

void NSTransformFeedbackObject::initGL()
{
	glGenTransformFeedbacks(1, &mGLName);
	GLError("NSTransformFeedbackObject::initGL");
}

void NSTransformFeedbackObject::release()
{
	glDeleteTransformFeedbacks(1, &mGLName);
	mGLName = 0;
	GLError("NSTransformFeedbackObject::release");
}

void NSTransformFeedbackObject::setPrimitive(PrimitiveMode pMode)
{
	mPrimMode = pMode;
}

void NSTransformFeedbackObject::setUpdate(nsbool pUpdate)
{
	mUpdate = pUpdate;
}

void NSTransformFeedbackObject::unbind()
{
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
	GLError("NSTransformFeedbackObject::unbind");
}

nsbool NSTransformFeedbackObject::update()
{
	return mUpdate;
}