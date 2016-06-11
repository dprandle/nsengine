/*!
  \file nsvideo_driver.cpp

  \brief Definition file for nsvideo_driver class

  This file contains all of the neccessary definitions for the nsvideo_driver class.

  \author Daniel Randle
  \date Feb 23 2016
  \copywrite Earth Banana Games 2013
*/

#include <nsvideo_driver.h>
#include <nsgl_framebuffer.h>
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

bool render_shaders::error() const
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

bool render_shaders::valid() const
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

vp_node::vp_node(const nsstring & vp_name_, viewport * vp_):
	vp_name(vp_name_),
	vp(vp_)
{}

vp_node::~vp_node()
{}

nsvideo_driver::nsvideo_driver():
	m_auto_cleanup(true)
{}

nsvideo_driver::~nsvideo_driver()
{}

void nsvideo_driver::clear_render_passes()
{
	if (m_current_context == nullptr)
	{
		dprint("nsvideo_driver::clear_render_passes Cannot clear passes until a valid context has been created");
		return;
	}

	while (m_current_context->render_passes.begin() != m_current_context->render_passes.end())
	{
		delete m_current_context->render_passes.back();
		m_current_context->render_passes.pop_back();
	}
}

render_pass_vector * nsvideo_driver::render_passes()
{
	if (m_current_context == nullptr)
	{
		dprint("nsvideo_driver::render_passes Cannot get passes until a valid context has been created");
		return nullptr;
	}	
	return &m_current_context->render_passes;
}

// system takes ownership and will delete on shutdown
bool nsvideo_driver::add_queue(const nsstring & name, render_queue * q)
{
	if (m_current_context == nullptr)
	{
		dprint("nsvideo_driver::add_queue Cannot add queue until a valid context has been created");
		return false;
	}
	return m_current_context->render_queues.emplace(name, q).second;
}

void nsvideo_driver::clear_render_queues()
{
	if (m_current_context == nullptr)
	{
		dprint("nsvideo_driver::clear_render_queues Cannot clear queues until a valid context has been created");
		return;
	}

	while (m_current_context->render_queues.begin() != m_current_context->render_queues.end())
		destroy_queue(m_current_context->render_queues.begin()->first);
	m_current_context->render_queues.clear();
}

render_queue * nsvideo_driver::create_queue(const nsstring & name)
{
	if (m_current_context == nullptr)
	{
		dprint("nsvideo_driver::create_queue Cannot create queue until a valid context has been created");
		return nullptr;
	}
	render_queue * q = new render_queue;
	if (!add_queue(name,q))
	{
		delete q;
		q = nullptr;
	}
	return q;
}

void nsvideo_driver::destroy_queue(const nsstring & name)
{
	if (m_current_context == nullptr)
	{
		dprint("nsvideo_driver::destroy_queue Cannot destroy queue until a valid context has been created");
		return;
	}
	render_queue * q = remove_queue(name);
	delete q;		
}

render_queue * nsvideo_driver::queue(const nsstring & name)
{
	if (m_current_context == nullptr)
	{
		dprint("nsvideo_driver::queue Cannot get queue until a valid context has been created");
		return nullptr;
	}

	auto iter = m_current_context->render_queues.find(name);
	if (iter != m_current_context->render_queues.end())
		return iter->second;
	return nullptr;	
}

render_queue * nsvideo_driver::remove_queue(const nsstring & name)
{
	if (m_current_context == nullptr)
	{
		dprint("nsvideo_driver::remove_queue Cannot remove queue until a valid context has been created");
		return nullptr;
	}
	
	render_queue * q = nullptr;
	auto iter = m_current_context->render_queues.find(name);
	if (iter != m_current_context->render_queues.end())
	{
		q = iter->second;
		m_current_context->render_queues.erase(iter);
	}
	return q;
}

