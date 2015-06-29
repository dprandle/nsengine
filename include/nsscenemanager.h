/*!
\file nsscenemanager.h

\brief Header file for NSSceneManager class

This file contains all of the neccessary declarations for the NSSceneManager class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#ifndef NSSCENEMANAGER_H
#define NSSCENEMANAGER_H

#include <nsscene.h>
#include <nsresmanager.h>

class NSSceneManager : public NSResManager
{
public:
	NSSceneManager();
	~NSSceneManager();

	template <class ResType>
	ResType * create(const nsstring & resName)
	{
		return NSResManager::create<ResType>(resName);
	}

	virtual NSScene * create(const nsstring & resName)
	{
		return NSResManager::create<NSScene>(resName);
	}

	virtual NSScene * create(const nsstring & resType, const nsstring & resName)
	{
		return static_cast<NSScene*>(NSResManager::create(resType, resName));
	}

	NSScene * current();

	template <class ResType, class T>
	ResType * get(const T & rname)
	{
		return NSResManager::get<ResType>(rname);
	}

	virtual NSScene * get(NSScene * scn)
	{
		return NSResManager::get<NSScene>(scn);
	}

	virtual NSScene * get(nsuint resid)
	{
		return static_cast<NSScene*>(NSResManager::get(resid));
	}

	virtual NSScene * get(const nsstring & resName)
	{
		return static_cast<NSScene*>(NSResManager::get(resName));
	}

	template<class ResType>
	ResType * load(const nsstring & pFileName, bool pAppendDirectories = true)
	{
		return NSResManager::load<ResType>(pFileName, pAppendDirectories);
	}

	virtual NSScene * load(const nsstring & resType, const nsstring & pFileName, bool pAppendDirectories = true);

	template<class ResType, class T >
	ResType * remove(const T & rname)
	{
		return NSResManager::remove<ResType>(rname);
	}

	virtual NSScene * remove(const nsstring & name)
	{
		return static_cast<NSScene*>(NSResManager::remove(name));
	}

	virtual NSScene * remove(nsuint id)
	{
		return static_cast<NSScene*>(NSResManager::remove(id));
	}

	virtual NSScene * remove(NSResource * res)
	{
		return static_cast<NSScene*>(NSResManager::remove(res));
	}

	template <class T>
	bool save(const T & rname)
	{
		return NSResManager::save(rname);
	}

	virtual bool save(const nsstring & resName, bool pAppendDirectories = true);

	template<class T>
	bool setCurrent(const T & scene, bool newScene = false, bool pSavePreviousScene = false)
	{
		NSScene * sc = get(scene);
		if (sc != NULL && sc != mCurrentScene)
		{
			if (pSavePreviousScene && mCurrentScene != NULL)
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

			if (rt != sc->typeString())
			{
				dprint("NSScene::setCurrent Loaded scene file: " + fName + " is not a scene file type - removing from scenes.");
				delete p;
				file.close();
				unload(sc);
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
			if (pSavePreviousScene)
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

	virtual nsstring typeString() { return getTypeString(); }

	static nsstring getTypeString();

private:
	NSScene * mCurrentScene;
};

#endif