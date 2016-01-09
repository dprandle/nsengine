/*! 
	\file nsmaterial.h
	
	\brief Header file for nsmaterial class

	This file contains all of the neccessary declarations for the nsmaterial class.

	\author Daniel Randle
	\date November 23 2013
	\copywrite Earth Banana Games 2013
*/

#ifndef NSMATERIAL_H
#define NSMATERIAL_H

// Inlcudes
#include <nsresource.h>
#include <nsunordered_map.h>

class nsmaterial : public nsresource
{
public:
	enum map_type {
		diffuse,
		normal,
		height,
		spec,
		displacement,
		ambient,
		emmisive,
		shininess,
		opacity,
		light,
		reflection
	};

	typedef std::unordered_map<map_type, uivec2, EnumHash> texmap_map;
	typedef texmap_map::iterator texmap_map_iter;
	typedef texmap_map::const_iterator texmap_map_const_iter;

	template<class PUPer>
	friend void pup(PUPer & p, nsmaterial & mat);

	struct specular_comp
	{
		specular_comp(float power_ = 0.0f, float intensity_ = 0.0f, fvec3 color_ = fvec3()) :power(power_), intensity(intensity_), color(color_)
		{};
		float power;
		float intensity;
		fvec3 color;
	};

	nsmaterial();

	nsmaterial(const nsmaterial & copy_);

	~nsmaterial();

	nsmaterial & operator=(nsmaterial rhs);

	texmap_map_iter begin();

	texmap_map_const_iter begin() const;

	void change_specular(float pow_amount, float intensity_amount);

	void change_specular_intensity(float amount);

	void change_specular_power(float amount);

	void clear();

	bool contains(const map_type & pMType);

	void enable_culling(bool pEnable);

	void enable_wireframe(bool pEnable);

	texmap_map_iter end();

	texmap_map_const_iter end() const;

	const fvec4 & color();

	bool color_mode();

	const uint32 & cull_mode() const;

	uivec2 map_tex_id(map_type pMapType);

	const uivec2 & shader_id();

	const texmap_map & tex_maps() const;

	virtual void pup(nsfile_pupper * p);

	const specular_comp & specular() const;

	const fvec3 & specular_color() const;

	float specular_power() const;

	float specular_intensity() const;

	/*!
	Get the other resources that this Material uses. This includes all texture maps.
	\return Map of resource ID to resource type containing all other used texture map IDs and a shader ID
	*/
	virtual uivec3_vector resources();

	void init();

	bool alpha_blend();

	bool culling() const;

	bool wireframe() const;

	/*!
	This should be called if there was a name change to a resource - will check if the resource is used by this component and if is
	is then it will update the handle
	*/
	virtual void name_change(const uivec2 & oldid, const uivec2 newid);

	bool remove_tex_map(map_type pMapType);

	void set_alpha_blend(bool pBlend);

	void use_alpha_from_color(bool enable);

	bool using_alpha_from_color() const;

	void set_color(const fvec4 & color_);

	void set_color_mode(bool pEnable);

	void set_cull_mode(uint32 pMode);

	bool set_map_tex_id(map_type pMapType, const uivec2 & resID, bool pOverwrite = false);

	bool set_map_tex_id(map_type pMapType, uint32 plugID, uint32 resID, bool pOverwrite = false)
	{
		return set_map_tex_id(pMapType, uivec2(plugID,resID), pOverwrite);
	}

	void set_shader_id(const uivec2 & resID);

	void set_shader_id(uint32 plugID, uint32 resID)
	{
		m_shader_id.set(plugID, resID);
	}

	void set_specular(const specular_comp & pSpecComp);

	void set_specular(float power_, float intensity_, const fvec3 & color_);

	void set_specular_color(const fvec3 & color_);

	void set_specular_power(float power_);

	void set_specular_intensity(float intensity_);

private:
	bool m_alpha_blend;
	uivec2 m_shader_id;
	fvec4 m_color;
	bool m_color_mode;
	bool m_culling_enabled;
	uint32 m_cull_mode;
	specular_comp m_spec_comp;
	texmap_map m_tex_maps;
	bool m_wireframe;
	bool m_force_alpha;
};


template<class PUPer>
void pup(PUPer & p, nsmaterial & mat)
{
	pup(p, mat.m_alpha_blend, "alpha_blend");
	pup(p, mat.m_shader_id, "shader_id");
	pup(p, mat.m_color, "color");
	pup(p, mat.m_color_mode, "color_mode");
	pup(p, mat.m_culling_enabled, "culling_enabled");
	pup(p, mat.m_cull_mode, "cull_mode");
	pup(p, mat.m_spec_comp, "spec_comp");
	pup(p, mat.m_tex_maps, "texture_maps");
	pup(p, mat.m_wireframe, "wireframe");
	pup(p, mat.m_force_alpha, "force_alpha");	
}

template<class PUPer>
void pup(PUPer & p, nsmaterial::specular_comp & mat, const nsstring & val_name)
{
	pup(p, mat.power, val_name + ".power");
	pup(p, mat.intensity, val_name + ".intensity");
	pup(p, mat.color, val_name + ".color");
}

template<class PUPer>
void pup(PUPer & p, nsmaterial::map_type & my, const nsstring & val_name)
{
	uint32 mt = static_cast<uint32>(my);
	pup(p, mt, val_name);
	my = static_cast<nsmaterial::map_type>(mt);
}

#endif
