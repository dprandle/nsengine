/*! 
	\file nsresmanager.h
	
	\brief Header file for NSResManager class

	This file contains all of the neccessary declarations for the NSResManager class.

	\author Daniel Randle
	\date November 23 2013
	\copywrite Earth Banana Games 2013
*/

#ifndef NSRESOURCEMANAGER_H
#define NSRESOURCEMANAGER_H

#include <nsglobal.h>
#include <nsresource.h>
#include <unordered_map>

class NSResManager
{
public:
	enum SaveMode
	{
		Binary,
		Text
	};

	typedef std::unordered_map<nsuint, NSResource*> MapType;

	NSResManager();

	virtual ~NSResManager();

	virtual nsbool add(NSResource * res);

	MapType::iterator begin();

	bool contains(nsuint pResourceID);

	bool contains(const nsstring & pResourceName);

	nsuint count() const;

	template <class ResType>
	ResType * create(const nsstring & resName)
	{
		return static_cast<ResType*>(create(ResType::getTypeString(), resName));
	}

	virtual NSResource * create(const nsstring & resType, const nsstring & resName);

	bool del(const nsstring & name);

	virtual bool del(nsuint resID);

	bool del(NSResource * res);

	MapType::iterator end();

	bool empty();

	template <class ResType, class T>
	ResType * get(const T & rname)
	{
		return static_cast<ResType*>(get(rname));
	}

	virtual NSResource * get(NSResource * res);

	virtual NSResource * get(nsuint resid);

	virtual NSResource * get(const nsstring & resName);

	virtual const nsstring & resourceDirectory();

	const nsstring & localDirectory();

	nsuint plugid();

	template<class ResType>
	ResType * load(const nsstring & pFileName, bool pAppendDirectories = true)
	{
		return static_cast<ResType*>(load(ResType::getTypeString(), pFileName, pAppendDirectories));
	}

	virtual NSResource * load(const nsstring & resType, const nsstring & pFileName, bool pAppendDirectories = true);

	/*!
	This should be called if there was a name change to a resource - will check if the resource is used by this component and if is
	is then it will update the handle
	*/
	virtual void nameChange(const uivec2 & oldid, const uivec2 newid);

	template<class ResType, class T >
	ResType * remove(const T & rname)
	{
		NSResource * res = remove(rname);
		if (res != NULL)
			return static_cast<ResType*>(res);
		return NULL;
	}

	virtual NSResource * remove(const nsstring & name);

	virtual NSResource * remove(nsuint id);

	virtual NSResource * remove(NSResource * res);

	/*
	Rename the resource on file
	*/
	virtual bool rename(const nsstring & oldName, const nsstring & newName);

	virtual bool save(bool pAppendDirectories=true);

	virtual bool save(const nsstring & resName, bool pAppendDirectories=true);

	bool save(nsuint resid);

	bool save(NSResource * res);

	void setPlugID(nsuint plugid);
	
	virtual void setResourceDirectory(const nsstring & pDirectory);

	void setLocalDirectory(const nsstring & pDirectory);

	SaveMode saveMode() const;

	void setSaveMode(SaveMode sm);

	bool unload();

	bool unload(const nsstring & name);

	virtual bool unload(nsuint resID);

	bool unload(NSResource * res);

	virtual nsstring typeString() = 0;

	static nsuint getHashedStringID(const nsstring & pString);

	static nsstring nameFromFilename(const nsstring & pFName);

protected:
	nsstring mResourceDirectory;
	nsstring mLocalDirectory;
	MapType mIDResourceMap;
	nsuint mPlugID;
	SaveMode mSaveMode;
};

#endif