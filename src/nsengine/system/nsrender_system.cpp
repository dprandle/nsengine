/*! 
  \file nsrendersystem.cpp
	
  \brief Definition file for nsrender_system class

  This file contains all of the neccessary definitions for the nsrender_system class.

  \author Daniel Randle
  \date November 23 2013
  \copywrite Earth Banana Games 2013
*/

#include <nsopengl_driver.h>
#include <nsengine.h>
#include <nsplugin_manager.h>
#include <nsshader_manager.h>
#include <nsrender_system.h>
#include <nsvideo_driver.h>
#include <nscam_comp.h>
#include <nsscene.h>
#include <nsplugin.h>
#include <nsmesh.h>
#include <nstexture.h>
#include <nsmaterial.h>
#include <nsshader.h>
#include <nsfb_object.h>
#include <nsui_system.h>

#include <nsanim_comp.h>
#include <nsterrain_comp.h>
#include <nssel_comp.h>
#include <nsrender_comp.h>
#include <nslight_comp.h>


nsrender_system::nsrender_system() :
	nssystem(),
	m_default_mat(nullptr),
	m_current_vp(nullptr)
{
	m_all_draw_calls.reserve(MAX_INSTANCED_DRAW_CALLS);
	m_light_draw_calls.reserve(MAX_LIGHTS_IN_SCENE);
}

nsrender_system::~nsrender_system()
{
	clear_render_queues();	
}

nsmaterial * nsrender_system::default_mat()
{
	return m_default_mat;
}

void nsrender_system::init()
{
	create_default_render_queues();
	register_handler(nsrender_system::_handle_window_resize);
	register_action_handler(nsrender_system::_handle_viewport_change, VIEWPORT_CHANGE);
}

void nsrender_system::set_default_mat(nsmaterial * pDefMat)
{
	m_default_mat = pDefMat;
}

void nsrender_system::update()
{
	nsscene * scn = current_scene();
	if (scn != nullptr)
	{		
		_prepare_tforms(scn);
		m_mat_shader_ids.clear();
		m_all_draw_calls.resize(0);
		m_light_draw_calls.resize(0);
		push_scene(scn);
	}	
}

void nsrender_system::render()
{
	auto iter = vp_list.begin();
	while (iter != vp_list.end())
	{
		nse.video_driver()->render(iter->vp);
		++iter;
	}
}

void nsrender_system::push_scene(nsscene * scn)
{
	_add_draw_calls_from_scene(scn);
	_add_lights_from_scene(scn);
}

int32 nsrender_system::update_priority()
{
	return RENDER_SYS_UPDATE_PR;
}

bool nsrender_system::insert_viewport(const nsstring & vp_name, const nsrender::viewport & vp, const nsstring & insert_before)
{
	if (viewport(vp_name) != nullptr)
	{
		dprint("nsrender_system::insert_viewport Cannot insert two viewpots with the same name - " + vp_name);
		return false;
	}
	
	if (insert_before.empty())
	{
		if (vp_name.empty())
			return false;
		
		m_current_vp = new nsrender::viewport(vp);
		vp_list.push_back(vp_node(vp_name, m_current_vp));
		return true;
	}
	else
	{
		auto liter = vp_list.begin();
		while (liter != vp_list.end())
		{
			if (liter->vp_name == insert_before)
			{
				m_current_vp = new nsrender::viewport(vp);
				vp_list.insert(liter, vp_node(vp_name, m_current_vp));
				return true;
			}
			++liter;
		}
		return false;
	}
}

bool nsrender_system::remove_viewport(const nsstring & vp_name)
{
	auto liter = vp_list.begin();
	while (liter != vp_list.end())
	{
		if (liter->vp_name == vp_name)
		{
			if (m_current_vp == liter->vp)
				m_current_vp = nullptr;
			
			delete liter->vp;
			liter->vp = nullptr;
			vp_list.erase(liter);

			if (m_current_vp == nullptr && !vp_list.empty())
				m_current_vp = vp_list.back().vp;
			
			return true;
		}
		++liter;
	}
	return false;
}

nsrender::viewport * nsrender_system::viewport(const nsstring & vp_name)
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

