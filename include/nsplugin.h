/*!
\file nsplugin.h

\brief Header file for NSPlugin class

This file contains all of the neccessary declarations for the NSPlugin class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#ifndef NSPLUGIN_H
#define NSPLUGIN_H

#include <nsglobal.h>
#include <nsresource.h>
#include <nsresmanager.h>
class NSEntity;
class NSScene;

class NSPlugin : public NSResource
{
public:
	template <class PUPer>
	friend void pup(PUPer & p, NSPlugin & plug);

	typedef std::unordered_map<nsstring, NSResManager*> ManagerMap;
	typedef std::unordered_multimap<nsstring, std::pair<std::string, std::string>> ResTypeMap;

	enum Tile_t
	{
		Full,
		Half
	};

	NSPlugin();
	~NSPlugin();

	nsbool add(NSResource * res);

	void addNameToResPath(nsbool add_);

	nsbool addingNameToResPath();

	nsbool addManager(NSResManager * pManager);

	bool contains(NSResource * res);

	template<class ResType, class T>
	bool contains(const T & res)
	{
		return manager(ResType::getManagerTypeString())->contains(res);
	}

	template<class ResType>
	ResType * create(const nsstring & resName)
	{
		return manager(ResType::getManagerTypeString())->create<ResType>(resName);
	}

	NSEntity * createCamera(const nsstring & name,
		float fov,
		const uivec2 & screenDim,
		const fvec2 & clipnf);

	NSEntity * createCamera(const nsstring & name, 
		const fvec2 & lrclip, 
		const fvec2 & tbclip, 
		const fvec2 & nfclip);

	NSEntity * createTerrain(const nsstring & name,
		float hmin,
		float hmax, 
		const nsstring & hmfile, 
		const nsstring & dmfile = "", 
		const nsstring & nmfile = "",
		bool importdir=true);

	NSEntity * createDirLight(const nsstring & name,
		float diffuse,
		float ambient,
		const fvec3 & color = fvec3(1, 1, 1),
		bool castshadows = true,
		float shadowdarkness = 1.0f,
		int shadowsamples = 2);

	NSEntity * createPointLight(const nsstring & name,
		float diffuse,
		float ambient,
		float distance,
		const fvec3 & color = fvec3(1, 1, 1),
		bool castshadows = true,
		float shadowdarkness = 1.0f,
		int shadowsamples = 2);

	NSEntity * createSpotLight(const nsstring & name,
		float diffuse,
		float ambient,
		float distance,
		float radius,
		const fvec3 & color = fvec3(1, 1, 1),
		bool castshadows = true,
		float shadowdarkness = 1.0f,
		int shadowsamples = 2);

	NSEntity * createTile(const nsstring & name,
		const nsstring & difftex,
		const nsstring & normtex,
		fvec3 m_col,
		float s_pwr,
		float s_int,
		fvec3 s_col,
		bool appenddirs,
		bool collides,
		Tile_t type = Full);

	NSEntity * createTile(const nsstring & name,
		NSMaterial * mat,
		bool collides,
		Tile_t type=Full);

	NSEntity * createTile(const nsstring & name,
		const nsstring & matname,
		bool collides, 
		Tile_t type = Full);

	NSEntity * createTile(const nsstring & name,
		nsuint matid,
		bool collides, 
		Tile_t type = Full);

	template<class ManagerType>
	ManagerType * createManager()
	{
		ManagerType * manager = new ManagerType();
		if (!addManager(manager))
		{
			delete manager;
			return NULL;
		}
		return manager;
	}

	NSResManager * createManager(const nsstring & managertype);

	NSScene * currentScene();

	template<class ResType, class T>
	nsbool del(const T & name)
	{
		return del(get<ResType>(name));
	}

	nsbool del(NSResource * res);

	template<class ResManager>
	bool delManager()
	{
		return delManager(ResManager::getTypeString());
	}

	bool delManager(const nsstring & managerType);

	bool hasParent(const nsstring & pname);

	template<class ResType, class T>
	ResType * resource(const T & name)
	{
		return manager(ResType::getManagerTypeString())->get<ResType>(name);
	}

	template<class T>
	NSResource * resource(const nsstring & managerType, const T & name)
	{
		return manager(managerType)->get<NSResource>(name);
	}

	template<class ManagerType>
	ManagerType * manager()
	{
		auto fIter = mManagers.find(ManagerType::getTypeString());
		if (fIter == mManagers.end())
			return NULL;
		return (ManagerType*)fIter->second;
	}

	NSResManager * manager(const nsstring & pManagerType);

	template<class ManagerType>
	nsbool hasManager()
	{
		auto fIter = mManagers.find(ManagerType::getTypeString());
		return (fIter != mManagers.end());
	}

	nsbool hasManager(const nsstring & pResType);

	void init();

	const nsstring & creator();

	nsstring details();

	const nsstring & notes();

	NSResource * load(const nsstring & managerType, const nsstring & restype, const nsstring & fileName, bool appendDirs = true);

	template<class ResType>
	ResType * load(const nsstring & fileName, bool appendDirs = true)
	{
		NSResource * res = load(ResType::getManagerTypeString(), ResType::getTypeString(), fileName, appendDirs);
		if (res != NULL)
			return (ResType*)res;
		return NULL;
	}

	NSEntity * loadModel(const nsstring & entname, nsstring fname, bool prefixWithImportDir, const nsstring & meshname = "", bool pFlipUVs = true);

	bool loadModelResources(nsstring fname, bool prefixWithImportDir, const nsstring & meshname = "", bool flipuv = true);
	
	nsbool bind();

	nsbool bound();

	const nsstring & editDate();

	const nsstringset & parents();

	virtual void pup(NSFilePUPer * p);

	const nsstring & creationDate();

	template<class ResManager>
	NSResManager * removeManager()
	{
		return removeManager(ResManager::getTypeString());
	}

	NSResManager * removeManager(const nsstring & managerType);

	nsuint resourceCount();

	const nsstring & resourceDirectory();

	const nsstring & importDir() { return mImportDir; }

	/*!
	This should be called if there was a name change to a resource - will check if the resource is used by this component and if is
	is then it will update the handle
	*/
	virtual void nameChange(const uivec2 & oldid, const uivec2 newid);

	bool parentsLoaded();

	template<class ResType, class T>
	nsbool save(const T & name)
	{
		return save(resource<ResType>(name));
	}

	nsbool save(NSResource * res);

	template<class ResType>
	nsbool save()
	{
		NSResManager * rm = manager(ResType::getManagerTypeString());
		return rm->save();
	}

	nsbool save();

	void setCreator(const nsstring & pCreator);

	void setNotes(const nsstring & pNotes);

	void setEditDate(const nsstring & pEditDate);

	void setCreationDate(const nsstring & pCreationDate);

	void setResourceDirectory(const nsstring & dir);

	void setImportDir(const nsstring & dir) { mImportDir = dir; }

	template<class ResType, class T>
	ResType * remove(const T & resName)
	{
		NSResManager * rm = manager(ResType::getManagerTypeString());
		return rm->remove<ResType>(resName);
	}

	NSResource * remove(NSResource * res);

	template<class ResType, class T>
	nsbool unload(const T & name)
	{
		return unload(resource<ResType>(name));
	}

	bool unload(NSResource * res);

	bool unbind();

	virtual nsstring typeString() { return getTypeString(); }

	nsstring managerTypeString() { return getManagerTypeString(); }

	static nsstring getTypeString() { return PLUGIN_TYPESTRING; }

	static nsstring getManagerTypeString() { return PLUGIN_MANAGER_TYPESTRING; }

private:
	void _updateParents();
	void _updateResMap();
	void _clear();

	nsstring mResDir;
	nsstring mImportDir;
	nsstring mNotes;
	nsstring mCreator;
	nsstring mCreationDate;
	nsstring mEditDate;

	nsbool mBound;
	ManagerMap mManagers;
	nsstringset mParents;
	ResTypeMap resmap;
	ResTypeMap unloaded;
	nsbool mAddname;
};

template <class PUPer>
void pup(PUPer & p, NSPlugin & plug)
{
	pup(p, plug.mParents, "parents");
	pup(p, plug.resmap, "resmap");
}

#endif