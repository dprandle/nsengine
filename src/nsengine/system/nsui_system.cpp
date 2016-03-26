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
	
}

void nsui_system::release()
{
	
}

void nsui_system::update()
{
	m_ui_draw_calls.resize(0);
	drawcall_queue * dc_dq = nse.system<nsrender_system>()->queue(UI_RENDER_QUEUE);
	dc_dq->resize(0);

	// go through and add only parent widgets that are showing
	m_ordered_ents.resize(0);
	auto ents = nsep.active()->manager<nsentity_manager>()->entities<nsui_comp>();
	auto ui_iter = ents.begin();
	while (ui_iter != ents.end())
	{
		nsui_comp * uic = (*ui_iter)->get<nsui_comp>();
		if (uic->show && uic->parent() == uivec2(0))
			m_ordered_ents.push_back(*ui_iter);
		++ui_iter;
	}
	_sort_ents();

	for (uint32 i = 0; i < m_ordered_ents.size(); ++i)
	{
		nsentity * cur_ent = m_ordered_ents[i];
		nsui_comp * uic = cur_ent->get<nsui_comp>();

		m_ui_draw_calls.resize(m_ui_draw_calls.size()+1);
		ui_draw_call * uidc = &m_ui_draw_calls[m_ui_draw_calls.size()-1];

		uidc->mat = get_resource<nsmaterial>(uic->content_properties.mat_id);
		uidc->pad_mat = get_resource<nsmaterial>(uic->outer_properties.mat_id);
		uidc->shdr = get_resource<nsshader>(uic->content_shader_id);
		uidc->border_shader = get_resource<nsshader>(uic->border_shader_id);

		uidc->entity_id = uivec3(cur_ent->full_id(),0);
		uidc->center = uic->center_npos;
		uidc->content_size = uic->content_properties.nsize;
		uidc->border_color = uic->outer_properties.border_color;

		uidc->content_tex_coord_rect = uic->content_properties.tex_coords_rect;
		uidc->padding_tex_coord_rect = uic->outer_properties.padding_tex_coords_rect;
		
		ivec2 screen_size = nse.video_driver<nsopengl_driver>()->default_target()->size();
		uidc->padding = fvec4(
			uic->outer_properties.padding.x,
			uic->outer_properties.padding.y,
			uic->outer_properties.padding.z,
			uic->outer_properties.padding.w) / fvec4(screen_size.w,screen_size.w,screen_size.h,screen_size.h);
		
		uidc->border = fvec4(
			uic->outer_properties.border.x,
			uic->outer_properties.border.y,
			uic->outer_properties.border.z,
			uic->outer_properties.border.w) / fvec4(screen_size.w,screen_size.w,screen_size.h,screen_size.h);

		fvec4 nmargins = fvec4(
			uic->outer_properties.margin.x,
			uic->outer_properties.margin.y,
			uic->outer_properties.margin.z,
			uic->outer_properties.margin.w) / fvec4(screen_size.w,screen_size.w,screen_size.h,screen_size.h);

		dc_dq->push_back(uidc);
	}
}

void nsui_system::_sort_ents()
{
	if (m_ordered_ents.size() <= 1)
		return;
	
	for (uint32 c = 0 ; c < (m_ordered_ents.size() - 1 ); ++c)
	{
		for (uint32 d = 0 ; d < m_ordered_ents.size() - c - 1; d++)
		{
			nsui_comp * uic = m_ordered_ents[d]->get<nsui_comp>();
			nsui_comp * uic2 = m_ordered_ents[d+1]->get<nsui_comp>();
			if (uic->layer > uic2->layer) /* For decreasing order use < */
			{
				nsentity * tmp = m_ordered_ents[d];
				m_ordered_ents[d]   = m_ordered_ents[d+1];
				m_ordered_ents[d+1] = tmp;
			}
		}
	}
}

void nsui_system::set_ui_shader(nsui_shader * shdr)
{
	main_ui_shader = shdr;
}

int32 nsui_system::update_priority()
{
	return UI_SYS_UPDATE_PR;
}
