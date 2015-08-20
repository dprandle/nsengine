/*!
\file nsparticlecomp.cpp

\brief Definition file for nsparticle_comp class

This file contains all of the neccessary definitions for the nsparticle_comp class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/


#include <nsparticle_comp.h>
#include <nsxfb_object.h>
#include <nsvertex_array_object.h>
#include <nsbuffer_object.h>
#include <nstex_manager.h>
#include <nsshader_manager.h>
#include <nsentity.h>
#include <nsmat_manager.h>
#include <nsengine.h>

nsparticle_comp::nsparticle_comp() :
front_buffer(new nsbuffer_object(nsbuffer_object::array, nsbuffer_object::storage_mutable)),
back_buffer(new nsbuffer_object(nsbuffer_object::array, nsbuffer_object::storage_mutable)),
m_buffer_index(0),
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
m_emitter_size(1.0f, 1.0f, 1.0f),
nscomponent()
{
	m_xfb_bufs[0] = new nsxfb_object();
	m_xfb_bufs[1] = new nsxfb_object();
	m_vaos[0] = new nsvertex_array_object();
	m_vaos[1] = new nsvertex_array_object();
	m_motion_keys[0] = fvec3();
	m_motion_keys[m_max_motion_keys] = fvec3();
	m_visual_keys[0] = fvec3(1.0f,1.0f,1.0f);
	m_visual_keys[m_max_visual_keys] = fvec3(1.0f, 1.0f, 1.0f);
}

nsparticle_comp::~nsparticle_comp()
{
	release();
	delete front_buffer;
	delete back_buffer;
	delete m_xfb_bufs[0];
	delete m_xfb_bufs[1];
	delete m_vaos[0];
	delete m_vaos[1];
}

nsparticle_comp* nsparticle_comp::copy(const nscomponent * pToCopy)
{
	if (pToCopy == NULL)
		return NULL;
	const nsparticle_comp * comp = (const nsparticle_comp*)pToCopy;
	(*this) = (*comp);
	return this;
}

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

/*!
Get the resources that the component uses. If no resources are used then leave this unimplemented - will return an empty map.
/return Map of resource ID to resource type containing all used resources
*/
uivec2_vector nsparticle_comp::resources()
{
	uivec2_vector ret;

	if (m_rand_tex_id != 0)
		ret.push_back(m_rand_tex_id);

	if (m_mat_id != 0)
		ret.push_back(m_mat_id);

	if (m_xfb_shader_id != 0)
		ret.push_back(m_xfb_shader_id);

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
{
	m_xfb_bufs[0]->init_gl();
	m_xfb_bufs[1]->init_gl();
	m_vaos[0]->init_gl();
	m_vaos[1]->init_gl();
	front_buffer->init_gl();
	back_buffer->init_gl();

	m_particles.resize(m_max_particle_count);
	m_particles[0].age_type_reserved.y = 1.0f;

	m_vaos[0]->bind();
	back_buffer->bind();
	back_buffer->allocate(m_particles,
					   nsbuffer_object::mutable_dynamic_draw,
					   static_cast<uint32>(m_particles.size()));
	m_vaos[0]->enable(0);
	m_vaos[0]->vertex_attrib_ptr(0, 4, GL_FLOAT, GL_FALSE, sizeof(particle), 0);
	m_vaos[0]->enable(1);
	m_vaos[0]->vertex_attrib_ptr(1, 4, GL_FLOAT, GL_FALSE, sizeof(particle), sizeof(fvec4));
	m_vaos[0]->enable(2);
	m_vaos[0]->vertex_attrib_ptr(2, 4, GL_FLOAT, GL_FALSE, sizeof(particle), sizeof(fvec4) * 2);
	m_vaos[0]->enable(3);
	m_vaos[0]->vertex_attrib_ptr(3, 4, GL_FLOAT, GL_FALSE, sizeof(particle), sizeof(fvec4) * 3);
	m_vaos[0]->unbind();

	m_vaos[1]->bind();
	front_buffer->bind();
	front_buffer->allocate(m_particles,
						nsbuffer_object::mutable_dynamic_draw,
						static_cast<uint32>(m_particles.size()));
	m_vaos[1]->enable(0);
	m_vaos[1]->vertex_attrib_ptr(0, 4, GL_FLOAT, GL_FALSE, sizeof(particle), 0);
	m_vaos[1]->enable(1);
	m_vaos[1]->vertex_attrib_ptr(1, 4, GL_FLOAT, GL_FALSE, sizeof(particle), sizeof(fvec4));
	m_vaos[1]->enable(2);
	m_vaos[1]->vertex_attrib_ptr(2, 4, GL_FLOAT, GL_FALSE, sizeof(particle), sizeof(fvec4) * 2);
	m_vaos[1]->enable(3);
	m_vaos[1]->vertex_attrib_ptr(3, 4, GL_FLOAT, GL_FALSE, sizeof(particle), sizeof(fvec4) * 3);
	m_vaos[1]->unbind();

	front_buffer->set_target(nsbuffer_object::transform_feedback);
	back_buffer->set_target(nsbuffer_object::transform_feedback);
	m_xfb_bufs[0]->set_primitive(nsxfb_object::gl_points);
	m_xfb_bufs[1]->set_primitive(nsxfb_object::gl_points);

	m_xfb_bufs[0]->bind();
	front_buffer->bind();
	front_buffer->bind(0);
	m_xfb_bufs[0]->unbind();

	m_xfb_bufs[1]->bind();
	back_buffer->bind();
	back_buffer->bind(0);
	m_xfb_bufs[1]->unbind();
}

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
	if (pMax > PARTICLE_MAX_MOTION_KEYS)
		return;

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
	if (pMax > PARTICLE_MAX_VISUAL_KEYS)
		return;

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
{
	m_particles.resize(m_max_particle_count);
	m_particles[0].age_type_reserved.y = 1.0f;

	front_buffer->set_target(nsbuffer_object::array);
	back_buffer->set_target(nsbuffer_object::array);

	back_buffer->bind();
	back_buffer->allocate(m_particles,
					   nsbuffer_object::mutable_dynamic_draw,
					   static_cast<uint32>(m_particles.size()));
	front_buffer->bind();
	back_buffer->allocate(m_particles,
					   nsbuffer_object::mutable_dynamic_draw,
					   static_cast<uint32>(m_particles.size()));
	front_buffer->unbind();
}

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

void nsparticle_comp::release()
{
	m_xfb_bufs[0]->release();
	m_xfb_bufs[1]->release();
	m_vaos[0]->release();
	m_vaos[1]->release();
	front_buffer->release();
	back_buffer->release();
}

void nsparticle_comp::set_rand_tex_id(const uivec2 & pID)
{
	m_rand_tex_id = pID;
}

const uivec2 & nsparticle_comp::rand_tex_id()
{
	return m_rand_tex_id;
}

uint32 nsparticle_comp::xfb_id()
{
	return m_xfb_bufs[1 - m_buffer_index]->gl_id();
}

nsxfb_object * nsparticle_comp::xfb_obj()
{
	return m_xfb_bufs[m_buffer_index];
}

nsvertex_array_object * nsparticle_comp::va_obj()
{
	return m_vaos[m_buffer_index];
}

void nsparticle_comp::swap()
{
	m_buffer_index = 1 - m_buffer_index;
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

nsparticle_comp & nsparticle_comp::operator=(const nsparticle_comp & rhs)
{
	// Check for self assignment
	if (this == &rhs)
		return *this;

	release();
	m_motion_keys.clear();
	m_visual_keys.clear();
	m_motion_keys.insert(rhs.m_motion_keys.begin(), rhs.m_motion_keys.end());
	m_visual_keys.insert(rhs.m_visual_keys.begin(), rhs.m_visual_keys.end());

	m_mat_id = rhs.m_mat_id;
	m_xfb_shader_id = rhs.m_xfb_shader_id;
	m_max_particle_count = rhs.m_max_particle_count;
	m_lifetime = rhs.m_lifetime;
	m_emission_rate = rhs.m_emission_rate;
	m_rand_tex_id = rhs.m_rand_tex_id;
	m_emitter_shape = rhs.m_emitter_shape;
	m_motion_key_type = rhs.m_motion_key_type;
	m_blend_mode = rhs.m_blend_mode;
	m_ang_vel = rhs.m_ang_vel;
	m_looping = rhs.m_looping;
	m_motion_key_interp = rhs.m_motion_key_interp;
	m_motion_global_time = rhs.m_motion_global_time;
	m_visual_key_interp = rhs.m_visual_key_interp;
	m_visual_global_time = rhs.m_visual_global_time;
	m_elapsed_time = rhs.m_elapsed_time;
	m_starting_size = rhs.m_starting_size;
	m_emitter_size = rhs.m_emitter_size;
	m_init_vel_mult = rhs.m_init_vel_mult;
	m_max_motion_keys = rhs.m_max_motion_keys;
	m_max_visual_keys = rhs.m_max_visual_keys;

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
