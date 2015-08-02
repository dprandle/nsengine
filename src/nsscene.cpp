/*! 
	\file nsscene.cpp
	
	\brief Definition file for NSScene class

	This file contains all of the neccessary definitions for the NSScene class.

	\author Daniel Randle
	\date November 23 2013
	\copywrite Earth Banana Games 2013
*/

#include <nstilebrushcomp.h>
#include <nsscene.h>
#include <nsscenemanager.h>
#include <nsentity.h>
#include <nsrendercomp.h>
#include <nsanimcomp.h>
#include <nstformcomp.h>
#include <nstilegrid.h>
#include <nsrendersystem.h>
#include <nscamcomp.h>
#include <nsresmanager.h>
#include <nsselcomp.h>
#include <nsengine.h>
#include <nsentitymanager.h>
#include <nsinputcomp.h>
#include <nsoccupycomp.h>
#include <nspluginmanager.h>

NSScene::NSScene():
	mTileGrid(new NSTileGrid()),
	mCamID(),
	mSkydomeID(),
	mMaxPlayers(0),
	mBackgroundColor(),
	mNotes(),
	mCreator(),
	mShowBit(false)
{
	setExtension(DEFAULT_SCENE_EXTENSION);
}

NSScene::~NSScene()
{
	delete mTileGrid;
}

void NSScene::clear()
{
	nspentityset ents = entities();
	auto eiter = ents.begin();
	while (eiter != ents.end())
	{
		remove(*eiter);
		++eiter;
	}

	mUnloaded.clear();
}

/*!
Add single instance of an entity at position given by pPos, rotation given by pRot, and scaling factor given by pScale.
If the entity contains an occupy comp, then only insert in to the scene if the space given by pPos along with the entire
occupy component tile set can be inserted in to the tile grid. If adding to the scene fails, this will return -1 and if it
succeeds it will return the transform ID of the newly inserted entity instance.
*/
nsuint NSScene::add(NSEntity * pEnt, const fvec3 & pPos, const fquat & pRot, const fvec3 & pScale)
{
	if (pEnt == NULL)
		return -1;

	NSTFormComp * tComp = pEnt->get<NSTFormComp>();
	NSOccupyComp * occComp = pEnt->get<NSOccupyComp>();

	bool addTComp = false;

	// If there is no tranform component, then make one! This will automatically
	// call the updateCompMaps function so only need to call that if no new component is created
	if (tComp == NULL)
	{
		// First we must set the scene ID correctly so that 
		// when all components are added the scene comp maps are updated correctly
		tComp = pEnt->create<NSTFormComp>();
		addTComp = true;
	}

	// Set up the first tranform
	NSTFormComp::InstTrans t;
	t.mPosition = pPos;
	t.mScaling = pScale;
	t.mOrientation = pRot;

	// If there is an occupy component, attemp at inserting it in to the map. It will fail if the space
	// is already occupied therefore we need to remove the tranform component and set the entities scene
	// ID to 0 again if the add fails, since that means no other instances of the entity are in the scene
	if (occComp != NULL)
	{
		// Get the transform ID that would result if we inserted it in to the scene
		// We don't want to insert it yet because we first want to check if the space is open
		nsuint pID = tComp->count();

		if (!mTileGrid->add(uivec3(pEnt->plugid(), pEnt->id(), pID), occComp->spaces(), pPos))
		{
			// This is the part that we check if we just added the transform component or not
			if (addTComp)
			{
				// Set the scene ID to zero and remove the transform component.. this will also update all of the
				// entity by component lists in the scene (when removeComponent is called)
				pEnt->del<NSTFormComp>();
			}
			return -1;
		}
	}
	
	// Adding transform will never fail unless out of memory

	nsuint index = tComp->add(t);
	tComp->setHiddenState(NSTFormComp::Show, mShowBit, index);
	return index;
}

