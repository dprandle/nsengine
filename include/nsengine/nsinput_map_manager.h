#ifndef NSINPUTMAP_MANAGER_H
#define NSINPUTMAP_MANAGER_H

#include <nsres_manager.h>
#include <nsinput_map.h>

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
		return create<NSInputMap>(resName); // Create 2d texture by default
	}

	template <class ResType, class T>
	ResType * get(const T & rname)
	{
		return NSResManager::get<ResType>(rname);
	}
	
	template<class T>
	NSInputMap * get(const T & resname)
	{
		return get<NSInputMap>(resname);
	}

	template<class ResType>
	ResType * load(const nsstring & fname)
	{
		return NSResManager::load<ResType>(fname);
	}

	NSInputMap * load(const nsstring & fname)
	{
		return load<NSInputMap>(fname);
	}
	
	template<class ResType, class T >
	ResType * remove(const T & rname)
	{
		return NSResManager::remove<ResType>(rname);
	}

	template<class T >
	NSInputMap * remove(const T & rname)
	{
		return remove<NSInputMap>(rname);
	}

};

#endif