viewport * nsvideo_driver::insert_viewport(
	const nsstring & vp_name,
	const viewport & vp,
	const nsstring & insert_before)
{
	if (m_current_context == nullptr)
	{
		dprint("nsvideo_driver::insert_viewport Cannot insert viewport until a valid context has been created");
		return nullptr;
	}

	if (find_viewport(vp_name) != nullptr)
	{
		dprint("nsvideo_driver::insert_viewport Cannot insert two viewpots with the same name - " + vp_name);
		return nullptr;
	}
	
	if (insert_before.empty())
	{
		if (vp_name.empty())
			return nullptr;
		
		m_current_context->focused_vp = new viewport(vp);
		vp_list.push_back(vp_node(vp_name, m_current_context->focused_vp));
		return m_current_context->focused_vp;
	}
	else
	{
		auto liter = vp_list.begin();
		while (liter != vp_list.end())
		{
			if (liter->vp_name == insert_before)
			{
				m_current_context->focused_vp = new viewport(vp);
				vp_list.insert(liter, vp_node(vp_name, m_current_context->focused_vp));
				return m_current_context->focused_vp;
			}
			++liter;
		}
		return nullptr;
	}
}

bool nsvideo_driver::remove_viewport(const nsstring & vp_name)
{
	if (m_current_context == nullptr)
	{
		dprint("nsvideo_driver::remove_viewport Cannot remove viewport until a valid context has been created");
		return false;
	}

	auto liter = vp_list.begin();
	while (liter != vp_list.end())
	{
		if (liter->vp_name == vp_name)
		{
			if (m_current_context->focused_vp == liter->vp)
				m_current_context->focused_vp = nullptr;
			
			delete liter->vp;
			liter->vp = nullptr;
			vp_list.erase(liter);

			if (m_current_context->focused_vp == nullptr && !vp_list.empty())
				m_current_context->focused_vp = vp_list.back().vp;
			
			return true;
		}
		++liter;
	}
	return false;
}

viewport * nsvideo_driver::find_viewport(const nsstring & vp_name)
{
	if (m_current_context == nullptr)
	{
		dprint("nsvideo_driver::find_viewport Cannot find viewport until a valid context has been created");
		return nullptr;
	}

	auto liter = vp_list.begin();
	while (liter != vp_list.end())
	{
		if (liter->vp_name == vp_name)
			return liter->vp;
		++liter;
	}
	return nullptr;
}

void nsvideo_driver::move_viewport_forward(const nsstring & vp_name)
{
	if (m_current_context == nullptr)
	{
		dprint("nsvideo_driver::move_viewport_forward Cannot move viewport until a valid context has been created");
		return;
	}

	if (vp_list.back().vp_name == vp_name)
		return;
	
	auto liter = vp_list.begin();
	while (liter != vp_list.end())
	{
		if (liter->vp_name == vp_name)
		{
			auto tmp_current = liter, liter_current = liter;
			++liter;
			(*liter_current) = (*liter);
			(*liter) = (*tmp_current);
			return;
		}
		++liter;
	}
}

void nsvideo_driver::move_viewport_back(const nsstring & vp_name)
{
	if (m_current_context == nullptr)
	{
		dprint("nsvideo_driver::move_viewport_back Cannot move viewport until a valid context has been created");
		return;
	}

	if (vp_list.front().vp_name == vp_name)
		return;
	
	auto liter = vp_list.begin();
	while (liter != vp_list.end())
	{
		if (liter->vp_name == vp_name)
		{
			auto tmp_current = liter, liter_current = liter;
			--liter;
			(*liter_current) = (*liter);
			(*liter) = (*tmp_current);
			return;
		}
		++liter;
	}		
}

void nsvideo_driver::move_viewport_to_back(const nsstring & vp_name)
{
	if (m_current_context == nullptr)
	{
		dprint("nsvideo_driver::move_viewport_to_back Cannot move viewport until a valid context has been created");
		return;
	}

	if (vp_list.front().vp_name == vp_name)
		return;

	viewport * vp = find_viewport(vp_name);
	if (vp != nullptr)
	{
		viewport vp_copy(*vp);
		remove_viewport(vp_name);
		insert_viewport(vp_name, vp_copy, vp_list.front().vp_name);
	}
}