/*!
Add gridded tiles to the scene in a fashion that agrees with the current grid settings.
The grid settings are located in the global variables X_GRID, Y_GRID, and Z_GRID
The tile grid also uses these grid variables, so changing them should change the overall
behavior of grid snap and tile occupation etc.
*/
nsuint NSScene::addGridded(
	NSEntity * pEnt,
	const ivec3 & pBounds,
	const fvec3 & pStartingPos,
	const fquat & pRot,
	const fvec3 & pScale
	)
{
	if (pEnt == NULL)
		return -1;

	// Get the transform and occupy components.. if no transform component then make one
	// We have to update the comp maps if create component is not called
	NSOccupyComp * occComp = pEnt->get<NSOccupyComp>();
	NSTFormComp * tComp = pEnt->get<NSTFormComp>();
	if (tComp == NULL)
	{
		// First we must set the scene ID correctly so that 
		// when all components are added the scene comp maps are updated correctly
		tComp = pEnt->create<NSTFormComp>();
	}

	// Figure out the total number of transforms needed and allocate that 
	// much memory (addTransforms does this)
	nsuint addSize = pBounds.x * pBounds.y * pBounds.z;
	tComp->add(addSize);
	
	// Now get the size and go through and attempt to add each transform
	nsuint count = tComp->count() - addSize;
	for (nsint z = 0; z < pBounds.z; ++z)
	{
		for (nsint y = 0; y < pBounds.y; ++y)
		{
			for (nsint x = 0; x < pBounds.x; ++x)
			{
				float xP = X_GRID * x * 2.0f;
				float yP = Y_GRID * y;
				float zP = Z_GRID * z;
				if (y % 2 != 0)
					xP += X_GRID;
				fvec3 pos(xP, yP, zP);
				pos += pStartingPos;

				if (occComp != NULL)
				{
					// If there is an occupy comp then make sure that it can be inserted in to the tile grid
					// If not, remove the transform and continue without incrementing the count
					if (!mTileGrid->add(uivec3(pEnt->plugid(),pEnt->id(), count), occComp->spaces(), pos))
					{
						tComp->remove(count);
						continue;
					}
				}

				tComp->setpos(pos, count);
				tComp->rotate(pRot, count);
				tComp->scale(pScale, count);
				tComp->setHiddenState(NSTFormComp::Show, mShowBit, count);
				++count;
			}
		}
	}

	// If there are no transforms at all, then set the scene ID to 0 and remove the component
	// and return false to indicate that no transforms were added
	if (tComp->count() == 0)
	{
		pEnt->del<NSTFormComp>();
		return false;
	}
	return true;
}

/*!
Change the aximum number of players allowable on this scene - pAmount can be positive or negative
*/
void NSScene::changeMaxPlayers(nsint pAmount)
{
	if ((mMaxPlayers + pAmount) > SCENE_MAX_PLAYERS || (mMaxPlayers + pAmount) < 2)
	{
		dprint("NSScene::changeMaxPlayers Players must be in max player range");
		return;
	}

	mMaxPlayers += pAmount;
}

void NSScene::enableShowBit(nsbool pEnable)
{
	mShowBit = pEnable;
}

bool NSScene::hasDirLight() const
{
	auto iter = entities().begin();
	while (iter != entities().end())
	{
		NSEntity * ent = *iter;
		NSLightComp * lc = ent->get<NSLightComp>();
		if (lc != NULL)
		{
			if (lc->type() == NSLightComp::Directional)
				return true;
		}
		++iter;
	}
	return false;
}

const fvec3 & NSScene::backgroundColor() const
{
	return mBackgroundColor;
}

const nsstring & NSScene::creator() const
{
	return mCreator;
}

void NSScene::pup(NSFilePUPer * p)
{
	if (p->type() == NSFilePUPer::Binary)
	{
		NSBinFilePUPer * bf = static_cast<NSBinFilePUPer *>(p);
		::pup(*bf, *this);
	}
	else
	{
		NSTextFilePUPer * tf = static_cast<NSTextFilePUPer *>(p);
		::pup(*tf, *this);
	}
}

NSEntity * NSScene::camera() const
{
	return entity(mCamID.x, mCamID.y);
}

nsuint NSScene::maxPlayers() const
{
	return mMaxPlayers;
}

uivec3 NSScene::refid(const fvec3 & pWorldPos) const
{
	return mTileGrid->get(pWorldPos);
}

