/*! 
  \file nsrendersystem.cpp
	
  \brief Definition file for nsrender_system class

  This file contains all of the neccessary definitions for the nsrender_system class.

  \author Daniel Randle
  \date November 23 2013
  \copywrite Earth Banana Games 2013
*/

#include <nsgl_driver.h>
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
#include <nsgl_framebuffer.h>
#include <nsui_system.h>

#include <nsanim_comp.h>
#include <nsterrain_comp.h>
#include <nssel_comp.h>
#include <nsrender_comp.h>
#include <nslight_comp.h>

nsrender_system::nsrender_system() :
	nssystem(type_to_hash(nsrender_system))
{}

nsrender_system::~nsrender_system()
{}

void nsrender_system::init()
{
	register_handler(nsrender_system::_handle_window_resize);
	register_action_handler(nsrender_system::_handle_viewport_change, VIEWPORT_CHANGE);
}

void nsrender_system::update()
{
	if (scene_error_check())
		return;
	
	auto scene_ents = m_active_scene->entities_in_scene();
	if (scene_ents == nullptr)
		return;

	// Go through and recursively update the scene
	auto ent_iter = scene_ents->begin();
	while (ent_iter != scene_ents->end())
	{
		nstform_comp *tfc = (*ent_iter)->get<nstform_comp>();
		for (uint32 i = 0; i < tfc->instance_count(m_active_scene); ++i)
		{
			auto itf = tfc->instance_transform(m_active_scene, i);
			if (itf->parent() == nullptr)
				itf->recursive_compute();
		}
		++ent_iter;
	}

	// now go through second time after recursive update
	ent_iter = scene_ents->begin();
	while (ent_iter != scene_ents->end())
	{
		nstform_comp * tForm = (*ent_iter)->get<nstform_comp>();
        if (tForm->update_posted())
        {
			tform_per_scene_info * psi = tForm->m_scenes_info.find(m_active_scene)->second;
			psi->video_update();
            tForm->post_update(false);
        }
		++ent_iter;
	}
}

int32 nsrender_system::update_priority()
{
	return RENDER_SYS_UPDATE_PR;
}

bool nsrender_system::_handle_window_resize(window_resize_event * evt)
{
	nse.video_driver()->window_resized(evt->new_size);
	return true;
}

bool nsrender_system::_handle_viewport_change(nsaction_event * evt)
{
	viewport * vp = nse.video_driver()->front_viewport(evt->norm_mpos);
	if (vp != nullptr)
		nse.video_driver()->set_focused_viewport(vp);
	return true;
}
