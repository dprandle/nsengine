#ifndef NSINPUTMAP_MANAGER_H
#define NSINPUTMAP_MANAGER_H

#include <nsresmanager.h>
#include <nsinputmap.h>

class NSInputMapManager : public NSResManager
{
public:

	NSInputMapManager();
	~NSInputMapManager();

	template <class ResType>
	ResType * create(const nsstring & resName)
	{
		return NSResManager::create<ResType>(resName);
	}

	virtual NSInputMap * create(const nsstring & resName)
	{
		return NSResManager::create<NSInputMap>(resName);
	}

	virtual NSInputMap * create(const nsstring & resType, const nsstring & resName)
	{
		return static_cast<NSInputMap*>(NSResManager::create(resType, resName));
	}

	template <class ResType, class T>
	ResType * get(const T & rname)
	{
		return NSResManager::get<ResType>(rname);
	}

	virtual NSInputMap * get(nsuint resid)
	{
		return static_cast<NSInputMap*>(NSResManager::get(resid));
	}

	virtual NSInputMap * get(const nsstring & resName)
	{
		return static_cast<NSInputMap*>(NSResManager::get(resName));
	}

	template<class ResType>
	ResType * load(const nsstring & pFileName, bool pAppendDirectories = true)
	{
		return NSResManager::load<ResType>(pFileName, pAppendDirectories);
	}

	virtual NSInputMap * load(const nsstring & resType, const nsstring & pFileName, bool pAppendDirectories = true)
	{
		return static_cast<NSInputMap*>(NSResManager::load(resType, pFileName, pAppendDirectories));
	}

	template<class ResType, class T >
	ResType * remove(const T & rname)
	{
		return NSResManager::remove<ResType>(rname);
	}

	virtual NSInputMap * remove(const nsstring & name)
	{
		return static_cast<NSInputMap*>(NSResManager::remove(name));
	}

	virtual NSInputMap * remove(nsuint id)
	{
		return static_cast<NSInputMap*>(NSResManager::remove(id));
	}

	virtual NSInputMap * remove(NSResource * res)
	{
		return static_cast<NSInputMap*>(NSResManager::remove(res));
	}
	
	virtual nsstring typeString() {return getTypeString();}

	static nsstring getTypeString() {return INPUTMAP_MANAGER_TYPESTRING;}
};

#endif
