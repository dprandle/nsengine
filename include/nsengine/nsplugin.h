/*!
\file nsplugin.h

\brief Header file for nsplugin class

This file contains all of the neccessary declarations for the nsplugin class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#ifndef NSPLUGIN_H
#define NSPLUGIN_H

#include <nsglobal.h>
#include <nsres_manager.h>
#include <nsresource.h>
class nsentity;
class nsscene;

class nsplugin : public nsresource
{
public:
	template <class PUPer>
	friend void pup(PUPer & p, nsplugin & plug);

	typedef std::unordered_map<uint32, nsres_manager*> manager_map;

	// This is manager type string to pair(get typestring, get name)
	// Sometimes managers can manage an abstract type - nstexture for example - which
	// means the get type string must also be included so we can get the right factory
	typedef std::unordered_multimap<nsstring, std::pair<std::string, std::string>> res_type_map;

	enum tile_t
	{
		tile_full,
		tile_half
	};

	nsplugin();
	~nsplugin();

	bool add(nsresource * res);

	void add_name_to_res_path(bool add_);

	bool adding_names_to_res_path();

	bool add_manager(nsres_manager * pManager);

	bool contains(nsresource * res);

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

	nsresource * create(uint32 res_typeid, const nsstring & resName);

	nsentity * create_camera(const nsstring & name,
		float fov,
		const uivec2 & screenDim,
		const fvec2 & clipnf);

	nsentity * create_camera(const nsstring & name, 
		const fvec2 & lrclip, 
		const fvec2 & tbclip, 
		const fvec2 & nfclip);

	nsentity * create_terrain(const nsstring & name,
		float hmin,
		float hmax, 
		const nsstring & hmfile, 
		const nsstring & dmfile = "", 
		const nsstring & nmfile = "",
		bool importdir=true);

	nsentity * create_dir_light(const nsstring & name,
		float diffuse,
		float ambient,
		const fvec3 & color = fvec3(1, 1, 1),
		bool castshadows = true,
		float shadowdarkness = 1.0f,
		int32 shadowsamples = 2);

	nsentity * create_point_light(const nsstring & name,
		float diffuse,
		float ambient,
		float distance,
		const fvec3 & color = fvec3(1, 1, 1),
		bool castshadows = true,
		float shadowdarkness = 1.0f,
		int32 shadowsamples = 2);

	nsentity * create_spot_light(const nsstring & name,
		float diffuse,
		float ambient,
		float distance,
		float radius,
		const fvec3 & color = fvec3(1, 1, 1),
		bool castshadows = true,
		float shadowdarkness = 1.0f,
		int32 shadowsamples = 2);

	nsentity * create_tile(const nsstring & name,
		const nsstring & difftex,
		const nsstring & normtex,
		fvec3 m_col,
		float s_pwr,
		float s_int32,
		fvec3 s_col,
		bool collides,
		tile_t type = tile_full);

	nsentity * create_tile(const nsstring & name,
		nsmaterial * mat,
		bool collides,
		tile_t type=tile_full);

	nsentity * create_tile(const nsstring & name,
		const nsstring & matname,
		bool collides, 
		tile_t type = tile_full);

	nsentity * create_tile(const nsstring & name,
		uint32 matid,
		bool collides, 
		tile_t type = tile_full);


	template<class ManagerType>
	ManagerType * create_manager()
	{
		uint32 hashed_type = type_to_hash(ManagerType);
		return create_manager(hashed_type);
	}

	nsres_manager * create_manager(uint32 manager_typeid);

	nsres_manager * create_manager(const nsstring & manager_guid);

	nsscene * current_scene();

	template<class ResType, class T>
	bool del(const T & name)
	{
		return del(get<ResType>(name));
	}

	bool del(nsresource * res);

	template<class ResManager>
	bool destroy_manager()
	{
		uint32 hashed_type = type_to_hash(ResManager);
		return destroy_manager(hashed_type);
	}

	bool destroy_manager(uint32 manager_typeid);

	bool destroy_manager(const nsstring & manager_guid);

	bool has_parent(const nsstring & pname);

	template<class ResType, class T>
	ResType * get(const T & name)
	{
		uint32 hashed_type = type_to_hash(ResType);
		return static_cast<ResType*>(get(hashed_type, name));
	}

	nsresource * get(uint32 res_typeid, uint32 resid);

	nsresource * get(uint32 res_typeid, const nsstring & resName);

	bool resource_changed(nsresource * res);

	template<class ManagerType>
	ManagerType * manager()
	{
		uint32 hashed_type = type_to_hash(ManagerType);
		return static_cast<ManagerType*>(manager(hashed_type));
	}

	nsres_manager * manager(const nsstring & manager_guid);
	
	nsres_manager * manager(uint32 manager_typeid);

	template<class ManagerType>
	bool has_manager()
	{
		uint32 hashed_type = type_to_hash(ManagerType);
		return has_manager(hashed_type);
	}

	bool has_manager(uint32 manager_typeid);

	bool has_manager(const nsstring & manager_guid);

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

	nsresource * load(uint32 res_typeid, const nsstring & fname);

	nsentity * load_model(const nsstring & entname,
						 nsstring fname,
						 bool prefixWithImportDir,
						 const nsstring & meshname = "",
						 bool pFlipUVs = true);

	bool load_model_resources(nsstring fname,
							bool prefixWithImportDir,
							const nsstring & meshname = "",
							bool flipuv = true);
	
	bool bind();

	bool bound();

	const nsstring & edit_date();

	const nsstringset & parents();

	virtual void pup(NSFilePUPer * p);

	const nsstring & creation_date();

	template<class ResManager>
	ResManager * remove_manager()
	{
		uint32 hashed_type = type_to_hash(ResManager);
		return static_cast<ResManager*>(remove_manager(hashed_type));
	}

	nsres_manager * remove_manager(uint32 manager_typeid);

	nsres_manager * remove_manager(const nsstring & manager_guid);

	uint32 resource_count();

	const nsstring & res_dir();

	const nsstring & import_dir() { return m_import_dir; }

	/*!
	This should be called if there was a name change to a get - will check if the get is used by this component and if is
	is then it will update the handle
	*/
	virtual void name_change(const uivec2 & oldid, const uivec2 newid);

	bool parents_loaded();

	template<class ResType, class T>
	bool save(const T & name, const nsstring & path="")
	{
		return save(get<ResType>(name),path);
	}

	bool save(nsresource * res, const nsstring & path="");

	template<class ResType>
	void save_all(const nsstring & path="", NSSaveResCallback * scallback = NULL)
	{
		uint32 hashed_type = type_to_hash(ResType);
		return save_all(hashed_type, path, scallback);
	}

	void save_all(const nsstring & path="", NSSaveResCallback * scallback = NULL);

	void save_all(uint32 res_typeid, const nsstring & path, NSSaveResCallback * scallback);
	
	template<class ResType, class T>
	bool save_as(const T & resname, const nsstring & fname)
	{
		nsresource * res = get<ResType>(resname);
		return save_as(res, fname);
	}

	bool save_as(nsresource * res, const nsstring & fname);

	void set_creator(const nsstring & pCreator);

	void set_notes(const nsstring & pNotes);

	void set_edit_date(const nsstring & pEditDate);

	void set_creation_date(const nsstring & pCreationDate);

	void set_res_dir(const nsstring & dir);

	void set_import_dir(const nsstring & dir) { m_import_dir = dir; }

	template<class ResType, class T>
	ResType * remove(const T & resName)
	{
		nsresource * res = get<ResType>(resName);
		return static_cast<ResType*>(remove(res));
	}

	nsresource * remove(nsresource * res);
	
	template<class ResType, class T>
	bool destroy(const T & name)
	{
		return destroy(get<ResType>(name));
	}

	bool destroy(nsresource * res);

	bool unbind();

private:
	void _update_parents();
	void _update_res_map();
	void _clear();
	
	nsstring m_res_dir;
	nsstring m_import_dir;
	nsstring m_notes;
	nsstring m_creator;
	nsstring m_creation_date;
	nsstring m_edit_date;

	bool m_bound;
	manager_map m_managers;
	nsstringset m_parents;
	res_type_map m_resmap;
	res_type_map m_unloaded;
	bool m_add_name;
};

template <class PUPer>
void pup(PUPer & p, nsplugin & plug)
{
	pup(p, plug.m_parents, "parents");
	pup(p, plug.m_resmap, "resmap");
}

#endif
