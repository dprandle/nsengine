/*!
\file nscamcontroller.h

\brief Definition file for NSCamController class

This file contains all of the neccessary definitions for the NSCamController class.

\author Daniel Randle
\date March 8 2014
\copywrite Earth Banana Games 2013
*/

#include <iostream>
#include <nsrender_system.h>
#include <nscamera_system.h>
#include <nsentity.h>
#include <nsselection_system.h>
#include <nsplugin_manager.h>
#include <nsevent_dispatcher.h>
#include <nsevent.h>
#include <nscam_comp.h>
#include <nstimer.h>
#include <nsscene.h>


nscamera_system::nscamera_system() :
	nssystem(),
	m_zoom_factor(DEFAULT_CAM_ZOOM_FACTOR),
	m_turn_sensitivity(DEFAULT_CAM_PIVOT_SENSITIVITY),
	m_strafe_sensitivity(DEFAULT_CAM_MOVE_SENSITIVITY),
	m_anim_time(0.35f),
	m_anim_elapsed(0.0f),
	m_anim_view(false),
	m_switch_back(false),
	m_free_mode_inverted(-1, -1),
	m_focus_mode_inverted(-1 ,-1),
	m_cam_mode(mode_free)
{}

nscamera_system::~nscamera_system()
{}

void nscamera_system::init()
{
	register_handler(nscamera_system::_handle_sel_focus_event);
	register_action_handler(nscamera_system::_handle_camera_forward, NSCAM_FORWARD);
	register_action_handler(nscamera_system::_handle_camera_backward, NSCAM_BACKWARD);
	register_action_handler(nscamera_system::_handle_camera_left, NSCAM_LEFT);
	register_action_handler(nscamera_system::_handle_camera_right, NSCAM_RIGHT);
	register_action_handler(nscamera_system::_handle_camera_tilt_pan, NSCAM_TILTPAN);
	register_action_handler(nscamera_system::_handle_camera_zoom, NSCAM_ZOOM);
	register_action_handler(nscamera_system::_handle_camera_move, NSCAM_MOVE);
	register_action_handler(nscamera_system::_handle_camera_top_view_0, NSCAM_TOPVIEW_0);
	register_action_handler(nscamera_system::_handle_camera_iso_view_0, NSCAM_ISOVIEW_0);
	register_action_handler(nscamera_system::_handle_camera_front_view_0, NSCAM_FRONTVIEW_0);
	register_action_handler(nscamera_system::_handle_camera_top_view_120, NSCAM_TOPVIEW_120);
	register_action_handler(nscamera_system::_handle_camera_iso_view_120, NSCAM_ISOVIEW_120);
	register_action_handler(nscamera_system::_handle_camera_front_view_120, NSCAM_FRONTVIEW_120);
	register_action_handler(nscamera_system::_handle_camera_top_view_240, NSCAM_TOPVIEW_240);
	register_action_handler(nscamera_system::_handle_camera_iso_view_240, NSCAM_ISOVIEW_240);
	register_action_handler(nscamera_system::_handle_camera_front_view_240, NSCAM_FRONTVIEW_240);
	register_action_handler(nscamera_system::_handle_camera_toggle_mode, NSCAM_TOGGLEMODE);
}

void nscamera_system::change_sensitivity(float pAmount, const sensitivity_t & pWhich)
{
	if (pWhich == sens_turn)
		m_turn_sensitivity += pAmount;
	else
		m_strafe_sensitivity += pAmount;
}

const nscamera_system::camera_mode & nscamera_system::mode() const
{
	return m_cam_mode;
}

const float & nscamera_system::sensitivity(const sensitivity_t & pWhich) const
{
	if (pWhich == sens_turn)
		return m_turn_sensitivity;
	return m_strafe_sensitivity;
}

float nscamera_system::zoom() const
{
	return m_zoom_factor;
}

