/*!
\file nsvertex_array_object.h

\brief Header file for nsvertex_array_object class

This file contains all of the neccessary declarations for the nsvertex_array_object class.

\author Daniel Randle
\date November 2 2013
\copywrite Earth Banana Games 2013
*/

#ifndef NSVERTEX_ARRAY_OBJECT_H
#define NSVERTEX_ARRAY_OBJECT_H

#include <nsgl_object.h>
#include <nsmap.h>
#include <nsset.h>

class nsbuffer_object;

class nsvertex_array_object : public nsgl_object
{
public:
	typedef std::map<uint32,ui_set> buffer_map;
	nsvertex_array_object();

	void add(nsbuffer_object * buffer_, uint32 attrib_loc_);

	void bind();

	bool contains(nsbuffer_object * buffer_);

	void enable(uint32 attrib_loc_);

	void disable(uint32 attrib_loc_);

	void init_gl();

	void remove(nsbuffer_object * buffer_);

	void remove(nsbuffer_object * buffer_, uint32 attrib_loc_);

	void release();

	void unbind();

	void vertex_attrib_div(uint32 attrib_loc_, uint32 pDivisor);

	void vertex_attrib_ptr(uint32 attrib_loc_, uint32 elements_per_attrib_, uint32 gl_element_type_, bool normalized_, uint32 attrib_stride_, uint32 byte_offset_ = 0);

	void vertex_attrib_I_ptr(uint32 attrib_loc_, uint32 elements_per_attrib_, uint32 gl_element_type_, uint32 attrib_stride_, uint32 byte_offset_ = 0);

	void vertex_attrib_L_ptr(uint32 attrib_loc_, uint32 elements_per_attrib_, uint32 gl_element_type_, uint32 attrib_stride_, uint32 byte_offset_ = 0);

private:
	buffer_map m_owned_buffers;
};

#endif
