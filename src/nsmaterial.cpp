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
	m_alpha_blend(false),
	m_shader_id(0),
	m_color(1.0f,1.0f,1.0f,1.0f),
	m_color_mode(false),
	m_spec_comp(),
	m_tex_maps(),
	m_culling_enabled(true),
	m_cull_mode(GL_BACK),
	m_wireframe(false)
{
	set_ext(DEFAULT_MAT_EXTENSION);
}

nsmaterial::~nsmaterial()
{}

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

const GLenum & nsmaterial::cull_mode() const
{
	return m_cull_mode;
}

bool nsmaterial::color_mode()
{
	return m_color_mode;
}

void nsmaterial::pup(NSFilePUPer * p)
{
	if (p->type() == NSFilePUPer::Binary)
	{
		NSBinFilePUPer * bf = static_cast<NSBinFilePUPer *>(p);
		::pup(*bf, *this);
	}
	else
	{
		NSTextFilePUPer * tf = static_cast<NSTextFilePUPer *>(p);
		::pup(*tf, *this);
	}
}

uivec2 nsmaterial::map_tex_id(map_type pMapType)
{
	texmap_map::iterator iter = m_tex_maps.find(pMapType);
	if (iter != m_tex_maps.end())
		return iter->second;
	return uivec2();
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
uivec2array nsmaterial::resources()
{
	uivec2array ret;

	// add all texture maps that are available
	auto iter = m_tex_maps.begin();
	while (iter != m_tex_maps.end())
	{
		if (iter->second != 0) // only add if not equal to 0
			ret.push_back(iter->second);
		++iter;
	}

	// Add the shader and all resources shader may use
	if (m_shader_id != 0)
		ret.push_back(m_shader_id);

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
		if (iter->second.x == oldid.x)
		{
			iter->second.x = newid.x;
			if (iter->second.y == oldid.y)
				iter->second.y = newid.y;
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

bool nsmaterial::remove_tex_map(map_type pMapType)
{
	return m_tex_maps.erase(pMapType) && 1;
}

void nsmaterial::set_alpha_blend(bool pBlend)
{
	m_alpha_blend = pBlend;
}

void nsmaterial::set_cull_mode(GLenum pMode)
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

bool nsmaterial::set_map_tex_id(map_type pMapType, const uivec2 & pID, bool pOverwrite)
{
	texmap_map::iterator iter = m_tex_maps.find(pMapType);
	if (iter != m_tex_maps.end())
	{
		if (pOverwrite)
		{
			m_tex_maps.erase(iter);
			m_tex_maps.emplace(pMapType, pID);
			return true;
		}
		else
			return false;
	}

	m_tex_maps.emplace(pMapType, pID);
	return true;
}
