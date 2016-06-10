/*! 
  \file nsframebuffer.cpp
	
  \brief Definition file for nsgl_framebuffer class

  This file contains all of the neccessary definitions for the nsgl_framebuffer class.

  \author Daniel Randle
  \date November 2 2013
  \copywrite Earth Banana Games 2013
*/


#include <nsgl_renderbuffer.h>
#include <nsgl_framebuffer.h>
#include <nsres_manager.h>
#include <nsengine.h>
#include <nsgl_texture.h>

nsgl_framebuffer::nsgl_framebuffer() :
	target(fb_read_draw),
	depth_stencil_att(NULL),
	color_atts(),
	nsgl_obj()
{}

nsgl_framebuffer::~nsgl_framebuffer()
{}

bool nsgl_framebuffer::add(attachment * pAttachment, bool pOverwrite)
{
	if (pAttachment == NULL)
		return false;

	if ((has(pAttachment->m_att_point) && !pOverwrite) || !pAttachment->valid())
		return false;

	if (has(pAttachment->m_att_point) && att(pAttachment->m_att_point)->m_texture == pAttachment->m_texture)
		return false;

	if (pAttachment->m_renderbuf != NULL)
	{
		glFramebufferRenderbuffer(
			target,
			pAttachment->m_att_point,
			GL_RENDERBUFFER,
			pAttachment->m_renderbuf->gl_id);
	}
	else
	{
		if (pAttachment->m_texture->type() == type_to_hash(nstex_cubemap))
		{
			glFramebufferTexture(
				target,
				pAttachment->m_att_point,
				pAttachment->m_texture->video_texture<nsgl_texture>()->gl_id,
				0);
		}
		else
		{
			glFramebufferTexture2D(
				target,
				pAttachment->m_att_point,
				pAttachment->m_texture->video_texture<nsgl_texture>()->target,
				pAttachment->m_texture->video_texture<nsgl_texture>()->gl_id,
				0);
		}
	}

	gl_err_check("nsgl_framebuffer::add");

	if (pAttachment->m_att_point == att_depth ||
		pAttachment->m_att_point == att_stencil ||
		pAttachment->m_att_point == att_depth_stencil)
		depth_stencil_att = pAttachment;
	else
		color_atts.push_back(pAttachment);

#ifdef NSDEBUG_RT
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		dprint("nsgl_framebuffer::add Error in adding attachment - Error Code: " + std::to_string(status));
		return false;
	}
#endif
	return true;
}

uivec3 nsgl_framebuffer::pick(float mouse_x, float mouse_y, uint32 att_index)
{
	ivec2 tex_dim(mouse_x*size.x,mouse_y*size.y);
	target = nsgl_framebuffer::fb_read;
	bind();
	set_read_buffer(nsgl_framebuffer::att_color+att_index);
	uivec3 index;
	glReadPixels(tex_dim.x, tex_dim.y, 1, 1, GL_RGB_INTEGER, GL_UNSIGNED_INT, &index);
	gl_err_check("nsgl_framebuffer::pick");
	set_read_buffer(nsgl_framebuffer::att_none);
	return index;
}


void nsgl_framebuffer::bind()
{
	// use mTarget - easily set with setTarget
	glBindFramebuffer(target, gl_id);
	gl_err_check("nsgl_framebuffer::bind");
}

nsgl_framebuffer::attachment * nsgl_framebuffer::create_renderbuffer_attachment(
	attach_point att_point_,
	int32 internal_format_,
	bool overwrite)
{
	nsgl_renderbuffer * rBuf = new nsgl_renderbuffer();

	// Set up render buffer
	rBuf->init();
	rBuf->internal_format = internal_format_;
	rBuf->resize(size.w, size.h);
	rBuf->bind();
	// Allocate the necessary space for the render buffer
	rBuf->allocate();

	// Texture is left as NULL to indicate that we are using a texture at this attachment location on not a render buffer
	attachment * att = new attachment();
	att->m_att_point = att_point_;
	att->m_renderbuf = rBuf;
	att->m_owning_fb = gl_id;

	if (!add(att, overwrite))
	{
		dprint("nsgl_framebuffer::Attachment unable to add attachment at " + std::to_string(att->m_att_point));
		delete att;
		att = NULL;
	}

	return att;
}

