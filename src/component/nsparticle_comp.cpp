/*!
\file nsparticlecomp.cpp

\brief Definition file for nsparticle_comp class

This file contains all of the neccessary definitions for the nsparticle_comp class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/


#include <component/nsparticle_comp.h>
#include <opengl/nsgl_xfb.h>
#include <opengl/nsgl_vao.h>
#include <opengl/nsgl_buffer.h>
#include <asset/nstex_manager.h>
#include <asset/nsshader_manager.h>
#include <nsentity.h>
#include <asset/nsmat_manager.h>
#include <nsengine.h>

nsparticle_comp::nsparticle_comp() :
	nscomponent(type_to_hash(nsparticle_comp)),
	nsvideo_object("nsparticle_comp"),
	m_simulating(false),
	m_first(true),
	m_mat_id(0),
	m_xfb_shader_id(0),
	m_rand_tex_id(0),
	m_max_particle_count(DEFAULT_MAX_PARTICLES),
	m_lifetime(DEFAULT_PART_LIFETIME),
	m_emission_rate(DEFAULT_PART_EMISSION),
	m_max_motion_keys(DEFAULT_MAX_FORCE_KEYS),
	m_max_visual_keys(DEFAULT_MAX_RENDER_KEYS),
	m_looping(false),
	m_motion_key_type(key_vel),
	m_motion_key_interp(true),
	m_motion_global_time(false),
	m_visual_global_time(false),
	m_visual_key_interp(true),
	m_blend_mode(b_mix),
	m_init_vel_mult(1.0f, 1.0f, 1.0f),
	m_ang_vel(0),
	m_elapsed_time(0.0f),
	m_starting_size(1.0f, 1.0f),
	m_emitter_shape(shape_cube),
	m_emitter_size(1.0f, 1.0f, 1.0f)
{
	m_motion_keys[0] = fvec3();
	m_motion_keys[m_max_motion_keys] = fvec3();
	m_visual_keys[0] = fvec3(1.0f,1.0f,1.0f);
	m_visual_keys[m_max_visual_keys] = fvec3(1.0f, 1.0f, 1.0f);
	video_context_init();
}

nsparticle_comp::nsparticle_comp(const nsparticle_comp & copy):
	nscomponent(copy),
	nsvideo_object("nsparticle_comp"),
	m_motion_keys(copy.m_motion_keys),
	m_visual_keys(copy.m_visual_keys),
	m_mat_id(copy.m_mat_id),
	m_xfb_shader_id(copy.m_xfb_shader_id),
	m_max_particle_count(copy.m_max_particle_count),
	m_lifetime(copy.m_lifetime),
	m_emission_rate(copy.m_emission_rate),
	m_rand_tex_id(copy.m_rand_tex_id),
	m_emitter_shape(copy.m_emitter_shape),
	m_motion_key_type(copy.m_motion_key_type),
	m_blend_mode(copy.m_blend_mode),
	m_ang_vel(copy.m_ang_vel),
	m_looping(copy.m_looping),
	m_motion_key_interp(copy.m_motion_key_interp),
	m_motion_global_time(copy.m_motion_global_time),
	m_visual_key_interp(copy.m_visual_key_interp),
	m_visual_global_time(copy.m_visual_global_time),
	m_elapsed_time(copy.m_elapsed_time),
	m_starting_size(copy.m_starting_size),
	m_emitter_size(copy.m_emitter_size),
	m_init_vel_mult(copy.m_init_vel_mult),
	m_max_motion_keys(copy.m_max_motion_keys),
	m_max_visual_keys(copy.m_max_visual_keys),
	m_simulating(copy.m_simulating),
	m_first(copy.m_first)
{
	video_context_init();
}

nsparticle_comp::~nsparticle_comp()
{}

void nsparticle_comp::name_change(const uivec2 & oldid, const uivec2 newid)
{
	if (m_rand_tex_id.x == oldid.x)
	{
		m_rand_tex_id.x = newid.x;
		if (m_rand_tex_id.y == oldid.y)
			m_rand_tex_id.y = newid.y;
	}

	if (m_mat_id.x == oldid.x)
	{
		m_mat_id.x = newid.x;
		if (m_mat_id.y == oldid.y)
			m_mat_id.y = newid.y;
	}

	if (m_xfb_shader_id.x == oldid.x)
	{
		m_xfb_shader_id.x = newid.x;
		if (m_xfb_shader_id.y == oldid.y)
			m_xfb_shader_id.y = newid.y;
	}
}

uivec3_vector nsparticle_comp::resources()
{
	uivec3_vector ret;

	nstexture * _tex_ = get_asset<nstexture>(m_rand_tex_id);
	if (_tex_ != NULL)
	{
		uivec3_vector tmp = _tex_->resources();
		ret.insert(ret.end(), tmp.begin(), tmp.end());
		ret.push_back(uivec3(_tex_->full_id(), type_to_hash(nstexture)));
	}

	nsmaterial * _mat_ = get_asset<nsmaterial>(m_mat_id);
	if (_mat_ != NULL)
	{
		uivec3_vector tmp = _mat_->resources();
		ret.insert(ret.end(), tmp.begin(), tmp.end());
		ret.push_back(uivec3(_mat_->full_id(), type_to_hash(nsmaterial)));
	}

	nsshader * _shdr_ = get_asset<nsshader>(m_xfb_shader_id);
	if (_shdr_ != NULL)
	{
		uivec3_vector tmp = _shdr_->resources();
		ret.insert(ret.end(), tmp.begin(), tmp.end());
		ret.push_back(uivec3(_shdr_->full_id(), type_to_hash(nsshader)));
	}
	return ret;
}

void nsparticle_comp::clear_motion_keys()
{
	m_motion_keys.clear();
	m_motion_keys[0] = fvec3();
	m_motion_keys[m_max_motion_keys] = fvec3();
}

void nsparticle_comp::clear_visual_keys()
{
	m_visual_keys.clear();
	m_visual_keys[0] = fvec3(1.0f, 1.0f, 1.0f);
	m_visual_keys[m_max_visual_keys] = fvec3(1.0f, 1.0f, 1.0f);
}

void nsparticle_comp::set_angular_vel(int32 pVel)
{
	m_ang_vel = pVel;
}

fvec2 nsparticle_comp::starting_size()
{
	return m_starting_size;
}

int32 nsparticle_comp::angular_vel()
{
	return m_ang_vel;
}

void nsparticle_comp::enable_motion_global_time(bool pEnable)
{
	m_motion_global_time = pEnable;
}

bool nsparticle_comp::motion_global_time()
{
	return m_motion_global_time;
}

void nsparticle_comp::set_emitter_shape(const emitter_shape_t & pShape)
{
	m_emitter_shape = pShape;
}

void nsparticle_comp::set_motion_key_type(const motion_key_t & pType)
{
	m_motion_key_type = pType;
}

const nsparticle_comp::motion_key_t & nsparticle_comp::motion_key_type()
{
	return m_motion_key_type;
}

bool nsparticle_comp::motion_key_interpolation()
{
	return m_motion_key_interp;
}

void nsparticle_comp::enable_motion_key_interp(bool pEnable)
{
	m_motion_key_interp = pEnable;
}

const nsparticle_comp::emitter_shape_t & nsparticle_comp::emitter_shape()
{
	return m_emitter_shape;
}

const fvec3 & nsparticle_comp::emitter_size()
{
	return m_emitter_size;
}

nsparticle_comp::blend_m nsparticle_comp::blend_mode()
{
	return m_blend_mode;
}

void nsparticle_comp::set_blend_mode(const blend_m & pMode)
{
	m_blend_mode = pMode;
}

void nsparticle_comp::set_emitter_size(const fvec3 & pSize)
{
	m_emitter_size = pSize;
}

fvec3 nsparticle_comp::motion_key_at(float pTime)
{
	uint32 index = static_cast<uint32>(pTime * m_max_motion_keys);
	ui_fvec3_map::iterator fIter = m_motion_keys.find(index);
	if (fIter != m_motion_keys.end())
		return fIter->second;
	return fvec3();
}

fvec3 nsparticle_comp::visual_key_at(float pTime)
{
	uint32 index = static_cast<uint32>(pTime * m_max_visual_keys);
	ui_fvec3_map::iterator rIter = m_visual_keys.find(index);
	if (rIter != m_visual_keys.end())
		return rIter->second;
	return fvec3();
}

void nsparticle_comp::init()
{}

void nsparticle_comp::remove_motion_key(float pTime)
{
	if (pTime >= 1.0f)
	{
		return;
	}
	else if (pTime <= 0.0f)
	{
		return;
	}
	uint32 index = static_cast<uint32>(pTime * m_max_motion_keys);
	m_motion_keys.erase(index);
}

void nsparticle_comp::enable_visual_global_time(bool pEnable)
{
	m_visual_global_time = pEnable;
}

void nsparticle_comp::enable_visual_key_interp(bool pEnable)
{
	m_visual_key_interp = pEnable;
}

bool nsparticle_comp::visual_key_interpolation()
{
	return m_visual_key_interp;
}

bool nsparticle_comp::visual_global_time()
{
	return m_visual_global_time;
}

void nsparticle_comp::remove_visual_key(float pTime)
{
	if (pTime >= 1.0f)
	{
		return;
	}
	else if (pTime <= 0.0f)
	{
		return;
	}
	uint32 index = static_cast<uint32>(pTime * m_max_visual_keys);
	m_visual_keys.erase(index);
}


const uivec2 & nsparticle_comp::shader_id()
{
	return m_xfb_shader_id;
}

uint32 nsparticle_comp::max_motion_keys()
{
	return m_max_motion_keys;
}

uint32 nsparticle_comp::max_visual_keys()
{
	return m_max_visual_keys;
}

void nsparticle_comp::set_max_motion_keys(uint32 pMax)
{
	ui_fvec3_map tmpMap;
	ui_fvec3_map::iterator mapIter = m_motion_keys.begin();
	while (mapIter != m_motion_keys.end())
	{
		tmpMap[static_cast<uint32>(float(mapIter->first) / float(m_max_motion_keys) * pMax)] = mapIter->second;
		++mapIter;
	}
	std::swap(tmpMap, m_motion_keys);
	m_max_motion_keys = pMax;
}

const ui_fvec3_map & nsparticle_comp::motion_keys()
{
	return m_motion_keys;
}

const ui_fvec3_map & nsparticle_comp::visual_keys()
{
	return m_visual_keys;
}

void nsparticle_comp::set_max_visual_keys(uint32 pMax)
{
	ui_fvec3_map tmpMap;
	ui_fvec3_map::iterator mapIter = m_visual_keys.begin();
	while (mapIter != m_visual_keys.end())
	{
		tmpMap[static_cast<uint32>(float(mapIter->first) / float(m_max_visual_keys) * pMax)] = mapIter->second;
		++mapIter;
	}
	std::swap(tmpMap, m_visual_keys);
	m_max_visual_keys = pMax;
}

void nsparticle_comp::set_motion_key(float pTime, const fvec3 & pForce)
{
	if (pTime > 1.0f)
	{
		dprint("nsparticle_comp::setForceKey - Warning : setting time above 1.0 clamps the time to 1");
		pTime = 1.0f;
	}
	else if (pTime < 0.0f)
	{
		dprint("nsparticle_comp::setForceKey - Warning : setting time below 0.0 clamps the time to 0");
		pTime = 0.0f;
	}
	uint32 index = static_cast<uint32>(pTime * m_max_motion_keys);
	m_motion_keys[index] = pForce;
}

void nsparticle_comp::set_visual_key(float pTime, const fvec3 & pRenderKey)
{
	if (pTime > 1.0f)
	{
		dprint("nsparticle_comp::setRenderKey - Warning : setting time above 1.0 clamps the time to 1");
		pTime = 1.0f;
	}
	else if (pTime < 0.0f)
	{
		dprint("nsparticle_comp::setRenderKey - Warning : setting time below 0.0 clamps the time to 0");
		pTime = 0.0f;
	}
	uint32 index = static_cast<uint32>(pTime * m_max_visual_keys);
	m_visual_keys[index] = pRenderKey;
}

void nsparticle_comp::set_shader_id(const uivec2 & pID)
{
	m_xfb_shader_id = pID;
}

const uivec2 & nsparticle_comp::material_id()
{
	return m_mat_id;
}

uint32 nsparticle_comp::max_particles()
{
	return m_max_particle_count;
}

float & nsparticle_comp::elapsed()
{
	return m_elapsed_time;
}

uint32 nsparticle_comp::lifetime()
{
	return m_lifetime;
}

uint32 nsparticle_comp::emission_rate()
{
	return m_emission_rate;
}

const fvec3 & nsparticle_comp::init_vel_mult()
{
	return m_init_vel_mult;
}

void nsparticle_comp::set_init_vel_mult(const fvec3 & pMult)
{
	m_init_vel_mult = pMult;
}

uint32 nsparticle_comp::motion_key_count()
{
	return static_cast<uint32>(m_motion_keys.size());
}

uint32 nsparticle_comp::visual_key_count()
{
	return static_cast<uint32>(m_visual_keys.size());
}

ui_fvec3_map::iterator nsparticle_comp::begin_motion_key()
{
	return m_motion_keys.begin();
}

ui_fvec3_map::iterator nsparticle_comp::begin_visual_key()
{
	return m_visual_keys.begin();
}

ui_fvec3_map::iterator nsparticle_comp::end_motion_key()
{
	return m_motion_keys.end();
}

ui_fvec3_map::iterator nsparticle_comp::end_visual_key()
{
	return m_visual_keys.end();
}

void nsparticle_comp::set_emission_rate(uint32 pRate)
{
	if (pRate * m_lifetime >= 1000 * m_max_particle_count)
	{
		dprint("nsparticle_comp::setEmissionRate(uint32 pRate) - Emission rate limited by max particle count");
		return;
	}
	m_emission_rate = pRate;
}

bool nsparticle_comp::has_motion_key(float pTime)
{
	int32 index = static_cast<uint32>(pTime*m_max_motion_keys);
	return (m_motion_keys.find(index) != m_motion_keys.end());
}

bool nsparticle_comp::has_visual_key(float pTime)
{
	int32 index = static_cast<uint32>(pTime*m_max_visual_keys);
	return (m_visual_keys.find(index) != m_visual_keys.end());
}

void nsparticle_comp::set_starting_size(const fvec2 & pSize)
{
	m_starting_size = pSize;
}

void nsparticle_comp::set_starting_size(float pWidth, float pHeight)
{
	m_starting_size.u = pWidth;
	m_starting_size.v = pHeight;
}

void nsparticle_comp::set_lifetime(uint32 pLifetime)
{
	if (m_emission_rate * pLifetime >= 1000 * m_max_particle_count)
	{
		dprint("nsparticle_comp::setLoopTime(uint32 pRate) - Lifetime limited by max particle count");
		return;
	}
	m_lifetime = pLifetime;
}

void nsparticle_comp::allocate_buffers()
{}

void nsparticle_comp::set_max_particles(uint32 pMaxParticles)
{
	m_max_particle_count = pMaxParticles;
	if (m_emission_rate * m_lifetime >= 1000 * m_max_particle_count)
	{
		m_lifetime = 1000 * m_max_particle_count / m_emission_rate;
	}
	allocate_buffers();
}

void nsparticle_comp::enable_looping(bool pEnable)
{
	m_looping = pEnable;
}

void nsparticle_comp::set_material_id(const uivec2 & pID)
{
	m_mat_id = pID;
}

bool nsparticle_comp::looping()
{
	return m_looping;
}

void nsparticle_comp::video_context_init()
{
	vid_ctxt * vc = nse.video_driver()->current_context();
	if (vc != nullptr)
	{
		if (ctxt_objs[vc->context_id] == nullptr)
		{
			ctxt_objs[vc->context_id] = nse.factory<nsvid_obj_factory>(PARTICLE_VID_OBJ_GUID)->create(this);
		}
		else
		{
			dprint("nsparticle_comp::video_context_init - Context has already been initialized for particle_comp " + m_owner->name() + " in ctxtid " + std::to_string(vc->context_id));
		}
	}
}

void nsparticle_comp::release()
{}

void nsparticle_comp::set_rand_tex_id(const uivec2 & pID)
{
	m_rand_tex_id = pID;
}

const uivec2 & nsparticle_comp::rand_tex_id()
{
	return m_rand_tex_id;
}

void nsparticle_comp::pup(nsfile_pupper * p)
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

bool nsparticle_comp::first()
{
	return m_first;
}

void nsparticle_comp::reset()
{
	m_elapsed_time = 0.0f;
}

void nsparticle_comp::set_first(bool pSet)
{
	m_first = pSet;
}

nsparticle_comp & nsparticle_comp::operator=(nsparticle_comp rhs)
{
	nscomponent::operator=(rhs);
	std::swap(m_motion_keys, rhs.m_motion_keys);
	std::swap(m_visual_keys, rhs.m_visual_keys);
	std::swap(m_mat_id, rhs.m_mat_id);
	std::swap(m_xfb_shader_id, rhs.m_xfb_shader_id);
	std::swap(m_max_particle_count, rhs.m_max_particle_count);
	std::swap(m_lifetime, rhs.m_lifetime);
	std::swap(m_emission_rate, rhs.m_emission_rate);
	std::swap(m_rand_tex_id, rhs.m_rand_tex_id);
	std::swap(m_emitter_shape, rhs.m_emitter_shape);
	std::swap(m_motion_key_type, rhs.m_motion_key_type);
	std::swap(m_blend_mode, rhs.m_blend_mode);
	std::swap(m_ang_vel, rhs.m_ang_vel);
	std::swap(m_looping, rhs.m_looping);
	std::swap(m_motion_key_interp, rhs.m_motion_key_interp);
	std::swap(m_motion_global_time, rhs.m_motion_global_time);
	std::swap(m_visual_key_interp, rhs.m_visual_key_interp);
	std::swap(m_visual_global_time, rhs.m_visual_global_time);
	std::swap(m_elapsed_time, rhs.m_elapsed_time);
	std::swap(m_starting_size, rhs.m_starting_size);
	std::swap(m_emitter_size, rhs.m_emitter_size);
	std::swap(m_init_vel_mult, rhs.m_init_vel_mult);
	std::swap(m_max_motion_keys, rhs.m_max_motion_keys);
	std::swap(m_max_visual_keys, rhs.m_max_visual_keys);
	std::swap(m_simulating, rhs.m_simulating);
	std::swap(m_first, rhs.m_first);
	post_update(true);
	return (*this);
}

bool nsparticle_comp::simulating()
{
	return m_simulating;
}

void nsparticle_comp::enable_simulation(bool pEnable)
{
	m_simulating = pEnable;
}
