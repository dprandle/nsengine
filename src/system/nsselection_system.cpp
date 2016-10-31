/*!
\file nscontroller_system.h

\brief Definition file for NSControllerSystem class

This file contains all of the neccessary definitions for the NSControllerSystem class.

\author Daniel Randle
\date March 8 2014
\copywrite Earth Banana Games 2013
*/

#include <nscube_grid.h>
#include <system/nsui_system.h>
#include <component/nsrect_tform_comp.h>
#include <nsvideo_driver.h>
#include <iostream>
#include <nsvector.h>
#include <system/nsselection_system.h>
#include <asset/nsscene_manager.h>
#include <asset/nsscene.h>
#include <nsevent_dispatcher.h>
#include <system/nstform_system.h>
#include <component/nssel_comp.h>
#include <asset/nsmesh_manager.h>
#include <component/nstile_comp.h>
#include <asset/nsshader.h>
#include <nsevent.h>
#include <asset/nsentity_manager.h>
#include <component/nsoccupy_comp.h>
#include <asset/nsmat_manager.h>
#include <nstile_grid.h>
#include <component/nstile_brush_comp.h>
#include <system/nsbuild_system.h>
#include <component/nscam_comp.h>
#include <component/nsterrain_comp.h>
#include <asset/nsplugin_manager.h>
#include <nsevent.h>
#include <nsmath/nsmath.h>
#include <opengl/nsgl_gbuffer.h>
#include <component/nsanim_comp.h>
#include <component/nsrender_comp.h>
#include <component/nsphysic_comp.h>

nsselection_system::nsselection_system() :
    nssystem(type_to_hash(nsselection_system)),
	m_focus_ent(),
	m_pick_pos(),
	m_selected_ents(),
	m_mirror_selection(false),
	m_cached_point(),
	m_moving(false),
	m_cached_point_last(),
	m_draw_occ(false),
	m_trans(),
	m_toggle_move(false),
	m_send_foc_event(false),
	m_mirror_tile_color(fvec4(1.0f, 0.0f, 1.0f, 0.7f)),
	m_started_drag_over_ui(false),
	last_pressed(nullptr)
{}

nsselection_system::~nsselection_system()
{}

void nsselection_system::enable_mirror_selection(bool enable_)
{
	m_mirror_selection = enable_;
}

bool nsselection_system::mirror_selection_enabled()
{
	return m_mirror_selection;
}

bool nsselection_system::add_to_selection(nsentity * ent)
{
	if (ent == nullptr)
		return false;

	nssel_comp * selComp = ent->get<nssel_comp>();
	if (selComp == nullptr)
		return false;

	// This little peice of code makes it so that when selecting stuff in mirror mode the mirrored stuff is
	// also selected
	if (m_mirror_selection)
	{		
		nstform_comp * tForm = ent->get<nstform_comp>();
		if (tForm == nullptr)
		{
			dprint("nsselection_system::add_to_selection tForm is null for ent " + ent->name());
			return false;
		}

		fvec3 wp = tForm->world_position();
		fvec3 newPos = nse.system<nsbuild_system>()->center()*2.0f - wp;
		newPos.z = wp.z;

		uivec2 id = m_active_scene->grid().get(newPos);
		nsentity * mir_ent = m_active_scene->find_entity(id);
		if (mir_ent != nullptr)
		{
			nssel_comp * scmirror = mir_ent->get<nssel_comp>();
			if (scmirror != nullptr)
			{
				selComp->set_selected(true);
				m_selected_ents.emplace(ent);
			}
		}
	}
	
	sig_connect(ent->resource_destroyed, nsselection_system::on_ent_destroyed);
	selComp->set_selected(true);
	auto ret = m_selected_ents.emplace(ent);
	return ret.second;
}

bool nsselection_system::add_selection_to_grid(nsscene * scn)
{
	auto iter = m_selected_ents.begin();
	while (iter != m_selected_ents.end())
	{
		nssel_comp * selComp = (*iter)->get<nssel_comp>();
		nstform_comp * tComp = (*iter)->get<nstform_comp>();
		nsoccupy_comp * occComp = (*iter)->get<nsoccupy_comp>();
		nsphysic_comp * phcomp = (*iter)->get<nsphysic_comp>();
		
		fvec3 pos = tComp->world_position();				
		if (occComp != nullptr)
		{
			if (!scn->grid().add((*iter)->full_id(), occComp->spaces(), pos))
			{
				dprint("nsselection_system::add_selection_to_grid Could not add selection to grid");
				return false;
			}
		}

		if (phcomp != nullptr && !phcomp->dynamic)
			tComp->in_cube_grid = false;
					
		++iter;
	}
	return true;
}

