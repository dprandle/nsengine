/*!
\file nsscenemanager.cpp

\brief Definition file for NSSceneManager class

This file contains all of the neccessary definitions for the NSSceneManager class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#include <nsscene_manager.h>
#include <nsscene.h>
#include <nsentity.h>
#include <nsengine.h>
#include <nsentity_manager.h>
#include <nsinput_comp.h>
#include <nsfile_os.h>
using namespace nsfileio;

NSSceneManager::NSSceneManager() : 
mCurrentScene(NULL)
{
	setLocalDirectory(LOCAL_SCENE_DIR_DEFAULT);
}

NSSceneManager::~NSSceneManager()
{}

NSScene * NSSceneManager::current()
{
	return mCurrentScene;
}

NSScene * NSSceneManager::load(uint32 res_type_id, const nsstring & fname)
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

	// check if the file exists.. if it doesnt then return without creating scene
	if (!file_exists(fName))
		return NULL;

	NSScene * scene = get(resName);
	if (scene == NULL)
		scene = create(resName);
	else
		return NULL;

	scene->setSubDir(subDir); // should be "" for false appendDirectories is false
	scene->setExtension(resExtension);
	return scene;
}

bool NSSceneManager::save(NSResource * res, const nsstring & path)
{
	NSScene * scene = get(res->id());
	if (scene == NULL)
		return false;

	nsstring fName(scene->name() + scene->extension());
	if (path == "")
		fName = mResourceDirectory + mLocalDirectory + res->subDir() + fName;
	else
		fName = path + fName;
	// otherwise create in cwd

	bool fret = create_dir(fName);
	if (fret)
		dprint("NSSceneManager::save Created directory " + fName);


    // If a scene other than the current scene is being saved, check first to make sure that there isnt a file
	// for the scene - if there is then do not save the scene because it will overwrite the scene file with
	// empty data.. if there is not a scene file then save it
	if (scene != mCurrentScene && file_exists(fName))
		return true;

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
		dprint("NSSceneManager::save : Error opening file with name - " + fName);
		delete p;
		return false;
	}
	nsstring rest = nsengine.guid(scene->type());
	if (mSaveMode == Binary)
		pup(*(static_cast<NSBinFilePUPer*>(p)), rest, "type");
	else
		pup(*(static_cast<NSTextFilePUPer*>(p)), rest, "type");
	
	scene->pup(p);
	delete p;
	file.close();
	return true;
}

bool NSSceneManager::setCurrent(NSScene * sc, bool newScene, bool savePrevious)
{
	if (sc != NULL && sc != mCurrentScene)
	{
		if (savePrevious && mCurrentScene != NULL)
			save(mCurrentScene);

		nsstring fName = mResourceDirectory + mLocalDirectory + sc->subDir() + sc->name() + sc->extension();
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

		// If file is not open that means scene has not yet been saved and therefor there is nothing to pup
		if (!file.is_open() && newScene)
		{
			if (mCurrentScene != NULL)
				mCurrentScene->clear();
			sc->clear();
			mCurrentScene = sc;
			delete p;
			return true;
		}

		nsstring rt;
		if (mSaveMode == Binary)
			pup(*(static_cast<NSBinFilePUPer*>(p)), rt, "type");
		else
			pup(*(static_cast<NSTextFilePUPer*>(p)), rt, "type");

		nsstring guid_ = nsengine.guid(sc->type());
		if (rt != guid_)
		{
			dprint("NSScene::setCurrent Loaded scene file: " + fName + " is not a scene file type - removing from scenes.");
			delete p;
			file.close();
			destroy(sc);
			return false;
		}
		if (mCurrentScene != NULL)
			mCurrentScene->clear();
		mCurrentScene = sc;
		sc->pup(p);
		delete p;
		file.close();
		return true;
	}
	else if (sc == NULL && sc != mCurrentScene)
	{
		if (savePrevious)
			save(mCurrentScene);
		mCurrentScene->clear();
		mCurrentScene = sc;
		return true;
	}
	else
	{
		return false;
	}
}
