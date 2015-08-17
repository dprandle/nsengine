/*! 
	\file nsglobject.cpp
	
	\brief Definition file for NSGLObject class

	This file contains all of the neccessary definitions for the NSGLObject class.

	\author Daniel Randle
	\date November 2 2013
	\copywrite Earth Banana Games 2013
*/

#include <nsgl_object.h>
#include <nslog_file.h>

NSGLObject::NSGLObject() :mGLName(0)
{}

NSGLObject::~NSGLObject()
{
	if (mGLName != 0)
		NSLogFile("Warning : Failed to call \"release()\" on NSBuffer object - openGL resource was not released","enginecreatedestroy.log");
}

uint32 NSGLObject::glid()
{
	return mGLName;
}