entity_ptr_set & nsselection_system::current_selection()
{
	return m_selected_ents;
}

bool nsselection_system::selection_contains(const uivec2 & itemid)
{
	nsentity * ent = m_active_scene->find_entity(itemid);
	if (ent == nullptr)
		return false;

	auto fiter = m_selected_ents.find(ent);
	return fiter != m_selected_ents.end();	
}

bool nsselection_system::selection_being_dragged()
{
	return m_moving;
}

void nsselection_system::set_focus_entity(const uivec2 & focus_ent)
{
	m_focus_ent = focus_ent;
	m_send_foc_event = true;
}

void nsselection_system::change_layer(int32 pChange)
{
	// remove_from_grid();
	// translate(fvec3(0, 0, pChange*-Z_GRID));
	// if (!collision())
	// 	translate(fvec3(0, 0, pChange*Z_GRID));
	// snap_z();
	// add_to_grid();
}

bool nsselection_system::selection_collides_with_tilegrid()
{
	return false;
	// if (m_active_scene == nullptr)
	// 	return false;

	// bool has_collision = false;
	// auto iter = m_selected_ents.begin();
	// while (iter != m_selected_ents.end())
	// {
	// 	nssel_comp * selComp = (*iter)->get<nssel_comp>();
	// 	nstform_comp * tComp = (*iter)->get<nstform_comp>();
	// 	tform_per_scene_info * tpsi = tComp->per_scene_info(m_active_scene);
	// 	nsoccupy_comp * occComp = (*iter)->get<nsoccupy_comp>();
	// 	if (occComp != nullptr)
	// 	{
	// 		auto selection = selComp->selection(m_active_scene); 
	// 		auto selIter = selection->begin();
	// 		while (selIter != selection->end())
	// 		{
	// 			fvec3 wpos = tpsi->m_tforms[*selIter].world_position();
	// 			has_collision = has_collision ||
	// 				m_active_scene->grid().occupied(occComp->spaces(), wpos);
	// 			++selIter;
	// 		}
	// 	}			
	// 	++iter;
	// }
	// return has_collision;
}

void nsselection_system::clear_selection()
{
	auto iter = m_selected_ents.begin();
	while (iter != m_selected_ents.end())
	{
		
		nssel_comp * sc = (*iter)->get<nssel_comp>();
		sc->set_selected(false);
		sig_disconnect((*iter)->resource_destroyed);
		++iter;
	}
	m_selected_ents.clear();
	m_focus_ent = uivec2();
	m_send_foc_event = true;
}

uivec3 nsselection_system::pick(const fvec2 & mpos)
{
	return pick(mpos.x, mpos.y);
}

uivec3 nsselection_system::pick(float mousex, float mousey)
{
	return nse.video_driver()->current_context()->pick(fvec2(mousex,mousey));
}

void nsselection_system::set_mirror_tile_color(const fvec4 & color)
{
	m_mirror_tile_color = color;
}

const fvec4 & nsselection_system::mirror_tile_color()
{
	return m_mirror_tile_color;
}

void nsselection_system::_draw_ent_occ(nsentity * ent)
{
	// // Draw the occupy component if draw enabled
	// nsoccupy_comp * occComp = ent->get<nsoccupy_comp>();
	// nssel_comp * selComp = ent->get<nssel_comp>();
	// nstform_comp * tComp = ent->get<nstform_comp>();
	// if (occComp != nullptr && selComp != nullptr && tComp != nullptr && occComp->draw_enabled())
	// {
	// 	nsmesh * occMesh = get_resource<nsmesh>(occComp->mesh_id());
	// 	nsmaterial * mat = get_resource<nsmaterial>(occComp->material_id());
	// 	if (occMesh != nullptr)
	// 	{
	// 		auto selIter = selComp->begin();
	// 		while (selIter != selComp->end())
	// 		{
	// 			for (uint32 i = 0; i < occMesh->count(); ++i)
	// 			{
	// 				nsmesh::submesh * occSub = occMesh->sub(i);

	// 				if (occSub->m_node != nullptr)
	// 					m_sel_shader->set_node_transform(occSub->m_node->m_world_tform);
	// 				else
	// 					m_sel_shader->set_node_transform(fmat4());

	// 				m_sel_shader->set_has_bones(false);
	// 				occSub->m_vao.bind();

	// 				glDisable(GL_STENCIL_TEST);

	// 				if (mat != nullptr)
	// 					m_sel_shader->set_frag_color_out(mat->color());
	// 				else
	// 					m_sel_shader->set_frag_color_out(fvec4(1.0f, 0.0f, 1.0f, 0.5f));

	// 				auto spaceIter = occComp->begin();
	// 				while (spaceIter != occComp->end())
	// 				{
	// 					m_sel_shader->set_transform(translation_mat4(nstile_grid::world(*spaceIter, tComp->wpos(*selIter))));
	// 					glDrawElements(occSub->m_prim_type,
	// 								   static_cast<GLsizei>(occSub->m_indices.size()),
	// 								   GL_UNSIGNED_INT,
	// 								   0);
	// 					++spaceIter;
	// 				}
	// 				occSub->m_vao.unbind();
	// 			}
	// 			++selIter;
	// 		}
	// 	}
	// }
}

