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

NSCameraSystem::NSCameraSystem() :
	NSSystem(),
	mZoomFactor(DEFAULT_CAM_ZOOM_FACTOR),
	mTurnSensitivity(DEFAULT_CAM_SENSITIVITY),
	mStrafeSensitivity(DEFAULT_CAM_SENSITIVITY),
	animTime(0.300f),
	anim_elapsed(0.0f),
	anim_view(false),
	switch_back(false),
	mFreeModeInverted(-1, -1),
	mFocusModeInverted(-1 ,-1),
	mMode(Free)
{}

NSCameraSystem::~NSCameraSystem()
{}

void NSCameraSystem::init()
{
	registerHandlerFunc(this, &NSCameraSystem::_handleStateEvent);
	registerHandlerFunc(this, &NSCameraSystem::_handleActionEvent);
	registerHandlerFunc(this, &NSCameraSystem::_handleSelFocusEvent);

	addTriggerHash(CameraForward, NSCAM_FORWARD);
	addTriggerHash(CameraBackward, NSCAM_BACKWARD);
	addTriggerHash(CameraLeft, NSCAM_LEFT);
	addTriggerHash(CameraRight, NSCAM_RIGHT);
	addTriggerHash(CameraTiltPan, NSCAM_TILTPAN);
	addTriggerHash(CameraZoom, NSCAM_ZOOM);
	addTriggerHash(CameraMove, NSCAM_MOVE);
	addTriggerHash(CameraTopView, NSCAM_TOPVIEW);
	addTriggerHash(CameraIsoView, NSCAM_ISOVIEW);
	addTriggerHash(CameraFrontView, NSCAM_FRONTVIEW);
	addTriggerHash(CameraToggleMode, NSCAM_TOGGLEMODE);
}

void NSCameraSystem::changeSensitivity(float pAmount, const Sensitivity & pWhich)
{
	if (pWhich == Turn)
		mTurnSensitivity += pAmount;
	else
		mStrafeSensitivity += pAmount;
}

const NSCameraSystem::CameraMode & NSCameraSystem::mode() const
{
	return mMode;
}

const float & NSCameraSystem::sensitivity(const Sensitivity & pWhich) const
{
	if (pWhich == Turn)
		return mTurnSensitivity;
	return mStrafeSensitivity;
}

float NSCameraSystem::zoom() const
{
	return mZoomFactor;
}

bool NSCameraSystem::xinv(const CameraMode & pMode)
{
	if (pMode == Free)
		return (mFreeModeInverted.x == -1);
	else
		return (mFocusModeInverted.x == -1);
}

bool NSCameraSystem::yinv(const CameraMode & pMode)
{
	if (pMode == Free)
		return (mFreeModeInverted.y == -1);
	else
		return (mFocusModeInverted.y == -1);
}

void NSCameraSystem::setxinv(bool pInvert, const CameraMode & pMode)
{
	if (pMode == Free)
		mFreeModeInverted.x = int32(pInvert) * -2 + 1;
	else
		mFocusModeInverted.x = int32(pInvert) * -2 + 1;
}

void NSCameraSystem::setyinv(bool pInvert, const CameraMode & pMode)
{
	if (pMode == Free)
		mFreeModeInverted.y = int32(pInvert) * -2 + 1;
	else
		mFocusModeInverted.y = int32(pInvert) * -2 + 1;
}

void NSCameraSystem::setSensitivity(float pSensitivity, const Sensitivity & pWhich)
{
	if (pWhich == Turn)
		mTurnSensitivity = pSensitivity;
	else
		mStrafeSensitivity = pSensitivity;
}

