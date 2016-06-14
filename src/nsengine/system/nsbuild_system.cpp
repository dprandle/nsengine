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
#include <nsplugin_manager.h>
#include <nsinput_system.h>
#include <nsentity.h>
#include <nslight_comp.h>
#include <nscam_comp.h>
#include <nscamera_system.h>

nsbuild_system::nsbuild_system():
nssystem(type_to_hash(nsbuild_system)),
m_enabled(false),
m_tile_brush(nullptr),
m_object_brush(nullptr),
m_tile_build_ent(nullptr),
m_object_build_ent(nullptr),
m_mirror_brush(nullptr),
m_layer(0),
	m_current_mode(build_mode),
	m_current_brush_type(brush_none),
	m_tile_brush_center_tform_id(0),
	m_overwrite(false),
	m_mirror_mode(false),
m_painting(false),
	m_erasing(false),
	m_stamp_mode(false)
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
	if (m_active_scene == nullptr)
		return;
	
	if (pEnable && !m_enabled)
	{
		m_enabled = pEnable;
		nse.system<nsinput_system>()->push_context(BUILD_MODE_CTXT);
		nse.system<nsselection_system>()->clear_selection();

		nse.event_dispatch()->push<nsaction_event>(hash_id(NSSEL_MOVE_TOGGLE), nsaction_event::begin);

		if (m_current_brush_type == brush_tile)
		{
			if (m_tile_brush == nullptr)
				return;

			if (m_mirror_mode)
			{
				m_mirror_brush = nse.core()->create<nsentity>(ENT_MIRROR_BRUSH, m_tile_brush);
			}

			nstile_brush_comp * brushComp = m_tile_brush->get<nstile_brush_comp>();

			nssel_comp * selComp = m_tile_brush->get<nssel_comp>();
			if (selComp == nullptr || brushComp == nullptr)
				return;

            bool tmp = nse.system<nsselection_system>()->mirror_selection_enabled();
            nse.system<nsselection_system>()->enable_mirror_selection(false);

			auto brushIter = brushComp->begin();
			while (brushIter != brushComp->end())
			{
				fvec3 pos = nstile_grid::world(ivec3(brushIter->x, brushIter->y, m_layer));
				uint32 tFormID = m_active_scene->add(m_tile_brush, nullptr, true, pos);	
				nse.system<nsselection_system>()->add_to_selection(m_tile_brush, tFormID);
				nse.system<nsselection_system>()->set_focus_entity(uivec3(m_tile_brush->full_id(),tFormID));					
				if (m_mirror_mode)
				{
					fvec3 mirrorPos = m_mirror_center*2.0f - pos;
					mirrorPos.z = pos.z;
					uint32 mirror_tform_id = m_active_scene->add(m_mirror_brush, nullptr, true, mirrorPos);
					nse.system<nsselection_system>()->add_to_selection(m_mirror_brush, mirror_tform_id);
					auto tfi = m_mirror_brush->get<nstform_comp>()->instance_transform(m_active_scene, mirror_tform_id);
					tfi->set_hidden_state(nstform_comp::hide_all);
				}

				if (*brushIter == ivec2())
					m_tile_brush_center_tform_id = tFormID;
				
				auto tfi = m_tile_brush->get<nstform_comp>()->instance_transform(m_active_scene, tFormID);
				tfi->set_hidden_state(nstform_comp::hide_all);
				++brushIter;
			}

			//nse.system<nsselection_system>()->set_focus_entity(
			//	uivec3(m_tile_brush->full_id(),m_tile_brush_center_tform_id)
			//	);
			
			selComp->set_selected(m_active_scene, true);

            nse.system<nsselection_system>()->enable_mirror_selection(tmp);
			to_cursor();
		}
		else if (m_current_brush_type == brush_object)
		{
			
			if (m_object_brush == nullptr)
				return;

			if (m_object_build_ent == nullptr)
				return;

			m_object_brush->del<nsrender_comp>();
			m_object_brush->del<nslight_comp>();
            m_object_brush->create(m_object_build_ent->get<nsrender_comp>());
			m_object_brush->create(m_object_build_ent->get<nslight_comp>());
			fvec3 pos = nstile_grid::world(ivec3(0,0,m_layer));
			
			if (m_mirror_mode)
				m_mirror_brush = nse.core()->create<nsentity>(ENT_MIRROR_BRUSH, m_object_brush);

			nssel_comp * selComp = m_object_brush->get<nssel_comp>();
			uint32 tFormID = m_active_scene->add(m_object_brush, nullptr, false, pos);
			nse.system<nsselection_system>()->add_to_selection(m_object_brush, tFormID);
			selComp->set_selected(m_active_scene, true);

			nse.system<nsselection_system>()->set_focus_entity(
				uivec3(m_object_brush->full_id(),tFormID)
				);

			auto tfi = m_object_brush->get<nstform_comp>()->instance_transform(m_active_scene, tFormID);
			tfi->set_hidden_state(nstform_comp::hide_all);
			
			bool tmp = nse.system<nsselection_system>()->mirror_selection_enabled();				
			nse.system<nsselection_system>()->enable_mirror_selection(false);				
				
			if (m_mirror_mode)
			{
				fvec3 mirrorPos = m_mirror_center*2.0f - pos;
				mirrorPos.z = pos.z;
				uint32 mirror_tform_id = m_active_scene->add(m_mirror_brush, nullptr, false, mirrorPos);
				nse.system<nsselection_system>()->add_to_selection(m_mirror_brush, mirror_tform_id);
				auto tfi = m_mirror_brush->get<nstform_comp>()->instance_transform(m_active_scene, mirror_tform_id);
				tfi->set_hidden_state(nstform_comp::hide_all);
			}

			m_object_brush->del<nsoccupy_comp>();
            m_object_brush->create(m_object_build_ent->get<nsoccupy_comp>());

			if (m_mirror_mode)
			{
				m_mirror_brush->del<nsoccupy_comp>();
				m_mirror_brush->create(m_object_build_ent->get<nsoccupy_comp>());
			}
				

			nse.system<nsselection_system>()->enable_mirror_selection(tmp);
			to_cursor();
		}
		else
			return;
	}
	else if (!pEnable && m_enabled)
	{
		m_enabled = pEnable;
		nse.event_dispatch()->push<nsaction_event>(hash_id(NSSEL_MOVE_TOGGLE), nsaction_event::end);
		nse.system<nsinput_system>()->pop_context();

		if (m_current_brush_type == brush_object && m_object_brush != nullptr)
		{
			m_object_brush->del<nsoccupy_comp>();
			m_object_brush->del<nsrender_comp>();
			m_object_brush->del<nslight_comp>();
			m_active_scene->remove(m_object_brush, true);
		}

		if (m_current_brush_type == brush_tile && m_tile_brush != nullptr)
		{
			m_active_scene->remove(m_tile_brush, true);
		}

		nse.system<nsselection_system>()->clear_selection();
        nse.core()->destroy<nsentity>(ENT_MIRROR_BRUSH);
		m_mirror_brush = nullptr;
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
	if (m_active_scene == nullptr)
		return;

	if (m_current_brush_type == brush_tile)
	{
		if (m_tile_brush == nullptr)
			return;

		nstile_brush_comp * brushComp = m_tile_brush->get<nstile_brush_comp>();
		if (brushComp == nullptr)
			return;

		auto brushIter = brushComp->begin();
		while (brushIter != brushComp->end())
		{
			for (int32 i = 0; i < brushComp->height(); ++i)
			{
				auto tfi = m_tile_brush->get<nstform_comp>()->instance_transform(m_active_scene,
																				 m_tile_brush_center_tform_id);
				fvec3 pos = tfi->world_position() + nstile_grid::world(ivec3(brushIter->x, brushIter->y, -i));
				m_active_scene->remove(pos, false);
				if (m_mirror_mode)
				{
					fvec3 new_pos = m_mirror_center*2.0f - pos;
					new_pos.z = pos.z;
					m_active_scene->remove(new_pos, false);
				}
			}
			++brushIter;
		}
	}
}