void nsrender_system::move_viewport_forward(const nsstring & vp_name)
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

void nsrender_system::move_viewport_back(const nsstring & vp_name)
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

void nsrender_system::move_viewport_to_back(const nsstring & vp_name)
{
	if (vp_list.front().vp_name == vp_name)
		return;

	nsrender::viewport * vp = viewport(vp_name);
	if (vp != nullptr)
	{
		nsrender::viewport vp_copy(*vp);
		remove_viewport(vp_name);
		insert_viewport(vp_name, vp_copy, vp_list.front().vp_name);
	}
}

nsrender::viewport * nsrender_system::front_viewport(const fvec2 & screen_pos)
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


// system takes ownership and will delete on shutdown
bool nsrender_system::add_queue(const nsstring & name, drawcall_queue * q)
{
	return m_render_queues.emplace(name, q).second;	
}

void nsrender_system::clear_render_queues()
{
	while (m_render_queues.begin() != m_render_queues.end())
		destroy_queue(m_render_queues.begin()->first);
	m_render_queues.clear();
}

drawcall_queue * nsrender_system::create_queue(const nsstring & name)
{
	drawcall_queue * q = new drawcall_queue;
	if (!add_queue(name,q))
	{
		delete q;
		q = nullptr;
	}
	return q;
}

void nsrender_system::destroy_queue(const nsstring & name)
{
	drawcall_queue * q = remove_queue(name);
	delete q;		
}

drawcall_queue * nsrender_system::queue(const nsstring & name)
{
	auto iter = m_render_queues.find(name);
	if (iter != m_render_queues.end())
		return iter->second;
	return nullptr;	
}

drawcall_queue * nsrender_system::remove_queue(const nsstring & name)
{
	drawcall_queue * q = nullptr;
	auto iter = m_render_queues.find(name);
	if (iter != m_render_queues.end())
	{
		q = iter->second;
		m_render_queues.erase(iter);
	}
	return q;
}

void nsrender_system::create_default_render_queues()
{
	create_queue(SCENE_OPAQUE_QUEUE)->reserve(MAX_GBUFFER_DRAWS);
	create_queue(SCENE_TRANSLUCENT_QUEUE)->reserve(MAX_GBUFFER_DRAWS);
	create_queue(SCENE_SELECTION_QUEUE)->reserve(MAX_GBUFFER_DRAWS);
	create_queue(DIR_LIGHT_QUEUE)->reserve(MAX_LIGHTS_IN_SCENE);
	create_queue(SPOT_LIGHT_QUEUE)->reserve(MAX_LIGHTS_IN_SCENE);
	create_queue(POINT_LIGHT_QUEUE)->reserve(MAX_LIGHTS_IN_SCENE);
	create_queue(UI_RENDER_QUEUE)->reserve(MAX_UI_DRAW_CALLS);
}

uivec3 nsrender_system::pick(const fvec2 & screen_pos)
{
	return nse.video_driver()->pick(screen_pos);
}

void nsrender_system::move_viewport_to_front(const nsstring & vp_name)
{
	if (vp_list.back().vp_name == vp_name)
		return;

	nsrender::viewport * vp = viewport(vp_name);
	if (vp != nullptr)
	{
		nsrender::viewport vp_copy(*vp);
		remove_viewport(vp_name);
		insert_viewport(vp_name, vp_copy);
	}
}

bool nsrender_system::_handle_window_resize(window_resize_event * evt)
{
	auto iter = vp_list.begin();
	while (iter != vp_list.end())
	{
		if (iter->vp->window_tag == evt->window_tag && iter->vp->normalized_bounds != fvec4(0.0f))
		{
			iter->vp->bounds.x = iter->vp->normalized_bounds.x * evt->new_size.x;
			iter->vp->bounds.y = iter->vp->normalized_bounds.y * evt->new_size.y;
            iter->vp->bounds.z = iter->vp->normalized_bounds.z * evt->new_size.x;
			iter->vp->bounds.w = iter->vp->normalized_bounds.w * evt->new_size.y;
			if (iter->vp->camera != nullptr)
			{
				nscam_comp * cc = iter->vp->camera->get<nscam_comp>();
				cc->resize_screen(iter->vp->bounds.zw() - iter->vp->bounds.xy());
			}
		}
		++iter;
	}
	nse.video_driver()->resize_screen(evt->new_size);
	return true;
}

