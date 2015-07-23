/*!
\file nscamcontroller.h

\brief Definition file for NSCamController class

This file contains all of the neccessary definitions for the NSCamController class.

\author Daniel Randle
\date March 8 2014
\copywrite Earth Banana Games 2013
*/

#include <nscamerasystem.h>
#include <nsentity.h>
#include <nsselectionsystem.h>
#include <nseventdispatcher.h>
#include <nscamcomp.h>
#include <nstimer.h>
#include <nsscene.h>
#include <nsinputcomp.h>

NSCameraSystem::NSCameraSystem() : NSSystem(),
mZoomFactor(DEFAULT_CAM_ZOOM_FACTOR),
mTurnSensitivity(DEFAULT_CAM_SENSITIVITY),
mStrafeSensitivity(DEFAULT_CAM_SENSITIVITY),
mFreeModeInverted(1, 1),
mFocusModeInverted(1 ,1),
mMode(Free),
mView(Normal)
{}

NSCameraSystem::~NSCameraSystem()
{}

//bool NSCameraSystem::handleEvent(NSEvent * pEvent)
//{
	// NSScene * scene = nsengine.currentScene();
	// if (scene == NULL)
	// 	return false;

	// if (pEvent->mID == NSEvent::SelFocusChangeEvent)
	// {
	// 	if (nsengine.system<NSCameraSystem>()->mode() != NSCameraSystem::Focus)
	// 		return false;

	// 	NSEntity * cam = scene->camera();
	// 	if (cam == NULL)
	// 		return false;

	// 	NSCamComp * camComp = cam->get<NSCamComp>();
	// 	NSTFormComp * camTComp = cam->get<NSTFormComp>();

	// 	NSSelFocusChangeEvent * selEvent = (NSSelFocusChangeEvent*)pEvent;
	// 	uivec3 foc = selEvent->mNewSelCenter;
	// 	NSEntity * ent = scene->entity(foc.x, foc.y);
	// 	if (ent != NULL)
	// 	{
	// 		NSTFormComp * tC = ent->get<NSTFormComp>();
	// 		if (tC->count() > foc.z)
	// 		{
	// 			camTComp->setOrientation(camTComp->orientation() * camComp->focusOrientation());
	// 			camComp->setFocusRot(fquat());
	// 			camComp->setFocusPoint(tC->lpos(foc.z));
	// 			camTComp->setpos(camTComp->wpos() - camComp->focusPoint());
	// 		}
	// 	}
	// }
	// return true;
//}

void NSCameraSystem::init()
{
	//nsengine.events()->addListener(this, NSEvent::SelFocusChangeEvent);
}

void NSCameraSystem::onCamBackward(NSCamComp * pCam, nsbool pAnimate)
{
	pCam->setFly(NSCamComp::DirNeg, pAnimate);
}

void NSCameraSystem::onCamForward(NSCamComp * pCam, nsbool pAnimate)
{
	pCam->setFly(NSCamComp::DirPos, pAnimate);
}

void NSCameraSystem::onCamLeft(NSCamComp * pCam, nsbool pAnimate)
{
	pCam->setStrafe(NSCamComp::DirNeg, pAnimate);
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

const nsfloat & NSCameraSystem::sensitivity(const Sensitivity & pWhich) const
{
	if (pWhich == Turn)
		return mTurnSensitivity;
	return mStrafeSensitivity;
}

const NSCameraSystem::CameraView & NSCameraSystem::view() const
{
	return mView;
}

nsfloat NSCameraSystem::zoom() const
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
		mFreeModeInverted.x = nsint(pInvert) * -2 + 1;
	else
		mFocusModeInverted.x = nsint(pInvert) * -2 + 1;
}