const fvec4 nsbuild_system::active_brush_color() const
{
	if (m_current_brush_type == brush_tile && m_tile_brush != nullptr)
		return m_tile_brush->get<nssel_comp>()->default_color();
	else if (m_current_brush_type == brush_object && m_object_brush != nullptr)
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
	nsplugin * cplg = nse.core();

	if (cplg == nullptr)
	{
		dprint("nsbuild_system::init - You must first call nse.start before initializing");
		return;
	}
	
	m_object_brush = cplg->create<nsentity>(ENT_OBJECT_BRUSH);
	nssel_comp * sc = m_object_brush->create<nssel_comp>();	
	sc->set_default_sel_color(fvec4(0.0f, 1.0f, 0.0f, 1.0f));
	sc->set_color(fvec4(0.0f, 1.0f, 0.0f, 1.0f));
	sc->set_mask_alpha(0.2f);
	sc->enable_draw(true);
	sc->enable_move(true);

	register_action_handler(nsbuild_system::_handle_select_move_toggle, NSSEL_MOVE_TOGGLE);
	register_action_handler(nsbuild_system::_handle_snap_z, NSSNAP_BRUSH_Z);
	register_action_handler(nsbuild_system::_handle_initial_snap_brush_z, NSINITIAL_SNAP_BRUSH_Z);
	register_action_handler(nsbuild_system::_handle_insert_entity, NSINSERT_ENTITY);
	register_action_handler(nsbuild_system::_handle_toggle_build_erase_mode, NSTOGGLE_BUILD_ERASE_MODE);
	register_action_handler(nsbuild_system::_handle_toggle_build, NSTOGGLE_BUILD);
	register_action_handler(nsbuild_system::_handle_toggle_tile_build_mode, NSTOGGLE_TILE_BUILD_MODE);
	register_handler(nsbuild_system::_handle_cam_change_event);
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
	viewport * vp = nse.video_driver()->focused_viewport();
	if (vp == nullptr)
		return;

	nsentity * camera = vp->camera;
	if (camera == nullptr)
		return;

	nstform_comp * cam_tform = camera->get<nstform_comp>();
	nscam_comp * camc = camera->get<nscam_comp>();
	fvec2 cursor_pos = platform_normalized_mpos();

	nstform_comp * brush_tform = nullptr;
	instance_tform * itf = nullptr;
	fvec3 original_pos;
	
	if (m_current_brush_type == brush_tile)
	{
		if (m_tile_brush == nullptr)
			return;
		brush_tform = m_tile_brush->get<nstform_comp>();
		if (brush_tform == nullptr)
			return;
		itf = brush_tform->instance_transform(m_active_scene, m_tile_brush_center_tform_id);
		original_pos = itf->world_position();
	}
	else if (m_current_brush_type == brush_object)
	{
		if (m_object_brush == nullptr)
			return;
		brush_tform = m_object_brush->get<nstform_comp>();
		if (brush_tform == nullptr)
			return;
		itf = brush_tform->instance_transform(m_active_scene, 0);
		original_pos = itf->world_position();
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
	fvec3 castVec = fpos - cam_tform->instance_transform(m_active_scene, 0)->world_position();
	castVec.normalize();

	fvec3 normal(0.0f,0.0f,-1.0f);
	float denom = normal * castVec;
	float depth = 0.0f;
	depth = (normal * (original_pos - fpos)) / denom;
	
	fpos += castVec*depth;
	fpos -= original_pos;
	
	// for (uint32 i = 0; i < brush_tform->instance_count(m_active_scene); ++i)
	// 	brush_tform->translate(fpos, i);
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
	if (m_active_scene == nullptr)
		return;
	
	if (m_current_brush_type == brush_tile)
	{
		if (m_tile_build_ent == nullptr)
			return;

		if (m_tile_brush == nullptr)
			return;

		nstile_brush_comp * brushComp = m_tile_brush->get<nstile_brush_comp>();
		if (brushComp == nullptr)
			return;

		auto brushIter = brushComp->begin();
		while (brushIter != brushComp->end())
		{
			for (int32 i = 0; i < brushComp->height(); ++i)
			{
				fvec3 pos = m_tile_brush->get<nstform_comp>()->instance_transform(m_active_scene, m_tile_brush_center_tform_id)->world_position() + nstile_grid::world(ivec3(brushIter->x, brushIter->y, -i)); // add in height when get working
				nstile_grid::snap(pos);
				
				if (m_overwrite)
				{
					m_active_scene->remove(pos, false);
					if (m_mirror_mode)
					{
						fvec3 new_pos = m_mirror_center*2.0f - pos;
						new_pos.z = pos.z;
						m_active_scene->remove(new_pos, false);
					}
				}

				uint32 tFormID = m_active_scene->add(m_tile_build_ent, nullptr, true, pos);
				
				if (tFormID != -1)
				{
					if (m_mirror_mode)
					{
						fvec3 new_pos = m_mirror_center*2.0f - pos;
						new_pos.z = pos.z;
						uint32 tFormMID = m_active_scene->add(m_tile_build_ent, nullptr, true, new_pos);
						if (tFormMID == -1)
						{
							m_active_scene->remove(m_tile_build_ent, tFormID);
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
		if (m_object_brush == nullptr || m_object_build_ent == nullptr)
			return;

		fvec3 pos = m_object_brush->get<nstform_comp>()->instance_transform(m_active_scene, 0)->world_position();
		nstile_grid::snap(pos);
		uint32 tFormID = m_active_scene->add(m_object_build_ent, nullptr, false, pos);

		if (tFormID != -1)
		{
			if (m_mirror_mode)
			{
				fvec3 new_pos = m_mirror_center*2.0f - pos;
				new_pos.z = pos.z;
				uint32 tFormMID = m_active_scene->add(m_object_build_ent, nullptr, false, new_pos);
				if (tFormMID == -1)
				{
					m_active_scene->remove(m_object_build_ent, tFormID, false);
					return;
				}
			}
		}
	}
	to_cursor();
}

void nsbuild_system::set_active_brush_color(const fvec4 & pColor)
{
	if (m_current_brush_type == brush_tile && m_tile_brush != nullptr)
		m_tile_brush->get<nssel_comp>()->set_color(pColor);
	else if (m_current_brush_type == brush_object && m_object_brush != nullptr)
		m_object_brush->get<nssel_comp>()->set_color(pColor);
	
	if (m_mirror_mode && m_mirror_brush != nullptr)
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
		m_tile_build_ent = nullptr;
		m_object_build_ent = nullptr;
		break;
	case (brush_tile):
		if (m_tile_build_ent == nullptr)
			return;
		if (!m_tile_build_ent->has<nstile_comp>())
			m_tile_build_ent = nullptr;
		break;
	case (brush_object):
		if (m_object_build_ent == nullptr)
			return;
		if (m_object_build_ent->has<nstile_comp>())
			m_object_build_ent = nullptr;
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
	if (pBuildEnt == nullptr || pBuildEnt->has<nstile_comp>())
		m_tile_build_ent = pBuildEnt;
}

void nsbuild_system::set_object_build_ent(nsentity * pBuildEnt)
{
	bool tog = m_enabled;
	if (tog)
		toggle();
	if (pBuildEnt == nullptr || !pBuildEnt->has<nstile_comp>())
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
	if (scene_error_check())
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
        if (m_current_brush_type == brush_tile && m_tile_brush != nullptr && m_mirror_mode)
		{
			nstform_comp * brush_tform = m_tile_brush->get<nstform_comp>();
			for (uint32 i = 0; i < brush_tform->instance_count(m_active_scene); ++i)
			{
				nstform_comp * mirror_tform = m_mirror_brush->get<nstform_comp>();
				
				fvec3 wp = brush_tform->instance_transform(m_active_scene, i)->world_position();
				fvec3 new_pos = m_mirror_center*2.0f - wp;
				new_pos.z = wp.z;
				mirror_tform->instance_transform(m_active_scene, i)->set_world_position(new_pos);
			}
		}
        else if (m_current_brush_type == brush_object && m_object_build_ent != nullptr)
		{
			nstform_comp * obj_tform = m_object_brush->get<nstform_comp>();
			fvec3 wp = obj_tform->instance_transform(m_active_scene, 0)->world_position();
			nssel_comp * selComp = m_object_brush->get<nssel_comp>();
			bool no_collision = true;

			nsoccupy_comp * occComp = m_object_brush->get<nsoccupy_comp>();
			if (occComp != nullptr)
			{
				auto selection = selComp->selection(m_active_scene);
				if (selection != nullptr)
				{
					auto selIter = selection->begin();
					while (selIter != selection->end())
					{
						no_collision = no_collision && !m_active_scene->grid().occupied(occComp->spaces(), obj_tform->instance_transform(m_active_scene, *selIter)->world_position());
						++selIter;
					}
				}
			}

			if (m_mirror_mode)
			{
				nstform_comp * mirror_tform = m_mirror_brush->get<nstform_comp>();				
				fvec3 new_pos = m_mirror_center*2.0f - wp;
				new_pos.z = wp.z;
				auto mirror_itf = mirror_tform->instance_transform(m_active_scene, 0);
				mirror_itf->set_world_position(new_pos);				
				nssel_comp * selComp2 = m_mirror_brush->get<nssel_comp>();
				nsoccupy_comp * occComp2 = m_mirror_brush->get<nsoccupy_comp>();
				if (occComp2 != nullptr)
				{
					auto selection = selComp2->selection(m_active_scene);
					if (selection != nullptr)
					{
						auto selIter = selection->begin();
						while (selIter != selection->end())
						{
							no_collision = no_collision && !m_active_scene->grid().occupied(occComp2->spaces(), mirror_itf->world_position());
							++selIter;
						}
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
		if (m_current_brush_type == brush_tile && m_tile_brush != nullptr)
			z = m_tile_brush->get<nstform_comp>()->instance_transform(m_active_scene, 0)->world_position().z;
		else if (m_current_brush_type == brush_object && m_object_build_ent != nullptr)
			z = m_object_brush->get<nstform_comp>()->instance_transform(m_active_scene, 0)->world_position().z;
		m_layer = nstile_grid::grid(fvec3(0,0,z)).z;

		if (m_current_mode == erase_mode && m_tile_brush != nullptr)
			set_active_brush_color(fvec4(1.0f,0.0f,0.0f,1.0f));
		else if (m_current_brush_type == brush_tile && m_tile_brush != nullptr)
			set_active_brush_color(m_tile_brush->get<nssel_comp>()->default_color());
	}
}

bool nsbuild_system::_handle_cam_change_event(nscam_change_event * evnt)
{
	to_cursor();
	return true;
}

bool nsbuild_system::_handle_initial_snap_brush_z(nsaction_event * evnt)
{
	if (m_enabled)
	{
		if (m_object_brush != nullptr)
		{
			nstform_comp * tc = m_object_brush->get<nstform_comp>();
			if (tc != nullptr)
			{
				for (uint32 i = 0; i < tc->instance_count(m_active_scene); ++i)
				{
					auto itf = tc->instance_transform(m_active_scene, i);
					fvec3 wpos = itf->world_position();
					nstile_grid::snap(wpos);
					itf->set_world_position(wpos);
				}
			}
		}
		if (m_tile_brush != nullptr)
		{
			nstform_comp * tc = m_tile_brush->get<nstform_comp>();
			if (tc != nullptr)
			{
				for (uint32 i = 0; i < tc->instance_count(m_active_scene); ++i)
				{
					auto itf = tc->instance_transform(m_active_scene, i);
					fvec3 wpos = itf->world_position();
					nstile_grid::snap(wpos);
					itf->set_world_position(wpos);
				}

			}
		}
	}
	return true;
}

bool nsbuild_system::_handle_snap_z(nsaction_event * evnt)
{
	if (m_enabled)
	{
		if (m_object_brush != nullptr)
		{
			nstform_comp * tc = m_object_brush->get<nstform_comp>();
			if (tc != nullptr)
			{
				for (uint32 i = 0; i < tc->instance_count(m_active_scene); ++i)
				{
					auto itf = tc->instance_transform(m_active_scene, i);
					fvec3 wpos = itf->world_position();
					nstile_grid::snap(wpos);
					itf->set_world_position(fvec3(itf->world_position().xy(),wpos.z));
				}
				to_cursor();
			}			
		}
		if (m_tile_brush != nullptr)
		{
			nstform_comp * tc = m_tile_brush->get<nstform_comp>();
			if (tc != nullptr)
			{
				for (uint32 i = 0; i < tc->instance_count(m_active_scene); ++i)
				{
					auto itf = tc->instance_transform(m_active_scene, i);
					fvec3 wpos = itf->world_position();
					nstile_grid::snap(wpos);
					itf->set_world_position(fvec3(itf->world_position().xy(),wpos.z));
				}
				to_cursor();
			}
		}
	}
	return true;
}

bool nsbuild_system::_handle_toggle_build_erase_mode(nsaction_event * evnt)
{
	if (m_current_mode == erase_mode)
		set_mode(build_mode);
	else
		set_mode(erase_mode);
	return true;
}

bool nsbuild_system::_handle_toggle_build(nsaction_event * evnt)
{
	toggle();
	return true;
}

bool nsbuild_system::_handle_toggle_tile_build_mode(nsaction_event * evnt)
{
	if (m_current_brush_type == brush_object)
		set_brush_type(brush_tile);
	else
		set_brush_type(brush_object);
	return true;
}

bool nsbuild_system::_handle_select_move_toggle(nsaction_event * evnt)
{
	if (m_enabled && !evnt->cur_state)
		nse.event_dispatch()->push<nsaction_event>(hash_id(NSSEL_MOVE_TOGGLE), nsaction_event::begin);
	return true;
}

bool nsbuild_system::_handle_insert_entity(nsaction_event * evnt)
{
	if (m_enabled)
	{
		switch (m_current_mode)
		{
		  case(build_mode) :
			  m_painting = evnt->cur_state;
			  break;
		  case(erase_mode) :
			  m_erasing = evnt->cur_state;
			  break;
		}
	}
	return true;
}