void nsvideo_driver::move_viewport_to_front(const nsstring & vp_name)
{
	if (m_current_context == nullptr)
	{
		dprint("nsvideo_driver::move_viewport_to_front Cannot move viewport until a valid context has been created");
		return;
	}

	if (vp_list.back().vp_name == vp_name)
		return;

	viewport * vp = find_viewport(vp_name);
	if (vp != nullptr)
	{
		viewport vp_copy(*vp);
		remove_viewport(vp_name);
		insert_viewport(vp_name, vp_copy);
	}
}

viewport * nsvideo_driver::focused_viewport()
{
	if (m_current_context == nullptr)
	{
		dprint("nsvideo_driver::focused_viewport Cannot get viewport until a valid context has been created");
		return nullptr;
	}

	return m_current_context->focused_vp;
}
void nsvideo_driver::set_focused_viewport(viewport * vp)
{
	if (m_current_context == nullptr)
	{
		dprint("nsvideo_driver::set_focused_viewport Cannot set viewport until a valid context has been created");
		return;
	}

	m_current_context->focused_vp = vp;
}

viewport * nsvideo_driver::front_viewport(const fvec2 & screen_pos)
{
	auto riter = vp_list.rbegin();
	while (riter != vp_list.rend())
	{
		if (screen_pos < riter->vp->normalized_bounds.zw() && screen_pos > riter->vp->normalized_bounds.xy())
			return riter->vp;
		++riter;
	}
	return nullptr;
}

bool nsvideo_driver::destroy_context(uint8 c_id)
{
	if (c_id >= MAX_CONTEXT_COUNT)
		return false;
	
	delete m_contexts[c_id];
	m_contexts[c_id] = nullptr;
	return true;
}

bool nsvideo_driver::make_context_current(uint8 c_id)
{
	if (c_id >= MAX_CONTEXT_COUNT)
		return false;
	m_current_context = m_contexts[c_id];
	if (m_auto_cleanup)
		cleanup_vid_objs();
	return true;
}

vid_ctxt * nsvideo_driver::current_context()
{
	return m_current_context;
}

void nsvideo_driver::cleanup_vid_objs()
{
	if (m_current_context == nullptr)
		return;
	
	for (uint32 i = 0; i < m_current_context->need_release.size(); ++i)
		delete m_current_context->need_release[i];

	m_current_context->need_release.clear();
}

void nsvideo_driver::enable_auto_cleanup(bool enable)
{
	m_auto_cleanup = enable;
}

bool nsvideo_driver::auto_cleanup()
{
	return m_auto_cleanup;
}

bool nsvideo_driver::initialized()
{
	if (m_current_context == nullptr)
		return false;
	return m_current_context->initialized;
}

nsvideo_object::nsvideo_object():
	ctxt_objs()
{}

bool nsvideo_object::initialized()
{
	uint8 context_id = nse.video_driver()->current_context()->context_id;
	return ctxt_objs[context_id] != nullptr;
}

nsvideo_object::~nsvideo_object()
{
	video_context_release();
	for (uint8 i = 0; i < 16; ++i)
	{
		if (ctxt_objs[i] != nullptr)
		{
			vid_ctxt * ctxt = nse.video_driver()->context(i);
			ctxt->need_release.push_back(ctxt_objs[i]);
			ctxt_objs[i] = nullptr;
		}
	}
}
	
void nsvideo_object::video_context_release()
{
	uint8 context_id = nse.video_driver()->current_context()->context_id;
	if (ctxt_objs[context_id] != nullptr)
	{
		delete ctxt_objs[context_id];
		ctxt_objs[context_id] = nullptr;
		dprint("nsvideo_object::video_context_release Video object of context " + std::to_string(context_id) + " was successfully released");
	}
}
	
void nsvideo_object::video_update()
{
	for (uint8 i = 0; i < 16; ++i)
	{
		if (ctxt_objs[i] != nullptr)
			ctxt_objs[i]->update();
	}
}
