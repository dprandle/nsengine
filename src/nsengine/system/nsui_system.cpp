/*!
\file nsui_system.cpp

\brief Header file for nsui_system class

This file contains all of the neccessary declarations for the nsui_system class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#include <nsui_system.h>
#include <nsentity.h>
#include <nsscene.h>
#include <nsplugin_manager.h>
#include <nsui_comp.h>
#include <nsshader.h>
#include <nsmaterial.h>
#include <nsentity_manager.h>
#include <nsopengl_driver.h>
#include <nsfb_object.h>
#include <nstimer.h>
#include <nsrect_tform_comp.h>
#include <nsui_canvas_comp.h>

nsui_system::nsui_system()
{
	m_ui_draw_calls.reserve(MAX_UI_DRAW_CALLS);
	m_ordered_ents.reserve(START_MAX_ENTS);
}

nsui_system::~nsui_system()
{
	
}

void nsui_system::init()
{
	register_handler(nsui_system::_handle_mouse_event);
}

void nsui_system::release()
{
	
}

void nsui_system::set_active_viewport(nsrender::viewport * vp)
{
	m_active_vp = vp;
}

nsrender::viewport * nsui_system::active_viewport()
{
	return m_active_vp;
}

void nsui_system::push_draw_calls()
{
	if (m_active_vp == nullptr)
		return;
	
	m_ui_draw_calls.resize(0);
	drawcall_queue * dc_dq = nse.system<nsrender_system>()->queue(UI_RENDER_QUEUE);
	dc_dq->resize(0);

	for (uint32 i = 0; i < m_active_vp->ui_canvases.size(); ++i)
	{
		nsui_canvas_comp * uicc = m_active_vp->ui_canvases[i]->get<nsui_canvas_comp>();
		m_ordered_ents.resize(0);
		auto ents = uicc->entities_in_canvas();
		if (ents == nullptr)
			continue;
	
		auto ui_iter = ents->begin();
		while (ui_iter != ents->end())
		{
			nsui_comp * uic = (*ui_iter)->get<nsui_comp>();
			nsrect_tform_comp * tuic = (*ui_iter)->get<nsrect_tform_comp>();
			if (uic != nullptr && uic->show)
				m_ordered_ents.push_back(*ui_iter);
			++ui_iter;
		}
		_sort_ents(uicc);

		for (uint32 i = 0; i < m_ordered_ents.size(); ++i)
		{
			nsentity * cur_ent = m_ordered_ents[i];
			nsui_comp * uic = cur_ent->get<nsui_comp>();
			nsrect_tform_comp * tuic = cur_ent->get<nsrect_tform_comp>();
			m_ui_draw_calls.resize(m_ui_draw_calls.size()+1);
			ui_draw_call * uidc = &m_ui_draw_calls[m_ui_draw_calls.size()-1];

			uidc->mat = get_resource<nsmaterial>(uic->content_properties.mat_id);
			uidc->shdr = get_resource<nsshader>(uic->content_shader_id);
			uidc->border_shader = get_resource<nsshader>(uic->border_shader_id);
			uidc->entity_id = uivec3(cur_ent->full_id(),0);
			uidc->content_wscale = tuic->content_world_scale(uicc);
			uidc->border_pix = uic->outer_properties.border;
			uidc->content_tform = tuic->content_transform(uicc);
			uidc->border_color = uic->outer_properties.border_color;
			uidc->content_tex_coord_rect = uidc->mat->map_tex_coord_rect(nsmaterial::diffuse);
			dc_dq->push_back(uidc);
		}
	}
}

void nsui_system::update()
{
	auto vp_list = nse.system<nsrender_system>()->viewports();
	auto vp_iter = vp_list->begin();
	while (vp_iter != vp_list->end())
	{
		for (uint32 i = 0; i < vp_iter->vp->ui_canvases.size(); ++i)
		{
			const ivec2 & sz = nse.video_driver<nsopengl_driver>()->default_target()->size();
			fvec2 vp_size = vp_iter->vp->normalized_bounds.zw() - vp_iter->vp->normalized_bounds.xy();
			fvec2 fsz(sz.x,sz.y);
			nsentity * canvas = vp_iter->vp->ui_canvases[i];
			nsui_canvas_comp * uicc = canvas->get<nsui_canvas_comp>();
			uicc->update_rects(vp_size % fsz);
		}
		++vp_iter;
	}
}

void nsui_system::_sort_ents(nsui_canvas_comp * uicc)
{
	if (m_ordered_ents.size() <= 1)
		return;
	
	for (uint32 c = 0 ; c < (m_ordered_ents.size() - 1 ); ++c)
	{
		for (uint32 d = 0 ; d < m_ordered_ents.size() - c - 1; d++)
		{
			auto * uic = m_ordered_ents[d]->get<nsrect_tform_comp>()->canvas_info(uicc);
			auto * uic2 = m_ordered_ents[d+1]->get<nsrect_tform_comp>()->canvas_info(uicc);
			if (uic->layer > uic2->layer) /* For decreasing order use < */
			{
				nsentity * tmp = m_ordered_ents[d];
				m_ordered_ents[d]   = m_ordered_ents[d+1];
				m_ordered_ents[d+1] = tmp;
			}
		}
	}
}

int32 nsui_system::update_priority()
{
	return UI_SYS_UPDATE_PR;
}

bool nsui_system::_handle_mouse_event(nsmouse_move_event * evnt)
{
	for (uint32 i = 0; i < m_ordered_ents.size(); ++i)
	{
		// nsrect_tform_comp * uic = m_ordered_ents[i]->get<nsrect_tform_comp>();
		// nsui_comp * uicc = m_ordered_ents[i]->get<nsui_comp>();
		// if (uic->owner()->name() == "button_fun" || uic->owner()->name() == "button_start-screen")
		// 	continue;
		// nsmaterial * mat = get_resource<nsmaterial>(uicc->content_properties.mat_id);

		// fvec2 mouse_vec = evnt->normalized_mpos - uic->global_pos;
		// const ivec2 & sz= nse.video_driver<nsopengl_driver>()->default_target()->size();
		// mouse_vec = mouse_vec % fvec2(sz.w,sz.h);
		// fvec2 moved_vec = rotation2d_mat2(-uic->angle) * mouse_vec;
		// moved_vec = moved_vec / fvec2(sz.w,sz.h) + uic->global_pos;
		// fvec4 rect = fvec4(uic->global_pos - uic->global_size/2.0f,uic->global_pos + uic->global_size/2.0f);
		// if (moved_vec > rect.xy() && moved_vec < rect.zw())
		// {
		// 	mat->set_map_tex_coord_rect(nsmaterial::diffuse, fvec4(0.0,0.5,1.0,0.75));
		// }
		// else
		// {
		// 	mat->set_map_tex_coord_rect(nsmaterial::diffuse, fvec4(0.0,0.75,1.0,1.0));	
		// }
	}
	return true;
}
