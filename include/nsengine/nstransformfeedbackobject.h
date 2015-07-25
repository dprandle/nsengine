/*!
\file nstransformfeedbackobject.h

\brief Header file for NSTransformFeedbackObject class

This file contains all of the neccessary declarations for the NSTransformFeedbackObject class.

\author Daniel Randle
\date November 2 2013
\copywrite Earth Banana Games 2013
*/

#ifndef NSTFORMFB_H
#define NSTFORMFB_H

#include <nsglobject.h>
#include <myGL\glew.h>
#include <vector>
#include <nstexture.h>

class NSTransformFeedbackObject : public NSGLObject
{
public:

	enum PrimitiveMode {
		Points = GL_POINTS,
		Lines = GL_LINES,
		Triangles = GL_TRIANGLES
	};

	NSTransformFeedbackObject();
	void begin();
	void bind();
	void end();
	PrimitiveMode primitive();
	void initGL();
	void release();
	void setPrimitive(PrimitiveMode pMode);
	void setUpdate(nsbool pUpdate);
	void unbind();
	nsbool update();
private:
	PrimitiveMode mPrimMode;
	nsbool mUpdate;
};

#endif