/*! 
	\file nsframebuffer.cpp
	
	\brief Definition file for nsfb_object class

	This file contains all of the neccessary definitions for the nsfb_object class.

	\author Daniel Randle
	\date November 2 2013
	\copywrite Earth Banana Games 2013
*/


#include <nsrenderbuf_object.h>
#include <nsfb_object.h>
#include <nsres_manager.h>
#include <nsengine.h>
#include <nsgl_texture.h>

nsgl_framebuffer::nsgl_framebuffer() :
	m_target(fb_read_draw),
	m_depth_stencil_att(NULL),
	m_color_atts(),
	nsgl_object()
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

	gl_err_check("pre nsfb_object::add");
	if (pAttachment->m_renderbuf != NULL)
	{
		glFramebufferRenderbuffer(
			m_target,
			pAttachment->m_att_point,
			GL_RENDERBUFFER,
			pAttachment->m_renderbuf->gl_id());
	}
	else
	{
		if (pAttachment->m_texture->type() == type_to_hash(nstex_cubemap))
		{
			glFramebufferTexture(
				m_target,
				pAttachment->m_att_point,
				pAttachment->m_texture->video_texture<nsgl_texture>()->gl_obj.gl_id(),
				0);
		}
		else
		{
			glFramebufferTexture2D(
				m_target,
				pAttachment->m_att_point,
				pAttachment->m_texture->video_texture<nsgl_texture>()->m_target,
				pAttachment->m_texture->video_texture<nsgl_texture>()->gl_obj.gl_id(),
				0);
		}
	}

	gl_err_check("post nsfb_object::add");

	if (pAttachment->m_att_point == att_depth ||
		pAttachment->m_att_point == att_stencil ||
		pAttachment->m_att_point == att_depth_stencil)
		m_depth_stencil_att = pAttachment;
	else
		m_color_atts.push_back(pAttachment);

#ifdef NSDEBUG_RT
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		dprint("nsfb_object::add Error in adding attachment - Error Code: " + std::to_string(status));
		return false;
	}
#endif
	return true;
}

uivec3 nsgl_framebuffer::pick(float mouse_x, float mouse_y, uint32 att_index)
{
	ivec2 tex_dim(mouse_x*m_size.x,mouse_y*m_size.y);
	set_target(nsgl_framebuffer::fb_read);
	bind();
	set_read_buffer(nsgl_framebuffer::att_color+att_index);
	uivec3 index;
	gl_err_check("pre nsfb_object::pick");
	glReadPixels(tex_dim.x, tex_dim.y, 1, 1, GL_RGB_INTEGER, GL_UNSIGNED_INT, &index);
	gl_err_check("post nsfb_object::pick");
	set_read_buffer(nsgl_framebuffer::att_none);
	return index;
}


void nsgl_framebuffer::bind() const
{
	// use mTarget - easily set with setTarget
	gl_err_check("pre nsfb_object::bind");
	glBindFramebuffer(m_target, gl_id());
	gl_err_check("post nsfb_object::bind");
}

void nsgl_framebuffer::init()
{}

nsgl_framebuffer::attachment * nsgl_framebuffer::create_renderbuffer_attachment(
	attach_point pAttPoint,
	uint32 pSampleNumber,
	int32 pInternalFormat,
	bool overwrite)
{
	nsgl_renderbuffer * rBuf = new nsgl_renderbuffer();

	// Set up render buffer
	rBuf->video_init();
	rBuf->set_internal_format(pInternalFormat);
	rBuf->set_multisample(pSampleNumber);
	rBuf->resize(m_size.w, m_size.h);
	rBuf->bind();
	// Allocate the necessary space for the render buffer
	rBuf->allocate();

	// Texture is left as NULL to indicate that we are using a texture at this attachment location on not a render buffer
	attachment * att = new attachment();
	att->m_att_point = pAttPoint;
	att->m_renderbuf = rBuf;
	att->m_owning_fb = gl_id();

	if (!add(att, overwrite))
	{
		dprint("nsfb_object::Attachment unable to add attachment at " + std::to_string(att->m_att_point));
		delete att;
		att = NULL;
	}

	return att;
}

nsgl_framebuffer::attachment * nsgl_framebuffer::att(attach_point pAttPoint)
{
	if (pAttPoint == att_depth || pAttPoint == att_stencil || pAttPoint == att_depth_stencil)
		return m_depth_stencil_att;

	for (uint32 i = 0; i < m_color_atts.size(); ++i)
	{
		if (m_color_atts[i]->m_att_point == pAttPoint)
			return m_color_atts[i];
	}

    // Return NULL attachment if nothing was found
	return NULL;
}

nsgl_framebuffer::fb_target nsgl_framebuffer::target()
{
	return m_target;
}

bool nsgl_framebuffer::has(attach_point pAttPoint)
{
    // Should return invalid attachment if there is no attachment found
	attachment * attmt = att(pAttPoint);
	return (attmt != NULL);
}

void nsgl_framebuffer::video_init()
{
	uint32 glid;
	glGenFramebuffers(1, &glid);
	set_gl_id(glid);
	gl_err_check("post nsfb_object::init_gl");
}

void nsgl_framebuffer::video_release()
{
	while (m_color_atts.begin() != m_color_atts.end())
	{
		if (m_color_atts.back()->m_owning_fb == gl_id())
		{
			delete m_color_atts.back();
			m_color_atts.pop_back();
		}
	}
	if (m_depth_stencil_att != NULL && m_depth_stencil_att->m_owning_fb == gl_id())
	{
		delete m_depth_stencil_att;
		m_depth_stencil_att = NULL;
	}

	// Attachment destructor deletes the render buffer and texture - which in turn release their
	// open gl resources
	uint32 glid = gl_id();
	glDeleteFramebuffers(1, &glid);
	set_gl_id(0);
	gl_err_check("post nsfb_object::release");
}

