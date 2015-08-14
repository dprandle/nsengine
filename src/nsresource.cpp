/*! 
	\file nsresource.cpp
	
	\brief Definition file for NSResource class

	This file contains all of the neccessary definitions for the NSResource class.

	\author Daniel Randle
	\date November 23 2013
	\copywrite Earth Banana Games 2013
*/

#include <nsresource.h>
#ifdef NOTIFY_ENGINE_NAME_CHANGE
#include <nsengine.h>
#endif

NSResource::NSResource() :
mIconPath(),
mIconTexID(),
mName(),
mSubDir(),
mID(0),
mPlugID(0),
mHashedType(0),
mOwned(false)
{}

NSResource::~NSResource()
{}

const nsstring & NSResource::extension() const
{
	return mExtension;
}

uivec2 NSResource::fullid()
{
	return uivec2(mPlugID, mID);
}

void NSResource::setIconPath(const nsstring & pIconPath)
{
	mIconPath = pIconPath;
}

void NSResource::setIconTexID(const uivec2 & texID)
{
	mIconTexID = texID;
}

const nsstring & NSResource::name() const
{
	return mName;
}

uint32 NSResource::plugid() const
{
	return mPlugID;
}

uint32 NSResource::id() const
{
	return mID;
}

const nsstring & NSResource::subDir() const
{
	return mSubDir;
}

const nsstring & NSResource::iconPath()
{
	return mIconPath;
}

const uivec2 & NSResource::iconTexID()
{
	return mIconTexID;
}

/*!
Get the other resources that this resource uses. If no other resources are used then leave this unimplemented - will return an empty map.
*/
uivec2array  NSResource::resources()
{
	return uivec2array();
}

/*!
This should be called if there was a name change to a resource - will check if the resource is used by this component and if is
is then it will update the handle
*/
void NSResource::nameChange(const uivec2 & oldid, const uivec2 newid)
{
	// do nothing
}

uint32 NSResource::type()
{
	return mHashedType;
}

void NSResource::setExtension(const nsstring & pExt)
{
	mExtension = pExt;
}

void NSResource::rename(const nsstring & pRefName)
{
	uint32 tmp = mID;

	mName = pRefName;
	mID = hash_id(pRefName);

#ifdef NOTIFY_ENGINE_NAME_CHANGE
	if (mOwned) // if a manager owns this resource - otherwise we dont care
		nsengine.nameChange(uivec2(mPlugID, tmp),uivec2(mPlugID, mID));
#endif
}

void NSResource::setSubDir(const nsstring & pDir)
{
	mSubDir = pDir;
}
