#include <nsgbuf_object.h>
#include <nsshader.h>
#include <nsengine.h>

nsgbuf_object::nsgbuf_object() :
m_multisample_level(0)
{}

nsgbuf_object::~nsgbuf_object()
{}

void nsgbuf_object::debug_blit(const ivec2 & scrn)
{
	ivec2 hlf = scrn / 2;

	set_target(nsgl_framebuffer::fb_read);
	bind();

	gl_err_check("pre nsfb_object::debug_blit");
	set_read_buffer(nsgl_framebuffer::attach_point(nsgl_framebuffer::att_color + col_diffuse));
	glBlitFramebuffer(0, 0, scrn.w, scrn.h, 0, 0, hlf.w, hlf.h, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	set_read_buffer(nsgl_framebuffer::attach_point(nsgl_framebuffer::att_color + col_position));
	glBlitFramebuffer(0, 0, scrn.w, scrn.h, 0, hlf.h, hlf.w, scrn.h, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	set_read_buffer(nsgl_framebuffer::attach_point(nsgl_framebuffer::att_color + col_normal));
	glBlitFramebuffer(0, 0, scrn.w, scrn.h, hlf.w, hlf.h, scrn.w, scrn.h, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	gl_err_check("post nsfb_object::debug_blit");

	set_read_buffer(nsgl_framebuffer::att_none);
	unbind();
}

nsgl_framebuffer::attachment * nsgbuf_object::color(uint32 att_type)
{
    // If this check fails it probably means the GBuffer was not initialized
	if (att_type >= attrib_count)
	{
		dprint("nsgbuf_object::color Parameter att_type is larger than the color attachments vector");
		return NULL;
	}
	return att(nsgl_framebuffer::attach_point(nsgl_framebuffer::att_color + att_type));
}

nsgl_framebuffer::attachment * nsgbuf_object::depth()
{
	return att(nsgl_framebuffer::att_depth_stencil);
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
	nsgl_framebuffer::attachment * att = NULL;

	bind();

    // Depth attachment
	att = create_renderbuffer_attachment(nsgl_framebuffer::att_depth_stencil, 0, GL_DEPTH32F_STENCIL8);

	tex_params lin;
	lin.anistropic_filtering = 4.0f;
	lin.min_filter = tmin_linear;
	lin.mag_filter = tmag_linear;

	// The order these are added here determines the layout in the shader
	// Diffuse color data texture: layout = 0
	att = create_texture_attachment<nstex2d>(
		"GBufferDiffuse",
		nsgl_framebuffer::attach_point(nsgl_framebuffer::att_color + col_diffuse),
		G_DIFFUSE_TEX_UNIT,
		tex_rgb,
		tex_float,
		lin);	

	lin.anistropic_filtering = 0.0f;
	lin.min_filter = tmin_nearest;
	lin.mag_filter = tmag_nearest;

	att = create_texture_attachment<nstex2d>(
		"GBufferPicking",
		nsgl_framebuffer::attach_point(nsgl_framebuffer::att_color + col_picking),
		G_PICKING_TEX_UNIT,
		tex_irgba,
		tex_u32,
		lin);	

	att = create_texture_attachment<nstex2d>(
		"GBufferPosition",
		nsgl_framebuffer::attach_point(nsgl_framebuffer::att_color + col_position),
		G_WORLD_POS_TEX_UNIT,
		tex_rgba,
		tex_float,
		lin);

	att = create_texture_attachment<nstex2d>(
		"GBufferNormal",
		nsgl_framebuffer::attach_point(nsgl_framebuffer::att_color + col_normal),
		G_NORMAL_TEX_UNIT,
		tex_rgb,
		tex_float,
		lin);
	
	update_draw_buffers();
	return (att != NULL);
}

