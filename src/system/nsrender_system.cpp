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

nstform_system::nstform_system() :
	nssystem(type_to_hash(nstform_system))
{}

nstform_system::~nstform_system()
{}

void nstform_system::init()
{
	register_handler(nstform_system::_handle_window_resize);
	register_action_handler(nstform_system::_handle_viewport_change, VIEWPORT_CHANGE);
}

void nstform_system::release()
{
	
}

void nstform_system::update()
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
		tform_per_scene_info * psi = (*ent_iter)->get<nstform_comp>()->per_scene_info(m_active_scene);
		for (uint32 i = 0; i < psi->m_tforms.size(); ++i)
		{
			instance_tform & itf = psi->m_tforms[i];
			if (itf.parent() == nullptr)
				itf.recursive_compute();
		}
		++ent_iter;
	}

	ent_iter = scene_ents->begin();
	while (ent_iter != scene_ents->end())
	{
		nstform_comp * tForm = (*ent_iter)->get<nstform_comp>();
        if (tForm->update_posted())
		{
			tform_per_scene_info * psi = tForm->per_scene_info(m_active_scene);
			psi->video_update();
			tForm->post_update(false);
		}
		++ent_iter;
	}

}

int32 nstform_system::update_priority()
{
	return TFORM_SYS_UPDATE_PR;
}

bool nstform_system::_handle_window_resize(window_resize_event * evt)
{
	// This assumes that the event generated is for the current context - should be true unless there is
	// a bug in the input event generating code
	nse.video_driver()->current_context()->window_resized(evt->new_size);
	return true;
}

bool nstform_system::_handle_viewport_change(nsaction_event * evt)
{
	viewport * vp = nse.video_driver()->current_context()->front_viewport_at_screen_pos(evt->norm_mpos);
	if (vp != nullptr)
		nse.video_driver()->current_context()->focused_vp = vp;
	return true;
}
