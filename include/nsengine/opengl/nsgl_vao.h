/*!
\file nsgl_vao.h

\brief Header file for nsvertex_array_object class

This file contains all of the neccessary declarations for the nsvertex_array_object class.

\author Daniel Randle
\date November 2 2013
\copywrite Earth Banana Games 2013
*/

#ifndef NSGL_VAO_H
#define NSGL_VAO_H

#include <opengl/nsgl_object.h>
#include <nsmap.h>
#include <nsset.h>

struct nsgl_buffer;

struct nsgl_vao : public nsgl_obj
{
	typedef std::map<uint32,ui_set> buffer_map;

	nsgl_vao();

	void add(nsgl_buffer * buffer_, uint32 attrib_loc_);

	void bind();

	bool contains(nsgl_buffer * buffer_);

	void enable(uint32 attrib_loc_);

	void disable(uint32 attrib_loc_);

	void init();

	void remove(nsgl_buffer * buffer_);

	void remove(nsgl_buffer * buffer_, uint32 attrib_loc_);

	void release();

	void unbind();

	void vertex_attrib_div(uint32 attrib_loc_, uint32 pDivisor);

	void vertex_attrib_ptr(
		uint32 attrib_loc_,
		uint32 elements_per_attrib_,
		uint32 gl_element_type_,
		bool normalized_,
		uint32 attrib_stride_,
		uint32 byte_offset_ = 0);

	void vertex_attrib_I_ptr(
		uint32 attrib_loc_,
		uint32 elements_per_attrib_,
		uint32 gl_element_type_,
		uint32 attrib_stride_,
		uint32 byte_offset_ = 0);

	void vertex_attrib_L_ptr(
		uint32 attrib_loc_,
		uint32 elements_per_attrib_,
		uint32 gl_element_type_,
		uint32 attrib_stride_,
		uint32 byte_offset_ = 0);

	buffer_map owned_buffers;
};

#endif
