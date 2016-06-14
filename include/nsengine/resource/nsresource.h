/*! 
	\file nsresource.h
	
	\brief Header file for nsresource class

	This file contains all of the neccessary declarations for the nsresource class.

	\author Daniel Randle
	\date November 23 2013
	\copywrite Earth Banana Games 2013
*/
#ifndef NSRESOURCE_H
#define NSRESOURCE_H

#define NOTIFY_ENGINE_NAME_CHANGE

#define DEFAULT_TEX_EXTENSION ".png"
#define DEFAULT_MAT_EXTENSION ".mat"
#define DEFAULT_MESH_EXTENSION ".msh"
#define DEFAULT_ANIM_EXTENSION ".anm"
#define DEFAULT_SHADER_EXTENSION ".shr"
#define DEFAULT_SCENE_EXTENSION ".map"
#define DEFAULT_ENTITY_EXTENSION ".ent"
#define DEFAULT_PLUGIN_EXTENSION ".bbp"
#define DEFAULT_INPUT_MAP_EXTENSION ".nsi"
#define DEFAULT_FONT_EXTENSION ".bft"
#define CUBEMAP_TEX_EXTENSION ".cube"

#include <nspupper.h>
#include <nsvector.h>
#include <nssignal.h>

class nsresource_inst;

class nsresource
{
public:
	friend class nsres_manager;
	friend class nsres_factory;

	nsresource(uint32 type_hash);
	nsresource(const nsresource & copy);
	
	virtual ~nsresource();

	virtual void init() = 0;

	virtual void release() {}

	const nsstring & extension() const;

	const nsstring & name() const;

	uint32 plugin_id() const;

	uint32 id() const;

	uint32 type() const;

	virtual void finalize() {};

	const nsstring & subdir() const;

	virtual uivec3_vector resources();

	const nsstring & icon_path() const;

	const uivec2 & icon_tex_id() const;

	virtual void name_change(const uivec2 & oldid, const uivec2 newid);

	void enable_group_save(bool enable);

	bool group_save_enabled();

	virtual void pup(nsfile_pupper * p) = 0;

	bool owned() { return m_owned; }

	void set_ext( const nsstring & pExt);

	void rename(const nsstring & pResName);

	uivec2 full_id();

	void set_icon_path(const nsstring & pIconPath);

	void set_icon_tex_id(const uivec2 & texID);

	void set_subdir(const nsstring & pDir);

	nsresource & operator=(nsresource_inst rhs);

	ns::signal<uint32, uivec2> resource_destroyed;
	
protected:

	uint32 m_hashed_type;
	nsstring m_icon_path;
	uivec2 m_icon_tex_id;
	nsstring m_name;
	nsstring m_subdir;
	nsstring m_ext;
	uint32 m_id;
	uint32 m_plugin_id;
	bool m_owned;
	bool m_save_with_group;
};

class nsresource_inst : public nsresource
{
	nsresource_inst(const nsresource & rhs):nsresource(rhs) {}
	void init() {std::terminate();}
 	void pup(nsfile_pupper *) {std::terminate();}
};

#endif
