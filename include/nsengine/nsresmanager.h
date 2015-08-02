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

#include <nsengine.h>
#include <nsglobal.h>
#include <unordered_map>
#include <nsresource.h>

class NSResManager
{
public:

	NSResManager();

	virtual ~NSResManager();
	
	friend class NSResManagerFactory;
	
	enum SaveMode
	{
		Binary,
		Text
	};

	typedef std::unordered_map<nsuint, NSResource*> MapType;

	virtual nsbool add(NSResource * res);

	MapType::iterator begin();

	template<class T>
	nsbool changed(const T & resource, const nsstring & fname="")
	{
		NSResource * res = get(resource);
		return changed(res,fname);
	}
	
	template<class T>
	bool contains(const T & res_name)
	{
		return contains(get(res_name));
	}

	template <class ResType>
	ResType * create(const nsstring & resName)
	{
		nsuint res_type_id = type_to_hash(ResType);
		return static_cast<ResType*>(create(res_type_id, resName));
	}

	template<class T>
	bool del(const T & res_name)
	{
		NSResource * res = get(res_name);
		return del(res);
	}

	template <class ResType, class T>
	ResType * get(const T & rname)
	{
		return static_cast<ResType*>(get(rname));
	}

	template<class ResType>
	ResType * load(const nsstring & fname)
	{
		nsuint res_type_id = type_to_hash(ResType);
		return static_cast<ResType*>(load(res_type_id, fname));
	}
	
	template<class ResType, class T >
	ResType * remove(const T & rname)
	{
		NSResource * res = get(rname);
		return static_cast<ResType*>(remove(res));
	}

	template<class T>
	bool save(const T & res_name, nsstring path="")
	{
		NSResource * res = get(res_name);
		return save(res, path);
	}

	template<class T>
	bool saveAs(const T & resname, const nsstring & fname)
	{
		NSResource * res = get(resname);
		return saveAs(res, fname);
	}

	virtual nsbool changed(NSResource * res, nsstring fname);

	virtual bool contains(NSResource * res);

	nsuint count() const;

	virtual NSResource * create(const nsstring & resName)=0;

	virtual NSResource * create(nsuint res_type_id, const nsstring & resName);

	NSResource * create(const nsstring & guid_, const nsstring & resName);

	virtual bool del(NSResource * res);

	MapType::iterator end();

	bool empty();

	nsuint type();

	virtual NSResource * get(nsuint resid);

	virtual NSResource * get(const nsstring & resName);

	virtual NSResource * get(NSResource * res);

	virtual const nsstring & resourceDirectory();

	const nsstring & localDirectory();

	nsuint plugid();

	virtual NSResource * load(const nsstring & fname) = 0;
	
	virtual NSResource * load(nsuint res_type_id, const nsstring & fname);
	
	NSResource * load(const nsstring & res_guid, const nsstring & fname);
	
	virtual void nameChange(const uivec2 & oldid, const uivec2 newid);

	virtual NSResource * remove(nsuint res_type_id);

	virtual NSResource * remove(const nsstring & resName);
	
	virtual NSResource * remove(NSResource * res);

	virtual bool rename(const nsstring & oldName, const nsstring & newName);
	
	virtual bool save(NSResource * res,const nsstring & path);

	virtual void saveAll(const nsstring & path="", NSSaveResCallback * scallback = NULL);

	virtual bool saveAs(NSResource * res, const nsstring & fname);

	void setPlugID(nsuint plugid);
	
	virtual void setResourceDirectory(const nsstring & pDirectory);

	void setLocalDirectory(const nsstring & pDirectory);

	SaveMode saveMode() const;

	void setSaveMode(SaveMode sm);

	virtual void destroyAll();

	template<class T>
	bool destroy(const T & resname)
	{
		NSResource * res = get(resname);
		return destroy(res);
	}
	
	virtual bool destroy(NSResource * res);

	static nsstring nameFromFilename(const nsstring & fname);

	static nsstring pathFromFilename(const nsstring & fname);
	
protected:
	nsuint mHashedType;
	nsstring mResourceDirectory;
	nsstring mLocalDirectory;
	MapType mIDResourceMap;
	nsuint mPlugID;
	SaveMode mSaveMode;
};

#endif
