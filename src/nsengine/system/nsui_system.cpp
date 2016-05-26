/*!
\file nsui_system.cpp

\brief Header file for nsui_system class

This file contains all of the neccessary declarations for the nsui_system class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#include <nsselection_system.h>
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
#include <nsui_button_comp.h>
#include <nsfont.h>

nsui_system::nsui_system():
	m_ui_draw_calls(),
	m_active_vp(nullptr),
	m_pressed_button(nullptr),
	m_focused_ui_ent(nullptr)
{
	m_ui_draw_calls.reserve(MAX_UI_DRAW_CALLS);
}

nsui_system::~nsui_system()
{
	
}

void nsui_system::init()
{
	register_handler(nsui_system::_handle_mouse_event);
	
	// handle key presses for ui elements that allow text editing input
	register_action_handler(nsui_system::_handle_mouse_press, UI_MOUSE_PRESS);
	register_action_handler(nsui_system::_handle_mouse_release, UI_MOUSE_RELEASE);
	register_action_handler(nsui_system::_handle_key_press, UI_KEY_PRESS);
	register_action_handler(nsui_system::_handle_shifted_key_press, UI_SHIFTED_KEY_PRESS);
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
	{
		dprint("nsui_system::push_draw_calls No active viewport set");
		return;
	}
	
	m_ui_draw_calls.resize(0);
	drawcall_queue * dc_dq = nse.system<nsrender_system>()->queue(UI_RENDER_QUEUE);
	dc_dq->resize(0);
	for (uint32 i = 0; i < m_active_vp->ui_canvases.size(); ++i)
	{
		nsui_canvas_comp * uicc = m_active_vp->ui_canvases[i]->get<nsui_canvas_comp>();
		for (uint32 i = 0; i < uicc->m_ordered_ents.size(); ++i)
		{
			nsentity * cur_ent = uicc->m_ordered_ents[i];
			nsui_comp * uic = cur_ent->get<nsui_comp>();
			nsrect_tform_comp * tuic = cur_ent->get<nsrect_tform_comp>();
			nsui_button_comp * uib = cur_ent->get<nsui_button_comp>();
			
			m_ui_draw_calls.resize(m_ui_draw_calls.size()+1);
			ui_draw_call * uidc = &m_ui_draw_calls[m_ui_draw_calls.size()-1];

			uidc->mat = get_resource<nsmaterial>(uic->mat_id);
			uidc->shdr = get_resource<nsshader>(uic->content_shader_id);
			uidc->text_shader = get_resource<nsshader>(uic->text_shader_id);
			uidc->border_shader = get_resource<nsshader>(uic->border_shader_id);
			uidc->entity_id = uivec3(cur_ent->full_id(),0);
			uidc->content_wscale = tuic->content_world_scale(uicc);
			uidc->border_pix = uic->border;
			uidc->content_tform = tuic->content_transform(uicc);
			uidc->border_color = uic->border_color;
			uidc->content_tex_coord_rect = uidc->mat->map_tex_coord_rect(nsmaterial::diffuse);
			uidc->fnt = get_resource<nsfont>(uic->font_id);
			uidc->text = uic->text;
			uidc->color_multiplier = fvec4(1.0f);
			if (uib != nullptr && uib->change_button_using == color_multiplier)
			{
				if (uib->is_hover)
				{
					if (uib->is_pressed)
					{
						uidc->color_multiplier = uib->colors[1];
					}
					else
					{
						uidc->color_multiplier = uib->colors[0];
					}
				}
			}
			dc_dq->push_back(uidc);
		}
	}
}

void nsui_system::update()
{
	// update each canvas' transform information
	auto vp_list = nse.system<nsrender_system>()->viewports();
	auto vp_iter = vp_list->begin();
	while (vp_iter != vp_list->end())
	{
		// sort all the entities in each canvas
		for (uint32 i = 0; i < vp_iter->vp->ui_canvases.size(); ++i)
		{
			nsui_canvas_comp * uicc = vp_iter->vp->ui_canvases[i]->get<nsui_canvas_comp>();
			uicc->m_ordered_ents.resize(0);
			auto ents = uicc->entities_in_canvas();
			if (ents == nullptr)
				continue;
	
			auto ui_iter = ents->begin();
			while (ui_iter != ents->end())
			{
				nsui_comp * uic = (*ui_iter)->get<nsui_comp>();
				nsrect_tform_comp * tuic = (*ui_iter)->get<nsrect_tform_comp>();
				if (uic != nullptr && uic->show)
					uicc->m_ordered_ents.push_back(*ui_iter);
				++ui_iter;
			}
			_sort_ents(uicc);
		}

		for (uint32 i = 0; i < vp_iter->vp->ui_canvases.size(); ++i)
		{
			const ivec2 & sz = nse.video_driver<nsopengl_driver>()->default_target()->size();
			fvec2 vp_size = vp_iter->vp->normalized_bounds.zw() - vp_iter->vp->normalized_bounds.xy();
			fvec2 fsz(sz.x,sz.y);
			nsentity * canvas = vp_iter->vp->ui_canvases[i];
			nsui_canvas_comp * uicc = canvas->get<nsui_canvas_comp>();
			uicc->update_rects(vp_size % fsz);

			auto rect_comps = uicc->entities_in_canvas();
			if (rect_comps != nullptr)
			{
				auto iter = rect_comps->begin();
				while (iter != rect_comps->end())
				{
					nsui_button_comp * uib = (*iter)->get<nsui_button_comp>();
					nsui_comp * uic = (*iter)->get<nsui_comp>();
					if (uib != nullptr && uic != nullptr)
					{
						nsmaterial * mat = get_resource<nsmaterial>(uic->mat_id);
						if (mat == nullptr)
						{
							++iter;
							continue;
						}
						
						if (uib->is_hover)
						{
							uint32 index = 0;
							if (uib->is_pressed)
								index = 1;
							
							switch (uib->change_button_using)
							{
							  case(tex_coord_rect):								  
								  if (uib->m_mat_norm_tex_rect == fvec4(-1.0f))
								  {
									  uib->m_mat_norm_tex_rect = mat->map_tex_coord_rect(nsmaterial::diffuse);
								  }
								  mat->set_map_tex_coord_rect(nsmaterial::diffuse, uib->tex_coord_rects[index]);
								  break;
							  case(change_color):
								  if (uib->m_mat_norm_color == fvec4(-1.0f))
								  {
									  uib->m_mat_norm_color = mat->color();
									  uib->m_mat_color_mode = mat->color_mode();
									  mat->set_color_mode(true);
								  }
								  mat->set_color(uib->colors[index]);
								  break;
							  case(color_multiplier):
								  break;
							}
						}
						else
						{
							if (uib->m_mat_norm_color != fvec4(-1.0f))
							{
								mat->set_color(uib->m_mat_norm_color);
								mat->set_color_mode(uib->m_mat_color_mode);
								uib->m_mat_color_mode = false;
								uib->m_mat_norm_color = fvec4(-1.0f);
							}
							if(uib->m_mat_norm_tex_rect != fvec4(-1.0f))
							{
								mat->set_map_tex_coord_rect(nsmaterial::diffuse, uib->m_mat_norm_tex_rect);
								uib->m_mat_norm_tex_rect = fvec4(-1.0f);
							}
						}
					}
					++iter;
				}
			}
		}
		++vp_iter;
	}
}

void nsui_system::_sort_ents(nsui_canvas_comp * uicc)
{
	if (uicc->m_ordered_ents.size() <= 1)
		return;

	for (uint32 c = 0 ; c < (uicc->m_ordered_ents.size() - 1 ); ++c)
	{
		for (uint32 d = 0 ; d < uicc->m_ordered_ents.size() - c - 1; d++)
		{
			auto * uic = uicc->m_ordered_ents[d]->get<nsrect_tform_comp>()->canvas_info(uicc);
			auto * uic2 = uicc->m_ordered_ents[d+1]->get<nsrect_tform_comp>()->canvas_info(uicc);
			if (uic->layer > uic2->layer) /* For decreasing order use < */
			{
				nsentity * tmp = uicc->m_ordered_ents[d];
				uicc->m_ordered_ents[d]   = uicc->m_ordered_ents[d+1];
				uicc->m_ordered_ents[d+1] = tmp;
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
	if (nse.system<nsselection_system>()->selection_being_dragged())
		return true;
	
	nsrender::viewport * vp = nse.system<nsrender_system>()->current_viewport();
	if (vp == nullptr)
		return true;

	nsentity * cam = vp->camera;
	if (cam == NULL)
		return true;

	fvec4 vps(vp->bounds.x,vp->bounds.y,vp->bounds.z,vp->bounds.w);
	fvec2 vp_size = vps.zw() - vps.xy();

	fvec2 vpnsize = vp->normalized_bounds.zw() - vp->normalized_bounds.xy();
	fvec2 mpos((evnt->normalized_mpos - vp->normalized_bounds.xy()) / vpnsize);
	
	for (uint32 i = 0; i < vp->ui_canvases.size(); ++i)
	{
		nsui_canvas_comp * uicc = vp->ui_canvases[i]->get<nsui_canvas_comp>();
		for (uint i = 0; i < uicc->m_ordered_ents.size(); ++i)
		{
			nsrect_tform_comp * rtc = uicc->m_ordered_ents[i]->get<nsrect_tform_comp>();
			nsui_button_comp * uibtn = rtc->owner()->get<nsui_button_comp>();

			if (uibtn == nullptr)
				continue;

            fvec2 scale = rtc->content_world_scale(uicc);
			fvec2 outer_pos = (vp_size) % scale + vps.xy();
			fmat3 tform = rtc->content_transform(uicc);
			
			fvec2 pos[4];
			pos[0] = (tform * fvec3(0.0f,0.0f,1.0f)).xy();
			pos[1] = (tform * fvec3(0.0f,outer_pos.y,1.0f)).xy();
			pos[2] = (tform * fvec3(outer_pos.x, 0.0f,1.0f)).xy();
			pos[3] = (tform * fvec3(outer_pos,1.0f)).xy();
			mpos %= vp_size;
            if (point_in_rect(mpos, pos))
            {
				uibtn->is_hover = true;
            }
			else
			{
				uibtn->is_hover = false;
			}
		}
	}
	return true;
}

