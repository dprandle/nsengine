#include <nsgbuf_object.h>
#include <nsshader.h>
#include <nsengine.h>

nsgbuf_object::nsgbuf_object() :
m_tex_fb(NULL),
m_multisample_level(0),
m_screen_size(0)
{}

nsgbuf_object::~nsgbuf_object()
{}

void nsgbuf_object::bind()
{
	m_tex_fb->set_target(nsfb_object::fb_draw);
	m_tex_fb->bind();
}

void nsgbuf_object::debug_blit(const ivec2 & scrn)
{
	ivec2 hlf = scrn / 2;

	m_tex_fb->set_target(nsfb_object::fb_read);
	m_tex_fb->bind();


	glDisable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_BLEND);
	

	m_tex_fb->set_read_buffer(nsfb_object::attach_point(nsfb_object::att_color + col_diffuse));
	glBlitFramebuffer(0, 0, scrn.w, scrn.h, 0, 0, hlf.w, hlf.h, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	GLError("nsfb_object::debugBlit");

	m_tex_fb->set_read_buffer(nsfb_object::attach_point(nsfb_object::att_color + col_position));
	glBlitFramebuffer(0, 0, scrn.w, scrn.h, 0, hlf.h, hlf.w, scrn.h, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	GLError("nsfb_object::debugBlit2");

	m_tex_fb->set_read_buffer(nsfb_object::attach_point(nsfb_object::att_color + col_normal));
	glBlitFramebuffer(0, 0, scrn.w, scrn.h, hlf.w, hlf.h, scrn.w, scrn.h, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	GLError("nsfb_object::debugBlit3");

	m_tex_fb->set_read_buffer(nsfb_object::attach_point(nsfb_object::att_color + col_material));
	glBlitFramebuffer(0, 0, scrn.w, scrn.h, hlf.w, 0, scrn.w, hlf.h, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	GLError("nsfb_object::debugBlit4");

	m_tex_fb->set_read_buffer(nsfb_object::att_none);
	m_tex_fb->unbind();
	m_tex_fb->set_target(nsfb_object::fb_read_draw);
}

void nsgbuf_object::disable_textures()
{
	m_tex_fb->disable_textures();
}

void nsgbuf_object::enabled_color_write(bool pEnable)
{
	if (pEnable)
		m_tex_fb->update_draw_buffers();
	else
		m_tex_fb->set_draw_buffer(nsfb_object::att_none);
}

void nsgbuf_object::enable_textures()
{
	m_tex_fb->enable_textures();
}

nsfb_object::attachment * nsgbuf_object::color(color_attachment_t pType)
{
    // If this check fails it probably means the GBuffer was not initialized
	if (pType >= attrib_count)
	{
		dprint("NSGFrameBuffer::getColorAttachment Parameter pType is larger than the color attachments vector");
		return NULL;
	}
	return m_tex_fb->att(nsfb_object::attach_point(nsfb_object::att_color + pType));
}

nsfb_object::attachment * nsgbuf_object::depth()
{
	return m_tex_fb->att(nsfb_object::att_depth_stencil);
}

nsfb_object * nsgbuf_object::fb()
{
	return m_tex_fb;
}

void nsgbuf_object::resize_fb(uint32 w, uint32 h)
{
	m_tex_fb->resize(w, h);
}

void nsgbuf_object::resize_fb(const uivec2 & size_)
{
	m_tex_fb->resize(size_);
}

void nsgbuf_object::set_fb(nsfb_object * fb)
{
	m_tex_fb = fb;
}

void nsgbuf_object::resize_screen(uint32 w, uint32 h)
{
	m_screen_size.set(w, h);
}

void nsgbuf_object::resize_screen(const uivec2 & size_)
{
	m_screen_size = size_;
}

void nsgbuf_object::init()
{
	if (m_tex_fb == NULL)
		return;

	if (m_tex_fb->size().x == 0 || m_tex_fb->size().y == 0)
		return;

	// Bind the resource - only in this subclass will we bind in an init function
	if (!_create_texture_attachments())
	{
		dprint("NSGFrameBuffer::init - Error in adding a color attachment - see previous error message - not updating draw buffers");
	}
}

void nsgbuf_object::unbind()
{
	m_tex_fb->unbind();
}

bool nsgbuf_object::_create_texture_attachments()
{
	// Attach the different textures for the Gbuffer - doing it explicitly using the enum because
	// its a lot less confusing later than if you use a for loop to attach the colors.. to add more
	// texture attachments to the gbuffer simply add an enum value and two lines to this function
	bool flag = true;
	nsfb_object::attachment * att = NULL;

	m_tex_fb->bind();
	// Depth attachment
	att = m_tex_fb->create(nsfb_object::att_depth_stencil, 0, GL_DEPTH32F_STENCIL8);

	// The order these are added here determines the layout in the shader
	// Diffuse color data texture: layout = 0
	att = m_tex_fb->create<nstex2d>("GBufferDiffuse", nsfb_object::attach_point(nsfb_object::att_color + col_diffuse), G_DIFFUSE_TEX_UNIT, GL_RGBA8, GL_RGBA, GL_FLOAT);
	if (att != NULL)
	{
		att->m_texture->set_parameter_f(nstexture::min_filter, GL_LINEAR);
		att->m_texture->set_parameter_f(nstexture::mag_filter, GL_LINEAR);
	}

	// Picking data texture: layout = 1
	att = m_tex_fb->create<nstex2d>("GBufferPicking", nsfb_object::attach_point(nsfb_object::att_color + col_picking), G_PICKING_TEX_UNIT, GL_RGB32UI, GL_RGB_INTEGER, GL_UNSIGNED_INT);
	if (att != NULL)
	{
		att->m_texture->set_parameter_i(nstexture::min_filter, GL_NEAREST);
		att->m_texture->set_parameter_i(nstexture::mag_filter, GL_NEAREST);
	}

	// Position data texture: layout = 2
	att = m_tex_fb->create<nstex2d>("GBufferPosition", nsfb_object::attach_point(nsfb_object::att_color + col_position), G_WORLD_POS_TEX_UNIT, GL_RGB32F, GL_RGB, GL_FLOAT);
	if (att != NULL)
	{
		att->m_texture->set_parameter_f(nstexture::min_filter, GL_NEAREST);
		att->m_texture->set_parameter_f(nstexture::mag_filter, GL_NEAREST);
	}

	// Normal data texture: layout = 3
	att = m_tex_fb->create<nstex2d>("GBufferNormal", nsfb_object::attach_point(nsfb_object::att_color + col_normal), G_NORMAL_TEX_UNIT, GL_RGBA32F, GL_RGBA, GL_FLOAT);
	if (att != NULL)
	{
		att->m_texture->set_parameter_f(nstexture::min_filter, GL_NEAREST);
		att->m_texture->set_parameter_f(nstexture::mag_filter, GL_NEAREST);
	}

	// Normal data texture: layout = 4
	att = m_tex_fb->create<nstex2d>("GBufferMaterial", nsfb_object::attach_point(nsfb_object::att_color + col_material), G_MATERIAL_TEX_UNIT, GL_RGBA32F, GL_RGBA, GL_FLOAT);
	if (att != NULL)
	{
		att->m_texture->set_parameter_f(nstexture::min_filter, GL_NEAREST);
		att->m_texture->set_parameter_f(nstexture::mag_filter, GL_NEAREST);
	}
	m_tex_fb->update_draw_buffers();
	return (att != NULL);
}

