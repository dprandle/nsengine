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

#include <asset/nsasset_manager.h>
#include <asset/nsasset.h>
#include <nsset.h>

#define DEFAULT_ADD_NAME_TO_RES_DIR true

class nsinput_map;
class nsentity;
class nsmap_area;
class nsmaterial;
class nsanim_set;
class nsmesh;

class nsplugin : public nsasset
{
public:
	template <class PUPer>
	friend void pup(PUPer & p, nsplugin & plug);

	typedef std::unordered_map<uint32, nsasset_manager*> manager_map;

	struct res_info
	{
		res_info(const nsstring & res_guid="",
				 const nsstring & subdir_and_name="",
				 const nsstring & icon_path="");
		nsstring m_res_guid;
		nsstring m_res_subdir_and_name;
		nsstring m_icon_path;
	};
	
	// This is manager type string to pair(get typestring, get name)
	// Sometimes managers can manage an abstract type - nstexture for example - which
	// means the get type string must also be included so we can get the right factory
	// manager guid 
	typedef std::unordered_multimap<nsstring, res_info> res_type_map;

	nsplugin();
	nsplugin(const nsplugin & copy_);

	~nsplugin();

	nsplugin & operator=(nsplugin rhs);

	bool add(nsasset * res);

	bool add_manager(nsasset_manager * pManager);

	bool contains(nsasset * res);

	template<class ResType, class T>
	bool contains(const T & res)
	{	
		return contains(get<ResType>(res));
	}

	template<class ResType>
	ResType * create(const nsstring & resName, nsasset * to_copy=nullptr)
	{
		uint32 hashed_type = type_to_hash(ResType);
		return static_cast<ResType*>(create(hashed_type, resName, to_copy));
	}

	nsasset * create(uint32 res_typeid, const nsstring & resName, nsasset * to_copy);

	nsinput_map * create_global_input_map(const nsstring & imap_name, const nsstring & global_ctxt_name);

	template<class ManagerType>
	ManagerType * create_manager()
	{
		uint32 hashed_type = type_to_hash(ManagerType);
		return create_manager(hashed_type);
	}

	nsasset_manager * create_manager(uint32 manager_typeid);

	nsasset_manager * create_manager(const nsstring & manager_guid);

	template<class ResType, class T>
	bool del(const T & name)
	{
		return del(get<ResType>(name));
	}

	bool del(nsasset * res);

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

	nsasset * get(uint32 res_typeid, uint32 resid);

	nsasset * get(uint32 res_typeid, nsasset * res);

	nsasset * get(uint32 res_typeid, const nsstring & resName);

	bool resource_changed(nsasset * res);

	template<class ManagerType>
	ManagerType * manager()
	{
		uint32 hashed_type = type_to_hash(ManagerType);
		return static_cast<ManagerType*>(manager(hashed_type));
	}

	nsasset_manager * manager(const nsstring & manager_guid);
	
	nsasset_manager * manager(uint32 manager_typeid);

	template<class ManagerType>
	bool has_manager()
	{
		uint32 hashed_type = type_to_hash(ManagerType);
		return has_manager(hashed_type);
	}

	bool has_manager(uint32 manager_typeid);

	bool has_manager(const nsstring & manager_guid);

	void init();

	void release();

	const nsstring & creator();

	nsstring details();

	const nsstring & notes();
	
	template<class ResType>
	ResType * load(const nsstring & fname, bool finalize_)
	{
		uint32 hashed_type = type_to_hash(ResType);
		return static_cast<ResType*>(load(hashed_type, fname, finalize_));
	}

	nsasset * load(uint32 res_typeid, const nsstring & fname, bool finalize_);

	bool load_model_assets(
		nsstring fname,
		const nsstring & meshname = "",
		bool flipuv = false);

	nsmesh * load_model_mesh(
		nsstring fname,
		const nsstring & meshname = "",
		bool flipuv = false);

	bool load_model_mats(
		nsstring fname,
		const nsstring & meshname = "",
		bool flipuv = false);

	nsanim_set * load_model_anim(
		nsstring fname,
		const nsstring & meshname = "",
		bool flipuv = false);

	void enable(bool enable_);

	bool is_enabled();

	const nsstring & edit_date();

	const nsstring_set & parents();

	virtual void pup(nsfile_pupper * p);

	const nsstring & creation_date();

	template<class ResManager>
	ResManager * remove_manager()
	{
		uint32 hashed_type = type_to_hash(ResManager);
		return static_cast<ResManager*>(remove_manager(hashed_type));
	}

	nsasset_manager * remove_manager(uint32 manager_typeid);

	nsasset_manager * remove_manager(const nsstring & manager_guid);

	uint32 resource_count();

	virtual void name_change(const uivec2 & oldid, const uivec2 newid);

	bool parents_enabled();

	void save_all_in_plugin(const nsstring & path="", nssave_resouces_callback * scallback = NULL);

	template<class ResType, class T>
	bool save(const T & name, const nsstring & path="")
	{
		return save(get<ResType>(name),path);
	}

	bool save(nsasset * res, const nsstring & path="");

	template<class ResType>
	void save_all(const nsstring & path="", nssave_resouces_callback * scallback = NULL)
	{
		uint32 hashed_type = type_to_hash(ResType);
		return save_all(hashed_type, path, scallback);
	}

	void save_all(uint32 res_typeid, const nsstring & path, nssave_resouces_callback * scallback);
	
