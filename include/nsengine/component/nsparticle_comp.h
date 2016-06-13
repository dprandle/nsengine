/*!
\file nsparticle_comp.h

\brief Header file for nsparticle_comp class

This file contains all of the neccessary declarations for the nsparticle_comp class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#ifndef NSPARTICLECOMP_H
#define NSPARTICLECOMP_H

#define DEFAULT_MAX_FORCE_KEYS 50
#define DEFAULT_MAX_RENDER_KEYS 50
#define DEFAULT_MAX_PARTICLES 1000
#define DEFAULT_PART_LIFETIME 5000
#define DEFAULT_PART_EMISSION 180

#include <nscomponent.h>
#include <nsmap.h>
#include <nsvideo_driver.h>

class nsentity;
class nsgl_xfb;
class nsgl_vao;
class nsgl_buffer;
class nsevent;

struct particle
{
	fvec4 pos;
	fvec4 vel;
	fvec4 scale_and_angle;
	fvec4 age_type_reserved;
};

class nsparticle_comp : public nscomponent, public nsvideo_object
{
public:

	enum emitter_shape_t
	{
		shape_cube,
		shape_ellipse
	};

	enum motion_key_t
	{
		key_vel,
		key_accel
	};

	enum blend_m
	{
		b_mix,
		b_add
	};

	typedef std::vector<particle> particle_array;

	template <class PUPer>
	friend void pup(PUPer & p, nsparticle_comp & pc);

	friend class nsparticle_system;
	
	nsparticle_comp();

	nsparticle_comp(const nsparticle_comp & );
	
	virtual ~nsparticle_comp();

	void allocate_buffers();

	virtual void init();

	uint32 motion_key_count();

	uint32 visual_key_count();

	float & elapsed();

	bool looping();

	bool simulating();

	void video_context_init();

	bool motion_key_interpolation();

	bool visual_key_interpolation();

	bool visual_global_time();

	ui_fvec3_map::iterator begin_motion_key();
	ui_fvec3_map::iterator begin_visual_key();

	ui_fvec3_map::iterator end_motion_key();
	ui_fvec3_map::iterator end_visual_key();

	void enable_looping(bool enable_);

	void enable_simulation(bool enable_);

	void enable_motion_global_time(bool enable_);

	void enable_motion_key_interp(bool enable_);

	void enable_visual_global_time(bool enable_);

	void enable_visual_key_interp(bool enable_);

	void clear_motion_keys();

	void clear_visual_keys();

	virtual void name_change(const uivec2 & old_id_, const uivec2 new_id_);

	virtual uivec3_vector resources();

	void set_motion_key_type(const motion_key_t & type_);

	const motion_key_t & motion_key_type();

	void set_emitter_shape(const emitter_shape_t & shape_);

	const emitter_shape_t & emitter_shape();

	nsparticle_comp & operator=(nsparticle_comp rhs_);

	const uivec2 & shader_id();

	const uivec2 & material_id();

	uint32 max_particles();

	const ui_fvec3_map & motion_keys();

	const ui_fvec3_map & visual_keys();

	const fvec3 & emitter_size();

	void set_emitter_size(const fvec3 & size_);

	uint32 max_motion_keys();

	uint32 max_visual_keys();

	bool motion_global_time();

	fvec3 motion_key_at(float time_);

	fvec3 visual_key_at(float time_);

	bool has_motion_key(float time_);

	bool has_visual_key(float time_);

	void remove_motion_key(float time_);

	void remove_visual_key(float time_);

	void set_max_motion_keys(uint32 max_);

	void set_max_visual_keys(uint32 max_);

	void set_motion_key(float time_, const fvec3 & force_);

	void set_visual_key(float time_, const fvec3 & render_key_);

	uint32 lifetime();

	uint32 emission_rate();

	void release();

	bool first();

	void set_first(bool set_);

	virtual void pup(nsfile_pupper * p);

	void reset();

	const fvec3 & init_vel_mult();

	void set_init_vel_mult(const fvec3 & mult_);

	const uivec2 & rand_tex_id();

	void set_emission_rate(uint32 rate_);

	void set_lifetime(uint32 lifetime_);

	void set_max_particles(uint32 max_particles_);

	void set_shader_id(uint32 plug_id_, uint32 res_id_)
	{
		m_xfb_shader_id.x = plug_id_; m_xfb_shader_id.y = res_id_;
		post_update(true);
	}

	void set_shader_id(const uivec2 &);

	void set_material_id(uint32 plug_id_, uint32 res_id_)
	{
		m_mat_id.x = plug_id_; m_mat_id.y = res_id_;
		post_update(true);
	}

	void set_material_id(const uivec2 & res_id_);

	void set_rand_tex_id(const uivec2 & res_id_);

	void set_rand_tex_id(uint32 plug_id_, uint32 res_id_)
	{
		m_rand_tex_id.x = plug_id_; m_rand_tex_id.y = res_id_;
		post_update(true);
	}

	void set_angular_vel(int32 vel_);

	void set_starting_size(const fvec2 & size_);

	void set_starting_size(float width_, float height_);

	blend_m blend_mode();

	void set_blend_mode(const blend_m & mode_);

	fvec2 starting_size();

	int32 angular_vel();

private:
	ui_fvec3_map m_motion_keys;
	ui_fvec3_map m_visual_keys;

	uivec2 m_mat_id; //!< Material ID for the rendered quad
	uivec2 m_xfb_shader_id; //!< Shader used for transform feedback
	uint32 m_max_particle_count; //!< Maximum particles allowed
	uint32 m_lifetime; //!< How long should these particles stay alive
	uint32 m_emission_rate; //!< Particles emitted per second
	uivec2 m_rand_tex_id; // Random texture ID
	emitter_shape_t m_emitter_shape; // Cube or Ellipse
	motion_key_t m_motion_key_type; // Velocity or Acceleration motion keyframes
	blend_m m_blend_mode; // Additive or Source minus destination bla bla
	int32 m_ang_vel; // self explanitory
	bool m_looping; // wrap elapsed time back to 0 at end of lifetime and restart
	bool m_motion_key_interp; // Interpolate between motion key frames?
	bool m_motion_global_time; // Should the particles all be affected by motion keyframes in global elapsed time or per particle lifetime
	bool m_visual_key_interp; // Interpolate between keys?
	bool m_visual_global_time; // Should the particles all be affected by visual keyframes in global elapsed time or per particle lifetime
	float m_elapsed_time; // Amount of time elapsed since sim started
	fvec2 m_starting_size; // Startung size of screen facing quads in world space sizes (not screen space)
	fvec3 m_emitter_size; // x,y,z size of the emitter space where particles generate
	fvec3 m_init_vel_mult; // Initial velocity x,y,z multiplier - multiplies the random number by this
	uint32 m_max_motion_keys; // Maximum number of motion key frames - should be set to match whatever is in shader
	uint32 m_max_visual_keys; // Maximum number of visual key frames - also should match whatever is set in shader

	/* This stuff does not need to be saved*/
	bool m_simulating; // Are we simulating?
	bool m_first; // Is it the first time running the simulation since being reset (need to render using glDrawElements rather than feedback draw)
};