nsgl_framebuffer::attachment * nsgl_framebuffer::att(attach_point att_point_)
{
	if (att_point_ == att_depth || att_point_ == att_stencil || att_point_ == att_depth_stencil)
		return depth_stencil_att;

	for (uint32 i = 0; i < color_atts.size(); ++i)
	{
		if (color_atts[i]->m_att_point == att_point_)
			return color_atts[i];
	}

    // Return NULL attachment if nothing was found
	return NULL;
}

bool nsgl_framebuffer::has(attach_point att_point_)
{
    // Should return invalid attachment if there is no attachment found
	attachment * attmt = att(att_point_);
	return (attmt != NULL);
}

void nsgl_framebuffer::init()
{
	if (gl_id != 0)
	{
		dprint("nsfb_framebuffer::init Error initializing framebuffer which is already initialized");
		return;
	}
	glGenFramebuffers(1, &gl_id);
	gl_err_check("nsgl_framebuffer::init");
}

void nsgl_framebuffer::release()
{
	while (color_atts.begin() != color_atts.end())
	{
		if (color_atts.back()->m_owning_fb == gl_id)
		{
			delete color_atts.back();
			color_atts.pop_back();
		}
	}
	if (depth_stencil_att != NULL && depth_stencil_att->m_owning_fb == gl_id)
	{
		delete depth_stencil_att;
		depth_stencil_att = NULL;
	}

	// Attachment destructor deletes the render buffer and texture - which in turn release their
	// open gl resources
	glDeleteFramebuffers(1, &gl_id);
	gl_id = 0;
	gl_err_check("nsgl_framebuffer::release");
}

void nsgl_framebuffer::resize(int32 w, int32 h, uint32 layers)
{
	// Resize the frame buffer by resizing all associated textures
	// This is relatively expensive operation and should only
	// be done when neccessary
	size.set(w, h);

	attachment_array::iterator iter = color_atts.begin();
	uint32 m = 0;
	while (iter != color_atts.end())
	{
		if ((*iter)->m_renderbuf != NULL)
			(*iter)->m_renderbuf->resize(w, h);
		if ((*iter)->m_texture != NULL)
		{
			uint32 tt = (*iter)->m_texture->video_texture<nsgl_texture>()->target;
			if (tt == nsgl_texture::tex_1d)
				((nstex1d*)(*iter)->m_texture)->resize(w);
			else if (tt == nsgl_texture ::tex_2d || tt == nsgl_texture::tex_1d_array)
				((nstex2d*)(*iter)->m_texture)->resize(ivec2(w, h));
			else if (tt == nsgl_texture::tex_cubemap)
				((nstex_cubemap*)(*iter)->m_texture)->resize(ivec2(w, h));
			else if (tt == nsgl_texture::tex_3d || tt == nsgl_texture::tex_2d_array)
				((nstex3d*)(*iter)->m_texture)->resize(ivec3(w, h, layers));
			(*iter)->m_texture->bind();
			(*iter)->m_texture->video_allocate();
		}
		++iter;
		++m;
	}

	if (depth_stencil_att != NULL)
	{
		if (depth_stencil_att->m_renderbuf != NULL)
			depth_stencil_att->m_renderbuf->resize(w, h);
		if (depth_stencil_att->m_texture != NULL)
		{
			uint32 tt = depth_stencil_att->m_texture->video_texture<nsgl_texture>()->target;
			if (tt == nsgl_texture::tex_1d)
				((nstex1d*)depth_stencil_att->m_texture)->resize(w);
			else if (tt == nsgl_texture::tex_2d || tt == nsgl_texture::tex_1d_array)
				((nstex2d*)depth_stencil_att->m_texture)->resize(ivec2(w, h));
			else if (tt == nsgl_texture::tex_cubemap)
				((nstex_cubemap*)depth_stencil_att->m_texture)->resize(ivec2(w, h));
			else if (tt == nsgl_texture::tex_3d || tt == nsgl_texture::tex_2d_array)
				((nstex3d*)depth_stencil_att->m_texture)->resize(ivec3(w, h, layers));
			depth_stencil_att->m_texture->bind();
			depth_stencil_att->m_texture->video_allocate();
		}
	}
}