bool nscamera_system::x_inverted(const camera_mode & pMode)
{
	if (pMode == mode_free)
		return (m_free_mode_inverted.x == -1);
	else
		return (m_focus_mode_inverted.x == -1);
}

bool nscamera_system::y_inverted(const camera_mode & pMode)
{
	if (pMode == mode_free)
		return (m_free_mode_inverted.y == -1);
	else
		return (m_focus_mode_inverted.y == -1);
}

void nscamera_system::invert_x(bool pInvert, const camera_mode & pMode)
{
	if (pMode == mode_free)
		m_free_mode_inverted.x = int32(pInvert) * -2 + 1;
	else
		m_focus_mode_inverted.x = int32(pInvert) * -2 + 1;
}

void nscamera_system::invert_y(bool pInvert, const camera_mode & pMode)
{
	if (pMode == mode_free)
		m_free_mode_inverted.y = int32(pInvert) * -2 + 1;
	else
		m_focus_mode_inverted.y = int32(pInvert) * -2 + 1;
}

void nscamera_system::set_sensitivity(float pSensitivity, const sensitivity_t & pWhich)
{
	if (pWhich == sens_turn)
		m_turn_sensitivity = pSensitivity;
	else
		m_strafe_sensitivity = pSensitivity;
}

void nscamera_system::set_view(camera_view_t view_)
{	
	if (m_active_scene == nullptr)
		return;
	nsrender::viewport * vp = nse.system<nsrender_system>()->current_viewport();
	if (vp == nullptr)
		return;
	nsentity * cam = vp->camera;
	if (cam == NULL)
		return;

	nstform_comp * camTComp = cam->get<nstform_comp>();
	nscam_comp * camComp = cam->get<nscam_comp>();

	nstform_comp * manip_tc = m_cam_focus_manipulator->get<nstform_comp>();
	if (manip_tc == nullptr)
	{
		m_active_scene->add(m_cam_focus_manipulator);
		manip_tc = m_cam_focus_manipulator->get<nstform_comp>();
		manip_tc->save_with_scene = false;
	}
	
	m_anim_view = true;
	float deg_0 = 0.0f;
	float deg_120 = 120.0f;
	float deg_240 = -120.0f;	
	switch (view_)
	{
	  case (view_top_0) :
		  m_final_orient = orientation(fvec4(0,0,-1,deg_0));
		  break;
	  case (view_top_120) :
		  m_final_orient = orientation(fvec4(0,0,-1,deg_120));
		  break;
	  case (view_top_240) :
		  m_final_orient = orientation(fvec4(0,0,-1,deg_240));
		  break;
	  case (view_iso_0) :
		  m_final_orient = orientation(fvec4(0,0,-1,deg_0));
		  m_final_orient = orientation(fvec4(m_final_orient.right(),-45.0f)) * m_final_orient;
		  break;
	  case (view_iso_120) :
		  m_final_orient = orientation(fvec4(0,0,-1,deg_120));
		  m_final_orient = orientation(fvec4(m_final_orient.right(),-45.0f)) * m_final_orient;
		  break;
	  case (view_iso_240) :
		  m_final_orient = orientation(fvec4(0,0,-1,deg_240));
		  m_final_orient = orientation(fvec4(m_final_orient.right(),-45.0f)) * m_final_orient;
		  break;
	  case (view_front_0) :
		  m_final_orient = orientation(fvec4(0,0,-1,deg_0));
		  m_final_orient = orientation(fvec4(m_final_orient.right(),-90.0f)) * m_final_orient;
		  break;
	  case (view_front_120) :
		  m_final_orient = orientation(fvec4(0,0,-1,deg_120));
		  m_final_orient = orientation(fvec4(m_final_orient.right(),-90.0f)) * m_final_orient;
		  break;
	  case (view_front_240) :
		  m_final_orient = orientation(fvec4(0,0,-1,deg_240));
		  m_final_orient = orientation(fvec4(m_final_orient.right(),-90.0f)) * m_final_orient;
		  break;
	}

	auto itf = camTComp->instance_transform(m_active_scene, 0);
	auto manip_itf = manip_tc->instance_transform(m_active_scene, 0);
	
	m_start_pos = itf->world_position();
    m_start_orient = itf->world_orientation();
    m_final_pos = manip_itf->world_position() + rotation_mat3(m_final_orient) * fvec3(0.0f, 0.0f, DEFAULT_CAM_VIEW_Z);

	camComp->post_update(true);
}