bool nsui_system::_handle_mouse_press(nsaction_event * evnt)
{
	nsrender::viewport * vp = nse.system<nsrender_system>()->current_viewport();
	if (vp == nullptr)
		return true;

	nsentity * cam = vp->camera;
	if (cam == NULL)
		return true;

	fvec4 vps(vp->bounds.x,vp->bounds.y,vp->bounds.z,vp->bounds.w);
	fvec2 vp_size = vps.zw() - vps.xy();

	fvec2 vpnsize = vp->normalized_bounds.zw() - vp->normalized_bounds.xy();
	fvec2 mpos((evnt->norm_mpos - vp->normalized_bounds.xy()) / vpnsize);

	m_pressed_button = nullptr;
	
	for (uint32 i = 0; i < vp->ui_canvases.size(); ++i)
	{
		nsui_canvas_comp * uicc = vp->ui_canvases[i]->get<nsui_canvas_comp>();
		for (uint i = 0; i < uicc->m_ordered_ents.size(); ++i)
		{
			nsrect_tform_comp * rtc = uicc->m_ordered_ents[i]->get<nsrect_tform_comp>();
			nsui_button_comp * uibtn = rtc->owner()->get<nsui_button_comp>();

            fvec2 scale = rtc->content_world_scale(uicc);
			fvec2 outer_pos = (vp_size) % scale + vps.xy();
			fmat3 tform = rtc->content_transform(uicc);			
			fvec2 pos[4];
			pos[0] = (tform * fvec3(0.0f,0.0f,1.0f)).xy();
			pos[1] = (tform * fvec3(0.0f,outer_pos.y,1.0f)).xy();
			pos[2] = (tform * fvec3(outer_pos.x, 0.0f,1.0f)).xy();
			pos[3] = (tform * fvec3(outer_pos,1.0f)).xy();
			mpos %= vp_size;
            if (point_in_rect(mpos, pos))
            {
				m_focused_ui_ent = uicc->m_ordered_ents[i];
				if (uibtn != nullptr)
				{
					uibtn->is_pressed = true;
					uibtn->clicked();
					m_pressed_button = uibtn;
				}
            }
		}
	}
	return true;
}