void nsgl_framebuffer::resize(const ivec2 & size_, uint32 layers_)
{
	return resize(size_.x,size_.y,layers_);
}

bool nsgl_framebuffer::set_cube_face(attach_point att_point_, uint8 pFace)
{
	attachment * attmt = att(att_point_);
	if (attmt == NULL || attmt->m_texture == NULL || attmt->m_texture->type() != type_to_hash(nstex_cubemap))
		return false;

	glFramebufferTexture2D(
		target,
		att_point_,
		BASE_CUBEMAP_FACE + pFace,
		attmt->m_texture->video_texture<nsgl_texture>()->gl_id,
		0);
	
	gl_err_check("nsgl_framebuffer::set_cube_face");

#ifdef NSDEBUG_RT
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		nsstringstream ss;
		ss << "nsgl_framebuffer::set_cube_face Error in adding attachment - Error Code: " << status;
		dprint(ss.str());
		return false;
	}
#endif
	return true;
}

void nsgl_framebuffer::set_draw_buffer(attach_point att_point_)
{
	glDrawBuffer(att_point_);
	gl_err_check("nsgl_framebuffer::set_draw_buffer");
}

void nsgl_framebuffer::set_draw_buffers(attachment_point_array * pAttArray)
{
	if (pAttArray == NULL)
	{
		dprint("nsgl_framebuffer::set_draw_buffers - Error passed NULL attachment point array");
		return;
	}

	if (pAttArray->empty())
	{
		dprint("nsgl_framebuffer::set_draw_buffers - Error empty attachment point array");
		return;
	}
	glDrawBuffers(static_cast<GLsizei>(pAttArray->size()), &(*pAttArray)[0]);
	gl_err_check("nsgl_framebuffer::set_draw_buffers");
}

void nsgl_framebuffer::set_read_buffer(uint32 att_point)
{
	glReadBuffer(att_point);
	gl_err_check("nsgl_framebuffer::set_read_buffer");
}

void nsgl_framebuffer::unbind()
{
	// By unbind I just mean bind the main frame buffer back
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	gl_err_check("post nsgl_framebuffer::unbind");
}

void nsgl_framebuffer::update_draw_buffers()
{
	attachment_point_array bufferAttachments;

	attachment_array::iterator iter = color_atts.begin();
	while (iter != color_atts.end())
	{
		bufferAttachments.push_back( GLenum((*iter)->m_att_point) );
		++iter;
	}
	set_draw_buffers(&bufferAttachments);
}

nsgl_framebuffer::attachment::attachment():
	m_texture(NULL),
	m_renderbuf(NULL),
	m_att_point(att_none),
	m_owning_fb(0),
	m_tex_unit(0)
{}

nsgl_framebuffer::attachment::~attachment()
{
	if (m_renderbuf != NULL)
		m_renderbuf->release();
	delete m_renderbuf;
}

bool nsgl_framebuffer::attachment::valid()
{
    // there are two main conditions for a valid attachement
    // 1) the attatchment point cannot be None AND
    // 2) the render buffer XOR the texture must not be NULL AND
	return ( m_att_point != att_none && ( (m_renderbuf != NULL) != (m_texture != NULL) ) );
}