const nsuint NSScene::referenceCount() const
{
	nsuint count = 0;

	auto iter = entities().begin();
	while (iter != entities().end())
	{
		NSTFormComp * tComp = (*iter)->get<NSTFormComp>();
		if (!(*iter)->has<NSTileBrushComp>())
			count += tComp->count();
		++iter;
	}
	return count;
}

NSEntity * NSScene::skydome() const
{
	return entity(mSkydomeID.x, mSkydomeID.y);
}

const nspentityset & NSScene::entities() const
{
	return entities<NSTFormComp>();
}

/*!
Get the other resources that this Scene uses. This is given by all the Entities that currently exist in the scene.
*/
uivec2array NSScene::resources()
{
	uivec2array ret;
	auto iter = entities().begin();
	while (iter != entities().end())
	{
		uivec2array tmp = (*iter)->resources();
		ret.insert(ret.end(), tmp.begin(), tmp.end() );
		++iter;
	}
	return ret;
}

const nsstring & NSScene::notes() const
{
	return mNotes;
}

NSTileGrid & NSScene::grid()
{
	return *mTileGrid;
}

void NSScene::hideLayer(nsint pLayer, nsbool pHide)
{
	NSTileGrid::GridBounds g = mTileGrid->occupiedGridBounds();
	for (nsint y = g.minSpace.y; y <= g.maxSpace.y; ++y)
	{
		for (nsint x = g.minSpace.x; x <= g.maxSpace.x; ++x)
		{
			uivec3 id = mTileGrid->get(ivec3(x, y, -pLayer));
			if (id != uivec3())
			{
				NSEntity * ent = entity(id.x, id.y);
				if (ent != NULL)
				{
					ent->get<NSTFormComp>()->setHiddenState(NSTFormComp::LayerHide, pHide, id.z);
				}
			}
		}
	}
}

void NSScene::hideLayersAbove(nsint pBaseLayer, nsbool pHide)
{
	NSTileGrid::GridBounds g = mTileGrid->occupiedGridBounds();
	pBaseLayer *= -1;

	for (nsint z = pBaseLayer-1; z >= g.minSpace.z; --z)
	{
		for (nsint y = g.minSpace.y; y <= g.maxSpace.y; ++y)
		{
			for (nsint x = g.minSpace.x; x <= g.maxSpace.x; ++x)
			{
				uivec3 id = mTileGrid->get(ivec3(x, y, z));
				if (id != uivec3())
				{
					NSEntity * ent = entity(id.x, id.y);
					if (ent != NULL)
					{
						ent->get<NSTFormComp>()->setHiddenState(NSTFormComp::LayerHide, pHide, id.z);
					}
				}
			}
		}
	}

	if (pHide)
	{
		for (nsint y = g.minSpace.y; y <= g.maxSpace.y; ++y)
		{
			for (nsint x = g.minSpace.x; x <= g.maxSpace.x; ++x)
			{
				uivec3 id = mTileGrid->get(ivec3(x, y, pBaseLayer));
				if (id != uivec3())
				{
					NSEntity * ent = entity(id.x, id.y);
					if (ent != NULL)
					{
						ent->get<NSTFormComp>()->setHiddenState(NSTFormComp::LayerHide, !pHide, id.z);
					}
				}
			}
		}
	}
}

void NSScene::hideLayersBelow(nsint pTopLayer, nsbool pHide)
{
	NSTileGrid::GridBounds g = mTileGrid->occupiedGridBounds();
	pTopLayer *= -1;

	for (nsint z = pTopLayer+1; z <= g.maxSpace.z; ++z)
	{
		for (nsint y = g.minSpace.y; y <= g.maxSpace.y; ++y)
		{
			for (nsint x = g.minSpace.x; x <= g.maxSpace.x; ++x)
			{
				uivec3 id = mTileGrid->get(ivec3(x, y, z));
				if (id != uivec3())
				{
					NSEntity * ent = entity(id.x, id.y);
					if (ent != NULL)
					{
						ent->get<NSTFormComp>()->setHiddenState(NSTFormComp::LayerHide, pHide, id.z);
					}
				}
			}
		}
	}

	if (pHide)
	{
		for (nsint y = g.minSpace.y; y <= g.maxSpace.y; ++y)
		{
			for (nsint x = g.minSpace.x; x <= g.maxSpace.x; ++x)
			{
				uivec3 id = mTileGrid->get(ivec3(x, y, pTopLayer));
				if (id != uivec3())
				{
					NSEntity * ent = entity(id.x, id.y);
					if (ent != NULL)
					{
						ent->get<NSTFormComp>()->setHiddenState(NSTFormComp::LayerHide, !pHide, id.z);
					}
				}
			}
		}
	}
}

