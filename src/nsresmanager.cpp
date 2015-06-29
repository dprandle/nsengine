/*!
\file nsresmanager.cpp

\brief Header file for NSResManager class

This file contains all of the neccessary definitions for the NSResManager class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#include <nsengine.h>
#include <nsglobal.h>
#include <nsresmanager.h>
#include <nspupper.h>
#include <nsfileos.h>
#include <filesystem>
#include <nsfactory.h>
using namespace nsfileio;

NSResManager::NSResManager():
mResourceDirectory(),
mLocalDirectory(),
mIDResourceMap(),
mPlugID(),
mSaveMode(Binary)
{}


NSResManager::~NSResManager()
{
	unload();
}

nsbool NSResManager::add(NSResource * res)
{
	if (res == NULL)
	{
		dprint("NSResManager::add Can not add NULL valued resource");
		return false;
	}

	auto check = mIDResourceMap.emplace(res->id(), res);
	if (check.second)
	{
		dprint("NSResManager::add Successfully added " + res->typeString() + " with name " + res->name());
		res->mPlugID = mPlugID;
		res->mOwned = true;
	}
	else
		dprint("NSResManager::add Could not add " + res->typeString() + " with name " + res->name() + " because " + res->typeString() + " with that name already exists");
	return check.second;
	
}

NSResManager::MapType::iterator NSResManager::begin()
{
	return mIDResourceMap.begin();
}

NSResource * NSResManager::create(const nsstring & resType, const nsstring & resName)
{
	// Create the resource and add it to the map - if there is a resource with the same name already
	// in the map then insertion will have failed, so delete the created resource and retun NULL
	NSResource * res = nsengine.factory<NSResFactory>(resType)->create();
	res->rename(resName);
	res->init();
	if (!add(res))
	{
		dprint("NSResManager::create Deleting unadded " + resType + " with name " + resName);
		delete res;
		return NULL;
	}
	return res;
}

bool NSResManager::contains(nsuint pResourceID)
{
	auto fIter = mIDResourceMap.find(pResourceID);
	return (fIter != mIDResourceMap.end());
}

bool NSResManager::contains(const nsstring & pResourceName)
{
	return contains(getHashedStringID(pResourceName));
}

nsuint NSResManager::count() const
{
	return mIDResourceMap.size();
}

/*!
Removes the resource with pResID from the resource ID map, as well deletes it. If the resource
ID cannot be found, will return false and do nothing.
*/
bool NSResManager::del(nsuint resID)
{
	NSResource * res = get(resID);
	if (res == NULL)
		return false;

	nsstring dir = mResourceDirectory + mLocalDirectory + res->subDir();
	nsstring fName = dir + res->name() + res->extension();
	bool ret = remove_file(fName);

	if (ret)
	{
		dprint("NSResManager::del - Succesfully deleted file with name: " + fName);
		unload(res);
	}
	else
	{
		dprint("NSResManager::del - Could not delete file with name: " + fName);
	}
	
	return (ret == 0);
}

bool NSResManager::del(const nsstring & name)
{
	return del(HashedStringID(name));
}

bool NSResManager::del(NSResource * res)
{
	return del(res->id());
}

NSResManager::MapType::iterator NSResManager::end()
{
	return mIDResourceMap.end();
}

bool NSResManager::empty()
{
	return mIDResourceMap.empty();
}

NSResource * NSResManager::get(NSResource * res)
{
	if (res == NULL)
		return NULL;
	NSResource * ret = get(res->id());
	return ret;
}

NSResource * NSResManager::get(nsuint resid)
{
	MapType::iterator iter = mIDResourceMap.find(resid);
	if (iter != mIDResourceMap.end())
		return iter->second;
	return NULL;
}

NSResource * NSResManager::get(const nsstring & resName)
{
	return get(getHashedStringID(resName));
}

NSResManager::SaveMode NSResManager::saveMode() const
{
	return mSaveMode;
}

void NSResManager::setSaveMode(SaveMode sm)
{
	mSaveMode = sm;
}

