/*!
\file nsmovementsystem.cpp

\brief Definition file for nsmovement_system class

This file contains all of the neccessary definitions for the nsmovement_system class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#include <nsmovement_system.h>
#include <nstform_comp.h>
#include <nsscene.h>
#include <nsentity.h>

nsmovement_system::nsmovement_system() :nssystem()
{}

nsmovement_system::~nsmovement_system()
{}

void nsmovement_system::init()
{}

void nsmovement_system::update()
{
	nsscene * scene = nsengine.current_scene();
	if (scene == NULL)
		return;

	auto entIter = scene->entities().begin();
	while (entIter != scene->entities().end())
	{
		nstform_comp * tForm = (*entIter)->get<nstform_comp>();
		if (tForm->update_posted())
		{
			nsbuffer_object & tFormBuf = *tForm->transform_buffer();
			nsbuffer_object & tFormIDBuf = *tForm->transform_id_buffer();

			if (tForm->buffer_resized())
			{
				tFormBuf.bind();
				tFormBuf.allocate<fmat4>(nsbuffer_object::mutable_dynamic_draw, tForm->count());
				tFormIDBuf.bind();
				tFormIDBuf.allocate<uint32>(nsbuffer_object::mutable_dynamic_draw, tForm->count());
			}

			tFormBuf.bind();
			fmat4 * mappedT = tFormBuf.map<fmat4>(0, tForm->count(), nsbuffer_object::access_map_range(nsbuffer_object::map_write));
			tFormIDBuf.bind();
			uint32 * mappedI = tFormIDBuf.map<uint32>(0, tForm->count(), nsbuffer_object::access_map_range(nsbuffer_object::map_write));
			tFormIDBuf.unbind();

			uint32 visibleCount = 0;
			for (uint32 i = 0; i < tForm->count(); ++i)
			{
				if (tForm->transform_update(i))
				{
					uivec3 parentID = tForm->parent_id(i);
					if (parentID != 0)
					{
						nsentity * ent = scene->entity(parentID.x, parentID.y);
						if (ent != NULL)
						{
							nstform_comp * tComp2 = ent->get<nstform_comp>();
							if (parentID.z < tComp2->count())
								tForm->set_parent(tComp2->transform(parentID.z));
						}
					}

					tForm->compute_transform(i);
					tForm->set_instance_update(false, i);
				}
				int32 state = tForm->hidden_state(i);

				bool layerBit = state & nstform_comp::hide_layer && true;
				bool objectBit = state & nstform_comp::hide_object && true;
				bool showBit = state & nstform_comp::hide_none && true;
				bool hideBit = state & nstform_comp::hide_all && true;

				if (!hideBit && (!layerBit && (showBit || !objectBit)))
				{
					mappedT[visibleCount] = tForm->transform(i);
					mappedI[visibleCount] = i;
					++visibleCount;
				}
			}
			tForm->set_visible_instance_count(visibleCount);


			tFormBuf.bind();
			tFormBuf.unmap();
			tFormIDBuf.bind();
			tFormIDBuf.unmap();
			tFormIDBuf.unbind();
			tForm->post_update(tForm->buffer_resized());
			tForm->set_buffer_resize(false);
		}
		++entIter;
	}
}

int32 nsmovement_system::update_priority()
{
	return MOVE_SYS_UPDATE_PR;
}