void nscamera_system::set_zoom(float pZFactor)
{
	m_zoom_factor = pZFactor;
}

void nscamera_system::setup_input_map(nsinput_map *imap, const nsstring & global_imap_name)
{
	nsinput_map::trigger camforward(
		NSCAM_FORWARD,
		nsinput_map::t_both
		);
	camforward.add_mouse_mod(nsinput_map::any_button);
	imap->add_key_trigger(global_imap_name, nsinput_map::key_w, camforward);

	nsinput_map::trigger cambackward(
		NSCAM_BACKWARD,
		nsinput_map::t_both
		);
	cambackward.add_mouse_mod(nsinput_map::any_button);
	imap->add_key_trigger(global_imap_name, nsinput_map::key_s, cambackward);

	nsinput_map::trigger camleft(
		NSCAM_LEFT,
		nsinput_map::t_both
		);
	camleft.add_mouse_mod(nsinput_map::any_button);
	imap->add_key_trigger(global_imap_name, nsinput_map::key_a, camleft);

	nsinput_map::trigger camright(
		NSCAM_RIGHT,
		nsinput_map::t_both
		);
	camright.add_mouse_mod(nsinput_map::any_button);
	imap->add_key_trigger(global_imap_name, nsinput_map::key_d, camright);

	nsinput_map::trigger camtilt(
		NSCAM_TILTPAN,
		nsinput_map::t_pressed);
    camtilt.add_key_mod(nsinput_map::key_any);
    camtilt.add_mouse_mod(nsinput_map::right_button);
	imap->add_mouse_trigger(global_imap_name, nsinput_map::movement, camtilt);

	nsinput_map::trigger cammove(
		NSCAM_MOVE,
		nsinput_map::t_pressed);
	cammove.add_key_mod(nsinput_map::key_any);
    cammove.add_mouse_mod(nsinput_map::left_button);
    cammove.add_mouse_mod(nsinput_map::right_button);
    imap->add_mouse_trigger(global_imap_name, nsinput_map::movement,cammove);

	nsinput_map::trigger camzoom(
		NSCAM_ZOOM,
		nsinput_map::t_pressed);
	camzoom.add_key_mod(nsinput_map::key_any);
    imap->add_mouse_trigger(global_imap_name, nsinput_map::scrolling, camzoom);

	nsinput_map::trigger camtopview_0(
        NSCAM_TOPVIEW_0,
        nsinput_map::t_pressed);
    camtopview_0.add_mouse_mod(nsinput_map::any_button);
    imap->add_key_trigger(global_imap_name, nsinput_map::key_keypad_8, camtopview_0);

    nsinput_map::trigger cameraiso_0(
        NSCAM_ISOVIEW_0,
        nsinput_map::t_pressed);
    cameraiso_0.add_mouse_mod(nsinput_map::any_button);
    imap->add_key_trigger(global_imap_name, nsinput_map::key_keypad_5, cameraiso_0);

    nsinput_map::trigger camfrontview_0(
        NSCAM_FRONTVIEW_0,
        nsinput_map::t_pressed);
    camfrontview_0.add_mouse_mod(nsinput_map::any_button);
    imap->add_key_trigger(global_imap_name, nsinput_map::key_keypad_2, camfrontview_0);

	nsinput_map::trigger camtopview_120(
		NSCAM_TOPVIEW_120,
        nsinput_map::t_pressed);
	camtopview_120.add_mouse_mod(nsinput_map::any_button);
    imap->add_key_trigger(global_imap_name, nsinput_map::key_keypad_7, camtopview_120);

    nsinput_map::trigger cameraiso_120(
        NSCAM_ISOVIEW_120,
        nsinput_map::t_pressed);
    cameraiso_120.add_mouse_mod(nsinput_map::any_button);
    imap->add_key_trigger(global_imap_name, nsinput_map::key_keypad_4, cameraiso_120);

    nsinput_map::trigger camfrontview_120(
        NSCAM_FRONTVIEW_120,
        nsinput_map::t_pressed);
    camfrontview_120.add_mouse_mod(nsinput_map::any_button);
    imap->add_key_trigger(global_imap_name, nsinput_map::key_keypad_1, camfrontview_120);

	nsinput_map::trigger camtopview_240(
        NSCAM_TOPVIEW_240,
        nsinput_map::t_pressed);
    camtopview_240.add_mouse_mod(nsinput_map::any_button);
    imap->add_key_trigger(global_imap_name, nsinput_map::key_keypad_9, camtopview_240);

    nsinput_map::trigger cameraiso_240(
        NSCAM_ISOVIEW_240,
        nsinput_map::t_pressed);
    cameraiso_240.add_mouse_mod(nsinput_map::any_button);
    imap->add_key_trigger(global_imap_name, nsinput_map::key_keypad_6, cameraiso_240);

    nsinput_map::trigger camfrontview_240(
        NSCAM_FRONTVIEW_240,
        nsinput_map::t_pressed);
    camfrontview_240.add_mouse_mod(nsinput_map::any_button);
    imap->add_key_trigger(global_imap_name, nsinput_map::key_keypad_3, camfrontview_240);
	
	nsinput_map::trigger camtogglemode(
        NSCAM_TOGGLEMODE,
        nsinput_map::t_pressed);
    camtogglemode.add_mouse_mod(nsinput_map::any_button);
    imap->add_key_trigger(global_imap_name, nsinput_map::key_f, camtogglemode);
}

