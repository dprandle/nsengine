/*! 
	\file nsresource.h
	
	\brief Header file for NSResource class

	This file contains all of the neccessary declarations for the NSResource class.

	\author Daniel Randle
	\date November 23 2013
	\copywrite Earth Banana Games 2013
*/
#ifndef NSRESOURCE_H
#define NSRESOURCE_H

#include <nsglobal.h>
#include <nsmath.h>
#include <nspupper.h>

class NSResource
{
public:
	friend class NSResManager;
	friend class NSResFactory;

	NSResource();
	virtual ~NSResource();

	virtual void init() = 0;

	const nsstring & extension() const;

	const nsstring & name() const;

	nsuint plugid() const;

	nsuint id() const;

	nsuint type();

	const nsstring & subDir() const;

	/*!
	Get the other resources that this resource uses. If no other resources are used then leave this unimplemented - will return an empty map.
	\return Map of resource ID to resource type containing all other used resources (doesn't include this resource)
	*/
	virtual uivec2array resources();

	const nsstring & iconPath();

	const uivec2 & iconTexID();

	/*!
	This should be called if there was a name change to a resource - will check if the resource is used by this component and if is
	is then it will update the handle
	*/
	virtual void nameChange(const uivec2 & oldid, const uivec2 newid);

	virtual void pup(NSFilePUPer * p) = 0;

	nsbool owned() { return mOwned; }

	void setExtension( const nsstring & pExt);

	void rename(const nsstring & pResName);

	uivec2 fullid();

	void setIconPath(const nsstring & pIconPath);

	void setIconTexID(const uivec2 & texID);

	void setSubDir(const nsstring & pDir);

protected:
	nsuint mHashedType;
	nsstring mIconPath;
	uivec2 mIconTexID;
	nsstring mName;
	nsstring mSubDir;
	nsstring mExtension;
	nsuint mID;
	nsuint mPlugID;
	nsbool mOwned;
};

#endif
