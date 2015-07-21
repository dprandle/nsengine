/*!
\file nsbuildsystem.cpp

\brief Definition file for NSBuildSystem class

This file contains all of the neccessary definitions for the NSBuildSystem class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#include <nsbuildsystem.h>
#include <nseventdispatcher.h>
#include <nstilebrushcomp.h>
#include <nsevent.h>
#include <nsscene.h>
#include <nstilecomp.h>
#include <nstilegrid.h>
#include <nsselcomp.h>
#include <nsinputcomp.h>
#include <nsoccupycomp.h>
#include <nsrendersystem.h>
#include <nsselectionsystem.h>
#include <nsrendercomp.h>
#include <nsinputmapmanager.h>
#include <nsentitymanager.h>
#include <nsplugin.h>
#include <nsinputsystem.h>

NSBuildSystem::NSBuildSystem() :
mEnabled(false),
mTileBrush(NULL),
mObjectBrush(NULL),
mBuildEnt(NULL),
mMirrorBrush(NULL),
mLayer(0),
mCurrentMode(Build),
mCurrentBrushMode(None),
mTBCenterTFormID(0),
mOverwrite(false),
mMirrorMode(false),
NSSystem()
{
	
}

NSBuildSystem::~NSBuildSystem()
{

}

void NSBuildSystem::changeLayer(const nsint & pAmount)
{
	mLayer += pAmount;
}

void NSBuildSystem::enable(const bool & pEnable, const fvec2 & pMousePos)
{
	NSScene * scene = nsengine.currentScene();
	if (scene == NULL)
		return;
	
	if (pEnable && !mEnabled)
	{
		mEnabled = pEnable;
		nsengine.system<NSInputSystem>()->pushContext("BuildMode");
		nsengine.events()->send(new NSClearSelectionEvent("ClearSelection")); // process now

		if (mCurrentBrushMode == Tile)
		{
			if (mTileBrush == NULL)
				return;

			if (mMirrorMode)
			{
				mMirrorBrush = nsengine.engplug()->create<NSEntity>("MirrorBrush");
				*mMirrorBrush = *mTileBrush;
			}

			NSTileBrushComp * brushComp = mTileBrush->get<NSTileBrushComp>();

			NSSelComp * selComp = mTileBrush->get<NSSelComp>();
			if (selComp == NULL || brushComp == NULL)
				return;

			auto brushIter = brushComp->begin();
			while (brushIter != brushComp->end())
			{
				fvec3 pos = NSTileGrid::world(ivec3(brushIter->x, brushIter->y, mLayer)); // add in height when get working
				nsuint tFormID = scene->add(mTileBrush, pos);

				// This tmp thing is a hack to get around added the mirrored guys to the selection
				bool tmp = mMirrorMode;
				if (tmp)
					mMirrorMode = false;

				nsengine.events()->send(new NSSelAddEvent("AddToSel", uivec3(mTileBrush->plugid(), mTileBrush->id(), tFormID)));

				if (tmp)
				{
					fvec3 mirrorPos = mMirrorCenter*2.0f - pos;
					mirrorPos.z = pos.z;
					nsuint tFormIDMirror = scene->add(mMirrorBrush, mirrorPos);
					nsengine.events()->send(new NSSelAddEvent("AddToSel", uivec3(mMirrorBrush->plugid(), mMirrorBrush->id(), tFormIDMirror)));
					mMirrorBrush->get<NSTFormComp>()->setHiddenState(NSTFormComp::Hide, true, tFormIDMirror);
				}
				mMirrorMode = tmp;

				if (*brushIter == ivec2())
					mTBCenterTFormID = tFormID;

				mTileBrush->get<NSTFormComp>()->setHiddenState(NSTFormComp::Hide, true, tFormID);
				++brushIter;
			}
			nsengine.events()->send(new NSSelSetEvent("Center", uivec3(mTileBrush->plugid(), mTileBrush->id(), mTBCenterTFormID)));
			selComp->setSelected(true);
			toCursor(pMousePos);


		}
		else if (mCurrentBrushMode == Object)
		{
			if (mObjectBrush == NULL)
				return;

			if (mBuildEnt == NULL)
				return;

			nsengine.events()->send(new NSSelSetEvent("Center", uivec3(mObjectBrush->plugid(), mObjectBrush->id(), 0) ) );

			mObjectBrush->del<NSLightComp>();
			mObjectBrush->del<NSOccupyComp>();
			mObjectBrush->copy(mBuildEnt->get<NSRenderComp>());
			mObjectBrush->copy(mBuildEnt->get<NSOccupyComp>());
			mObjectBrush->copy(mBuildEnt->get<NSLightComp>());

			NSSelComp * selComp = mObjectBrush->get<NSSelComp>();
			if (selComp == NULL)
				return;

			nsuint tFormID = scene->add(mObjectBrush);
			mObjectBrush->get<NSTFormComp>()->setHiddenState(NSTFormComp::Hide, true, tFormID);
			nsengine.events()->send(new NSSelAddEvent("AddToSel", uivec3(mObjectBrush->plugid(), mObjectBrush->id(), tFormID)));
			selComp->setSelected(true);
			toCursor(pMousePos);
		}
		else
			return;
	}
	else if (!pEnable && mEnabled)
	{
		mEnabled = pEnable;
		nsengine.system<NSInputSystem>()->popContext();

		if (mObjectBrush != NULL)
		{
			mObjectBrush->del<NSOccupyComp>();
			scene->remove(mObjectBrush);
		}

		if (mTileBrush != NULL)
			scene->remove(mTileBrush);

		nsengine.events()->send(new NSClearSelectionEvent("ClearSelection")); // process now

		nsengine.engplug()->destroy<NSEntity>("MirrorBrush");
		mMirrorBrush = NULL;
	}
}

void NSBuildSystem::enableOverwrite(nsbool pEnable)
{
	mOverwrite = pEnable;
}

void NSBuildSystem::enableMirror(nsbool pEnable)
{
	mMirrorMode = pEnable;
}

void NSBuildSystem::erase()
{
	NSScene * scene = nsengine.currentScene();
	if (scene == NULL)
		return;

	if (mCurrentBrushMode == Tile)
	{
		if (mTileBrush == NULL)
			return;

		NSTileBrushComp * brushComp = mTileBrush->get<NSTileBrushComp>();
		if (brushComp == NULL)
			return;

		auto brushIter = brushComp->begin();
		while (brushIter != brushComp->end())
		{
			for (nsint i = 0; i < brushComp->height(); ++i)
			{
				fvec3 pos = mTileBrush->get<NSTFormComp>()->lpos(mTBCenterTFormID) + NSTileGrid::world(ivec3(brushIter->x, brushIter->y, -i)); // add in height when get working
				scene->remove(pos);

				if (mMirrorMode)
				{
					fvec3 newPos = mMirrorCenter*2.0f - pos;
					newPos.z = pos.z;
					scene->remove(newPos);
				}
			}
			++brushIter;
		}
	}
}

const fvec4 NSBuildSystem::activeBrushColor() const
{
	if (mCurrentBrushMode == Tile && mTileBrush != NULL)
		return mTileBrush->get<NSSelComp>()->defaultColor();
	else if (mCurrentBrushMode == Object && mObjectBrush != NULL)
		return mObjectBrush->get<NSSelComp>()->defaultColor();
	return fvec4();
}

NSEntity * NSBuildSystem::buildent()
{
	return mBuildEnt;
}

const NSBuildSystem::BrushMode & NSBuildSystem::brushMode()
{
	return mCurrentBrushMode;
}

const nsint & NSBuildSystem::layer() const
{
	return mLayer;
}

const fvec3 & NSBuildSystem::center() const
{
	return mMirrorCenter;
}

const NSBuildSystem::Mode & NSBuildSystem::mode() const
{
	return mCurrentMode;
}

void NSBuildSystem::init()
{
	nsengine.events()->addListener(this, NSEvent::InputKey);
}

bool NSBuildSystem::enabled() const
{
	return mEnabled;
}

nsbool NSBuildSystem::overwrite() const
{
	return mOverwrite;
}

nsbool NSBuildSystem::mirror() const
{
	return mMirrorMode;
}

bool NSBuildSystem::handleEvent(NSEvent * pEvent)
{
	NSScene * scene = nsengine.currentScene();
	if (scene == NULL)
		return false;

	if (pEvent == NULL)
	{
		dprint("NSBuildSystem::handleEvent Event is NULL - bad bad bad");
		return false;
	}

	if (pEvent->mID == NSEvent::InputKey)
	{
		NSInputKeyEvent * kEvent = (NSInputKeyEvent*)pEvent;
		// a lot of nonsense to get to this point - just put in debug mode if the debug
		// mode event is received
		if (kEvent->mName == "BuildMode" && kEvent->mPorR == 1)
		{
			setBrushMode(Tile);
			toggle(kEvent->mMousePos);
			return true;
		}
	}
	return false;
}

void NSBuildSystem::toCursor(const fvec2 & pCursorPos, bool pUpdateCamFirst)
{
	NSScene * scene = nsengine.currentScene();
	if (scene == NULL)
		return;

	NSEntity * camera = scene->camera();
	if (camera == NULL)
		return;

	NSTFormComp * camTForm = camera->get<NSTFormComp>();
	NSCamComp * camc = camera->get<NSCamComp>();

	NSTFormComp * brushTForm = NULL;
	fvec3 originalPos;

	if (mCurrentBrushMode == Tile)
	{
		if (mTileBrush == NULL)
			return;
		brushTForm = mTileBrush->get<NSTFormComp>();
		if (brushTForm == NULL)
			return;
		originalPos = brushTForm->lpos(mTBCenterTFormID);
	}
	else if (mCurrentBrushMode == Object)
	{
		if (mObjectBrush == NULL)
			return;
		brushTForm = mObjectBrush->get<NSTFormComp>();
		if (brushTForm == NULL)
			return;
		originalPos = brushTForm->lpos();
	}
	else
		return;

	if (pUpdateCamFirst)
		camTForm->computeTransform();

	fvec4 screenSpace = camc->projCam() * fvec4(originalPos, 1.0f);

	if (abs(screenSpace.w) <= EPS)
		return;

	screenSpace /= screenSpace.w;
	screenSpace.x = (2*pCursorPos.u-1);
	screenSpace.y = (2*pCursorPos.v-1);

	fvec4 newPos = camc->invProjCam() * screenSpace;

	if (abs(newPos.w) <= EPS)
		return;

	newPos /= newPos.w;

	fvec3 fpos(newPos.xyz());
	fvec3 castVec = fpos - camTForm->wpos();
	castVec.normalize();

	fvec3 normal(0.0f,0.0f,-1.0f);
	nsfloat denom = normal * castVec;
	float depth = 0.0f;

	if (abs(denom) >= EPS)
		depth = (normal * (originalPos - fpos)) / denom;
	
	fpos += castVec*depth;
	fpos -= originalPos;
	
	for (nsuint i = 0; i < brushTForm->count(); ++i)
	{
		brushTForm->translate(fpos, i);
		if (mMirrorMode)
		{
			NSTFormComp * mMirrorT = mMirrorBrush->get<NSTFormComp>();
			fvec3 wp = brushTForm->wpos(i);
			fvec3 newPos = mMirrorCenter*2.0f - wp;
			newPos.z = wp.z;
			mMirrorT->setpos(newPos, i);
		}
	}
}

NSEntity * NSBuildSystem::tilebrush()
{
	return mTileBrush;
}

NSEntity * NSBuildSystem::objectbrush()
{
	return mObjectBrush;
}

void NSBuildSystem::paint()
{
	NSScene * scene = nsengine.currentScene();
	if (scene == NULL)
		return;

	if (mBuildEnt == NULL)
		return;

	if (mCurrentBrushMode == Tile)
	{
		if (mTileBrush == NULL)
			return;

		NSTileBrushComp * brushComp = mTileBrush->get<NSTileBrushComp>();
		if (brushComp == NULL)
			return;

		auto brushIter = brushComp->begin();
		while (brushIter != brushComp->end())
		{
			for (nsint i = 0; i < brushComp->height(); ++i)
			{
				fvec3 pos = mTileBrush->get<NSTFormComp>()->lpos(mTBCenterTFormID) + NSTileGrid::world(ivec3(brushIter->x, brushIter->y, -i)); // add in height when get working
				
				if (mOverwrite)
				{
					scene->remove(pos);
					if (mMirrorMode)
					{
						fvec3 newPos = mMirrorCenter*2.0f - pos;
						newPos.z = pos.z;
						scene->remove(newPos);
					}
				}

				nsuint tFormID = scene->add(mBuildEnt, pos);
				
				if (tFormID != -1)
				{
					if (mMirrorMode)
					{
						fvec3 newPos = mMirrorCenter*2.0f - pos;
						newPos.z = pos.z;
						nsuint tFormMID = scene->add(mBuildEnt, newPos);
						if (tFormMID == -1)
						{
							scene->remove(mBuildEnt, tFormID);
							continue;
						}
						mBuildEnt->get<NSTFormComp>()->snap(tFormMID);
					}
					mBuildEnt->get<NSTFormComp>()->snap(tFormID);
				}
			}
			++brushIter;
		}
	}
	else if (mCurrentBrushMode == Object)
	{
		if (mObjectBrush == NULL)
			return;

		fvec3 pos = mObjectBrush->get<NSTFormComp>()->lpos();
		nsuint tFormID = scene->add(mBuildEnt, pos);
		if (tFormID != -1)
			mBuildEnt->get<NSTFormComp>()->snap(tFormID);
	}
}

void NSBuildSystem::setActiveBrushColor(const fvec4 & pColor)
{
	if (mCurrentBrushMode == Tile && mTileBrush != NULL)
	{
		mTileBrush->get<NSSelComp>()->setDefaultColor(pColor);
		if (mMirrorMode)
		{
			if (mMirrorBrush != NULL)
				mMirrorBrush->get<NSSelComp>()->setDefaultColor(pColor);
		}
	}
	else if (mCurrentBrushMode == Object && mObjectBrush != NULL)
		mObjectBrush->get<NSSelComp>()->setDefaultColor(pColor);
}

void NSBuildSystem::setBrushMode(const BrushMode & pBrushMode)
{
	mCurrentBrushMode = pBrushMode;
	if (mBuildEnt == NULL)
		return;

	switch (mCurrentBrushMode)
	{
	case (None) :
		mBuildEnt = NULL;
		break;
	case (Tile):
		if (!mBuildEnt->has<NSTileComp>())
			mBuildEnt = NULL;
		break;
	case (Object):
		if (mBuildEnt->has<NSTileComp>())
			mBuildEnt = NULL;
		break;
	}
}

void NSBuildSystem::setObjectBrush(NSEntity * pBrush)
{
	if (mEnabled)
		toggle(fvec2());

	mObjectBrush = pBrush;
}

void NSBuildSystem::setTileBrush(NSEntity * pBrush)
{
	if (mEnabled)
		toggle(fvec2());

	mTileBrush = pBrush;
}

void NSBuildSystem::setBuildEnt(NSEntity * pBuildEnt)
{
	if (pBuildEnt == NULL)
	{
		mBuildEnt = pBuildEnt;
		return;
	}

	if (pBuildEnt->has<NSTileComp>())
	{
		if (mCurrentBrushMode == Tile)
			mBuildEnt = pBuildEnt;
		return;
	}
	else
	{
		if (mCurrentBrushMode == Object)
			mBuildEnt = pBuildEnt;
		return;
	}
}

void NSBuildSystem::setMode(const Mode & pMode)
{
	mCurrentMode = pMode;
	if (pMode != Build)
		setBrushMode(Tile);
}

void NSBuildSystem::setLayer(const nsint & pLayer)
{
	mLayer = pLayer;
}

void NSBuildSystem::setCenter(const fvec3 & pMirrorCenter)
{
	mMirrorCenter = pMirrorCenter;
}

void NSBuildSystem::toggle(const fvec2 & pMousePos)
{
	enable(!mEnabled, pMousePos);
}

nsint NSBuildSystem::updatePriority()
{
	return BUILD_SYS_UPDATE_PR;
}

void NSBuildSystem::update()
{
	NSScene * scene = nsengine.currentScene();
	if (scene == NULL)
		return;

	nsengine.events()->process(this);
	if (scene == NULL)
		return;


	if (mEnabled)
	{
		NSEntity * cam = scene->camera();
		if (cam == NULL)
			return;

		NSInputComp * inComp = cam->get<NSInputComp>();
		if (inComp != NULL)
		{
			NSInputComp::Action * act = inComp->action(CAM_ZOOM);
			if (act != NULL && act->mActivated)
				toCursor(act->mPos, true);

			act = inComp->action(CAM_MOVE);
			if (act != NULL && act->mActivated)
				toCursor(act->mPos);

			act = inComp->action(CAM_TURN);
			if (act != NULL && act->mActivated)
				toCursor(act->mPos);

			act = inComp->action(CAM_FORWARD);
			if (act != NULL && act->mActivated)
				toCursor(act->mPos);

			act = inComp->action(CAM_BACKWARD);
			if (act != NULL && act->mActivated)
				toCursor(act->mPos);

			act = inComp->action(CAM_LEFT);
			if (act != NULL && act->mActivated)
				toCursor(act->mPos);

			act = inComp->action(CAM_RIGHT);
			if (act != NULL && act->mActivated)
				toCursor(act->mPos);
		}

		if (mCurrentBrushMode == Tile)
		{
			if (mTileBrush == NULL)
				return;

			NSInputComp * brushInComp = mTileBrush->get<NSInputComp>();
			if (brushInComp != NULL)
			{
				NSInputComp::Action * act = brushInComp->action(INSERT_OBJECT);
				if (act != NULL && act->mActivated && act->mPressed)
				{
					switch (mCurrentMode)
					{
					case(Build) :
						paint();
						break;
					case(Erase) :
						erase();
						break;
					}
					toCursor(act->mPos);
				}
			}
		}
		else
		{
			if (mObjectBrush == NULL)
				return;

			NSInputComp * brushInComp = mObjectBrush->get<NSInputComp>();
			if (brushInComp != NULL)
			{
				NSInputComp::Action * act = brushInComp->action(INSERT_OBJECT);
				if (act != NULL && act->mActivated && act->mPressed)
				{
					switch (mCurrentMode)
					{
					case(Build) :
						paint();
						break;
					case(Erase) :
						erase();
						break;
					}
					toCursor(act->mPos);
				}
			}
		}
	}
}
