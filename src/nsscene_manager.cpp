/*!
\file nsscenemanager.cpp

\brief Definition file for nsscene_manager class

This file contains all of the neccessary definitions for the nsscene_manager class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#include <nsscene_manager.h>
#include <nsscene.h>
#include <nsentity.h>
#include <nsengine.h>
#include <nsentity_manager.h>
#include <nsfile_os.h>
using namespace nsfile_os;

nsscene_manager::nsscene_manager() : 
mCurrentScene(NULL)
{
	set_local_dir(LOCAL_SCENE_DIR_DEFAULT);
}

nsscene_manager::~nsscene_manager()
{}

nsscene * nsscene_manager::current()
{
	return mCurrentScene;
}

nsscene * nsscene_manager::load(uint32 res_type_id, const nsstring & fname)
{
	nsstring resName(fname);
	nsstring resExtension;
	nsstring fName;
	nsstring subDir;
	bool shouldPrefix = false;
	
	nsstring prefixdirs = m_res_dir + m_local_dir;

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

	nsscene * scene = get(resName);
	if (scene == NULL)
		scene = create(resName);
	else
		return NULL;

	scene->set_subdir(subDir); // should be "" for false appendDirectories is false
	scene->set_ext(resExtension);
	return scene;
}

bool nsscene_manager::save(nsresource * res, const nsstring & path)
{
	nsscene * scene = get(res->id());
	if (scene == NULL)
		return false;

	nsstring fName(scene->name() + scene->extension());
	if (path == "")
		fName = m_res_dir + m_local_dir + res->subdir() + fName;
	else
		fName = path + fName;
	// otherwise create in cwd

	bool fret = create_dir(fName);
	if (fret)
		dprint("nsscene_manager::save Created directory " + fName);


    // If a scene other than the current scene is being saved, check first to make sure that there isnt a file
	// for the scene - if there is then do not save the scene because it will overwrite the scene file with
	// empty data.. if there is not a scene file then save it
	if (scene != mCurrentScene && file_exists(fName))
		return true;

	nsfstream file;
	nsfile_pupper * p;
	if (m_save_mode == binary)
	{
		file.open(fName, nsfstream::out | nsfstream::binary);
		p = new nsbinary_file_pupper(file, PUP_OUT);
	}
	else
	{
		file.open(fName, nsfstream::out);
		p = new nstext_file_pupper(file, PUP_OUT);
	}

	if (!file.is_open())
	{
		dprint("nsscene_manager::save : Error opening file with name - " + fName);
		delete p;
		return false;
	}
	nsstring rest = nse.guid(scene->type());
	if (m_save_mode == binary)
		pup(*(static_cast<nsbinary_file_pupper*>(p)), rest, "type");
	else
		pup(*(static_cast<nstext_file_pupper*>(p)), rest, "type");
	
	scene->pup(p);
	delete p;
	file.close();
	return true;
}

bool nsscene_manager::set_current(nsscene * sc, bool newScene, bool savePrevious)
{
	if (sc != NULL && sc != mCurrentScene)
	{
		if (savePrevious && mCurrentScene != NULL)
			save(mCurrentScene);

		nsstring fName = m_res_dir + m_local_dir + sc->subdir() + sc->name() + sc->extension();
		nsfstream file;
		nsfile_pupper * p;
		if (m_save_mode == binary)
		{
			file.open(fName, nsfstream::in | nsfstream::binary);
			p = new nsbinary_file_pupper(file, PUP_IN);
		}
		else
		{
			file.open(fName, nsfstream::in);
			p = new nstext_file_pupper(file, PUP_IN);
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
		if (m_save_mode == binary)
			pup(*(static_cast<nsbinary_file_pupper*>(p)), rt, "type");
		else
			pup(*(static_cast<nstext_file_pupper*>(p)), rt, "type");

		nsstring guid_ = nse.guid(sc->type());
		if (rt != guid_)
		{
			dprint("nsscene::setCurrent Loaded scene file: " + fName + " is not a scene file type - removing from scenes.");
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
