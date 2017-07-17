/*!
\file nsui_system.cpp

\brief Header file for nsui_system class

This file contains all of the neccessary declarations for the nsui_system class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#include <asset/nsinput_map.h>
#include <system/nsselection_system.h>
#include <system/nsui_system.h>
#include <nsentity.h>
#include <asset/nsplugin_manager.h>
#include <component/nsui_comp.h>
#include <asset/nsshader.h>
#include <asset/nsmaterial.h>
#include <opengl/nsgl_driver.h>
#include <opengl/nsgl_framebuffer.h>
#include <nstimer.h>
#include <component/nsrect_tform_comp.h>
#include <component/nsui_canvas_comp.h>
#include <component/nsui_button_comp.h>
#include <asset/nsfont.h>
#include <system/nsinput_system.h>

nsui_system::nsui_system():
	nssystem(type_to_hash(nsui_system)),
	m_pressed_button(nullptr),
	m_focused_ui_ent(nullptr)
{}

nsui_system::~nsui_system()
{}

void nsui_system::init()
{
	register_handler(nsui_system::_handle_mouse_event);
	register_handler(nsui_system::_handle_key_press);
	
	// handle key presses for ui elements that allow text editing input
	register_action_handler(nsui_system::_handle_mouse_press, UI_MOUSE_PRESS);
	register_action_handler(nsui_system::_handle_mouse_release, UI_MOUSE_RELEASE);
}

void nsui_system::release()
{}

void nsui_system::update()
{
	// turn off input event dispatching if text is being edited

    if (m_focused_ui_ent != nullptr && m_focused_ui_ent->has<nsui_text_input_comp>())
        nse.system<nsinput_system>()->key_dispatch_enabled = false;
    else
        nse.system<nsinput_system>()->key_dispatch_enabled = true;

	// update each canvas' transform information
	std::list<vp_node> * vp_list = &nse.video_driver()->current_context()->vp_list;
	auto vp_iter = vp_list->begin();
	while (vp_iter != vp_list->end())
	{
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
				nsui_material_comp * uimat = (*ui_iter)->get<nsui_material_comp>();
				nsrect_tform_comp * tuic = (*ui_iter)->get<nsrect_tform_comp>();
				nsui_text_comp * uitxt = (*ui_iter)->get<nsui_text_comp>();
				nsui_text_input_comp * uitxt_input = (*ui_iter)->get<nsui_text_input_comp>();
				nsui_button_comp * uib = (*ui_iter)->get<nsui_button_comp>();
				uicc->m_ordered_ents.push_back(*ui_iter);
				
				// Switch the button material based on the state of the button
				if (uib != nullptr)
				{
					tex_map_info ti_mat, ti_border, ti_text;
					uint32 index;
					if (!uib->is_enabled)
						index = 3;
					else if (uib->is_pressed)
						index = 2;
					else if (uib->is_hover)
						index = 1;
					else
						index = 0;

					button_state * bs;
					if (uib->toggle_enabled && uib->is_toggled)
						bs = &uib->toggled_button_states[index];
					else
						bs = &uib->button_states[index];
					
					ti_mat.color_add = bs->mat_color_add;
					ti_mat.color_mult = bs->mat_color_mult;
					ti_mat.coord_rect = bs->mat_tex_coord_rect;

					ti_border.color_add = bs->border_color_add;
					ti_border.color_mult = bs->border_color_mult;
					ti_border.coord_rect = bs->border_tex_coord_rect;

					ti_text.color_add = bs->text_color_add;
					ti_text.color_mult = bs->text_color_mult;
					ti_text.coord_rect = bs->text_tex_coord_rect;
					if (uimat != nullptr)
					{
						nsmaterial * mmat = get_asset<nsmaterial>(uimat->mat_id);
						nsmaterial * border_mat = get_asset<nsmaterial>(uimat->border_mat_id);

						uimat->border_size = bs->border_size[index];
						uimat->top_border_radius = bs->top_border_radius;
						uimat->bottom_border_radius = bs->bottom_border_radius;
						
						if (mmat != nullptr)
						{
							ti_mat.tex_id = mmat->map_tex_id(nsmaterial::diffuse);
							if (mmat->set_map_tex_info(nsmaterial::diffuse, ti_mat))
								mmat->set_color(bs->mat_color);
							else
								mmat->set_color(bs->mat_color % bs->mat_color_mult + bs->mat_color_add);
						}
						if (border_mat != nullptr)
						{
							ti_border.tex_id = border_mat->map_tex_id(nsmaterial::diffuse);
							if (border_mat->set_map_tex_info(nsmaterial::diffuse, ti_border))
								border_mat->set_color(bs->border_color);
							else
								border_mat->set_color(bs->border_color % bs->border_color_mult + bs->border_color_add);
						}
					}
					if (uitxt != nullptr)
					{
						nsfont * fnt = get_asset<nsfont>(uitxt->font_id);
						if (fnt != nullptr)
						{
							nsmaterial * fnt_mat = get_asset<nsmaterial>(uitxt->font_material_id);
							if (fnt_mat != nullptr)
							{
								ti_border.tex_id = fnt_mat->map_tex_id(nsmaterial::diffuse);
								if (fnt_mat->set_map_tex_info(nsmaterial::diffuse, ti_text))
									fnt_mat->set_color(bs->text_color);
								else
									fnt_mat->set_color(bs->text_color % bs->text_color_mult + bs->text_color_add);
							}
						}
					}
				}

				// determine the size of each line of text (also how many lines there are)
				if (uitxt != nullptr)
				{
					uitxt->text_line_sizes.resize(1);
					uitxt->text_line_sizes[0] = 0;
					uint32 cur_line = 0;
					for (uint32 i = 0; i < uitxt->text.size(); ++i)
					{
						if (uitxt->text[i] == '\n')
						{
							uitxt->text_line_sizes.push_back(0);
							++cur_line;
						}
						else
							++uitxt->text_line_sizes[cur_line];
					}

					if (uitxt_input != nullptr)
					{
						if (uitxt_input->cursor_offset.y > cur_line)
							uitxt_input->cursor_offset.y = cur_line;
						if (uitxt_input->cursor_offset.x > uitxt->text_line_sizes[uitxt_input->cursor_offset.y])
							uitxt_input->cursor_offset.x = uitxt->text_line_sizes[uitxt_input->cursor_offset.y];
					}
				}
				++ui_iter;
			}

			// Do the update stuff and then sort
			const ivec2 & sz = nse.video_driver()->current_context()->window_size();
			fvec2 vp_size = vp_iter->vp->normalized_bounds.zw() - vp_iter->vp->normalized_bounds.xy();
			fvec2 fsz(sz.x,sz.y);
			nsentity * canvas = vp_iter->vp->ui_canvases[i];
			uicc->update_rects(vp_size % fsz);
			_sort_ents(uicc);

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
			auto * uic1 = uicc->m_ordered_ents[d]->get<nsrect_tform_comp>()->canvas_info(uicc);
			auto * uic2 = uicc->m_ordered_ents[d+1]->get<nsrect_tform_comp>()->canvas_info(uicc);
			if (uic1->layer > uic2->layer) /* For decreasing order use < */
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

