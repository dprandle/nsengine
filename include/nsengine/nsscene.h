/*! 
	\file nsscene.h
	
	\brief Header file for NSScene class

	This file contains all of the neccessary declarations for the NSScene class.

	\author Daniel Randle
	\date November 23 2013
	\copywrite Earth Banana Games 2013
*/

#ifndef NSSCENE_H
#define NSSCENE_H

#include <vector>
#include <map>
#include <nsresource.h>
#include <nsentity.h>
#include <nsentitymanager.h>
#include <nsglobal.h>
#include <nsoccupycomp.h>
#include <nstilegrid.h>

class NSRenderComp;
class NSRenderSystem;

class NSScene : public NSResource
{
public:
	typedef std::map<nsuint, nspentityset> EntitiesByComp;

	template<class PUPer>
	friend void pup(PUPer & p, NSScene & sc);

	NSScene();
	~NSScene();

	nsuint add(
		NSEntity * pEnt,
		const fvec3 & pPos = fvec3(),
		const fquat & pRot = fquat(),
		const fvec3 & pScale = fvec3(1.0f, 1.0f, 1.0f)
		);

	template<class T1, class T2>
	nsuint add(
		const T1 & plug,
		const T2 & ref,
		const fvec3 & pPos = fvec3(),
		const fquat & pRot = fquat(),
		const fvec3 & pScale = fvec3(1.0f, 1.0f, 1.0f)
		)
	{
		return add(nsengine.resource<NSEntity>(plug, ref), pPos, pRot, pScale);
	}

	nsuint addGridded(
		NSEntity * pEnt, 
		const ivec3 & pBounds, 
		const fvec3 & pStartingPos=fvec3(),
		const fquat & pRot = fquat(),
		const fvec3 & pScale = fvec3(1.0f,1.0f,1.0f)
		);

	template<class T1, class T2>
	nsuint addGridded(
		const T1 & plug,
		const T2 & ref,
		const ivec3 & pBounds,
		const fvec3 & pStartingPos = fvec3(),
		const fquat & pRot = fquat(),
		const fvec3 & pScale = fvec3(1.0f, 1.0f, 1.0f)
		)
	{
		return addGridded(nsengine.resource<NSEntity>(plug, ref), pBounds, pStartingPos, pRot, pScale);
	}

	void changeMaxPlayers(nsint pAmount);

	void clear();

	void enableShowBit(nsbool pEnable);

	template<class T1, class T2>
	NSEntity * entity(const T1 & plug, const T2 & res) const
	{
		NSEntity * ent = nsengine.resource<NSEntity>(plug, res);
		if (ent == NULL || !ent->has<NSTFormComp>())
			return NULL;
		return ent;
	}

	NSEntity * entity(const uivec2 & id) const { return entity(id.x, id.y); }

	uivec3 refid(const fvec3 & pWorldPos) const;

	const fvec3 & backgroundColor() const;

	NSEntity * camera() const;

	bool hasDirLight() const;

	nsuint maxPlayers() const;

	const nsstring & notes() const;

	const nsuint referenceCount() const;

	NSEntity * skydome() const;

	const nspentityset & entities() const;

	virtual void pup(NSFilePUPer * p);

	void init();

	template<class CompType>
	const nspentityset & entities() const
	{
		nsuint type_id = nsengine.typeID(std::type_index(typeid(CompType)));
		auto fiter = mEntsByCompType.find(type_id);
		if (fiter != mEntsByCompType.end())
			return fiter->second;
		return dummyret;
	}

	/*!
	Get the other resources that this Scene uses. This is given by all the Entities that currently exist in the scene.
	\return Map of resource ID to resource type containing all used resources
	*/
	virtual uivec2array resources();

	NSTileGrid & grid();

	void hideLayer(nsint pLayer, nsbool pHide);

	void hideLayersAbove(nsint pBaseLayer, nsbool pHide);

	void hideLayersBelow(nsint pTopLayer, nsbool pHide);

	nsbool showBit() const;

	/*!
	This should be called if there was a name change to a resource - will check if the resource is used by this component and if is
	is then it will update the handle
	*/
	virtual void nameChange(const uivec2 & oldid, const uivec2 newid);

	nsuint replace(NSEntity * oldent, nsuint tformID, NSEntity * newent);

	template<class T1, class T2>
	nsuint replace(const T1 & oldplug, const T2 & oldent, nsuint tformID, NSEntity * newent)
	{
		return replace(entity(oldplug, oldent), tformID, newent);
	}

