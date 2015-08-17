/*!
\file nspluginmanager.cpp

\brief Definition file for NSPluginManager class

This file contains all of the neccessary definitions for the NSPluginManager class.

\author Daniel Randle
\date August 23 2014
\copywrite Earth Banana Games 2013
*/

#include <nsplugin_manager.h>
#include <nsplugin.h>
#include <nsmesh_manager.h>
#include <nstex_manager.h>
#include <nsanim_manager.h>
#include <nsengine.h>
#include <nsmat_manager.h>
#include <nsscene_manager.h>
#include <nsentity_manager.h>

NSPluginManager::NSPluginManager() :
NSResManager()
{
	setSaveMode(Text);
}

NSPluginManager::~NSPluginManager()
{}

bool NSPluginManager::add(NSResource * res)
{
	if (NSResManager::add(res))
	{
		NSPlugin * plug = static_cast<NSPlugin*>(res);
		plug->setResourceDirectory(mResourceDirForOwnedPlugs);
		plug->setImportDir(nsengine.importdir());
		return true;
	}
	return false;
}

bool NSPluginManager::bind(NSPlugin * plg)
{
	return plg->bind();
}

void NSPluginManager::setResourceDirectory(const nsstring & pDirectory)
{
	mResourceDirForOwnedPlugs = pDirectory;
	auto iter = mIDResourceMap.begin();
	while (iter != mIDResourceMap.end())
	{
		NSPlugin * plug = get(iter->first);
		plug->setResourceDirectory(mResourceDirForOwnedPlugs);
		++iter;
	}
}

void NSPluginManager::setPluginDirectory(const nsstring & dir)
{
	mResourceDirectory = dir;
}

const nsstring & NSPluginManager::pluginDirectory()
{
	return mResourceDirectory;
}

const nsstring & NSPluginManager::resourceDirectory()
{
	return mResourceDirForOwnedPlugs;
}

NSPlugin * NSPluginManager::active()
{
	return get(mActivePlugin);
}

void NSPluginManager::setActive(NSPlugin * plg)
{
	if (plg != NULL)
	{
		if (!plg->bound())
			plg->bind();
		mActivePlugin = plg->id();
	}
	else
		mActivePlugin = 0;
}

bool NSPluginManager::unbind(NSPlugin * plg)
{
	return plg->unbind();
}
