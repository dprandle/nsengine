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

	NSPlugin * active();

	nsbool add(NSResource * res);

	template<class T>
	nsbool bind(const T & name)
	{
		NSPlugin * plug = get(name);
		if (plug == NULL)
			return false;
		return plug->bind();
	}

	template <class ResType>
	ResType * create(const nsstring & resName)
	{
		return NSResManager::create<ResType>(resName);
	}

	virtual NSPlugin * create(const nsstring & resName)
	{
		return NSResManager::create<NSPlugin>(resName);
	}

	virtual NSPlugin * create(const nsstring & resType, const nsstring & resName)
	{
		return static_cast<NSPlugin*>(NSResManager::create(resType, resName));
	}

	template <class ResType, class T>
	ResType * get(const T & rname)
	{
		return NSResManager::get<ResType>(rname);
	}

	template <class T>
	NSPlugin * get(const T & rname)
	{
		return NSResManager::get<NSPlugin>(rname);
	}

	virtual NSPlugin * get(nsuint resid)
	{
		return static_cast<NSPlugin*>(NSResManager::get(resid));
	}

	virtual NSPlugin * get(const nsstring & resName)
	{
		return static_cast<NSPlugin*>(NSResManager::get(resName));
	}

	template<class ResType>
	ResType * load(const nsstring & pFileName, bool pAppendDirectories = true)
	{
		return NSResManager::load<ResType>(pFileName, pAppendDirectories);
	}

	virtual NSPlugin * load(const nsstring & resType, const nsstring & pFileName, bool pAppendDirectories = true)
	{
		return static_cast<NSPlugin*>(NSResManager::load(resType, pFileName, pAppendDirectories));
	}

	template<class ResType, class T >
	ResType * remove(const T & rname)
	{
		return NSResManager::remove<ResType>(rname);
	}

	virtual NSPlugin * remove(const nsstring & name)
	{
		return static_cast<NSPlugin*>(NSResManager::remove(name));
	}

	virtual NSPlugin * remove(nsuint id)
	{
		return static_cast<NSPlugin*>(NSResManager::remove(id));
	}

	virtual NSPlugin * remove(NSResource * res)
	{
		return static_cast<NSPlugin*>(NSResManager::remove(res));
	}

	virtual bool save(const nsstring & resName, bool pAppendDirectories = true);

	void setPluginDirectory(const nsstring & dir);

	const nsstring & pluginDirectory();

	void setResourceDirectory(const nsstring & pDirectory);

	const nsstring & resourceDirectory();

	template<class T>
	nsbool unbind(const T & name)
	{
		NSPlugin * plug = get(name);
		if (plug == NULL)
			return false;
		return plug->unbind();
	}

	/*!
	\brief Set the active plugin for which new resources will be added to
	Set the active plugin. The active plugin is used when adding resources and
	saving resources with the engine. If the plugin is not bound, calling this
	function will bing the plugin.
	\param name Either name, id, or pointer of/to plugin
	*/
	template<class T>
	void setActive(const T & name)
	{
		NSPlugin * plug = get(name);
		if (plug != NULL)
		{
			if (!plug->bound())
				plug->bind();
			mActivePlugin = plug->id();
		}
		else
			mActivePlugin = 0;
	}

	virtual nsstring typeString() { return getTypeString(); }

	static nsstring getTypeString();

private:
	nsuint mActivePlugin;
	nsstring mResourceDirForOwnedPlugs;
};

#endif