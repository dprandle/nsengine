/*!
\file nspluginmanager.h

\brief Header file for NSPluginManager class

This file contains all of the neccessary declarations for the NSPluginManager class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#ifndef NSPLUGINMANAGER_H
#define NSPLUGINMANAGER_H

#include <nsresmanager.h>
#include <nsglobal.h>
#include <nsplugin.h>

class NSPluginManager : public NSResManager
{
public:
	using NSResManager::save;
	NSPluginManager();
	~NSPluginManager();

	virtual bool add(NSResource * res);
	
	template <class ResType>
	ResType * create(const nsstring & resName)
	{
		return NSResManager::create<ResType>(resName);
	}

	virtual NSPlugin * create(const nsstring & resName)
	{
		return create<NSPlugin>(resName); // Create 2d texture by default
	}

	template <class ResType, class T>
	ResType * get(const T & rname)
	{
		return NSResManager::get<ResType>(rname);
	}
	
	template<class T>
	NSPlugin * get(const T & resname)
	{
		return get<NSPlugin>(resname);
	}

	template<class ResType>
	ResType * load(const nsstring & fname)
	{
		return NSResManager::load<ResType>(fname);
	}

	NSPlugin * load(const nsstring & fname)
	{
		return load<NSPlugin>(fname);
	}
	
	template<class ResType, class T >
	ResType * remove(const T & rname)
	{
		return NSResManager::remove<ResType>(rname);
	}

	template<class T >
	NSPlugin * remove(const T & rname)
	{
		return remove<NSPlugin>(rname);
	}

	NSPlugin * active();
	
	template<class T>
	bool bind(const T & name)
	{
		NSPlugin * plug = get(name);
		return bind(plug);
	}

	bool bind(NSPlugin * plg);
		
	void setPluginDirectory(const nsstring & dir);

	const nsstring & pluginDirectory();

	void setResourceDirectory(const nsstring & pDirectory);

	const nsstring & resourceDirectory();

	template<class T>
	bool unbind(const T & name)
	{
		NSPlugin * plg = get(name);
		return unbind(plg);
	}

	bool unbind(NSPlugin * plg);

	template<class T>
	void setActive(const T & name)
	{
		NSPlugin * plg = get(name);
		setActive(plg);
	}

	void setActive(NSPlugin * plg);
	
private:
	uint32 mActivePlugin;
	nsstring mResourceDirForOwnedPlugs;
};

#endif
