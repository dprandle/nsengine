/*! 
	\file nsrendercomp.cpp
	
	\brief Definition file for NSRenderComp class

	This file contains all of the neccessary definitions for the NSRenderComp class.

	\author Daniel Randle
	\date December 17 2013
	\copywrite Earth Banana Games 2013
*/

#include <nsrender_comp.h>
#include <nsmesh.h>
#include <nsentity.h>
#include <nstimer.h>
#include <nsmaterial.h>
#include <nsmat_manager.h>
#include <nsengine.h>
#include <nsmesh_manager.h>


NSRenderComp::NSRenderComp() : 
NSComponent(),
mCastShadow(true),
mMeshID(0)
{}

NSRenderComp::~NSRenderComp()
{}

void NSRenderComp::clearMats()
{
	mMats.clear();
}

NSRenderComp* NSRenderComp::copy(const NSComponent * pToCopy)
{
	if (pToCopy == NULL)
		return NULL;
	const NSRenderComp * comp = (const NSRenderComp*)pToCopy;
	(*this) = (*comp);
	return this;
}


bool NSRenderComp::castShadow()
{
	return mCastShadow;
}

uivec2 NSRenderComp::materialID(uint32 subMeshIndex)
{
	auto fiter = mMats.find(subMeshIndex);
	if (fiter != mMats.end())
		return fiter->second;
	return uivec2();
}

const uivec2 & NSRenderComp::meshID()
{
	return mMeshID;
}

void NSRenderComp::name_change(const uivec2 & oldid, const uivec2 newid)
{
	if (mMeshID.x == oldid.x)
	{
		mMeshID.x = newid.x;
		if (mMeshID.y == oldid.y)
			mMeshID.y = newid.y;
	}

	// Add all materials
	auto iter = matBegin();
	while (iter != matEnd())
	{
		if (iter->second.x == oldid.x)
		{
			iter->second.x = newid.x;
			if (iter->second.y == oldid.y)
				iter->second.y = newid.y;
		}
		++iter;
	}
}

/*!
Get the resources that the component uses. The render comp uses a mesh and possibly multiple materials.
*/
uivec2array NSRenderComp::resources()
{
	// Build map
	uivec2array ret;

	// only add resources if they are not 0
	if (mMeshID != 0)
		ret.push_back(mMeshID);

	// Add all materials
	auto iter = matBegin();
	while (iter != matEnd())
	{
		if (iter->second != 0) // only add if the submesh has a material assigned (it might not)
			ret.push_back(iter->second);
		++iter;
	}

	return ret;
}

void NSRenderComp::pup(nsfile_pupper * p)
{
	if (p->type() == nsfile_pupper::pup_binary)
	{
		nsbinary_file_pupper * bf = static_cast<nsbinary_file_pupper *>(p);
		::pup(*bf, *this);
	}
	else
	{
		nstext_file_pupper * tf = static_cast<nstext_file_pupper *>(p);
		::pup(*tf, *this);
	}
}

bool NSRenderComp::hasMaterial(uint32 pSubMeshIndex)
{
	return (mMats.find(pSubMeshIndex) != mMats.end());
}


void NSRenderComp::init()
{}

uivec2uimap::iterator NSRenderComp::matBegin()
{
	return mMats.begin();
}

uivec2uimap::const_iterator NSRenderComp::matBegin() const
{
	return mMats.begin();
}

uivec2uimap::iterator NSRenderComp::matEnd()
{
	return mMats.end();
}

uivec2uimap::const_iterator NSRenderComp::matEnd() const
{
	return mMats.end();
}

bool NSRenderComp::removeMaterial(uint32 pSubMeshIndex)
{
	if (!hasMaterial(pSubMeshIndex))
		return false;
	mMats.erase(pSubMeshIndex);
	return true;
}


bool NSRenderComp::removeMaterialAll(const uivec2 & toremove)
{
	uivec2uimap::iterator iter = mMats.begin();
	bool ret = false;
	while (iter != mMats.end())
	{
		if (iter->second == toremove)
		{
			iter = mMats.erase(iter);
			ret = true;
		}
		else
			++iter;
	}
	return ret;
}


bool NSRenderComp::replaceMaterial(const uivec2 & oldid, const uivec2 & newid)
{
	uivec2uimap::iterator iter = mMats.begin();
	bool ret = false;
	while (iter != mMats.end())
	{
		if (iter->second == oldid)
		{
			iter->second = newid;
			ret = true;
		}
		++iter;
	}
	return ret;
}


bool NSRenderComp::setMaterial(uint32 pSubMeshIndex, const uivec2 & pMatID, bool pReplace)
{
	if (hasMaterial(pSubMeshIndex))
	{
		if (!pReplace)
			return false;
		mMats.at(pSubMeshIndex) = pMatID;
		return true;
	}
	mMats[pSubMeshIndex] = pMatID;
	return true;
}


void NSRenderComp::setCastShadow(bool pShadow)
{
	mCastShadow = pShadow;
}

void NSRenderComp::setMeshID(const uivec2 & pMeshID)
{
	mMeshID = pMeshID;
}

NSRenderComp & NSRenderComp::operator=(const NSRenderComp & pRHSComp)
{
	mMeshID = pRHSComp.mMeshID;
	mCastShadow = pRHSComp.mCastShadow;
	
	mMats.clear();
	mMats.insert(pRHSComp.mMats.begin(), pRHSComp.mMats.end());
	post_update(true);
	return (*this);
}
