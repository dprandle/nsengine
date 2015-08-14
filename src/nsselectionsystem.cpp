 /*!
\file nscontrollersystem.h

\brief Definition file for NSControllerSystem class

This file contains all of the neccessary definitions for the NSControllerSystem class.

\author Daniel Randle
\date March 8 2014
\copywrite Earth Banana Games 2013
*/

#include <iostream>

#include <nsselectionsystem.h>
#include <nsscenemanager.h>
#include <nsscene.h>
#include <nseventdispatcher.h>
#include <nsrendersystem.h>
#include <nsinputcomp.h>
#include <nsselcomp.h>
#include <nsmeshmanager.h>
#include <nstilecomp.h>
#include <nsshader.h>
#include <nsevent.h>
#include <nsentitymanager.h>
#include <nsoccupycomp.h>
#include <nsmatmanager.h>
#include <nstilegrid.h>
#include <nstilebrushcomp.h>
#include <nsbuildsystem.h>
#include <nscamcomp.h>
#include <nsterraincomp.h>
#include <nsplugin.h>
#include <nsevent.h>
#include <nsmath.h>
/*
Notes:

Need to fix the bug where when holding z in build mode and clicking to select is still selecting tiles

*/


NSSelectionSystem::NSSelectionSystem() :
	mFocusEnt(),
	mPickPos(),
	mSelectedEnts(),
	selShader(NULL),
	mCachedPoint(),
	mMoving(false),
	mLayerMode(false),
	mLayer(0),
	mCachedLPoint(),
	drawOcc(false),
	finalBuf(0),
	pickBuf(0),
	trans(),
	mToggleMove(false),
	mSendFocEvent(false),
	NSSystem()
{}

NSSelectionSystem::~NSSelectionSystem()
{}

bool NSSelectionSystem::add(NSEntity * ent, uint32 tformid)
{
	if (ent == NULL)
		return false;

	NSSelComp * selComp = ent->get<NSSelComp>();
	if (selComp == NULL)
		return false;

	// TODO: Make this an event - that is a mirror mode event to put build and sel system in mirror mode
	if (nsengine.system<NSBuildSystem>()->mirror())
	{
		NSScene * scn = nsengine.currentScene();
		if (scn == NULL)
			return false;
		
		NSTFormComp * tForm = ent->get<NSTFormComp>();
		if (tForm == NULL)
		{
			dprint("NSSelectionSystem::add tForm is null for ent " + ent->name());
			return false;
		}

		fvec3 wp = tForm->wpos(tformid);
		fvec3 newPos = nsengine.system<NSBuildSystem>()->center()*2.0f - wp;
		newPos.z = wp.z;

		uivec3 id = scn->grid().get(newPos);
		if (id != 0)
		{
			NSEntity * entAdd = scn->entity(id.x,id.y);
			mSelectedEnts.insert(entAdd);
			entAdd->get<NSSelComp>()->setSelected(true);
			entAdd->get<NSSelComp>()->add(id.y);
		}
	}
	mSelectedEnts.insert(ent);
	selComp->setSelected(true);
	return selComp->add(tformid);
}

bool NSSelectionSystem::addToGrid()
{
	NSScene * scene = nsengine.currentScene();
	if (scene == NULL)
		return false;

	auto iter = mSelectedEnts.begin();
	while (iter != mSelectedEnts.end())
	{
		NSSelComp * selComp = (*iter)->get<NSSelComp>();
		NSTFormComp * tComp = (*iter)->get<NSTFormComp>();
		NSOccupyComp * occComp = (*iter)->get<NSOccupyComp>();

		if (occComp != NULL)
		{
			auto selIter = selComp->begin();
			while (selIter != selComp->end())
			{
				if (!scene->grid().add(uivec3((*iter)->plugid(), (*iter)->id(), *selIter), occComp->spaces(), tComp->lpos(*selIter)))
				{
					dprint("Could not add selection to grid");
					return false;
				}
				++selIter;
			}
		}
		++iter;
	}
	return true;
}

bool NSSelectionSystem::contains(const uivec3 & itemid)
{
	auto iter = mSelectedEnts.begin();
	while (iter != mSelectedEnts.end())
	{
		if ((*iter)->fullid() == itemid.xy())
		{
			if ((*iter)->get<NSSelComp>()->contains(itemid.z))
				return true;
		}
		++iter;
	}
	return false;
}

// bool NSSelectionSystem::handleEvent(NSEvent * pEvent)
// {
// 	if (pEvent->mID == NSEvent::SelPick)
// 	{
// 		NSSelPickEvent * selEvent = (NSSelPickEvent*)pEvent;
// 		mPickPos = selEvent->mPickPos;
// 		return true;
// 	}
// 	else if (pEvent->mID == NSEvent::ClearSelection)
// 	{
// 		clear();
// 		return true;
// 	}
// 	else if (pEvent->mID == NSEvent::SelSet)
// 	{
// 		NSSelSetEvent * selEvent = (NSSelSetEvent*)pEvent;
// 		mFocusEnt = selEvent->mEntRefID;
// 		//set(mScene->get(mFocusEnt.y)->get<NSSelComp>(), mFocusEnt.z);
// 		return true;
// 	}
// 	else if (pEvent->mID == NSEvent::SelAdd)
// 	{
// 		// This will handle any add to selection events denoted as SelAdd
// 		NSSelSetEvent * selEvent = (NSSelSetEvent*)pEvent; // Get the specific event

// 		// Get the selection entity and make sure it is valid
// 		NSEntity * selEnt = nsengine.resource<NSEntity>(selEvent->mEntRefID.x, selEvent->mEntRefID.y);
// 		if (selEnt == NULL)
// 		{
// 			dprint("NSSelectionSystem::handleEvent Selection entity sent in event is NULL");
// 			return false;
// 		}

// 		// Add the selection to the current selection
// 		add(selEnt, selEvent->mEntRefID.z);
// 		return true;
// 	}
// 	return false;
// }

void NSSelectionSystem::changeLayer(int32 pChange)
{
	removeFromGrid();
	translate(fvec3(0, 0, pChange*-Z_GRID));
	if (!collision())
		translate(fvec3(0, 0, pChange*Z_GRID));
	snapZ();
	addToGrid();
}

bool NSSelectionSystem::collision()
{
	NSScene * scene = nsengine.currentScene();
	if (scene == NULL)
		return false;

	bool noCollision = true;
	auto iter = mSelectedEnts.begin();
	while (iter != mSelectedEnts.end())
	{
		NSSelComp * selComp = (*iter)->get<NSSelComp>();
		NSTFormComp * tComp = (*iter)->get<NSTFormComp>();
		NSOccupyComp * occComp = (*iter)->get<NSOccupyComp>();
		if (occComp != NULL)
		{
			auto selIter = selComp->begin();
			while (selIter != selComp->end())
			{
				noCollision = !scene->grid().occupied(occComp->spaces(), tComp->wpos(*selIter)) && noCollision;
				++selIter;
			}
		}			
		++iter;
	}
	return noCollision;
}

void NSSelectionSystem::clear()
{
	auto iter = mSelectedEnts.begin();
	while (iter != mSelectedEnts.end())
	{
		(*iter)->get<NSSelComp>()->clear();
		++iter;
	}
	mSelectedEnts.clear();
	mFocusEnt = uivec3();
	//nsengine.events()->send(new NSSelFocusChangeEvent("FocusEvent", mFocusEnt));
}

void NSSelectionSystem::setPickfbo(uint32 fbo)
{
	pickBuf = fbo;
}

void NSSelectionSystem::setFinalfbo(uint32 fbo)
{
	finalBuf = fbo;
}