void NSScene::init()
{
}

nsbool NSScene::showBit() const
{
	return mShowBit;
}

/*!
This should be called if there was a name change to a resource
*/
void NSScene::nameChange(const uivec2 & oldid, const uivec2 newid)
{
	if (mCamID.x == oldid.x)
	{
		mCamID.x = newid.x;
		if (mCamID.y == oldid.y)
			mCamID.y = newid.y;
	}

	if (mSkydomeID.x == oldid.x)
	{
		mSkydomeID.x = newid.x;
		if (mSkydomeID.y == oldid.y)
			mSkydomeID.y = newid.y;
	}

	mTileGrid->nameChange(oldid, newid);
}

nsuint NSScene::replace(NSEntity * oldent, nsuint tformID, NSEntity * newent)
{
	if (oldent == NULL || newent == NULL)
		return false;

	NSTFormComp * tComp = oldent->get<NSTFormComp>();
	if (tComp == NULL)
		return false;

	fvec3 pos = tComp->wpos(tformID);

	remove(oldent, tformID);
	return add(newent, pos);
}

nsbool NSScene::replace(NSEntity * oldent, NSEntity * newent)
{
	if (oldent == NULL || newent == NULL)
		return false;

	NSTFormComp * tComp = oldent->get<NSTFormComp>();
	if (tComp == NULL)
		return false;

	bool ret = true;
	for (nsuint i = 0; i < tComp->count(); ++i)
		ret = replace(oldent, i, newent) && ret;

	return ret;
}

/*!
Removes the entity from the scene - if the entity is not part of the scene then will do nothing
This is true for all overloaded functions as well
*/
nsbool NSScene::remove(NSEntity * entity, nsuint tformid)
{
	if (entity == NULL)
		return false;

	NSTFormComp * tComp = entity->get<NSTFormComp>();
	if (tComp == NULL)
		return false;

	nsuint size = tComp->count();
	fvec3 pos = tComp->wpos(tformid);
	NSOccupyComp * occComp = entity->get<NSOccupyComp>();


	nsuint newSize = tComp->remove(tformid);
	bool ret = (newSize == (size - 1));

	if (occComp != NULL)
	{
		mTileGrid->remove(occComp->spaces(), pos);

		// This should take care of updating the reference ID for the space that got switched..
		// When removing transforms from the tForm comp internally it moves the last tForm to the position
		// being erased and pops of the last tForm.. this means that if the last tfrom is occupying some space
		// we need to update that space with the correct new tFormID
		if (newSize != 0 && newSize != tformid)
		{
			fvec3 newPos = tComp->wpos(tformid);
			mTileGrid->remove(occComp->spaces(), newPos);
			mTileGrid->add(uivec3(entity->plugid(), entity->id(), tformid), occComp->spaces(), newPos);
		}
	}

	if (newSize == 0)
	{
		ret = entity->del<NSTFormComp>();

		// If the enity being removed from the scene is the current camera or current skybox then make sure to set these to 0
		if (mSkydomeID == uivec2(entity->plugid(), entity->id()))
			mSkydomeID = 0;
		if (mCamID == uivec2(entity->plugid(),entity->id()))
			mCamID = 0;
	}
	
	return ret;
}

