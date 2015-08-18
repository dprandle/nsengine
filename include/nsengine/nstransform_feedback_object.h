/*!
\file nstransform_feedback_object.h

\brief Header file for NSTransformFeedbackObject class

This file contains all of the neccessary declarations for the NSTransformFeedbackObject class.

\author Daniel Randle
\date November 2 2013
\copywrite Earth Banana Games 2013
*/

#ifndef NSTFORMFB_H
#define NSTFORMFB_H

#include <nsgl_object.h>
#include <myGL/glew.h>
#include <vector>
#include <nstexture.h>

class NSTransformFeedbackObject : public nsgl_object
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
	void init_gl();
	void release();
	void setPrimitive(PrimitiveMode pMode);
	void setUpdate(bool pUpdate);
	void unbind();
	bool update();
private:
	PrimitiveMode mPrimMode;
	bool mUpdate;
};

#endif