void nsselection_system::enable_draw_occupied_grid(bool enable_)
{
	m_draw_occ = enable_;
}

bool nsselection_system::draw_occupied_grid()
{
	return m_draw_occ;
}
		
void nsselection_system::_draw_occ()
{
	// nsscene * scene = current_scene();
	// if (scene == nullptr || m_sel_shader == nullptr)
	// 	return;

	// if (!m_draw_occ)
	// 	return;

	// nsmesh * occMesh = nse.core()->get<nsmesh>(MESH_FULL_TILE);
	// for (uint32 i = 0; i < occMesh->count(); ++i)
	// {
	// 	nsmesh::submesh * occSub = occMesh->sub(i);

	// 	if (occSub->m_node != nullptr)
	// 		m_sel_shader->set_node_transform(occSub->m_node->m_world_tform);
	// 	else
	// 		m_sel_shader->set_node_transform(fmat4());

	// 	m_sel_shader->set_has_bones(false);
	// 	m_sel_shader->set_frag_color_out(fvec4(1.0f, 0.0f, 0.0f, 0.1f));

	// 	nstile_grid::grid_bounds g = scene->grid().occupied_bounds();
	// 	for (int32 z = g.min_space.z; z <= g.max_space.z; ++z)
	// 	{
	// 		for (int32 y = g.min_space.y; y <= g.max_space.y; ++y)
	// 		{
	// 			for (int32 x = g.min_space.x; x <= g.max_space.x; ++x)
	// 			{
	// 				uivec3 id = scene->grid().get(ivec3(x, y, z));
	// 				nsentity * ent = get_resource<nsentity>(id.x, id.y);
	// 				if (ent != nullptr)
	// 				{
	// 					m_trans.set_column(3, nstile_grid::world(ivec3(x,y,z)));
	// 					m_sel_shader->set_transform(m_trans);
	// 					occSub->m_vao.bind();
	// 					glDrawElements(occSub->m_prim_type,
	// 								   static_cast<GLsizei>(occSub->m_indices.size()),
	// 								   GL_UNSIGNED_INT,
	// 								   0);
	// 					occSub->m_vao.unbind();
	// 				}
	// 			}
	// 		}
	// 	}
	// }

}

void nsselection_system::_draw_hidden()
{
	// nsscene * scene = current_scene();
	// if (scene == nullptr || m_sel_shader == nullptr)
	// 	return;

	// nsentity * cam = scene->camera();
	// if (cam == nullptr)
	// 	return;

	// // Draw all hidden objects - This should probably be moved to the render system or something - or to a forward
	// // renderer when that gets set up so that we can draw transparent stuff
	// entity_ptr_set sceneEnts = scene->entities();
	// auto sceneEntIter = sceneEnts.begin();
	// while (sceneEntIter != sceneEnts.end())
	// {
	// 	nstform_comp * tComp = (*sceneEntIter)->get<nstform_comp>();
	// 	nsrender_comp * renComp = (*sceneEntIter)->get<nsrender_comp>();

	// 	if (renComp == nullptr)
	// 	{
	// 		++sceneEntIter;
	// 		continue;
	// 	}

	// 	nsanim_comp * animComp = (*sceneEntIter)->get<nsanim_comp>();
	// 	nsmesh * rMesh = get_resource<nsmesh>(renComp->mesh_id());

	// 	if (rMesh == nullptr)
	// 	{
	// 		++sceneEntIter;
	// 		continue;
	// 	}

	// 	for (uint32 index = 0; index < tComp->count(); ++index)
	// 	{
	// 		int32 state = tComp->hidden_state(index);

	// 		bool layerBit = state & nstform_comp::hide_layer && true;
	// 		bool objectBit = state & nstform_comp::hide_object && true;
	// 		bool showBit = state & nstform_comp::hide_none && true;
	// 		bool hideBit = state & nstform_comp::hide_all && true;

	// 		if (!hideBit && (!layerBit && objectBit))
	// 		{
	// 			m_sel_shader->set_transform(tComp->transform(index));
	// 			for (uint32 i = 0; i < rMesh->count(); ++i)
	// 			{
	// 				nsmesh::submesh * cSub = rMesh->sub(i);

	// 				if (cSub->m_node != nullptr)
	// 					m_sel_shader->set_node_transform(cSub->m_node->m_world_tform);
	// 				else
	// 					m_sel_shader->set_node_transform(fmat4());

	// 				if (animComp != nullptr)
	// 				{
	// 					m_sel_shader->set_has_bones(true);
	// 					m_sel_shader->set_bone_transform(*animComp->final_transforms());
	// 				}
	// 				else
	// 					m_sel_shader->set_has_bones(false);

	// 				cSub->m_vao.bind();
	// 				fvec4 col(1.0f, 1.0f, 1.0f, 0.04f);
	// 				m_sel_shader->set_frag_color_out(col);
	// 				glDrawElements(cSub->m_prim_type,
	// 							   static_cast<GLsizei>(cSub->m_indices.size()),
	// 							   GL_UNSIGNED_INT,
	// 							   0);
	// 				cSub->m_vao.unbind();
	// 			}
	// 		}
	// 	}
	// 	++sceneEntIter;
	// }
}