	template<class ResType, class T>
	bool save_as(const T & resname, const nsstring & fname)
	{
		nsasset * res = get<ResType>(resname);
		return save_as(res, fname);
	}

	bool save_as(nsasset * res, const nsstring & fname);

	void set_creator(const nsstring & pCreator);

	void set_notes(const nsstring & pNotes);

	void set_edit_date(const nsstring & pEditDate);

	void set_creation_date(const nsstring & pCreationDate);

	void set_managers_res_dir(const nsstring & dir);

	template<class ResType, class T>
	ResType * remove(const T & resName)
	{
		nsasset * res = get<ResType>(resName);
		return static_cast<ResType*>(remove(res));
	}

	nsasset * remove(nsasset * res);
	
	template<class ResType, class T>
	bool destroy(const T & name)
	{
		return destroy(get<ResType>(name));
	}

	bool destroy(nsasset * res);
	
	void destroy_all();

  private:
	
	void _update_parents();
	void _update_res_map();
	
	nsstring m_notes;
	nsstring m_creator;
	nsstring m_creation_date;
	nsstring m_edit_date;

	bool m_enabled;
	manager_map m_managers;
	nsstring_set m_parents;
	res_type_map m_resmap;
	res_type_map m_unloaded;
	bool m_add_name;
};

template <class PUPer>
void pup(PUPer & p, nsplugin::res_info & res_info, const nsstring & var_name)
{
	pup(p, res_info.m_res_guid, var_name + "guid");
	pup(p, res_info.m_res_subdir_and_name, var_name + "res_name");
	pup(p, res_info.m_icon_path, var_name + "icon_path");
}

template <class PUPer>
void pup(PUPer & p, nsplugin & plug)
{
	pup(p, plug.m_parents, "parents");
	pup(p, plug.m_resmap, "resmap");
}









// Move to different file when get a chance


enum tile_t
{
	tile_full,
	tile_half
};


nsentity * create_entity_from_model(
	nsplugin * assets,
	nstform_ent_chunk * chnk,
	const nsstring & entname,
	nsstring fname,
	const nsstring & meshname = "",
	bool occupy_comp = true, 
	bool pFlipUVs = false);

nsentity * create_sprite(
	nsplugin * assets,
	nstform_ent_chunk * chnk,
	const nsstring & sprite_name,
	const nsstring & tex_filename,
	bool match_tex_dims,
	bool alpha_enabled);

nsentity * create_camera(
	nstform_ent_chunk * chnk,
	const nsstring & name,
	float fov,
	const uivec2 & screenDim,
	const fvec2 & clipnf);

nsentity * create_camera(
	nstform_ent_chunk * chnk,
	const nsstring & name, 
	const fvec2 & lrclip, 
	const fvec2 & tbclip, 
	const fvec2 & nfclip);

nsentity * create_terrain(
	nsplugin * assets,
	nstform_ent_chunk * chnk,
	const nsstring & name,
	float hmin,
	float hmax, 
	const nsstring & hmfile, 
	const nsstring & dmfile = "", 
	const nsstring & nmfile = "");

nsentity * create_dir_light(
	nstform_ent_chunk * chnk,
	const nsstring & name,
	float diffuse,
	float ambient,
	const fvec3 & color = fvec3(1, 1, 1),
	bool castshadows = true,
	float shadowdarkness = 1.0f,
	int32 shadowsamples = 2);

nsentity * create_point_light(
	nstform_ent_chunk * chnk,
	const nsstring & name,
	float diffuse,
	float ambient,
	float distance,
	const fvec3 & color = fvec3(1, 1, 1),
	bool castshadows = true,
	float shadowdarkness = 1.0f,
	int32 shadowsamples = 2);

nsentity * create_spot_light(
	nstform_ent_chunk * chnk,
	const nsstring & name,
	float diffuse,
	float ambient,
	float distance,
	float radius,
	const fvec3 & color = fvec3(1, 1, 1),
	bool castshadows = true,
	float shadowdarkness = 1.0f,
	int32 shadowsamples = 2);

nsentity * create_tile(
	nsplugin * assets,
	nstform_ent_chunk * chnk,
	const nsstring & name,
	fvec4 m_col,
	float s_pwr,
	float s_int32,
	fvec3 s_col,
	bool collides,
	tile_t type = tile_full);

nsentity * create_tile(
	nsplugin * assets,
	nstform_ent_chunk * chnk,
	const nsstring & name,
	const nsstring & difftex,
	const nsstring & normtex,
	fvec4 m_col,
	float s_pwr,
	float s_int32,
	fvec3 s_col,
	bool collides,
	tile_t type = tile_full);

nsentity * create_tile(
	nsplugin * assets,
	nstform_ent_chunk * chnk,
	const nsstring & name,
	nsmaterial * mat,
	bool collides,
	tile_t type=tile_full);

nsentity * create_tile(
	nsplugin * assets,
	nstform_ent_chunk * chnk,
	const nsstring & name,
	const nsstring & matname,
	bool collides, 
	tile_t type = tile_full);

nsentity * create_tile(
	nsplugin * assets,
	nstform_ent_chunk * chnk,
	const nsstring & name,
	uint32 matid,
	bool collides, 
	tile_t type = tile_full);

nsentity * create_skydome(
	nsplugin * assets,
	nstform_ent_chunk * chnk,
	const nsstring & name,
	nsstring cubemap_relative_fname,
	const nsstring & image_ext,
	const nsstring & tex_subdir="");





#endif
