/*!
\file nsengine.h

\brief Header file for nsengine class

This file contains all of the neccessary declartations for the nsengine class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#ifndef NSENGINE_H
#define NSENGINE_H

#define ENGINE_PLUG "engine_core"

// Default relative directories
#define DEFAULT_RESOURCE_DIR "resources/"
#define DEFAULT_IMPORT_DIR "import/"
#define DEFAULT_CORE_DIR "resources/core/"

// Resource typenames
// Must add entry here for any custom resource types
#define ANIM_TYPESTRING "nsanim_set"
#define ENTITY_TYPESTRING "nsentity"
#define MATERIAL_TYPESTRING "nsmaterial"
#define MESH_TYPESTRING "nsmesh"
#define INPUTMAP_TYPESTRING "nsinput_map"
#define TEX1D_TYPESTRING "nstex1d"
#define TEX1DARRAY_TYPESTRING "nstex1d_array"
#define TEX2D_TYPESTRING "nstex2d"
#define TEXRECTANGLE_TYPESTRING "nstex_rectangle"
#define TEX2DMULTISAMPLE_TYPESTRING "nstex2d_multisample"
#define TEX2DMULTISAMPLEARRAY_TYPESTRING "nstex2d_multisample_array"
#define TEXBUFFER_TYPESTRING "nstex_buffer"
#define TEX2DARRAY_TYPESTRING "nstex2d_array"
#define TEX3D_TYPESTRING "nstex3d"
#define TEXCUBEMAP_TYPESTRING "nstex_cubemap"
#define TEXCUBEMAPARRAY_TYPESTRING "nstex_cubemap_array"
#define SHADER_TYPESTRING "nsshader"
#define DIRLIGHTSHADER_TYPESTRING "nsdir_light_shader"
#define POINTLIGHTSHADER_TYPESTRING "nspoint_light_shader"
#define SPOTLIGHTSHADER_TYPESTRING "nsspot_light_shader"
#define MATERIALSHADER_TYPESTRING "nsmaterial_shader"
#define PARTICLEPROCESSSHADER_TYPESTRING "nsparticle_process_shader"
#define PARTICLERENDERSHADER_TYPESTRING "nsparticle_render_shader"
#define DIR_SHADOWMAP_SHADER_TYPESTRING "nsdir_shadowmap_shader"
#define DIR_SHADOWMAPXFB_SHADER_TYPESTRING "nsdir_shadowmap_xfb_shader"
#define POINT_SHADOWMAP_SHADER_TYPESTRING "nspoint_shadowmap_shader"
#define POINT_SHADOWMAPXFB_SHADER_TYPESTRING "nspoint_shadowmap_xfb_shader"
#define SPOT_SHADOWMAP_SHADER_TYPESTRING "nsspot_shadowmap_shader"
#define SPOT_SHADOWMAPXFB_SHADER_TYPESTRING "nsspot_shadowmap_xfb_shader"
#define EARLYZ_SHADER_TYPESTRING "nsearlyz_shader"
#define EARLYZXFB_SHADER_TYPESTRING "nsearlyz_xfb_shader"
#define RENDERXFB_SHADER_TYPESTRING "nsrender_xfb_shader"
#define XFB_SHADER_TYPESTRING "nsxfb_shader"
#define LIGHTSTENCIL_SHADER_TYPESTRING "nslight_stencil_shader"
#define SKYBOX_SHADER_TYPESTRING "nsskybox_shader"
#define TRANSPARENCY_SHADER_TYPESTRING "nstransparency_shader"
#define SELECTION_SHADER_TYPESTRING "nsselection_shader"
#define SCENE_TYPESTRING "nsscene"
#define PLUGIN_TYPESTRING "nsplugin"

// Manager Typenames
#define ANIM_MANAGER_TYPESTRING "nsanim_manager"
#define ENTITY_MANAGER_TYPESTRING "nsentity_manager"
#define MAT_MANAGER_TYPESTRING "nsmat_manager"
#define MESH_MANAGER_TYPESTRING "nsmesh_manager"
#define TEX_MANAGER_TYPESTRING "nstex_manager"
#define SHADER_MANAGER_TYPESTRING "nsshader_manager"
#define SCENE_MANAGER_TYPESTRING "nsscene_manager"
#define PLUGIN_MANAGER_TYPESTRING "nsplugin_manager"
#define INPUTMAP_MANAGER_TYPESTRING "nsinput_map_manager"

// Component Typenames
#define ANIM_COMP_TYPESTRING "nsanim_comp"
#define CAM_COMP_TYPESTRING "nscam_comp"
#define LIGHT_COMP_TYPESTRING "nslight_comp"
#define OCCUPY_COMP_TYPESTRING "nsoccupy_comp"
#define RENDER_COMP_TYPESTRING "nsrender_comp"
#define SEL_COMP_TYPESTRING "nssel_comp"
#define TFORM_COMP_TYPESTRING "nstform_comp"
#define TILEBRUSH_COMP_TYPESTRING "nstile_brush_comp"
#define TILE_COMP_TYPESTRING "nstile_comp"
#define PARTICLE_COMP_TYPESTRING "nsparticle_comp"
#define TERRAIN_COMP_TYPESTRING "nsterrain_comp"

// System Typenames
#define ANIM_SYS_TYPESTRING "nsanim_system"
#define CAM_SYS_TYPESTRING "nscamera_system"
#define BUILD_SYS_TYPESTRING "nsbuild_system"
#define MOVE_SYS_TYPESTRING "nsmovement_system"
#define RENDER_SYS_TYPESTRING "nsrender_system"
#define SEL_SYS_TYPESTRING "nsselection_system"
#define UI_SYS_TYPESTRING "nsui_system"
#define INP_SYS_TYPESTRING "nsinput_system"
#define PARTICLE_SYS_TYPESTRING "nsparticle_system"

// System update priority
#define ANIM_SYS_UPDATE_PR 70000
#define CAM_SYS_UPDATE_PR 40000
#define BUILD_SYS_UPDATE_PR 30000
#define MOVE_SYS_UPDATE_PR 50000
#define PARTICLE_SYS_UPDATE_PR 80000
#define RENDER_SYS_UPDATE_PR 90000
#define SEL_SYS_UPDATE_PR 60000
#define INP_SYS_UPDATE_PR 10000
#define UI_SYS_UPDATE_PR 20000


class nsscene;
class nsvideo_driver;
class nstform_system;
class nsanim_manager;
class nsmesh_manager;
class nstex_manager;
class nsmat_manager;
class nsshader_manager;
class nsevent_handler;
class nsanim_system;
class nsmovement_system;
class nstimer;
class nsplugin_manager;
class nssystem;
class nsplugin;
class nsevent_dispatcher;
struct nssave_resouces_callback;
class nsasset_manager;
class nsengine;


// Debuggin setup
//#define NSDEBUG

// Engine macros
extern nsengine * global_engine_ptr;

#define nse (*global_engine_ptr)

#define type_to_guid(type) nse.guid(std::type_index(typeid(type)))
#define hash_to_guid(hash) nse.guid(hash)
#define type_to_hash(type) nse.type_id(std::type_index(typeid(type)))

#define NSDEBUG_RT
#ifdef NSDEBUG
#define dprint(str) nse.debug_print(str)
#else
#define dprint(str)
#endif

#include <map>
#include <nsfactory.h>
#include <typeindex>
#include <nsmath.h>
#include <unordered_map>

#ifdef NSDEBUG
class nsdebug;
#endif

typedef std::unordered_map<uint32, nssystem*> system_hash_map;
typedef std::unordered_map<uint32, uint32> res_manager_type_map;
typedef std::unordered_map<std::type_index, uint32> type_hash_map;
typedef std::unordered_map<uint32, nsstring> hash_string_map;
typedef std::unordered_map<uint32, nsfactory*> hash_factory_map;

uint32 hash_id(const nsstring & str);

class nsengine
{
public:
	nsengine();
	~nsengine();

	typedef std::map<int32, uint32> sys_priority_map;

    nsscene * active_scene();
	
	bool add_system(nssystem * pSystem);

	hash_factory_map::iterator begin_factory();

	system_hash_map::iterator begin_system();

	template<class T=nsvideo_driver>
	T * video_driver()
	{
		return static_cast<T*>(m_driver);
	}
	
	template<class sys_type>
	sys_type * create_system()
	{
		uint32 tid = type_id(std::type_index(typeid(sys_type)));
		return static_cast<sys_type*>(create_system(tid));
	}
	
	nssystem * create_system(uint32 type_id);
	
	nssystem * create_system(const nsstring & guid_);

	template<class sys_type>
	bool destroy_system()
	{
		uint32 tid = type_id(std::type_index(typeid(sys_type)));
		return destroy_system(tid);
	}

	bool destroy_system(uint32 type_id);

	bool destroy_system(const nsstring & guid_);

#ifdef NSDEBUG
	void debug_print(const nsstring & str);
#endif

	hash_factory_map::iterator end_factory();

	system_hash_map::iterator end_system();

	nsplugin * core();

	nsplugin_manager * plugins();

	nsevent_dispatcher * event_dispatch();

	template<class obj_type>
	nsfactory * factory()
	{
		uint32 hashed_type = type_id(std::type_index(typeid(obj_type)));
		return factory(hashed_type);
	}

	nsfactory * factory(uint32 hashid);

	template<class base_fac_type>
	base_fac_type * factory(uint32 hashid)
	{
		return static_cast<base_fac_type*>(factory(hashid));
	}

	template<class base_fac_type>
	base_fac_type * factory(const nsstring & guid_)
	{
		return factory<base_fac_type>(hash_id(guid_)) ;
	}

	template<class base_fac_type, class obj_type>
	base_fac_type * factory()
	{
		uint32 hashed_type = type_id(std::type_index(typeid(obj_type)));
		return static_cast<base_fac_type*>(factory(hashed_type));
	}

	template<class sys_type>
	bool has_system()
	{
		uint32 hashed_type = type_id(std::type_index(typeid(sys_type)));
		return has_system(hashed_type);
	}

	bool has_system(uint32 type_id);
	
	bool has_system(const nsstring & guid_);

	template<class comp_type>
	bool register_component(const nsstring & guid_)
	{
		uint32 hashed = hash_id(guid_);
		auto ret = m_obj_type_names.emplace(hashed, guid_);
		
		if (!ret.second)
		{
			dprint(nsstring("registerComponentType - Could not generate unique hash from ") + guid_);
			return false;
		}

		std::type_index ti(typeid(comp_type));
		auto check = m_obj_type_hashes.emplace(ti, hashed);

		if (!check.second)
		{
			dprint(nsstring("registerComponentType - Could not generate unique type_index from ") + ti.name());
			return false;
		}

		auto rf = _create_factory<nscomp_factory_type<comp_type>,comp_type>();
		if (rf == nullptr)
			return false;		
		return true;
	}

	template<class sys_type>
	bool register_system(const nsstring & guid_)
	{
		uint32 hashed = hash_id(guid_);
		auto ret = m_obj_type_names.emplace(hashed, guid_);
		
		if (!ret.second)
		{
			dprint(nsstring("registerSystemType - Could not generate unique hash from ") + guid_);
			return false;
		}

		std::type_index ti(typeid(sys_type));
		auto check = m_obj_type_hashes.emplace(ti, hashed);

		if (!check.second)
		{
			dprint(nsstring("registerSystemType - Could not generate unique type_index from ") + ti.name());
			return false;
		}

		auto rf = _create_factory<nssys_factory_type<sys_type>,sys_type>();
		if (rf == nullptr)
			return false;		
		return true;
	}

	template<class res_type, class manager_type>
	bool register_resource(const nsstring & guid_)
	{
		uint32 hashed = hash_id(guid_);
		auto ret = m_obj_type_names.emplace(hashed, guid_);
		
		if (!ret.second)
		{
			dprint(nsstring("registerResourceType - Could not generate unique hash from ") + guid_);
			return false;
		}

		std::type_index ti(typeid(res_type));
		auto check = m_obj_type_hashes.emplace(ti, hashed);

		if (!check.second)
		{
			dprint(nsstring("registerResourceType - Could not generate unique type_index from ") + ti.name());
			return false;
		}

		std::type_index tim(typeid(manager_type));
		auto fiter = m_obj_type_hashes.find(tim);

		if (fiter == m_obj_type_hashes.end())
		{
			dprint(nsstring("registerResourceType - Could not find hash_id for ") + tim.name() + nsstring(" - did you forget to register the manager first?"));
			return false;
		}


		nsasset_factory_type<res_type> * rf = _create_factory<nsasset_factory_type<res_type>, res_type>();
		if (rf == nullptr)
			return false;
		
		m_res_manager_map.emplace(hashed, fiter->second);
		return true;
	}

	template<class res_type, class manager_type>
	bool register_abstract_resource(const nsstring & guid_)
	{
		uint32 hashed = hash_id(guid_);
		auto ret = m_obj_type_names.emplace(hashed, guid_);
		
		if (!ret.second)
		{
			dprint(nsstring("registerAbstractResourceType - Could not generate unique hash from ") + guid_);
			return false;
		}

		std::type_index ti(typeid(res_type));
		auto check = m_obj_type_hashes.emplace(ti, hashed);

		if (!check.second)
		{
			dprint(nsstring("registerAbstractResourceType - Could not generate unique type_index from ") + ti.name());
			return false;
		}

		std::type_index tim(typeid(manager_type));
		auto fiter = m_obj_type_hashes.find(tim);

		if (fiter == m_obj_type_hashes.end())
		{
			dprint(nsstring("registerResourceType - Could not find hash_id for ") + tim.name() + nsstring(" - did you forget to register the manager first?"));
			return false;
		}

		m_res_manager_map.emplace(hashed, fiter->second);
		return true;
	}

	template<class vid_obj_type>
	bool register_vid_obj_type(const nsstring & guid_)
	{
		uint32 hashed = hash_id(guid_);
		auto ret = m_obj_type_names.emplace(hashed, guid_);
	
		if (!ret.second)
		{
			dprint(nsstring("register_vid_obj_type - Could not generate unique hash from ") + guid_);
			return false;
		}

		std::type_index ti(typeid(vid_obj_type));
		auto check = m_obj_type_hashes.emplace(ti, hashed);

		if (!check.second)
		{
			dprint(nsstring("register_vid_obj_type- Could not generate unique type_index from ") + ti.name());
			return false;
		}

		auto rf = _create_factory<nsvid_obj_factory_type<vid_obj_type>, vid_obj_type>();
		if (rf == nullptr)
			return false;
		return true;
	}

	template<class manager_type>
	bool register_manager(const nsstring & guid_)
	{
		uint32 hashed = hash_id(guid_);
		auto ret = m_obj_type_names.emplace(hashed, guid_);
		
		if (!ret.second)
		{
			dprint(nsstring("registerResourceManagerType - Could not generate unique hash from ") + guid_);
			return false;
		}

		std::type_index ti(typeid(manager_type));
		auto check = m_obj_type_hashes.emplace(ti, hashed);

		if (!check.second)
		{
			dprint(nsstring("registerResourceManagerType - Could not generate unique type_index from ") + ti.name());
			return false;
		}

		auto rf = _create_factory<nsmanager_factory_type<manager_type>,manager_type>();
		if (rf == nullptr)
			return false;		
		return true;
	}

	template<class sys_type>
	sys_type * remove_system()
	{
		uint32 hashed_type = type_id(std::type_index(typeid(sys_type)));
		return static_cast<sys_type*>(remove_system(hashed_type));
	}

	nssystem * remove_system(uint32 type_id);

	nssystem * remove_system(const nsstring & gui);
	
	void name_change(const uivec2 & oldid, const uivec2 newid);

	nsstring guid(uint32 hash);

	nsstring guid(std::type_index type);

	uint32 manager_id(uint32 res_id);

	uint32 manager_id(std::type_index res_type);

	uint32 manager_id(const nsstring & res_guid);

	uint32 type_id(std::type_index type);
	
	const nsstring & import_dir();

	void set_import_dir(const nsstring & dir);

	void release();

	bool running();

	void init(nsvideo_driver * driver);	

	void set_active_scene(nsscene * active_scene);

	void start();

	void stop();

	template<class sys_type>
	sys_type * system()
	{
		uint32 hashed_type = type_id(std::type_index(typeid(sys_type)));
		return static_cast<sys_type*>(system(hashed_type));
	}

	nssystem * system(uint32 type_id);

	nssystem * system(const nsstring & guid_);

	nstimer * timer();

	void update();

	const nsstring & cwd();

#ifdef NSDEBUG
	nsdebug * debug();
#endif

private:

	void _create_factory_systems();
	
	void _cleanup_driver();
	
	template<class obj_type>
	bool _add_factory(nsfactory * fac)
	{
		if (fac == nullptr)
			return false;

		std::type_index ti = std::type_index(typeid(obj_type));
		auto iter = m_obj_type_hashes.find(ti);

		if (iter == m_obj_type_hashes.end())
			return false;
		
		auto it = m_factories.emplace(iter->second, fac);
		return it.second;
	}
	
	template<class base_fac_type, class obj_type>
	base_fac_type * _create_factory()
	{
		base_fac_type * fac = new base_fac_type;
		if (!_add_factory<obj_type>(fac))
		{
			delete fac;
			return nullptr;
		}
		return fac;
	}
	
	template<class obj_type>
	bool _destroy_factory()
	{
		std::type_index ti = std::type_index(typeid(obj_type));
		auto iter = m_obj_type_hashes.find(ti);

		if (iter == m_obj_type_hashes.end())
			return false;

        return _destroy_factory(iter->second);
	}

	bool _destroy_factory(uint32 hashid);

	nsfactory * _remove_factory(uint32 hashid);
	
	template<class base_fac_type,class obj_type>
	base_fac_type * _remove_factory()
	{
		std::type_index ti = std::type_index(typeid(obj_type));
		auto iter = m_obj_type_hashes.find(ti);

		if (iter == m_obj_type_hashes.end())
			return false;

        return static_cast<base_fac_type*>(_remove_factory(iter->second));
	}

	void _init_factories();
	void _remove_sys(uint32 type_id);
		
	type_hash_map m_obj_type_hashes;
	hash_string_map m_obj_type_names;
	hash_factory_map m_factories;
	res_manager_type_map m_res_manager_map;

	nsvideo_driver * m_driver;

	sys_priority_map m_sys_update_order;
	system_hash_map * m_systems;
	nsplugin_manager * m_plugins;
	nsevent_dispatcher * m_event_disp;
	nstimer * m_timer;
    nsscene * m_active_scene;

#ifdef NSDEBUG
	nsdebug * m_deb;
#endif

	nsstring m_import_dir;
	nsstring m_cwd;
	bool m_running;
	bool m_initialized;
};

nsasset * get_asset(uint32 res_type, const uivec2 & res_id);

template<class res_type>
res_type * get_asset(const uivec2 & res_id)
{
	uint32 hashed_type = type_to_hash(res_type);
	return static_cast<res_type*>(get_asset(hashed_type,res_id));	
}


#endif
