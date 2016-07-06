/*! 
	\file nsgl_framebuffer.h
	
	\brief Header file for nsfb_object class

	This file contains all of the neccessary declarations for the nsfb_object class.

	\author Daniel Randle
	\date November 2 2013
	\copywrite Earth Banana Games 2013
*/

#ifndef NSFRAMEBUFFER_H
#define NSFRAMEBUFFER_H

#include <nsengine.h>
#include <nsgl_object.h>
#include <nstexture.h>
#include <nsplugin_manager.h>

struct nsgl_renderbuffer;

struct nsgl_framebuffer : public nsgl_obj
{
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
		nsgl_renderbuffer * m_renderbuf;
		attach_point m_att_point;
		uint32 m_tex_unit;
		uint32 m_owning_fb;
	};

	typedef std::vector<attachment*> attachment_array;
	typedef std::vector<GLenum> attachment_point_array;

	nsgl_framebuffer();
	~nsgl_framebuffer();

	// On attachment the frame buffer is responsible for destroying the texture
	bool add(attachment * att_, bool overwrite_=false);

	void bind();

	uivec3 pick(int pixel_x, int pixel_y, uint32 att_index);

	uivec3 pick(const ivec2 & pixel_xy, uint32 att_index);

	attachment * create_renderbuffer_attachment(
		attach_point att_point_,
		int32 internal_format_,
		bool overwrite_ = true);

	template<class tex_type>
	attachment * create_texture_attachment(
		const nsstring & pName,
		attach_point att_point_,
		uint32 tex_unit_,
		tex_format format_,
		pixel_component_type pixel_type_,
		tex_params pms = tex_params(),
		bool mipmaps=false)
	{
		nsplugin * plg = nsep.get(ENGINE_PLUG);
        tex_type * tex = plg->get<tex_type>(pName);
        if (tex == nullptr)
            tex = plg->create<tex_type>(pName);
		tex->set_format(format_);
		tex->set_parameters(pms);
		tex->enable_mipmap_autogen(mipmaps);
		tex->set_component_data_type(pixel_type_);
		tex->resize(size, false);
		tex->video_update();

		attachment * att = new attachment();
		att->m_att_point = att_point_;
		att->m_texture = tex;
		att->m_owning_fb = gl_id;
		att->m_tex_unit = tex_unit_;
		if (!add(att, true))
		{
			delete att;
			att = nullptr;
		}
		return att;	
	}

	attachment * att(attach_point att_point_);

	bool has(attach_point att_point_);

	virtual void init();

	virtual void release();

	virtual void resize(int32 w, int32 h=0, uint32 layers_=0);

	void resize(const ivec2 & size_, uint32 layers_=0);

	bool set_cube_face(attach_point att_point_, uint8 face_);

	void set_draw_buffer(attach_point att_point_);

	void set_draw_buffers(attachment_point_array * att_array_);

	void set_read_buffer(uint32 att_point);
	
	void unbind();

	void update_draw_buffers();

	ivec2 size;
	fb_target target;
	attachment * depth_stencil_att;
	attachment_array color_atts;
};

#endif