void nsselection_system::delete_selection()
{
	if (m_active_scene == nullptr)
		return;

    // we have to make a copy because if all instances of a selected entity
    // are removed from the scene the entity will be removed from m_selected_ents
    entity_ptr_set copy(m_selected_ents);
    auto iter = copy.begin();
    while (iter != copy.end())
	{
		m_active_scene->remove(*iter, false);
		++iter;
	}
}

bool nsselection_system::empty()
{
	return m_selected_ents.empty();
}

void nsselection_system::init()
{	
	register_action_handler(nsselection_system::_handle_selected_entity, NSSEL_SELECT);
	register_action_handler(nsselection_system::_handle_multi_select, NSSEL_MULTISELECT);
	register_action_handler(nsselection_system::_handle_shift_select, NSSEL_SHIFTSELECT);
	register_action_handler(nsselection_system::_handle_move_select, NSSEL_MOVE);
	register_action_handler(nsselection_system::_handle_move_selection_xy, NSSEL_MOVE_XY);
	register_action_handler(nsselection_system::_handle_move_selection_zy, NSSEL_MOVE_ZY);
	register_action_handler(nsselection_system::_handle_move_selection_zx, NSSEL_MOVE_ZX);
	register_action_handler(nsselection_system::_handle_move_selection_x, NSSEL_MOVE_X);
	register_action_handler(nsselection_system::_handle_move_selection_y, NSSEL_MOVE_Y);
	register_action_handler(nsselection_system::_handle_move_selection_z, NSSEL_MOVE_Z);
	register_action_handler(nsselection_system::_handle_move_selection_toggle, NSSEL_MOVE_TOGGLE);
	register_action_handler(nsselection_system::_handle_rotate_selection, "rotate_selection");
}

void nsselection_system::release()
{
	
}

int32 nsselection_system::update_priority()
{
	return SEL_SYS_UPDATE_PR;
}

void nsselection_system::translate_selection(const fvec3 & amount)
{
	if (m_active_scene == nullptr)
		return;
	
	auto iter = m_selected_ents.begin();
	while (iter != m_selected_ents.end())
	{
		nssel_comp * scomp = (*iter)->get<nssel_comp>();
		nstform_comp * tcomp = (*iter)->get<nstform_comp>();
		fvec3 translated_amount = tcomp->world_position() + amount;
		tcomp->set_world_position(translated_amount);
		++iter;
	}
}

void nsselection_system::rotate_selection(const fquat & rotation)
{
	if (m_active_scene == nullptr)
		return;
	
	auto iter = m_selected_ents.begin();
	while (iter != m_selected_ents.end())
	{
		nssel_comp * scomp = (*iter)->get<nssel_comp>();
		nstform_comp * tcomp = (*iter)->get<nstform_comp>();
		tcomp->rotate(rotation);
		++iter;
	}
}

void nsselection_system::set_occupied_spaces(bool show)
{
	m_draw_occ = show;
}

