/*!
\file nsbuildsystem.cpp

\brief Definition file for nsbuild_system class

This file contains all of the neccessary definitions for the nsbuild_system class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#include <iostream>
#include <nsmath.h>
#include <nsbuild_system.h>
#include <nsevent_dispatcher.h>
#include <nstile_brush_comp.h>
#include <nsevent.h>
#include <nsscene.h>
#include <nstile_comp.h>
#include <nstile_grid.h>
#include <nssel_comp.h>
#include <nsoccupy_comp.h>
#include <nsrender_system.h>
#include <nsselection_system.h>
#include <nsrender_comp.h>
#include <nsinput_map_manager.h>
#include <nsentity_manager.h>
#include <nsplugin.h>
#include <nsinput_system.h>
#include <nsentity.h>
#include <nslight_comp.h>
#include <nscam_comp.h>
#include <nscamera_system.h>

nsbuild_system::nsbuild_system():
m_enabled(false),
m_tile_brush(NULL),
m_object_brush(NULL),
m_tile_build_ent(NULL),
m_object_build_ent(NULL),
m_mirror_brush(NULL),
m_layer(0),
m_current_mode(build_mode),
m_current_brush_type(brush_none),
m_tile_brush_center_tform_id(0),
m_overwrite(false),
m_mirror_mode(false),
m_painting(false),
m_erasing(false),
m_stamp_mode(false),
nssystem()
{
	
}

nsbuild_system::~nsbuild_system()
{

}

void nsbuild_system::change_layer(const int32 & pAmount)
{
	m_layer += pAmount;
}

void nsbuild_system::enable(const bool & pEnable)
{
	nsscene * scene = nse.current_scene();
	if (scene == NULL)
		return;
	
	if (pEnable && !m_enabled)
	{
		m_enabled = pEnable;
		nse.system<nsinput_system>()->push_context(BUILD_MODE_CTXT);
		nse.system<nsselection_system>()->clear();

		nse.event_dispatch()->push<nsstate_event>(hash_id(NSSEL_MOVE_TOGGLE), true);

		if (m_current_brush_type == brush_tile)
		{
			if (m_tile_brush == NULL)
				return;

			if (m_mirror_mode)
			{
				m_mirror_brush = nse.core()->create<nsentity>(ENT_MIRROR_BRUSH);
				// copy the tile brush and assign it to the mirror brush
				m_mirror_brush->copy_all(m_tile_brush);
			}

			nstile_brush_comp * brushComp = m_tile_brush->get<nstile_brush_comp>();

			nssel_comp * selComp = m_tile_brush->get<nssel_comp>();
			if (selComp == NULL || brushComp == NULL)
				return;

            bool tmp = nse.system<nsselection_system>()->mirror_selection();
            nse.system<nsselection_system>()->enable_mirror_selection(false);

			auto brushIter = brushComp->begin();
			while (brushIter != brushComp->end())
			{
				fvec3 pos = nstile_grid::world(ivec3(brushIter->x, brushIter->y, m_layer));
				uint32 tFormID = scene->add(m_tile_brush, pos);	
				nse.system<nsselection_system>()->add(m_tile_brush, tFormID);
				nse.system<nsselection_system>()->set_focus_entity(uivec3(m_tile_brush->full_id(),tFormID));					
				if (m_mirror_mode)
				{
					fvec3 mirrorPos = m_mirror_center*2.0f - pos;
					mirrorPos.z = pos.z;
					uint32 mirror_tform_id = scene->add(m_mirror_brush, mirrorPos);
					nse.system<nsselection_system>()->add(m_mirror_brush, mirror_tform_id);
					m_mirror_brush->get<nstform_comp>()->set_hidden_state(nstform_comp::hide_all, true, mirror_tform_id);
				}

				if (*brushIter == ivec2())
					m_tile_brush_center_tform_id = tFormID;

				m_tile_brush->get<nstform_comp>()->set_hidden_state(nstform_comp::hide_all, true, tFormID);
				++brushIter;
			}

			//nse.system<nsselection_system>()->set_focus_entity(
			//	uivec3(m_tile_brush->full_id(),m_tile_brush_center_tform_id)
			//	);
			
			selComp->set_selected(true);

            nse.system<nsselection_system>()->enable_mirror_selection(tmp);
			to_cursor();
		}
		else if (m_current_brush_type == brush_object)
		{
			
			if (m_object_brush == NULL)
				return;

			if (m_object_build_ent == NULL)
				return;
			
            m_object_brush->copy(m_object_build_ent->get<nsrender_comp>());
			m_object_brush->copy(m_object_build_ent->get<nslight_comp>());
			fvec3 pos = nstile_grid::world(ivec3(0,0,m_layer));
			
			if (m_mirror_mode)
			{
				m_mirror_brush = nse.core()->create<nsentity>(ENT_MIRROR_BRUSH);
				// copy the tile brush and assign it to the mirror brush
				m_mirror_brush->copy_all(m_object_brush);
			}

			nssel_comp * selComp = m_object_brush->get<nssel_comp>();
			uint32 tFormID = scene->add(m_object_brush, pos);
			nse.system<nsselection_system>()->add(m_object_brush, tFormID);
			selComp->set_selected(true);

			nse.system<nsselection_system>()->set_focus_entity(
				uivec3(m_object_brush->full_id(),tFormID)
				);

			m_object_brush->get<nstform_comp>()->set_hidden_state(nstform_comp::hide_all, true, tFormID);
			
			bool tmp = nse.system<nsselection_system>()->mirror_selection();				
			nse.system<nsselection_system>()->enable_mirror_selection(false);				
				
			if (m_mirror_mode)
			{
				fvec3 mirrorPos = m_mirror_center*2.0f - pos;
				mirrorPos.z = pos.z;
				uint32 mirror_tform_id = scene->add(m_mirror_brush, mirrorPos);
				nse.system<nsselection_system>()->add(m_mirror_brush, mirror_tform_id);
				m_mirror_brush->get<nstform_comp>()->set_hidden_state(nstform_comp::hide_all, true, mirror_tform_id);
			}

            m_object_brush->copy(m_object_build_ent->get<nsoccupy_comp>());

			if (m_mirror_mode)
				m_mirror_brush->copy(m_object_build_ent->get<nsoccupy_comp>());

			nse.system<nsselection_system>()->enable_mirror_selection(tmp);
			to_cursor();
		}
		else
			return;
	}
	else if (!pEnable && m_enabled)
	{
		m_enabled = pEnable;
		nse.event_dispatch()->push<nsstate_event>(hash_id(NSSEL_MOVE_TOGGLE), false);
		nse.system<nsinput_system>()->pop_context();

		if (m_current_brush_type == brush_object && m_object_brush != NULL)
		{
			m_object_brush->del<nsoccupy_comp>();
			m_object_brush->del<nsrender_comp>();
			m_object_brush->del<nslight_comp>();
			scene->remove(m_object_brush);
		}

		if (m_current_brush_type == brush_tile && m_tile_brush != NULL)
		{
			scene->remove(m_tile_brush);
		}

		nse.system<nsselection_system>()->clear();
        nse.core()->destroy<nsentity>(ENT_MIRROR_BRUSH);
		m_mirror_brush = NULL;
	}
}

void nsbuild_system::enable_stamp_mode(bool enable)
{
	m_stamp_mode = enable;
}

void nsbuild_system::enable_overwrite(bool pEnable)
{
	m_overwrite = pEnable;
}

void nsbuild_system::enable_mirror(bool pEnable)
{
    bool state = m_enabled;
    if (state)
        toggle();
    m_mirror_mode = pEnable;
    if (state)
        toggle();
}

void nsbuild_system::erase()
{
	nsscene * scene = nse.current_scene();
	if (scene == NULL)
		return;

	if (m_current_brush_type == brush_tile)
	{
		if (m_tile_brush == NULL)
			return;

		nstile_brush_comp * brushComp = m_tile_brush->get<nstile_brush_comp>();
		if (brushComp == NULL)
			return;

		auto brushIter = brushComp->begin();
		while (brushIter != brushComp->end())
		{
			for (int32 i = 0; i < brushComp->height(); ++i)
			{
				fvec3 pos = m_tile_brush->get<nstform_comp>()->lpos(m_tile_brush_center_tform_id) + nstile_grid::world(ivec3(brushIter->x, brushIter->y, -i)); // add in height when get working
				scene->remove(pos);

				if (m_mirror_mode)
				{
					fvec3 new_pos = m_mirror_center*2.0f - pos;
					new_pos.z = pos.z;
					scene->remove(new_pos);
				}
			}
			++brushIter;
		}
	}
}

const fvec4 nsbuild_system::active_brush_color() const
{
	if (m_current_brush_type == brush_tile && m_tile_brush != NULL)
		return m_tile_brush->get<nssel_comp>()->default_color();
	else if (m_current_brush_type == brush_object && m_object_brush != NULL)
		return m_object_brush->get<nssel_comp>()->default_color();
	return fvec4();
}

nsentity * nsbuild_system::tile_build_ent()
{
	return m_tile_build_ent;
}

nsentity * nsbuild_system::object_build_ent()
{
	return m_object_build_ent;
}

const nsbuild_system::brush_t & nsbuild_system::brush_type()
{
	return m_current_brush_type;
}

const int32 & nsbuild_system::layer() const
{
	return m_layer;
}

const fvec3 & nsbuild_system::center() const
{
	return m_mirror_center;
}

const nsbuild_system::mode_t & nsbuild_system::mode() const
{
	return m_current_mode;
}

void nsbuild_system::init()
{
	m_object_brush = nse.core()->create<nsentity>(ENT_OBJECT_BRUSH);
	nssel_comp * sc = m_object_brush->create<nssel_comp>();
	
	sc->set_default_sel_color(fvec4(0.0f, 1.0f, 0.0f, 1.0f));
	sc->set_color(fvec4(0.0f, 1.0f, 0.0f, 1.0f));
	sc->set_mask_alpha(0.2f);
	sc->enable_draw(true);
	sc->enable_move(true);

	register_handler_func(this, &nsbuild_system::_handle_cam_change_event);
	register_handler_func(this, &nsbuild_system::_handle_state_event);
	register_handler_func(this, &nsbuild_system::_handle_action_event);
	
	add_trigger_hash(insert_entity, NSINSERT_ENTITY);
	add_trigger_hash(snap_brush_z, NSSNAP_BRUSH_Z);
	add_trigger_hash(initial_snap_brush_z, NSINITIAL_SNAP_BRUSH_Z);
	add_trigger_hash(toggle_build_erase_mode, NSTOGGLE_BUILD_ERASE_MODE);
	add_trigger_hash(toggle_build, NSTOGGLE_BUILD);
	add_trigger_hash(toggle_tile_build_mode, NSTOGGLE_TILE_BUILD_MODE);
	add_trigger_hash(select_move_toggle, NSSEL_MOVE_TOGGLE);
}

bool nsbuild_system::enabled() const
{
	return m_enabled;
}

bool nsbuild_system::overwrite() const
{
	return m_overwrite;
}

bool nsbuild_system::mirror() const
{
	return m_mirror_mode;
}

void nsbuild_system::to_cursor()
{
	nsscene * scene = nse.current_scene();
	if (scene == NULL)
		return;

	nsentity * camera = scene->camera();
	if (camera == NULL)
		return;

	nstform_comp * cam_tform = camera->get<nstform_comp>();
	nscam_comp * camc = camera->get<nscam_comp>();
	fvec2 cursor_pos = platform_normalized_mpos();

	nstform_comp * brush_tform = NULL;
	fvec3 original_pos;
	
	if (m_current_brush_type == brush_tile)
	{
		if (m_tile_brush == NULL)
			return;
		brush_tform = m_tile_brush->get<nstform_comp>();
		if (brush_tform == NULL)
			return;
		original_pos = brush_tform->lpos(m_tile_brush_center_tform_id);
	}
	else if (m_current_brush_type == brush_object)
	{
		if (m_object_brush == NULL)
			return;
		brush_tform = m_object_brush->get<nstform_comp>();
		if (brush_tform == NULL)
			return;
		original_pos = brush_tform->lpos();
	}
	else
		return;

	fvec4 screen_space = camc->proj_cam() * fvec4(original_pos, 1.0f);

	screen_space /= screen_space.w;
	screen_space.x = (2*cursor_pos.u-1);
	screen_space.y = (2*cursor_pos.v-1);

	fvec4 new_pos = camc->inv_proj_cam() * screen_space;
	
	new_pos /= new_pos.w;

	fvec3 fpos(new_pos.xyz());
	fvec3 castVec = fpos - cam_tform->wpos();
	castVec.normalize();

	fvec3 normal(0.0f,0.0f,-1.0f);
	float denom = normal * castVec;
	float depth = 0.0f;
	depth = (normal * (original_pos - fpos)) / denom;
	
	fpos += castVec*depth;
	fpos -= original_pos;
	
	for (uint32 i = 0; i < brush_tform->count(); ++i)
		brush_tform->translate(fpos, i);
}

nsentity * nsbuild_system::tile_brush()
{
	return m_tile_brush;
}

nsentity * nsbuild_system::object_brush()
{
	return m_object_brush;
}

void nsbuild_system::paint()
{
	nsscene * scene = nse.current_scene();
	if (scene == NULL)
		return;

	if (m_current_brush_type == brush_tile)
	{
		if (m_tile_build_ent == NULL)
			return;

		if (m_tile_brush == NULL)
			return;

		nstile_brush_comp * brushComp = m_tile_brush->get<nstile_brush_comp>();
		if (brushComp == NULL)
			return;

		auto brushIter = brushComp->begin();
		while (brushIter != brushComp->end())
		{
			for (int32 i = 0; i < brushComp->height(); ++i)
			{
				fvec3 pos = m_tile_brush->get<nstform_comp>()->wpos(m_tile_brush_center_tform_id) + nstile_grid::world(ivec3(brushIter->x, brushIter->y, -i)); // add in height when get working
				nstile_grid::snap(pos);
				
				if (m_overwrite)
				{
					scene->remove(pos);
					if (m_mirror_mode)
					{
						fvec3 new_pos = m_mirror_center*2.0f - pos;
						new_pos.z = pos.z;
						scene->remove(new_pos);
					}
				}

				uint32 tFormID = scene->add(m_tile_build_ent, pos);
				
				if (tFormID != -1)
				{
					if (m_mirror_mode)
					{
						fvec3 new_pos = m_mirror_center*2.0f - pos;
						new_pos.z = pos.z;
						uint32 tFormMID = scene->add(m_tile_build_ent, new_pos);
						if (tFormMID == -1)
						{
							scene->remove(m_tile_build_ent, tFormID);
							continue;
						}
					}
				}
			}
			++brushIter;
		}
	}
	else if (m_current_brush_type == brush_object)
	{
		if (m_object_brush == NULL || m_object_build_ent == NULL)
			return;

		fvec3 pos = m_object_brush->get<nstform_comp>()->wpos();
		nstile_grid::snap(pos);
		uint32 tFormID = scene->add(m_object_build_ent, pos);

		if (tFormID != -1)
		{
			if (m_mirror_mode)
			{
				fvec3 new_pos = m_mirror_center*2.0f - pos;
				new_pos.z = pos.z;
				uint32 tFormMID = scene->add(m_object_build_ent, new_pos);
				if (tFormMID == -1)
				{
					scene->remove(m_object_build_ent, tFormID);
					return;
				}
			}
		}
	}
	to_cursor();
}

void nsbuild_system::set_active_brush_color(const fvec4 & pColor)
{
	if (m_current_brush_type == brush_tile && m_tile_brush != NULL)
		m_tile_brush->get<nssel_comp>()->set_color(pColor);
	else if (m_current_brush_type == brush_object && m_object_brush != NULL)
		m_object_brush->get<nssel_comp>()->set_color(pColor);
	
	if (m_mirror_mode && m_mirror_brush != NULL)
		m_mirror_brush->get<nssel_comp>()->set_color(pColor);
}

void nsbuild_system::set_brush_type(const brush_t & brush_type_)
{
	if (m_current_mode == erase_mode)
		return;
	
	bool tmp = m_enabled;
	if (tmp)
		toggle();

	m_current_brush_type = brush_type_;
	switch (m_current_brush_type)
	{
	case (brush_none) :
		m_tile_build_ent = NULL;
		m_object_build_ent = NULL;
		break;
	case (brush_tile):
		if (m_tile_build_ent == NULL)
			return;
		if (!m_tile_build_ent->has<nstile_comp>())
			m_tile_build_ent = NULL;
		break;
	case (brush_object):
		if (m_object_build_ent == NULL)
			return;
		if (m_object_build_ent->has<nstile_comp>())
			m_object_build_ent = NULL;
		break;
	}

	if (tmp)
		toggle();
}

void nsbuild_system::set_object_brush(nsentity * pBrush)
{
	bool state = m_enabled;
	if (state)
		toggle();
	m_object_brush = pBrush;
	if (state)
		toggle();
}

void nsbuild_system::set_tile_brush(nsentity * pBrush)
{
	bool state = m_enabled;
	if (state)
		toggle();
	m_tile_brush = pBrush;
	if (state)
		toggle();
}

void nsbuild_system::set_tile_build_ent(nsentity * pBuildEnt)
{
	if (pBuildEnt == NULL || pBuildEnt->has<nstile_comp>())
		m_tile_build_ent = pBuildEnt;
}

void nsbuild_system::set_object_build_ent(nsentity * pBuildEnt)
{
	bool tog = m_enabled;
	if (tog)
		toggle();
	if (pBuildEnt == NULL || !pBuildEnt->has<nstile_comp>())
		m_object_build_ent = pBuildEnt;
	if (tog)
		toggle();
}

void nsbuild_system::set_mode(const mode_t & pMode)
{
	if (pMode == erase_mode)
		set_brush_type(brush_tile);
	m_current_mode = pMode;
	
}

void nsbuild_system::set_layer(const int32 & pLayer)
{
	m_layer = pLayer;
}

void nsbuild_system::set_center(const fvec3 & pMirrorCenter)
{
	m_mirror_center = pMirrorCenter;
}

bool nsbuild_system::stamp_mode()
{
	return m_stamp_mode;
}

void nsbuild_system::toggle()
{
	enable(!m_enabled);
}

int32 nsbuild_system::update_priority()
{
	return BUILD_SYS_UPDATE_PR;
}

void nsbuild_system::update()
{
	nsscene * scene = nse.current_scene();
	if (scene == NULL)
		return;

	if (m_painting)
	{
		paint();
		if (m_stamp_mode)
			m_painting = false;
	}

	if (m_erasing)
	{
		erase();
		if (m_stamp_mode)
			m_erasing = false;
	}

    if (m_enabled)
	{
        if (m_current_brush_type == brush_tile && m_tile_brush != NULL && m_mirror_mode)
		{
			nstform_comp * brush_tform = m_tile_brush->get<nstform_comp>();
			for (uint32 i = 0; i < brush_tform->count(); ++i)
			{
				nstform_comp * mirror_tform = m_mirror_brush->get<nstform_comp>();
				fvec3 wp = brush_tform->wpos(i);
				fvec3 new_pos = m_mirror_center*2.0f - wp;
				new_pos.z = wp.z;
				mirror_tform->set_pos(new_pos, i);
			}
		}
        else if (m_current_brush_type == brush_object && m_object_build_ent != NULL)
		{
			nstform_comp * obj_tform = m_object_brush->get<nstform_comp>();
			fvec3 wp = obj_tform->wpos();			
			nssel_comp * selComp = m_object_brush->get<nssel_comp>();
			bool no_collision = true;

			nsoccupy_comp * occComp = m_object_brush->get<nsoccupy_comp>();
			if (occComp != NULL)
			{
				auto selIter = selComp->begin();
				while (selIter != selComp->end())
				{
					no_collision = no_collision && !scene->grid().occupied(occComp->spaces(), obj_tform->wpos(*selIter));
					++selIter;
				}
			}

			if (m_mirror_mode)
			{
				nstform_comp * mirror_tform = m_mirror_brush->get<nstform_comp>();				
				fvec3 new_pos = m_mirror_center*2.0f - wp;
				new_pos.z = wp.z;
				mirror_tform->set_pos(new_pos);
				nssel_comp * selComp2 = m_mirror_brush->get<nssel_comp>();
				
				nsoccupy_comp * occComp2 = m_mirror_brush->get<nsoccupy_comp>();
				if (occComp2 != NULL)
				{
					auto selIter = selComp2->begin();
					while (selIter != selComp2->end())
					{
						no_collision = no_collision && !scene->grid().occupied(occComp2->spaces(), mirror_tform->wpos(*selIter));
						++selIter;
					}
				}	
			}	

			if (!no_collision)
				set_active_brush_color(fvec4(1.0f, 0.0f, 0.0f, 1.0f));
			else
				set_active_brush_color(m_object_brush->get<nssel_comp>()->default_color());
		}
		to_cursor();
	}

	if (m_enabled)
	{
		float z = 0.0;
		if (m_current_brush_type == brush_tile && m_tile_brush != NULL)
			z = m_tile_brush->get<nstform_comp>()->wpos().z;
		else if (m_current_brush_type == brush_object && m_object_build_ent != NULL)
			z = m_object_brush->get<nstform_comp>()->wpos().z;
		m_layer = nstile_grid::grid(fvec3(0,0,z)).z;

		if (m_current_mode == erase_mode && m_tile_brush != NULL)
			set_active_brush_color(fvec4(1.0f,0.0f,0.0f,1.0f));
		else if (m_current_brush_type == brush_tile && m_tile_brush != NULL)
			set_active_brush_color(m_tile_brush->get<nssel_comp>()->default_color());
	}
}

bool nsbuild_system::_handle_cam_change_event(nscam_change_event * evnt)
{
	to_cursor();
	return true;
}

bool nsbuild_system::_handle_action_event(nsaction_event * evnt)
{
	if (evnt->trigger_hash_name == trigger_hash(snap_brush_z) && m_enabled)
	{
		if (m_object_brush != NULL)
		{
			nstform_comp * tc = m_object_brush->get<nstform_comp>();
			if (tc != NULL)
			{
				tc->snap_z();
				to_cursor();
			}			
		}
		if (m_tile_brush != NULL)
		{
			nstform_comp * tc = m_tile_brush->get<nstform_comp>();
			if (tc != NULL)
			{
				tc->snap_z();
				to_cursor();
			}
		}
	}

	if (evnt->trigger_hash_name == trigger_hash(initial_snap_brush_z) && m_enabled)
	{
		if (m_object_brush != NULL)
		{
			nstform_comp * tc = m_object_brush->get<nstform_comp>();
			if (tc != NULL)
				tc->snap();
		}
		if (m_tile_brush != NULL)
		{
			nstform_comp * tc = m_tile_brush->get<nstform_comp>();
			if (tc != NULL)
				tc->snap();
		}
	}

	if (evnt->trigger_hash_name == trigger_hash(toggle_build))
		toggle();
	
	if (evnt->trigger_hash_name == trigger_hash(toggle_build_erase_mode))
	{
		if (m_current_mode == erase_mode)
			set_mode(build_mode);
		else
			set_mode(erase_mode);
	}

	if (evnt->trigger_hash_name == trigger_hash(toggle_tile_build_mode))
	{
		if (m_current_brush_type == brush_object)
			set_brush_type(brush_tile);
		else
			set_brush_type(brush_object);
	}

	return true;
}

bool nsbuild_system::_handle_state_event(nsstate_event * evnt)
{
	if (evnt->trigger_hash_name == trigger_hash(insert_entity) && m_enabled)
	{
		switch (m_current_mode)
		{
		  case(build_mode) :
			  m_painting = evnt->toggle;
			  break;
		  case(erase_mode) :
			  m_erasing = evnt->toggle;
			  break;
		}
	}
  	else if (evnt->trigger_hash_name == trigger_hash(select_move_toggle) && m_enabled && !evnt->toggle)
  			nse.event_dispatch()->push<nsstate_event>(hash_id(NSSEL_MOVE_TOGGLE), true);

	return true;
}