uivec3 NSSelectionSystem::pick(const fvec2 & mpos)
{
	return pick(mpos.x, mpos.y);
}

uivec3 NSSelectionSystem::pick(float mousex, float mousey)
{
	NSScene * scene = nsengine.currentScene();
	if (scene == NULL)
		return uivec3();

	NSEntity * cam = scene->camera();
	if (cam == NULL)
		return uivec3();

	NSFrameBuffer * pickingBuf = nsengine.framebuffer(pickBuf);
	if (pickingBuf == NULL)
		return uivec3();

	NSCamComp * cc = cam->get<NSCamComp>();
	ivec2 dim = cc->dim();
	float mouseX = mousex * float(dim.w);
	float mouseY = mousey * float(dim.h);

	pickingBuf->setTarget(NSFrameBuffer::Read);
	pickingBuf->bind();
	pickingBuf->setReadBuffer(NSFrameBuffer::AttachmentPoint(NSFrameBuffer::Color + NSGBuffer::Picking));

	uivec3 index;
	glReadPixels(int32(mouseX), int32(mouseY), 1, 1, GL_RGB_INTEGER, GL_UNSIGNED_INT, &index);
	GLError("NSSelectionSystem::pick");

	pickingBuf->setReadBuffer(NSFrameBuffer::None);
	pickingBuf->unbind();
	return index;
}

