/*------------------------------------------- Noble Steed Engine-------------------------------------*
Authors : Daniel Randle, Alex Bourne
Date Created : Mar 10 2013

File:
	nsmaterial.cpp

Description:
	This file contains the definition for the nsmaterial class along with any helper functions
	that the class may use
*--------------------------------------------------------------------------------------------------*/

#include <nsmaterial.h>
#include <nstexture.h>
#include <nslog_file.h>
#include <nsshader.h>
#include <nsengine.h>
#include <nsshader_manager.h>
#include <nstex_manager.h>

nsmaterial::nsmaterial():
	nsresource(),
	m_alpha_blend(false),
	m_shader_id(0),
	m_color(1.0f,1.0f,1.0f,1.0f),
	m_color_mode(false),
	m_spec_comp(),
	m_tex_maps(),
	m_culling_enabled(true),
	m_cull_mode(GL_BACK),
	m_wireframe(false),
	m_force_alpha(false),
	m_depth_write(true),
	m_depth_test(true),
	m_stencil_test(false),
	m_blend_func(),
	m_stencil_func(),
	m_stencil_op_back(),
	m_stencil_op_front()
{
	set_ext(DEFAULT_MAT_EXTENSION);
}

nsmaterial::nsmaterial(const nsmaterial & copy_):
	nsresource(copy_),
	m_alpha_blend(copy_.m_alpha_blend),
	m_shader_id(copy_.m_shader_id),
	m_color(copy_.m_color),
	m_color_mode(copy_.m_color_mode),
	m_culling_enabled(copy_.m_culling_enabled),
	m_cull_mode(copy_.m_cull_mode),
	m_spec_comp(copy_.m_spec_comp),
	m_tex_maps(copy_.m_tex_maps),
	m_wireframe(copy_.m_wireframe),
	m_force_alpha(copy_.m_force_alpha),
	m_depth_write(copy_.m_depth_write),
	m_depth_test(copy_.m_depth_test),
	m_stencil_test(copy_.m_stencil_test),
	m_blend_func(copy_.m_blend_func),
	m_stencil_func(copy_.m_stencil_func),
	m_stencil_op_back(copy_.m_stencil_op_back),
	m_stencil_op_front(copy_.m_stencil_op_front)
{}

nsmaterial::~nsmaterial()
{}

nsmaterial & nsmaterial::operator=(nsmaterial rhs)
{
	nsresource::operator=(rhs);
	std::swap(m_alpha_blend, rhs.m_alpha_blend);
	std::swap(m_shader_id, rhs.m_shader_id);
	std::swap(m_color, rhs.m_color);
	std::swap(m_color_mode, rhs.m_color_mode);
	std::swap(m_culling_enabled, rhs.m_culling_enabled);
	std::swap(m_cull_mode, rhs.m_cull_mode);
	std::swap(m_spec_comp, rhs.m_spec_comp);
	std::swap(m_tex_maps, rhs.m_tex_maps);
	std::swap(m_wireframe, rhs.m_wireframe);
	std::swap(m_force_alpha, rhs.m_force_alpha);
	std::swap(m_depth_write,rhs.m_depth_write);
	std::swap(m_depth_test,rhs.m_depth_test);
	std::swap(m_stencil_test,rhs.m_stencil_test);
	std::swap(m_blend_func,rhs.m_blend_func);
	std::swap(m_stencil_func,rhs.m_stencil_func);
	std::swap(m_stencil_op_back,rhs.m_stencil_op_back);
	std::swap(m_stencil_op_front,rhs.m_stencil_op_front);
	return *this;
}

nsmaterial::texmap_map_iter nsmaterial::begin()
{
	return m_tex_maps.begin();
}

nsmaterial::texmap_map_const_iter nsmaterial::begin() const
{
	return m_tex_maps.begin();
}

void nsmaterial::change_specular(float pPowAmount, float pIntensityAmount)
{
	change_specular_power(pPowAmount);
	change_specular_intensity(pIntensityAmount);
}