void nscamera_system::set_mode(camera_mode pMode)
{
	m_cam_mode = pMode;
}

void nscamera_system::toggle_mode()
{
	if (m_cam_mode == mode_free)
		set_mode(mode_focus);
	else
		set_mode(mode_free);
}

void nscamera_system::_on_cam_move(nscam_comp * pCam, nstform_comp * tComp, const fvec2 & pDelta)
{
	auto tfi = tComp->instance_transform(m_active_scene, 0);

	fvec2 factor(pDelta.u*m_strafe_sensitivity * m_free_mode_inverted.x,
				 pDelta.v*m_strafe_sensitivity * m_free_mode_inverted.y);

    tfi->translate_world_space(tfi->world_orientation().right() * factor.x);
    tfi->translate_world_space(tfi->world_orientation().up() * factor.y);

	pCam->post_update(true);
}

void nscamera_system::_on_cam_turn(nscam_comp * pCam, nstform_comp * tComp, const fvec2 & pDelta)
{
	// The negatives here are a preference thing.. basically Alex that pain in the ass
	// wants rotation by default to be opposite of the normal for focus mode

	auto tfi = tComp->instance_transform(m_active_scene, 0);

	if (m_cam_mode == mode_free)
	{
        tfi->rotate(::orientation(fvec4(tfi->world_orientation().right(),pDelta.v * m_turn_sensitivity * m_free_mode_inverted.x)));
        tfi->rotate(::orientation(fvec4(0,0,1,pDelta.u * m_turn_sensitivity * m_free_mode_inverted.y)));
	}
	else
	{
		nstform_comp * tc = m_cam_focus_manipulator->get<nstform_comp>();
		if (tc == nullptr)
			return;
		auto tfi_manip = tc->instance_transform(m_active_scene, 0);

        tfi_manip->rotate(::orientation(fvec4(tfi->world_orientation().right(),pDelta.v * m_turn_sensitivity * m_focus_mode_inverted.x)));
        tfi_manip->rotate(::orientation(fvec4(0,0,1,pDelta.u * m_turn_sensitivity * m_focus_mode_inverted.y)));
	}
	pCam->post_update(true);
}