nsentity * nsui_system::focused_ui_element()
{
	return m_focused_ui_ent;
}

bool nsui_system::_handle_mouse_event(nsmouse_move_event * evnt)
{
	if (nse.system<nsselection_system>()->selection_being_dragged())
		return true;
	
	viewport * vp = nse.video_driver()->current_context()->focused_vp;
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
        for (uint32 i = 0; i < uicc->m_ordered_ents.size(); ++i)
		{
			nsrect_tform_comp * rtc = uicc->m_ordered_ents[i]->get<nsrect_tform_comp>();
			nsui_button_comp * uibtn = rtc->owner()->get<nsui_button_comp>();

			if (uibtn == nullptr || !uibtn->is_enabled)
				continue;

            fvec2 scale = rtc->content_world_scale(uicc);
			fvec2 outer_pos = (vp_size) % scale + vps.xy();
			fmat3 tform = rtc->content_transform(uicc);
			
			fvec2 pos[4];
			pos[0] = (tform * fvec3(0.0f,0.0f,1.0f)).xy();
			pos[1] = (tform * fvec3(0.0f,outer_pos.y,1.0f)).xy();
			pos[2] = (tform * fvec3(outer_pos.x, 0.0f,1.0f)).xy();
			pos[3] = (tform * fvec3(outer_pos,1.0f)).xy();
            if (point_in_rect(mpos % vp_size, pos))
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
	viewport * vp = nse.video_driver()->current_context()->focused_vp;
	if (vp == nullptr)
		return true;

	nsentity * cam = vp->camera;
	if (cam == NULL)
		return true;

	fvec4 vps(vp->bounds.x,vp->bounds.y,vp->bounds.z,vp->bounds.w);
	fvec2 vp_size = vps.zw() - vps.xy();

	fvec2 vpnsize = vp->normalized_bounds.zw() - vp->normalized_bounds.xy();
	fvec2 mpos((evnt->norm_mpos - vp->normalized_bounds.xy()) / vpnsize);
	
	std::cout << "event norm_mpos: " << evnt->norm_mpos.to_string() << std::endl;
	std::cout << "vpnsize: " << vpnsize.to_string() << std::endl;

	m_pressed_button = nullptr;
	m_focused_ui_ent = nullptr;	
	for (uint32 i = 0; i < vp->ui_canvases.size(); ++i)
	{
		nsui_canvas_comp * uicc = vp->ui_canvases[i]->get<nsui_canvas_comp>();
        for (uint32 i = 0; i < uicc->m_ordered_ents.size(); ++i)
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
            if (point_in_rect(mpos % vp_size, pos))
            {
				m_focused_ui_ent = uicc->m_ordered_ents[i];
				if (uibtn != nullptr && uibtn->is_enabled)
				{
					uibtn->is_pressed = true;
					emit_sig uibtn->clicked();
					m_pressed_button = uibtn;
				}
            }
		}
	}
	return true;
}