void NSCameraSystem::setyinv(bool pInvert, const CameraMode & pMode)
{
	if (pMode == Free)
		mFreeModeInverted.y = nsint(pInvert) * -2 + 1;
	else
		mFocusModeInverted.y = nsint(pInvert) * -2 + 1;
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
	mView = pView;

	NSScene * scene = nsengine.currentScene();
	if (scene == NULL)
		return;

	NSEntity * cam = scene->camera();
	if (cam == NULL)
		return;

	NSCamComp * camComp = cam->get<NSCamComp>();
	NSTFormComp * camTComp = cam->get<NSTFormComp>();
	bool switchMode = (mMode == Free);

	fquat quat;
	fvec3 rightV;
	fvec3 targetV;
	fvec3 upVec;
	nsfloat ang = 0.0f;

	// Find which view we are switching too and change to angle about the horizontal axis accordingly
	// For each view we have to set the mode to focus if it is in free mode and then set it back to free
	// after doing the rotation. This is because in focus mode it will rotate about the selected object, which
	// is what we want.
	switch (mView)
	{
	case (Top) :
		{
			fvec3 focPos;

			uivec3 selCenter = nsengine.system<NSSelectionSystem>()->center();
			NSEntity * ent = scene->entity(selCenter.x,selCenter.y);
			if (ent != NULL)
				focPos = ent->get<NSTFormComp>()->wpos(selCenter.y);

			//// If mode is Free then switch to focus
			if (switchMode)
				setMode(Focus);

			camComp->setFocusPoint(focPos);
			camComp->setFocusRot(fquat());
			camTComp->setpos(fvec3(0.0f, 0.0f, DEFAULT_CAM_VIEW_Z));
			camTComp->setOrientation(fquat());
			if (switchMode)
			{
				camTComp->setParent(camComp->camFocusTForm());
				setMode(Free);
			}
			break;
		}
	case (Iso) :
		{
			fvec3 focPos;

			uivec3 selCenter = nsengine.system<NSSelectionSystem>()->center();
			NSEntity * ent = scene->entity(selCenter.x, selCenter.y);
			if (ent != NULL)
				focPos = ent->get<NSTFormComp>()->wpos(selCenter.y);

			//// If mode is Free then switch to focus
			if (switchMode)
				setMode(Focus);

			camComp->setFocusRot(fquat());
			camComp->setFocusPoint(focPos);
			camComp->rotateFocus(1.0f, 0.0f, 0.0f, 45);
			camTComp->setpos(fvec3(0.0f, 0.0f, DEFAULT_CAM_VIEW_Z));
			camTComp->setOrientation(fquat());
			if (switchMode)
			{
				camTComp->setParent(camComp->camFocusTForm());
				setMode(Free);
			}
			break;
		}
	case (Front) :
		{
			fvec3 focPos;

			uivec3 selCenter = nsengine.system<NSSelectionSystem>()->center();
			NSEntity * ent = scene->entity(selCenter.x, selCenter.y);
			if (ent != NULL)
				focPos = ent->get<NSTFormComp>()->wpos(selCenter.y);

			//// If mode is Free then switch to focus
			if (switchMode)
				setMode(Focus);

			camComp->setFocusRot(fquat());
			camComp->setFocusPoint(focPos);
			camComp->rotateFocus(1.0f, 0.0f, 0.0f, 90);
			camTComp->setpos(fvec3(0.0f, 0.0f, DEFAULT_CAM_VIEW_Z));
			camTComp->setOrientation(fquat());
			if (switchMode)
			{
				camTComp->setParent(camComp->camFocusTForm());
				setMode(Free);
			}
			break;
		}
	}
	camComp->postUpdate(true);
}

void NSCameraSystem::setZoom(nsfloat pZFactor)
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

		camTComp->setOrientation(camTComp->orientation() * camComp->focusOrientation());
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
		uivec3 foc = nsengine.system<NSSelectionSystem>()->center();

		NSEntity * ent = scene->entity(foc.x, foc.y);
		if (ent != NULL)
		{
			NSTFormComp * tC = ent->get<NSTFormComp>();
			if (tC->count() > foc.z)
			{
				camTComp->setOrientation(camTComp->orientation() * camComp->focusOrientation());
				camComp->setFocusRot(fquat());
				camComp->setFocusPoint(tC->lpos(foc.z));
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

void NSCameraSystem::onCamMove(NSCamComp * pCam, NSTFormComp * tComp, const fvec2 & pDelta, const fvec2 & pPos)
{
	tComp->translate(NSTFormComp::Right, pDelta.u*mStrafeSensitivity * mFreeModeInverted.x);
	tComp->translate(NSTFormComp::Up, pDelta.v*mStrafeSensitivity * mFreeModeInverted.y);
	pCam->postUpdate(true);
}

void NSCameraSystem::onCamRight(NSCamComp * pCam, nsbool pAnimate)
{
	pCam->setStrafe(NSCamComp::DirPos, pAnimate);
}

void NSCameraSystem::onCamTurn(NSCamComp * pCam, NSTFormComp * tComp, const fvec2 & pDelta, const fvec2 & pPos)
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
		if (mView == Normal)
			pCam->rotateFocus((tComp->orientation() * pCam->focusOrientation()).right(), pDelta.v * -1.0f * mTurnSensitivity * mFocusModeInverted.y);
	}
	pCam->postUpdate(true);
}

