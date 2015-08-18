/*!
\file nsmovementsystem.cpp

\brief Definition file for NSMovementSystem class

This file contains all of the neccessary definitions for the NSMovementSystem class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#include <nsmovement_system.h>
#include <nstform_comp.h>
#include <nsscene.h>
#include <nsentity.h>

NSMovementSystem::NSMovementSystem() :NSSystem()
{}

NSMovementSystem::~NSMovementSystem()
{}

void NSMovementSystem::init()
{}

// bool NSMovementSystem::handleEvent(NSEvent * pEvent)
// {
// 	return false;
// }

void NSMovementSystem::update()
{
	nsscene * scene = nsengine.currentScene();
	if (scene == NULL)
		return;

	auto entIter = scene->entities().begin();
	while (entIter != scene->entities().end())
	{
		NSTFormComp * tForm = (*entIter)->get<NSTFormComp>();
		if (tForm->updatePosted())
		{
			NSBufferObject & tFormBuf = *tForm->transformBuffer();
			NSBufferObject & tFormIDBuf = *tForm->transformIDBuffer();

			if (tForm->bufferResize())
			{
				tFormBuf.bind();
				tFormBuf.allocate<fmat4>(NSBufferObject::MutableDynamicDraw, tForm->count());
				tFormIDBuf.bind();
				tFormIDBuf.allocate<uint32>(NSBufferObject::MutableDynamicDraw, tForm->count());
			}

			tFormBuf.bind();
			fmat4 * mappedT = tFormBuf.map<fmat4>(0, tForm->count(), NSBufferObject::AccessMapRange(NSBufferObject::MapWrite));
			tFormIDBuf.bind();
			uint32 * mappedI = tFormIDBuf.map<uint32>(0, tForm->count(), NSBufferObject::AccessMapRange(NSBufferObject::MapWrite));
			tFormIDBuf.unbind();

			uint32 visibleCount = 0;
			for (uint32 i = 0; i < tForm->count(); ++i)
			{
				if (tForm->transUpdate(i))
				{
					uivec3 parentID = tForm->parentid(i);
					if (parentID != 0)
					{
						nsentity * ent = scene->entity(parentID.x, parentID.y);
						if (ent != NULL)
						{
							NSTFormComp * tComp2 = ent->get<NSTFormComp>();
							if (parentID.z < tComp2->count())
								tForm->setParent(tComp2->transform(parentID.z));
						}
					}

					tForm->computeTransform(i);
					tForm->setTransUpdate(false, i);
				}
				int32 state = tForm->hiddenState(i);

				bool layerBit = state & NSTFormComp::LayerHide && true;
				bool objectBit = state & NSTFormComp::ObjectHide && true;
				bool showBit = state & NSTFormComp::Show && true;
				bool hideBit = state & NSTFormComp::Hide && true;

				if (!hideBit && (!layerBit && (showBit || !objectBit)))
				{
					mappedT[visibleCount] = tForm->transform(i);
					mappedI[visibleCount] = i;
					++visibleCount;
				}
			}
			tForm->setVisibleTransformCount(visibleCount);


			tFormBuf.bind();
			tFormBuf.unmap();
			tFormIDBuf.bind();
			tFormIDBuf.unmap();
			tFormIDBuf.unbind();
			tForm->postUpdate(tForm->bufferResize());
			tForm->setBufferResize(false);
		}
		++entIter;
	}
}

int32 NSMovementSystem::update_priority()
{
	return MOVE_SYS_UPDATE_PR;
}