void nsselection_system::_on_draw_object(nsentity * ent, const fvec2 & pDelta, uint16 axis_, const fvec4 & norm_vp)
{
	viewport * vp = nse.video_driver()->current_context()->focused_vp;
	if (vp == nullptr)
		return;

	nsentity * cam = vp->camera;
	if (cam == nullptr)
		return;

	if (ent == nullptr)
		return;

	nstform_comp * cam_tform = cam->get<nstform_comp>();
	nscam_comp * camc = cam->get<nscam_comp>();
	
	nstform_comp * tComp = ent->get<nstform_comp>();

	if (tComp == nullptr)
    {
        dprint("nsselection_system::_on_draw_object Entity " + ent->name() + " not in scene");
        return;
    }
	
	fvec2 delta(pDelta * 2.0f / (norm_vp.zw() - norm_vp.xy()));
	fvec3 originalPos = tComp->world_position();
	fvec4 screenSpace = camc->proj_cam() * fvec4(originalPos, 1.0f);
	screenSpace /= screenSpace.w;
	screenSpace.x += delta.u;
	screenSpace.y += delta.v;

	fvec4 newPos = camc->inv_proj_cam() * screenSpace;
	newPos /= newPos.w;

	fvec3 fpos(newPos.xyz());
	fvec3 castVec = fpos - cam_tform->world_position();
	castVec.normalize();
	float depth = 0.0f;
    fvec3 normal(0.0f,0.0f,-1.0f);

    fvec3 targetVec = cam_tform->world_orientation().target();
    fvec3 projVec = project_plane(targetVec, normal);
    fvec2 projVecX = project(projVec.xy(), fvec2(1.0,0.0));
    float angleX = projVec.xy().angle_to(projVecX);

    // Set normal if not moving in a single plane
    uint32 result = axis_ & axis_z;
    if (result == axis_z)
    {
        if (angleX < 45.0f)
            normal.set(-1.0f,0.0f,0.0f);
        else
            normal.set(0.0f,-1.0f,0.0f);
    }

	depth = (normal * (originalPos - fpos)) / (normal * castVec);
	fpos += castVec*depth;
	fpos -= originalPos;	
	fpos %= fvec3(float((axis_ & axis_x) == axis_x), float((axis_ & axis_y) == axis_y), float((axis_ & axis_z) == axis_z));
	m_total_frame_translation += fpos;
}

const uivec2 & nsselection_system::selection_center_entity_id()
{
	return m_focus_ent;
}

void nsselection_system::_reset_focus(const uivec2 & pickid)
{
	if (m_selected_ents.empty())
	{
		m_focus_ent = uivec2();
		m_send_foc_event = true;
		return;
	}
	
	if (selection_contains(pickid))
	{
		m_focus_ent = pickid;
		m_send_foc_event = true;
		return;
	}

	if (selection_contains(m_focus_ent))
		return;
}

void nsselection_system::remove_from_selection(nsentity * ent)
{
	if (ent == nullptr || m_active_scene == nullptr)
		return;

	nssel_comp * selcomp = ent->get<nssel_comp>();
	if (selcomp == nullptr)
		return;

	selcomp->set_selected(false);
	m_selected_ents.erase(ent);
}

void nsselection_system::remove_from_grid()
{
	if (m_active_scene == nullptr)
		return;
	
	auto iter = m_selected_ents.begin();
	while (iter != m_selected_ents.end())
	{
		nssel_comp * selComp = (*iter)->get<nssel_comp>();
		nstform_comp * tComp = (*iter)->get<nstform_comp>();
		nsoccupy_comp * occComp = (*iter)->get<nsoccupy_comp>();
		nsphysic_comp * phcomp = (*iter)->get<nsphysic_comp>();

		if (selComp->selected())
		{
			if (occComp != nullptr)
				m_active_scene->grid().remove(occComp->spaces(), tComp->world_position());

			if (phcomp != nullptr && !phcomp->dynamic)
			{
//				m_active_scene->cube_grid->search_and_remove((*iter)->full_id(), itf.phys.aabb);
			}
		}
		++iter;
	}
}

void nsselection_system::reset_color()
{
	auto iter = m_selected_ents.begin();
	while (iter != m_selected_ents.end())
	{
		nssel_comp * selComp = (*iter)->get<nssel_comp>();
		selComp->set_color(selComp->default_color());
		++iter;
	}
}

bool nsselection_system::set_selection(nsentity * ent)
{
	clear_selection();
	return add_to_selection(ent);
}

void nsselection_system::set_selection_color(const fvec4 & pColor)
{
	auto iter = m_selected_ents.begin();
	while (iter != m_selected_ents.end())
	{
		nssel_comp * selComp = (*iter)->get<nssel_comp>();
		selComp->set_color(pColor);
		++iter;
	}
}

