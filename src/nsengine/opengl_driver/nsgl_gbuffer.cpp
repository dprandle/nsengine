#include <nsgl_gbuffer.h>
#include <nsgl_shader.h>
#include <nsengine.h>

nsgl_gbuffer::nsgl_gbuffer():
	nsgl_framebuffer()
{}

nsgl_gbuffer::~nsgl_gbuffer()
{}

void nsgl_gbuffer::debug_blit(const ivec2 & scrn)
{
	ivec2 hlf = scrn / 2;

	target = nsgl_framebuffer::fb_read;
	bind();

	set_read_buffer(nsgl_framebuffer::attach_point(nsgl_framebuffer::att_color + col_diffuse));
	glBlitFramebuffer(0, 0, scrn.w, scrn.h, 0, 0, hlf.w, hlf.h, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	gl_err_check("nsgl_gbuffer::debug_blit 1");
	
	set_read_buffer(nsgl_framebuffer::attach_point(nsgl_framebuffer::att_color + col_position));
	glBlitFramebuffer(0, 0, scrn.w, scrn.h, 0, hlf.h, hlf.w, scrn.h, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	gl_err_check("nsgl_gbuffer::debug_blit 2");
	
	set_read_buffer(nsgl_framebuffer::attach_point(nsgl_framebuffer::att_color + col_normal));
	glBlitFramebuffer(0, 0, scrn.w, scrn.h, hlf.w, hlf.h, scrn.w, scrn.h, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	gl_err_check("nsgl_gbuffer::debug_blit 3");

	set_read_buffer(nsgl_framebuffer::att_none);
	unbind();
}

nsgl_framebuffer::attachment * nsgl_gbuffer::color(uint32 att_type)
{
    // If this check fails it probably means the GBuffer was not initialized
	if (att_type >= attrib_count)
	{
		dprint("nsgl_gbuffer::color Parameter att_type is larger than the color attachments vector");
		return NULL;
	}
	return att(nsgl_framebuffer::attach_point(nsgl_framebuffer::att_color + att_type));
}

nsgl_framebuffer::attachment * nsgl_gbuffer::depth()
{
	return att(nsgl_framebuffer::att_depth_stencil);
}

void nsgl_gbuffer::init()
{
	if (size.x == 0 || size.y == 0)
		return;

	nsgl_framebuffer::init();

	// Bind the resource - only in this subclass will we bind in an init function
	if (!create_texture_attachments())
	{
		dprint("nsgl_gbuffer::init - Error in adding a color attachment - see previous error message - not updating draw buffers");
	}
}

bool nsgl_gbuffer::create_texture_attachments()
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
		"gbuffer_diffuse",
		nsgl_framebuffer::attach_point(nsgl_framebuffer::att_color + col_diffuse),
		G_DIFFUSE_TEX_UNIT,
		tex_rgb,
		tex_float,
		lin);	

	lin.anistropic_filtering = 0.0f;
	lin.min_filter = tmin_nearest;
	lin.mag_filter = tmag_nearest;

	att = create_texture_attachment<nstex2d>(
		"gbuffer_picking",
		nsgl_framebuffer::attach_point(nsgl_framebuffer::att_color + col_picking),
		G_PICKING_TEX_UNIT,
		tex_irgba,
		tex_u32,
		lin);	

	att = create_texture_attachment<nstex2d>(
		"gbuffer_wpos",
		nsgl_framebuffer::attach_point(nsgl_framebuffer::att_color + col_position),
		G_WORLD_POS_TEX_UNIT,
		tex_rgba,
		tex_float,
		lin);

	att = create_texture_attachment<nstex2d>(
		"gbuffer_normal",
		nsgl_framebuffer::attach_point(nsgl_framebuffer::att_color + col_normal),
		G_NORMAL_TEX_UNIT,
		tex_rgb,
		tex_float,
		lin);
	
	update_draw_buffers();
	return (att != NULL);
}

