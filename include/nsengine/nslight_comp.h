/*!
\file nslight_comp.h

\brief Header file for nslight_comp class

This file contains all of the neccessary declarations for the nslight_comp class.

\author Daniel Randle
\date January 27 2013
\copywrite Earth Banana Games 2013
*/

#ifndef NSLIGHT_COMP_H
#define NSLIGHT_COMP_H

#define LIGHT_ATTENUATION_CUTOFF 0.01f
#define SHADOW_LOWER_LIMIT 0.0
#define SHADOW_UPPER_LIMIT 1.0
#define LIGHT_CUTOFF 0.1f

#include <nscomponent.h>

class nsmesh;
class nsentity;
class nstimer;

class nslight_comp : public nscomponent
{
public:
	template <class PUPer>
	friend void pup(PUPer & p, nslight_comp & lc);

	enum light_t {
		l_dir,
		l_point,
		l_spot
	};

	enum adjustment_t {
		a_linear,
		a_exp
	};

	nslight_comp();

	nslight_comp(const nslight_comp & copy);

	~nslight_comp();

	void init();

	bool cast_shadows() const;

	void change_attenuation(float const_, float lin_, float exp_);

	void change_color(float red_, float green_, float blue_);

	void change_angle(float amount_);

	void change_cutoff(float amount_);

	void change_distance(float amount_, adjustment_t adj_ = a_exp);

	void change_intensity(float diff_, float amb_, adjustment_t adj_=a_exp);

	void change_shadow_darkness(float amount_);

	void change_radius(float amount_);

	const fvec3 & atten() const;

	const uivec2 & mesh_id() const;

	const fvec3 & color() const;

	float angle() const;

	float cutoff() const;

	float distance();

	light_t type() const;

	const fvec2 & intensity() const;

	const fmat4 & pov(uint32 index_) const;

	float radius() const;

	const fvec3 & scaling() const;

	virtual void pup(nsfile_pupper * p);

	float shadow_darkness() const;

	const int32 & shadow_samples() const;

	const fvec2 & shadow_clipping() const { return m_shadow_clip; }

	const fmat4 & transform(uint32 index_);

	uint32 transform_count();

	virtual void name_change(const uivec2 & old_id_, const uivec2 new_id_);

	/*!
	Get the resources that the component uses. The light comp uses a bounding mesh.
	/return Map of resource ID to resource type containing bounding mesh ID.
	*/
	virtual uivec3_vector resources();

	void set_attenuation(const fvec3 & atten_);

	void set_attenuation(float const_, float lin_, float exp_);

	void set_mesh_id(uint32 plug_id_, uint32 res_id_)
	{
		m_bounding_mesh_id.x = plug_id_; m_bounding_mesh_id.y = res_id_;
		post_update(true);
	}

	void set_shadow_clipping(const fvec2 & shadow_clip_) { m_shadow_clip = shadow_clip_; post_update(true); }

	void set_mesh_id(const uivec2 & res_id_);

	void set_cast_shadows(bool cast_shadows_);

	void set_color(const fvec3 & col_);

	void set_color(float red_, float green_, float blue_);

	void set_cutoff(float cutoff_);

	void set_angle(float angle_);

	void set_distance(float dist_, adjustment_t adj_ = a_exp);

	void set_type(light_t type_);

	void set_intensity(const fvec2 & intensity_, adjustment_t adj_ = a_exp);

	void set_intensity(float diff_, float amb_, adjustment_t adj_ = a_exp);

	void set_radius(float rad_);

	void set_shadow_darkness(float val_);

	void set_shadow_samples(int32 samples_);

	nslight_comp & operator=(nslight_comp rhs_);

private:

	void _update_mesh_radius();

	void _update_mesh_length();

	void _update_atten_comp(adjustment_t adj_);

	fmat4 m_tmp_ret;
	light_t m_light_type;
	fvec3 m_att_comp;
	fvec2 m_intensity_comp;
	float m_shadow_darkness;
	float m_angle;
	fvec3 m_color;
	bool m_cast_shadows;
	int32 m_shadow_samples;
	uivec2 m_bounding_mesh_id;
	fvec3 m_scaling;
	fvec2 m_shadow_clip;
};

template <class PUPer>
void pup(PUPer & p, nslight_comp & lc)
{
	pup(p, lc.m_light_type, "light_type");
	pup(p, lc.m_att_comp, "att_comp");
	pup(p, lc.m_intensity_comp, "intensity_comp");
	pup(p, lc.m_shadow_darkness, "shadow_darkness");
	pup(p, lc.m_angle, "angle");
	pup(p, lc.m_color, "color");
	pup(p, lc.m_cast_shadows, "cast_shadows");
	pup(p, lc.m_shadow_samples, "shadow_samples");
	pup(p, lc.m_bounding_mesh_id, "bounding_mesh_id");
	pup(p, lc.m_scaling, "scaling");
	lc.post_update(true);
}

template<class PUPer>
void pup(PUPer & p, nslight_comp::light_t & en, const nsstring & val_name_)
{
	uint32 in = static_cast<uint32>(en);
	pup(p, in, val_name_);
	en = static_cast<nslight_comp::light_t>(in);
}

#endif // !NSLIGHT_COMP_H
