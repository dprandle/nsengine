/*! 
	\file nsscene.cpp
	
	\brief Definition file for nsscene class

	This file contains all of the neccessary definitions for the nsscene class.

	\author Daniel Randle
	\date November 23 2013
	\copywrite Earth Banana Games 2013
*/

#include <nstile_brush_comp.h>
#include <nsscene.h>
#include <nsscene_manager.h>
#include <nsentity.h>
#include <nsrender_comp.h>
#include <nsanim_comp.h>
#include <nstform_comp.h>
#include <nstile_grid.h>
#include <nsrender_system.h>
#include <nscam_comp.h>
#include <nsres_manager.h>
#include <nssel_comp.h>
#include <nsengine.h>
#include <nsentity_manager.h>
#include <nsinput_comp.h>
#include <nsoccupy_comp.h>
#include <nsplugin_manager.h>

nsscene::nsscene():
	m_tile_grid(new NSTileGrid()),
	m_camera_id(),
	m_skydome_id(),
	m_max_players(0),
	m_bg_color(),
	m_notes(),
	m_creator(),
	m_show_bit(false)
{
	set_ext(DEFAULT_SCENE_EXTENSION);
}

nsscene::~nsscene()
{
	delete m_tile_grid;
}

void nsscene::clear()
{
	nspentityset ents = entities();
	auto eiter = ents.begin();
	while (eiter != ents.end())
	{
		remove(*eiter);
		++eiter;
	}

	m_unloaded.clear();
}

/*!
Add single instance of an entity at position given by pPos, rotation given by pRot, and scaling factor given by pScale.
If the entity contains an occupy comp, then only insert in to the scene if the space given by pPos along with the entire
occupy component tile set can be inserted in to the tile grid. If adding to the scene fails, this will return -1 and if it
succeeds it will return the transform ID of the newly inserted entity instance.
*/
uint32 nsscene::add(nsentity * pEnt, const fvec3 & pPos, const fquat & pRot, const fvec3 & pScale)
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
		uint32 pID = tComp->count();

		if (!m_tile_grid->add(uivec3(pEnt->plugin_id(), pEnt->id(), pID), occComp->spaces(), pPos))
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

	uint32 index = tComp->add(t);
	tComp->setHiddenState(NSTFormComp::Show, m_show_bit, index);
	return index;
}

