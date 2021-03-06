/*!
\file nsanimsystem.cpp

\brief Definition file for nsanim_system class

This file contains all of the neccessary definitions for the nsanim_system class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#include <system/nsanim_system.h>
#include <component/nsanim_comp.h>
#include <nsentity.h>
#include <asset/nsanim_manager.h>
#include <asset/nsmesh_manager.h>
#include <nsengine.h>
#include <nstimer.h>
#include <nsevent.h>
#include <nsevent_handler.h>
#include <asset/nsplugin_manager.h>
#include <asset/nsasset_manager.h>
#include <component/nsrender_comp.h>
#include <nsworld_data.h>
#include <nstform_ent_chunk.h>

nsanim_system::nsanim_system() :
	nssystem(type_to_hash(nsanim_system))
{
}

nsanim_system::~nsanim_system()
{}

void nsanim_system::init()
{}

void nsanim_system::release()
{}

void nsanim_system::update()
{
	if (chunk_error_check())
		return;
	
	auto ents = m_active_chunk->entities_with_comp<nsanim_comp>();

	if (ents == nullptr)
		return;
	
	auto entIter = ents->begin();
	while (entIter != ents->end())
	{
		nsanim_comp * animComp = (*entIter)->get<nsanim_comp>();
		nsrender_comp * renderComp = (*entIter)->get<nsrender_comp>();
		if (renderComp == nullptr)
		{
			dprint("nsanim_system::update Entity has animation comp but no render comp - Cannot update");
			++entIter;
			continue;
		}
		
		if (animComp->update_posted())
		{
			uivec2 meshID = renderComp->mesh_id();
			uivec2 animsetID = animComp->anim_set_id();
			nsstring mCurrentAnim = animComp->current_anim_name();
			if (meshID == 0 || animsetID == 0)
			{
				dprint("nsanim_system::update Cannot update animation without anim set and mesh id");
				animComp->post_update(false);
				++entIter;
				continue;
			}

			nsmesh * msh = get_asset<nsmesh>(meshID);
			if (msh == nullptr)
			{
				dprint("nsanim_system::update mesh with id " + meshID.to_string() + " is null in anim ent " + (*entIter)->name());
				++entIter;
				animComp->post_update(false);
				continue;
			}

			nsmesh::node_tree * nTree = msh->tree();
			if (nTree == nullptr)
			{
				dprint("nsanim_system::update msh node tree is null in anim ent " + (*entIter)->name());
				++entIter;
				animComp->post_update(false);
				continue;
			}

			auto finalTF = animComp->final_transforms();
			finalTF->resize(nTree->m_name_joint_map.size());

			nsanim_set * animset = get_asset<nsanim_set>(animsetID);
			if (animset == nullptr)
			{
				dprint("nsanim_system::update animset is null in anim ent " + (*entIter)->name());
				++entIter;
				animComp->post_update(false);
				continue;
			}

			animation_data * currAnim = animset->anim_data(mCurrentAnim);
			if (currAnim == nullptr)
			{
				dprint("nsanim_system::update anim set not found " + (*entIter)->name());
				animComp->post_update(false);
				++entIter;
				continue;
			}

			if (animComp->animating())
			{
				animComp->elapsed() += nse.timer()->fixed();
				if (animComp->looping())
				{
					if (animComp->elapsed() >= currAnim->duration)
						animComp->elapsed() = 0.0f;
					animComp->fill_joints(nTree, currAnim);
				}
				else
				{
					if (animComp->elapsed() >= currAnim->duration)
					{
						animComp->set_animate(false);
						animComp->elapsed() = 0.0f;
						return;
					}
					animComp->fill_joints(nTree, currAnim);
				}
				return;
			}
			animComp->post_update(false);
		}
		++entIter;
	}
}

int32 nsanim_system::update_priority()
{
	return ANIM_SYS_UPDATE_PR;
}
