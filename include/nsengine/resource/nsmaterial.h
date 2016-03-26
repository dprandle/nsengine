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

struct tex_map_info
{
	tex_map_info(const uivec2 & tex_id_=uivec2(), const fvec4 coord_rect_=fvec4(0.0f,0.0f,1.0f,1.0f)):
		tex_id(tex_id_),
		coord_rect(coord_rect_)
	{}
	uivec2 tex_id;
	fvec4 coord_rect;
};

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

	typedef std::unordered_map<map_type, tex_map_info, EnumHash> texmap_map;
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

	tex_map_info mat_tex_info(map_type mt);
	
	uivec2 map_tex_id(map_type pMapType);

	fvec4 map_tex_coord_rect(map_type mt);

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

	bool add_tex_map(map_type mt, const tex_map_info & ti, bool overwrite_existing);

	bool add_tex_map(map_type mt, const uivec2 & tex_id, const fvec4 & coord_rect, bool overwrite_existing);

	bool set_map_tex_info(map_type pMapType, tex_map_info ti);

	bool set_map_tex_coord_rect(map_type pMapType, fvec4 tex_coord_rect);

	bool set_map_tex_id(map_type pMapType, const uivec2 & resID);

	void set_shader_id(const uivec2 & resID);

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
	bool m_force_alpha; // use the diffuse texture (if there) but use alpha from color
	bool m_depth_write;
	bool m_depth_test;
	bool m_stencil_test;
	ivec2 m_blend_func;
	ivec3 m_stencil_func;
	ivec3 m_stencil_op_front;
	ivec3 m_stencil_op_back;
};

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

template<class PUPer>
void pup(PUPer & p, tex_map_info & ti, const nsstring & val_name)
{
	pup(p, ti.tex_id, val_name + ".tex_id");
	pup(p, ti.coord_rect, val_name + ".coord_rect");
}

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
	pup(p, mat.m_depth_write,"depth_write");
	pup(p, mat.m_depth_test,"depth_test");
	pup(p, mat.m_stencil_test,"stencil_test");
	pup(p, mat.m_blend_func,"blend_func");
	pup(p, mat.m_stencil_func,"stencil_func");
	pup(p, mat.m_stencil_op_front,"stencil_op_front");
	pup(p, mat.m_stencil_op_back,"stencil_op_back");
}
#endif