bool nsui_system::_handle_mouse_release(nsaction_event * evnt)
{
	nsrender::viewport * vp = nse.system<nsrender_system>()->current_viewport();
	if (vp == nullptr)
		return true;

	nsentity * cam = vp->camera;
	if (cam == NULL)
		return true;

	fvec4 vps(vp->bounds.x,vp->bounds.y,vp->bounds.z,vp->bounds.w);
	fvec2 vp_size = vps.zw() - vps.xy();

	fvec2 vpnsize = vp->normalized_bounds.zw() - vp->normalized_bounds.xy();
	fvec2 mpos((evnt->norm_mpos - vp->normalized_bounds.xy()) / vpnsize);
	
	for (uint32 i = 0; i < vp->ui_canvases.size(); ++i)
	{
		nsui_canvas_comp * uicc = vp->ui_canvases[i]->get<nsui_canvas_comp>();
		for (uint i = 0; i < uicc->m_ordered_ents.size(); ++i)
		{
			nsrect_tform_comp * rtc = uicc->m_ordered_ents[i]->get<nsrect_tform_comp>();
			nsui_button_comp * uibtn = rtc->owner()->get<nsui_button_comp>();

			if (uibtn == nullptr)
				continue;

            fvec2 scale = rtc->content_world_scale(uicc);
			fvec2 outer_pos = (vp_size) % scale + vps.xy();
			fmat3 tform = rtc->content_transform(uicc);
			
			fvec2 pos[4];
			pos[0] = (tform * fvec3(0.0f,0.0f,1.0f)).xy();
			pos[1] = (tform * fvec3(0.0f,outer_pos.y,1.0f)).xy();
			pos[2] = (tform * fvec3(outer_pos.x, 0.0f,1.0f)).xy();
			pos[3] = (tform * fvec3(outer_pos,1.0f)).xy();
			mpos %= vp_size;
            if (point_in_rect(mpos, pos) && m_pressed_button == uibtn)
            {
				uibtn->pressed();
            }
			uibtn->is_pressed = false;
		}
	}
	m_pressed_button = nullptr;
	return true;
}