nsrender::viewport * nsrender_system::current_viewport()
{
	return m_current_vp;
}

bool nsrender_system::_handle_viewport_change(nsaction_event * evt)
{
	nsrender::viewport * vp = front_viewport(evt->norm_mpos);
	if (vp != nullptr)
		m_current_vp = vp;
	return true;
}

vp_node::vp_node(const nsstring & vp_name_, nsrender::viewport * vp_):
	vp_name(vp_name_),
	vp(vp_)
{}

vp_node::~vp_node()
{}


void nsrender_system::_add_lights_from_scene(nsscene * scene)
{
	auto l_iter = scene->entities<nslight_comp>().begin();
	drawcall_queue * dc_dq = queue(DIR_LIGHT_QUEUE);
	drawcall_queue * dc_sq = queue(SPOT_LIGHT_QUEUE);
	drawcall_queue * dc_pq = queue(POINT_LIGHT_QUEUE);

	dc_dq->resize(0);
	dc_sq->resize(0);
	dc_pq->resize(0);

	while (l_iter != scene->entities<nslight_comp>().end())
	{
		nslight_comp * lcomp = (*l_iter)->get<nslight_comp>();
		nstform_comp * tcomp = (*l_iter)->get<nstform_comp>();
		nsmesh * boundingMesh = get_resource<nsmesh>(lcomp->mesh_id());
		
		m_light_draw_calls.resize(m_light_draw_calls.size()+1);

		fmat4 proj;
		if (lcomp->get_light_type() == nslight_comp::l_spot)
			proj = perspective(2*lcomp->angle(), 1.0f, lcomp->shadow_clipping().x, lcomp->shadow_clipping().y);
		else if(lcomp->get_light_type() == nslight_comp::l_point)
			proj = perspective(90.0f, 1.0f, lcomp->shadow_clipping().x, lcomp->shadow_clipping().y);
		else
			proj = ortho(-100.0f,100.0f,100.0f,-100.0f,100.0f,-100.0f);

		for (uint32 i = 0; i < lcomp->transform_count(); ++i)
		{
			light_draw_call * ldc = &m_light_draw_calls[m_light_draw_calls.size()-1];

			ldc->submesh = nullptr;
			ldc->bg_color = scene->bg_color();
			ldc->direction = lcomp->transform(i).target();
			ldc->diffuse_intensity = lcomp->intensity().x;
			ldc->ambient_intensity = lcomp->intensity().y;
			ldc->cast_shadows = lcomp->cast_shadows();
			ldc->light_color = lcomp->color();
			ldc->light_type = lcomp->get_light_type();
			ldc->shadow_samples = lcomp->shadow_samples();
			ldc->shadow_darkness = lcomp->shadow_darkness();
			ldc->material_ids = &m_mat_shader_ids;
			ldc->spot_atten = lcomp->atten();
			ldc->light_pos = tcomp->wpos(i);
			ldc->light_transform = lcomp->transform(i);
			ldc->max_depth = lcomp->shadow_clipping().y - lcomp->shadow_clipping().x;
			if (lcomp->get_light_type() == nslight_comp::l_dir)
			{
				ldc->proj_light_mat = proj * lcomp->pov(i);
				ldc->shdr = nse.core()->get<nsshader>(DIR_LIGHT_SHADER);
				dc_dq->push_back(ldc);
			}
			else if (lcomp->get_light_type() == nslight_comp::l_spot)
			{
				ldc->shdr = nse.core()->get<nsshader>(SPOT_LIGHT_SHADER);
				ldc->proj_light_mat = proj * lcomp->pov(i);
				if (boundingMesh != nullptr)
				{
					for (uint32 i = 0; i < boundingMesh->count(); ++i)
					{
						ldc->submesh = boundingMesh->sub(i);
						dc_sq->push_back(ldc);
					}
				}
			}
			else
			{
				ldc->shdr = nse.core()->get<nsshader>(POINT_LIGHT_SHADER);
				ldc->proj_light_mat = proj;
				if (boundingMesh != nullptr)
				{
					for (uint32 i = 0; i < boundingMesh->count(); ++i)
					{
						ldc->submesh = boundingMesh->sub(i);
						dc_pq->push_back(ldc);
					}
				}
			}
		}
		++l_iter;
	}
}

