 /*!
\file nscontroller_system.h

\brief Definition file for NSControllerSystem class

This file contains all of the neccessary definitions for the NSControllerSystem class.

\author Daniel Randle
\date March 8 2014
\copywrite Earth Banana Games 2013
*/

#include <iostream>

#include <nsselection_system.h>
#include <nsscene_manager.h>
#include <nsscene.h>
#include <nsevent_dispatcher.h>
#include <nsrender_system.h>
#include <nssel_comp.h>
#include <nsmesh_manager.h>
#include <nstile_comp.h>
#include <nsshader.h>
#include <nsevent.h>
#include <nsentity_manager.h>
#include <nsoccupy_comp.h>
#include <nsmat_manager.h>
#include <nstile_grid.h>
#include <nstile_brush_comp.h>
#include <nsbuild_system.h>
#include <nscam_comp.h>
#include <nsterrain_comp.h>
#include <nsplugin.h>
#include <nsevent.h>
#include <nsmath.h>
#include <nsgbuf_object.h>
#include <nsanim_comp.h>
#include <nsrender_comp.h>

nsselection_system::nsselection_system() :
	m_focus_ent(),
	m_pick_pos(),
	m_selected_ents(),
	m_sel_shader(NULL),
	m_cached_point(),
	m_moving(false),
	m_layer_mode(false),
	m_layer(0),
	m_cached_point_last(),
	m_draw_occ(false),
	m_final_buf(0),
	m_picking_buf(0),
	m_trans(),
	m_toggle_move(false),
	m_send_foc_event(false),
	nssystem()
{}

nsselection_system::~nsselection_system()
{}

bool nsselection_system::add(nsentity * ent, uint32 tformid)
{
	if (ent == NULL)
		return false;

	nssel_comp * selComp = ent->get<nssel_comp>();
	if (selComp == NULL)
		return false;

	// TODO: Make this an event - that is a mirror mode event to put build and sel system in mirror mode
	if (nse.system<nsbuild_system>()->mirror())
	{
		nsscene * scn = nse.current_scene();
		if (scn == NULL)
			return false;
		
		nstform_comp * tForm = ent->get<nstform_comp>();
		if (tForm == NULL)
		{
			dprint("nsselection_system::add tForm is null for ent " + ent->name());
			return false;
		}

		fvec3 wp = tForm->wpos(tformid);
		fvec3 newPos = nse.system<nsbuild_system>()->center()*2.0f - wp;
		newPos.z = wp.z;

		uivec3 id = scn->grid().get(newPos);
		if (id != 0)
		{
			nsentity * entAdd = scn->entity(id.x,id.y);
			m_selected_ents.insert(entAdd);
			entAdd->get<nssel_comp>()->set_selected(true);
			entAdd->get<nssel_comp>()->add(id.y);
		}
	}
	m_selected_ents.insert(ent);
	selComp->set_selected(true);
	return selComp->add(tformid);
}

bool nsselection_system::add_to_grid()
{
	nsscene * scene = nse.current_scene();
	if (scene == NULL)
		return false;

	auto iter = m_selected_ents.begin();
	while (iter != m_selected_ents.end())
	{
		nssel_comp * selComp = (*iter)->get<nssel_comp>();
		nstform_comp * tComp = (*iter)->get<nstform_comp>();
		nsoccupy_comp * occComp = (*iter)->get<nsoccupy_comp>();

		if (occComp != NULL)
		{
			auto selIter = selComp->begin();
			while (selIter != selComp->end())
			{
				if (!scene->grid().add(uivec3((*iter)->plugin_id(), (*iter)->id(), *selIter), occComp->spaces(), tComp->lpos(*selIter)))
				{
					dprint("Could not add selection to grid");
					return false;
				}
				++selIter;
			}
		}
		++iter;
	}
	return true;
}

bool nsselection_system::contains(const uivec3 & itemid)
{
	auto iter = m_selected_ents.begin();
	while (iter != m_selected_ents.end())
	{
		if ((*iter)->full_id() == itemid.xy())
		{
			if ((*iter)->get<nssel_comp>()->contains(itemid.z))
				return true;
		}
		++iter;
	}
	return false;
}

// bool nsselection_system::handleEvent(NSEvent * pEvent)
// {
// 	if (pEvent->mID == NSEvent::SelPick)
// 	{
// 		NSSelPickEvent * selEvent = (NSSelPickEvent*)pEvent;
// 		mPickPos = selEvent->mPickPos;
// 		return true;
// 	}
// 	else if (pEvent->mID == NSEvent::ClearSelection)
// 	{
// 		clear();
// 		return true;
// 	}
// 	else if (pEvent->mID == NSEvent::SelSet)
// 	{
// 		NSSelSetEvent * selEvent = (NSSelSetEvent*)pEvent;
// 		mFocusEnt = selEvent->mEntRefID;
// 		//set(mScene->get(mFocusEnt.y)->get<nssel_comp>(), mFocusEnt.z);
// 		return true;
// 	}
// 	else if (pEvent->mID == NSEvent::SelAdd)
// 	{
// 		// This will handle any add to selection events denoted as SelAdd
// 		NSSelSetEvent * selEvent = (NSSelSetEvent*)pEvent; // Get the specific event

// 		// Get the selection entity and make sure it is valid
// 		nsentity * selEnt = nse.resource<nsentity>(selEvent->mEntRefID.x, selEvent->mEntRefID.y);
// 		if (selEnt == NULL)
// 		{
// 			dprint("nsselection_system::handleEvent Selection entity sent in event is NULL");
// 			return false;
// 		}

// 		// Add the selection to the current selection
// 		add(selEnt, selEvent->mEntRefID.z);
// 		return true;
// 	}
// 	return false;
// }

void nsselection_system::change_layer(int32 pChange)
{
	remove_from_grid();
	translate(fvec3(0, 0, pChange*-Z_GRID));
	if (!collision())
		translate(fvec3(0, 0, pChange*Z_GRID));
	snap_z();
	add_to_grid();
}

bool nsselection_system::collision()
{
	nsscene * scene = nse.current_scene();
	if (scene == NULL)
		return false;

	bool noCollision = true;
	auto iter = m_selected_ents.begin();
	while (iter != m_selected_ents.end())
	{
		nssel_comp * selComp = (*iter)->get<nssel_comp>();
		nstform_comp * tComp = (*iter)->get<nstform_comp>();
		nsoccupy_comp * occComp = (*iter)->get<nsoccupy_comp>();
		if (occComp != NULL)
		{
			auto selIter = selComp->begin();
			while (selIter != selComp->end())
			{
				noCollision = !scene->grid().occupied(occComp->spaces(), tComp->wpos(*selIter)) && noCollision;
				++selIter;
			}
		}			
		++iter;
	}
	return noCollision;
}

void nsselection_system::clear()
{
	auto iter = m_selected_ents.begin();
	while (iter != m_selected_ents.end())
	{
		(*iter)->get<nssel_comp>()->clear();
		++iter;
	}
	m_selected_ents.clear();
	m_focus_ent = uivec3();
	//nse.events()->send(new NSSelFocusChangeEvent("FocusEvent", mFocusEnt));
}

void nsselection_system::set_picking_fbo(uint32 fbo)
{
	m_picking_buf = fbo;
}

void nsselection_system::set_final_fbo(uint32 fbo)
{
	m_final_buf = fbo;
}

uivec3 nsselection_system::pick(const fvec2 & mpos)
{
	return pick(mpos.x, mpos.y);
}

