/*!
\file nsresmanager.cpp

\brief Header file for NSResManager class

This file contains all of the neccessary definitions for the NSResManager class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#include <nsengine.h>
#include <nsresmanager.h>
#include <nspupper.h>
#include <nsfileos.h>
#include <nsfactory.h>
#include <nscallback.h>
#include <hash/sha256.h>

using namespace nsfileio;

NSResManager::NSResManager():
mResourceDirectory(),
mLocalDirectory(),
mIDResourceMap(),
mPlugID(),
mHashedType(0),
mSaveMode(Binary)
{}


NSResManager::~NSResManager()
{
	destroyAll();
}

bool NSResManager::add(NSResource * res)
{
	if (res == NULL)
	{
		dprint("NSResManager::add Can not add NULL valued resource");
		return false;
	}

	auto check = mIDResourceMap.emplace(res->id(), res);
	if (check.second)
	{
		dprint("NSResManager::add Successfully added resource with name " + res->name());
		res->mPlugID = mPlugID;
		res->mOwned = true;
	}
	else
	{
		dprint("NSResManager::add Could not add resource with name " + res->name() + " because resource with that name already exists");
	}
	return check.second;
	
}

NSResManager::MapType::iterator NSResManager::begin()
{
	return mIDResourceMap.begin();
}

bool NSResManager::changed(NSResource * res, nsstring fname)
{
	if (fname == "")
		fname = mResourceDirectory + mLocalDirectory + res->subDir() + res->name() + res->extension();

	saveAs(res, ".tmp");

	nschararray v1, v2;
	nsfileio::read(".tmp",&v1);
	nsfileio::read(fname, &v2);

	if (v1.empty() || v2.empty())
		return true;

	SHA256 sha256;
	std::string s1 = sha256(&v1[0], v1.size());
	std::string s2 = sha256(&v2[0], v2.size());
	nsfileio::remove_file(".tmp");
	return (s1 != s2);
}

uint32 NSResManager::type()
{
	return mHashedType;
}

NSResource * NSResManager::create(const nsstring & guid_, const nsstring & resName)
{
	return create(hash_id(guid_), resName);
}

NSResource * NSResManager::create(uint32 res_type_id, const nsstring & resName)
{
	// Create the resource and add it to the map - if there is a resource with the same name already
	// in the map then insertion will have failed, so delete the created resource and retun NULL
	NSResource * res = nsengine.factory<NSResFactory>(res_type_id)->create();
	res->rename(resName);
	if (!add(res))
	{
		dprint("NSResManager::create Deleting unadded " + nsengine.guid(res_type_id) + " with name " + resName);
		delete res;
		return NULL;
	}
	res->init();
	return res;
}

bool NSResManager::contains(NSResource * res)
{
	if (res == NULL)
		return false;
	return (get(res->id()) != NULL);
}

uint32 NSResManager::count() const
{
	return static_cast<uint32>(mIDResourceMap.size());
}

bool NSResManager::del(NSResource * res)
{
	nsstring dir = mResourceDirectory + mLocalDirectory + res->subDir();
	nsstring fName = dir + res->name() + res->extension();
	bool ret = remove_file(fName);

	if (ret)
	{
		dprint("NSResManager::del - Succesfully deleted file with name: " + fName);
		destroy(res);
	}
	else
	{
		dprint("NSResManager::del - Could not delete file with name: " + fName);
	}
	
	return (ret == 0);
}

NSResManager::MapType::iterator NSResManager::end()
{
	return mIDResourceMap.end();
}

bool NSResManager::empty()
{
	return mIDResourceMap.empty();
}

NSResource * NSResManager::get(uint32 resid)
{
	MapType::iterator iter = mIDResourceMap.find(resid);
	if (iter != mIDResourceMap.end())
		return iter->second;
	return NULL;
}

NSResource * NSResManager::get(const nsstring & resName)
{
	return get(hash_id(resName));
}

NSResource * NSResManager::get(NSResource * res)
{
	return get(res->id());
}

NSResManager::SaveMode NSResManager::saveMode() const
{
	return mSaveMode;
}

void NSResManager::setSaveMode(SaveMode sm)
{
	mSaveMode = sm;
}

NSResource * NSResManager::load(const nsstring & res_guid,
								const nsstring & fname)
{
	return load(hash_id(res_guid), fname);
}

NSResource * NSResManager::load(uint32 res_type_id, const nsstring & fname)
{
	nsstring resName(fname);
	nsstring resExtension;
	nsstring fName;
	nsstring subDir;
	bool shouldPrefix = false;
	
	nsstring prefixdirs = mResourceDirectory + mLocalDirectory;

	size_t pos = resName.find_last_of("/\\");
	if (pos != nsstring::npos)
	{
		if (resName[0] != '/' && resName[0] != '.' && resName.find(":") == nsstring::npos) // then subdir
		{
			subDir = resName.substr(0, pos + 1);
			shouldPrefix = true;
		}
		resName = resName.substr(pos + 1);
	}
	else
		shouldPrefix = true;

	size_t extPos = resName.find_last_of(".");
	resExtension = resName.substr(extPos);
	resName = resName.substr(0, extPos);

	if (shouldPrefix)
		fName = prefixdirs + subDir + resName + resExtension;
	else
		fName = fname;

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
		dprint("NSResManager::load : Error opening resource file " + fName);
		delete p;
		return NULL;
	}

	NSResource * res = get(resName);
	if (res == NULL)
	{
		res = create(res_type_id, resName);
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

	if (rt != nsengine.guid(res_type_id))
	{
		dprint("NSResManager::load Attempted to load resource type " + nsengine.guid(res_type_id) + " from file that is not of that type: " + fName);
		delete p;
		file.close();
		destroy(res);
		return NULL;
	}


	res->pup(p);
	dprint("NSResManager::load - Succesfully loaded resource from file " + fName);
	delete p;
	file.close();
	return res;
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

uint32 NSResManager::plugid()
{
	return mPlugID;
}

NSResource * NSResManager::remove(uint32 res_type_id)
{
	return remove((get(res_type_id)));
}

NSResource * NSResManager::remove(const nsstring & resName)
{
	return remove((get(resName)));
}

NSResource * NSResManager::remove(NSResource * res)
{
	if (res != NULL)
	{
		mIDResourceMap.erase(res->mID);
		res->mPlugID = 0;
		res->mOwned = false;
		dprint("NSResManager::remove - Succesfully removed resource " + res->name());
	}
	return res;
}

bool NSResManager::rename(const nsstring & oldName, const nsstring & newName)
{
	NSResource * res = get(oldName);

	if (res == NULL) // resource not in map
		return false;

	res->rename(newName); // name change will propagate


	nsstring dir = mResourceDirectory + mLocalDirectory + res->subDir();
	nsstring fNameOld = dir + oldName + res->extension();
	nsstring fNameNew = dir + res->name() + res->extension();
	int32 ret = rename_file(fNameOld.c_str(), fNameNew.c_str());
			
	if (ret == 0)
	{
		dprint("NSResManager::rename - Succesfully renamed file with old name: " + oldName + " to file with new name: " + newName);
	}
	else
	{
		dprint("NSResManager::rename - Could not rename file with old name: " + oldName + " to file with new name: " + newName);
	}
	
	return (ret == 0);
}

void NSResManager::saveAll(const nsstring & path, NSSaveResCallback * scallback)
{
	// Iterate through resources
	MapType::iterator iter = mIDResourceMap.begin();
	while (iter != mIDResourceMap.end())
	{
		// Save the current iterated resource to file
		bool success = save(iter->second, path);
		if (scallback != NULL)
		{
			scallback->saved = success;
			scallback->resid = iter->second->fullid();
			scallback->run();
		}
		++iter;
	}
}

bool NSResManager::save(NSResource * res,const nsstring & path)
{
	if (res == NULL)
	{
		dprint("NSResManager::save : Cannot save NULL valued resource");
		return false;
	}

	nsstring fName(res->name() + res->extension());

	if (path == "")
		fName = mResourceDirectory + mLocalDirectory + res->subDir() + fName;
	else
		fName = path + fName;
	// otherwise create in cwd

	bool fret = create_dir(fName);
	if (fret)
	{
		dprint("NSResManager::save Created directory " + fName);
	}

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
		dprint("NSResManager::save : Error opening file " + fName);
		delete p;
		return false;
	}

	uint32 hashed_type = res->type();
	nsstring rest = nsengine.guid(hashed_type);

	if (mSaveMode == Binary)
		pup(*(static_cast<NSBinFilePUPer*>(p)), rest, "type");
	else
		pup(*(static_cast<NSTextFilePUPer*>(p)), rest, "type");

	res->pup(p);
	dprint("NSResManager::save - Succesfully saved " + rest + " to file " + fName);
	delete p;
	file.close();
	return true;
}

bool NSResManager::saveAs(NSResource * res, const nsstring & fname)
{
	nsstring origName = res->name();
	res->rename(nameFromFilename(fname));
	bool success = save(res, pathFromFilename(fname));
	res->rename(origName);
	return success;
}

void NSResManager::setLocalDirectory(const nsstring & pDirectory)
{
	mLocalDirectory = pDirectory;
}

void NSResManager::setPlugID(uint32 plugid)
{
	mPlugID = plugid;
}

void NSResManager::destroyAll()
{
	while (begin() != end())
	{
		if (!destroy(begin()->second))
			return;
	}
	mIDResourceMap.clear();
}

bool NSResManager::destroy(NSResource * res)
{
	NSResource * res_ = remove(res);
	if (res_ == NULL)
	{
		dprint("NSResManager::destroy - Couldn't destroy " + res_->name());
		return false;
	}
	delete res_;
	return true;
}

void NSResManager::setResourceDirectory(const nsstring & pDirectory)
{
	mResourceDirectory = pDirectory;
}

nsstring NSResManager::nameFromFilename(const nsstring & fname)
{
	// full file path
	nsstring resName(fname);

	// After last / or \\ (ie if bla/blabla/blablabla/name.ext this will get position before name.ext)
	size_t pos = resName.find_last_of("/\\");

	// If position is valid, the extract name.ext. If it is not then means there was likely not a path in the first
	// place and so we leave resName alone.
	if (pos != nsstring::npos && (pos + 1) < fname.size())
		resName = resName.substr(pos + 1);

	// Find the position before the period
	size_t extPos = resName.find_last_of(".");

	// If valid position was found, extract name without characters after the period (name.ext) becomes (name)
	if (extPos != nsstring::npos)
		resName = resName.substr(0, extPos);

	return resName;
}

nsstring NSResManager::pathFromFilename(const nsstring & fname)
{
	nsstring path = "";

	size_t pos = fname.find_last_of("/\\");
	if (pos != nsstring::npos && (pos + 1) < fname.size())
		path = fname.substr(0, pos + 1);

	return path;
}