template<class PUPer>
void pup(PUPer & p, nsparticle_comp::emitter_shape_t & en, const nsstring & val_name_)
{
	uint32 in = static_cast<uint32>(en);
	pup(p, in, val_name_);
	en = static_cast<nsparticle_comp::emitter_shape_t>(in);
}

template<class PUPer>
void pup(PUPer & p, nsparticle_comp::motion_key_t & en, const nsstring & val_name_)
{
	uint32 in = static_cast<uint32>(en);
	pup(p, in, val_name_);
	en = static_cast<nsparticle_comp::motion_key_t>(in);
}

template<class PUPer>
void pup(PUPer & p, nsparticle_comp::blend_m & en, const nsstring & val_name_)
{
	uint32 in = static_cast<uint32>(en);
	pup(p, in, val_name_);
	en = static_cast<nsparticle_comp::blend_m>(in);
}

template <class PUPer>
void pup(PUPer & p, nsparticle_comp & pc)
{
	pup(p, pc.m_motion_keys, "motion_keys");
	pup(p, pc.m_visual_keys, "visual_keys");
	pup(p, pc.m_mat_id, "mat_id");
	pup(p, pc.m_xfb_shader_id, "xfb_shader_id");
	pup(p, pc.m_max_particle_count, "max_particle_count");
	pup(p, pc.m_lifetime, "lifetime");
	pup(p, pc.m_emission_rate, "emission_rate");
	pup(p, pc.m_rand_tex_id, "rand_text_id");
	pup(p, pc.m_emitter_shape, "emitter_shape");
	pup(p, pc.m_motion_key_type, "motion_key_type");
	pup(p, pc.m_blend_mode, "blend_mode");
	pup(p, pc.m_ang_vel, "angular_velocity");
	pup(p, pc.m_looping, "looping_enabled");
	pup(p, pc.m_motion_key_interp, "motion_key_inter");
	pup(p, pc.m_motion_global_time, "motion_global_time");
	pup(p, pc.m_visual_key_interp, "visual_key_interp");
	pup(p, pc.m_visual_global_time, "visual_global_time");
	pup(p, pc.m_elapsed_time, "elapsed_time");
	pup(p, pc.m_starting_size, "starting_size");
	pup(p, pc.m_emitter_size, "emitter_size");
	pup(p, pc.m_init_vel_mult, "init_velocity_mult");
	pup(p, pc.m_max_motion_keys, "max_motion_keys");
	pup(p, pc.m_max_visual_keys, "max_visual_keys");
	pc.post_update(true);
}

#endif
