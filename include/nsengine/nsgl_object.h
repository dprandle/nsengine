/*! 
	\file nsgl_object.h
	
	\brief Header file for NSGLObject class

	This file contains all of the neccessary declarations for the NSGLObject class.

	\author Daniel Randle
	\date November 2 2013
	\copywrite Earth Banana Games 2013
*/

#ifndef NSBUFFER_H
#define NSBUFFER_H

#include <nsglobal.h>

class NSGLObject
{
public:
	NSGLObject();
	virtual ~NSGLObject();
	virtual void bind()=0;
	virtual void initGL()=0;
	virtual void release()=0;
	virtual void unbind()=0;

	uint32 glid();

protected:
	uint32 mGLName;
};

#endif