void nsmaterial::change_specular_intensity(float pAmount)
{
	m_spec_comp.intensity += pAmount;
}

void nsmaterial::change_specular_power(float pAmount)
{
	m_spec_comp.power += pAmount;
}

void nsmaterial::clear()
{
	m_tex_maps.clear();
}

bool nsmaterial::contains(const map_type & pMType)
{
	auto iter = m_tex_maps.find(pMType);
	return (iter != m_tex_maps.end());
}

void nsmaterial::enable_culling(bool pEnable)
{
	m_culling_enabled = pEnable;
}

void nsmaterial::enable_wireframe(bool pEnable)
{
	m_wireframe = pEnable;
}

nsmaterial::texmap_map_iter nsmaterial::end()
{
	return m_tex_maps.end();
}

nsmaterial::texmap_map_const_iter nsmaterial::end() const
{
	return m_tex_maps.end();
}

const fvec4 & nsmaterial::color()
{
	return m_color;
}

const uint32 & nsmaterial::cull_mode() const
{
	return m_cull_mode;
}

bool nsmaterial::color_mode()
{
	return m_color_mode;
}

void nsmaterial::pup(nsfile_pupper * p)
{
	if (p->type() == nsfile_pupper::pup_binary)
	{
		nsbinary_file_pupper * bf = static_cast<nsbinary_file_pupper *>(p);
		::pup(*bf, *this);
	}
	else
	{
		nstext_file_pupper * tf = static_cast<nstext_file_pupper *>(p);
		::pup(*tf, *this);
	}
}

tex_map_info nsmaterial::mat_tex_info(map_type mt)
{
	texmap_map::iterator iter = m_tex_maps.find(mt);
	if (iter != m_tex_maps.end())
		return iter->second;
	return tex_map_info();
}

uivec2 nsmaterial::map_tex_id(map_type mt)
{
	texmap_map::iterator iter = m_tex_maps.find(mt);
	if (iter != m_tex_maps.end())
		return iter->second.tex_id;
	return uivec2();
}

fvec4 nsmaterial::map_tex_coord_rect(map_type mt)
{
	texmap_map::iterator iter = m_tex_maps.find(mt);
	if (iter != m_tex_maps.end())
		return iter->second.coord_rect;
	return fvec4();
}

const uivec2 & nsmaterial::shader_id()
{
	return m_shader_id;
}

const nsmaterial::specular_comp & nsmaterial::specular() const
{
	return m_spec_comp;
}

const fvec3 & nsmaterial::specular_color() const
{
	return m_spec_comp.color;
}

float nsmaterial::specular_power() const
{
	return m_spec_comp.power;
}

float nsmaterial::specular_intensity() const
{
	return m_spec_comp.intensity;
}

const nsmaterial::texmap_map & nsmaterial::tex_maps() const
{
	return m_tex_maps;
}

/*!
Get the other resources that this Material uses. This includes all texture maps.
*/
uivec3_vector nsmaterial::resources()
{
	uivec3_vector ret;

	// add all texture maps that are available
	auto iter = m_tex_maps.begin();
	while (iter != m_tex_maps.end())
	{
		nstexture * _tex_ = get_resource<nstexture>(iter->second.tex_id);
		if (_tex_ != NULL)
		{
			uivec3_vector tmp = _tex_->resources();
			ret.insert(ret.end(), tmp.begin(), tmp.end());
			ret.push_back(uivec3(_tex_->full_id(), type_to_hash(nstexture)));
		}
		++iter;
	}

	nsshader * _shdr_ = get_resource<nsshader>(m_shader_id);
	if (_shdr_ != NULL)
	{
		uivec3_vector tmp = _shdr_->resources();
		ret.insert(ret.end(), tmp.begin(), tmp.end());
		ret.push_back(uivec3(_shdr_->full_id(), type_to_hash(nsshader)));
	}
		
	return ret;
}

void nsmaterial::init()
{
	// do nothing
}

bool nsmaterial::alpha_blend()
{
	return m_alpha_blend;
}