NSResource * NSResManager::load(const nsstring & resType, const nsstring & pFileName, bool pAppendDirectories)
{
	nsstring resName(pFileName);
	nsstring resExtension;
	nsstring fName(pFileName);
	nsstring subDir;
	

	nsuint pos = resName.find_last_of("/\\");

	if (pos != nsstring::npos && (pos + 1) < resName.size())
	{
		if (pAppendDirectories)
			subDir = resName.substr(0, pos + 1);
		resName = resName.substr(pos + 1);
	}

	nsuint extPos = resName.find_last_of(".");
	resExtension = resName.substr(extPos);
	resName = resName.substr(0, extPos);

	if (pAppendDirectories)
		fName = mResourceDirectory + mLocalDirectory + subDir + resName + resExtension;

	nsfstream file;
	NSFilePUPer * p;
	if (mSaveMode == Binary)
	{
		file.open(fName, nsfstream::in | nsfstream::binary);
		p = new NSBinFilePUPer(file, PUP_IN);
	}
	else
	{
		file.open(fName, nsfstream::in);
		p = new NSTextFilePUPer(file, PUP_IN);
	}

	if (!file.is_open())
	{
		dprint("NSResManager::load : Error opening " + resType + " file " + pFileName);
		delete p;
		return NULL;
	}

	NSResource * res = get(resName);
	if (res == NULL)
	{
		res = create(resType, resName);
	}
	else
	{
		file.close();
		delete p;
		return NULL;
	}

	res->setSubDir(subDir); // should be "" for false appendDirectories is false
	res->setExtension(resExtension);

	nsstring rt;

	if (mSaveMode == Binary)
		pup(*(static_cast<NSBinFilePUPer*>(p)), rt, "type");
	else
		pup(*(static_cast<NSTextFilePUPer*>(p)), rt, "type");

	if (rt != resType)
	{
		dprint("NSResManager::load Attempted to load resource type " + resType + " from file that is not of that type: " + fName);
		delete p;
		file.close();
		unload(res);
		return NULL;
	}


	res->pup(p);
	dprint("NSResManager::load - Succesfully loaded " + resType + " from file " + fName);
	delete p;
	file.close();
	return res;
}

nsuint NSResManager::getHashedStringID(const nsstring & pString)
{
	return HashedStringID(pString);
}

const nsstring & NSResManager::localDirectory()
{
	return mLocalDirectory;
}

const nsstring & NSResManager::resourceDirectory()
{
	return mResourceDirectory;
}

/*!
This should be called if there was a name change to a resource - will check if the resource is used by this component and if is
is then it will update the handle
*/
void NSResManager::nameChange(const uivec2 & oldid, const uivec2 newid)
{
	// first see if we need to remove and re-add
	if (oldid.x == mPlugID)
	{
		NSResource * res = get<NSResource>(oldid.y);
		if (res != NULL)
		{
			remove(oldid.y);
			add(res);
		}
	}

	MapType::iterator resIter = mIDResourceMap.begin();
	while (resIter != mIDResourceMap.end())
	{
		resIter->second->nameChange(oldid, newid);
		++resIter;
	}
}

nsuint NSResManager::plugid()
{
	return mPlugID;
}

NSResource * NSResManager::remove(const nsstring & name)
{
	return remove(HashedStringID(name));
}

NSResource * NSResManager::remove(nsuint id)
{
	NSResource * res = get(id);

	if (res != NULL)
	{
		mIDResourceMap.erase(id);
		res->mPlugID = 0;
		res->mOwned = false;
		dprint("NSResManager::remove - Succesfully removed " + res->typeString() + " with name " + res->name());
	}

	return res;
}

NSResource * NSResManager::remove(NSResource * res)
{
	return remove(res->id());
}
/*!
Renames the resource with pOldName to pNewName - it does this by removing the resource
from the resource id map and re-inserting it with the new name. If the old name cannot be found
in the resource id map, or the new name already exists, this function will return false without
renaming anything (leaving the state of the resource unchanged).
*/
bool NSResManager::rename(const nsstring & oldName, const nsstring & newName)
{
	NSResource * res = get<NSResource>(oldName);

	if (res == NULL) // resource not in map
		return false;

	res->rename(newName); // name change will propagate


	nsstring dir = mResourceDirectory + mLocalDirectory + res->subDir();
	nsstring fNameOld = dir + oldName + res->extension();
	nsstring fNameNew = dir + res->name() + res->extension();
	int ret = rename_file(fNameOld.c_str(), fNameNew.c_str());
			
	if (ret == 0)
		dprint("NSResManager::rename - Succesfully renamed file with old name: " + oldName + " to file with new name: " + newName);
	else
		dprint("NSResManager::rename - Could not rename file with old name: " + oldName + " to file with new name: " + newName);
	
	return (ret == 0);
}

