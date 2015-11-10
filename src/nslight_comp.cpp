

/*!
\file nslightcomp.cpp

\brief Definition file for nslight_comp class

This file contains all of the neccessary definitions for the nslight_comp class.

\author Daniel Randle
\date January 27 2013
\copywrite Earth Banana Games 2013
*/

#include <nslight_comp.h>
#include <nsmesh.h>
#include <nstimer.h>
#include <nsentity.h>
#include <nstform_comp.h>
#include <nsengine.h>
#include <nsmesh_manager.h>
#include <nscam_comp.h>

nslight_comp::nslight_comp() : 
m_light_type(l_point),
m_att_comp(),
m_intensity_comp(),
m_shadow_darkness(1.0f),
m_angle(),
m_color(1.0f, 1.0f, 1.0f),
m_cast_shadows(true),
m_shadow_samples(0),
m_bounding_mesh_id(0),
m_scaling(1.0f, 1.0f, 1.0f),
m_shadow_clip(DEFAULT_Z_NEAR, DEFAULT_Z_FAR),
nscomponent()
{}

nslight_comp::~nslight_comp()
{}

nslight_comp* nslight_comp::copy(const nscomponent * copy_)
{
	if (copy_ == NULL)
		return NULL;
	const nslight_comp * comp = (const nslight_comp*)copy_;
	(*this) = (*comp);
	return this;
}

void nslight_comp::init()
{}

bool nslight_comp::cast_shadows() const
{
	return m_cast_shadows;
}

void nslight_comp::change_attenuation(float const_, float lin_, float exp_)
{
	set_attenuation(m_att_comp + fvec3(const_,lin_,exp_));
}

void nslight_comp::change_color(float red_, float green_, float blue_)
{
	m_color += fvec3(red_, green_, blue_);
	post_update(true);
}

void nslight_comp::change_angle(float amount_)
{
	set_angle(m_angle + amount_);
}

void nslight_comp::change_cutoff(float amount_)
{
	float cut = cutoff();
	cut += amount_;
	cut = clampf(cut, -1.0f, 1.0f);
	set_cutoff(cut);
}

void nslight_comp::change_distance(float amount_, adjustment_t adj_)
{
	set_distance(m_scaling.z + amount_, adj_);
}

void nslight_comp::change_intensity(float diff_, float amb_, adjustment_t adj_)
{
	set_intensity(m_intensity_comp + fvec2(diff_, amb_), adj_);
}

void nslight_comp::change_radius(float amount_)
{
	set_radius(m_scaling.x + amount_);
}

void nslight_comp::change_shadow_darkness(float amount_)
{
	if (amount_+m_shadow_darkness < SHADOW_LOWER_LIMIT)
		m_shadow_darkness = 0.0f;
	else if (amount_+m_shadow_darkness > SHADOW_UPPER_LIMIT)
		m_shadow_darkness = 1.0f;
	else
		m_shadow_darkness += amount_;
}

const fvec3 & nslight_comp::atten() const
{
	return m_att_comp;
}

const uivec2 & nslight_comp::mesh_id() const
{
	return m_bounding_mesh_id;
}

void nslight_comp::name_change(const uivec2 & old_id_, const uivec2 new_id_)
{
	if (m_bounding_mesh_id.x == old_id_.x)
	{
		m_bounding_mesh_id.x = new_id_.x;
		if (m_bounding_mesh_id.y == old_id_.y)
			m_bounding_mesh_id.y = new_id_.y;
	}
}

const fvec3 & nslight_comp::color() const
{
	return m_color;
}

float nslight_comp::angle() const
{
	return m_angle;
}


float nslight_comp::cutoff() const
{
	return cosf(radians(m_angle));
}

float nslight_comp::distance()
{
	return m_scaling.z;
}

nslight_comp::light_t nslight_comp::type() const
{
	return m_light_type;
}

const fvec2 & nslight_comp::intensity() const
{
	return m_intensity_comp;
}

float nslight_comp::radius() const
{
	return m_scaling.x;
}

const fvec3 & nslight_comp::scaling() const
{
	return m_scaling;
}

float nslight_comp::shadow_darkness() const
{
	return m_shadow_darkness;
}

const fmat4 & nslight_comp::pov(uint32 index_) const
{
	return m_owner->get<nstform_comp>()->pov(index_);
}

const int32 & nslight_comp::shadow_samples() const
{
	return m_shadow_samples;
}

const fmat4 & nslight_comp::transform(uint32 index_)
{
	nstform_comp * tc = m_owner->get<nstform_comp>();
	if (tc == NULL)
		return m_tmp_ret.set_identity();
	m_tmp_ret.set(rotation_mat3(tc->orientation(index_)) % m_scaling);
	m_tmp_ret.set_column(3, tc->lpos(index_).x, tc->lpos(index_).y, tc->lpos(index_).z, 1);
	return m_tmp_ret;
}