bool nsmaterial::culling() const
{
	return m_culling_enabled;
}

bool nsmaterial::wireframe() const
{
	return m_wireframe;
}

/*!
This should be called if there was a name change to a resource - will check if the resource is used by this component and if is
is then it will update the handle
*/
void nsmaterial::name_change(const uivec2 & oldid, const uivec2 newid)
{
	texmap_map::iterator iter = m_tex_maps.begin();
	while (iter != m_tex_maps.end())
	{
		if (iter->second.tex_id.x == oldid.x)
		{
			iter->second.tex_id.x = newid.x;
			if (iter->second.tex_id.y == oldid.y)
				iter->second.tex_id.y = newid.y;
		}
		++iter;
	}

	if (m_shader_id.x == oldid.x)
	{
		m_shader_id.x = newid.x;
		if (m_shader_id.y == oldid.y)
			m_shader_id.y = newid.y;
	}
}

bool nsmaterial::remove_tex_map(map_type mt)
{
	return m_tex_maps.erase(mt) >= 1;
}

void nsmaterial::set_alpha_blend(bool pBlend)
{
	m_alpha_blend = pBlend;
}

void nsmaterial::set_cull_mode(uint32 pMode)
{
	m_cull_mode = pMode;
}

void nsmaterial::set_color(const fvec4 & pColor)
{
	m_color = pColor;
}

void nsmaterial::set_color_mode(bool pColorMode)
{
	m_color_mode = pColorMode;
}

void nsmaterial::set_shader_id(const uivec2 & resID)
{
	m_shader_id = resID;
}

void nsmaterial::set_specular(const specular_comp & pSpecComp)
{
	m_spec_comp = pSpecComp;
}

void nsmaterial::set_specular(float pPower, float pIntensity,const fvec3 & pColor)
{
	set_specular_power(pPower);
	set_specular_intensity(pIntensity);
	set_specular_color(pColor);
}

void nsmaterial::set_specular_color(const fvec3 & pColor)
{
	m_spec_comp.color = pColor;
}

void nsmaterial::set_specular_power(float pPower)
{
	m_spec_comp.power = pPower;
}

void nsmaterial::set_specular_intensity(float pIntensity)
{
	m_spec_comp.intensity = pIntensity;
}

void nsmaterial::use_alpha_from_color(bool enable)
{
	m_force_alpha = enable;
}

bool nsmaterial::using_alpha_from_color() const
{
	return m_force_alpha;
}

bool nsmaterial::add_tex_map(map_type mt, const tex_map_info & ti, bool overwrite_existing)
{
	texmap_map::iterator iter = m_tex_maps.find(mt);
	if (iter != m_tex_maps.end())
	{
		if (overwrite_existing)
		{
			iter->second = ti;
			return true;
		}
		else
			return false;
	}
	m_tex_maps.emplace(mt, ti);
	return true;		
}

bool nsmaterial::add_tex_map(
	map_type mt,
	const uivec2 & tex_id,
	const fvec4 & coord_rect,
	bool overwrite_existing)
{
	return add_tex_map(mt, tex_map_info(tex_id,coord_rect),overwrite_existing);
}

bool nsmaterial::set_map_tex_info(map_type mt, tex_map_info ti)
{
	texmap_map::iterator iter = m_tex_maps.find(mt);
	if (iter != m_tex_maps.end())
	{
		iter->second = ti;
		return true;
	}
	return false;
}

bool nsmaterial::set_map_tex_coord_rect(map_type mt, fvec4 tex_coord_rect)
{
	texmap_map::iterator iter = m_tex_maps.find(mt);
	if (iter != m_tex_maps.end())
	{
		iter->second.coord_rect = tex_coord_rect;
		return true;
	}
	return false;	
}

bool nsmaterial::set_map_tex_id(map_type mt, const uivec2 & pID)
{
	texmap_map::iterator iter = m_tex_maps.find(mt);
	if (iter != m_tex_maps.end())
	{
		iter->second.tex_id = pID;
		return true;
	}
	return false;
}
