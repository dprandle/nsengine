/*! 
	\file nsrender_comp.h
	
	\brief Header file for NSRenderComp class

	This file contains all of the neccessary declarations for the NSRenderComp class.

	\author Daniel Randle
	\date December 17 2013
	\copywrite Earth Banana Games 2013
*/

#ifndef NSRENDERCOMP_H
#define NSRENDERCOMP_H

#include <vector>
#include <nscomponent.h>
#include <map>
#include <nsmath.h>

class nsmesh;
class nstimer;
class nsmaterial;

class NSRenderComp : public NSComponent
{
public:

	template <class PUPer>
	friend void pup(PUPer & p, NSRenderComp & rc);

	NSRenderComp();
	~NSRenderComp();

	void clearMats();

	NSRenderComp* copy(const NSComponent* pToCopy);

	bool castShadow();

	uivec2 materialID(uint32 subMeshIndex);

	const uivec2 & meshID();

	virtual void name_change(const uivec2 & oldid, const uivec2 newid);

	/*!
	Get the resources that the component uses. The render comp uses a mesh and possibly multiple materials.
	/return Map of resource ID to resource type containing mesh ID and all material IDs.
	*/
	virtual uivec2array resources();

	bool hasMaterial(uint32 pSubMeshIndex);

	void init();

	uivec2uimap::iterator matBegin();

	uivec2uimap::const_iterator matBegin() const;

	uivec2uimap::iterator matEnd();

	uivec2uimap::const_iterator matEnd() const;

	bool removeMaterial(uint32 subMeshIndex);

	bool removeMaterialAll(const uivec2 & toremove);

	bool replaceMaterial(const uivec2 & oldid, const uivec2 & newid);

	bool replaceMaterial(uint32 oldplugid, uint32 oldresid, uint32 newplugid, uint32 newresid)
	{
		return replaceMaterial(uivec2(oldplugid, oldresid), uivec2(newplugid, newresid));
	}

	virtual void pup(nsfile_pupper * p);

	void setCastShadow(bool pShadow);

	bool setMaterial(uint32 pSubMeshIndex, const uivec2 & pMatID, bool pReplace = false);

	bool setMaterial(uint32 pSubMeshIndex, uint32 matplugid, uint32 matresid, bool pReplace = false)
	{
		return setMaterial(pSubMeshIndex, uivec2(matplugid, matresid), pReplace);
	}

	void setMeshID(const uivec2 & pMeshID);

	void setMeshID(uint32 plugid, uint32 resid)
	{
		mMeshID.x = plugid; mMeshID.y = resid;
		post_update(true);
	}

	NSRenderComp & operator=(const NSRenderComp & pRHSComp);

private:
	bool mCastShadow;
	uivec2 mMeshID;
	uivec2uimap mMats;
};

template <class PUPer>
void pup(PUPer & p, NSRenderComp & rc)
{
	pup(p, rc.mCastShadow, "castShadow");
	pup(p, rc.mMeshID, "meshID");
	pup(p, rc.mMats, "mats");
}

#endif