void nsrender_system::_add_draw_calls_from_scene(nsscene * scene)
{
	nsrender::viewport * vp = nse.system<nsrender_system>()->current_viewport();
	if (vp == nullptr)
		return;

	nsentity * camera = vp->camera;
	if (camera == NULL)
		return;

	// update render components and the draw call list
	drawcall_queue * scene_dcq = queue(SCENE_OPAQUE_QUEUE);
	drawcall_queue * scene_tdcq = queue(SCENE_TRANSLUCENT_QUEUE);
	drawcall_queue * scene_sel = queue(SCENE_SELECTION_QUEUE);

	scene_dcq->resize(0);
	scene_tdcq->resize(0);
	scene_sel->resize(0);
	
	nscam_comp * cc = camera->get<nscam_comp>();	
	fvec2 terh;
	nsrender_comp * rComp = nullptr;
	nstform_comp * tComp = nullptr;
	nsanim_comp * animComp = nullptr;
	nsterrain_comp * terComp = nullptr;
	nsmesh * currentMesh = nullptr;
	nslight_comp * lc = nullptr;
	nssel_comp * sc = nullptr;
	bool transparent_picking = false;
	uint32 mat_id = 0;
	auto iter = scene->entities<nsrender_comp>().begin();
	while (iter != scene->entities<nsrender_comp>().end())
	{
		terh.set(0.0f, 1.0f);
		rComp = (*iter)->get<nsrender_comp>();
		tComp = (*iter)->get<nstform_comp>();
		lc = (*iter)->get<nslight_comp>();
		sc = (*iter)->get<nssel_comp>();
		animComp = (*iter)->get<nsanim_comp>();
		terComp = (*iter)->get<nsterrain_comp>();
		currentMesh = get_resource<nsmesh>(rComp->mesh_id());
		
		if (lc != nullptr)
		{
			if (lc->update_posted())
				lc->post_update(false);
		}

		if (rComp->update_posted())
			rComp->post_update(false);

		if (currentMesh == nullptr)
		{
			++iter;
			continue;
		}

		if (sc != nullptr)
		{
			transparent_picking = sc->transparent_picking_enabled();
		}
		
		nsmesh::submesh * mSMesh = nullptr;
		nsmaterial * mat = nullptr;
		fmat4_vector * fTForms = nullptr;
		nsshader * shader = nullptr;
		for (uint32 i = 0; i < currentMesh->count(); ++i)
		{
			mSMesh = currentMesh->sub(i);
			mat = get_resource<nsmaterial>(rComp->material_id(i));

			if (mat == nullptr)
				mat = nse.system<nsrender_system>()->default_mat();

			shader = get_resource<nsshader>(mat->shader_id());
			fTForms = nullptr;

			if (shader == nullptr)
			{
				if (mat->wireframe())
					shader = m_shaders.deflt_wireframe;
				else if (mat->alpha_blend())
					shader = m_shaders.deflt_translucent;
				else
					shader = m_shaders.deflt;
			}

			if (animComp != nullptr)
				fTForms = animComp->final_transforms();

			if (terComp != nullptr)
				terh = terComp->height_bounds();

			
			if (tComp != nullptr)
			{
				m_all_draw_calls.resize(m_all_draw_calls.size()+1);
				instanced_draw_call * dc = &m_all_draw_calls[m_all_draw_calls.size()-1];
				dc->submesh = mSMesh;
				dc->transform_buffer = tComp->transform_buffer();
				dc->transform_id_buffer = tComp->transform_id_buffer();
				dc->anim_transforms = fTForms;
				dc->height_minmax = terh;
				dc->entity_id = (*iter)->id();
				dc->plugin_id = (*iter)->plugin_id();
				dc->transform_count = tComp->visible_count();
				dc->casts_shadows = rComp->cast_shadow();
				dc->transparent_picking = false;
				dc->mat_index = mat_id;
				dc->mat = mat;
				dc->shdr = shader;
				if (mat->alpha_blend())
				{
					dc->transparent_picking = transparent_picking;
					scene_tdcq->push_back(dc);
				}
				else
				{
					scene_dcq->push_back(dc);
				}

				if (sc != nullptr && sc->selected())
				{
					m_all_draw_calls.resize(m_all_draw_calls.size()+1);
					instanced_draw_call * sel_dc = &m_all_draw_calls[m_all_draw_calls.size()-1];
					sel_dc->submesh = mSMesh;
					sel_dc->transform_buffer = sc->transform_buffer();
					sel_dc->transform_id_buffer = nullptr;
					sel_dc->anim_transforms = fTForms;
					sel_dc->height_minmax = terh;
					sel_dc->transform_count = sc->count();
					sel_dc->shdr = m_shaders.sel_shader;
					sel_dc->mat = mat;
					sel_dc->sel_color = sc->color();
					scene_sel->push_back(sel_dc);
				}

				auto inserted = m_mat_shader_ids.emplace(mat, mat_id);
				if (inserted.second)
					++mat_id;
			}
 		}
		
		++iter;
	}
}

