/*!
\file nscamcontroller.h

\brief Definition file for NSCamController class

This file contains all of the neccessary definitions for the NSCamController class.

\author Daniel Randle
\date March 8 2014
\copywrite Earth Banana Games 2013
*/

#include <iostream>

#include <nscamera_system.h>
#include <nsentity.h>
#include <nsselection_system.h>
#include <nsevent_dispatcher.h>
#include <nsevent.h>
#include <nscam_comp.h>
#include <nstimer.h>
#include <nsscene.h>
#include <nsinput_comp.h>

nscamera_system::nscamera_system() :
	nssystem(),
	m_zoom_factor(DEFAULT_CAM_ZOOM_FACTOR),
	m_turn_sensitivity(DEFAULT_CAM_SENSITIVITY),
	m_strafe_sensitivity(DEFAULT_CAM_SENSITIVITY),
	m_anim_time(0.300f),
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
	register_handler_func(this, &nscamera_system::_handle_state_event);
	register_handler_func(this, &nscamera_system::_handle_action_event);
	register_handler_func(this, &nscamera_system::_handle_sel_focus_event);

	add_trigger_hash(camera_forward, NSCAM_FORWARD);
	add_trigger_hash(camera_backward, NSCAM_BACKWARD);
	add_trigger_hash(camera_left, NSCAM_LEFT);
	add_trigger_hash(camera_right, NSCAM_RIGHT);
	add_trigger_hash(camera_tilt_pan, NSCAM_TILTPAN);
	add_trigger_hash(camera_zoom, NSCAM_ZOOM);
	add_trigger_hash(camera_move, NSCAM_MOVE);
	add_trigger_hash(camera_top_view, NSCAM_TOPVIEW);
	add_trigger_hash(camera_iso_view, NSCAM_ISOVIEW);
	add_trigger_hash(camera_front_view, NSCAM_FRONTVIEW);
	add_trigger_hash(camera_toggle_mode, NSCAM_TOGGLEMODE);
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
modes will result in the angle about the horizontal axis to be 0, 45, or 90 degrees depending on which view is
selected.
*/
void nscamera_system::set_view(camera_view_t pView)
{	
	nsscene * scene = nsengine.current_scene();
	if (scene == NULL)
		return;

	nsentity * cam = scene->camera();
	if (cam == NULL)
		return;

	nscam_comp * camComp = cam->get<nscam_comp>();
	NSTFormComp * camTComp = cam->get<NSTFormComp>();
	m_switch_back = (m_cam_mode == mode_free);
	m_anim_view = true;

	m_start_orient = camTComp->orientation();// * camComp->focusOrientation();
	m_start_pos = camTComp->wpos() - camComp->focus_point();

	if (m_switch_back)
	{
		set_mode(mode_focus);
		m_start_orient = camTComp->orientation();				
		m_start_pos = camTComp->wpos();
	}

	m_final_pos = fvec3(0.0f, 0.0f, DEFAULT_CAM_VIEW_Z);
	
	fvec3 target = m_start_orient.target();
	fvec2 projectedXY = projectPlane(target, fvec3(0.0f,0.0f,1.0f)).xy();
	fvec2 projectX = project(projectedXY,fvec2(1.0f,0.0f));
	fvec2 projectY = project(projectedXY,fvec2(0.0f,1.0f));

	fvec3 finalvec(projectX,0.0);
	if (projectX.length() < projectY.length())
		finalvec = fvec3(projectY,0.0f);
	finalvec.normalize();


	// Find which view we are switching too and change to angle about the horizontal axis accordingly
	// For each view we have to set the mode to focus if it is in free mode and then set it back to free
	// after doing the rotation. This is because in focus mode it will rotate about the selected object, which
	// is what we want.
	switch (pView)
	{
	case (view_top) :
		{		
			m_final_orient = orientation(fquat().up(), finalvec);
			//if (finalOrient.c == 1) // Special case for 180 degree rotation
			//	finalOrient.set(0, 0,-1,0);
			break;
		}
	case (view_iso) :
		{
			m_final_orient = orientation(fquat().up(), finalvec);
			//if (finalOrient.c == 1) // Special case for 180 degree rotation
			//.	finalOrient.set(0, 0,-1,0);
		}
	case (view_front) :
		{
			m_final_orient = orientation(fquat().target(), finalvec);
			if (m_final_orient.c == 1) // Special case for 180 degree rotation
				m_final_orient.set(0, 0,-1,0);
		}
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
	nsscene * scene = nsengine.current_scene();
	if (scene == NULL)
		return;

	if (pMode == mode_free)
	{
		nsentity * cam = scene->camera();
		if (cam == NULL)
			return;

		nscam_comp * camComp = cam->get<nscam_comp>();
		NSTFormComp * camTComp = cam->get<NSTFormComp>();

		camTComp->setOrientation(camComp->focus_orientation() * camTComp->orientation());
		camComp->set_focus_orientation(fquat());
		camComp->set_focus_point(fvec3());
		camTComp->setpos(camTComp->wpos() - camComp->focus_point());
	}
	else
	{
		nsentity * cam = scene->camera();
		if (cam == NULL)
			return;

		nscam_comp * camComp = cam->get<nscam_comp>();
		NSTFormComp * camTComp = cam->get<NSTFormComp>();
		nsentity * ent = scene->entity(m_focus_ent.xy());
		if (ent != NULL)
		{
			NSTFormComp * tC = ent->get<NSTFormComp>();
			if (tC->count() > m_focus_ent.z)
			{
				camTComp->setOrientation(camTComp->orientation() * camComp->focus_orientation());
				camComp->set_focus_orientation(fquat());
				camComp->set_focus_point(tC->wpos(m_focus_ent.z));
				camTComp->setpos(camTComp->wpos() - camComp->focus_point());
			}
		}
	}
}

void nscamera_system::toggle_mode()
{
	if (m_cam_mode == mode_free)
		set_mode(mode_focus);
	else
		set_mode(mode_free);
}

void nscamera_system::_on_cam_move(nscam_comp * pCam, NSTFormComp * tComp, const fvec2 & pDelta)
{
	tComp->translate(NSTFormComp::Right, pDelta.u*m_strafe_sensitivity * m_free_mode_inverted.x);
	tComp->translate(NSTFormComp::Up, pDelta.v*m_strafe_sensitivity * m_free_mode_inverted.y);
	pCam->post_update(true);
}

void nscamera_system::_on_cam_turn(nscam_comp * pCam, NSTFormComp * tComp, const fvec2 & pDelta)
{
	// The negatives here are a preference thing.. basically Alex that pain in the ass
	// wants rotation by default to be opposite of the normal for focus mode
	
	if (m_cam_mode == mode_free)
	{
		float tFac = 1.0f;
		fvec3 tVec = tComp->dirVec(NSTFormComp::Up);
		if (tVec.z < 0.0f)
			tFac = -1.0f;
	
		tComp->rotate(NSTFormComp::Right, pDelta.v * m_turn_sensitivity * m_free_mode_inverted.y);
		tComp->rotate(NSTFormComp::ZAxis, pDelta.u * -1.0f * m_turn_sensitivity * tFac * m_free_mode_inverted.x);
	}
	else
	{
		float tFac = 1.0f;
		fvec3 tVec = tComp->dirVec(NSTFormComp::Up);
		if (tVec.z < 0.0f)
			tFac = -1.0f;
		
		fquat first_rot = orientation(fvec4(0,0,1,pDelta.u * m_turn_sensitivity * tFac * m_focus_mode_inverted.x));
		fquat final_rot = first_rot * pCam->focus_orientation();

		fvec3 rvec = (final_rot * tComp->orientation()).right();
		fquat second_rot = orientation(fvec4(rvec,pDelta.v * m_turn_sensitivity * m_focus_mode_inverted.y));
		
		final_rot = second_rot * final_rot;
		
		pCam->set_focus_orientation(final_rot);
	}
	pCam->post_update(true);
}

void nscamera_system::_on_cam_zoom(nscam_comp * pCam, NSTFormComp * tComp, float pScroll)
{
	nscam_comp::dir_t dir = nscam_comp::dir_pos;
	if (pScroll < 0)
		dir = nscam_comp::dir_neg;

	if (pCam->proj_mode() == nscam_comp::proj_persp)
		tComp->translate(NSTFormComp::Target, float(dir) * m_zoom_factor);
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
	nsscene * scene = nsengine.current_scene();
	// Dont do anything if the scene is NULL
	if (scene == NULL)
		return;

	auto iter = scene->entities<nscam_comp>().begin();
	while (iter != scene->entities<nscam_comp>().end())
	{

		nscam_comp * camComp = (*iter)->get<nscam_comp>();
		NSInputComp * inComp = (*iter)->get<NSInputComp>();
		NSTFormComp * camTComp = (*iter)->get<NSTFormComp>();

		if ((*iter) == scene->camera())
		{
			if (m_anim_view)
			{
				fvec3 toset = lerp(m_start_pos, m_final_pos, m_anim_elapsed/m_anim_time);
				fquat tosetrot = slerp(m_start_orient, m_final_orient, m_anim_elapsed/m_anim_time);

				camTComp->setpos(toset);
				camTComp->setOrientation(tosetrot);
				m_anim_elapsed += nsengine.timer()->fixed();
		
				if (m_anim_elapsed >= m_anim_time)
				{
					camTComp->setpos(m_final_pos);
					camTComp->setOrientation(m_final_orient);
					if (m_switch_back)
					{
						camTComp->setParent(camComp->focus_transform());
						set_mode(mode_free);
					}
					m_anim_view = false;
					m_anim_elapsed = 0.0f;
				}
			}

			// Update the skybox!
			nsentity * skyDome = scene->skydome();
			if (skyDome != NULL)
			{
				NSTFormComp * tComp = skyDome->get<NSTFormComp>();
				tComp->setpos(camTComp->wpos());
			}
		}

		if (camComp->update_posted())
		{

			if (camComp->strafe().animating)
				camTComp->translate(NSTFormComp::Right, camComp->strafe().direction * camComp->speed() * nsengine.timer()->fixed());
			if (camComp->elevate().animating)
				camTComp->translate(NSTFormComp::Up, camComp->elevate().direction * camComp->speed() * nsengine.timer()->fixed());
			if (camComp->fly().animating)
				camTComp->translate(NSTFormComp::Target, camComp->fly().direction * camComp->speed() * nsengine.timer()->fixed());

			if (m_cam_mode == mode_focus)
				camTComp->setParent(camComp->focus_transform());
			else
				camTComp->setParent(fmat4());

			camTComp->computeTransform();
			camComp->m_proj_cam = camComp->proj() * camTComp->pov();
			camComp->m_inv_proj_cam = camTComp->transform() * camComp->inv_proj();

			camComp->post_update(
				(camComp->strafe().animating ||
				 camComp->elevate().animating ||
				 camComp->fly().animating) ||
				m_anim_view);

		}	   
		++iter;
	}
}

bool nscamera_system::_handle_action_event(nsaction_event * evnt)
{
	nsscene * scene = nsengine.current_scene();

	if (scene == NULL)
		return true;

	nsentity * camera = scene->camera();
	if (camera == NULL)
		return true;

	nscam_comp * camc = camera->get<nscam_comp>();
	NSTFormComp * tcomp = camera->get<NSTFormComp>();

	fvec2 mouseDelta;
	float scroll;
	auto xpos_iter = evnt->axes.find(nsinput_map::axis_mouse_xdelta),
		ypos_iter = evnt->axes.find(nsinput_map::axis_mouse_ydelta),
		scroll_iter = evnt->axes.find(nsinput_map::axis_scroll_delta);

	if (xpos_iter != evnt->axes.end())
		mouseDelta.x = xpos_iter->second;

	if (ypos_iter != evnt->axes.end())
		mouseDelta.y = ypos_iter->second;

	if (scroll_iter != evnt->axes.end())
		scroll = scroll_iter->second;

	if (evnt->trigger_hash_name == trigger_hash(camera_tilt_pan))
		_on_cam_turn(camc, tcomp, mouseDelta);

	if (evnt->trigger_hash_name == trigger_hash(camera_move))
		_on_cam_move(camc, tcomp, mouseDelta);

	if (evnt->trigger_hash_name == trigger_hash(camera_zoom))
		_on_cam_zoom(camc, tcomp, scroll);

	if (evnt->trigger_hash_name == trigger_hash(camera_top_view))
		set_view(view_top);
		
	if (evnt->trigger_hash_name == trigger_hash(camera_iso_view))
		set_view(view_iso);
	
	if (evnt->trigger_hash_name == trigger_hash(camera_front_view))
		set_view(view_front);

	if (evnt->trigger_hash_name == trigger_hash(camera_toggle_mode))
		toggle_mode();

	return true;
}

bool nscamera_system::_handle_state_event(nsstate_event * evnt)
{
	nsscene * scene = nsengine.current_scene();

	if (scene == NULL)
		return true;
	
	nsentity * camera = scene->camera();
	if (camera == NULL)
		return true;

	nscam_comp * camc = camera->get<nscam_comp>();

	if (evnt->trigger_hash_name == trigger_hash(camera_forward))
		camc->set_fly(nscam_comp::dir_pos, evnt->toggle);

	if (evnt->trigger_hash_name == trigger_hash(camera_backward))
		camc->set_fly(nscam_comp::dir_neg, evnt->toggle);

	if (evnt->trigger_hash_name == trigger_hash(camera_left))
		camc->set_strafe(nscam_comp::dir_neg, evnt->toggle);

	if (evnt->trigger_hash_name == trigger_hash(camera_right))
		camc->set_strafe(nscam_comp::dir_pos, evnt->toggle);
	return true;
}

bool nscamera_system::_handle_sel_focus_event(nssel_focus_event * evnt)
{
	m_focus_ent = evnt->focus_id;

	if (m_cam_mode == mode_free)
		return true;
	nsscene * scn = nsengine.current_scene();
	if (scn == NULL)
		return true;
	nsentity * cam = scn->camera();
	if (cam == NULL)
		return true;
	
	nsentity * ent = scn->entity(evnt->focus_id.xy());
	if (ent != NULL)
	{
		NSTFormComp * tC = ent->get<NSTFormComp>();
		nscam_comp * camc = cam->get<nscam_comp>();
		NSTFormComp * camtc = cam->get<NSTFormComp>();
		
		if (evnt->focus_id.z < tC->count())
		{
			camtc->setOrientation(camc->focus_orientation() * camtc->orientation());
			camc->set_focus_orientation(fquat());
			camc->set_focus_point(tC->wpos(evnt->focus_id.z));
			camtc->setpos(camtc->wpos() - camc->focus_point());
		}
	}
	return true;
}

int32 nscamera_system::update_priority()
{
	return CAM_SYS_UPDATE_PR;
}
