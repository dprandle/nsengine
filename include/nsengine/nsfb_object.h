/*! 
	\file nsfb_object.h
	
	\brief Header file for nsfb_object class

	This file contains all of the neccessary declarations for the nsfb_object class.

	\author Daniel Randle
	\date November 2 2013
	\copywrite Earth Banana Games 2013
*/

#ifndef NSFRAMEBUFFER_H
#define NSFRAMEBUFFER_H

#include <nsgl_object.h>
#include <nstexture.h>

class nsrenderbuf_object;

class nsfb_object : public nsgl_object
{
public:

	enum attach_point {  
		att_none = GL_NONE,
		att_color = GL_COLOR_ATTACHMENT0,
		att_depth = GL_DEPTH_ATTACHMENT,
		att_stencil = GL_STENCIL_ATTACHMENT,
		att_depth_stencil = GL_DEPTH_STENCIL_ATTACHMENT };

	enum fb_target {
		fb_read = GL_READ_FRAMEBUFFER,
		fb_draw = GL_DRAW_FRAMEBUFFER,
		fb_read_draw = GL_FRAMEBUFFER };

	struct attachment
    {
		attachment();
		~attachment();
		bool valid();

		nstexture * m_texture;
		nsrenderbuf_object * m_renderbuf;
		attach_point m_att_point;
		uint32 m_tex_unit;
		uint32 m_owning_fb;
	};

	typedef std::vector<attachment*> attachment_array;
	typedef std::vector<GLenum> attachment_point_array;

	nsfb_object();
	~nsfb_object();

	// On attachment the frame buffer is responsible for destroying the texture
	bool add(attachment * att_, bool overwrite_=false);

	void bind() const;

	uivec3 pick(float norm_mouse_x, float norm_mouse_y, uint32 att_index);

	attachment * create(attach_point att_point_, uint32 sampler_num_, int32 internal_format_, bool overwrite_ = true);

	template<class tex_type>
	attachment * create(const nsstring & pName, attach_point att_point_, uint32 tex_unit_, int32 internal_format_, int32 format_, int32 pixel_data_t_)
	{
		tex_type * tex = new tex_type();

		// Set up the texture according to the format provided in the function arguements
		tex->init_gl();
		tex->rename(pName);
		tex->set_format(format_);
		tex->set_internal_format(internal_format_);
		tex->set_pixel_data_type(pixel_data_t_);
		tex->resize(m_size.x,m_size.y);
		tex->bind();
		// Fill the texture with NULL data
		tex->allocate(NULL);

		// Render buffer is left as NULL to indicate that we are using a texture at this attachment location on not a render buffer
		attachment * att = new attachment();
		att->m_att_point = att_point_;
		att->m_texture = tex;
		att->m_owning_fb = m_gl_name;
		att->m_tex_unit = tex_unit_;

		if (!add(att, true))
		{
			delete att;
			att = NULL;
		}

		return att;
	}

    // Will return attachment with NULL as renderbuffer pointer and texture pointer if
    // there is no attachment at the attachment point
	attachment * att(attach_point att_point_);

	const ivec2 & size();

	fb_target target();

	bool has(attach_point att_point_);

	virtual void init_gl();

	virtual void init();

	virtual void release();

	virtual void resize(int32 w, int32 h=0, uint32 layers_=0);

	virtual void resize(const ivec2 & size_, uint32 layers_=0);

	bool set_cube_face(attach_point att_point_, nstex_cubemap::cube_face face_);

	void set_draw_buffer(attach_point att_point_);

	void set_draw_buffers(attachment_point_array * att_array_);

	void set_read_buffer(uint32 att_point);

	void set_target(fb_target target_);
	
	void unbind() const;

	void update_draw_buffers();

protected:
	fb_target m_target;
	ivec2 m_size;

	attachment * m_depth_stencil_att;
	attachment_array m_color_atts;
};

#endif
