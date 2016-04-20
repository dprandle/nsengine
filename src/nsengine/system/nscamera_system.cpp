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
	m_turn_sensitivity(DEFAULT_CAM_SENSITIVITY),
	m_strafe_sensitivity(DEFAULT_CAM_SENSITIVITY),
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

/*!
Set the camera view so that it locks and does not allow movement about the right vector axis. That is, it does
not allow the pitch to be changed, only the yaw. There are three different angles.. front, isometric (45 degrees)
and top down. No matter what the camera mode is in or what angle it is at, setting the camera to any of these
modes will result in the angle about the horizontal axis to be 0, 45, or deg_0 degrees depending on which view is
selected.
*/
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

	nscam_comp * camComp = cam->get<nscam_comp>();
	nstform_comp * camTComp = cam->get<nstform_comp>();

	fvec3 focus_pos;
	nsentity * ent = m_active_scene->find_entity(m_focus_ent.xy());
	nstform_comp * tC = NULL;
	if (ent != NULL)
	{
		tC = ent->get<nstform_comp>();
		focus_pos = tC->instance_transform(m_active_scene, m_focus_ent.z)->world_position();
	}

	
	m_anim_view = true;

	float deg_0 = 0.0f;
	float deg_120 = 120.0f;
	float deg_240 = -120.0f;

	//90
	//210
	//deg_240
	
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
	m_final_pos = fvec3(0.0f, 0.0f, DEFAULT_CAM_VIEW_Z);
		
	m_start_pos = itf->position;
	if (m_cam_mode == mode_focus)
	{
		m_start_orient = camComp->focus_orientation();
		m_start_local_orient = itf->orient;
	}
	else
	{
		m_start_orient = itf->orient;		
		m_final_pos = focus_pos + rotation_mat3(m_final_orient) * m_final_pos;
	}

	camComp->post_update(true);
}

void nscamera_system::set_zoom(float pZFactor)
{
	m_zoom_factor = pZFactor;
}

void nscamera_system::set_mode(camera_mode pMode)
{
	m_cam_mode = pMode;

	if (m_active_scene == nullptr)
		return;

	nsrender::viewport * vp = nse.system<nsrender_system>()->current_viewport();
	if (vp == nullptr)
		return;

	nsentity * cam = vp->camera;
	if (cam == NULL)
		return;

	nscam_comp * camComp = cam->get<nscam_comp>();
	nstform_comp * camTComp = cam->get<nstform_comp>();
	nsentity * ent = m_active_scene->find_entity(m_focus_ent.xy());
	nstform_comp * tC = NULL;
	if (ent != NULL)
		tC = ent->get<nstform_comp>();

	auto itf = camTComp->instance_transform(m_active_scene, 0);
	itf->orient = camComp->focus_orientation() * itf->orient;
	itf->update = true;
	camComp->set_focus_orientation(fquat());

	if (pMode == mode_free || tC == NULL)
	{
		camComp->set_focus_point(fvec3());
		itf->position = itf->world_position();
	}
	else
	{
		camComp->set_focus_point(tC->instance_transform(m_active_scene, m_focus_ent.z)->world_position());
		itf->position = itf->world_position() - camComp->focus_point();
	}
	camComp->compute_focus_transform();
	camTComp->instance_transform(m_active_scene, 0)->recursive_compute();
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
				 pDelta.u*m_strafe_sensitivity * m_free_mode_inverted.y);
	
	tfi->position += tfi->orient.right() * factor.x;
	tfi->position += tfi->orient.up() * factor.y;
	tfi->update = true;
	tComp->post_update(true);
	pCam->post_update(true);
}