void nsselection_system::setup_input_map(nsinput_map * imap, const nsstring & global_ctxt_name)
{
	
	nsinput_map::trigger selectentity(
		NSSEL_SELECT,
		nsinput_map::t_both);
    imap->add_mouse_trigger(global_ctxt_name, nsinput_map::left_button,selectentity);

	nsinput_map::trigger shiftselect(
		NSSEL_SHIFTSELECT,
		nsinput_map::t_pressed);
	shiftselect.add_key_mod(nsinput_map::key_lshift);
    imap->add_mouse_trigger(global_ctxt_name, nsinput_map::movement,shiftselect);
	nsinput_map::trigger multiselect(
		NSSEL_MULTISELECT,
		nsinput_map::t_pressed);
	multiselect.add_key_mod(nsinput_map::key_lctrl);
    imap->add_mouse_trigger(global_ctxt_name, nsinput_map::left_button,multiselect);

    nsinput_map::trigger selectmove(
        NSSEL_MOVE,
        nsinput_map::t_pressed);
    selectmove.add_mouse_mod(nsinput_map::left_button);
    imap->add_mouse_trigger(global_ctxt_name, nsinput_map::movement,selectmove);

    nsinput_map::trigger select_rotate(
        "rotate_selection",
        nsinput_map::t_pressed);
    select_rotate.add_key_mod(nsinput_map::key_any);
    imap->add_key_trigger(global_ctxt_name, nsinput_map::key_r,select_rotate);


	nsinput_map::trigger selectmovexy(
		NSSEL_MOVE_XY,
		nsinput_map::t_pressed);
    selectmovexy.add_key_mod(nsinput_map::key_x);
    selectmovexy.add_key_mod(nsinput_map::key_y);
	selectmovexy.add_mouse_mod(nsinput_map::left_button);
    imap->add_mouse_trigger(global_ctxt_name, nsinput_map::movement,selectmovexy);

	nsinput_map::trigger selectmovezy(
		NSSEL_MOVE_ZY,
		nsinput_map::t_pressed);
	selectmovezy.add_key_mod(nsinput_map::key_z);
	selectmovezy.add_key_mod(nsinput_map::key_y);
	selectmovezy.add_mouse_mod(nsinput_map::left_button);
    imap->add_mouse_trigger(global_ctxt_name, nsinput_map::movement,selectmovezy);

	nsinput_map::trigger selectmovezx(
        NSSEL_MOVE_ZX,
		nsinput_map::t_pressed);
	selectmovezx.add_key_mod(nsinput_map::key_z);
	selectmovezx.add_key_mod(nsinput_map::key_x);
    selectmovezx.add_mouse_mod(nsinput_map::left_button);
    imap->add_mouse_trigger(global_ctxt_name, nsinput_map::movement,selectmovezx);

	nsinput_map::trigger selectmovex(
		NSSEL_MOVE_X,
		nsinput_map::t_pressed);
	selectmovex.add_mouse_mod(nsinput_map::left_button);
	selectmovex.add_key_mod(nsinput_map::key_x);
    imap->add_mouse_trigger(global_ctxt_name, nsinput_map::movement,selectmovex);

	nsinput_map::trigger selectmovey(
		NSSEL_MOVE_Y,
		nsinput_map::t_pressed);
	selectmovey.add_key_mod(nsinput_map::key_y);
	selectmovey.add_mouse_mod(nsinput_map::left_button);
    imap->add_mouse_trigger(global_ctxt_name, nsinput_map::movement,selectmovey);

	nsinput_map::trigger selectmovez(
        NSSEL_MOVE_Z,
		nsinput_map::t_pressed);
	selectmovez.add_key_mod(nsinput_map::key_z);
    selectmovez.add_mouse_mod(nsinput_map::left_button);
    imap->add_mouse_trigger(global_ctxt_name, nsinput_map::movement,selectmovez);

	nsinput_map::trigger selectmovetoggle(
        NSSEL_MOVE_TOGGLE,
        nsinput_map::t_both
		);
	selectmovetoggle.add_key_mod(nsinput_map::key_any);
    imap->add_mouse_trigger(global_ctxt_name, nsinput_map::left_button,selectmovetoggle);

}

bool nsselection_system::valid_tile_swap()
{
	if (m_selected_ents.empty())
		return false;

	auto iter = m_selected_ents.begin();
	while (iter != m_selected_ents.end())
	{
		if (!(*iter)->has<nstile_comp>())
			return false;
		++iter;
	}
	return true;
}

