/*!
  \file nsvideo_driver.cpp

  \brief Definition file for nsvideo_driver class

  This file contains all of the neccessary definitions for the nsvideo_driver class.

  \author Daniel Randle
  \date Feb 23 2016
  \copywrite Earth Banana Games 2013
*/

#include <nsvideo_driver.h>
#include <nsfb_object.h>
#include <nsengine.h>
#include <nsshader.h>

render_shaders::render_shaders() :
	deflt(nullptr),
	deflt_wireframe(nullptr),
	deflt_translucent(nullptr),
	dir_light(nullptr),
	point_light(nullptr),
	spot_light(nullptr),
	light_stencil(nullptr),
	shadow_cube(nullptr),
	shadow_2d(nullptr),
	frag_sort(nullptr),
	deflt_particle(nullptr),
	sel_shader(nullptr)
{}

bool render_shaders::error()
{
	return (
		deflt->error() != nsshader::error_none ||
		deflt_wireframe->error() != nsshader::error_none ||
		deflt_translucent->error() != nsshader::error_none ||
		dir_light->error() != nsshader::error_none ||
		point_light->error() != nsshader::error_none ||
		spot_light->error() != nsshader::error_none ||
		light_stencil->error() != nsshader::error_none ||
		shadow_cube->error() != nsshader::error_none ||
		shadow_2d->error() != nsshader::error_none ||
		frag_sort->error() != nsshader::error_none ||
		deflt_particle->error() != nsshader::error_none ||
		sel_shader->error() != nsshader::error_none);
}

bool render_shaders::valid()
{
	return (
		deflt != nullptr &&
		deflt_wireframe != nullptr &&
		deflt_translucent != nullptr &&
		dir_light != nullptr &&
		point_light != nullptr &&
		spot_light != nullptr &&
		light_stencil != nullptr &&
		shadow_cube != nullptr &&
		shadow_2d != nullptr &&
		frag_sort != nullptr &&
		deflt_particle != nullptr &&
		sel_shader != nullptr);
}

// system takes ownership and will delete on shutdown
bool nsvideo_driver::add_queue(const nsstring & name, drawcall_queue * q)
{
	return m_render_queues.emplace(name, q).second;	
}

void nsvideo_driver::clear_render_queues()
{
	while (m_render_queues.begin() != m_render_queues.end())
		destroy_queue(m_render_queues.begin()->first);
	m_render_queues.clear();	
}

drawcall_queue * nsvideo_driver::create_queue(const nsstring & name)
{
	drawcall_queue * q = new drawcall_queue;
	if (!add_queue(name,q))
	{
		delete q;
		q = nullptr;
	}
	return q;
}

void nsvideo_driver::destroy_queue(const nsstring & name)
{
	drawcall_queue * q = remove_queue(name);
	delete q;		
}

bool nsvideo_driver::initialized()
{
	return m_initialized;
}

void nsvideo_driver::init()
{
		
}

void nsvideo_driver::clear_render_targets()
{
	while (m_render_targets.begin() != m_render_targets.end())
		destroy_render_target(m_render_targets.begin()->first);
	m_render_targets.clear();	
}

void nsvideo_driver::clear_render_passes()
{
	while (m_render_passes.begin() != m_render_passes.end())
	{
		delete m_render_passes.back();
		m_render_passes.pop_back();
	}	
}

render_pass_vector * nsvideo_driver::render_passes()
{
	return &m_render_passes;
}

bool nsvideo_driver::add_render_target(const nsstring & name, nsrender_target * rt)
{
	return m_render_targets.emplace(name, rt).second;
}

nsrender_target * nsvideo_driver::default_target()
{
	return m_default_target;
}

nsrender_target * nsvideo_driver::remove_render_target(const nsstring & name)
{
	nsrender_target * fb = nullptr;
	auto iter = m_render_targets.find(name);
	if (iter != m_render_targets.end())
	{
		fb = iter->second;
		m_render_targets.erase(iter);
	}
	return fb;
}

nsrender_target * nsvideo_driver::render_target(const nsstring & name)
{
	auto iter = m_render_targets.find(name);
	if (iter != m_render_targets.end())
		return iter->second;
	return nullptr;
}

void nsvideo_driver::destroy_render_target(const nsstring & name)
{
	nsrender_target * rt = remove_render_target(name);
	rt->release();
	delete rt;
}


drawcall_queue * nsvideo_driver::queue(const nsstring & name)
{
	auto iter = m_render_queues.find(name);
	if (iter != m_render_queues.end())
		return iter->second;
	return nullptr;	
}

drawcall_queue * nsvideo_driver::remove_queue(const nsstring & name)
{
	drawcall_queue * q = nullptr;
	auto iter = m_render_queues.find(name);
	if (iter != m_render_queues.end())
	{
		q = iter->second;
		m_render_queues.erase(iter);
	}
	return q;
}

void nsvideo_driver::set_render_shaders(render_shaders rs)
{
	m_shaders = rs;
}

void nsvideo_driver::release()
{
	delete m_default_target;
	m_default_target = nullptr;

	clear_render_queues();
	clear_render_passes();
	clear_render_targets();
}

void nsvideo_driver::render(nsrender::viewport *vp)
{
	for (uint32 i = 0; i < m_render_passes.size(); ++i)
	{
		render_pass * rp = m_render_passes[i];
		if (rp == nullptr || rp->ren_target == nullptr)
		{
			dprint("nsvideo_driver::_render_pass - pass " + std::to_string(i) + " is not correctly setup");
		}
		else if (rp->enabled)
		{
			rp->vp = vp;
			rp->setup_pass();
			rp->render();
		}
	}
}
