#include <nsshadowbuf_object.h>
#include <nsshader.h>
#include <nsfb_object.h>

nsshadow_tex2d_target::nsshadow_tex2d_target()
{}

nsshadow_tex2d_target::~nsshadow_tex2d_target()
{}

void nsshadow_tex2d_target::init()
{
	set_target(nsfb_object::fb_draw);
	bind();
	set_draw_buffer(nsfb_object::att_none);
	nsfb_object::attachment * att = create<nstex2d>(
		"shadow_map_2d",
		nsfb_object::att_depth,
		SHADOW_TEX_UNIT,
		GL_DEPTH_COMPONENT,
		GL_DEPTH_COMPONENT,
		GL_FLOAT);
	att->m_texture->set_parameter_i(nstexture::min_filter, GL_LINEAR);
	att->m_texture->set_parameter_i(nstexture::mag_filter, GL_LINEAR);
	att->m_texture->set_parameter_i(nstexture::compare_mode, GL_COMPARE_REF_TO_TEXTURE);
	att->m_texture->set_parameter_i(nstexture::compare_func, GL_LESS);
	att->m_texture->set_parameter_i(nstexture::wrap_s, GL_CLAMP_TO_EDGE);
	att->m_texture->set_parameter_i(nstexture::wrap_t, GL_CLAMP_TO_EDGE);
}


nsshadow_tex_cubemap_target::nsshadow_tex_cubemap_target()
{}

nsshadow_tex_cubemap_target::~nsshadow_tex_cubemap_target()
{}

void nsshadow_tex_cubemap_target::init()
{
	set_target(nsfb_object::fb_draw);
	bind();
	set_draw_buffer(nsfb_object::att_none);
	nsfb_object::attachment * att3 = create<nstex_cubemap>(
		"point_light_shadow",
		nsfb_object::att_depth,
		SHADOW_TEX_UNIT,
		GL_DEPTH_COMPONENT,
		GL_DEPTH_COMPONENT,
		GL_FLOAT);
	att3->m_texture->set_parameter_i(nstexture::min_filter, GL_LINEAR);
	att3->m_texture->set_parameter_i(nstexture::mag_filter, GL_LINEAR);
	att3->m_texture->set_parameter_i(nstexture::compare_mode, GL_COMPARE_REF_TO_TEXTURE);
	att3->m_texture->set_parameter_i(nstexture::compare_func, GL_LEQUAL);
	att3->m_texture->set_parameter_i(nstexture::wrap_s, GL_CLAMP_TO_EDGE);
	att3->m_texture->set_parameter_i(nstexture::wrap_t, GL_CLAMP_TO_EDGE);
	att3->m_texture->set_parameter_i(nstexture::wrap_r, GL_CLAMP_TO_EDGE);
}