void nsselection_system::update()
{
	if (scene_error_check())
		return;
	
	if (m_send_foc_event)
	{
		nse.event_dispatch()->push<nssel_focus_event>(m_focus_ent);
		m_send_foc_event = false;
	}
	

    if (m_toggle_move)
	{
		m_toggle_move = false;
		nsentity * ent = m_active_scene->find_entity(m_focus_ent);
		if (ent != nullptr)
		{
			nstform_comp * foc_tform = ent->get<nstform_comp>();
			if (m_moving)
			{
				fvec3 originalPos = foc_tform->world_position();
				m_cached_point = originalPos;
				if (!nse.system<nsbuild_system>()->enabled())
					remove_from_grid();
			}
			else
			{
				snap_selection_to_grid();
				if (selection_collides_with_tilegrid())
				{
					foc_tform->recursive_compute();
					fvec3 pTranslate = m_cached_point - foc_tform->world_position();
					translate_selection(pTranslate);
					set_selection_color(fvec4(DEFAULT_SEL_R, DEFAULT_SEL_G, DEFAULT_SEL_B, DEFAULT_SEL_A));
					if (!add_selection_to_grid(m_active_scene))
					{
						dprint("nsselection_system::onSelect Error in resetting tiles to original grid position");
					}
				}
				else
				{
					fvec3 pos = foc_tform->world_position();
					m_active_scene->grid().snap(pos);
					if (!nse.system<nsbuild_system>()->enabled())
						add_selection_to_grid(m_active_scene);
					nse.event_dispatch()->push<nssel_focus_event>(m_focus_ent);
				}
				
				m_cached_point = fvec3();
			}
		}
	}

	// Move the selection the correct amount for this frame
	if (m_moving && !m_started_drag_over_ui)
	{
		if (!nse.system<nsbuild_system>()->enabled())
		{
			if (selection_collides_with_tilegrid())
				set_selection_color(fvec4(1.0f, 0.0f, 0.0f, 0.5f));
			else
				reset_color();
		}
		
		translate_selection(m_total_frame_translation);
		m_total_frame_translation = 0.0f;
	}

	auto iter = m_selected_ents.begin();
	while (iter != m_selected_ents.end())
	{
		nssel_comp * sc = (*iter)->get<nssel_comp>();
		if (sc == nullptr)
		{
			dprint("removing " + (*iter)->name() + " from selection as it now longer has sel comp");
			iter = m_selected_ents.erase(iter);
			continue;
		}
		else
		{
			++iter;
		}
	}
}

void nsselection_system::snap_selection_to_grid()
{
	if (m_active_scene == nullptr)
		return;
	
	auto iter = m_selected_ents.begin();
	while (iter != m_selected_ents.end())
	{
		nstform_comp * tcomp = (*iter)->get<nstform_comp>();		
		fvec3 snapped_pos = tcomp->world_position();
		nstile_grid::snap(snapped_pos);
		tcomp->set_world_position(snapped_pos);
		++iter;
	}
	
}

bool nsselection_system::_handle_selected_entity(nsaction_event * evnt)
{		
	uivec2 pickid = pick(evnt->norm_mpos).xy();
	nsentity * selectedEnt = get_asset<nsentity>(pickid);

	if (evnt->cur_state == nsaction_event::begin)
	{
		last_pressed = selectedEnt;
		if (selectedEnt != nullptr)
		{
			if (selectedEnt->has<nsrect_tform_comp>())
				return true;
			nssel_comp * sc = selectedEnt->get<nssel_comp>();
			sc->pressed(selectedEnt);
		}
		if (!selection_contains(pickid))
			clear_selection();
		add_to_selection(selectedEnt);
		_reset_focus(pickid);
	}
	else
	{
		if (selectedEnt != nullptr)
		{
			nssel_comp * sc = selectedEnt->get<nssel_comp>();
			sc->released(selectedEnt);
			
			if (last_pressed == selectedEnt)
				sc->clicked(selectedEnt);
		}
		last_pressed = nullptr;
	}
	
	return true;
}

bool nsselection_system::_handle_multi_select(nsaction_event * evnt)
{		
	uivec2 pickid = pick(evnt->norm_mpos).xy();
	nsentity * selectedEnt = get_asset<nsentity>(pickid);

	if (selectedEnt != nullptr && selectedEnt->has<nsrect_tform_comp>())
		return true;

	if (selection_contains(pickid))
		remove_from_selection(selectedEnt);
	else
		add_to_selection(selectedEnt);
	_reset_focus(pickid);
	return true;
}

bool nsselection_system::_handle_shift_select(nsaction_event * evnt)
{
	uivec2 pickid = pick(evnt->norm_mpos).xy();
	nsentity * selectedEnt = get_asset<nsentity>(pickid);

	if (selectedEnt != nullptr && selectedEnt->has<nsrect_tform_comp>())
		return true;

	add_to_selection(selectedEnt);
	_reset_focus(pickid);
	return true;
}