/*!
Set the camera view so that it locks and does not allow movement about the right vector axis. That is, it does
not allow the pitch to be changed, only the yaw. There are three different angles.. front, isometric (45 degrees)
and top down. No matter what the camera mode is in or what angle it is at, setting the camera to any of these
modes will result in the angle about the horizontal axis to be 0, 45, or 90 degrees depending on which view is
selected.
*/
void NSCameraSystem::setView(CameraView pView)
{	
	NSScene * scene = nsengine.currentScene();
	if (scene == NULL)
		return;

	NSEntity * cam = scene->camera();
	if (cam == NULL)
		return;

	NSCamComp * camComp = cam->get<NSCamComp>();
	NSTFormComp * camTComp = cam->get<NSTFormComp>();
	switch_back = (mMode == Free);
	anim_view = true;

	startOrient = camTComp->orientation();// * camComp->focusOrientation();
	startPos = camTComp->wpos() - camComp->focusPoint();

	if (switch_back)
	{
		setMode(Focus);
		startOrient = camTComp->orientation();				
		startPos = camTComp->wpos();
	}

	finalPos = fvec3(0.0f, 0.0f, DEFAULT_CAM_VIEW_Z);
	
	fvec3 target = startOrient.target();
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
	case (Top) :
		{		
			finalOrient = orientation(fquat().up(), finalvec);
			//if (finalOrient.c == 1) // Special case for 180 degree rotation
			//	finalOrient.set(0, 0,-1,0);
			break;
		}
	case (Iso) :
		{
			finalOrient = orientation(fquat().up(), finalvec);
			//if (finalOrient.c == 1) // Special case for 180 degree rotation
			//.	finalOrient.set(0, 0,-1,0);
		}
	case (Front) :
		{
			finalOrient = orientation(fquat().target(), finalvec);
			if (finalOrient.c == 1) // Special case for 180 degree rotation
				finalOrient.set(0, 0,-1,0);
		}
	}
	camComp->postUpdate(true);
}

void NSCameraSystem::setZoom(float pZFactor)
{
	mZoomFactor = pZFactor;
}

void NSCameraSystem::setMode(CameraMode pMode)
{
	mMode = pMode;
	NSScene * scene = nsengine.currentScene();
	if (scene == NULL)
		return;

	if (pMode == Free)
	{
		NSEntity * cam = scene->camera();
		if (cam == NULL)
			return;

		NSCamComp * camComp = cam->get<NSCamComp>();
		NSTFormComp * camTComp = cam->get<NSTFormComp>();

		camTComp->setOrientation(camComp->focusOrientation() * camTComp->orientation());
		camComp->setFocusRot(fquat());
		camComp->setFocusPoint(fvec3());
		camTComp->setpos(camTComp->wpos() - camComp->focusPoint());
	}
	else
	{
		NSEntity * cam = scene->camera();
		if (cam == NULL)
			return;

		NSCamComp * camComp = cam->get<NSCamComp>();
		NSTFormComp * camTComp = cam->get<NSTFormComp>();
		NSEntity * ent = scene->entity(mFocusEnt.xy());
		if (ent != NULL)
		{
			NSTFormComp * tC = ent->get<NSTFormComp>();
			if (tC->count() > mFocusEnt.z)
			{
				camTComp->setOrientation(camTComp->orientation() * camComp->focusOrientation());
				camComp->setFocusRot(fquat());
				camComp->setFocusPoint(tC->wpos(mFocusEnt.z));
				camTComp->setpos(camTComp->wpos() - camComp->focusPoint());
			}
		}
	}
}

void NSCameraSystem::toggleMode()
{
	if (mMode == Free)
		setMode(Focus);
	else
		setMode(Free);
}

void NSCameraSystem::_onCamMove(NSCamComp * pCam, NSTFormComp * tComp, const fvec2 & pDelta)
{
	tComp->translate(NSTFormComp::Right, pDelta.u*mStrafeSensitivity * mFreeModeInverted.x);
	tComp->translate(NSTFormComp::Up, pDelta.v*mStrafeSensitivity * mFreeModeInverted.y);
	pCam->postUpdate(true);
}