void nscamera_system::_on_cam_zoom(nscam_comp * pCam, nstform_comp * tComp, float pScroll)
{
	nscam_comp::dir_t dir = nscam_comp::dir_pos;
	if (pScroll < 0)
		dir = nscam_comp::dir_neg;

	if (pCam->proj_mode() == nscam_comp::proj_persp)
	{
		auto tfi = tComp->instance_transform(m_active_scene, 0);
        tfi->translate_world_space(tfi->world_orientation().target() * float(dir) * m_zoom_factor);
	}
	else
	{
		float factor = 0.95f;
		if (dir < 0)
			factor = 2 - factor;
		pCam->set_ortho_lr_clip(pCam->ortho_lr_clip()*factor);
		pCam->set_ortho_tb_clip(pCam->ortho_tb_clip()*factor);
	}
	
	pCam->post_update(true);
}

void nscamera_system::update()
{
	if (scene_error_check())
		return;
	
	nsrender::viewport * vp = nse.system<nsrender_system>()->current_viewport();
	if (vp == nullptr)
		return;

	nsentity * cam = vp->camera;
	if (cam == NULL)
		return;

    nscam_comp * camComp = cam->get<nscam_comp>();
    nstform_comp * camTComp = cam->get<nstform_comp>();
    auto tfi = camTComp->instance_transform(m_active_scene, 0);

	nstform_comp * tc = m_cam_focus_manipulator->get<nstform_comp>();
	if (tc == nullptr)
	{
		m_active_scene->add(m_cam_focus_manipulator);
		tc = m_cam_focus_manipulator->get<nstform_comp>();
		tc->save_with_scene = false;
	}

    auto tfi_manip = tc->instance_transform(m_active_scene, 0);
    if (m_cam_mode == mode_focus && tfi->parent() == nullptr)
        tfi->set_parent(tfi_manip, true);
    else if (m_cam_mode == mode_free && tfi->parent() != nullptr)
    {
        tfi->set_parent(nullptr, true);
        tfi_manip->set_local_orientation(fquat());
        tfi_manip->set_local_position(fvec3());
    }

		
    if (camComp->update_posted())
    {
        // generate a camera changed event
        if (!m_anim_view)
            nse.event_dispatch()->push<nscam_change_event>();

        if (camComp->strafe().animating)
        {
            tfi->translate_world_space(tfi->world_orientation().right() *
                           camComp->strafe().direction *
                           camComp->speed() *
                           nse.timer()->fixed());
        }
        if (camComp->elevate().animating)
        {
            tfi->translate_world_space(tfi->world_orientation().up() *
                           camComp->elevate().direction *
                           camComp->speed() *
                           nse.timer()->fixed());
        }
        if (camComp->fly().animating)
        {
            tfi->translate_world_space(tfi->world_orientation().target() *
                           camComp->fly().direction *
                           camComp->speed() *
                           nse.timer()->fixed());
        }

        if (tfi_manip != nullptr)
            tfi_manip->recursive_compute();

        tfi->update = true;
        tfi->recursive_compute();
        camComp->m_proj_cam = camComp->proj() * tfi->world_inv_tf();
        camComp->m_inv_proj_cam = tfi->world_tf() * camComp->inv_proj();

        camComp->post_update(
                    (camComp->strafe().animating ||
                     camComp->elevate().animating ||
                     camComp->fly().animating) ||
                    m_anim_view);

        if (m_anim_view)
        {
            float frac_time = m_anim_elapsed/m_anim_time;
            fvec3 toset = lerp(m_start_pos, m_final_pos, frac_time);
            fquat tosetrot = slerp(m_start_orient, m_final_orient, frac_time);

            tfi->set_world_position(toset);
            tfi->set_world_orientation(tosetrot);

            m_anim_elapsed += nse.timer()->fixed();
            if (m_anim_elapsed >= m_anim_time)
            {
                m_anim_view = false;
                m_anim_elapsed = 0.0f;
            }
        }

		// Update the skybox with the current camera's position
		nsentity * skyDome = m_active_scene->skydome();
		if (skyDome != NULL)
		{
			nstform_comp * tComp = skyDome->get<nstform_comp>();
			tComp->instance_transform(m_active_scene, 0)->set_world_position(tfi->world_position());
		}
	}
}