void NSSelectionSystem::draw()
{
	NSScene * scene = nsengine.currentScene();
	if (scene == NULL || selShader == NULL)
		return;

	NSEntity * cam = scene->camera();
	if (cam == NULL)
		return;

	NSFrameBuffer * finalB = nsengine.framebuffer(finalBuf);
	finalB->setTarget(NSFrameBuffer::Draw);
	finalB->bind();

	glDepthMask(GL_TRUE);
	glDisable(GL_DEPTH_TEST);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glClear(GL_STENCIL_BUFFER_BIT);
	glStencilFunc(GL_ALWAYS, 1, -1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	NSTFormComp * camTComp = cam->get<NSTFormComp>();
	NSCamComp * camc = cam->get<NSCamComp>();
	selShader->bind();
	selShader->setProjCamMat(camc->projCam());

	// Go through and stencil each selected item
	auto iter = mSelectedEnts.begin();
	while (iter != mSelectedEnts.end())
	{
		NSTFormComp * tComp = (*iter)->get<NSTFormComp>();
		NSSelComp * selComp = (*iter)->get<NSSelComp>();
		NSRenderComp * renComp = (*iter)->get<NSRenderComp>();
		NSAnimComp * animComp = (*iter)->get<NSAnimComp>();
		NSTerrainComp * terComp = (*iter)->get<NSTerrainComp>();

		if (selComp->selected() && selComp->drawEnabled())
		{
			auto selIter = selComp->begin();
			while (selIter != selComp->end())
			{
				selShader->setTransform(tComp->transform(*selIter));
				NSMesh * rMesh = nsengine.resource<NSMesh>(renComp->meshID());
				for (uint32 i = 0; i < rMesh->count(); ++i)
				{
					NSMesh::SubMesh * cSub = rMesh->submesh(i);

					selShader->setHeightMapEnabled(false);
					NSMaterial * mat = nsengine.resource<NSMaterial>(renComp->materialID(i));
					if (mat != NULL)
					{
						NSTexture * tex = nsengine.resource<NSTexture>(mat->mapTextureID(NSMaterial::Height));
						if (tex != NULL)
						{
							selShader->setHeightMapEnabled(true);
							tex->enable(NSMaterial::Height);
						}
					}

					if (cSub->mNode != NULL)
						selShader->setNodeTransform(cSub->mNode->mWorldTransform);
					else
						selShader->setNodeTransform(fmat4());

					if (animComp != NULL)
					{
						selShader->setHasBones(true);
						selShader->setBoneTransforms(*animComp->finalTransforms());
					}
					else
						selShader->setHasBones(false);

					if (terComp != NULL)
						selShader->setHeightMinMax(terComp->heightBounds());

					cSub->mVAO.bind();
					glDrawElements(cSub->mPrimType,
								   static_cast<GLsizei>(cSub->mIndices.size()),
								   GL_UNSIGNED_INT,
								   0);
					cSub->mVAO.unbind();
				}
				++selIter;
			}
		}
		++iter;
	}

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glLineWidth(3.0f);
	iter = mSelectedEnts.begin();
	while (iter != mSelectedEnts.end())
	{
		NSTFormComp * tComp = (*iter)->get<NSTFormComp>();
		NSSelComp * selComp = (*iter)->get<NSSelComp>();
		NSRenderComp * renComp = (*iter)->get<NSRenderComp>();
		NSAnimComp * animComp = (*iter)->get<NSAnimComp>();
		NSTerrainComp * terComp = (*iter)->get<NSTerrainComp>();

		if (renComp == NULL || tComp == NULL)
		{
			++iter;
			continue;
		}

		if (selComp->selected() && selComp->drawEnabled())
		{
			auto selIter = selComp->begin();
			while (selIter != selComp->end())
			{
				selShader->setTransform(tComp->transform(*selIter));
				NSMesh * rMesh = nsengine.resource<NSMesh>(renComp->meshID());
				for (uint32 i = 0; i < rMesh->count(); ++i)
				{
					NSMesh::SubMesh * cSub = rMesh->submesh(i);

					selShader->setHeightMapEnabled(false);
					NSMaterial * mat = nsengine.resource<NSMaterial>(renComp->materialID(i));
					if (mat != NULL)
					{
						NSTexture * tex = nsengine.resource<NSTexture>(mat->mapTextureID(NSMaterial::Height));
						if (tex != NULL)
						{
							selShader->setHeightMapEnabled(true);
							tex->enable(NSMaterial::Height);
						}
					}

					if (cSub->mNode != NULL)
						selShader->setNodeTransform(cSub->mNode->mWorldTransform);
					else
						selShader->setNodeTransform(fmat4());

					if (animComp != NULL)
					{
						selShader->setHasBones(true);
						selShader->setBoneTransforms(*animComp->finalTransforms());
					}
					else
						selShader->setHasBones(false);

					if (terComp != NULL)
						selShader->setHeightMinMax(terComp->heightBounds());

					cSub->mVAO.bind();

					glDisable(GL_DEPTH_TEST);
					glPolygonMode(GL_FRONT, GL_LINE);
					glStencilFunc(GL_NOTEQUAL, 1, -1);
					glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

					fvec4 selCol = selComp->color();
					selShader->setFragOutColor(selCol);
					glDrawElements(cSub->mPrimType,
								   static_cast<GLsizei>(cSub->mIndices.size()),
								   GL_UNSIGNED_INT,
								   0);

					glPolygonMode(GL_FRONT, GL_FILL);
					glEnable(GL_DEPTH_TEST);
					glStencilFunc(GL_EQUAL, 1, 0);
					selCol.w = selComp->maskAlpha();

					if (mFocusEnt.y == (*iter)->id() && mFocusEnt.z == *selIter)
						selCol.w = 0.4f;

					selShader->setUniform("fragColOut", selCol);
					glDrawElements(cSub->mPrimType,
								   static_cast<GLsizei>(cSub->mIndices.size()),
								   GL_UNSIGNED_INT,
								   0);
					cSub->mVAO.unbind();
				}
				++selIter;
			}
		}

		_drawEntOcc(*iter);
		++iter;
	}

	_drawOcc();
	_drawHidden();


	// THIS SHOULD BE MOVED TO BUILD SYSTEM
	// Draw mirror mode center
	if (nsengine.system<NSBuildSystem>()->mirror())
	{
		NSMesh * tileM = nsengine.engplug()->get<NSMesh>(MESH_FULL_TILE);
		fvec3 mypos = nsengine.system<NSBuildSystem>()->mirror();
		mypos.z = nsengine.system<NSBuildSystem>()->layer() * Z_GRID;
		selShader->setTransform(translationMat4(mypos));
		for (uint32 i = 0; i < tileM->count(); ++i)
		{
			NSMesh::SubMesh * cSub = tileM->submesh(i);

			if (cSub->mNode != NULL)
				selShader->setNodeTransform(cSub->mNode->mWorldTransform);
			else
				selShader->setNodeTransform(fmat4());

			selShader->setHasBones(false);

			cSub->mVAO.bind();
			fvec4 col(1.0f, 0.0f, 1.0f, 0.7f);
			selShader->setFragOutColor(col);
			glDrawElements(cSub->mPrimType,
						   static_cast<GLsizei>(cSub->mIndices.size()),
						   GL_UNSIGNED_INT,
						   0);
			cSub->mVAO.unbind();
		}
	}

	selShader->unbind();
	glLineWidth(1.0f);
}

void NSSelectionSystem::_drawEntOcc(NSEntity * ent)
{
	// Draw the occupy component if draw enabled
	NSOccupyComp * occComp = ent->get<NSOccupyComp>();
	NSSelComp * selComp = ent->get<NSSelComp>();
	NSTFormComp * tComp = ent->get<NSTFormComp>();
	if (occComp != NULL && selComp != NULL && tComp != NULL && occComp->drawEnabled())
	{
		NSMesh * occMesh = nsengine.resource<NSMesh>(occComp->meshid());
		NSMaterial * mat = nsengine.resource<NSMaterial>(occComp->matid());
		if (occMesh != NULL)
		{
			auto selIter = selComp->begin();
			while (selIter != selComp->end())
			{
				for (uint32 i = 0; i < occMesh->count(); ++i)
				{
					NSMesh::SubMesh * occSub = occMesh->submesh(i);

					if (occSub->mNode != NULL)
						selShader->setNodeTransform(occSub->mNode->mWorldTransform);
					else
						selShader->setNodeTransform(fmat4());

					selShader->setHasBones(false);
					occSub->mVAO.bind();

					glDisable(GL_STENCIL_TEST);

					if (mat != NULL)
						selShader->setFragOutColor(mat->color());
					else
						selShader->setFragOutColor(fvec4(1.0f, 0.0f, 1.0f, 0.5f));

					auto spaceIter = occComp->begin();
					while (spaceIter != occComp->end())
					{
						selShader->setTransform(translationMat4(NSTileGrid::world(*spaceIter, tComp->wpos(*selIter))));
						glDrawElements(occSub->mPrimType,
									   static_cast<GLsizei>(occSub->mIndices.size()),
									   GL_UNSIGNED_INT,
									   0);
						++spaceIter;
					}
					occSub->mVAO.unbind();
				}
				++selIter;
			}
		}
	}
}

void NSSelectionSystem::_drawOcc()
{
	NSScene * scene = nsengine.currentScene();
	if (scene == NULL || selShader == NULL)
		return;

	if (!drawOcc)
		return;

	NSMesh * occMesh = nsengine.engplug()->get<NSMesh>(MESH_FULL_TILE);
	for (uint32 i = 0; i < occMesh->count(); ++i)
	{
		NSMesh::SubMesh * occSub = occMesh->submesh(i);

		if (occSub->mNode != NULL)
			selShader->setNodeTransform(occSub->mNode->mWorldTransform);
		else
			selShader->setNodeTransform(fmat4());

		selShader->setHasBones(false);
		occSub->mPosBuf.bind();
		occSub->mVAO.vertexAttribPtr(NSShader::Position, 3, GL_FLOAT, GL_FALSE, sizeof(fvec3), 0);
		occSub->mIndiceBuf.bind();

		selShader->setFragOutColor(fvec4(1.0f, 0.0f, 0.0f, 1.0f));

		NSTileGrid::GridBounds g = scene->grid().occupiedGridBounds();


		for (int32 z = g.minSpace.z; z <= g.maxSpace.z; ++z)
		{
			for (int32 y = g.minSpace.y; y <= g.maxSpace.y; ++y)
			{
				for (int32 x = g.minSpace.x; x <= g.maxSpace.x; ++x)
				{
					uivec3 id = scene->grid().get(ivec3(x, y, z));
					NSEntity * ent = nsengine.resource<NSEntity>(id.x, id.y);
					if (ent != NULL)
					{
						trans.setColumn(3,fvec4(ent->get<NSTFormComp>()->lpos(id.z),1.0f));
						selShader->setTransform(trans);
						glDrawElements(occSub->mPrimType,
									   static_cast<GLsizei>(occSub->mIndices.size()),
									   GL_UNSIGNED_INT,
									   0);
					}
				}
			}
		}
	}

}

void NSSelectionSystem::_drawHidden()
{
	NSScene * scene = nsengine.currentScene();
	if (scene == NULL || selShader == NULL)
		return;

	NSEntity * cam = scene->camera();
	if (cam == NULL)
		return;

	// Draw all hidden objects - This should probably be moved to the render system or something - or to a forward
	// renderer when that gets set up so that we can draw transparent stuff
	nspentityset sceneEnts = scene->entities();
	auto sceneEntIter = sceneEnts.begin();
	while (sceneEntIter != sceneEnts.end())
	{
		NSTFormComp * tComp = (*sceneEntIter)->get<NSTFormComp>();
		NSRenderComp * renComp = (*sceneEntIter)->get<NSRenderComp>();

		if (renComp == NULL)
		{
			++sceneEntIter;
			continue;
		}

		NSAnimComp * animComp = (*sceneEntIter)->get<NSAnimComp>();
		NSMesh * rMesh = nsengine.resource<NSMesh>(renComp->meshID());

		if (rMesh == NULL)
		{
			++sceneEntIter;
			continue;
		}

		for (uint32 index = 0; index < tComp->count(); ++index)
		{
			int32 state = tComp->hiddenState(index);

			bool layerBit = state & NSTFormComp::LayerHide && true;
			bool objectBit = state & NSTFormComp::ObjectHide && true;
			bool showBit = state & NSTFormComp::Show && true;
			bool hideBit = state & NSTFormComp::Hide && true;

			if (!hideBit && (!layerBit && objectBit))
			{
				selShader->setTransform(tComp->transform(index));
				for (uint32 i = 0; i < rMesh->count(); ++i)
				{
					NSMesh::SubMesh * cSub = rMesh->submesh(i);

					if (cSub->mNode != NULL)
						selShader->setNodeTransform(cSub->mNode->mWorldTransform);
					else
						selShader->setNodeTransform(fmat4());

					if (animComp != NULL)
					{
						selShader->setHasBones(true);
						selShader->setBoneTransforms(*animComp->finalTransforms());
					}
					else
						selShader->setHasBones(false);

					cSub->mVAO.bind();
					fvec4 col(1.0f, 1.0f, 1.0f, 0.04f);
					selShader->setFragOutColor(col);
					glDrawElements(cSub->mPrimType,
								   static_cast<GLsizei>(cSub->mIndices.size()),
								   GL_UNSIGNED_INT,
								   0);
					cSub->mVAO.unbind();
				}
			}
		}
		++sceneEntIter;
	}
}

void NSSelectionSystem::del()
{
	NSScene * scene = nsengine.currentScene();
	if (scene == NULL)
		return;

	std::vector<fvec3> posVec;

	auto iter = mSelectedEnts.begin();
	while (iter != mSelectedEnts.end())
	{
		NSSelComp * selC = (*iter)->get<NSSelComp>();
		NSTFormComp * tComp = (*iter)->get<NSTFormComp>();

		posVec.resize(selC->count());
		uint32 i = 0;
		auto iter2 = selC->begin();
		while (iter2 != selC->end())
		{
			posVec[i] = tComp->wpos(*iter2);
			++iter2;
			++i;
		}

		for (uint32 cur = 0; cur < posVec.size(); ++cur)
			scene->remove(posVec[cur]);

		++iter;
	}
	clear();
}

bool NSSelectionSystem::empty()
{
	return mSelectedEnts.empty();
}

void NSSelectionSystem::enableLayerMode(const bool & pMode)
{
	mLayerMode = pMode;
}

void NSSelectionSystem::init()
{
	//nsengine.events()->addListener(this, NSEvent::SelPick);
	//nsengine.events()->addListener(this, NSEvent::SelSet);
	//nsengine.events()->addListener(this, NSEvent::SelAdd);
	//nsengine.events()->addListener(this, NSEvent::ClearSelection);
	registerHandlerFunc(this, &NSSelectionSystem::_handleActionEvent);
	registerHandlerFunc(this, &NSSelectionSystem::_handleStateEvent);

	addTriggerHash(SelectEntity, NSSEL_SELECT);
	addTriggerHash(MultiSelect, NSSEL_MULTISELECT);
	addTriggerHash(ShiftSelect, NSSEL_SHIFTSELECT);
	addTriggerHash(MoveSelection, NSSEL_MOVE);
	addTriggerHash(MoveSelectionXY, NSSEL_MOVE_XY);
	addTriggerHash(MoveSelectionZY, NSSEL_MOVE_ZY);
	addTriggerHash(MoveSelectionZX, NSSEL_MOVE_ZX);
	addTriggerHash(MoveSelectionX, NSSEL_MOVE_X);
	addTriggerHash(MoveSelectionY, NSSEL_MOVE_Y);
	addTriggerHash(MoveSelectionZ, NSSEL_MOVE_Z);
	addTriggerHash(MoveSelectionToggle, NSSEL_MOVE_TOGGLE);
}

int32 NSSelectionSystem::drawPriority()
{
	return SEL_SYS_DRAW_PR;
}

int32 NSSelectionSystem::updatePriority()
{
	return SEL_SYS_UPDATE_PR;
}

void NSSelectionSystem::_onRotateX(NSEntity * ent, bool pPressed)
{
	if (ent == NULL)
		return;
	NSSelComp * sc = ent->get<NSSelComp>();
	if (sc->selected() && pPressed)
		rotate(ent, NSTFormComp::Right, 45.0f);
}

void NSSelectionSystem::_onRotateY(NSEntity * ent, bool pPressed)
{
	if (ent == NULL)
		return;
	NSSelComp * sc = ent->get<NSSelComp>();
	if (sc->selected() && pPressed)
		rotate(ent, NSTFormComp::Target, 45.0f);
}

void NSSelectionSystem::_onRotateZ(NSEntity * ent, bool pPressed)
{
	if (ent == NULL)
		return;
	NSSelComp * sc = ent->get<NSSelComp>();
	if (sc->selected() && pPressed)
		rotate(ent, NSTFormComp::Up, 45.0f);
}

void NSSelectionSystem::_onSelect(NSEntity * ent, bool pPressed, const uivec3 & pID, bool pSnapZOnly)
{
	NSScene * scene = nsengine.currentScene();
	if (scene == NULL)
		return;

	NSEntity * cam = scene->camera();
	if (cam == NULL)
		return;

	if (ent == NULL)
		return;

	NSSelComp * sc = ent->get<NSSelComp>();
	NSTFormComp * tc = ent->get<NSTFormComp>();

	if (pPressed)
	{
		if (ent->plugid() == pID.x && ent->id() == pID.y)
		{
			if (!sc->contains(pID.z))
				set(ent, pID.z);

			mFocusEnt = pID;
			if (!mMoving)
			{
				fvec3 originalPos = tc->wpos(mFocusEnt.z);
				mCachedPoint = originalPos;
				removeFromGrid();
				mMoving = true;
			}

			//nsengine.events()->send(new NSSelFocusChangeEvent("FocusEvent", mFocusEnt));
		}
		else if (!contains(pID))
		{
			clear();
			mFocusEnt = uivec3();
			//nsengine.events()->send(new NSSelFocusChangeEvent("FocusEvent", mFocusEnt));
		}
	}
	else
	{
		if (sc->selected())
		{
			if (pSnapZOnly)
				snapZ(ent);
			else
				snap(ent);
		}
			

		if (mMoving)
		{		
			NSEntity * ent = scene->entity(mFocusEnt.x, mFocusEnt.y);
			if (ent == NULL)
				return;
			if (!collision())
			{
				ent->get<NSTFormComp>()->computeTransform(mFocusEnt.z);
				fvec3 pTranslate = mCachedPoint - ent->get<NSTFormComp>()->wpos(mFocusEnt.z);
				translate(pTranslate);
				setColor(fvec4(DEFAULT_SEL_R, DEFAULT_SEL_G, DEFAULT_SEL_B, DEFAULT_SEL_A));

				if (!addToGrid())
				{
					dprint("NSSelectionSystem::onSelect Error in resetting tiles to original grid position");
				}

				//nsengine.events()->send(new NSSelFocusChangeEvent("FocusEvent", mFocusEnt));
			}
			else
			{
				fvec3 pos = ent->get<NSTFormComp>()->wpos(mFocusEnt.z);
				scene->grid().snap(pos);
				addToGrid();
			}

			mMoving = false;
			mCachedPoint = fvec3();
		}
	}
}

void NSSelectionSystem::showOccupiedSpaces(bool show)
{
	drawOcc = show;
}

void NSSelectionSystem::_onPaintSelect(NSEntity * ent, const fvec2 & pPos)
{
	if (ent == NULL)
		return;

	NSScene * scene = nsengine.currentScene();
	if (scene == NULL)
		return;

	uivec3 pi = pick(pPos.x, pPos.y);
	NSSelComp * sc = ent->get<NSSelComp>();

	if (ent->plugid() == pi.x && ent->id() == pi.y) // needs the pointing thing
	{
		mFocusEnt = pi;
		//nsengine.events()->send(new NSSelFocusChangeEvent("FocusEvent", mFocusEnt));

		NSEntity * tileBrush = nsengine.system<NSBuildSystem>()->tilebrush();
		if (tileBrush == NULL)
		{
			add(ent, pi.z);
			return;
		}
		NSTileBrushComp * brushComp = tileBrush->get<NSTileBrushComp>();
		if (brushComp == NULL)
		{
			add(ent, pi.z);
			return;
		}

		auto brushIter = brushComp->begin();
		while (brushIter != brushComp->end())
		{
			for (int32 i = 0; i < brushComp->height(); ++i)
			{
				NSEntity * focEnt = scene->entity(mFocusEnt.x, mFocusEnt.y);
				if (focEnt == NULL)
					continue;

				NSTFormComp * tForm = focEnt->get<NSTFormComp>();
				fvec3 pos = tForm->lpos(mFocusEnt.z) + NSTileGrid::world(ivec3(brushIter->x, brushIter->y, -i)); // add in height when get working
				uivec3 refid = scene->refid(pos);
				NSEntity * selEnt = nsengine.resource<NSEntity>(refid.x, refid.y);
				if (selEnt == NULL)
					continue;
				NSSelComp * selComp = selEnt->get<NSSelComp>();
				if (selComp == NULL)
					continue;

				add(ent, refid.z);
			}
			++brushIter;
		}

	}
}

void NSSelectionSystem::_onDragObject(NSEntity * ent, const fvec2 & pDelta, uint16 axis_)
{
	NSScene * scene = nsengine.currentScene();
	if (scene == NULL)
		return;

	NSEntity * cam = scene->camera();
	if (cam == NULL)
		return;

	if (ent == NULL)
		return;

	NSSelComp * sc = ent->get<NSSelComp>();
	NSTFormComp * camTForm = cam->get<NSTFormComp>();
	NSCamComp * camc = cam->get<NSCamComp>();
	NSTFormComp * tComp = ent->get<NSTFormComp>();

	// get change in terms of NDC
	fvec2 delta(pDelta * 2.0f);

	fvec3 originalPos = tComp->wpos(mFocusEnt.z);
	fvec4 screenSpace = camc->projCam() * fvec4(originalPos, 1.0f);
	screenSpace /= screenSpace.w;
	screenSpace.x += delta.u;
	screenSpace.y += delta.v;

	fvec4 newPos = camc->invProjCam() * screenSpace;
	newPos /= newPos.w;

	fvec3 fpos(newPos.xyz());
	fvec3 castVec = fpos - camTForm->wpos();
	castVec.normalize();
	float depth = 0.0f;
	fvec3 normal;

	fvec3 targetVec = (camc->focusOrientation() * camTForm->orientation()).target();
	fvec3 projVec = projectPlane(targetVec, fvec3(0.0f,0.0f,-1.0f));
	fvec2 projVecX = project(projVec.xy(), fvec2(1.0,0.0));
	
	float angle = targetVec.angleTo(projVec);
	float angleX = projVec.xy().angleTo(projVecX);


	// Set normal if not moving in a single plane
	if ((axis_ == (XAxis | YAxis | ZAxis) && angle > 35.0f) || (axis_ & ZAxis) != ZAxis)
		normal.set(0.0f,0.0f,-1.0f);
	else
	{
		if (axis_ == ZAxis || axis_ == (XAxis | YAxis | ZAxis))
		{
			if (angleX < 45.0f)
				normal.set(-1.0f,0.0f,0.0f);
			else
				normal.set(0.0f,-1.0f,0.0f);
		}
		else
		{
			if ((axis_ & XAxis) != XAxis)
				normal.set(-1.0f,0.0f,0.0f);
			else
				normal.set(0.0f,-1.0f,0.0f);
		}
	}
	
	depth = (normal * (originalPos - fpos)) / (normal * castVec);
	fpos += castVec*depth;
	fpos -= originalPos;	
	fpos %= fvec3(float((axis_ & XAxis) == XAxis), float((axis_ & YAxis) == YAxis), float((axis_ & ZAxis) == ZAxis));
	mTotalFrameTranslation += fpos;
}

int32 NSSelectionSystem::layer() const
{
	return mLayer;
}

const uivec3 & NSSelectionSystem::center()
{
	return mFocusEnt;
}

void NSSelectionSystem::_onMultiSelect(NSEntity * ent, bool pPressed, const uivec3 & pID)
{
	NSScene * scene = nsengine.currentScene();
	if (scene == NULL)
		return;

	if (ent == NULL)
		return;

	NSSelComp * sc = ent->get<NSSelComp>();


	if (pPressed)
	{
		NSEntity * ent = sc->owner();
		if (ent->plugid() == pID.x && ent->id() == pID.y)
		{
			if (!contains(pID))
			{
				add(ent, pID.z);
				mFocusEnt = pID;
				//nsengine.events()->send(new NSSelFocusChangeEvent("FocusEvent", mFocusEnt));
			}
			else
			{
				remove(ent, pID.z);
				mFocusEnt = uivec3();
				if (!mSelectedEnts.empty())
				{
					NSEntity * ent = scene->entity(pID.x,pID.y);
					if (ent != NULL)
					{
						auto iter = mSelectedEnts.find(ent);
						if (iter != mSelectedEnts.end())
						{
							NSSelComp * selComp = (*iter)->get<NSSelComp>();
							auto first = selComp->begin();
							mFocusEnt.x = (*iter)->plugid();
							mFocusEnt.y = (*iter)->id();
							mFocusEnt.z = (*first);
						}
						else
						{
							auto entFirst = mSelectedEnts.begin();
							NSSelComp * selComp = (*entFirst)->get<NSSelComp>();
							auto first = selComp->begin();
							mFocusEnt.x = (*entFirst)->plugid();
							mFocusEnt.y = (*entFirst)->id();
							mFocusEnt.z = (*first);
						}
					}
					else
					{
						auto entFirst = mSelectedEnts.begin();
						NSSelComp * selComp = (*entFirst)->get<NSSelComp>();
						auto first = selComp->begin();
						mFocusEnt.y = (*entFirst)->plugid();
						mFocusEnt.y = (*entFirst)->id();
						mFocusEnt.z = (*first);
					}
				}
				//nsengine.events()->send(new NSSelFocusChangeEvent("FocusEvent", mFocusEnt));
			}
		}
	}
}

void NSSelectionSystem::_reset_focus(const uivec3 & pickid)
{	
	if (mSelectedEnts.empty())
	{
		mFocusEnt = uivec3();
		mSendFocEvent = true;
		return;
	}
	

	if (contains(pickid))
	{
		mFocusEnt = pickid;
		mSendFocEvent = true;
		return;
	}

	if (contains(mFocusEnt))
		return;

	NSEntity * ent = nsengine.resource<NSEntity>(pickid.xy());
	NSSelComp * sc;
	if (ent == NULL || (sc = ent->get<NSSelComp>()) == NULL)
		return;

	NSTFormComp * tc = ent->get<NSTFormComp>();
	fvec3 original_pos = tc->wpos(pickid.z);
	
	auto iter = mSelectedEnts.find(ent);
	if (iter != mSelectedEnts.end())
	{
		auto sel_iter = sc->begin();
		uint closest_tformid = *sel_iter;
		while (sel_iter != sc->end())
		{
			if ( (original_pos - tc->wpos(*sel_iter)).lengthSq() <
				 (original_pos - tc->wpos(closest_tformid)).lengthSq() )
				closest_tformid = *sel_iter;
			++sel_iter;
		}
		mFocusEnt.set(pickid.xy(),closest_tformid);
	}
	else
	{
		auto entFirst = mSelectedEnts.begin();
		NSSelComp * selComp = (*entFirst)->get<NSSelComp>();
		auto first = selComp->begin();
		mFocusEnt.set((*entFirst)->fullid(), *first);
	}
	mSendFocEvent = true;
}

void NSSelectionSystem::remove(NSEntity * ent, uint32 pTFormID)
{
	if (ent == NULL)
		return;

	NSSelComp * selcomp = ent->get<NSSelComp>();
	if (selcomp == NULL)
		return;

	selcomp->remove(pTFormID);
	if (selcomp->empty())
	{
		auto iter = mSelectedEnts.find(ent);
		mSelectedEnts.erase(iter);
	}
}

void NSSelectionSystem::removeFromGrid()
{
	NSScene * scene = nsengine.currentScene();
	if (scene == NULL)
		return;

	auto iter = mSelectedEnts.begin();
	while (iter != mSelectedEnts.end())
	{
		NSSelComp * selComp = (*iter)->get<NSSelComp>();
		NSTFormComp * tComp = (*iter)->get<NSTFormComp>();
		NSOccupyComp * occComp = (*iter)->get<NSOccupyComp>();

		if (occComp != NULL)
		{
			auto selIter = selComp->begin();
			while (selIter != selComp->end())
			{
				scene->grid().remove(occComp->spaces(), tComp->wpos(*selIter));
				++selIter;
			}
		}
		++iter;
	}
}

void NSSelectionSystem::resetColor()
{
	auto iter = mSelectedEnts.begin();
	while (iter != mSelectedEnts.end())
	{
		NSSelComp * selComp = (*iter)->get<NSSelComp>();
		auto selIter = selComp->begin();
		while (selIter != selComp->end())
		{
			selComp->setColor(selComp->defaultColor());
			++selIter;
		}
		++iter;
	}
}

void NSSelectionSystem::rotate(NSEntity * ent, const fvec4 & axisangle)
{
	if (ent == NULL)
		return;

	NSSelComp * scomp = ent->get<NSSelComp>();
	NSTFormComp * tcomp = ent->get<NSTFormComp>();
	if (scomp == NULL || tcomp == NULL)
		return;

	auto selIter = scomp->begin();
	while (selIter != scomp->end())
	{
		tcomp->rotate(axisangle, *selIter);
		++selIter;
	}
}

void NSSelectionSystem::rotate(const fvec4 & axisangle)
{
	auto iter = mSelectedEnts.begin();
	while (iter != mSelectedEnts.end())
	{
		rotate(*iter, axisangle);
		++iter;
	}
}

void NSSelectionSystem::rotate(NSEntity * ent, NSTFormComp::DirVec axis, float angle)
{
	if (ent == NULL)
		return;

	NSSelComp * scomp = ent->get<NSSelComp>();
	NSTFormComp * tcomp = ent->get<NSTFormComp>();
	if (scomp == NULL || tcomp == NULL)
		return;

	auto selIter = scomp->begin();
	while (selIter != scomp->end())
	{
		tcomp->rotate(axis, angle, *selIter);
		++selIter;
	}
}

void NSSelectionSystem::rotate(NSTFormComp::DirVec axis, float angle)
{
	auto iter = mSelectedEnts.begin();
	while (iter != mSelectedEnts.end())
	{
		rotate(*iter, axis, angle);
		++iter;
	}
}

void NSSelectionSystem::rotate(NSEntity * ent, NSTFormComp::Axis axis, float angle)
{
	if (ent == NULL)
		return;

	NSSelComp * scomp = ent->get<NSSelComp>();
	NSTFormComp * tcomp = ent->get<NSTFormComp>();
	if (scomp == NULL || tcomp == NULL)
		return;

	auto selIter = scomp->begin();
	while (selIter != scomp->end())
	{
		tcomp->rotate(axis, angle, *selIter);
		++selIter;
	}
}

void NSSelectionSystem::rotate(NSTFormComp::Axis axis, float angle)
{
	auto iter = mSelectedEnts.begin();
	while (iter != mSelectedEnts.end())
	{
		rotate(*iter, axis, angle);
		++iter;
	}
}

void NSSelectionSystem::rotate(NSEntity * ent, const fvec3 & euler)
{
	if (ent == NULL)
		return;

	NSSelComp * scomp = ent->get<NSSelComp>();
	NSTFormComp * tcomp = ent->get<NSTFormComp>();
	if (scomp == NULL || tcomp == NULL)
		return;

	auto selIter = scomp->begin();
	while (selIter != scomp->end())
	{
		tcomp->rotate(euler, *selIter);
		++selIter;
	}
}

void NSSelectionSystem::rotate(const fvec3 & euler)
{
	auto iter = mSelectedEnts.begin();
	while (iter != mSelectedEnts.end())
	{
		rotate(*iter, euler);
		++iter;
	}
}

void NSSelectionSystem::rotate(NSEntity * ent, const fquat & orientation)
{
	if (ent == NULL)
		return;

	NSSelComp * scomp = ent->get<NSSelComp>();
	NSTFormComp * tcomp = ent->get<NSTFormComp>();
	if (scomp == NULL || tcomp == NULL)
		return;

	auto selIter = scomp->begin();
	while (selIter != scomp->end())
	{
		tcomp->rotate(orientation, *selIter);
		++selIter;
	}
}

void NSSelectionSystem::rotate(const fquat & orientation)
{
	auto iter = mSelectedEnts.begin();
	while (iter != mSelectedEnts.end())
	{
		rotate(*iter, orientation);
		++iter;
	}
}

void NSSelectionSystem::scale(NSEntity * ent, const fvec3 & pAmount)
{
	if (ent == NULL)
		return;

	NSSelComp * scomp = ent->get<NSSelComp>();
	NSTFormComp * tcomp = ent->get<NSTFormComp>();
	if (scomp == NULL || tcomp == NULL)
		return;

	auto selIter = scomp->begin();
	while (selIter != scomp->end())
	{
		tcomp->scale(pAmount, *selIter);
		++selIter;
	}
}

void NSSelectionSystem::scale(const fvec3 & pAmount)
{
	auto iter = mSelectedEnts.begin();
	while (iter != mSelectedEnts.end())
	{
		scale(*iter, pAmount);
		++iter;
	}
}

void NSSelectionSystem::scale(NSEntity * ent, float x, float y, float z)
{
	scale(ent, fvec3(x, y, z));
}

void NSSelectionSystem::scale(float x, float y, float z)
{
	scale(fvec3(x, y, z));
}

bool NSSelectionSystem::set(NSEntity * ent, uint32 tformid)
{
	clear();
	return add(ent, tformid);
}

void NSSelectionSystem::setColor(const fvec4 & pColor)
{
	auto iter = mSelectedEnts.begin();
	while (iter != mSelectedEnts.end())
	{
		NSSelComp * selComp = (*iter)->get<NSSelComp>();
		auto selIter = selComp->begin();
		while (selIter != selComp->end())
		{
			selComp->setColor(pColor);
			++selIter;
		}
		++iter;
	}
}

void NSSelectionSystem::setHiddenState(NSTFormComp::HiddenState pState, bool pSet)
{
	auto iter = mSelectedEnts.begin();
	while (iter != mSelectedEnts.end())
	{
		NSSelComp * selComp = (*iter)->get<NSSelComp>();
		NSTFormComp * tForm = (*iter)->get<NSTFormComp>();

		auto selIter = selComp->begin();
		while (selIter != selComp->end())
		{
			tForm->setHiddenState(pState, pSet, *selIter);
			++selIter;
		}
		++iter;
	}
}

void NSSelectionSystem::setLayer(int32 pLayer)
{
	mLayer = pLayer;
}

void NSSelectionSystem::setShader(NSSelectionShader * selShader_)
{
	selShader = selShader_;
}

void NSSelectionSystem::snap(NSEntity * ent)
{
	if (ent == NULL)
		return;

	NSSelComp * scomp = ent->get<NSSelComp>();
	NSTFormComp * tcomp = ent->get<NSTFormComp>();
	if (scomp == NULL || tcomp == NULL)
		return;

	auto selIter = scomp->begin();
	while (selIter != scomp->end())
	{
		tcomp->snap(*selIter);
		++selIter;
	}
}

void NSSelectionSystem::snap()
{
	auto iter = mSelectedEnts.begin();
	while (iter != mSelectedEnts.end())
	{
		snap(*iter);
		++iter;
	}
}

void NSSelectionSystem::snapX(NSEntity * ent)
{
	if (ent == NULL)
		return;

	NSSelComp * scomp = ent->get<NSSelComp>();
	NSTFormComp * tcomp = ent->get<NSTFormComp>();
	if (scomp == NULL || tcomp == NULL)
		return;

	auto selIter = scomp->begin();
	while (selIter != scomp->end())
	{
		tcomp->snapX(*selIter);
		++selIter;
	}
}

void NSSelectionSystem::snapX()
{
	auto iter = mSelectedEnts.begin();
	while (iter != mSelectedEnts.end())
	{
		snapX(*iter);
		++iter;
	}
}

void NSSelectionSystem::snapY(NSEntity * ent)
{
	if (ent == NULL)
		return;

	NSSelComp * scomp = ent->get<NSSelComp>();
	NSTFormComp * tcomp = ent->get<NSTFormComp>();
	if (scomp == NULL || tcomp == NULL)
		return;

	auto selIter = scomp->begin();
	while (selIter != scomp->end())
	{
		tcomp->snapY(*selIter);
		++selIter;
	}
}

void NSSelectionSystem::snapY()
{
	auto iter = mSelectedEnts.begin();
	while (iter != mSelectedEnts.end())
	{
		snapY(*iter);
		++iter;
	}
}

void NSSelectionSystem::snapZ(NSEntity * ent)
{
	if (ent == NULL)
		return;

	NSSelComp * scomp = ent->get<NSSelComp>();
	NSTFormComp * tcomp = ent->get<NSTFormComp>();
	if (scomp == NULL || tcomp == NULL)
		return;

	auto selIter = scomp->begin();
	while (selIter != scomp->end())
	{
		tcomp->snapZ(*selIter);
		++selIter;
	}
}

void NSSelectionSystem::snapZ()
{
	auto iter = mSelectedEnts.begin();
	while (iter != mSelectedEnts.end())
	{
		snapZ(*iter);
		++iter;
	}
}


void NSSelectionSystem::tileswap(NSEntity * pNewTile)
{
	NSScene * scene = nsengine.currentScene();
	if (scene == NULL)
		return;

	if (pNewTile == NULL)
	{
		dprint("NSSelectionSystem::swapTiles pNewTile is NULL");
		return;
	}
	if (!tileSwapValid())
		return;

	std::vector<fvec3> posVec;

	auto iter = mSelectedEnts.begin();
	while (iter != mSelectedEnts.end())
	{
		NSSelComp * selC = (*iter)->get<NSSelComp>();
		NSTFormComp * tComp = (*iter)->get<NSTFormComp>();

		posVec.resize(selC->count());
		uint32 i = 0;
		auto iter2 = selC->begin();
		while (iter2 != selC->end())
		{
			posVec[i] = tComp->wpos(*iter2);
			++iter2;
			++i;
		}

		for (uint32 cur = 0; cur < posVec.size(); ++cur)
		{
			uivec3 id = scene->refid(posVec[cur]);
			scene->replace(id.x, id.y, id.z, pNewTile);
		}

		++iter;
	}
	clear();
}

void NSSelectionSystem::translate(NSEntity * ent, const fvec3 & pAmount)
{
	if (ent == NULL)
		return;

	NSSelComp * scomp = ent->get<NSSelComp>();
	NSTFormComp * tcomp = ent->get<NSTFormComp>();
	if (scomp == NULL || tcomp == NULL)
		return;

	auto selIter = scomp->begin();
	while (selIter != scomp->end())
	{
		tcomp->translate(pAmount, *selIter);
		++selIter;
	}
}

void NSSelectionSystem::translate(const fvec3 & pAmount)
{
	auto iter = mSelectedEnts.begin();
	while (iter != mSelectedEnts.end())
	{
		translate(*iter, pAmount);
		++iter;
	}
}

void NSSelectionSystem::translate(float x, float y, float z)
{
	translate(fvec3(x, y, z));
}

void NSSelectionSystem::translate(NSEntity * ent, float x, float y, float z)
{
	translate(ent, fvec3(x, y, z));
}

void NSSelectionSystem::translate(NSEntity * ent, NSTFormComp::DirVec pDir, float pAmount)
{
	if (ent == NULL)
		return;

	NSSelComp * scomp = ent->get<NSSelComp>();
	NSTFormComp * tcomp = ent->get<NSTFormComp>();
	if (scomp == NULL || tcomp == NULL)
		return;

	auto selIter = scomp->begin();
	while (selIter != scomp->end())
	{
		tcomp->translate(pDir, pAmount, *selIter);
		++selIter;
	}
}

void NSSelectionSystem::translate(NSTFormComp::DirVec pDir, float pAmount)
{
	auto iter = mSelectedEnts.begin();
	while (iter != mSelectedEnts.end())
	{
		translate(*iter, pDir, pAmount);
		++iter;
	}
}

void NSSelectionSystem::translate(NSEntity * ent, NSTFormComp::Axis pDir, float pAmount)
{
	if (ent == NULL)
		return;

	NSSelComp * scomp = ent->get<NSSelComp>();
	NSTFormComp * tcomp = ent->get<NSTFormComp>();
	if (scomp == NULL || tcomp == NULL)
		return;

	auto selIter = scomp->begin();
	while (selIter != scomp->end())
	{
		tcomp->translate(pDir, pAmount, *selIter);
		++selIter;
	}
}

void NSSelectionSystem::translate(NSTFormComp::Axis pDir, float pAmount)
{
	auto iter = mSelectedEnts.begin();
	while (iter != mSelectedEnts.end())
	{
		translate(*iter, pDir, pAmount);
		++iter;
	}
}

bool NSSelectionSystem::layerMode() const
{
	return mLayerMode;
}

bool NSSelectionSystem::brushValid()
{
	if (mSelectedEnts.empty())
		return false;

	auto iter = mSelectedEnts.begin();
	while (iter != mSelectedEnts.end())
	{
		if (!(*iter)->has<NSTileComp>())
			return false;

		NSSelComp * selComp = (*iter)->get<NSSelComp>();
		NSTFormComp * tComp = (*iter)->get<NSTFormComp>();

		int32 newZ = NSTileGrid::indexZ(tComp->wpos().z);

		auto selIter = selComp->begin();
		int32 z = 0;

		if (selIter != selComp->end())
			z = NSTileGrid::indexZ(tComp->wpos(*selIter).z);
		else
			return false;

		while (selIter != selComp->end())
		{
			int32 newZ = NSTileGrid::indexZ(tComp->wpos(*selIter).z);
			if (newZ != z)
				return false;
			++selIter;
		}
		++iter;
	}
	return true;
}

bool NSSelectionSystem::tileSwapValid()
{
	if (mSelectedEnts.empty())
		return false;

	auto iter = mSelectedEnts.begin();
	while (iter != mSelectedEnts.end())
	{
		NSSelComp * selComp = (*iter)->get<NSSelComp>();
		NSTFormComp * tComp = (*iter)->get<NSTFormComp>();
		if (!(*iter)->has<NSTileComp>())
			return false;
		++iter;
	}
	return true;
}

void NSSelectionSystem::update()
{

	NSScene * scn = nsengine.currentScene();
	if (scn == NULL)
		return;
	NSEntity * ent = scn->entity(mFocusEnt.xy());
	if (ent == NULL)
		return;
	NSTFormComp * foc_tform = ent->get<NSTFormComp>();

	if (mSendFocEvent)
	{
		nsengine.eventDispatch()->push<NSSelFocusEvent>(mFocusEnt);
		mSendFocEvent = false;
	}

	if (mToggleMove)
	{
		if (mMoving)
		{
			fvec3 originalPos = foc_tform->wpos(mFocusEnt.z);
			mCachedPoint = originalPos;
			removeFromGrid();
		}
		else
		{
			snap();
			if (!collision())
			{
				foc_tform->computeTransform(mFocusEnt.z);
				fvec3 pTranslate = mCachedPoint - foc_tform->wpos(mFocusEnt.z);
				translate(pTranslate);
				setColor(fvec4(DEFAULT_SEL_R, DEFAULT_SEL_G, DEFAULT_SEL_B, DEFAULT_SEL_A));

				if (!addToGrid())
				{
					dprint("NSSelectionSystem::onSelect Error in resetting tiles to original grid position");
				}
			}
			else
			{
				fvec3 pos = foc_tform->wpos(mFocusEnt.z);
				scn->grid().snap(pos);
				addToGrid();
				nsengine.eventDispatch()->push<NSSelFocusEvent>(mFocusEnt);
			}
			mCachedPoint = fvec3();
		}
		mToggleMove = false;
	}

	if (mMoving)
	{		
		if (!collision())
			setColor(fvec4(1.0f, 0.0f, 0.0f, 1.0f));
		else
			resetColor();
		translate(mTotalFrameTranslation);
		mTotalFrameTranslation = 0.0f;
	}
}

bool NSSelectionSystem::_handleActionEvent(NSActionEvent * evnt)
{
	if (evnt->mTriggerHashName == triggerHash(SelectEntity))
	{
		auto xpos_iter = evnt->axes.find(NSInputMap::MouseXPos);
		auto ypos_iter = evnt->axes.find(NSInputMap::MouseYPos);
		fvec2 mpos;
		
		if (xpos_iter != evnt->axes.end())
			mpos.x = xpos_iter->second;
		if (ypos_iter != evnt->axes.end())
			mpos.y = ypos_iter->second;
		
		uivec3 pickid = pick(mpos);
		if (!contains(pickid))
			clear();
		
		NSEntity * selectedEnt = nsengine.resource<NSEntity>(pickid.xy());
        add(selectedEnt, pickid.z);
		_reset_focus(pickid);
	}
	else if (evnt->mTriggerHashName == triggerHash(MultiSelect))
	{
		auto xpos_iter = evnt->axes.find(NSInputMap::MouseXPos);
		auto ypos_iter = evnt->axes.find(NSInputMap::MouseYPos);
		fvec2 mpos;
		
		if (xpos_iter != evnt->axes.end())
			mpos.x = xpos_iter->second;
		if (ypos_iter != evnt->axes.end())
			mpos.y = ypos_iter->second;
		
		uivec3 pickid = pick(mpos);
		NSEntity * selectedEnt = nsengine.resource<NSEntity>(pickid.xy());
		NSTFormComp * tc;
			
		if (contains(pickid))
			remove(selectedEnt,pickid.z);
		else
			add(selectedEnt, pickid.z);

		_reset_focus(pickid);
	}
	else if (evnt->mTriggerHashName == triggerHash(ShiftSelect))
	{
		auto xpos_iter = evnt->axes.find(NSInputMap::MouseXPos);
		auto ypos_iter = evnt->axes.find(NSInputMap::MouseYPos);
		fvec2 mpos;
		
		if (xpos_iter != evnt->axes.end())
			mpos.x = xpos_iter->second;
		if (ypos_iter != evnt->axes.end())
			mpos.y = ypos_iter->second;
		
		uivec3 pickid = pick(mpos);
		NSEntity * selectedEnt = nsengine.resource<NSEntity>(pickid.xy());
		add(selectedEnt, pickid.z);
		_reset_focus(pickid);
	}
	else if (evnt->mTriggerHashName == triggerHash(MoveSelection))
	{
		auto xdelta_iter = evnt->axes.find(NSInputMap::MouseXDelta);
		auto ydelta_iter = evnt->axes.find(NSInputMap::MouseYDelta);
		fvec2 mdelta;
		
		if (xdelta_iter != evnt->axes.end())
			mdelta.x = xdelta_iter->second;
		if (ydelta_iter != evnt->axes.end())
			mdelta.y = ydelta_iter->second;

		NSEntity * ent = nsengine.resource<NSEntity>(mFocusEnt.xy());
		_onDragObject(ent, mdelta, XAxis | YAxis | ZAxis);
	}
	else if (evnt->mTriggerHashName == triggerHash(MoveSelectionXY))
	{
		auto xdelta_iter = evnt->axes.find(NSInputMap::MouseXDelta);
		auto ydelta_iter = evnt->axes.find(NSInputMap::MouseYDelta);
		fvec2 mdelta;
		
		if (xdelta_iter != evnt->axes.end())
			mdelta.x = xdelta_iter->second;
		if (ydelta_iter != evnt->axes.end())
			mdelta.y = ydelta_iter->second;

		NSEntity * ent = nsengine.resource<NSEntity>(mFocusEnt.xy());
		_onDragObject(ent, mdelta, XAxis | YAxis);
	}
	else if (evnt->mTriggerHashName == triggerHash(MoveSelectionZY))
	{
		auto xdelta_iter = evnt->axes.find(NSInputMap::MouseXDelta);
		auto ydelta_iter = evnt->axes.find(NSInputMap::MouseYDelta);
		fvec2 mdelta;
		
		if (xdelta_iter != evnt->axes.end())
			mdelta.x = xdelta_iter->second;
		if (ydelta_iter != evnt->axes.end())
			mdelta.y = ydelta_iter->second;
		
		NSEntity * ent = nsengine.resource<NSEntity>(mFocusEnt.xy());
		_onDragObject(ent, mdelta, YAxis | ZAxis);
	}
	else if (evnt->mTriggerHashName == triggerHash(MoveSelectionZX))
	{
		auto xdelta_iter = evnt->axes.find(NSInputMap::MouseXDelta);
		auto ydelta_iter = evnt->axes.find(NSInputMap::MouseYDelta);
		fvec2 mdelta;
		
		if (xdelta_iter != evnt->axes.end())
			mdelta.x = xdelta_iter->second;
		if (ydelta_iter != evnt->axes.end())
			mdelta.y = ydelta_iter->second;

		NSEntity * ent = nsengine.resource<NSEntity>(mFocusEnt.xy());
		_onDragObject(ent, mdelta, XAxis | ZAxis);
	}
	else if (evnt->mTriggerHashName == triggerHash(MoveSelectionX))
	{
		auto xdelta_iter = evnt->axes.find(NSInputMap::MouseXDelta);
		auto ydelta_iter = evnt->axes.find(NSInputMap::MouseYDelta);
		fvec2 mdelta;
		
		if (xdelta_iter != evnt->axes.end())
			mdelta.x = xdelta_iter->second;
		if (ydelta_iter != evnt->axes.end())
			mdelta.y = ydelta_iter->second;

		NSEntity * ent = nsengine.resource<NSEntity>(mFocusEnt.xy());
		_onDragObject(ent, mdelta, XAxis);
	}
	else if (evnt->mTriggerHashName == triggerHash(MoveSelectionY))
	{
		auto xdelta_iter = evnt->axes.find(NSInputMap::MouseXDelta);
		auto ydelta_iter = evnt->axes.find(NSInputMap::MouseYDelta);
		fvec2 mdelta;
		
		if (xdelta_iter != evnt->axes.end())
			mdelta.x = xdelta_iter->second;
		if (ydelta_iter != evnt->axes.end())
			mdelta.y = ydelta_iter->second;

		NSEntity * ent = nsengine.resource<NSEntity>(mFocusEnt.xy());
		_onDragObject(ent, mdelta, YAxis);
	}
	else if (evnt->mTriggerHashName == triggerHash(MoveSelectionZ))
	{
		auto xdelta_iter = evnt->axes.find(NSInputMap::MouseXDelta);
		auto ydelta_iter = evnt->axes.find(NSInputMap::MouseYDelta);
		fvec2 mdelta;
		
		if (xdelta_iter != evnt->axes.end())
			mdelta.x = xdelta_iter->second;
		if (ydelta_iter != evnt->axes.end())
			mdelta.y = ydelta_iter->second;

		NSEntity * ent = nsengine.resource<NSEntity>(mFocusEnt.xy());
		_onDragObject(ent, mdelta, ZAxis);
	}
	return true;	
}

bool NSSelectionSystem::_handleStateEvent(NSStateEvent * evnt)
{
	if (evnt->mTriggerHashName == triggerHash(MoveSelectionToggle))
	{
		mToggleMove = true;
		mMoving = evnt->mToggle;
	}
	return true;
}
