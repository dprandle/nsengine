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
#include <nsresmanager.h>
#include <nsresource.h>
class NSEntity;
class NSScene;

class NSPlugin : public NSResource
{
public:
	template <class PUPer>
	friend void pup(PUPer & p, NSPlugin & plug);

	typedef std::unordered_map<uint32, NSResManager*> ManagerMap;

	// This is manager type string to pair(get typestring, get name)
	// Sometimes managers can manage an abstract type - NSTexture for example - which
	// means the get type string must also be included so we can get the right factory
	typedef std::unordered_multimap<nsstring, std::pair<std::string, std::string>> ResTypeMap;

	enum Tile_t
	{
		Full,
		Half
	};

	NSPlugin();
	~NSPlugin();

	bool add(NSResource * res);

	void addNameToResPath(bool add_);

	bool addingNameToResPath();

	bool addManager(NSResManager * pManager);

	bool contains(NSResource * res);

	template<class ResType, class T>
	bool contains(const T & res)
	{	
		return contains(get<ResType>(res));
	}

	template<class ResType>
	ResType * create(const nsstring & resName)
	{
		uint32 hashed_type = type_to_hash(ResType);
		return static_cast<ResType*>(create(hashed_type, resName));
	}

	NSResource * create(uint32 res_typeid, const nsstring & resName);

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
		int32 shadowsamples = 2);

	NSEntity * createPointLight(const nsstring & name,
		float diffuse,
		float ambient,
		float distance,
		const fvec3 & color = fvec3(1, 1, 1),
		bool castshadows = true,
		float shadowdarkness = 1.0f,
		int32 shadowsamples = 2);

	NSEntity * createSpotLight(const nsstring & name,
		float diffuse,
		float ambient,
		float distance,
		float radius,
		const fvec3 & color = fvec3(1, 1, 1),
		bool castshadows = true,
		float shadowdarkness = 1.0f,
		int32 shadowsamples = 2);

	NSEntity * createTile(const nsstring & name,
		const nsstring & difftex,
		const nsstring & normtex,
		fvec3 m_col,
		float s_pwr,
		float s_int32,
		fvec3 s_col,
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
		uint32 matid,
		bool collides, 
		Tile_t type = Full);


	template<class ManagerType>
	ManagerType * createManager()
	{
		uint32 hashed_type = type_to_hash(ManagerType);
		return createManager(hashed_type);
	}

	NSResManager * createManager(uint32 manager_typeid);

	NSResManager * createManager(const nsstring & manager_guid);

	NSScene * currentScene();

	template<class ResType, class T>
	bool del(const T & name)
	{
		return del(get<ResType>(name));
	}

	bool del(NSResource * res);

	template<class ResManager>
	bool destroyManager()
	{
		uint32 hashed_type = type_to_hash(ResManager);
		return destroyManager(hashed_type);
	}

	bool destroyManager(uint32 manager_typeid);

	bool destroyManager(const nsstring & manager_guid);

	bool hasParent(const nsstring & pname);

	template<class ResType, class T>
	ResType * get(const T & name)
	{
		uint32 hashed_type = type_to_hash(ResType);
		return static_cast<ResType*>(get(hashed_type, name));
	}

	NSResource * get(uint32 res_typeid, uint32 resid);

	NSResource * get(uint32 res_typeid, const nsstring & resName);

	bool resourceChanged(NSResource * res);

	template<class ManagerType>
	ManagerType * manager()
	{
		uint32 hashed_type = type_to_hash(ManagerType);
		return static_cast<ManagerType*>(manager(hashed_type));
	}

	NSResManager * manager(const nsstring & manager_guid);
	
	NSResManager * manager(uint32 manager_typeid);

	template<class ManagerType>
	bool hasManager()
	{
		uint32 hashed_type = type_to_hash(ManagerType);
		return hasManager(hashed_type);
	}

	bool hasManager(uint32 manager_typeid);

	bool hasManager(const nsstring & manager_guid);

	void init();

	const nsstring & creator();

	nsstring details();

	const nsstring & notes();
	
	template<class ResType>
	ResType * load(const nsstring & fname)
	{
		uint32 hashed_type = type_to_hash(ResType);
		return static_cast<ResType*>(load(hashed_type, fname));
	}

	NSResource * load(uint32 res_typeid, const nsstring & fname);

	NSEntity * loadModel(const nsstring & entname,
						 nsstring fname,
						 bool prefixWithImportDir,
						 const nsstring & meshname = "",
						 bool pFlipUVs = true);

	bool loadModelResources(nsstring fname,
							bool prefixWithImportDir,
							const nsstring & meshname = "",
							bool flipuv = true);
	
	bool bind();

	bool bound();

	const nsstring & editDate();

	const nsstringset & parents();

	virtual void pup(NSFilePUPer * p);

	const nsstring & creationDate();

	template<class ResManager>
	ResManager * removeManager()
	{
		uint32 hashed_type = type_to_hash(ResManager);
		return static_cast<ResManager*>(removeManager(hashed_type));
	}

	NSResManager * removeManager(uint32 manager_typeid);

	NSResManager * removeManager(const nsstring & manager_guid);

	uint32 resourceCount();

	const nsstring & resourceDirectory();

	const nsstring & importDir() { return mImportDir; }

	/*!
	This should be called if there was a name change to a get - will check if the get is used by this component and if is
	is then it will update the handle
	*/
	virtual void nameChange(const uivec2 & oldid, const uivec2 newid);

	bool parentsLoaded();

	template<class ResType, class T>
	bool save(const T & name, const nsstring & path="")
	{
		return save(get<ResType>(name),path);
	}

	bool save(NSResource * res, const nsstring & path="");

	template<class ResType>
	void saveAll(const nsstring & path="", NSSaveResCallback * scallback = NULL)
	{
		uint32 hashed_type = type_to_hash(ResType);
		return saveAll(hashed_type, path, scallback);
	}

	void saveAll(const nsstring & path="", NSSaveResCallback * scallback = NULL);

	void saveAll(uint32 res_typeid, const nsstring & path, NSSaveResCallback * scallback);
	
	template<class ResType, class T>
	bool saveAs(const T & resname, const nsstring & fname)
	{
		NSResource * res = get<ResType>(resname);
		return saveAs(res, fname);
	}

	bool saveAs(NSResource * res, const nsstring & fname);

	void setCreator(const nsstring & pCreator);

	void setNotes(const nsstring & pNotes);

	void setEditDate(const nsstring & pEditDate);

	void setCreationDate(const nsstring & pCreationDate);

	void setResourceDirectory(const nsstring & dir);

	void setImportDir(const nsstring & dir) { mImportDir = dir; }

	template<class ResType, class T>
	ResType * remove(const T & resName)
	{
		NSResource * res = get<ResType>(resName);
		return static_cast<ResType*>(remove(res));
	}

	NSResource * remove(NSResource * res);
	
	template<class ResType, class T>
	bool destroy(const T & name)
	{
		return destroy(get<ResType>(name));
	}

	bool destroy(NSResource * res);

	bool unbind();

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

	bool mBound;
	ManagerMap mManagers;
	nsstringset mParents;
	ResTypeMap resmap;
	ResTypeMap unloaded;
	bool mAddname;
};

template <class PUPer>
void pup(PUPer & p, NSPlugin & plug)
{
	pup(p, plug.mParents, "parents");
	pup(p, plug.resmap, "resmap");
}

#endif
