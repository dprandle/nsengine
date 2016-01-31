#include <nsgbuf_object.h>
#include <nsshader.h>
#include <nsgl_context.h>
#include <nsengine.h>

nsgbuf_object::nsgbuf_object() :
m_multisample_level(0)
{}

nsgbuf_object::~nsgbuf_object()
{}

uivec3 nsgbuf_object::pick(float mouse_x, float mouse_y)
{
	ivec2 tex_dim(mouse_x*m_size.x,mouse_y*m_size.y);
	set_target(nsfb_object::fb_read);
	bind();
	set_read_buffer(nsfb_object::attach_point(nsfb_object::att_color + nsgbuf_object::col_picking));
	uivec3 index;
	glReadPixels(tex_dim.x, tex_dim.y, 1, 1, GL_RGB_INTEGER, GL_UNSIGNED_INT, &index);
	gl_err_check("nsselection_system::pick");
	set_read_buffer(nsfb_object::att_none);
	return index;
}

void nsgbuf_object::debug_blit(const ivec2 & scrn)
{
	ivec2 hlf = scrn / 2;

	set_target(nsfb_object::fb_read);
	bind();

	set_read_buffer(nsfb_object::attach_point(nsfb_object::att_color + col_diffuse));
	glBlitFramebuffer(0, 0, scrn.w, scrn.h, 0, 0, hlf.w, hlf.h, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	set_read_buffer(nsfb_object::attach_point(nsfb_object::att_color + col_position));
	glBlitFramebuffer(0, 0, scrn.w, scrn.h, 0, hlf.h, hlf.w, scrn.h, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	set_read_buffer(nsfb_object::attach_point(nsfb_object::att_color + col_normal));
	glBlitFramebuffer(0, 0, scrn.w, scrn.h, hlf.w, hlf.h, scrn.w, scrn.h, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	set_read_buffer(nsfb_object::att_none);
	unbind();
}

nsfb_object::attachment * nsgbuf_object::color(uint32 att_type)
{
    // If this check fails it probably means the GBuffer was not initialized
	if (att_type >= attrib_count)
	{
		dprint("NSGFrameBuffer::getColorAttachment Parameter att_type is larger than the color attachments vector");
		return NULL;
	}
	return att(nsfb_object::attach_point(nsfb_object::att_color + att_type));
}

nsfb_object::attachment * nsgbuf_object::depth()
{
	return att(nsfb_object::att_depth_stencil);
}

void nsgbuf_object::init()
{
	if (size().x == 0 || size().y == 0)
		return;

	// Bind the resource - only in this subclass will we bind in an init function
	if (!_create_texture_attachments())
	{
		dprint("NSGFrameBuffer::init - Error in adding a color attachment - see previous error message - not updating draw buffers");
	}
}

bool nsgbuf_object::_create_texture_attachments()
{
	// Attach the different textures for the Gbuffer - doing it explicitly using the enum because
	// its a lot less confusing later than if you use a for loop to attach the colors.. to add more
	// texture attachments to the gbuffer simply add an enum value and two lines to this function
	bool flag = true;
	nsfb_object::attachment * att = NULL;

	bind();
	// Depth attachment
	att = create(nsfb_object::att_depth_stencil, 0, GL_DEPTH32F_STENCIL8);

	// The order these are added here determines the layout in the shader
	// Diffuse color data texture: layout = 0
	att = create<nstex2d>("GBufferDiffuse", nsfb_object::attach_point(nsfb_object::att_color + col_diffuse), G_DIFFUSE_TEX_UNIT, GL_RGB8, GL_RGB, GL_FLOAT);
	if (att != NULL)
	{
		att->m_texture->set_parameter_f(nstexture::min_filter, GL_LINEAR);
		att->m_texture->set_parameter_f(nstexture::mag_filter, GL_LINEAR);
	}

	// Picking data texture: layout = 1
	att = create<nstex2d>("GBufferPicking", nsfb_object::attach_point(nsfb_object::att_color + col_picking), G_PICKING_TEX_UNIT, GL_RGBA32UI, GL_RGBA_INTEGER, GL_UNSIGNED_INT);
	if (att != NULL)
	{
		att->m_texture->set_parameter_i(nstexture::min_filter, GL_NEAREST);
		att->m_texture->set_parameter_i(nstexture::mag_filter, GL_NEAREST);
	}

	// Position data texture: layout = 2
	att = create<nstex2d>("GBufferPosition", nsfb_object::attach_point(nsfb_object::att_color + col_position), G_WORLD_POS_TEX_UNIT, GL_RGBA32F, GL_RGBA, GL_FLOAT);
	if (att != NULL)
	{
		att->m_texture->set_parameter_f(nstexture::min_filter, GL_NEAREST);
		att->m_texture->set_parameter_f(nstexture::mag_filter, GL_NEAREST);
	}

	// Normal data texture: layout = 3
	att = create<nstex2d>("GBufferNormal", nsfb_object::attach_point(nsfb_object::att_color + col_normal), G_NORMAL_TEX_UNIT, GL_RGB16F, GL_RGB, GL_FLOAT);
	if (att != NULL)
	{
		att->m_texture->set_parameter_f(nstexture::min_filter, GL_NEAREST);
		att->m_texture->set_parameter_f(nstexture::mag_filter, GL_NEAREST);
	}
	update_draw_buffers();
	return (att != NULL);
}