	template<class T3, class T4>
	nsuint replace(NSEntity * oldent, nsuint tformID, const T3 & newplug, const T4 & newent)
	{
		return replace(oldent, tformID, entity(newplug, newent));
	}

	template<class T1, class T2, class T3, class T4>
	nsuint replace(const T1 & oldplug, const T2 & oldent, nsuint tformID, const T3 & newplug, const T4 & newent)
	{
		return replace(entity(oldplug, oldent), tformID, entity(newplug, newent));
	}

	nsbool replace(NSEntity * oldent, NSEntity * newent);

	template<class T1, class T2>
	nsuint replace(const T1 & oldplug, const T2 & oldent, NSEntity * newent)
	{
		return replace(entity(oldplug, oldent), newent);
	}

	template<class T3, class T4>
	nsuint replace(NSEntity * oldent, const T3 & newplug, const T4 & newent)
	{
		return replace(oldent, entity(newplug, newent));
	}

	template<class T1, class T2, class T3, class T4>
	nsuint replace(const T1 & oldplug, const T2 & oldent, const T3 & newplug, const T4 & newent)
	{
		return replace(entity(oldplug, oldent), entity(newplug, newent));
	}

	template<class T1, class T2>
	nsbool remove(const T1 & plug, const T2 & res, nsuint tformid)
	{
		return remove(entity(plug, res), tformid);
	}

	nsbool remove(fvec3 & pWorldPos);

	nsbool remove(NSEntity * ent);

	template<class T1, class T2>
	nsbool remove(const T1 & plug, const T2 & res)
	{
		remove(entity(plug, res));
	}

	nsbool remove(NSEntity * entity, nsuint tformid);

	void setBackgroundColor(const fvec3 & pColor);

	void setCreator(const nsstring & pCreator);

	const nsstring & creator() const;

	void setCamera(NSEntity * cam, bool addToSceneIfNeeded = true);

	template<class T1, class T2>
	void setCamera(const T1 & plug, const T2 & camid, bool addToSceneIfNeeded = true)
	{
		setCamera(nsengine.resource<NSEntity>(plug, camid), addToSceneIfNeeded);
	}

	void setMaxPlayers(nsuint pMaxPlayers);

	void setNotes(const nsstring & pNotes);

	template<class T1, class T2>
	void setSkydome(const T1 & plug, const T2 & skyid, bool addToSceneIfNeeded = true)
	{
		setSkydome(nsengine.resource<NSEntity>(plug, skyid), addToSceneIfNeeded);
	}

	void setSkydome(NSEntity * skydome, bool addToSceneIfNeeded = true);

	void updateCompMaps(nsuint plugid, nsuint entid);

	uivec2array & unloaded();

private:

	uivec2 mCamID;
	uivec2 mSkydomeID;
	nsuint mMaxPlayers;
	fvec3 mBackgroundColor;
	EntitiesByComp mEntsByCompType;
	nsstring mNotes;
	nsstring mCreator;
	NSTileGrid * mTileGrid;
	nsbool mShowBit;

	uivec2array mUnloaded;
	nspentityset dummyret;
};


template<class PUPer>
void pup(PUPer & p, NSScene & sc)
{
	pup(p, sc.mCamID, "camID");
	pup(p, sc.mSkydomeID, "skydomeID");
	pup(p, sc.mMaxPlayers, "maxPlayers");
	pup(p, sc.mBackgroundColor, "backgroundColor");
	pup(p, sc.mNotes, "notes");
	pup(p, sc.mCreator, "creator");
	pup(p, sc.mShowBit, "showBit");

	sc.unloaded().clear();
	uivec2array entids;


	auto iter = sc.entities().begin();
	while (iter != sc.entities().end())
	{
		entids.push_back(uivec2((*iter)->plugid(), (*iter)->id()));
		++iter;
	}

	pup(p, entids, "entids");

	for (nsuint i = 0; i < entids.size(); ++i)
	{
		NSEntity * ent = nsengine.resource<NSEntity>(entids[i]);

		if (ent == NULL)
		{
			sc.unloaded().push_back(entids[i]);
			continue;
		}

		NSTFormComp * tc = ent->get<NSTFormComp>();
		if (tc == NULL)
			tc = ent->create<NSTFormComp>();
		tc->pup(&p);
		if (p.mode() == PUP_IN)
		{
			NSOccupyComp * oc = ent->get<NSOccupyComp>();
			if (oc != NULL)
			{
				for (nsuint i = 0; i < tc->count(); ++i)
					sc.grid().add(uivec3(ent->plugid(), ent->id(), i), tc->wpos(i));
			}
		}
	}
}

#endif