void NSCameraSystem::_onCamTurn(NSCamComp * pCam, NSTFormComp * tComp, const fvec2 & pDelta)
{
	// The negatives here are a preference thing.. basically Alex that pain in the ass
	// wants rotation by default to be opposite of the normal for focus mode
	
	if (mMode == Free)
	{
		float tFac = 1.0f;
		fvec3 tVec = tComp->dirVec(NSTFormComp::Up);
		if (tVec.z < 0.0f)
			tFac = -1.0f;
	
		tComp->rotate(NSTFormComp::Right, pDelta.v * mTurnSensitivity * mFreeModeInverted.y);
		tComp->rotate(NSTFormComp::ZAxis, pDelta.u * -1.0f * mTurnSensitivity * tFac * mFreeModeInverted.x);
	}
	else
	{
		float tFac = 1.0f;
		fvec3 tVec = tComp->dirVec(NSTFormComp::Up);
		if (tVec.z < 0.0f)
			tFac = -1.0f;

		pCam->rotateFocus(0.0f, 0.0f, 1.0f, pDelta.u * mTurnSensitivity * tFac * mFocusModeInverted.x);
		pCam->rotateFocus((pCam->focusOrientation()*tComp->orientation()).right(), pDelta.v * mTurnSensitivity * mFocusModeInverted.y);
	}
	pCam->postUpdate(true);
}

void NSCameraSystem::_onCamZoom(NSCamComp * pCam, NSTFormComp * tComp, float pScroll)
{
	NSCamComp::Direction dir = NSCamComp::DirPos;
	if (pScroll < 0)
		dir = NSCamComp::DirNeg;

	if (pCam->projectionMode() == NSCamComp::Persp)
		tComp->translate(NSTFormComp::Target, float(dir) * mZoomFactor);
	else
	{
		float factor = 0.95f;
		if (dir < 0)
			factor = 2 - factor;
		pCam->setOrthoLRClip(pCam->orthoLRClip()*factor);
		pCam->setOrthoTBClip(pCam->orthoTBClip()*factor);
	}

	pCam->postUpdate(true);
}

void NSCameraSystem::update()
{
	NSScene * scene = nsengine.currentScene();
	// Dont do anything if the scene is NULL
	if (scene == NULL)
		return;

	auto iter = scene->entities<NSCamComp>().begin();
	while (iter != scene->entities<NSCamComp>().end())
	{

		NSCamComp * camComp = (*iter)->get<NSCamComp>();
		NSInputComp * inComp = (*iter)->get<NSInputComp>();
		NSTFormComp * camTComp = (*iter)->get<NSTFormComp>();

		if ((*iter) == scene->camera())
		{
			if (anim_view)
			{
				fvec3 toset = lerp(startPos, finalPos, anim_elapsed/animTime);
				fquat tosetrot = slerp(startOrient, finalOrient, anim_elapsed/animTime);

				camTComp->setpos(toset);
				camTComp->setOrientation(tosetrot);
				anim_elapsed += nsengine.timer()->fixed();
		
				if (anim_elapsed >= animTime)
				{
					camTComp->setpos(finalPos);
					camTComp->setOrientation(finalOrient);
					if (switch_back)
					{
						camTComp->setParent(camComp->camFocusTForm());
						setMode(Free);
					}
					anim_view = false;
					anim_elapsed = 0.0f;
				}
			}

			// Update the skybox!
			NSEntity * skyDome = scene->skydome();
			if (skyDome != NULL)
			{
				NSTFormComp * tComp = skyDome->get<NSTFormComp>();
				tComp->setpos(camTComp->wpos());
			}
		}

		if (camComp->updatePosted())
		{

			if (camComp->strafe().mAnimate)
				camTComp->translate(NSTFormComp::Right, camComp->strafe().mDir * camComp->speed() * nsengine.timer()->fixed());
			if (camComp->elevate().mAnimate)
				camTComp->translate(NSTFormComp::Up, camComp->elevate().mDir * camComp->speed() * nsengine.timer()->fixed());
			if (camComp->fly().mAnimate)
				camTComp->translate(NSTFormComp::Target, camComp->fly().mDir * camComp->speed() * nsengine.timer()->fixed());

			if (mMode == Focus)
				camTComp->setParent(camComp->camFocusTForm());
			else
				camTComp->setParent(fmat4());

			camTComp->computeTransform();
			camComp->mProjCam = camComp->proj() * camTComp->pov();
			camComp->mInvProjCam = camTComp->transform() * camComp->invproj();

			camComp->postUpdate(
				(camComp->strafe().mAnimate ||
				 camComp->elevate().mAnimate ||
				 camComp->fly().mAnimate) ||
				anim_view);

		}	   
		++iter;
	}
}

