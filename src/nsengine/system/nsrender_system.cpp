/*! 
  \file nsrendersystem.cpp
	
  \brief Definition file for nsrender_system class

  This file contains all of the neccessary definitions for the nsrender_system class.

  \author Daniel Randle
  \date November 23 2013
  \copywrite Earth Banana Games 2013
*/

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

nsrender_system::nsrender_system() :
	nssystem(),
	m_default_mat(nullptr),
	m_current_vp(nullptr)
{}

nsrender_system::~nsrender_system()
{}

nsmaterial * nsrender_system::default_mat()
{
	return m_default_mat;
}

void nsrender_system::init()
{
	register_handler(nsrender_system::_handle_window_resize);
	register_action_handler(nsrender_system::_handle_viewport_change, VIEWPORT_CHANGE);

	nsplugin * cplg = nse.core();	
		
	// Default material
	nstexture * tex = cplg->load<nstex2d>(nsstring(DEFAULT_MATERIAL) + nsstring(DEFAULT_TEX_EXTENSION));
	m_default_mat = cplg->load<nsmaterial>(nsstring(DEFAULT_MATERIAL) + nsstring(DEFAULT_MAT_EXTENSION));

    nsstring shext = nsstring(DEFAULT_SHADER_EXTENSION);
	render_shaders rs;
	rs.deflt = cplg->load<nsmaterial_shader>(nsstring(GBUFFER_SHADER) + shext);
	rs.deflt_wireframe = cplg->load<nsmaterial_shader>(nsstring(GBUFFER_WF_SHADER) + shext);
	rs.deflt_translucent = cplg->load<nsmaterial_shader>(nsstring(GBUFFER_TRANS_SHADER) + shext);
	rs.light_stencil = cplg->load<nslight_stencil_shader>(nsstring(LIGHTSTENCIL_SHADER) + shext);
	rs.frag_sort = cplg->load<nsfragment_sort_shader>(nsstring(FRAGMENT_SORT_SHADER) + shext);
	rs.dir_light = cplg->load<nsdir_light_shader>(nsstring(DIR_LIGHT_SHADER) + shext);
	rs.point_light = cplg->load<nspoint_light_shader>(nsstring(POINT_LIGHT_SHADER) + shext);
	rs.spot_light = cplg->load<nsspot_light_shader>(nsstring(SPOT_LIGHT_SHADER) + shext);
	rs.shadow_cube = cplg->load<nsshadow_cubemap_shader>(nsstring(POINT_SHADOWMAP_SHADER) + shext);
	rs.shadow_2d = cplg->load<nsshadow_2dmap_shader>(nsstring(SPOT_SHADOWMAP_SHADER) + shext);
	rs.sel_shader = cplg->load<nsselection_shader>(nsstring(SELECTION_SHADER) + shext);
	rs.deflt_particle = cplg->load<nsparticle_render_shader>(nsstring(RENDER_PARTICLE_SHADER) + shext);
	cplg->load<nsmaterial_shader>(nsstring(SKYBOX_SHADER) + shext);
	cplg->manager<nsshader_manager>()->compile_all();
	cplg->manager<nsshader_manager>()->link_all();
	cplg->manager<nsshader_manager>()->init_uniforms_all();
	
	// Light bounds, skydome, and tile meshes
    cplg->load<nsmesh>(nsstring(MESH_FULL_TILE) + nsstring(DEFAULT_MESH_EXTENSION));
    cplg->load<nsmesh>(nsstring(MESH_TERRAIN) + nsstring(DEFAULT_MESH_EXTENSION));
    cplg->load<nsmesh>(nsstring(MESH_HALF_TILE) + nsstring(DEFAULT_MESH_EXTENSION));
	cplg->load<nsmesh>(nsstring(MESH_POINTLIGHT_BOUNDS) + nsstring(DEFAULT_MESH_EXTENSION));
	cplg->load<nsmesh>(nsstring(MESH_SPOTLIGHT_BOUNDS) + nsstring(DEFAULT_MESH_EXTENSION));
	cplg->load<nsmesh>(nsstring(MESH_DIRLIGHT_BOUNDS) + nsstring(DEFAULT_MESH_EXTENSION));
	cplg->load<nsmesh>(nsstring(MESH_SKYDOME) + nsstring(DEFAULT_MESH_EXTENSION));

	nse.video_driver()->set_render_shaders(rs);
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
		nse.video_driver()->push_scene(scn);
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
