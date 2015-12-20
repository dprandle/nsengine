#include <nsshadowbuf_object.h>
#include <nsshader.h>
#include <nsfb_object.h>

nsshadowbuf_object::nsshadowbuf_object() :
m_spot_fb(),
m_point_fb(),
m_dir_fb()
{}

nsshadowbuf_object::~nsshadowbuf_object()
{}

void nsshadowbuf_object::bind(const map_t & pMType)
{
	switch (pMType)
	{
	case (Direction) :
		m_dir_fb->bind();
		break;
	case (Spot) :
		m_spot_fb->bind();
		break;
	case (Point) :
		m_point_fb->bind();
		break;
	}
}

void nsshadowbuf_object::set_point_face(nstex_cubemap::cube_face face_)
{
	m_spot_fb->set_cube_face(nsfb_object::att_depth, face_);
}

void nsshadowbuf_object::set_fb(nsfb_object * fb, const map_t & map)
{
	switch (map)
	{
	case (Direction) :
		m_dir_fb = fb;
		break;
	case (Spot) :
		m_spot_fb = fb;
		break;
	case (Point) :
		m_point_fb = fb;
		break;
	}
}

const uivec2 & nsshadowbuf_object::size(const map_t & pMap)
{
	switch (pMap)
	{
	case (Direction) :
		return m_dir_fb->size();
	case (Spot) :
		return m_spot_fb->size();
	case (Point) :
		return m_point_fb->size();
	default:
		throw(std::exception());
	}
}

nsfb_object * nsshadowbuf_object::fb(const map_t & pMap)
{
	switch (pMap)
	{
	case (Direction) :
		return m_dir_fb;
	case (Spot) :
		return m_spot_fb;
	case (Point) :
		return m_point_fb;
	default:
		return NULL;
	}
}

void nsshadowbuf_object::init()
{
	if (m_spot_fb == NULL || m_dir_fb == NULL || m_point_fb == NULL)
		return;

	m_spot_fb->set_target(nsfb_object::fb_draw);
	m_spot_fb->bind();
	m_spot_fb->set_draw_buffer(nsfb_object::att_none);
	nsfb_object::attachment * att = m_spot_fb->create<nstex2d>("SpotLightShadow", nsfb_object::att_depth, SHADOW_TEX_UNIT, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_FLOAT);
	att->m_texture->set_parameter_i(nstexture::min_filter, GL_LINEAR);
	att->m_texture->set_parameter_i(nstexture::mag_filter, GL_LINEAR);
	att->m_texture->set_parameter_i(nstexture::compare_mode, GL_COMPARE_REF_TO_TEXTURE);
	att->m_texture->set_parameter_i(nstexture::compare_func, GL_LESS);
	att->m_texture->set_parameter_i(nstexture::wrap_s, GL_CLAMP_TO_EDGE);
	att->m_texture->set_parameter_i(nstexture::wrap_t, GL_CLAMP_TO_EDGE);

	m_dir_fb->set_target(nsfb_object::fb_draw);
	m_dir_fb->bind();
	m_dir_fb->set_draw_buffer(nsfb_object::att_none);
	nsfb_object::attachment * att2 = m_dir_fb->create<nstex2d>("DirLightShadow", nsfb_object::att_depth, SHADOW_TEX_UNIT, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_FLOAT);
	att2->m_texture->set_parameter_i(nstexture::min_filter, GL_LINEAR);
	att2->m_texture->set_parameter_i(nstexture::mag_filter, GL_LINEAR);
	att2->m_texture->set_parameter_i(nstexture::compare_mode, GL_COMPARE_REF_TO_TEXTURE);
	att2->m_texture->set_parameter_i(nstexture::compare_func, GL_LEQUAL);
	att2->m_texture->set_parameter_i(nstexture::wrap_s, GL_CLAMP_TO_EDGE);
	att2->m_texture->set_parameter_i(nstexture::wrap_t, GL_CLAMP_TO_EDGE);

	m_point_fb->set_target(nsfb_object::fb_draw);
	m_point_fb->bind();
	m_point_fb->set_draw_buffer(nsfb_object::att_none);
	nsfb_object::attachment * att3 = m_point_fb->create<nstex_cubemap>("PointLightShadow", nsfb_object::att_depth, SHADOW_TEX_UNIT, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_FLOAT);
	att3->m_texture->set_parameter_i(nstexture::min_filter, GL_LINEAR);
	att3->m_texture->set_parameter_i(nstexture::mag_filter, GL_LINEAR);
	att3->m_texture->set_parameter_i(nstexture::compare_mode, GL_COMPARE_REF_TO_TEXTURE);
	att3->m_texture->set_parameter_i(nstexture::compare_func, GL_LEQUAL);
	att3->m_texture->set_parameter_i(nstexture::wrap_s, GL_CLAMP_TO_EDGE);
	att3->m_texture->set_parameter_i(nstexture::wrap_t, GL_CLAMP_TO_EDGE);
	att3->m_texture->set_parameter_i(nstexture::wrap_r, GL_CLAMP_TO_EDGE);
}

void nsshadowbuf_object::resize(uint32 w, uint32 h)
{
	m_spot_fb->resize(w,h);
	m_point_fb->resize(w, h);
	m_dir_fb->resize(w, h);
}

void nsshadowbuf_object::resize(const uivec2 & size_)
{
	m_spot_fb->resize(size_);
	m_point_fb->resize(size_);
	m_dir_fb->resize(size_);
}

void nsshadowbuf_object::resize(const map_t & pMap, uint32 w, uint32 h)
{
	fb(pMap)->resize(w, h);
}

void nsshadowbuf_object::resize(const map_t & pMap, const uivec2 & size_)
{
	fb(pMap)->resize(size_);
}