void nsrender_system::_prepare_tforms(nsscene * scene)
{
	auto ent_iter = scene->entities().begin();
	while (ent_iter != scene->entities().end())
	{
		nstform_comp * tForm = (*ent_iter)->get<nstform_comp>();
		if (tForm->update_posted())
		{
			nsbuffer_object & tFormBuf = *tForm->transform_buffer();
			nsbuffer_object & tFormIDBuf = *tForm->transform_id_buffer();

			if (tForm->buffer_resized())
			{
				tFormBuf.bind();
				tFormBuf.allocate<fmat4>(nsbuffer_object::mutable_dynamic_draw, tForm->count());
				tFormIDBuf.bind();
				tFormIDBuf.allocate<uint32>(nsbuffer_object::mutable_dynamic_draw, tForm->count());
			}

			tFormBuf.bind();
			fmat4 * mappedT = tFormBuf.map<fmat4>(0, tForm->count(),
											   nsbuffer_object::access_map_range(nsbuffer_object::map_write));
			tFormIDBuf.bind();
			uint32 * mappedI = tFormIDBuf.map<uint32>(0, tForm->count(), nsbuffer_object::access_map_range(nsbuffer_object::map_write));
			tFormIDBuf.unbind();

			uint32 visibleCount = 0;
			for (uint32 i = 0; i < tForm->count(); ++i)
			{
				if (tForm->transform_update(i))
				{
					uivec3 parentID = tForm->parent_id(i);
					if (parentID != 0)
					{
						nsentity * ent = scene->get_entity(parentID.x, parentID.y);
						if (ent != nullptr)
						{
							nstform_comp * tComp2 = ent->get<nstform_comp>();
							if (parentID.z < tComp2->count())
								tForm->set_parent(tComp2->transform(parentID.z));
						}
					}

					tForm->compute_transform(i);
					tForm->set_instance_update(false, i);
				}
				int32 state = tForm->hidden_state(i);

				bool layerBit = state & nstform_comp::hide_layer && true;
				bool objectBit = state & nstform_comp::hide_object && true;
				bool showBit = state & nstform_comp::hide_none && true;
				bool hideBit = state & nstform_comp::hide_all && true;

				if (!hideBit && (!layerBit && (showBit || !objectBit)))
				{
					mappedT[visibleCount] = tForm->transform(i);
					mappedI[visibleCount] = i;
					++visibleCount;
				}
			}
			tForm->set_visible_instance_count(visibleCount);


			tFormBuf.bind();
			tFormBuf.unmap();
			tFormIDBuf.bind();
			tFormIDBuf.unmap();
			tFormIDBuf.unbind();
			tForm->post_update(tForm->buffer_resized());
			tForm->set_buffer_resize(false);
		}
		++ent_iter;
	}
}

const render_shaders & nsrender_system::get_render_shaders()
{
	return m_shaders;
}

void nsrender_system::set_render_shaders(const render_shaders & rs)
{
	m_shaders = rs;
}


std::list<vp_node> * nsrender_system::viewports()
{
	return &vp_list;
}