bool nsselection_system::_handle_move_select(nsaction_event * evnt)
{
    if (m_started_drag_over_ui)
        return true;

	viewport * vp = nse.video_driver()->current_context()->front_viewport_at_screen_pos(evnt->norm_mpos);
	if (vp == nullptr)
		return true;
	nsentity * ent = get_asset<nsentity>(m_focus_ent);
    _on_draw_object(ent, evnt->norm_delta, axis_x | axis_y, vp->normalized_bounds);
	return true;
}

bool nsselection_system::_handle_move_selection_xy(nsaction_event * evnt)
{
    if (m_started_drag_over_ui)
        return true;
	viewport * vp = nse.video_driver()->current_context()->front_viewport_at_screen_pos(evnt->norm_mpos);
	if (vp == nullptr)
        return true;
	nsentity * ent = get_asset<nsentity>(m_focus_ent);
	_on_draw_object(ent, evnt->norm_delta, axis_x | axis_y, vp->normalized_bounds);
	return true;
}

bool nsselection_system::_handle_move_selection_zy(nsaction_event * evnt)
{
    if (m_started_drag_over_ui)
        return true;
	viewport * vp = nse.video_driver()->current_context()->front_viewport_at_screen_pos(evnt->norm_mpos);
	if (vp == nullptr)
		return true;		
	nsentity * ent = get_asset<nsentity>(m_focus_ent);
	_on_draw_object(ent, evnt->norm_delta, axis_y | axis_z, vp->normalized_bounds);
	return true;
}

bool nsselection_system::_handle_move_selection_zx(nsaction_event * evnt)
{
    if (m_started_drag_over_ui)
        return true;
	viewport * vp = nse.video_driver()->current_context()->front_viewport_at_screen_pos(evnt->norm_mpos);
	if (vp == nullptr)
		return true;		
	nsentity * ent = get_asset<nsentity>(m_focus_ent);
	_on_draw_object(ent, evnt->norm_delta, axis_x | axis_z, vp->normalized_bounds);
	return true;
}

bool nsselection_system::_handle_move_selection_x(nsaction_event * evnt)
{
    if (m_started_drag_over_ui)
        return true;
	viewport * vp = nse.video_driver()->current_context()->front_viewport_at_screen_pos(evnt->norm_mpos);
	if (vp == nullptr)
		return true;		
	nsentity * ent = get_asset<nsentity>(m_focus_ent);
	_on_draw_object(ent, evnt->norm_delta, axis_x, vp->normalized_bounds);
	return true;
}

bool nsselection_system::_handle_move_selection_y(nsaction_event * evnt)
{
    if (m_started_drag_over_ui)
        return true;
	viewport * vp = nse.video_driver()->current_context()->front_viewport_at_screen_pos(evnt->norm_mpos);
	if (vp == nullptr)
		return true;		
	nsentity * ent = get_asset<nsentity>(m_focus_ent);
	_on_draw_object(ent, evnt->norm_delta, axis_y, vp->normalized_bounds);
	return true;
}

bool nsselection_system::_handle_move_selection_z(nsaction_event * evnt)
{
    if (m_started_drag_over_ui)
        return true;
	viewport * vp = nse.video_driver()->current_context()->front_viewport_at_screen_pos(evnt->norm_mpos);
	if (vp == nullptr)
		return true;		
	nsentity * ent = get_asset<nsentity>(m_focus_ent);
	_on_draw_object(ent, evnt->norm_delta, axis_z,vp->normalized_bounds);
	return true;
}

bool nsselection_system::_handle_move_selection_toggle(nsaction_event * evnt)
{
	viewport * vp = nse.video_driver()->current_context()->front_viewport_at_screen_pos(evnt->norm_mpos);
	if (vp == nullptr)
		return true;		
    if (nse.system<nsui_system>()->mpos_over_element(evnt->norm_mpos, vp) && evnt->cur_state == nsaction_event::begin)
    {
        m_started_drag_over_ui = true;
		return true;
    }
    if (m_started_drag_over_ui)
	{
		m_started_drag_over_ui = false;
        return true;
	}
	m_toggle_move = true;
	m_moving = evnt->cur_state;
	return true;
}

bool nsselection_system::_handle_rotate_selection(nsaction_event * evnt)
{
	rotate_selection(::orientation(fvec4(0,0,1,30)));
	return true;
}

void nsselection_system::on_ent_destroyed(uint32 type_id, uivec2 ent_id)
{
	auto iter = m_selected_ents.begin();
	while (iter != m_selected_ents.end())
	{
		if (type_id == (*iter)->type() && (*iter)->full_id() == ent_id)
		{
			iter = m_selected_ents.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