void NSCameraSystem::onCamZoom(NSCamComp * pCam, NSTFormComp * tComp, nsfloat pScroll)
{
	NSCamComp::Direction dir = NSCamComp::DirPos;
	if (pScroll < 0)
		dir = NSCamComp::DirNeg;

	if (pCam->projectionMode() == NSCamComp::Persp)
		tComp->translate(NSTFormComp::Target, nsfloat(dir) * mZoomFactor);
	else
	{
		nsfloat factor = 0.95f;
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
	//nsengine.events()->process(this); // process all events for this system

	// Dont do anything if the scene is NULL
	if (scene == NULL)
		return;

	auto iter = scene->entities<NSCamComp>().begin();
	while (iter != scene->entities<NSCamComp>().end())
	{

		NSCamComp * camComp = (*iter)->get<NSCamComp>();
		NSInputComp * inComp = (*iter)->get<NSInputComp>();
		NSTFormComp * camTComp = (*iter)->get<NSTFormComp>();

		// Only process input if the camera is the current camera
		if (inComp != NULL && (*iter) == scene->camera())
		{
			NSInputComp::Action * act = inComp->action(CAM_ZOOM);
			if (act != NULL && act->mActivated)
				onCamZoom(camComp, camTComp, act->mScroll);

			act = inComp->action(CAM_MOVE);
			if (act != NULL && act->mActivated)
				onCamMove(camComp, camTComp, act->mDelta, act->mPos);

			act = inComp->action(CAM_TURN);
			if (act != NULL && act->mActivated)
				onCamTurn(camComp, camTComp, act->mDelta, act->mPos);

			act = inComp->action(CAM_FORWARD);
			if (act != NULL && act->mActivated)
				onCamForward(camComp, act->mPressed);

			act = inComp->action(CAM_BACKWARD);
			if (act != NULL && act->mActivated)
				onCamBackward(camComp, act->mPressed);

			act = inComp->action(CAM_LEFT);
			if (act != NULL && act->mActivated)
				onCamLeft(camComp, act->mPressed);

			act = inComp->action(CAM_RIGHT);
			if (act != NULL && act->mActivated)
				onCamRight(camComp, act->mPressed);
		}


		if (camComp->updatePosted())
		{

			if (camComp->strafe().mAnimate)
				camTComp->translate(NSTFormComp::Right, camComp->strafe().mDir * camComp->speed() * nsengine.timer()->fixed());
			if (camComp->elevate().mAnimate)
				camTComp->translate(NSTFormComp::Up, camComp->elevate().mDir * camComp->speed() * nsengine.timer()->fixed());
			if (camComp->fly().mAnimate)
				camTComp->translate(NSTFormComp::Target, camComp->fly().mDir * camComp->speed() * nsengine.timer()->fixed());

			if (!(camComp->strafe().mAnimate || camComp->elevate().mAnimate || camComp->fly().mAnimate))
				camComp->postUpdate(false);

			if (mMode == Focus)
				camTComp->setParent(camComp->camFocusTForm());
			else
				camTComp->setParent(fmat4());

			camComp->mProjCam = camComp->proj() * camTComp->pov();
			camComp->mInvProjCam = camTComp->transform() * camComp->invproj();
		}
		++iter;
	}

	// Update the skybox!
	NSEntity * skyDome = scene->skydome();
	if (skyDome != NULL)
	{
		NSTFormComp * camTComp = scene->camera()->get<NSTFormComp>();
		NSTFormComp * tComp = skyDome->get<NSTFormComp>();
		tComp->setpos(camTComp->wpos());
	}
}

nsint NSCameraSystem::updatePriority()
{
	return CAM_SYS_UPDATE_PR;
}
