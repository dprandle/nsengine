/*! 
	\file nsrendercomp.h
	
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

class NSMesh;
class NSTimer;
class NSMaterial;

class NSRenderComp : public NSComponent
{
public:

	template <class PUPer>
	friend void pup(PUPer & p, NSRenderComp & rc);

	NSRenderComp();
	~NSRenderComp();

	void clearMats();

	NSRenderComp* copy(const NSComponent* pToCopy);

	nsbool castShadow();

	uivec2 materialID(nsuint subMeshIndex);

	const uivec2 & meshID();

	virtual void nameChange(const uivec2 & oldid, const uivec2 newid);

	/*!
	Get the resources that the component uses. The render comp uses a mesh and possibly multiple materials.
	/return Map of resource ID to resource type containing mesh ID and all material IDs.
	*/
	virtual uivec2array resources();

	nsbool hasMaterial(nsuint pSubMeshIndex);

	void init();

	uivec2uimap::iterator matBegin();

	uivec2uimap::const_iterator matBegin() const;

	uivec2uimap::iterator matEnd();

	uivec2uimap::const_iterator matEnd() const;

	nsbool removeMaterial(nsuint subMeshIndex);

	nsbool removeMaterialAll(const uivec2 & toremove);

	nsbool replaceMaterial(const uivec2 & oldid, const uivec2 & newid);

	nsbool replaceMaterial(nsuint oldplugid, nsuint oldresid, nsuint newplugid, nsuint newresid)
	{
		return replaceMaterial(uivec2(oldplugid, oldresid), uivec2(newplugid, newresid));
	}

	virtual void pup(NSFilePUPer * p);

	void setCastShadow(nsbool pShadow);

	nsbool setMaterial(nsuint pSubMeshIndex, const uivec2 & pMatID, nsbool pReplace = false);

	nsbool setMaterial(nsuint pSubMeshIndex, nsuint matplugid, nsuint matresid, nsbool pReplace = false)
	{
		return setMaterial(pSubMeshIndex, uivec2(matplugid, matresid), pReplace);
	}

	void setMeshID(const uivec2 & pMeshID);

	void setMeshID(nsuint plugid, nsuint resid)
	{
		mMeshID.x = plugid; mMeshID.y = resid;
		postUpdate(true);
	}

	static nsstring getTypeString();

	virtual nsstring typeString() { return getTypeString(); }

	NSRenderComp & operator=(const NSRenderComp & pRHSComp);

private:
	nsbool mCastShadow;
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