void nscamera_system::_on_cam_turn(nscam_comp * pCam, nstform_comp * tComp, const fvec2 & pDelta)
{
	// The negatives here are a preference thing.. basically Alex that pain in the ass
	// wants rotation by default to be opposite of the normal for focus mode

	auto tfi = tComp->instance_transform(m_active_scene, 0);
	
	if (m_cam_mode == mode_free)
	{
		float tFac = 1.0f;
		if (tfi->orient.up().z < 0.0f)
			tFac = -1.0f;

		tFac *= pDelta.u * m_turn_sensitivity * m_free_mode_inverted.x;
		tfi->orient = ::orientation(fvec4(tfi->orient.right(),tFac)) * tfi->orient;
		tfi->orient = ::orientation(fvec4(0,0,1,tFac)) * tfi->orient;
		tfi->update = true;
		tComp->post_update(true);
	}
	else
	{
		// float tFac = 1.0f;
		// fvec3 tVec = tComp->dvec(nstform_comp::dir_up);
		// if (tVec.z < 0.0f)
		// 	tFac = -1.0f;
		
		// fquat first_rot = orientation(fvec4(0,0,1,pDelta.u * m_turn_sensitivity * tFac * m_focus_mode_inverted.x));
		// fquat final_rot = first_rot * pCam->focus_orientation();

		// fvec3 rvec = (final_rot * tComp->orientation()).right();
		// fquat second_rot = orientation(fvec4(rvec,pDelta.v * m_turn_sensitivity * m_focus_mode_inverted.y));
		
		// final_rot = second_rot * final_rot;
		
		// pCam->set_focus_orientation(final_rot);
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
		tfi->position += tfi->orient.target() * float(dir) * m_zoom_factor;
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

	auto ents = m_active_scene->entities_with_comp<nscam_comp>();
	if (ents == nullptr)
		return;
	
	auto iter = ents->begin();
	while (iter != ents->end())
	{
		nscam_comp * camComp = (*iter)->get<nscam_comp>();
		nstform_comp * camTComp = (*iter)->get<nstform_comp>();
		auto tfi = camTComp->instance_transform(m_active_scene, 0);
		
		if (camComp->update_posted())
		{
			// generate a camera changed event
			if ((*iter) == cam && !m_anim_view)
				nse.event_dispatch()->push<nscam_change_event>();
			
			if (camComp->strafe().animating)
				tfi->position += tfi->orient.right() * camComp->strafe().direction * camComp->speed() * nse.timer()->fixed();
			if (camComp->elevate().animating)
				tfi->position += tfi->orient.up() * camComp->elevate().direction * camComp->speed() * nse.timer()->fixed();
			if (camComp->fly().animating)
				tfi->position += tfi->orient.target() * camComp->fly().direction * camComp->speed() * nse.timer()->fixed();

			// if (m_cam_mode == mode_focus)
			// 	camTComp->set_parent(camComp->focus_transform());
			// else
			// 	camTComp->set_parent(fmat4());

			tfi->recursive_compute();
			camComp->m_proj_cam = camComp->proj() * tfi->world_inv_tf();
			camComp->m_inv_proj_cam = tfi->world_tf() * camComp->inv_proj();

			camComp->post_update(
				(camComp->strafe().animating ||
				 camComp->elevate().animating ||
				 camComp->fly().animating) ||
				m_anim_view);
		}

		if ((*iter) == cam)
		{
			if (m_anim_view)
			{
				float frac_time = m_anim_elapsed/m_anim_time;
				fvec3 toset = lerp(m_start_pos, m_final_pos, frac_time);
				fquat tosetrot = slerp(m_start_orient, m_final_orient, frac_time);
				
				tfi->position = toset;
				
				if (m_cam_mode == mode_focus)
				{
					camComp->set_focus_orientation(tosetrot);
					tfi->orient = slerp(m_start_local_orient,fquat(), frac_time);
				}
				else
					tfi->orient = tosetrot;
				
				m_anim_elapsed += nse.timer()->fixed();
		
				if (m_anim_elapsed >= m_anim_time)
				{
					m_anim_view = false;
					m_anim_elapsed = 0.0f;
				}
			}
		}

		// Update the skybox with the current camera's position
		nsentity * skyDome = m_active_scene->skydome();
		if (skyDome != NULL)
		{
			nstform_comp * tComp = skyDome->get<nstform_comp>();
			tComp->instance_transform(m_active_scene, 0)->position = tfi->world_position();
		}
		++iter;
	}
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
	_on_cam_turn(camc, tcomp, evnt->norm_delta);
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
	_on_cam_move(camc, tcomp, evnt->norm_delta);
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

void nscamera_system::_on_cam_rotate_horizontal(nscam_comp * pCam, nstform_comp * tComp, bool left)
{
	fquat rotation = orientation(fvec4(0,0,1,30.0f*int(left)));
	if (m_cam_mode == mode_free)
	{
	}
	else
	{
		pCam->set_focus_orientation(rotation);
	}
	pCam->post_update(true);	
}

void nscamera_system::_on_cam_rotate_vertical(nscam_comp * pCam, nstform_comp * tComp, bool up)
{
	fvec3 rvec = (pCam->focus_orientation() * tComp->instance_transform(m_active_scene, 0)->orient).right();
	fquat rot = orientation(fvec4(rvec,90.0f * int(up)));
	if (m_cam_mode == mode_free)
	{
	}
	else
	{
		pCam->set_focus_orientation(rot);
	}
	pCam->post_update(true);	
}

bool nscamera_system::_handle_sel_focus_event(nssel_focus_event * evnt)
{
	m_focus_ent = evnt->focus_id;

	if (m_cam_mode == mode_free)
		return true;
	
	nsrender::viewport * vp = nse.system<nsrender_system>()->current_viewport();
	if (vp == nullptr)
		return true;

	nsentity * cam = vp->camera;
	if (cam == NULL)
		return true;

	set_mode(mode_focus);
	return true;
}

int32 nscamera_system::update_priority()
{
	return CAM_SYS_UPDATE_PR;
}
