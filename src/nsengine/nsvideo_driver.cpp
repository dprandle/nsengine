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
#include <nsentity.h>
#include <nscam_comp.h>

vp_node::vp_node(const nsstring & vp_name_, viewport * vp_):
	vp_name(vp_name_),
	vp(vp_)
{}

vp_node::~vp_node()
{}

vid_ctxt::vid_ctxt(uint32 cntxt_id):
	context_id(cntxt_id),
	initialized(false),
	render_queues(),
	render_passes(),
	need_release(),
	focused_vp(nullptr),
	vp_list()
{}

vid_ctxt::~vid_ctxt()
{
	while (render_passes.begin() != render_passes.end())
	{
		delete render_passes.back();
		render_passes.pop_back();
	}
	while (render_queues.begin() != render_queues.end())
	{
		delete render_queues.begin()->second;
		render_queues.erase(render_queues.begin());
	}
	while (vp_list.begin() != vp_list.end())
	{
		delete vp_list.back().vp;
		vp_list.pop_back();
	}
}

void vid_ctxt::update_vid_objs()
{
	auto iter = registered_vid_objs.begin();
	while (iter != registered_vid_objs.end())
	{
		if ((*iter)->needs_update)
			(*iter)->update();
		++iter;
	}
}

nsvideo_driver::nsvideo_driver():
	m_auto_update_vobjs(true),
	m_auto_cleanup(true),
	m_current_context(nullptr),
	m_contexts()
{}

nsvideo_driver::~nsvideo_driver()
{
	if (m_current_context != nullptr)
		destroy_context(m_current_context->context_id);
	for (uint8 i = 0; i < MAX_CONTEXT_COUNT; ++i)
	{
		if (m_contexts[i] != nullptr)
		{
			dprint("nsvideo_driver::~nsvideo_driver() Warning - deleting video driver without having deleted all contexts first");
		}
	}
}

void nsvideo_driver::destroy_all_render_passes()
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

void nsvideo_driver::clear_render_queues()
{
	auto rq_iter = m_current_context->render_queues.begin();
	while (rq_iter != m_current_context->render_queues.end())
	{
		rq_iter->second->resize(0);
		++rq_iter;
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

void nsvideo_driver::destroy_all_render_queues()
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
		m_current_context->vp_list.push_back(vp_node(vp_name, m_current_context->focused_vp));
		return m_current_context->focused_vp;
	}
	else
	{
		auto liter = m_current_context->vp_list.begin();
		while (liter != m_current_context->vp_list.end())
		{
			if (liter->vp_name == insert_before)
			{
				m_current_context->focused_vp = new viewport(vp);
				m_current_context->vp_list.insert(liter, vp_node(vp_name, m_current_context->focused_vp));
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

	auto liter = m_current_context->vp_list.begin();
	while (liter != m_current_context->vp_list.end())
	{
		if (liter->vp_name == vp_name)
		{
			if (m_current_context->focused_vp == liter->vp)
				m_current_context->focused_vp = nullptr;
			
			delete liter->vp;
			liter->vp = nullptr;
			m_current_context->vp_list.erase(liter);

			if (m_current_context->focused_vp == nullptr && !m_current_context->vp_list.empty())
				m_current_context->focused_vp = m_current_context->vp_list.back().vp;
			
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

	auto liter = m_current_context->vp_list.begin();
	while (liter != m_current_context->vp_list.end())
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

	if (m_current_context->vp_list.back().vp_name == vp_name)
		return;
	
	auto liter = m_current_context->vp_list.begin();
	while (liter != m_current_context->vp_list.end())
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

	if (m_current_context->vp_list.front().vp_name == vp_name)
		return;
	
	auto liter = m_current_context->vp_list.begin();
	while (liter != m_current_context->vp_list.end())
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

	if (m_current_context->vp_list.front().vp_name == vp_name)
		return;

	viewport * vp = find_viewport(vp_name);
	if (vp != nullptr)
	{
		viewport vp_copy(*vp);
		remove_viewport(vp_name);
		insert_viewport(vp_name, vp_copy, m_current_context->vp_list.front().vp_name);
	}
}

std::list<vp_node> * nsvideo_driver::viewports()
{
	if (m_current_context == nullptr)
	{
		dprint("nsvideo_driver::viewports Cannot get viewports until a valid context has been created");
		return nullptr;
	}
	return &m_current_context->vp_list;
}

vid_ctxt * nsvideo_driver::context(uint8 id)
{
	return m_contexts[id];
}

void nsvideo_driver::move_viewport_to_front(const nsstring & vp_name)
{
	if (m_current_context == nullptr)
	{
		dprint("nsvideo_driver::move_viewport_to_front Cannot move viewport until a valid context has been created");
		return;
	}

	if (m_current_context->vp_list.back().vp_name == vp_name)
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
	auto riter = m_current_context->vp_list.rbegin();
	while (riter != m_current_context->vp_list.rend())
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

	m_contexts[c_id]->release();
	delete m_contexts[c_id];
	m_contexts[c_id] = nullptr;
	return true;
}

bool nsvideo_driver::make_context_current(uint8 c_id)
{
	if (c_id >= MAX_CONTEXT_COUNT)
		return false;
	m_current_context = m_contexts[c_id];
	context_switch(m_current_context);
	if (m_auto_cleanup)
		cleanup_vid_objs();
	if (m_auto_update_vobjs)
		m_current_context->update_vid_objs();
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

void nsvideo_driver::window_resized(const ivec2 & new_size)
{
	if (m_current_context == nullptr)
		return;

	// ubdate the actual vp pixel sizes and resize the viewports' camera
	auto iter = m_current_context->vp_list.begin();
	while (iter != m_current_context->vp_list.end())
	{
		if (iter->vp->normalized_bounds != fvec4(0.0f))
		{
			iter->vp->bounds.x = iter->vp->normalized_bounds.x * new_size.x;
			iter->vp->bounds.y = iter->vp->normalized_bounds.y * new_size.y;
            iter->vp->bounds.z = iter->vp->normalized_bounds.z * new_size.x;
			iter->vp->bounds.w = iter->vp->normalized_bounds.w * new_size.y;
			if (iter->vp->camera != nullptr)
			{
				nscam_comp * cc = iter->vp->camera->get<nscam_comp>();
				cc->resize_screen(iter->vp->bounds.zw() - iter->vp->bounds.xy());
			}
		}
		++iter;
	}
}

nsvid_obj::nsvid_obj(nsvideo_object * parent_):
		needs_update(true),
		parent(parent_)
{
	nse.video_driver()->current_context()->registered_vid_objs.emplace(this);
}
	
nsvid_obj::~nsvid_obj()
{
	nse.video_driver()->current_context()->registered_vid_objs.erase(this);
}

nsvideo_object::nsvideo_object():
	ctxt_objs(),
	share_between_contexts(true)
{
	sig_connect(nse.video_driver()->context_switch, nsvideo_object::on_context_switch);
}

bool nsvideo_object::context_sharing()
{
	return share_between_contexts;
}

void nsvideo_object::enable_context_sharing(bool enable)
{
	share_between_contexts = enable;
}

void nsvideo_object::on_context_switch(vid_ctxt * current_ctxt)
{
	if (!initialized(current_ctxt))
		video_context_init();
}

bool nsvideo_object::initialized(vid_ctxt * ctxt)
{
	return ctxt_objs[ctxt->context_id] != nullptr;
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

nsvid_obj * nsvideo_object::video_obj()
{
	uint8 context_id = nse.video_driver()->current_context()->context_id;
	return ctxt_objs[context_id];
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
