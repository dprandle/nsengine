/*!
  \file nsvideo_driver.cpp

  \brief Definition file for nsvideo_driver class

  This file contains all of the neccessary definitions for the nsvideo_driver class.

  \author Daniel Randle
  \date Feb 23 2016
  \copywrite Earth Banana Games 2013
*/

#include <nsvideo_driver.h>
#include <opengl/nsgl_framebuffer.h>
#include <nsengine.h>
#include <asset/nsshader.h>
#include <asset/nsentity.h>
#include <component/nscam_comp.h>

vp_node::vp_node(const nsstring & vp_name_, viewport * vp_):
	vp_name(vp_name_),
	vp(vp_)
{}

vp_node::~vp_node()
{}

vid_ctxt::vid_ctxt(uint32 cntxt_id):
	auto_cleanup(true),
	auto_update_vobjs(true),
	context_id(cntxt_id),
	initialized(false),
	render_queues(),
	render_passes(),
	need_release(),
	focused_vp(nullptr),
	vp_list(),
	registered_vid_objs()
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

void vid_ctxt::release()
{
	while (registered_vid_objs.begin() != registered_vid_objs.end())
		(*registered_vid_objs.begin())->parent->video_context_release();
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

viewport * vid_ctxt::insert_viewport(
	const nsstring & vp_name,
	const viewport & vp,
	const nsstring & insert_before)
{
	if (find_viewport(vp_name) != nullptr)
	{
		dprint("vid_ctxt::insert_viewport Cannot insert two viewpots with the same name - " + vp_name);
		return nullptr;
	}
	
	if (insert_before.empty())
	{
		if (vp_name.empty())
			return nullptr;
		
		focused_vp = new viewport(vp);
		vp_list.push_back(vp_node(vp_name, focused_vp));
		return focused_vp;
	}
	else
	{
		auto liter = vp_list.begin();
		while (liter != vp_list.end())
		{
			if (liter->vp_name == insert_before)
			{
				focused_vp = new viewport(vp);
				vp_list.insert(liter, vp_node(vp_name, focused_vp));
				return focused_vp;
			}
			++liter;
		}
		return nullptr;
	}
}

bool vid_ctxt::remove_viewport(const nsstring & vp_name)
{
	auto liter = vp_list.begin();
	while (liter != vp_list.end())
	{
		if (liter->vp_name == vp_name)
		{
			if (focused_vp == liter->vp)
				focused_vp = nullptr;
			
			delete liter->vp;
			liter->vp = nullptr;
			vp_list.erase(liter);

			if (focused_vp == nullptr && !vp_list.empty())
				focused_vp = vp_list.back().vp;
			
			return true;
		}
		++liter;
	}
	return false;
}

viewport * vid_ctxt::find_viewport(const nsstring & vp_name)
{
	auto liter = vp_list.begin();
	while (liter != vp_list.end())
	{
		if (liter->vp_name == vp_name)
			return liter->vp;
		++liter;
	}
	return nullptr;
}

void vid_ctxt::move_viewport_forward(const nsstring & vp_name)
{
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

void vid_ctxt::move_viewport_back(const nsstring & vp_name)
{
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

void vid_ctxt::move_viewport_to_back(const nsstring & vp_name)
{
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

void vid_ctxt::move_viewport_to_front(const nsstring & vp_name)
{
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

viewport * vid_ctxt::front_viewport_at_screen_pos(const fvec2 & screen_pos)
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

void vid_ctxt::cleanup_vid_objs()
{
	for (uint32 i = 0; i < need_release.size(); ++i)
		delete need_release[i];
	need_release.clear();
}

// system takes ownership and will delete on shutdown
bool vid_ctxt::add_queue(const nsstring & name, render_queue * q)
{
	return render_queues.emplace(name, q).second;
}

void vid_ctxt::destroy_all_render_queues()
{
	while (render_queues.begin() != render_queues.end())
		destroy_queue(render_queues.begin()->first);
	render_queues.clear();
}

render_queue * vid_ctxt::create_queue(const nsstring & name)
{
	render_queue * q = new render_queue;
	if (!add_queue(name,q))
	{
		delete q;
		q = nullptr;
	}
	return q;
}

void vid_ctxt::destroy_queue(const nsstring & name)
{
	render_queue * q = remove_queue(name);
	delete q;		
}

render_queue * vid_ctxt::queue(const nsstring & name)
{
	auto iter = render_queues.find(name);
	if (iter != render_queues.end())
		return iter->second;
	return nullptr;	
}

render_queue * vid_ctxt::remove_queue(const nsstring & name)
{
	render_queue * q = nullptr;
	auto iter = render_queues.find(name);
	if (iter != render_queues.end())
	{
		q = iter->second;
		render_queues.erase(iter);
	}
	return q;
}

void vid_ctxt::clear_render_queues()
{
	auto rq_iter = render_queues.begin();
	while (rq_iter != render_queues.end())
	{
		rq_iter->second->resize(0);
		++rq_iter;
	}
}

void vid_ctxt::destroy_all_render_passes()
{
	while (render_passes.begin() != render_passes.end())
	{
		delete render_passes.back();
		render_passes.pop_back();
	}
}

void vid_ctxt::window_resized(const ivec2 & new_size)
{
	// ubdate the actual vp pixel sizes and resize the viewports' camera
	auto iter = vp_list.begin();
	while (iter != vp_list.end())
	{
		if (iter->vp->normalized_bounds != fvec4(0.0f))
		{
            iter->vp->bounds.x = int32(iter->vp->normalized_bounds.x * new_size.x);
            iter->vp->bounds.y = int32(iter->vp->normalized_bounds.y * new_size.y);
            iter->vp->bounds.z = int32(iter->vp->normalized_bounds.z * new_size.x);
            iter->vp->bounds.w = int32(iter->vp->normalized_bounds.w * new_size.y);
			if (iter->vp->camera != nullptr)
			{
				nscam_comp * cc = iter->vp->camera->get<nscam_comp>();
				cc->resize_screen(iter->vp->bounds.zw() - iter->vp->bounds.xy());
			}
		}
		++iter;
	}
}

nsvideo_driver::nsvideo_driver():
	m_current_context(nullptr),
	m_contexts(),
	m_initialized(false)
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

void nsvideo_driver::init()
{
	m_initialized = true;
}

void nsvideo_driver::release()
{
	m_initialized = false;
}

vid_ctxt * nsvideo_driver::context(uint8 id)
{
	return m_contexts[id];
}

bool nsvideo_driver::destroy_context(uint8 c_id)
{
	if (c_id >= MAX_CONTEXT_COUNT)
		return false;
	uint8 current_id = m_current_context->context_id;
	
	m_contexts[c_id]->release();
	delete m_contexts[c_id];
	m_contexts[c_id] = nullptr;

	if (c_id == current_id)
		m_current_context = nullptr;
	
	return true;
}

bool nsvideo_driver::make_context_current(uint8 c_id)
{
	if (c_id >= MAX_CONTEXT_COUNT)
		return false;
	m_current_context = m_contexts[c_id];

	// Context switch signal occurs which should be connected to the
	// slot on each nsvid_obj - this slot will initialize any uninitialized vid_ctxt_objs in theory
	context_switch(m_current_context);

	// Auto cleanup will delete all vid_ctxt_objs that were deleted
	if (m_current_context->auto_cleanup)
		m_current_context->cleanup_vid_objs();

	return true;
}

vid_ctxt * nsvideo_driver::current_context()
{
	return m_current_context;
}

bool nsvideo_driver::initialized()
{
	return m_initialized;
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
			ctxt_objs[i]->parent = nullptr;
			ctxt->need_release.push_back(ctxt_objs[i]);
			ctxt_objs[i] = nullptr;
		}
	}
}

nsvid_obj * nsvideo_object::video_obj()
{
	vid_ctxt * vcxt = nse.video_driver()->current_context();
	if (vcxt != nullptr)
		return ctxt_objs[vcxt->context_id];
	return nullptr;
}
	
void nsvideo_object::video_context_release()
{
	vid_ctxt * vcxt = nse.video_driver()->current_context();
	if (vcxt != nullptr)
	{
		if (ctxt_objs[vcxt->context_id] != nullptr)
		{
			delete ctxt_objs[vcxt->context_id];
			ctxt_objs[vcxt->context_id] = nullptr;
			dprint("nsvideo_object::video_context_release Video object of context " + std::to_string(vcxt->context_id) + " was successfully released");
		}
	}
}
	
void nsvideo_object::video_update()
{
	vid_ctxt * cxt = nse.video_driver()->current_context();
	if (cxt != nullptr)
	{
		for (uint8 i = 0; i < 16; ++i)
		{
			if (ctxt_objs[i] != nullptr)
				ctxt_objs[i]->needs_update = true;
		}
	}
}