/*!
Add gridded tiles to the scene in a fashion that agrees with the current grid settings.
The grid settings are located in the global variables X_GRID, Y_GRID, and Z_GRID
The tile grid also uses these grid variables, so changing them should change the overall
behavior of grid snap and tile occupation etc.
*/
uint32 nsscene::add_gridded(
	nsentity * pEnt,
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
	uint32 addSize = pBounds.x * pBounds.y * pBounds.z;
	tComp->add(addSize);
	
	// Now get the size and go through and attempt to add each transform
	uint32 count = tComp->count() - addSize;
	for (int32 z = 0; z < pBounds.z; ++z)
	{
		for (int32 y = 0; y < pBounds.y; ++y)
		{
			for (int32 x = 0; x < pBounds.x; ++x)
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
					if (!m_tile_grid->add(uivec3(pEnt->plugin_id(),pEnt->id(), count), occComp->spaces(), pos))
					{
						tComp->remove(count);
						continue;
					}
				}

				tComp->setpos(pos, count);
				tComp->rotate(pRot, count);
				tComp->scale(pScale, count);
				tComp->setHiddenState(NSTFormComp::Show, m_show_bit, count);
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
void nsscene::change_max_players(int32 pAmount)
{
	if ((m_max_players + pAmount) > SCENE_MAX_PLAYERS || (m_max_players + pAmount) < 2)
	{
		dprint("nsscene::changeMaxPlayers Players must be in max player range");
		return;
	}

	m_max_players += pAmount;
}

void nsscene::enable_show_bit(bool pEnable)
{
	m_show_bit = pEnable;
}

bool nsscene::has_dir_light() const
{
	auto iter = entities().begin();
	while (iter != entities().end())
	{
		nsentity * ent = *iter;
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

const fvec3 & nsscene::bg_color() const
{
	return m_bg_color;
}

const nsstring & nsscene::creator() const
{
	return m_creator;
}

void nsscene::pup(NSFilePUPer * p)
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

nsentity * nsscene::camera() const
{
	return entity(m_camera_id.x, m_camera_id.y);
}

uint32 nsscene::max_players() const
{
	return m_max_players;
}

uivec3 nsscene::ref_id(const fvec3 & pWorldPos) const
{
	return m_tile_grid->get(pWorldPos);
}

const uint32 nsscene::ref_count() const
{
	uint32 count = 0;

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

nsentity * nsscene::skydome() const
{
	return entity(m_skydome_id.x, m_skydome_id.y);
}

const nspentityset & nsscene::entities() const
{
	return entities<NSTFormComp>();
}

/*!
Get the other resources that this Scene uses. This is given by all the Entities that currently exist in the scene.
*/
uivec2array nsscene::resources()
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

const nsstring & nsscene::notes() const
{
	return m_notes;
}

NSTileGrid & nsscene::grid()
{
	return *m_tile_grid;
}

void nsscene::hide_layer(int32 pLayer, bool pHide)
{
	NSTileGrid::GridBounds g = m_tile_grid->occupiedGridBounds();
	for (int32 y = g.minSpace.y; y <= g.maxSpace.y; ++y)
	{
		for (int32 x = g.minSpace.x; x <= g.maxSpace.x; ++x)
		{
			uivec3 id = m_tile_grid->get(ivec3(x, y, -pLayer));
			if (id != uivec3())
			{
				nsentity * ent = entity(id.x, id.y);
				if (ent != NULL)
				{
					ent->get<NSTFormComp>()->setHiddenState(NSTFormComp::LayerHide, pHide, id.z);
				}
			}
		}
	}
}

void nsscene::hide_layers_above(int32 pBaseLayer, bool pHide)
{
	NSTileGrid::GridBounds g = m_tile_grid->occupiedGridBounds();
	pBaseLayer *= -1;

	for (int32 z = pBaseLayer-1; z >= g.minSpace.z; --z)
	{
		for (int32 y = g.minSpace.y; y <= g.maxSpace.y; ++y)
		{
			for (int32 x = g.minSpace.x; x <= g.maxSpace.x; ++x)
			{
				uivec3 id = m_tile_grid->get(ivec3(x, y, z));
				if (id != uivec3())
				{
					nsentity * ent = entity(id.x, id.y);
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
		for (int32 y = g.minSpace.y; y <= g.maxSpace.y; ++y)
		{
			for (int32 x = g.minSpace.x; x <= g.maxSpace.x; ++x)
			{
				uivec3 id = m_tile_grid->get(ivec3(x, y, pBaseLayer));
				if (id != uivec3())
				{
					nsentity * ent = entity(id.x, id.y);
					if (ent != NULL)
					{
						ent->get<NSTFormComp>()->setHiddenState(NSTFormComp::LayerHide, !pHide, id.z);
					}
				}
			}
		}
	}
}

void nsscene::hide_layers_below(int32 pTopLayer, bool pHide)
{
	NSTileGrid::GridBounds g = m_tile_grid->occupiedGridBounds();
	pTopLayer *= -1;

	for (int32 z = pTopLayer+1; z <= g.maxSpace.z; ++z)
	{
		for (int32 y = g.minSpace.y; y <= g.maxSpace.y; ++y)
		{
			for (int32 x = g.minSpace.x; x <= g.maxSpace.x; ++x)
			{
				uivec3 id = m_tile_grid->get(ivec3(x, y, z));
				if (id != uivec3())
				{
					nsentity * ent = entity(id.x, id.y);
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
		for (int32 y = g.minSpace.y; y <= g.maxSpace.y; ++y)
		{
			for (int32 x = g.minSpace.x; x <= g.maxSpace.x; ++x)
			{
				uivec3 id = m_tile_grid->get(ivec3(x, y, pTopLayer));
				if (id != uivec3())
				{
					nsentity * ent = entity(id.x, id.y);
					if (ent != NULL)
					{
						ent->get<NSTFormComp>()->setHiddenState(NSTFormComp::LayerHide, !pHide, id.z);
					}
				}
			}
		}
	}
}

void nsscene::init()
{
}

bool nsscene::show_bit() const
{
	return m_show_bit;
}

/*!
This should be called if there was a name change to a resource
*/
void nsscene::name_change(const uivec2 & oldid, const uivec2 newid)
{
	if (m_camera_id.x == oldid.x)
	{
		m_camera_id.x = newid.x;
		if (m_camera_id.y == oldid.y)
			m_camera_id.y = newid.y;
	}

	if (m_skydome_id.x == oldid.x)
	{
		m_skydome_id.x = newid.x;
		if (m_skydome_id.y == oldid.y)
			m_skydome_id.y = newid.y;
	}

	m_tile_grid->name_change(oldid, newid);
}

uint32 nsscene::replace(nsentity * oldent, uint32 tformID, nsentity * newent)
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

bool nsscene::replace(nsentity * oldent, nsentity * newent)
{
	if (oldent == NULL || newent == NULL)
		return false;

	NSTFormComp * tComp = oldent->get<NSTFormComp>();
	if (tComp == NULL)
		return false;

	bool ret = true;
	for (uint32 i = 0; i < tComp->count(); ++i)
		ret = replace(oldent, i, newent) && ret;

	return ret;
}

/*!
Removes the entity from the scene - if the entity is not part of the scene then will do nothing
This is true for all overloaded functions as well
*/
bool nsscene::remove(nsentity * entity, uint32 tformid)
{
	if (entity == NULL)
		return false;

	NSTFormComp * tComp = entity->get<NSTFormComp>();
	if (tComp == NULL)
		return false;

	uint32 size = tComp->count();
	fvec3 pos = tComp->wpos(tformid);
	NSOccupyComp * occComp = entity->get<NSOccupyComp>();


	uint32 newSize = tComp->remove(tformid);
	bool ret = (newSize == (size - 1));

	if (occComp != NULL)
	{
		m_tile_grid->remove(occComp->spaces(), pos);

		// This should take care of updating the reference ID for the space that got switched..
		// When removing transforms from the tForm comp internally it moves the last tForm to the position
		// being erased and pops of the last tForm.. this means that if the last tfrom is occupying some space
		// we need to update that space with the correct new tFormID
		if (newSize != 0 && newSize != tformid)
		{
			fvec3 newPos = tComp->wpos(tformid);
			m_tile_grid->remove(occComp->spaces(), newPos);
			m_tile_grid->add(uivec3(entity->plugin_id(), entity->id(), tformid), occComp->spaces(), newPos);
		}
	}

	if (newSize == 0)
	{
		ret = entity->del<NSTFormComp>();

		// If the enity being removed from the scene is the current camera or current skybox then make sure to set these to 0
		if (m_skydome_id == uivec2(entity->plugin_id(), entity->id()))
			m_skydome_id = 0;
		if (m_camera_id == uivec2(entity->plugin_id(),entity->id()))
			m_camera_id = 0;
	}
	
	return ret;
}

/*!
This overload is special in that it will remove whatever entity + refID is at the position given by pWorldPos.
If there is nothing in that position then returns false.
Note that in order for this function to work the entity that is being removed must have an occupy component or else
it will not be included in the tile grid.
*/
bool nsscene::remove(fvec3 & pWorldPos)
{
	uivec3 refid = m_tile_grid->get(pWorldPos);
	if (refid == 0)
		return false;

	return remove(refid.x, refid.y, refid.z);
}

/*!
Remove all instances of the entity with name pEntName from the scene
Does so by entering a while loop that will become false once the entity
runs out of transforms
*/
bool nsscene::remove(nsentity * ent)
{
	if (ent == NULL)
		return false;

	bool ret = true;
	while (ret)
		ret = remove(ent, uint32(0)) && ret;

	return ret;
}

void nsscene::set_bg_color(const fvec3 & pBackgroundColor)
{
	m_bg_color = pBackgroundColor;
}

void nsscene::set_creator(const nsstring & pCreator)
{
	m_creator = pCreator;
}


void nsscene::set_camera(nsentity * cam, bool addToSceneIfNeeded)
{
	if (cam == NULL)
		return;

	NSTFormComp * camtf = cam->get<NSTFormComp>();

	if (camtf != NULL)
	{
		// The parent must be enabled in the transform component for the camera to be
		// able to switch between camera modes properly
		camtf->enableParent(true);
		m_camera_id = uivec2(cam->plugin_id(),cam->id());
		dprint("nsscene::setCamera - Map \"" + m_name + "\"'s camera set to \"" + cam->name() + "\"");
	}
	else
	{
		// If cam is still NULL this means that it did not exist already in the scene.. Here we will add it or not depending
		// on whether or not pAddToSceneIfNeeded is true or false
		if (addToSceneIfNeeded)
		{
			if (add(cam,fvec3(0.0f,0.0f,-10.0f)) != -1) // -1 indicates failure
			{
				set_camera(cam);
			}
			else
			{
				dprint("nsscene::setCamera - Camera " + cam->name() + " could not be loaded in to the scene");
			}
		}
	}
}

void nsscene::set_max_players(uint32 pMaxPlayers)
{
	if (pMaxPlayers > SCENE_MAX_PLAYERS || pMaxPlayers < 2)
	{
		dprint("nsscene::setMaxPlayers Players must be in max player range");
		return;
	}
	m_max_players = pMaxPlayers;
}

void nsscene::set_notes(const nsstring & pNotes)
{
	m_notes = pNotes;
}

void nsscene::set_skydome(nsentity * skydome, bool addToSceneIfNeeded)
{
	if (skydome == NULL)
		return;

	NSTFormComp * skytf = skydome->get<NSTFormComp>();

	if (skytf != NULL)
	{
		m_skydome_id = uivec2(skydome->plugin_id(), skydome->id());
		dprint("nsscene::setSkydome - Map \"" + m_name + "\"'s skydome set to \"" + skydome->name() + "\"");
	}
	else
	{
		if (addToSceneIfNeeded)
		{
			if (add(skydome->plugin_id(), skydome->id()) != -1) // -1 indicates failure
			{
				set_skydome(skydome);
				dprint("nsscene::setSkydome - Map \"" + m_name + "\"'s skydome set to \"" + skydome->name() + "\"");
			}
		}
	}
}

uivec2array & nsscene::unloaded()
{
	return m_unloaded;
}

/*!
Go through all entities and add only entities here that are part of the scene
*/
void nsscene::update_comp_maps(uint32 plugid, uint32 entid)
{
	nsentity * ent = nsengine.resource<nsentity>(plugid, entid);
	if (ent == NULL)
		return;

	if (ent->has<NSTFormComp>()) // if the entity is in the scene
	{
		// Insert the entity to each component type set - its okay if its already there insertion will just fail
		auto compiter = ent->begin();
		while (compiter != ent->end())
		{
			m_ents_by_comp_type[compiter->first].emplace(ent);
			++compiter;
		}
	}
	else
	{
		auto ctiter = m_ents_by_comp_type.begin();
		while (ctiter != m_ents_by_comp_type.end())
		{
			ctiter->second.erase(ent);
			if (ctiter->second.empty())
				ctiter = m_ents_by_comp_type.erase(ctiter);
			else
				++ctiter;
		}
	}

}
