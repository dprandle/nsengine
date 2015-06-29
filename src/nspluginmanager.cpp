/*!
\file nspluginmanager.cpp

\brief Definition file for NSPluginManager class

This file contains all of the neccessary definitions for the NSPluginManager class.

\author Daniel Randle
\date August 23 2014
\copywrite Earth Banana Games 2013
*/

#include <nspluginmanager.h>
#include <nsplugin.h>
#include <nsmeshmanager.h>
#include <nstexmanager.h>
#include <nsanimmanager.h>
#include <nsengine.h>
#include <nsmatmanager.h>
#include <nsscenemanager.h>
#include <nsentitymanager.h>

NSPluginManager::NSPluginManager() :
NSResManager()
{
	setSaveMode(Text);
}

NSPluginManager::~NSPluginManager()
{}

nsbool NSPluginManager::add(NSResource * res)
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

void NSPluginManager::setResourceDirectory(const nsstring & pDirectory)
{
	mResourceDirForOwnedPlugs = pDirectory;
	auto iter = mIDResourceMap.begin();
	while (iter != mIDResourceMap.end())
	{
		NSPlugin * plug = get(iter->second);
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

bool NSPluginManager::save(const nsstring & resName, bool pAppendDirectories)
{
	NSPlugin * plug = NULL;
	if (!pAppendDirectories)
		plug = get(nameFromFilename(resName));
	else
		plug = get(resName);
	if (plug == NULL)
		return false;
	plug->save();
	return NSResManager::save(resName, pAppendDirectories);
}

NSPlugin * NSPluginManager::active()
{
	return get(mActivePlugin);
}

nsstring NSPluginManager::getTypeString()
{
	return PLUGIN_MANAGER_TYPESTRING;
}