void nsgl_framebuffer::resize(int32 w, int32 h, uint32 layers)
{
	// Resize the frame buffer by resizing all associated textures
	// This is relatively expensive operation and should only
	// be done when neccessary
	m_size.set(w, h);

	attachment_array::iterator iter = m_color_atts.begin();
	uint32 m = 0;
	while (iter != m_color_atts.end())
	{
		if ((*iter)->m_renderbuf != NULL)
			(*iter)->m_renderbuf->resize(w, h);
		if ((*iter)->m_texture != NULL)
		{
			uint32 tt = (*iter)->m_texture->video_texture<nsgl_texture>()->m_target;
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

	if (m_depth_stencil_att != NULL)
	{
		if (m_depth_stencil_att->m_renderbuf != NULL)
			m_depth_stencil_att->m_renderbuf->resize(w, h);
		if (m_depth_stencil_att->m_texture != NULL)
		{
			uint32 tt = m_depth_stencil_att->m_texture->video_texture<nsgl_texture>()->m_target;
			if (tt == nsgl_texture::tex_1d)
				((nstex1d*)m_depth_stencil_att->m_texture)->resize(w);
			else if (tt == nsgl_texture::tex_2d || tt == nsgl_texture::tex_1d_array)
				((nstex2d*)m_depth_stencil_att->m_texture)->resize(ivec2(w, h));
			else if (tt == nsgl_texture::tex_cubemap)
				((nstex_cubemap*)m_depth_stencil_att->m_texture)->resize(ivec2(w, h));
			else if (tt == nsgl_texture::tex_3d || tt == nsgl_texture::tex_2d_array)
				((nstex3d*)m_depth_stencil_att->m_texture)->resize(ivec3(w, h, layers));
			m_depth_stencil_att->m_texture->bind();
			m_depth_stencil_att->m_texture->video_allocate();
		}
	}
}

void nsgl_framebuffer::resize(const ivec2 & size_, uint32 layers_)
{
	return resize(size_.x,size_.y,layers_);
}

const ivec2 & nsgl_framebuffer::size()
{
	return m_size;
}

bool nsgl_framebuffer::set_cube_face(attach_point pAttPoint, uint8 pFace)
{
	attachment * attmt = att(pAttPoint);
	if (attmt == NULL || attmt->m_texture == NULL || attmt->m_texture->type() != type_to_hash(nstex_cubemap))
		return false;

	glFramebufferTexture2D(
		m_target,
		pAttPoint,
		BASE_CUBEMAP_FACE + pFace,
		attmt->m_texture->video_texture<nsgl_texture>()->gl_obj.gl_id(),
		0);
	
	gl_err_check("post nsfb_object::set_cube_face");

#ifdef NSDEBUG_RT
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		nsstringstream ss;
		ss << "nsfb_object::setCubeAttachmentFace Error in adding attachment - Error Code: " << status;
		dprint(ss.str());
		return false;
	}
#endif
	return true;
}

void nsgl_framebuffer::set_draw_buffer(attach_point pAttPoint)
{
	glDrawBuffer(pAttPoint);
	gl_err_check("post nsfb_object::set_draw_buffer");
}

void nsgl_framebuffer::set_draw_buffers(attachment_point_array * pAttArray)
{
	if (pAttArray == NULL)
	{
		dprint("nsfb_object::set_draw_buffers - Error passed NULL attachment point array");
		return;
	}

	if (pAttArray->empty())
	{
		dprint("nsfb_object::set_draw_buffers - Error empty attachment point array");
		return;
	}
	glDrawBuffers(static_cast<GLsizei>(pAttArray->size()), &(*pAttArray)[0]);
	gl_err_check("post nsfb_object::set_draw_buffers");
}

void nsgl_framebuffer::set_read_buffer(uint32 att_point)
{
	glReadBuffer(att_point);
	gl_err_check("post nsfb_object::setReadBuffer");
}

void nsgl_framebuffer::set_target(fb_target pTarget)
{
	m_target = pTarget;
}

void nsgl_framebuffer::unbind() const
{
	// By unbind I just mean bind the main frame buffer back
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	gl_err_check("post nsfb_object::unbind");
}

void nsgl_framebuffer::update_draw_buffers()
{
	attachment_point_array bufferAttachments;

	attachment_array::iterator iter = m_color_atts.begin();
	while (iter != m_color_atts.end())
	{
		bufferAttachments.push_back( GLenum((*iter)->m_att_point) );
		++iter;
	}
	set_draw_buffers(&bufferAttachments);
}

nsgl_framebuffer::attachment::attachment(): m_texture(NULL),
m_renderbuf(NULL),
m_att_point(att_none),
m_owning_fb(0),
m_tex_unit(0)
{}

nsgl_framebuffer::attachment::~attachment()
{
	if (m_renderbuf != NULL)
		m_renderbuf->video_release();
	delete m_renderbuf;
}

bool nsgl_framebuffer::attachment::valid()
{
    // there are two main conditions for a valid attachement
    // 1) the attatchment point cannot be None AND
    // 2) the render buffer XOR the texture must not be NULL AND
	return ( m_att_point != att_none && ( (m_renderbuf != NULL) != (m_texture != NULL) ) );
}