bool nsui_system::mpos_over_element(const fvec2 & norm_mpos, nsrender::viewport * vp)
{
	nsentity * cam = vp->camera;
	if (cam == NULL)
		return true;

	fvec4 vps(vp->bounds.x,vp->bounds.y,vp->bounds.z,vp->bounds.w);
	fvec2 vp_size = vps.zw() - vps.xy();

	fvec2 vpnsize = vp->normalized_bounds.zw() - vp->normalized_bounds.xy();
	fvec2 mpos((norm_mpos - vp->normalized_bounds.xy()) / vpnsize);
	
	for (uint32 i = 0; i < vp->ui_canvases.size(); ++i)
	{
		nsui_canvas_comp * uicc = vp->ui_canvases[i]->get<nsui_canvas_comp>();
		for (uint i = 0; i < uicc->m_ordered_ents.size(); ++i)
		{
			nsrect_tform_comp * rtc = uicc->m_ordered_ents[i]->get<nsrect_tform_comp>();
			nsui_button_comp * uibtn = rtc->owner()->get<nsui_button_comp>();

			if (uibtn == nullptr)
				continue;

            fvec2 scale = rtc->content_world_scale(uicc);
			fvec2 outer_pos = (vp_size) % scale + vps.xy();
			fmat3 tform = rtc->content_transform(uicc);
			
			fvec2 pos[4];
			pos[0] = (tform * fvec3(0.0f,0.0f,1.0f)).xy();
			pos[1] = (tform * fvec3(0.0f,outer_pos.y,1.0f)).xy();
			pos[2] = (tform * fvec3(outer_pos.x, 0.0f,1.0f)).xy();
			pos[3] = (tform * fvec3(outer_pos,1.0f)).xy();
			mpos %= vp_size;
            if (point_in_rect(mpos, pos))
            {
				return true;
            }
		}
	}
	return false;
}

void nsui_system::setup_input_map(nsinput_map * imap, const nsstring & global_imap_ctxt_name)
{
	imap_id = imap->full_id();
	imap->create_context(UI_IMAP_TEXT_EDIT_CTXT);
	
    nsinput_map::trigger ui_mouse_press(
        UI_MOUSE_PRESS,
        nsinput_map::t_pressed);
    imap->add_mouse_trigger(global_imap_ctxt_name, nsinput_map::left_button,ui_mouse_press);

    nsinput_map::trigger ui_mouse_release(
        UI_MOUSE_RELEASE,
        nsinput_map::t_released);
    imap->add_mouse_trigger(global_imap_ctxt_name, nsinput_map::left_button,ui_mouse_release);

    nsinput_map::trigger ui_key_press(
        UI_KEY_PRESS,
        nsinput_map::t_pressed);
    imap->add_key_trigger(UI_IMAP_TEXT_EDIT_CTXT, nsinput_map::key_any, ui_key_press);

    nsinput_map::trigger ui_shifted_key_pressl(
        UI_SHIFTED_KEY_PRESS,
        nsinput_map::t_pressed);
	ui_shifted_key_pressl.add_key_mod(nsinput_map::key_lshift);
    imap->add_key_trigger(UI_IMAP_TEXT_EDIT_CTXT, nsinput_map::key_any, ui_shifted_key_pressl);

	nsinput_map::trigger ui_shifted_key_pressr(
        UI_SHIFTED_KEY_PRESS,
        nsinput_map::t_pressed);
	ui_shifted_key_pressr.add_key_mod(nsinput_map::key_rshift);
    imap->add_key_trigger(UI_IMAP_TEXT_EDIT_CTXT, nsinput_map::key_any, ui_shifted_key_pressr);
}

bool nsui_system::_handle_key_press(nsaction_event * evnt)
{
	return true;
}

bool nsui_system::_handle_shifted_key_press(nsaction_event * evnt)
{
	return true;
}
