/*! 
	\file nsasset_manager.h
	
	\brief Header file for nsasset_manager class

	This file contains all of the neccessary declarations for the nsasset_manager class.

	\author Daniel Randle
	\date November 23 2013
	\copywrite Earth Banana Games 2013
*/

#ifndef NSASSET_MANAGER_H
#define NSASSET_MANAGER_H

#define LOCAL_ANIMATION_DIR_DEFAULT "anims/"
#define LOCAL_MESH_DIR_DEFAULT "meshes/"
#define LOCAL_SHADER_DIR_DEFAULT "shaders/"
#define LOCAL_TEXTURE_DIR_DEFAULT "textures/"
#define LOCAL_MATERIAL_DIR_DEFAULT "materials/"
#define LOCAL_ENTITY_DIR_DEFAULT "entities/"
#define LOCAL_INPUT_DIR_DEFAULT "input/"
#define LOCAL_SCENE_DIR_DEFAULT "maps/"
#define LOCAL_PLUGIN_DIR_DEFAULT "plugins/"
#define LOCAL_FONT_DIR_DEFAULT "fonts/"
#define LOCAL_AUDIO_DIR_DEFAULT "audio/"

#define MANAGER_TEMPLATES using nsasset_manager::create; \
	using nsasset_manager::get;						   \
	using nsasset_manager::load;					   \
	using nsasset_manager::remove;

#include "nsengine.h"

class nsasset;

class nsasset_manager
{
public:

	nsasset_manager(uint32 hashed_type);

	virtual ~nsasset_manager();

	virtual bool add(nsasset * res);

	virtual nsasset * create(uint32 res_type_id,
							 const nsstring & res_name,
							 nsasset * to_copy=nullptr);

	virtual bool del(nsasset * res);

	virtual nsasset * load(uint32 res_type_id, const nsstring & fname, bool finalize);
	
	virtual void name_change(const uivec2 & oldid, const uivec2 newid);

	virtual nsasset * remove(uint32 res_type_id);

	virtual bool rename(const nsstring & oldName, const nsstring & newName);
	
	virtual bool save(nsasset * res, const nsstring & path);
	
	virtual void save_all(const nsstring & path="", nssave_resouces_callback * scallback = NULL);

	virtual bool save_as(nsasset * res, const nsstring & fname);
	
	virtual void set_res_dir(const nsstring & pDirectory);

	virtual void destroy_all();
	
	virtual bool destroy(nsasset * res);

	enum s_mode
	{
		binary,
		text
	};

	typedef std::unordered_map<uint32, nsasset*> map_type;

	map_type::iterator begin();

	template<class T>
	bool changed(const T & resource, const nsstring & fname="")
	{
		nsasset * res = get(resource);
		return changed(res,fname);
	}
	
	template<class T>
	bool contains(const T & res_name)
	{
		return contains(get(res_name));
	}

	template <class res_type>
	res_type * create(const nsstring & res_name, nsasset * to_copy=nullptr)
	{
		uint32 res_type_id = type_to_hash(res_type);
		return static_cast<res_type*>(create(res_type_id, res_name, to_copy));
	}

	template<class T>
	bool del(const T & res_name)
	{
		nsasset * res = get(res_name);
		return del(res);
	}

	template<class T>
	bool destroy(const T & resname)
	{
		nsasset * res = get(resname);
		return destroy(res);
	}

	template <class res_type, class T>
	res_type * get(const T & rname)
	{
		return static_cast<res_type*>(get(rname));
	}

	template<class res_type>
	res_type * load(const nsstring & fname, bool finalize_)
	{
		uint32 res_type_id = type_to_hash(res_type);
		return static_cast<res_type*>(load(res_type_id, fname, finalize_));
	}
	
	template<class res_type, class T >
	res_type * remove(const T & rname)
	{
		nsasset * res = get(rname);
		return static_cast<res_type*>(remove(res));
	}

	template<class T>
	bool save(const T & res_name, nsstring path="")
	{
		nsasset * res = get(res_name);
		return save(res, path);
	}

	template<class T>
	bool save_as(const T & resname, const nsstring & fname)
	{
		nsasset * res = get(resname);
		return save_as(res, fname);
	}

	bool changed(nsasset * res, nsstring fname);

	bool contains(nsasset * res);

	uint32 count() const;

	nsasset * create(const nsstring & guid_,
					 const nsstring & res_name,
					 nsasset * to_copy=nullptr);

	map_type::iterator end();

	bool empty();

	uint32 type();

	void finalize_all();

	nsasset * get(uint32 resid);

	nsasset * get(const nsstring & res_name);

	nsasset * get(nsasset * res);

	const nsstring & res_dir();

	const nsstring & local_dir();

	uint32 plugin_id();
	
	nsasset * load(const nsstring & res_guid, const nsstring & fname, bool finalize);

	nsasset * remove(const nsstring & res_name);
	
	nsasset * remove(nsasset * res);

	void set_plugin_id(uint32 plugid);

	void set_local_dir(const nsstring & pDirectory);

	s_mode save_mode() const;

	void set_save_mode(s_mode sm);

	static nsstring name_from_filename(const nsstring & fname);

	static nsstring path_from_filename(const nsstring & fname);
	
protected:
	uint32 m_hashed_type;
	nsstring m_res_dir;
	nsstring m_local_dir;
	map_type m_id_resmap;
	uint32 m_plugin_id;
	s_mode m_save_mode;
};

#endif