bool NSResManager::save(bool pAppendDirectories)
{
	// Iterate through resources
	MapType::iterator iter = mIDResourceMap.begin();
	bool ret = true;

	while (iter != mIDResourceMap.end())
	{
		// Save the current iterated resource to file
		ret = save(iter->second->name(), true) || ret;
		++iter;
	}
	return ret;
}

bool NSResManager::save(nsuint resid)
{
	NSResource * res = get(resid);
	if (res == NULL)
		return false;

	return save(res->name(), true);
}

bool NSResManager::save(NSResource * res)
{
	return save(res->name(), true);
}

bool NSResManager::save(const nsstring & resName, bool pAppendDirectories)
{
	NSResource * res = NULL;

	if (!pAppendDirectories)
		res = get(nameFromFilename(resName));
	else
		res = get(resName);

	if (res == NULL)
	{
		dprint("NSResManager::save : Cannot save NULL valued resource");
		return false;
	}

	nsstring fName(resName);

	if (pAppendDirectories)
		fName = mResourceDirectory + mLocalDirectory + res->subDir() + res->name() + res->extension();

	bool fret = create_dir(fName);
	if (fret)
		dprint("NSResManager::save Created directory " + fName);

	nsfstream file;
	NSFilePUPer * p;
	if (mSaveMode == Binary)
	{
		file.open(fName, nsfstream::out | nsfstream::binary);
		p = new NSBinFilePUPer(file, PUP_OUT);
	}
	else
	{
		file.open(fName, nsfstream::out);
		p = new NSTextFilePUPer(file, PUP_OUT);
	}

	if (!file.is_open())
	{
		dprint("NSResManager::save : Error opening " + res->typeString() + " file " + fName);
		delete p;
		return false;
	}

	nsstring rest = res->typeString();

	if (mSaveMode == Binary)
		pup(*(static_cast<NSBinFilePUPer*>(p)), rest, "type");
	else
		pup(*(static_cast<NSTextFilePUPer*>(p)), rest, "type");

	res->pup(p);
	dprint("NSResManager::save - Succesfully saved " + res->typeString() + " to file " + fName);
	delete p;
	file.close();
	return true;
}

/*!
Set the local resource directory - ResourceDir/ *LocalDir* /SubDir/ResourceName.ResExt
*/
void NSResManager::setLocalDirectory(const nsstring & pDirectory)
{
	mLocalDirectory = pDirectory;
}

void NSResManager::setPlugID(nsuint plugid)
{
	mPlugID = plugid;
}

bool NSResManager::unload()
{
	MapType::iterator iter = mIDResourceMap.begin();
	while (iter != mIDResourceMap.end())
	{
		delete iter->second;
		iter->second = NULL;
		++iter;
	}
	mIDResourceMap.clear();
	return true;
}

bool NSResManager::unload(const nsstring & name)
{
	return unload(HashedStringID(name));
}

bool NSResManager::unload(nsuint resID)
{
	NSResource * res = remove(resID);
	if (res == NULL)
	{
		dprint("NSResManager::unload - Could not unload NULL valued resource");
		return false;
	}
	dprint("NSResManager::unload - Successfully deleted " + res->typeString() + " with name " + res->name());
	delete res;
	return true;
}

bool NSResManager::unload(NSResource * res)
{
	return unload(res->id());
}

/*!
Set the local resource directory - *ResourceDir* /LocalDir/SubDir/ResourceName.ResExt
Resource managers should really all have the same Resource directory.. but they can be
different if that is really needed for some reason
*/
void NSResManager::setResourceDirectory(const nsstring & pDirectory)
{
	mResourceDirectory = pDirectory;
}

/*!
Using the full path the resource name is extracted. This just finds the name after the last
forward slash or double backslash (/\\) and before the extension marker (.) and uses this
as the resource name.
*/
nsstring NSResManager::nameFromFilename(const nsstring & pFName)
{
	// full file path
	nsstring resName(pFName);

	// After last / or \\ (ie if bla/blabla/blablabla/name.ext this will get position before name.ext)
	nsuint pos = resName.find_last_of("/\\");

	// If position is valid, the extract name.ext. If it is not then means there was likely not a path in the first
	// place and so we leave resName alone.
	if (pos != nsstring::npos && (pos + 1) < pFName.size())
		resName = resName.substr(pos + 1);

	// Find the position before the period
	nsuint extPos = resName.find_last_of(".");

	// If valid position was found, extract name without characters after the period (name.ext) becomes (name)
	if (extPos != nsstring::npos)
		resName = resName.substr(0, extPos);

	return resName;
}