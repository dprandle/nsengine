/*!
\file nsbuildsystem.cpp

\brief Definition file for nsbuild_system class

This file contains all of the neccessary definitions for the nsbuild_system class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#include <nsbuild_system.h>
#include <nsevent_dispatcher.h>
#include <nstile_brush_comp.h>
#include <nsevent.h>
#include <nsscene.h>
#include <nstile_comp.h>
#include <nstile_grid.h>
#include <nssel_comp.h>
#include <nsinput_comp.h>
#include <nsoccupy_comp.h>
#include <nsrender_system.h>
#include <nsselection_system.h>
#include <nsrender_comp.h>
#include <nsinput_map_manager.h>
#include <nsentity_manager.h>
#include <nsplugin.h>
#include <nsinput_system.h>

nsbuild_system::nsbuild_system() :
m_enabled(false),
m_tile_brush(NULL),
m_object_brush(NULL),
m_build_ent(NULL),
m_mirror_brush(NULL),
m_layer(0),
m_current_mode(build_mode),
m_current_brush_type(brush_none),
m_tile_brush_center_tform_id(0),
m_overwrite(false),
m_mirror_mode(false),
NSSystem()
{
	
}

nsbuild_system::~nsbuild_system()
{

}

void nsbuild_system::change_layer(const int32 & pAmount)
{
	m_layer += pAmount;
}

void nsbuild_system::enable(const bool & pEnable, const fvec2 & pMousePos)
{
	nsscene * scene = nsengine.currentScene();
	if (scene == NULL)
		return;
	
	if (pEnable && !m_enabled)
	{
		m_enabled = pEnable;
		nsengine.system<NSInputSystem>()->pushContext("BuildMode");
		//nsengine.eventDispatch()->send(new NSClearSelectionEvent("ClearSelection")); // process now

		if (m_current_brush_type == brush_tile)
		{
			if (m_tile_brush == NULL)
				return;

			if (m_mirror_mode)
			{
				m_mirror_brush = nsengine.engplug()->create<nsentity>("MirrorBrush");
				*m_mirror_brush = *m_tile_brush;
			}

			NSTileBrushComp * brushComp = m_tile_brush->get<NSTileBrushComp>();

			NSSelComp * selComp = m_tile_brush->get<NSSelComp>();
			if (selComp == NULL || brushComp == NULL)
				return;

			auto brushIter = brushComp->begin();
			while (brushIter != brushComp->end())
			{
				fvec3 pos = NSTileGrid::world(ivec3(brushIter->x, brushIter->y, m_layer)); // add in height when get working
				uint32 tFormID = scene->add(m_tile_brush, pos);

				// This tmp thing is a hack to get around added the mirrored guys to the selection
				bool tmp = m_mirror_mode;
				if (tmp)
					m_mirror_mode = false;

//				nsengine.eventDispatch()->send(new NSSelAddEvent("AddToSel", uivec3(mTileBrush->plugin_id(), mTileBrush->id(), tFormID)));

				if (tmp)
				{
					fvec3 mirrorPos = m_mirror_center*2.0f - pos;
					mirrorPos.z = pos.z;
					uint32 tFormIDMirror = scene->add(m_mirror_brush, mirrorPos);
//					nsengine.eventDispatch()->send(new NSSelAddEvent("AddToSel", uivec3(mMirrorBrush->plugin_id(), mMirrorBrush->id(), tFormIDMirror)));
					m_mirror_brush->get<NSTFormComp>()->setHiddenState(NSTFormComp::Hide, true, tFormIDMirror);
				}
				m_mirror_mode = tmp;

				if (*brushIter == ivec2())
					m_tile_brush_center_tform_id = tFormID;

				m_tile_brush->get<NSTFormComp>()->setHiddenState(NSTFormComp::Hide, true, tFormID);
				++brushIter;
			}
//			nsengine.eventDispatch()->send(new NSSelSetEvent("Center", uivec3(mTileBrush->plugin_id(), mTileBrush->id(), mTBCenterTFormID)));
			selComp->setSelected(true);
			to_cursor(pMousePos);


		}
		else if (m_current_brush_type == brush_object)
		{
			if (m_object_brush == NULL)
				return;

			if (m_build_ent == NULL)
				return;

			//nsengine.eventDispatch()->send(new NSSelSetEvent("Center", uivec3(mObjectBrush->plugin_id(), mObjectBrush->id(), 0) ) );

			m_object_brush->del<NSLightComp>();
			m_object_brush->del<NSOccupyComp>();
			m_object_brush->copy(m_build_ent->get<NSRenderComp>());
			m_object_brush->copy(m_build_ent->get<NSOccupyComp>());
			m_object_brush->copy(m_build_ent->get<NSLightComp>());

			NSSelComp * selComp = m_object_brush->get<NSSelComp>();
			if (selComp == NULL)
				return;

			uint32 tFormID = scene->add(m_object_brush);
			m_object_brush->get<NSTFormComp>()->setHiddenState(NSTFormComp::Hide, true, tFormID);
//			nsengine.eventDispatch()->send(new NSSelAddEvent("AddToSel", uivec3(mObjectBrush->plugin_id(), mObjectBrush->id(), tFormID)));
			selComp->setSelected(true);
			to_cursor(pMousePos);
		}
		else
			return;
	}
	else if (!pEnable && m_enabled)
	{
		m_enabled = pEnable;
		nsengine.system<NSInputSystem>()->popContext();

		if (m_object_brush != NULL)
		{
			m_object_brush->del<NSOccupyComp>();
			scene->remove(m_object_brush);
		}

		if (m_tile_brush != NULL)
			scene->remove(m_tile_brush);

		//nsengine.eventDispatch()->send(new NSClearSelectionEvent("ClearSelection")); // process now

		nsengine.engplug()->destroy<nsentity>("MirrorBrush");
		m_mirror_brush = NULL;
	}
}

void nsbuild_system::enable_overwrite(bool pEnable)
{
	m_overwrite = pEnable;
}

void nsbuild_system::enable_mirror(bool pEnable)
{
	m_mirror_mode = pEnable;
}

void nsbuild_system::erase()
{
	nsscene * scene = nsengine.currentScene();
	if (scene == NULL)
		return;

	if (m_current_brush_type == brush_tile)
	{
		if (m_tile_brush == NULL)
			return;

		NSTileBrushComp * brushComp = m_tile_brush->get<NSTileBrushComp>();
		if (brushComp == NULL)
			return;

		auto brushIter = brushComp->begin();
		while (brushIter != brushComp->end())
		{
			for (int32 i = 0; i < brushComp->height(); ++i)
			{
				fvec3 pos = m_tile_brush->get<NSTFormComp>()->lpos(m_tile_brush_center_tform_id) + NSTileGrid::world(ivec3(brushIter->x, brushIter->y, -i)); // add in height when get working
				scene->remove(pos);

				if (m_mirror_mode)
				{
					fvec3 newPos = m_mirror_center*2.0f - pos;
					newPos.z = pos.z;
					scene->remove(newPos);
				}
			}
			++brushIter;
		}
	}
}

const fvec4 nsbuild_system::active_brush_color() const
{
	if (m_current_brush_type == brush_tile && m_tile_brush != NULL)
		return m_tile_brush->get<NSSelComp>()->defaultColor();
	else if (m_current_brush_type == brush_object && m_object_brush != NULL)
		return m_object_brush->get<NSSelComp>()->defaultColor();
	return fvec4();
}

nsentity * nsbuild_system::build_ent()
{
	return m_build_ent;
}

const nsbuild_system::brush_t & nsbuild_system::brush_type()
{
	return m_current_brush_type;
}

const int32 & nsbuild_system::layer() const
{
	return m_layer;
}

const fvec3 & nsbuild_system::center() const
{
	return m_mirror_center;
}

const nsbuild_system::mode_t & nsbuild_system::mode() const
{
	return m_current_mode;
}

void nsbuild_system::init()
{
	//nsengine.eventDispatch()->addListener(this, NSEvent::InputKey);
}

bool nsbuild_system::enabled() const
{
	return m_enabled;
}

bool nsbuild_system::overwrite() const
{
	return m_overwrite;
}

bool nsbuild_system::mirror() const
{
	return m_mirror_mode;
}

// bool nsbuild_system::handleEvent(NSEvent * pEvent)
// {
// 	nsscene * scene = nsengine.currentScene();
// 	if (scene == NULL)
// 		return false;

// 	if (pEvent == NULL)
// 	{
// 		dprint("nsbuild_system::handleEvent Event is NULL - bad bad bad");
// 		return false;
// 	}

// 	if (pEvent->mID == NSEvent::InputKey)
// 	{
// 		NSInputKeyEvent * kEvent = (NSInputKeyEvent*)pEvent;
// 		// a lot of nonsense to get to this point - just put in debug mode if the debug
// 		// mode event is received
// 		if (kEvent->mName == "BuildMode" && kEvent->mPorR == 1)
// 		{
// 			setBrushMode(Tile);
// 			toggle(kEvent->mMousePos);
// 			return true;
// 		}
// 	}
// 	return false;
// }

void nsbuild_system::to_cursor(const fvec2 & pCursorPos, bool pUpdateCamFirst)
{
	nsscene * scene = nsengine.currentScene();
	if (scene == NULL)
		return;

	nsentity * camera = scene->camera();
	if (camera == NULL)
		return;

	NSTFormComp * camTForm = camera->get<NSTFormComp>();
	NSCamComp * camc = camera->get<NSCamComp>();

	NSTFormComp * brushTForm = NULL;
	fvec3 originalPos;

	if (m_current_brush_type == brush_tile)
	{
		if (m_tile_brush == NULL)
			return;
		brushTForm = m_tile_brush->get<NSTFormComp>();
		if (brushTForm == NULL)
			return;
		originalPos = brushTForm->lpos(m_tile_brush_center_tform_id);
	}
	else if (m_current_brush_type == brush_object)
	{
		if (m_object_brush == NULL)
			return;
		brushTForm = m_object_brush->get<NSTFormComp>();
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
	float denom = normal * castVec;
	float depth = 0.0f;

	if (abs(denom) >= EPS)
		depth = (normal * (originalPos - fpos)) / denom;
	
	fpos += castVec*depth;
	fpos -= originalPos;
	
	for (uint32 i = 0; i < brushTForm->count(); ++i)
	{
		brushTForm->translate(fpos, i);
		if (m_mirror_mode)
		{
			NSTFormComp * mMirrorT = m_mirror_brush->get<NSTFormComp>();
			fvec3 wp = brushTForm->wpos(i);
			fvec3 newPos = m_mirror_center*2.0f - wp;
			newPos.z = wp.z;
			mMirrorT->setpos(newPos, i);
		}
	}
}

nsentity * nsbuild_system::tile_brush()
{
	return m_tile_brush;
}

nsentity * nsbuild_system::object_brush()
{
	return m_object_brush;
}

void nsbuild_system::paint()
{
	nsscene * scene = nsengine.currentScene();
	if (scene == NULL)
		return;

	if (m_build_ent == NULL)
		return;

	if (m_current_brush_type == brush_tile)
	{
		if (m_tile_brush == NULL)
			return;

		NSTileBrushComp * brushComp = m_tile_brush->get<NSTileBrushComp>();
		if (brushComp == NULL)
			return;

		auto brushIter = brushComp->begin();
		while (brushIter != brushComp->end())
		{
			for (int32 i = 0; i < brushComp->height(); ++i)
			{
				fvec3 pos = m_tile_brush->get<NSTFormComp>()->lpos(m_tile_brush_center_tform_id) + NSTileGrid::world(ivec3(brushIter->x, brushIter->y, -i)); // add in height when get working
				
				if (m_overwrite)
				{
					scene->remove(pos);
					if (m_mirror_mode)
					{
						fvec3 newPos = m_mirror_center*2.0f - pos;
						newPos.z = pos.z;
						scene->remove(newPos);
					}
				}

				uint32 tFormID = scene->add(m_build_ent, pos);
				
				if (tFormID != -1)
				{
					if (m_mirror_mode)
					{
						fvec3 newPos = m_mirror_center*2.0f - pos;
						newPos.z = pos.z;
						uint32 tFormMID = scene->add(m_build_ent, newPos);
						if (tFormMID == -1)
						{
							scene->remove(m_build_ent, tFormID);
							continue;
						}
						m_build_ent->get<NSTFormComp>()->snap(tFormMID);
					}
					m_build_ent->get<NSTFormComp>()->snap(tFormID);
				}
			}
			++brushIter;
		}
	}
	else if (m_current_brush_type == brush_object)
	{
		if (m_object_brush == NULL)
			return;

		fvec3 pos = m_object_brush->get<NSTFormComp>()->lpos();
		uint32 tFormID = scene->add(m_build_ent, pos);
		if (tFormID != -1)
			m_build_ent->get<NSTFormComp>()->snap(tFormID);
	}
}

void nsbuild_system::set_active_brush_color(const fvec4 & pColor)
{
	if (m_current_brush_type == brush_tile && m_tile_brush != NULL)
	{
		m_tile_brush->get<NSSelComp>()->setDefaultColor(pColor);
		if (m_mirror_mode)
		{
			if (m_mirror_brush != NULL)
				m_mirror_brush->get<NSSelComp>()->setDefaultColor(pColor);
		}
	}
	else if (m_current_brush_type == brush_object && m_object_brush != NULL)
		m_object_brush->get<NSSelComp>()->setDefaultColor(pColor);
}

void nsbuild_system::set_brush_type(const brush_t & pBrushMode)
{
	m_current_brush_type = pBrushMode;
	if (m_build_ent == NULL)
		return;

	switch (m_current_brush_type)
	{
	case (brush_none) :
		m_build_ent = NULL;
		break;
	case (brush_tile):
		if (!m_build_ent->has<NSTileComp>())
			m_build_ent = NULL;
		break;
	case (brush_object):
		if (m_build_ent->has<NSTileComp>())
			m_build_ent = NULL;
		break;
	}
}

void nsbuild_system::set_object_brush(nsentity * pBrush)
{
	if (m_enabled)
		toggle(fvec2());

	m_object_brush = pBrush;
}

void nsbuild_system::set_tile_brush(nsentity * pBrush)
{
	if (m_enabled)
		toggle(fvec2());

	m_tile_brush = pBrush;
}

void nsbuild_system::set_build_ent(nsentity * pBuildEnt)
{
	if (pBuildEnt == NULL)
	{
		m_build_ent = pBuildEnt;
		return;
	}

	if (pBuildEnt->has<NSTileComp>())
	{
		if (m_current_brush_type == brush_tile)
			m_build_ent = pBuildEnt;
		return;
	}
	else
	{
		if (m_current_brush_type == brush_object)
			m_build_ent = pBuildEnt;
		return;
	}
}

void nsbuild_system::set_mode(const mode_t & pMode)
{
	m_current_mode = pMode;
	if (pMode != build_mode)
		set_brush_type(brush_tile);
}

void nsbuild_system::set_layer(const int32 & pLayer)
{
	m_layer = pLayer;
}

void nsbuild_system::setCenter(const fvec3 & pMirrorCenter)
{
	m_mirror_center = pMirrorCenter;
}

void nsbuild_system::toggle(const fvec2 & pMousePos)
{
	enable(!m_enabled, pMousePos);
}

int32 nsbuild_system::update_priority()
{
	return BUILD_SYS_UPDATE_PR;
}

void nsbuild_system::update()
{
	nsscene * scene = nsengine.currentScene();
	if (scene == NULL)
		return;

	nsengine.eventDispatch()->process(this);
	if (scene == NULL)
		return;


	if (m_enabled)
	{
		nsentity * cam = scene->camera();
		if (cam == NULL)
			return;

		NSInputComp * inComp = cam->get<NSInputComp>();
		if (inComp != NULL)
		{
			NSInputComp::Action * act = inComp->action(CAM_ZOOM);
			if (act != NULL && act->mActivated)
				to_cursor(act->mPos, true);

			act = inComp->action(CAM_MOVE);
			if (act != NULL && act->mActivated)
				to_cursor(act->mPos);

			act = inComp->action(CAM_TURN);
			if (act != NULL && act->mActivated)
				to_cursor(act->mPos);

			act = inComp->action(CAM_FORWARD);
			if (act != NULL && act->mActivated)
				to_cursor(act->mPos);

			act = inComp->action(CAM_BACKWARD);
			if (act != NULL && act->mActivated)
				to_cursor(act->mPos);

			act = inComp->action(CAM_LEFT);
			if (act != NULL && act->mActivated)
				to_cursor(act->mPos);

			act = inComp->action(CAM_RIGHT);
			if (act != NULL && act->mActivated)
				to_cursor(act->mPos);
		}

		if (m_current_brush_type == brush_tile)
		{
			if (m_tile_brush == NULL)
				return;

			NSInputComp * brushInComp = m_tile_brush->get<NSInputComp>();
			if (brushInComp != NULL)
			{
				NSInputComp::Action * act = brushInComp->action(INSERT_OBJECT);
				if (act != NULL && act->mActivated && act->mPressed)
				{
					switch (m_current_mode)
					{
					case(build_mode) :
						paint();
						break;
					case(erase_mode) :
						erase();
						break;
					}
					to_cursor(act->mPos);
				}
			}
		}
		else
		{
			if (m_object_brush == NULL)
				return;

			NSInputComp * brushInComp = m_object_brush->get<NSInputComp>();
			if (brushInComp != NULL)
			{
				NSInputComp::Action * act = brushInComp->action(INSERT_OBJECT);
				if (act != NULL && act->mActivated && act->mPressed)
				{
					switch (m_current_mode)
					{
					case(build_mode) :
						paint();
						break;
					case(erase_mode) :
						erase();
						break;
					}
					to_cursor(act->mPos);
				}
			}
		}
	}
}