/*!
This overload is special in that it will remove whatever entity + refID is at the position given by pWorldPos.
If there is nothing in that position then returns false.
Note that in order for this function to work the entity that is being removed must have an occupy component or else
it will not be included in the tile grid.
*/
nsbool NSScene::remove(fvec3 & pWorldPos)
{
	uivec3 refid = mTileGrid->get(pWorldPos);
	if (refid == 0)
		return false;

	return remove(refid.x, refid.y, refid.z);
}

/*!
Remove all instances of the entity with name pEntName from the scene
Does so by entering a while loop that will become false once the entity
runs out of transforms
*/
nsbool NSScene::remove(NSEntity * ent)
{
	if (ent == NULL)
		return false;

	bool ret = true;
	while (ret)
		ret = remove(ent, nsuint(0)) && ret;

	return ret;
}

void NSScene::setBackgroundColor(const fvec3 & pBackgroundColor)
{
	mBackgroundColor = pBackgroundColor;
}

void NSScene::setCreator(const nsstring & pCreator)
{
	mCreator = pCreator;
}


void NSScene::setCamera(NSEntity * cam, bool addToSceneIfNeeded)
{
	if (cam == NULL)
		return;

	NSTFormComp * camtf = cam->get<NSTFormComp>();

	if (camtf != NULL)
	{
		// The parent must be enabled in the transform component for the camera to be
		// able to switch between camera modes properly
		camtf->enableParent(true);
		mCamID = uivec2(cam->plugid(),cam->id());
		dprint("NSScene::setCamera - Map \"" + mName + "\"'s camera set to \"" + cam->name() + "\"");
	}
	else
	{
		// If cam is still NULL this means that it did not exist already in the scene.. Here we will add it or not depending
		// on whether or not pAddToSceneIfNeeded is true or false
		if (addToSceneIfNeeded)
		{
			if (add(cam->plugid(),cam->id()) != -1) // -1 indicates failure
			{
				setCamera(cam);
			}
			else
			{
				dprint("NSScene::setCamera - Camera " + cam->name() + " could not be loaded in to the scene");
			}
		}
	}
}

void NSScene::setMaxPlayers(nsuint pMaxPlayers)
{
	if (pMaxPlayers > SCENE_MAX_PLAYERS || pMaxPlayers < 2)
	{
		dprint("NSScene::setMaxPlayers Players must be in max player range");
		return;
	}
	mMaxPlayers = pMaxPlayers;
}

void NSScene::setNotes(const nsstring & pNotes)
{
	mNotes = pNotes;
}

void NSScene::setSkydome(NSEntity * skydome, bool addToSceneIfNeeded)
{
	if (skydome == NULL)
		return;

	NSTFormComp * skytf = skydome->get<NSTFormComp>();

	if (skytf != NULL)
	{
		mSkydomeID = uivec2(skydome->plugid(), skydome->id());
		dprint("NSScene::setSkydome - Map \"" + mName + "\"'s skydome set to \"" + skydome->name() + "\"");
	}
	else
	{
		if (addToSceneIfNeeded)
		{
			if (add(skydome->plugid(), skydome->id()) != -1) // -1 indicates failure
			{
				setSkydome(skydome);
				dprint("NSScene::setSkydome - Map \"" + mName + "\"'s skydome set to \"" + skydome->name() + "\"");
			}
		}
	}
}

uivec2array & NSScene::unloaded()
{
	return mUnloaded;
}

/*!
Go through all entities and add only entities here that are part of the scene
*/
void NSScene::updateCompMaps(nsuint plugid, nsuint entid)
{
	NSEntity * ent = nsengine.resource<NSEntity>(plugid, entid);
	if (ent == NULL)
		return;

	if (ent->has<NSTFormComp>()) // if the entity is in the scene
	{
		// Insert the entity to each component type set - its okay if its already there insertion will just fail
		auto compiter = ent->begin();
		while (compiter != ent->end())
		{
			mEntsByCompType[compiter->first].emplace(ent);
			++compiter;
		}
	}
	else
	{
		auto ctiter = mEntsByCompType.begin();
		while (ctiter != mEntsByCompType.end())
		{
			ctiter->second.erase(ent);
			if (ctiter->second.empty())
				ctiter = mEntsByCompType.erase(ctiter);
			else
				++ctiter;
		}
	}

}
