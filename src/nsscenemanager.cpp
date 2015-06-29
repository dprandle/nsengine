/*!
\file nsscenemanager.cpp

\brief Definition file for NSSceneManager class

This file contains all of the neccessary definitions for the NSSceneManager class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#include <nsscenemanager.h>
#include <nsscene.h>
#include <nsentity.h>
#include <nsengine.h>
#include <nsentitymanager.h>
#include <nsinputcomp.h>
#include <nsfileos.h>
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

NSScene * NSSceneManager::load(const nsstring & resType, const nsstring & pFileName, bool pAppendDirectories)
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

	// check if the file exists.. if it doesnt then return without creating scene
	if (!file_exists(fName))
		return NULL;

	NSScene * scene = get(resName);
	if (scene == NULL)
		scene = create(resType, resName);
	else
		return NULL;

	scene->setSubDir(subDir); // should be "" for false appendDirectories is false
	scene->setExtension(resExtension);
	return scene;
}

bool NSSceneManager::save(const nsstring & resName, bool pAppendDirectories)
{
	NSScene * scene;

	if (!pAppendDirectories)
		scene = get(nameFromFilename(resName));
	else
		scene = get(resName);

	if (scene == NULL)
		return false;

	nsstring fName(resName);

	if (pAppendDirectories)
		fName = mResourceDirectory + mLocalDirectory + scene->subDir() + scene->name() + scene->extension();

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
	nsstring rest = scene->typeString();

	if (mSaveMode == Binary)
		pup(*(static_cast<NSBinFilePUPer*>(p)), rest, "type");
	else
		pup(*(static_cast<NSTextFilePUPer*>(p)), rest, "type");
	scene->pup(p);
	delete p;
	file.close();
	return true;
}

nsstring NSSceneManager::getTypeString()
{
	return SCENE_MANAGER_TYPESTRING;
}