uivec3 nsselection_system::pick(float mousex, float mousey)
{
	nsscene * scene = nse.current_scene();
	if (scene == NULL)
		return uivec3();

	nsentity * cam = scene->camera();
	if (cam == NULL)
		return uivec3();

	nsfb_object * pickingBuf = nse.framebuffer(m_picking_buf);
	if (pickingBuf == NULL)
		return uivec3();

	nscam_comp * cc = cam->get<nscam_comp>();
	ivec2 dim = cc->screen_size();
	float mouseX = mousex * float(dim.w);
	float mouseY = mousey * float(dim.h);

	pickingBuf->set_target(nsfb_object::fb_read);
	pickingBuf->bind();
	pickingBuf->set_read_buffer(nsfb_object::attach_point(nsfb_object::att_color + nsgbuf_object::col_picking));

	uivec3 index;
	glReadPixels(int32(mouseX), int32(mouseY), 1, 1, GL_RGB_INTEGER, GL_UNSIGNED_INT, &index);
	GLError("nsselection_system::pick");

	pickingBuf->set_read_buffer(nsfb_object::att_none);
	pickingBuf->unbind();
	return index;
}

void nsselection_system::draw()
{
	nsscene * scene = nse.current_scene();
	if (scene == NULL || m_sel_shader == NULL)
		return;

	nsentity * cam = scene->camera();
	if (cam == NULL)
		return;

	nsfb_object * finalB = nse.framebuffer(m_final_buf);
	finalB->set_target(nsfb_object::fb_draw);
	finalB->bind();

	glDepthMask(GL_TRUE);
	glDisable(GL_DEPTH_TEST);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glClear(GL_STENCIL_BUFFER_BIT);
	glStencilFunc(GL_ALWAYS, 1, -1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	nstform_comp * camTComp = cam->get<nstform_comp>();
	nscam_comp * camc = cam->get<nscam_comp>();
	m_sel_shader->bind();
	m_sel_shader->set_proj_cam_mat(camc->proj_cam());

	// Go through and stencil each selected item
	auto iter = m_selected_ents.begin();
	while (iter != m_selected_ents.end())
	{
		nstform_comp * tComp = (*iter)->get<nstform_comp>();
		nssel_comp * selComp = (*iter)->get<nssel_comp>();
		nsrender_comp * renComp = (*iter)->get<nsrender_comp>();
		nsanim_comp * animComp = (*iter)->get<nsanim_comp>();
		nsterrain_comp * terComp = (*iter)->get<nsterrain_comp>();

		if (selComp->selected() && selComp->draw_enabled())
		{
			auto selIter = selComp->begin();
			while (selIter != selComp->end())
			{
				m_sel_shader->set_transform(tComp->transform(*selIter));
				nsmesh * rMesh = nse.resource<nsmesh>(renComp->mesh_id());
				for (uint32 i = 0; i < rMesh->count(); ++i)
				{
					nsmesh::submesh * cSub = rMesh->sub(i);

					m_sel_shader->set_heightmap_enabled(false);
					nsmaterial * mat = nse.resource<nsmaterial>(renComp->material_id(i));
					if (mat != NULL)
					{
						nstexture * tex = nse.resource<nstexture>(mat->map_tex_id(nsmaterial::height));
						if (tex != NULL)
						{
							m_sel_shader->set_heightmap_enabled(true);
							tex->enable(nsmaterial::height);
						}
					}

					if (cSub->node_ != NULL)
						m_sel_shader->set_node_transform(cSub->node_->world_transform);
					else
						m_sel_shader->set_node_transform(fmat4());

					if (animComp != NULL)
					{
						m_sel_shader->set_has_bones(true);
						m_sel_shader->set_bone_transform(*animComp->final_transforms());
					}
					else
						m_sel_shader->set_has_bones(false);

					if (terComp != NULL)
						m_sel_shader->set_height_minmax(terComp->height_bounds());

					cSub->vao.bind();
					glDrawElements(cSub->primitive_type,
								   static_cast<GLsizei>(cSub->indices.size()),
								   GL_UNSIGNED_INT,
								   0);
					cSub->vao.unbind();
				}
				++selIter;
			}
		}
		++iter;
	}

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glLineWidth(3.0f);
	iter = m_selected_ents.begin();
	while (iter != m_selected_ents.end())
	{
		nstform_comp * tComp = (*iter)->get<nstform_comp>();
		nssel_comp * selComp = (*iter)->get<nssel_comp>();
		nsrender_comp * renComp = (*iter)->get<nsrender_comp>();
		nsanim_comp * animComp = (*iter)->get<nsanim_comp>();
		nsterrain_comp * terComp = (*iter)->get<nsterrain_comp>();

		if (renComp == NULL || tComp == NULL)
		{
			++iter;
			continue;
		}

		if (selComp->selected() && selComp->draw_enabled())
		{
			auto selIter = selComp->begin();
			while (selIter != selComp->end())
			{
				m_sel_shader->set_transform(tComp->transform(*selIter));
				nsmesh * rMesh = nse.resource<nsmesh>(renComp->mesh_id());
				for (uint32 i = 0; i < rMesh->count(); ++i)
				{
					nsmesh::submesh * cSub = rMesh->sub(i);

					m_sel_shader->set_heightmap_enabled(false);
					nsmaterial * mat = nse.resource<nsmaterial>(renComp->material_id(i));
					if (mat != NULL)
					{
						nstexture * tex = nse.resource<nstexture>(mat->map_tex_id(nsmaterial::height));
						if (tex != NULL)
						{
							m_sel_shader->set_heightmap_enabled(true);
							tex->enable(nsmaterial::height);
						}
					}

					if (cSub->node_ != NULL)
						m_sel_shader->set_node_transform(cSub->node_->world_transform);
					else
						m_sel_shader->set_node_transform(fmat4());

					if (animComp != NULL)
					{
						m_sel_shader->set_has_bones(true);
						m_sel_shader->set_bone_transform(*animComp->final_transforms());
					}
					else
						m_sel_shader->set_has_bones(false);

					if (terComp != NULL)
						m_sel_shader->set_height_minmax(terComp->height_bounds());

					cSub->vao.bind();

					glDisable(GL_DEPTH_TEST);
					glPolygonMode(GL_FRONT, GL_LINE);
					glStencilFunc(GL_NOTEQUAL, 1, -1);
					glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

					fvec4 selCol = selComp->color();
					m_sel_shader->set_frag_color_out(selCol);
					glDrawElements(cSub->primitive_type,
								   static_cast<GLsizei>(cSub->indices.size()),
								   GL_UNSIGNED_INT,
								   0);

					glPolygonMode(GL_FRONT, GL_FILL);
					glEnable(GL_DEPTH_TEST);
					glStencilFunc(GL_EQUAL, 1, 0);
					selCol.w = selComp->mask_alpha();

					if (m_focus_ent.y == (*iter)->id() && m_focus_ent.z == *selIter)
						selCol.w = 0.4f;

					m_sel_shader->set_uniform("fragColOut", selCol);
					glDrawElements(cSub->primitive_type,
								   static_cast<GLsizei>(cSub->indices.size()),
								   GL_UNSIGNED_INT,
								   0);
					cSub->vao.unbind();
				}
				++selIter;
			}
		}

		_draw_ent_occ(*iter);
		++iter;
	}

	_draw_occ();
	_draw_hidden();


	// THIS SHOULD BE MOVED TO BUILD SYSTEM
	// Draw mirror mode center
	if (nse.system<nsbuild_system>()->mirror())
	{
		nsmesh * tileM = nse.core()->get<nsmesh>(MESH_FULL_TILE);
		fvec3 mypos = nse.system<nsbuild_system>()->mirror();
		mypos.z = nse.system<nsbuild_system>()->layer() * Z_GRID;
		m_sel_shader->set_transform(translation_mat4(mypos));
		for (uint32 i = 0; i < tileM->count(); ++i)
		{
			nsmesh::submesh * cSub = tileM->sub(i);

			if (cSub->node_ != NULL)
				m_sel_shader->set_node_transform(cSub->node_->world_transform);
			else
				m_sel_shader->set_node_transform(fmat4());

			m_sel_shader->set_has_bones(false);

			cSub->vao.bind();
			fvec4 col(1.0f, 0.0f, 1.0f, 0.7f);
			m_sel_shader->set_frag_color_out(col);
			glDrawElements(cSub->primitive_type,
						   static_cast<GLsizei>(cSub->indices.size()),
						   GL_UNSIGNED_INT,
						   0);
			cSub->vao.unbind();
		}
	}

	m_sel_shader->unbind();
	glLineWidth(1.0f);
}

void nsselection_system::_draw_ent_occ(nsentity * ent)
{
	// Draw the occupy component if draw enabled
	nsoccupy_comp * occComp = ent->get<nsoccupy_comp>();
	nssel_comp * selComp = ent->get<nssel_comp>();
	nstform_comp * tComp = ent->get<nstform_comp>();
	if (occComp != NULL && selComp != NULL && tComp != NULL && occComp->draw_enabled())
	{
		nsmesh * occMesh = nse.resource<nsmesh>(occComp->mesh_id());
		nsmaterial * mat = nse.resource<nsmaterial>(occComp->material_id());
		if (occMesh != NULL)
		{
			auto selIter = selComp->begin();
			while (selIter != selComp->end())
			{
				for (uint32 i = 0; i < occMesh->count(); ++i)
				{
					nsmesh::submesh * occSub = occMesh->sub(i);

					if (occSub->node_ != NULL)
						m_sel_shader->set_node_transform(occSub->node_->world_transform);
					else
						m_sel_shader->set_node_transform(fmat4());

					m_sel_shader->set_has_bones(false);
					occSub->vao.bind();

					glDisable(GL_STENCIL_TEST);

					if (mat != NULL)
						m_sel_shader->set_frag_color_out(mat->color());
					else
						m_sel_shader->set_frag_color_out(fvec4(1.0f, 0.0f, 1.0f, 0.5f));

					auto spaceIter = occComp->begin();
					while (spaceIter != occComp->end())
					{
						m_sel_shader->set_transform(translation_mat4(nstile_grid::world(*spaceIter, tComp->wpos(*selIter))));
						glDrawElements(occSub->primitive_type,
									   static_cast<GLsizei>(occSub->indices.size()),
									   GL_UNSIGNED_INT,
									   0);
						++spaceIter;
					}
					occSub->vao.unbind();
				}
				++selIter;
			}
		}
	}
}

void nsselection_system::_draw_occ()
{
	nsscene * scene = nse.current_scene();
	if (scene == NULL || m_sel_shader == NULL)
		return;

	if (!m_draw_occ)
		return;

	nsmesh * occMesh = nse.core()->get<nsmesh>(MESH_FULL_TILE);
	for (uint32 i = 0; i < occMesh->count(); ++i)
	{
		nsmesh::submesh * occSub = occMesh->sub(i);

		if (occSub->node_ != NULL)
			m_sel_shader->set_node_transform(occSub->node_->world_transform);
		else
			m_sel_shader->set_node_transform(fmat4());

		m_sel_shader->set_has_bones(false);
		occSub->pos_buf.bind();
		occSub->vao.vertex_attrib_ptr(nsshader::loc_position, 3, GL_FLOAT, GL_FALSE, sizeof(fvec3), 0);
		occSub->indice_buf.bind();

		m_sel_shader->set_frag_color_out(fvec4(1.0f, 0.0f, 0.0f, 1.0f));

		nstile_grid::grid_bounds g = scene->grid().occupied_bounds();


		for (int32 z = g.min_space.z; z <= g.max_space.z; ++z)
		{
			for (int32 y = g.min_space.y; y <= g.max_space.y; ++y)
			{
				for (int32 x = g.min_space.x; x <= g.max_space.x; ++x)
				{
					uivec3 id = scene->grid().get(ivec3(x, y, z));
					nsentity * ent = nse.resource<nsentity>(id.x, id.y);
					if (ent != NULL)
					{
						m_trans.set_column(3,fvec4(ent->get<nstform_comp>()->lpos(id.z),1.0f));
						m_sel_shader->set_transform(m_trans);
						glDrawElements(occSub->primitive_type,
									   static_cast<GLsizei>(occSub->indices.size()),
									   GL_UNSIGNED_INT,
									   0);
					}
				}
			}
		}
	}

}

void nsselection_system::_draw_hidden()
{
	nsscene * scene = nse.current_scene();
	if (scene == NULL || m_sel_shader == NULL)
		return;

	nsentity * cam = scene->camera();
	if (cam == NULL)
		return;

	// Draw all hidden objects - This should probably be moved to the render system or something - or to a forward
	// renderer when that gets set up so that we can draw transparent stuff
	entity_ptr_set sceneEnts = scene->entities();
	auto sceneEntIter = sceneEnts.begin();
	while (sceneEntIter != sceneEnts.end())
	{
		nstform_comp * tComp = (*sceneEntIter)->get<nstform_comp>();
		nsrender_comp * renComp = (*sceneEntIter)->get<nsrender_comp>();

		if (renComp == NULL)
		{
			++sceneEntIter;
			continue;
		}

		nsanim_comp * animComp = (*sceneEntIter)->get<nsanim_comp>();
		nsmesh * rMesh = nse.resource<nsmesh>(renComp->mesh_id());

		if (rMesh == NULL)
		{
			++sceneEntIter;
			continue;
		}

		for (uint32 index = 0; index < tComp->count(); ++index)
		{
			int32 state = tComp->hidden_state(index);

			bool layerBit = state & nstform_comp::hide_layer && true;
			bool objectBit = state & nstform_comp::hide_object && true;
			bool showBit = state & nstform_comp::hide_none && true;
			bool hideBit = state & nstform_comp::hide_all && true;

			if (!hideBit && (!layerBit && objectBit))
			{
				m_sel_shader->set_transform(tComp->transform(index));
				for (uint32 i = 0; i < rMesh->count(); ++i)
				{
					nsmesh::submesh * cSub = rMesh->sub(i);

					if (cSub->node_ != NULL)
						m_sel_shader->set_node_transform(cSub->node_->world_transform);
					else
						m_sel_shader->set_node_transform(fmat4());

					if (animComp != NULL)
					{
						m_sel_shader->set_has_bones(true);
						m_sel_shader->set_bone_transform(*animComp->final_transforms());
					}
					else
						m_sel_shader->set_has_bones(false);

					cSub->vao.bind();
					fvec4 col(1.0f, 1.0f, 1.0f, 0.04f);
					m_sel_shader->set_frag_color_out(col);
					glDrawElements(cSub->primitive_type,
								   static_cast<GLsizei>(cSub->indices.size()),
								   GL_UNSIGNED_INT,
								   0);
					cSub->vao.unbind();
				}
			}
		}
		++sceneEntIter;
	}
}

void nsselection_system::del()
{
	nsscene * scene = nse.current_scene();
	if (scene == NULL)
		return;

	std::vector<fvec3> posVec;

	auto iter = m_selected_ents.begin();
	while (iter != m_selected_ents.end())
	{
		nssel_comp * selC = (*iter)->get<nssel_comp>();
		nstform_comp * tComp = (*iter)->get<nstform_comp>();

		posVec.resize(selC->count());
		uint32 i = 0;
		auto iter2 = selC->begin();
		while (iter2 != selC->end())
		{
			posVec[i] = tComp->wpos(*iter2);
			++iter2;
			++i;
		}

		for (uint32 cur = 0; cur < posVec.size(); ++cur)
			scene->remove(posVec[cur]);

		++iter;
	}
	clear();
}

bool nsselection_system::empty()
{
	return m_selected_ents.empty();
}

void nsselection_system::enable_layer_mode(const bool & pMode)
{
	m_layer_mode = pMode;
}

void nsselection_system::init()
{
	//nse.events()->addListener(this, NSEvent::SelPick);
	//nse.events()->addListener(this, NSEvent::SelSet);
	//nse.events()->addListener(this, NSEvent::SelAdd);
	//nse.events()->addListener(this, NSEvent::ClearSelection);
	register_handler_func(this, &nsselection_system::_handle_action_event);
	register_handler_func(this, &nsselection_system::_handle_state_event);

	add_trigger_hash(selected_entity, NSSEL_SELECT);
	add_trigger_hash(multi_select, NSSEL_MULTISELECT);
	add_trigger_hash(shift_select, NSSEL_SHIFTSELECT);
	add_trigger_hash(move_select, NSSEL_MOVE);
	add_trigger_hash(move_selection_xy, NSSEL_MOVE_XY);
	add_trigger_hash(move_selection_zy, NSSEL_MOVE_ZY);
	add_trigger_hash(move_selection_zx, NSSEL_MOVE_ZX);
	add_trigger_hash(move_selection_x, NSSEL_MOVE_X);
	add_trigger_hash(move_selection_y, NSSEL_MOVE_Y);
	add_trigger_hash(move_selection_z, NSSEL_MOVE_Z);
	add_trigger_hash(move_selection_toggle, NSSEL_MOVE_TOGGLE);
}

int32 nsselection_system::draw_priority()
{
	return SEL_SYS_DRAW_PR;
}

int32 nsselection_system::update_priority()
{
	return SEL_SYS_UPDATE_PR;
}

void nsselection_system::_on_rotate_x(nsentity * ent, bool pPressed)
{
	if (ent == NULL)
		return;
	nssel_comp * sc = ent->get<nssel_comp>();
	if (sc->selected() && pPressed)
		rotate(ent, nstform_comp::dir_right, 45.0f);
}

void nsselection_system::_on_rotate_y(nsentity * ent, bool pPressed)
{
	if (ent == NULL)
		return;
	nssel_comp * sc = ent->get<nssel_comp>();
	if (sc->selected() && pPressed)
		rotate(ent, nstform_comp::dir_target, 45.0f);
}

void nsselection_system::_on_rotate_z(nsentity * ent, bool pPressed)
{
	if (ent == NULL)
		return;
	nssel_comp * sc = ent->get<nssel_comp>();
	if (sc->selected() && pPressed)
		rotate(ent, nstform_comp::dir_up, 45.0f);
}

void nsselection_system::_on_select(nsentity * ent, bool pPressed, const uivec3 & pID, bool pSnapZOnly)
{
	nsscene * scene = nse.current_scene();
	if (scene == NULL)
		return;

	nsentity * cam = scene->camera();
	if (cam == NULL)
		return;

	if (ent == NULL)
		return;

	nssel_comp * sc = ent->get<nssel_comp>();
	nstform_comp * tc = ent->get<nstform_comp>();

	if (pPressed)
	{
		if (ent->plugin_id() == pID.x && ent->id() == pID.y)
		{
			if (!sc->contains(pID.z))
				set(ent, pID.z);

			m_focus_ent = pID;
			if (!m_moving)
			{
				fvec3 originalPos = tc->wpos(m_focus_ent.z);
				m_cached_point = originalPos;
				remove_from_grid();
				m_moving = true;
			}

			//nse.events()->send(new NSSelFocusChangeEvent("FocusEvent", mFocusEnt));
		}
		else if (!contains(pID))
		{
			clear();
			m_focus_ent = uivec3();
			//nse.events()->send(new NSSelFocusChangeEvent("FocusEvent", mFocusEnt));
		}
	}
	else
	{
		if (sc->selected())
		{
			if (pSnapZOnly)
				snap_z(ent);
			else
				snap(ent);
		}
			

		if (m_moving)
		{		
			nsentity * ent = scene->entity(m_focus_ent.x, m_focus_ent.y);
			if (ent == NULL)
				return;
			if (!collision())
			{
				ent->get<nstform_comp>()->compute_transform(m_focus_ent.z);
				fvec3 pTranslate = m_cached_point - ent->get<nstform_comp>()->wpos(m_focus_ent.z);
				translate(pTranslate);
				set_color(fvec4(DEFAULT_SEL_R, DEFAULT_SEL_G, DEFAULT_SEL_B, DEFAULT_SEL_A));

				if (!add_to_grid())
				{
					dprint("nsselection_system::onSelect Error in resetting tiles to original grid position");
				}

				//nse.events()->send(new NSSelFocusChangeEvent("FocusEvent", mFocusEnt));
			}
			else
			{
				fvec3 pos = ent->get<nstform_comp>()->wpos(m_focus_ent.z);
				scene->grid().snap(pos);
				add_to_grid();
			}

			m_moving = false;
			m_cached_point = fvec3();
		}
	}
}

void nsselection_system::set_occupied_spaces(bool show)
{
	m_draw_occ = show;
}

void nsselection_system::_on_paint_select(nsentity * ent, const fvec2 & pPos)
{
	if (ent == NULL)
		return;

	nsscene * scene = nse.current_scene();
	if (scene == NULL)
		return;

	uivec3 pi = pick(pPos.x, pPos.y);
	nssel_comp * sc = ent->get<nssel_comp>();

	if (ent->plugin_id() == pi.x && ent->id() == pi.y) // needs the pointing thing
	{
		m_focus_ent = pi;
		//nse.events()->send(new NSSelFocusChangeEvent("FocusEvent", mFocusEnt));

		nsentity * tileBrush = nse.system<nsbuild_system>()->tile_brush();
		if (tileBrush == NULL)
		{
			add(ent, pi.z);
			return;
		}
		nstile_brush_comp * brushComp = tileBrush->get<nstile_brush_comp>();
		if (brushComp == NULL)
		{
			add(ent, pi.z);
			return;
		}

		auto brushIter = brushComp->begin();
		while (brushIter != brushComp->end())
		{
			for (int32 i = 0; i < brushComp->height(); ++i)
			{
				nsentity * focEnt = scene->entity(m_focus_ent.x, m_focus_ent.y);
				if (focEnt == NULL)
					continue;

				nstform_comp * tForm = focEnt->get<nstform_comp>();
				fvec3 pos = tForm->lpos(m_focus_ent.z) + nstile_grid::world(ivec3(brushIter->x, brushIter->y, -i)); // add in height when get working
				uivec3 refid = scene->ref_id(pos);
				nsentity * selEnt = nse.resource<nsentity>(refid.x, refid.y);
				if (selEnt == NULL)
					continue;
				nssel_comp * selComp = selEnt->get<nssel_comp>();
				if (selComp == NULL)
					continue;

				add(ent, refid.z);
			}
			++brushIter;
		}

	}
}

void nsselection_system::_on_draw_object(nsentity * ent, const fvec2 & pDelta, uint16 axis_)
{
	nsscene * scene = nse.current_scene();
	if (scene == NULL)
		return;

	nsentity * cam = scene->camera();
	if (cam == NULL)
		return;

	if (ent == NULL)
		return;

	nssel_comp * sc = ent->get<nssel_comp>();
	nstform_comp * camTForm = cam->get<nstform_comp>();
	nscam_comp * camc = cam->get<nscam_comp>();
	nstform_comp * tComp = ent->get<nstform_comp>();

	// get change in terms of NDC
	fvec2 delta(pDelta * 2.0f);

	fvec3 originalPos = tComp->wpos(m_focus_ent.z);
	fvec4 screenSpace = camc->proj_cam() * fvec4(originalPos, 1.0f);
	screenSpace /= screenSpace.w;
	screenSpace.x += delta.u;
	screenSpace.y += delta.v;

	fvec4 newPos = camc->inv_proj_cam() * screenSpace;
	newPos /= newPos.w;

	fvec3 fpos(newPos.xyz());
	fvec3 castVec = fpos - camTForm->wpos();
	castVec.normalize();
	float depth = 0.0f;
	fvec3 normal;

	fvec3 targetVec = (camc->focus_orientation() * camTForm->orientation()).target();
	fvec3 projVec = projectPlane(targetVec, fvec3(0.0f,0.0f,-1.0f));
	fvec2 projVecX = project(projVec.xy(), fvec2(1.0,0.0));
	
	float angle = targetVec.angle_to(projVec);
	float angleX = projVec.xy().angle_to(projVecX);


	// Set normal if not moving in a single plane
	if ((axis_ == (axis_x | axis_y | axis_z) && angle > 35.0f) || (axis_ & axis_z) != axis_z)
		normal.set(0.0f,0.0f,-1.0f);
	else
	{
		if (axis_ == axis_z || axis_ == (axis_x | axis_y | axis_z))
		{
			if (angleX < 45.0f)
				normal.set(-1.0f,0.0f,0.0f);
			else
				normal.set(0.0f,-1.0f,0.0f);
		}
		else
		{
			if ((axis_ & axis_x) != axis_x)
				normal.set(-1.0f,0.0f,0.0f);
			else
				normal.set(0.0f,-1.0f,0.0f);
		}
	}
	
	depth = (normal * (originalPos - fpos)) / (normal * castVec);
	fpos += castVec*depth;
	fpos -= originalPos;	
	fpos %= fvec3(float((axis_ & axis_x) == axis_x), float((axis_ & axis_y) == axis_y), float((axis_ & axis_z) == axis_z));
	m_total_frame_translation += fpos;
}

int32 nsselection_system::layer() const
{
	return m_layer;
}

const uivec3 & nsselection_system::center()
{
	return m_focus_ent;
}

void nsselection_system::_on_multi_select(nsentity * ent, bool pPressed, const uivec3 & pID)
{
	nsscene * scene = nse.current_scene();
	if (scene == NULL)
		return;

	if (ent == NULL)
		return;

	nssel_comp * sc = ent->get<nssel_comp>();


	if (pPressed)
	{
		nsentity * ent = sc->owner();
		if (ent->plugin_id() == pID.x && ent->id() == pID.y)
		{
			if (!contains(pID))
			{
				add(ent, pID.z);
				m_focus_ent = pID;
				//nse.events()->send(new NSSelFocusChangeEvent("FocusEvent", mFocusEnt));
			}
			else
			{
				remove(ent, pID.z);
				m_focus_ent = uivec3();
				if (!m_selected_ents.empty())
				{
					nsentity * ent = scene->entity(pID.x,pID.y);
					if (ent != NULL)
					{
						auto iter = m_selected_ents.find(ent);
						if (iter != m_selected_ents.end())
						{
							nssel_comp * selComp = (*iter)->get<nssel_comp>();
							auto first = selComp->begin();
							m_focus_ent.x = (*iter)->plugin_id();
							m_focus_ent.y = (*iter)->id();
							m_focus_ent.z = (*first);
						}
						else
						{
							auto entFirst = m_selected_ents.begin();
							nssel_comp * selComp = (*entFirst)->get<nssel_comp>();
							auto first = selComp->begin();
							m_focus_ent.x = (*entFirst)->plugin_id();
							m_focus_ent.y = (*entFirst)->id();
							m_focus_ent.z = (*first);
						}
					}
					else
					{
						auto entFirst = m_selected_ents.begin();
						nssel_comp * selComp = (*entFirst)->get<nssel_comp>();
						auto first = selComp->begin();
						m_focus_ent.y = (*entFirst)->plugin_id();
						m_focus_ent.y = (*entFirst)->id();
						m_focus_ent.z = (*first);
					}
				}
				//nse.events()->send(new NSSelFocusChangeEvent("FocusEvent", mFocusEnt));
			}
		}
	}
}

void nsselection_system::_reset_focus(const uivec3 & pickid)
{	
	if (m_selected_ents.empty())
	{
		m_focus_ent = uivec3();
		m_send_foc_event = true;
		return;
	}
	

	if (contains(pickid))
	{
		m_focus_ent = pickid;
		m_send_foc_event = true;
		return;
	}

	if (contains(m_focus_ent))
		return;

	nsentity * ent = nse.resource<nsentity>(pickid.xy());
	nssel_comp * sc;
	if (ent == NULL || (sc = ent->get<nssel_comp>()) == NULL)
		return;

	nstform_comp * tc = ent->get<nstform_comp>();
	fvec3 original_pos = tc->wpos(pickid.z);
	
	auto iter = m_selected_ents.find(ent);
	if (iter != m_selected_ents.end())
	{
		auto sel_iter = sc->begin();
		uint closest_tformid = *sel_iter;
		while (sel_iter != sc->end())
		{
			if ( (original_pos - tc->wpos(*sel_iter)).length_sq() <
				 (original_pos - tc->wpos(closest_tformid)).length_sq() )
				closest_tformid = *sel_iter;
			++sel_iter;
		}
		m_focus_ent.set(pickid.xy(),closest_tformid);
	}
	else
	{
		auto entFirst = m_selected_ents.begin();
		nssel_comp * selComp = (*entFirst)->get<nssel_comp>();
		auto first = selComp->begin();
		m_focus_ent.set((*entFirst)->full_id(), *first);
	}
	m_send_foc_event = true;
}

void nsselection_system::remove(nsentity * ent, uint32 pTFormID)
{
	if (ent == NULL)
		return;

	nssel_comp * selcomp = ent->get<nssel_comp>();
	if (selcomp == NULL)
		return;

	selcomp->remove(pTFormID);
	if (selcomp->empty())
	{
		auto iter = m_selected_ents.find(ent);
		m_selected_ents.erase(iter);
	}
}

void nsselection_system::remove_from_grid()
{
	nsscene * scene = nse.current_scene();
	if (scene == NULL)
		return;

	auto iter = m_selected_ents.begin();
	while (iter != m_selected_ents.end())
	{
		nssel_comp * selComp = (*iter)->get<nssel_comp>();
		nstform_comp * tComp = (*iter)->get<nstform_comp>();
		nsoccupy_comp * occComp = (*iter)->get<nsoccupy_comp>();

		if (occComp != NULL)
		{
			auto selIter = selComp->begin();
			while (selIter != selComp->end())
			{
				scene->grid().remove(occComp->spaces(), tComp->wpos(*selIter));
				++selIter;
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
		auto selIter = selComp->begin();
		while (selIter != selComp->end())
		{
			selComp->set_color(selComp->default_color());
			++selIter;
		}
		++iter;
	}
}

void nsselection_system::rotate(nsentity * ent, const fvec4 & axisangle)
{
	if (ent == NULL)
		return;

	nssel_comp * scomp = ent->get<nssel_comp>();
	nstform_comp * tcomp = ent->get<nstform_comp>();
	if (scomp == NULL || tcomp == NULL)
		return;

	auto selIter = scomp->begin();
	while (selIter != scomp->end())
	{
		tcomp->rotate(axisangle, *selIter);
		++selIter;
	}
}

void nsselection_system::rotate(const fvec4 & axisangle)
{
	auto iter = m_selected_ents.begin();
	while (iter != m_selected_ents.end())
	{
		rotate(*iter, axisangle);
		++iter;
	}
}

void nsselection_system::rotate(nsentity * ent, nstform_comp::dir_vec axis, float angle)
{
	if (ent == NULL)
		return;

	nssel_comp * scomp = ent->get<nssel_comp>();
	nstform_comp * tcomp = ent->get<nstform_comp>();
	if (scomp == NULL || tcomp == NULL)
		return;

	auto selIter = scomp->begin();
	while (selIter != scomp->end())
	{
		tcomp->rotate(axis, angle, *selIter);
		++selIter;
	}
}

void nsselection_system::rotate(nstform_comp::dir_vec axis, float angle)
{
	auto iter = m_selected_ents.begin();
	while (iter != m_selected_ents.end())
	{
		rotate(*iter, axis, angle);
		++iter;
	}
}

void nsselection_system::rotate(nsentity * ent, nstform_comp::world_axis axis, float angle)
{
	if (ent == NULL)
		return;

	nssel_comp * scomp = ent->get<nssel_comp>();
	nstform_comp * tcomp = ent->get<nstform_comp>();
	if (scomp == NULL || tcomp == NULL)
		return;

	auto selIter = scomp->begin();
	while (selIter != scomp->end())
	{
		tcomp->rotate(axis, angle, *selIter);
		++selIter;
	}
}

void nsselection_system::rotate(nstform_comp::world_axis axis, float angle)
{
	auto iter = m_selected_ents.begin();
	while (iter != m_selected_ents.end())
	{
		rotate(*iter, axis, angle);
		++iter;
	}
}

void nsselection_system::rotate(nsentity * ent, const fvec3 & euler)
{
	if (ent == NULL)
		return;

	nssel_comp * scomp = ent->get<nssel_comp>();
	nstform_comp * tcomp = ent->get<nstform_comp>();
	if (scomp == NULL || tcomp == NULL)
		return;

	auto selIter = scomp->begin();
	while (selIter != scomp->end())
	{
		tcomp->rotate(euler, *selIter);
		++selIter;
	}
}

void nsselection_system::rotate(const fvec3 & euler)
{
	auto iter = m_selected_ents.begin();
	while (iter != m_selected_ents.end())
	{
		rotate(*iter, euler);
		++iter;
	}
}

void nsselection_system::rotate(nsentity * ent, const fquat & orientation)
{
	if (ent == NULL)
		return;

	nssel_comp * scomp = ent->get<nssel_comp>();
	nstform_comp * tcomp = ent->get<nstform_comp>();
	if (scomp == NULL || tcomp == NULL)
		return;

	auto selIter = scomp->begin();
	while (selIter != scomp->end())
	{
		tcomp->rotate(orientation, *selIter);
		++selIter;
	}
}

void nsselection_system::rotate(const fquat & orientation)
{
	auto iter = m_selected_ents.begin();
	while (iter != m_selected_ents.end())
	{
		rotate(*iter, orientation);
		++iter;
	}
}

void nsselection_system::scale(nsentity * ent, const fvec3 & pAmount)
{
	if (ent == NULL)
		return;

	nssel_comp * scomp = ent->get<nssel_comp>();
	nstform_comp * tcomp = ent->get<nstform_comp>();
	if (scomp == NULL || tcomp == NULL)
		return;

	auto selIter = scomp->begin();
	while (selIter != scomp->end())
	{
		tcomp->scale(pAmount, *selIter);
		++selIter;
	}
}

void nsselection_system::scale(const fvec3 & pAmount)
{
	auto iter = m_selected_ents.begin();
	while (iter != m_selected_ents.end())
	{
		scale(*iter, pAmount);
		++iter;
	}
}

void nsselection_system::scale(nsentity * ent, float x, float y, float z)
{
	scale(ent, fvec3(x, y, z));
}

void nsselection_system::scale(float x, float y, float z)
{
	scale(fvec3(x, y, z));
}

bool nsselection_system::set(nsentity * ent, uint32 tformid)
{
	clear();
	return add(ent, tformid);
}

void nsselection_system::set_color(const fvec4 & pColor)
{
	auto iter = m_selected_ents.begin();
	while (iter != m_selected_ents.end())
	{
		nssel_comp * selComp = (*iter)->get<nssel_comp>();
		auto selIter = selComp->begin();
		while (selIter != selComp->end())
		{
			selComp->set_color(pColor);
			++selIter;
		}
		++iter;
	}
}

void nsselection_system::set_hidden_state(nstform_comp::h_state pState, bool pSet)
{
	auto iter = m_selected_ents.begin();
	while (iter != m_selected_ents.end())
	{
		nssel_comp * selComp = (*iter)->get<nssel_comp>();
		nstform_comp * tForm = (*iter)->get<nstform_comp>();

		auto selIter = selComp->begin();
		while (selIter != selComp->end())
		{
			tForm->set_hidden_state(pState, pSet, *selIter);
			++selIter;
		}
		++iter;
	}
}

void nsselection_system::set_layer(int32 pLayer)
{
	m_layer = pLayer;
}

void nsselection_system::set_shader(nsselection_shader * selShader_)
{
	m_sel_shader = selShader_;
}

void nsselection_system::snap(nsentity * ent)
{
	if (ent == NULL)
		return;

	nssel_comp * scomp = ent->get<nssel_comp>();
	nstform_comp * tcomp = ent->get<nstform_comp>();
	if (scomp == NULL || tcomp == NULL)
		return;

	auto selIter = scomp->begin();
	while (selIter != scomp->end())
	{
		tcomp->snap(*selIter);
		++selIter;
	}
}

void nsselection_system::snap()
{
	auto iter = m_selected_ents.begin();
	while (iter != m_selected_ents.end())
	{
		snap(*iter);
		++iter;
	}
}

void nsselection_system::snap_x(nsentity * ent)
{
	if (ent == NULL)
		return;

	nssel_comp * scomp = ent->get<nssel_comp>();
	nstform_comp * tcomp = ent->get<nstform_comp>();
	if (scomp == NULL || tcomp == NULL)
		return;

	auto selIter = scomp->begin();
	while (selIter != scomp->end())
	{
		tcomp->snap_x(*selIter);
		++selIter;
	}
}

void nsselection_system::snap_x()
{
	auto iter = m_selected_ents.begin();
	while (iter != m_selected_ents.end())
	{
		snap_x(*iter);
		++iter;
	}
}

void nsselection_system::snap_y(nsentity * ent)
{
	if (ent == NULL)
		return;

	nssel_comp * scomp = ent->get<nssel_comp>();
	nstform_comp * tcomp = ent->get<nstform_comp>();
	if (scomp == NULL || tcomp == NULL)
		return;

	auto selIter = scomp->begin();
	while (selIter != scomp->end())
	{
		tcomp->snap_y(*selIter);
		++selIter;
	}
}

void nsselection_system::snap_y()
{
	auto iter = m_selected_ents.begin();
	while (iter != m_selected_ents.end())
	{
		snap_y(*iter);
		++iter;
	}
}

void nsselection_system::snap_z(nsentity * ent)
{
	if (ent == NULL)
		return;

	nssel_comp * scomp = ent->get<nssel_comp>();
	nstform_comp * tcomp = ent->get<nstform_comp>();
	if (scomp == NULL || tcomp == NULL)
		return;

	auto selIter = scomp->begin();
	while (selIter != scomp->end())
	{
		tcomp->snap_z(*selIter);
		++selIter;
	}
}

void nsselection_system::snap_z()
{
	auto iter = m_selected_ents.begin();
	while (iter != m_selected_ents.end())
	{
		snap_z(*iter);
		++iter;
	}
}


void nsselection_system::tile_swap(nsentity * pNewTile)
{
	nsscene * scene = nse.current_scene();
	if (scene == NULL)
		return;

	if (pNewTile == NULL)
	{
		dprint("nsselection_system::swapTiles pNewTile is NULL");
		return;
	}
	if (!valid_tile_swap())
		return;

	std::vector<fvec3> posVec;

	auto iter = m_selected_ents.begin();
	while (iter != m_selected_ents.end())
	{
		nssel_comp * selC = (*iter)->get<nssel_comp>();
		nstform_comp * tComp = (*iter)->get<nstform_comp>();

		posVec.resize(selC->count());
		uint32 i = 0;
		auto iter2 = selC->begin();
		while (iter2 != selC->end())
		{
			posVec[i] = tComp->wpos(*iter2);
			++iter2;
			++i;
		}

		for (uint32 cur = 0; cur < posVec.size(); ++cur)
		{
			uivec3 id = scene->ref_id(posVec[cur]);
			scene->replace(id.x, id.y, id.z, pNewTile);
		}

		++iter;
	}
	clear();
}

void nsselection_system::translate(nsentity * ent, const fvec3 & pAmount)
{
	if (ent == NULL)
		return;

	nssel_comp * scomp = ent->get<nssel_comp>();
	nstform_comp * tcomp = ent->get<nstform_comp>();
	if (scomp == NULL || tcomp == NULL)
		return;

	auto selIter = scomp->begin();
	while (selIter != scomp->end())
	{
		tcomp->translate(pAmount, *selIter);
		++selIter;
	}
}

void nsselection_system::translate(const fvec3 & pAmount)
{
	auto iter = m_selected_ents.begin();
	while (iter != m_selected_ents.end())
	{
		translate(*iter, pAmount);
		++iter;
	}
}

void nsselection_system::translate(float x, float y, float z)
{
	translate(fvec3(x, y, z));
}

void nsselection_system::translate(nsentity * ent, float x, float y, float z)
{
	translate(ent, fvec3(x, y, z));
}

void nsselection_system::translate(nsentity * ent, nstform_comp::dir_vec pDir, float pAmount)
{
	if (ent == NULL)
		return;

	nssel_comp * scomp = ent->get<nssel_comp>();
	nstform_comp * tcomp = ent->get<nstform_comp>();
	if (scomp == NULL || tcomp == NULL)
		return;

	auto selIter = scomp->begin();
	while (selIter != scomp->end())
	{
		tcomp->translate(pDir, pAmount, *selIter);
		++selIter;
	}
}

void nsselection_system::translate(nstform_comp::dir_vec pDir, float pAmount)
{
	auto iter = m_selected_ents.begin();
	while (iter != m_selected_ents.end())
	{
		translate(*iter, pDir, pAmount);
		++iter;
	}
}

void nsselection_system::translate(nsentity * ent, nstform_comp::world_axis pDir, float pAmount)
{
	if (ent == NULL)
		return;

	nssel_comp * scomp = ent->get<nssel_comp>();
	nstform_comp * tcomp = ent->get<nstform_comp>();
	if (scomp == NULL || tcomp == NULL)
		return;

	auto selIter = scomp->begin();
	while (selIter != scomp->end())
	{
		tcomp->translate(pDir, pAmount, *selIter);
		++selIter;
	}
}

void nsselection_system::translate(nstform_comp::world_axis pDir, float pAmount)
{
	auto iter = m_selected_ents.begin();
	while (iter != m_selected_ents.end())
	{
		translate(*iter, pDir, pAmount);
		++iter;
	}
}

bool nsselection_system::layer_mode() const
{
	return m_layer_mode;
}

bool nsselection_system::valid_brush()
{
	if (m_selected_ents.empty())
		return false;

	auto iter = m_selected_ents.begin();
	while (iter != m_selected_ents.end())
	{
		if (!(*iter)->has<nstile_comp>())
			return false;

		nssel_comp * selComp = (*iter)->get<nssel_comp>();
		nstform_comp * tComp = (*iter)->get<nstform_comp>();

		int32 newZ = nstile_grid::index_z(tComp->wpos().z);

		auto selIter = selComp->begin();
		int32 z = 0;

		if (selIter != selComp->end())
			z = nstile_grid::index_z(tComp->wpos(*selIter).z);
		else
			return false;

		while (selIter != selComp->end())
		{
			int32 newZ = nstile_grid::index_z(tComp->wpos(*selIter).z);
			if (newZ != z)
				return false;
			++selIter;
		}
		++iter;
	}
	return true;
}

bool nsselection_system::valid_tile_swap()
{
	if (m_selected_ents.empty())
		return false;

	auto iter = m_selected_ents.begin();
	while (iter != m_selected_ents.end())
	{
		nssel_comp * selComp = (*iter)->get<nssel_comp>();
		nstform_comp * tComp = (*iter)->get<nstform_comp>();
		if (!(*iter)->has<nstile_comp>())
			return false;
		++iter;
	}
	return true;
}

void nsselection_system::update()
{

	nsscene * scn = nse.current_scene();
	if (scn == NULL)
		return;
	nsentity * ent = scn->entity(m_focus_ent.xy());
	if (ent == NULL)
		return;
	nstform_comp * foc_tform = ent->get<nstform_comp>();

	if (m_send_foc_event)
	{
		nse.event_dispatch()->push<nssel_focus_event>(m_focus_ent);
		m_send_foc_event = false;
	}

	if (m_toggle_move)
	{
		if (m_moving)
		{
			fvec3 originalPos = foc_tform->wpos(m_focus_ent.z);
			m_cached_point = originalPos;
			remove_from_grid();
		}
		else
		{
			snap();
			if (!collision())
			{
				foc_tform->compute_transform(m_focus_ent.z);
				fvec3 pTranslate = m_cached_point - foc_tform->wpos(m_focus_ent.z);
				translate(pTranslate);
				set_color(fvec4(DEFAULT_SEL_R, DEFAULT_SEL_G, DEFAULT_SEL_B, DEFAULT_SEL_A));

				if (!add_to_grid())
				{
					dprint("nsselection_system::onSelect Error in resetting tiles to original grid position");
				}
			}
			else
			{
				fvec3 pos = foc_tform->wpos(m_focus_ent.z);
				scn->grid().snap(pos);
				add_to_grid();
				nse.event_dispatch()->push<nssel_focus_event>(m_focus_ent);
			}
			m_cached_point = fvec3();
		}
		m_toggle_move = false;
	}

	if (m_moving)
	{		
		if (!collision())
			set_color(fvec4(1.0f, 0.0f, 0.0f, 1.0f));
		else
			reset_color();
		translate(m_total_frame_translation);
		m_total_frame_translation = 0.0f;
	}
}

bool nsselection_system::_handle_action_event(nsaction_event * evnt)
{
	if (evnt->trigger_hash_name == trigger_hash(selected_entity))
	{
		auto xpos_iter = evnt->axes.find(nsinput_map::axis_mouse_xpos);
		auto ypos_iter = evnt->axes.find(nsinput_map::axis_mouse_ypos);
		fvec2 mpos;
		
		if (xpos_iter != evnt->axes.end())
			mpos.x = xpos_iter->second;
		if (ypos_iter != evnt->axes.end())
			mpos.y = ypos_iter->second;
		
		uivec3 pickid = pick(mpos);
		if (!contains(pickid))
			clear();
		
		nsentity * selectedEnt = nse.resource<nsentity>(pickid.xy());
        add(selectedEnt, pickid.z);
		_reset_focus(pickid);
	}
	else if (evnt->trigger_hash_name == trigger_hash(multi_select))
	{
		auto xpos_iter = evnt->axes.find(nsinput_map::axis_mouse_xpos);
		auto ypos_iter = evnt->axes.find(nsinput_map::axis_mouse_ypos);
		fvec2 mpos;
		
		if (xpos_iter != evnt->axes.end())
			mpos.x = xpos_iter->second;
		if (ypos_iter != evnt->axes.end())
			mpos.y = ypos_iter->second;
		
		uivec3 pickid = pick(mpos);
		nsentity * selectedEnt = nse.resource<nsentity>(pickid.xy());
		nstform_comp * tc;
			
		if (contains(pickid))
			remove(selectedEnt,pickid.z);
		else
			add(selectedEnt, pickid.z);

		_reset_focus(pickid);
	}
	else if (evnt->trigger_hash_name == trigger_hash(shift_select))
	{
		auto xpos_iter = evnt->axes.find(nsinput_map::axis_mouse_xpos);
		auto ypos_iter = evnt->axes.find(nsinput_map::axis_mouse_ypos);
		fvec2 mpos;
		
		if (xpos_iter != evnt->axes.end())
			mpos.x = xpos_iter->second;
		if (ypos_iter != evnt->axes.end())
			mpos.y = ypos_iter->second;
		
		uivec3 pickid = pick(mpos);
		nsentity * selectedEnt = nse.resource<nsentity>(pickid.xy());
		add(selectedEnt, pickid.z);
		_reset_focus(pickid);
	}
	else if (evnt->trigger_hash_name == trigger_hash(move_select))
	{
		auto xdelta_iter = evnt->axes.find(nsinput_map::axis_mouse_xdelta);
		auto ydelta_iter = evnt->axes.find(nsinput_map::axis_mouse_ydelta);
		fvec2 mdelta;
		
		if (xdelta_iter != evnt->axes.end())
			mdelta.x = xdelta_iter->second;
		if (ydelta_iter != evnt->axes.end())
			mdelta.y = ydelta_iter->second;

		nsentity * ent = nse.resource<nsentity>(m_focus_ent.xy());
		_on_draw_object(ent, mdelta, axis_x | axis_y | axis_z);
	}
	else if (evnt->trigger_hash_name == trigger_hash(move_selection_xy))
	{
		auto xdelta_iter = evnt->axes.find(nsinput_map::axis_mouse_xdelta);
		auto ydelta_iter = evnt->axes.find(nsinput_map::axis_mouse_ydelta);
		fvec2 mdelta;
		
		if (xdelta_iter != evnt->axes.end())
			mdelta.x = xdelta_iter->second;
		if (ydelta_iter != evnt->axes.end())
			mdelta.y = ydelta_iter->second;

		nsentity * ent = nse.resource<nsentity>(m_focus_ent.xy());
		_on_draw_object(ent, mdelta, axis_x | axis_y);
	}
	else if (evnt->trigger_hash_name == trigger_hash(move_selection_zy))
	{
		auto xdelta_iter = evnt->axes.find(nsinput_map::axis_mouse_xdelta);
		auto ydelta_iter = evnt->axes.find(nsinput_map::axis_mouse_ydelta);
		fvec2 mdelta;
		
		if (xdelta_iter != evnt->axes.end())
			mdelta.x = xdelta_iter->second;
		if (ydelta_iter != evnt->axes.end())
			mdelta.y = ydelta_iter->second;
		
		nsentity * ent = nse.resource<nsentity>(m_focus_ent.xy());
		_on_draw_object(ent, mdelta, axis_y | axis_z);
	}
	else if (evnt->trigger_hash_name == trigger_hash(move_selection_zx))
	{
		auto xdelta_iter = evnt->axes.find(nsinput_map::axis_mouse_xdelta);
		auto ydelta_iter = evnt->axes.find(nsinput_map::axis_mouse_ydelta);
		fvec2 mdelta;
		
		if (xdelta_iter != evnt->axes.end())
			mdelta.x = xdelta_iter->second;
		if (ydelta_iter != evnt->axes.end())
			mdelta.y = ydelta_iter->second;

		nsentity * ent = nse.resource<nsentity>(m_focus_ent.xy());
		_on_draw_object(ent, mdelta, axis_x | axis_z);
	}
	else if (evnt->trigger_hash_name == trigger_hash(move_selection_x))
	{
		auto xdelta_iter = evnt->axes.find(nsinput_map::axis_mouse_xdelta);
		auto ydelta_iter = evnt->axes.find(nsinput_map::axis_mouse_ydelta);
		fvec2 mdelta;
		
		if (xdelta_iter != evnt->axes.end())
			mdelta.x = xdelta_iter->second;
		if (ydelta_iter != evnt->axes.end())
			mdelta.y = ydelta_iter->second;

		nsentity * ent = nse.resource<nsentity>(m_focus_ent.xy());
		_on_draw_object(ent, mdelta, axis_x);
	}
	else if (evnt->trigger_hash_name == trigger_hash(move_selection_y))
	{
		auto xdelta_iter = evnt->axes.find(nsinput_map::axis_mouse_xdelta);
		auto ydelta_iter = evnt->axes.find(nsinput_map::axis_mouse_ydelta);
		fvec2 mdelta;
		
		if (xdelta_iter != evnt->axes.end())
			mdelta.x = xdelta_iter->second;
		if (ydelta_iter != evnt->axes.end())
			mdelta.y = ydelta_iter->second;

		nsentity * ent = nse.resource<nsentity>(m_focus_ent.xy());
		_on_draw_object(ent, mdelta, axis_y);
	}
	else if (evnt->trigger_hash_name == trigger_hash(move_selection_z))
	{
		auto xdelta_iter = evnt->axes.find(nsinput_map::axis_mouse_xdelta);
		auto ydelta_iter = evnt->axes.find(nsinput_map::axis_mouse_ydelta);
		fvec2 mdelta;
		
		if (xdelta_iter != evnt->axes.end())
			mdelta.x = xdelta_iter->second;
		if (ydelta_iter != evnt->axes.end())
			mdelta.y = ydelta_iter->second;

		nsentity * ent = nse.resource<nsentity>(m_focus_ent.xy());
		_on_draw_object(ent, mdelta, axis_z);
	}
	return true;	
}

bool nsselection_system::_handle_state_event(nsstate_event * evnt)
{
	if (evnt->trigger_hash_name == trigger_hash(move_selection_toggle))
	{
		m_toggle_move = true;
		m_moving = evnt->toggle;
	}
	return true;
}