/*!
Get the resources that the component uses. The light comp uses a bounding mesh.
*/
uivec3_vector nslight_comp::resources()
{
	// Build map
	uivec3_vector ret;

	// only add if not 0
	if (m_bounding_mesh_id != 0)
		ret.push_back(uivec3(m_bounding_mesh_id, type_to_hash(nsmesh)));

	return ret;
}

void nslight_comp::set_attenuation(const fvec3 & att_)
{
	m_att_comp = att_;
	_update_mesh_length();
	_update_mesh_radius();
	post_update(true);
}

void nslight_comp::pup(nsfile_pupper * p)
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

void nslight_comp::set_attenuation(float const_, float lin_, float exp_)
{
	m_att_comp.set(const_, lin_, exp_);
	_update_mesh_length();
	_update_mesh_radius();
	post_update(true);
}

void nslight_comp::set_mesh_id(const uivec2 & mesh_id_)
{
	m_bounding_mesh_id = mesh_id_;
	post_update(true);
}

void nslight_comp::set_cast_shadows(bool cast_shadows_)
{
	m_cast_shadows = cast_shadows_;
	post_update(true);
}

void nslight_comp::set_color(const fvec3 & col_)
{
	m_color = col_;
	post_update(true);
}

void nslight_comp::set_color(float red_, float green_, float blue_)
{
	set_color(fvec3(red_, green_, blue_));
}

void nslight_comp::set_angle(float angle_)
{
	m_angle = clampf(angle_,0.0f,180.0f);
	_update_mesh_radius();
	post_update(true);
}

void nslight_comp::set_cutoff(float cutoff_)
{
	cutoff_ = clampf(cutoff_, -1.0f, 1.0f);
	set_angle(degrees(acosf(cutoff_)));
}

void nslight_comp::set_distance(float dist_, adjustment_t adj_)
{
	if (m_light_type == l_spot)
	{
		m_scaling.z = dist_;
		_update_mesh_radius();
	}
	else if (m_light_type == l_point)
		m_scaling.set(dist_, dist_, dist_);

	_update_atten_comp(adj_);
	post_update(true);
}

void nslight_comp::set_type(light_t type_)
{
	m_light_type = type_;
	post_update(true);
}

void nslight_comp::set_intensity(const fvec2 & intensity_, adjustment_t adj_)
{
	m_intensity_comp = intensity_;
	_update_atten_comp(adj_);
	post_update(true);
}

void nslight_comp::set_intensity(float diff_, float amb_, adjustment_t adj_)
{
	set_intensity(fvec2(diff_, amb_),adj_);
}

void nslight_comp::set_radius(float rad_)
{
	if (m_light_type == l_spot)
	{
		float pAngle = degrees(atan2f(rad_, m_scaling.z));
		set_angle(pAngle);
	}
	else if (m_light_type == l_point)
		set_distance(rad_);
}

void nslight_comp::set_shadow_darkness(float val_)
{
	if (val_ < SHADOW_LOWER_LIMIT)
		m_shadow_darkness = 0.0f;
	else if (val_ > SHADOW_UPPER_LIMIT)
		m_shadow_darkness = 1.0f;
	else
		m_shadow_darkness = val_;
}

void nslight_comp::set_shadow_samples(int32 samples_)
{
	m_shadow_samples = samples_;
	post_update(true);
}

nslight_comp & nslight_comp::operator=(const nslight_comp & rhs_)
{
	m_light_type = rhs_.m_light_type;
	m_att_comp = rhs_.m_att_comp;
	m_intensity_comp = rhs_.m_intensity_comp;
	m_angle = rhs_.m_angle;
	m_color = rhs_.m_color;
	m_bounding_mesh_id = rhs_.m_bounding_mesh_id;
	m_shadow_samples = rhs_.m_shadow_samples;
	m_shadow_darkness = rhs_.m_shadow_darkness;
	post_update(true);
	return (*this);
}

void nslight_comp::_update_mesh_radius()
{
	if (m_light_type == l_spot)
	{
		float scaleXY = tanf(radians(m_angle)) * m_scaling.z;
		m_scaling.x = m_scaling.y = scaleXY;
	}
}

void nslight_comp::_update_mesh_length()
{
	float c = m_att_comp.x - m_intensity_comp.x / (LIGHT_CUTOFF);
	float b = m_att_comp.y;
	float a = m_att_comp.z;
	float ans = (-b* + sqrt(b*b - 4*a*c)) / (2 * a);
	if (m_light_type == l_spot)
		m_scaling.z = ans;
	else if (m_light_type == l_point)
		m_scaling.set(ans, ans, ans);
}

void nslight_comp::_update_atten_comp(adjustment_t adj_)
{
	if (adj_ == a_linear)
		m_att_comp.y = 1.0f / (LIGHT_CUTOFF * m_scaling.z) - m_att_comp.z * m_scaling.z - m_att_comp.x / m_scaling.z;
	else
		m_att_comp.z = 1.0f / (LIGHT_CUTOFF * m_scaling.z * m_scaling.z) - m_att_comp.y / m_scaling.z - m_att_comp.x / (m_scaling.z * m_scaling.z);
}
