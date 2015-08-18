/*!
\file nsanimsystem.cpp

\brief Definition file for NSAnimSystem class

This file contains all of the neccessary definitions for the NSAnimSystem class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#include <nsanim_system.h>
#include <nsanim_comp.h>
#include <nsentity.h>
#include <nsscene.h>
#include <nsglobal.h>
#include <nsanim_manager.h>
#include <nsmesh_manager.h>
#include <nsengine.h>
#include <nstimer.h>
#include <nsevent.h>
#include <nsevent_handler.h>
#include <nsentity_manager.h>
#include <nsplugin.h>
#include <nsres_manager.h>

NSAnimSystem::NSAnimSystem() :NSSystem()
{
}

NSAnimSystem::~NSAnimSystem()
{}

void NSAnimSystem::init()
{
}

// bool NSAnimSystem::handleEvent(NSEvent * pEvent)
// {
// 	nsscene * scene = nsengine.currentScene();
// 	if (scene == NULL)
// 		return false;

// 	if (pEvent == NULL)
// 	{
// 		dprint("NSAnimSystem::handleEvent Event is NULL - bad bad bad");
// 		return false;
// 	}
	
// 	return false;
// }

void NSAnimSystem::update()
{
	nsscene * scene = nsengine.currentScene();
	NSTimer * timer = nsengine.timer();
	if (scene == NULL)
		return;
	
	nspentityset ents = scene->entities<NSAnimComp>();
	auto entIter = ents.begin();
	while (entIter != ents.end())
	{
		NSAnimComp * animComp = (*entIter)->get<NSAnimComp>();
		NSRenderComp * renderComp = (*entIter)->get<NSRenderComp>();
		if (renderComp == NULL)
		{
			dprint("NSAnimSystem::update Entity has animation comp but no render comp - Cannot update");
			++entIter;
			continue;
		}
		
		if (animComp->updatePosted())
		{
			uivec2 meshID = renderComp->meshID();
			uivec2 animsetID = animComp->animationSetID();
			nsstring mCurrentAnim = animComp->currentAnimation();
			if (meshID == 0 || animsetID == 0)
			{
				dprint("NSAnimSystem::update Cannot update animation without AnimSetID and MeshID");
				++entIter;
				continue;
			}

			nsmesh * msh = nsengine.resource<nsmesh>(meshID);
			if (msh == NULL)
			{
				dprint("NSAnimSystem::update mesh with id " + meshID.toString() + " is null in anim ent " + (*entIter)->name());
				++entIter;
				continue;
			}

			nsmesh::node_tree * nTree = msh->tree();
			if (nTree == NULL)
			{
				dprint("NSAnimSystem::update msh node tree is null in anim ent " + (*entIter)->name());
				++entIter;
				continue;
			}

			auto finalTF = animComp->finalTransforms();
			finalTF->resize(nTree->bone_name_map.size());

			nsanim_set * animset = nsengine.resource<nsanim_set>(animsetID);
			if (animset == NULL)
			{
				++entIter;
				continue;
			}

			nsanim_set::animation_data * currAnim = animset->anim_data(mCurrentAnim);
			if (currAnim == NULL)
			{
				dprint("NSAnimSystem::update anim set not found " + (*entIter)->name());
				++entIter;
				continue;
			}

			if (animComp->animating())
			{
				animComp->elapsed() += timer->fixed();
				if (animComp->looping())
				{
					if (animComp->elapsed() >= currAnim->duration)
						animComp->elapsed() = 0.0f;
					animComp->fillBones(nTree, currAnim);
				}
				else
				{
					if (animComp->elapsed() >= currAnim->duration)
					{
						animComp->setAnimate(false);
						animComp->elapsed() = 0.0f;
						return;
					}
					animComp->fillBones(nTree, currAnim);
				}
				return;
			}
			animComp->postUpdate(false);
		}
		++entIter;
	}
}

int32 NSAnimSystem::update_priority()
{
	return ANIM_SYS_UPDATE_PR;
}