void nscamera_system::set_camera_focus_manipulator(nsentity * cam_manip)
{
	m_cam_focus_manipulator = cam_manip;
}

bool nscamera_system::_handle_camera_tilt_pan(nsaction_event * evnt)
{
	nsrender::viewport * vp = nse.system<nsrender_system>()->current_viewport();
	if (vp == nullptr)
		return true;

	nsentity * cam = vp->camera;
	if (cam == NULL)
		return true;

	if (m_anim_view)
		return true;

	nscam_comp * camc = cam->get<nscam_comp>();
	nstform_comp * tcomp = cam->get<nstform_comp>();

	fvec2 delta(evnt->norm_delta * 2.0f / (vp->normalized_bounds.zw() - vp->normalized_bounds.xy()));
	_on_cam_turn(camc, tcomp, delta);
	return true;
}

bool nscamera_system::_handle_camera_move(nsaction_event * evnt)
{
	nsrender::viewport * vp = nse.system<nsrender_system>()->current_viewport();
	if (vp == nullptr)
		return true;

	nsentity * cam = vp->camera;
	if (cam == NULL)
		return true;

	if (m_anim_view)
		return true;

	nscam_comp * camc = cam->get<nscam_comp>();
	nstform_comp * tcomp = cam->get<nstform_comp>();
	
	fvec2 delta(evnt->norm_delta * 2.0f / (vp->normalized_bounds.zw() - vp->normalized_bounds.xy()));
	_on_cam_move(camc, tcomp, delta);
	return true;
}

bool nscamera_system::_handle_camera_zoom(nsaction_event * evnt)
{
	nsrender::viewport * vp = nse.system<nsrender_system>()->current_viewport();
	if (vp == nullptr)
		return true;

	nsentity * cam = vp->camera;
	if (cam == NULL)
		return true;

	if (m_anim_view)
		return true;

	nscam_comp * camc = cam->get<nscam_comp>();
	nstform_comp * tcomp = cam->get<nstform_comp>();
	_on_cam_zoom(camc, tcomp, evnt->scroll);
	return true;
}

bool nscamera_system::_handle_camera_top_view_0(nsaction_event * evnt)
{
	set_view(view_top_0);
	return true;
}		

bool nscamera_system::_handle_camera_iso_view_0(nsaction_event * evnt)
{
	set_view(view_iso_0);
	return true;
}	

bool nscamera_system::_handle_camera_front_view_0(nsaction_event * evnt)
{
	set_view(view_front_0);
	return true;
}

bool nscamera_system::_handle_camera_top_view_120(nsaction_event * evnt)
{
	set_view(view_top_120);
	return true;
}		

bool nscamera_system::_handle_camera_iso_view_120(nsaction_event * evnt)
{
	set_view(view_iso_120);
	return true;
}	

bool nscamera_system::_handle_camera_front_view_120(nsaction_event * evnt)
{
	set_view(view_front_120);
	return true;
}

bool nscamera_system::_handle_camera_top_view_240(nsaction_event * evnt)
{
	set_view(view_top_240);
	return true;
}		