bool NSCameraSystem::_handleActionEvent(NSActionEvent * evnt)
{
	NSScene * scene = nsengine.currentScene();

	if (scene == NULL)
		return true;

	NSEntity * camera = scene->camera();
	if (camera == NULL)
		return true;

	NSCamComp * camc = camera->get<NSCamComp>();
	NSTFormComp * tcomp = camera->get<NSTFormComp>();

	fvec2 mouseDelta;
	float scroll;
	auto xpos_iter = evnt->axes.find(NSInputMap::MouseXDelta),
		ypos_iter = evnt->axes.find(NSInputMap::MouseYDelta),
		scroll_iter = evnt->axes.find(NSInputMap::ScrollDelta);

	if (xpos_iter != evnt->axes.end())
		mouseDelta.x = xpos_iter->second;

	if (ypos_iter != evnt->axes.end())
		mouseDelta.y = ypos_iter->second;

	if (scroll_iter != evnt->axes.end())
		scroll = scroll_iter->second;

	if (evnt->mTriggerHashName == triggerHash(CameraTiltPan))
		_onCamTurn(camc, tcomp, mouseDelta);

	if (evnt->mTriggerHashName == triggerHash(CameraMove))
		_onCamMove(camc, tcomp, mouseDelta);

	if (evnt->mTriggerHashName == triggerHash(CameraZoom))
		_onCamZoom(camc, tcomp, scroll);

	if (evnt->mTriggerHashName == triggerHash(CameraTopView))
		setView(Top);
		
	if (evnt->mTriggerHashName == triggerHash(CameraIsoView))
		setView(Iso);
	
	if (evnt->mTriggerHashName == triggerHash(CameraFrontView))
		setView(Front);

	if (evnt->mTriggerHashName == triggerHash(CameraToggleMode))
		toggleMode();

	return true;
}

bool NSCameraSystem::_handleStateEvent(NSStateEvent * evnt)
{
	NSScene * scene = nsengine.currentScene();

	if (scene == NULL)
		return true;
	
	NSEntity * camera = scene->camera();
	if (camera == NULL)
		return true;

	NSCamComp * camc = camera->get<NSCamComp>();

	if (evnt->mTriggerHashName == triggerHash(CameraForward))
		camc->setFly(NSCamComp::DirPos, evnt->mToggle);

	if (evnt->mTriggerHashName == triggerHash(CameraBackward))
		camc->setFly(NSCamComp::DirNeg, evnt->mToggle);

	if (evnt->mTriggerHashName == triggerHash(CameraLeft))
		camc->setStrafe(NSCamComp::DirNeg, evnt->mToggle);

	if (evnt->mTriggerHashName == triggerHash(CameraRight))
		camc->setStrafe(NSCamComp::DirPos, evnt->mToggle);
	return true;
}

bool NSCameraSystem::_handleSelFocusEvent(NSSelFocusEvent * evnt)
{
	mFocusEnt = evnt->mFocID;

	if (mMode == Free)
		return true;
	NSScene * scn = nsengine.currentScene();
	if (scn == NULL)
		return true;
	NSEntity * cam = scn->camera();
	if (cam == NULL)
		return true;
	
	NSEntity * ent = scn->entity(evnt->mFocID.xy());
	if (ent != NULL)
	{
		NSTFormComp * tC = ent->get<NSTFormComp>();
		NSCamComp * camc = cam->get<NSCamComp>();
		NSTFormComp * camtc = cam->get<NSTFormComp>();
		
		if (evnt->mFocID.z < tC->count())
		{
			camtc->setOrientation(camc->focusOrientation() * camtc->orientation());
			camc->setFocusRot(fquat());
			camc->setFocusPoint(tC->wpos(evnt->mFocID.z));
			camtc->setpos(camtc->wpos() - camc->focusPoint());
		}
	}
	return true;
}

int32 NSCameraSystem::updatePriority()
{
	return CAM_SYS_UPDATE_PR;
}
