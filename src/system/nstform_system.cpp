/*! 
  \file nsrendersystem.cpp
	
  \brief Definition file for nsrender_system class

  This file contains all of the neccessary definitions for the nsrender_system class.

  \author Daniel Randle
  \date November 23 2013
  \copywrite Earth Banana Games 2013
*/

#include <opengl/nsgl_driver.h>
#include <nsengine.h>
#include <asset/nsplugin_manager.h>
#include <asset/nsshader_manager.h>
#include <system/nstform_system.h>
#include <nsvideo_driver.h>
#include <component/nscam_comp.h>
#include <nsworld_data.h>
#include <asset/nsplugin.h>
#include <asset/nsmesh.h>
#include <asset/nstexture.h>
#include <asset/nsmaterial.h>
#include <asset/nsshader.h>
#include <opengl/nsgl_framebuffer.h>
#include <system/nsui_system.h>
#include <nstform_ent_chunk.h>

#include <component/nsanim_comp.h>
#include <component/nsterrain_comp.h>
#include <component/nssel_comp.h>
#include <component/nsrender_comp.h>
#include <component/nslight_comp.h>

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

void nstform_system::setup_input_map(nsinput_map * im, const nsstring & global_ctxt)
{
	nsinput_map::trigger change_vp(VIEWPORT_CHANGE, nsinput_map::t_pressed);
    change_vp.add_key_mod(nsinput_map::key_any);
    im->add_mouse_trigger(global_ctxt, nsinput_map::left_button, change_vp);
}

void nstform_system::update()
{
	if (chunk_error_check())
		return;
	
	auto chunk_ents = m_active_chunk->all_entities();
	if (chunk_ents == nullptr)
		return;
	

	// Go through and recursively update the scene
	auto ent_iter = chunk_ents->begin();
	while (ent_iter != chunk_ents->end())
	{
		nstform_comp * tfc = (*ent_iter)->get<nstform_comp>();
		if (tfc->parent() == nullptr)
			tfc->recursive_compute();
		++ent_iter;
	}

	vid_ctxt * vctxt = nse.video_driver()->current_context();
	auto iter = vctxt->instance_objs.begin();
	while (iter != vctxt->instance_objs.end())
	{
		(*iter)->video_update();
		++iter;
	}
}

int32 nstform_system::update_priority()
{
	return TFORM_SYS_UPDATE_PR;
}

bool nstform_system::_handle_window_resize(window_resized * evt)
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