bool nscamera_system::_handle_camera_iso_view_240(nsaction_event * evnt)
{
	set_view(view_iso_240);
	return true;
}	

bool nscamera_system::_handle_camera_front_view_240(nsaction_event * evnt)
{
	set_view(view_front_240);
	return true;
}

bool nscamera_system::_handle_camera_toggle_mode(nsaction_event * evnt)
{
	toggle_mode();
	return true;
}

bool nscamera_system::_handle_camera_forward(nsaction_event * evnt)
{
	nsrender::viewport * vp = nse.system<nsrender_system>()->current_viewport();
	if (vp == nullptr)
		return true;
	nsentity * cam = vp->camera;
	if (cam == NULL)
		return true;
	if (m_anim_view)
		return true;
	
	nscam_comp * camc = cam->get<nscam_comp>();
	camc->set_fly(nscam_comp::dir_pos, evnt->cur_state);
	return true;
}

bool nscamera_system::_handle_camera_backward(nsaction_event * evnt)
{
	nsrender::viewport * vp = nse.system<nsrender_system>()->current_viewport();
	if (vp == nullptr)
		return true;
	nsentity * cam = vp->camera;
	if (cam == NULL)
		return true;
	if (m_anim_view)
		return true;
	
	nscam_comp * camc = cam->get<nscam_comp>();

	camc->set_fly(nscam_comp::dir_neg, evnt->cur_state);
	return true;
}

bool nscamera_system::_handle_camera_left(nsaction_event * evnt)
{
	nsrender::viewport * vp = nse.system<nsrender_system>()->current_viewport();
	if (vp == nullptr)
		return true;
	nsentity * cam = vp->camera;
	if (cam == NULL)
		return true;
	if (m_anim_view)
		return true;
	
	nscam_comp * camc = cam->get<nscam_comp>();

	camc->set_strafe(nscam_comp::dir_neg, evnt->cur_state);
	return true;
}

bool nscamera_system::_handle_camera_right(nsaction_event * evnt)
{
	nsrender::viewport * vp = nse.system<nsrender_system>()->current_viewport();
	if (vp == nullptr)
		return true;
	nsentity * cam = vp->camera;
	if (cam == NULL)
		return true;
	if (m_anim_view)
		return true;
	
	nscam_comp * camc = cam->get<nscam_comp>();
	camc->set_strafe(nscam_comp::dir_pos, evnt->cur_state);
	return true;
}

void nscamera_system::set_active_scene(nsscene * active_scene)
{
	if (m_active_scene != nullptr)
		m_active_scene->remove(m_cam_focus_manipulator, false);
	nssystem::set_active_scene(active_scene);
}

bool nscamera_system::_handle_sel_focus_event(nssel_focus_event * evnt)
{
	nsentity * ent = m_active_scene->find_entity(evnt->focus_id.xy());
	if (ent == nullptr)
		return true;
	
	nstform_comp * tC = ent->get<nstform_comp>();
	if (tC == nullptr)
		return true;

	nstform_comp * tc = m_cam_focus_manipulator->get<nstform_comp>();
	if (tc == nullptr)
	{
		m_active_scene->add(m_cam_focus_manipulator);
		tc = m_cam_focus_manipulator->get<nstform_comp>();
		tc->save_with_scene = false;
	}

	auto itf = tC->instance_transform(m_active_scene, evnt->focus_id.z);
	auto itf_manip = tc->instance_transform(m_active_scene, 0);
    auto child_itf = itf_manip->child(0);

    fvec3 wpos;
    if (child_itf != nullptr)
        wpos = child_itf->world_position();

    itf_manip->set_world_position(itf->world_position());
    itf_manip->recursive_compute();

    if (child_itf != nullptr)
        child_itf->set_world_position(wpos);
	return true;
}

int32 nscamera_system::update_priority()
{
	return CAM_SYS_UPDATE_PR;
}