bool nsui_system::_handle_mouse_release(nsaction_event * evnt)
{
	viewport * vp = nse.video_driver()->current_context()->focused_vp;
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
        for (uint32 i = 0; i < uicc->m_ordered_ents.size(); ++i)
		{
			nsrect_tform_comp * rtc = uicc->m_ordered_ents[i]->get<nsrect_tform_comp>();
			nsui_button_comp * uibtn = rtc->owner()->get<nsui_button_comp>();

			if (uibtn == nullptr || !uibtn->is_enabled)
				continue;

            fvec2 scale = rtc->content_world_scale(uicc);
			fvec2 outer_pos = (vp_size) % scale + vps.xy();
			fmat3 tform = rtc->content_transform(uicc);
			
			fvec2 pos[4];
			pos[0] = (tform * fvec3(0.0f,0.0f,1.0f)).xy();
			pos[1] = (tform * fvec3(0.0f,outer_pos.y,1.0f)).xy();
			pos[2] = (tform * fvec3(outer_pos.x, 0.0f,1.0f)).xy();
			pos[3] = (tform * fvec3(outer_pos,1.0f)).xy();
            if (point_in_rect(mpos % vp_size, pos) && m_pressed_button == uibtn)
            {
				emit_sig uibtn->pressed();
				if (uibtn->toggle_enabled)
				{
					uibtn->is_toggled = !uibtn->is_toggled;
					emit_sig uibtn->toggled(uibtn->is_toggled);
				}
            }
			uibtn->is_pressed = false;
		}
	}
	m_pressed_button = nullptr;
	return true;
}

bool nsui_system::mpos_over_element(const fvec2 & norm_mpos, viewport * vp)
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
        for (uint32 i = 0; i < uicc->m_ordered_ents.size(); ++i)
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
    nsinput_map::trigger ui_mouse_press(
        UI_MOUSE_PRESS,
        nsinput_map::t_pressed);
    imap->add_mouse_trigger(global_imap_ctxt_name, nsinput_map::left_button,ui_mouse_press);

    nsinput_map::trigger ui_mouse_release(
        UI_MOUSE_RELEASE,
        nsinput_map::t_released);
    imap->add_mouse_trigger(global_imap_ctxt_name, nsinput_map::left_button,ui_mouse_release);
}

bool nsui_system::_handle_key_press(nskey_event * evnt)
{
	if (m_focused_ui_ent == nullptr)
		return true;
	
	nsui_text_input_comp * uitcomp = m_focused_ui_ent->get<nsui_text_input_comp>();
	nsui_text_comp * uitxt = m_focused_ui_ent->get<nsui_text_comp>();
	if (uitcomp == nullptr || uitxt == nullptr)
		return true;

	if (!evnt->pressed)
		return true;
	
	if (evnt->key == nsinput_map::key_left)
	{
		uitcomp->cursor_offset.x -= 1;
	}
	else if (evnt->key == nsinput_map::key_right)
	{
		uitcomp->cursor_offset.x += 1;
	}
	else if (evnt->key == nsinput_map::key_up)
	{
		uitcomp->cursor_offset.y -= 1;
	}
	else if (evnt->key == nsinput_map::key_down)
	{
		uitcomp->cursor_offset.y += 1;
	}
	else if (evnt->key == nsinput_map::key_backspace)
	{
		int32 index = 0;
		for (uint32 i = 0; i < uitcomp->cursor_offset.y; ++i)
			index += uitxt->text_line_sizes[i] + 1; // newline char
		index += uitcomp->cursor_offset.x - 1;
		if (index < 0)
			return true;
		uitxt->text.erase(uitxt->text.begin() + index);
		if (uitcomp->cursor_offset.x == 0)
		{
			uitcomp->cursor_offset.y -= 1;
			uitcomp->cursor_offset.x = uitxt->text_line_sizes[uitcomp->cursor_offset.y]+1;
		}
		uitcomp->cursor_offset.x -= 1;
	}
	else
	{
		bool cap = nse.system<nsinput_system>()->caps_locked() ||
			nse.system<nsinput_system>()->is_key_pressed(nsinput_map::key_lshift) ||
			nse.system<nsinput_system>()->is_key_pressed(nsinput_map::key_rshift);
		char letter = input_key_to_ascii(evnt->key, cap);
		if (letter != -1)
		{
			uint32 index = 0;
			for (uint32 i = 0; i < uitcomp->cursor_offset.y; ++i)
				index += uitxt->text_line_sizes[i] + 1; // newline char
			index += uitcomp->cursor_offset.x;
			uitxt->text.insert(uitxt->text.begin()+index, letter);
			if (letter == '\n')
			{
				uitcomp->cursor_offset.y += 1;
				uitcomp->cursor_offset.x = 0;
			}
			else
				uitcomp->cursor_offset.x += 1;
		}
	}
	return true;
}
