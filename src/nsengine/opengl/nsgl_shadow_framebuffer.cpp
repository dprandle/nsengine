#include <nsgl_shadow_framebuffer.h>
#include <nsgl_shader.h>
#include <nsgl_texture.h>
#include <nsgl_vid_objs.h>

nsshadow_tex2d_target::nsshadow_tex2d_target()
{}

nsshadow_tex2d_target::~nsshadow_tex2d_target()
{}

void nsshadow_tex2d_target::init(const nsstring & tex_name)
{
	nsgl_framebuffer::init();
	target = nsgl_framebuffer::fb_draw;
	bind();
	set_draw_buffer(nsgl_framebuffer::att_none);

	tex_params tp;
	tp.edge_behavior.set(te_clamp,te_clamp,te_clamp);
	tp.min_filter = tmin_linear;
	tp.depth_mode = tex_dm_compare;
	tp.depth_func = tex_dc_less;

	nsgl_framebuffer::attachment * att = create_texture_attachment<nstex2d>(
		tex_name,
		nsgl_framebuffer::att_depth,
		SHADOW_TEX_UNIT,
		tex_depth,
		tex_float,
		tp);
}


nsshadow_tex_cubemap_target::nsshadow_tex_cubemap_target()
{}

nsshadow_tex_cubemap_target::~nsshadow_tex_cubemap_target()
{}

void nsshadow_tex_cubemap_target::init(const nsstring & tex_name)
{
	nsgl_framebuffer::init();
	target = nsgl_framebuffer::fb_draw;
	bind();

	tex_params tp;
	tp.edge_behavior.set(te_clamp,te_clamp,te_clamp);
	tp.min_filter = tmin_linear;
	tp.depth_mode = tex_dm_compare;
	tp.depth_func = tex_dc_less;

	set_draw_buffer(nsgl_framebuffer::att_none);
	nsgl_framebuffer::attachment * att = create_texture_attachment<nstex_cubemap>(
		tex_name,
		nsgl_framebuffer::att_depth,
		SHADOW_TEX_UNIT,
		tex_depth,
		tex_float,
		tp);
		
}
