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
#define DEFAULT_CORE_DIR "core/"

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
#define UI_SYS_TYPESTRING "NSUISystem"
#define INP_SYS_TYPESTRING "nsinput_system"
#define PARTICLE_SYS_TYPESTRING "nsparticle_system"

// System update priority
#define ANIM_SYS_UPDATE_PR 70000
#define CAM_SYS_UPDATE_PR 40000
#define BUILD_SYS_UPDATE_PR 50000
#define MOVE_SYS_UPDATE_PR 60000
#define PARTICLE_SYS_UPDATE_PR 80000
#define RENDER_SYS_UPDATE_PR 90000
#define SEL_SYS_UPDATE_PR 30000
#define INP_SYS_UPDATE_PR 10000
#define UI_SYS_UPDATE_PR 20000

// System draw priority
#define RENDER_SYS_DRAW_PR 10000
#define PARTICLE_SYS_DRAW_PR 20000
#define SEL_SYS_DRAW_PR 30000
#define UI_SYS_DRAW_PR 40000
#define NO_DRAW_PR 0

// Engine macros
#define nse nsengine::inst()
#define type_to_guid(type) nse.guid(std::type_index(typeid(type)))
#define hash_to_guid(hash) nse.guid(hash)
#define type_to_hash(type) nse.type_id(std::type_index(typeid(type)))

// Debuggin setup
//#define NSDEBUG
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

class nsscene;
class nsrender_system;
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
class nsres_manager;
struct gl_ctxt;
class nsfb_object;
#ifdef NSDEBUG
class nsdebug;
#endif

typedef std::unordered_map<uint32, nssystem*> system_hash_map;
typedef std::unordered_map<uint32, uint32> res_manager_type_map;
typedef std::unordered_map<std::type_index, uint32> type_hash_map;
typedef std::unordered_map<uint32, nsstring> hash_string_map;
typedef std::unordered_map<uint32, nsfactory*> hash_factory_map;
typedef std::unordered_map<uint32, gl_ctxt*> gl_context_map;
typedef std::unordered_map<uint32, nsfb_object*> framebuffer_map;

uint32 hash_id(const nsstring & str);

// make sure all directories end with "/"

/*!
Multiple contexts works like this - GLEW has a context struct for storing the function pointers for the opengl functions...
Basically - as far as I understand - when initializing GLEW it will use whichever gl context is the current context to get the function
pointers - a GLEWContext is NOT the actual opengl context but just a struct to hold function pointers.. This means, for example, you could
have to different "contexts" (being GLEWContext not actual opengl contexts) that define function pointers for the same actual context...

For this reason from now on I will refer to an actual opengl context as a "rendering context", and a GLEWContext (which has a bunch of function
pointers initialized from whicher rendering context was current at the time of initialization) as a glew context.

The engine doesnt care about which rendering context is current, or which device context (which is the window) is being used by the rendering
context - the engine only cares about which glew context is being used and whoever is using the engine must take care to make sure that
each rendering context has a glew context and that the rendering context is active when initializing the glew context.

The user must make sure that when switching rendering contexts, the engine's current glew context is also changed to match. When a rendering context is created
and made current, a glew context should be created and initialized (in that order). When a rendering context is made active, the corresponding glew context should be made active. When a rendering
context is deleted, the corresponding glew context should be deleted (freeing all opengl resources used by that context). The rendering context should be deleted AFTER the glew context is
deleted, and the rendering context should be the current context when deleting the glew context. This is so that the glDeleteX functions do not fail.

Summary (leaving out device context instructions - see MSDN for that)

****Creation*****
Create rendering context and make current
Create glew context and initialize (engine initializes automatically when creating)

****Switching Contexts****
Make different rendering context current
Change the current glew context to match the rendering context

****Deleting Contexts****
Make context current
Delete the glew context (engine will delete all resources on deleteContext call)
Delete rendering context

To associate a glew context with a rendering context, the engine stores the glew contexts under a string name. You can pick
whatever string you want, but make sure you do not give two rendering contexts the same name. They are stored in a map so adding a glew
context with the same name will fail.

We dont share resources between contexts in the engine, because VAOs are used for almost everything which doesn't allow for
sharing - we could if we really wanted make everything just use VBOs, then a different context could do its thing, but thats annoying
Its easier to just not allow sharing
*/
class nsengine
{
public:
	nsengine();
	~nsengine();
	typedef std::map<int32, uint32> sys_priority_map;

	bool add_plugin(nsplugin * plug);

	/* Add a system to the current context */
	bool add_system(nssystem * pSystem);

	nsplugin * active();

	hash_factory_map::iterator begin_factory();

	system_hash_map::iterator begin_system();

	uint32 composite_framebuffer();

	/*!
	Create a GLContext and return a unique id - this id can be used to set the current context and
	delete the context later

	This does not initialize the context - call start/shutdown to initialize and release opengl and all
	resources associated with the context

	The newly created context will be set as the current context.
	*/
	uint32 create_context(bool addDefaultFactories = true //<! Add the normal system/component/resource/resource manager factories
		);

	uint32 create_framebuffer();

	nsplugin * create_plugin(const nsstring & plugname, bool makeactive=true);

	template<class sys_type>
	sys_type * create_system()
	{
		uint32 tid = type_id(std::type_index(typeid(sys_type)));
		return static_cast<sys_type*>(create_system(tid));
	}

	nssystem * create_system(uint32 type_id);
	
	nssystem * create_system(const nsstring & guid_);

	gl_ctxt * current();

	uint32 current_id();

	nsscene * current_scene();

	/*!
	Delete the context with ID cID
	Make sure shutdown has been called with the context to be deleted set as the current context
	*/
	bool destroy_context(uint32 cID);

	template<class T>
	bool del_plugin(const T & name)
	{
		nsplugin * plg = plugin(name);
		return del_plugin(plg);
	}

	bool del_plugin(nsplugin * plg);

	bool del_framebuffer(uint32 fbid);

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

	template<class T>
	bool has_plugin(const T & name)
	{
		nsplugin * plg = plugin(name);
		return has_plugin(plg);
	}

	bool has_plugin(nsplugin * plg);

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

	nsfb_object * framebuffer(uint32 id);

	template<class sys_type>
	bool has_system()
	{
		uint32 hashed_type = type_id(std::type_index(typeid(sys_type)));
		return has_system(hashed_type);
	}

	bool has_system(uint32 type_id);
	
	bool has_system(const nsstring & guid_);

	nsplugin * load_plugin(const nsstring & fname);

	template<class manager_type, class T>
	manager_type * manager(const T & plugname)
	{
		uint32 hashed_type = type_to_hash(manager_type);
		return static_cast<manager_type*>(manager(hashed_type, plugin(plugname)));
	}

	nsres_manager * manager(uint32 manager_typeid, nsplugin * plg);

	nsres_manager * manager(const nsstring & manager_guid, nsplugin * plg);

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
		if (rf == NULL)
			return false;
		
		rf->type_id = hashed;
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
		if (rf == NULL)
			return false;
		
		rf->type_id = hashed;
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


		nsres_factory_type<res_type> * rf = _create_factory<nsres_factory_type<res_type>, res_type>();
		if (rf == NULL)
			return false;
		
		rf->type_id = hashed;
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
		if (rf == NULL)
			return false;
		
		rf->type_id = hashed;
		return true;
	}

	void save_core(nssave_resouces_callback * scallback=NULL);

	bool save_plugin(nsplugin * plg, bool saveOwnedResources=false, nssave_resouces_callback * scallback=NULL);

	void save_plugins(bool saveOwnedResources=false, nssave_resouces_callback * scallback = NULL);

	void set_plugin_dir(const nsstring & plugdir);

	const nsstring & plugin_dir();

	template<class T>
	nsplugin * remove_plugin(const T & name)
	{
		nsplugin * plg = plugin(name);
		return remove_plugin(plg);
	}

	nsplugin * remove_plugin(nsplugin * plg);

	template<class sys_type>
	sys_type * remove_system()
	{
		uint32 hashed_type = type_id(std::type_index(typeid(sys_type)));
		return static_cast<sys_type*>(remove_system(hashed_type));
	}

	nssystem * remove_system(uint32 type_id);

	nssystem * remove_system(const nsstring & gui);

	nsplugin * plugin(const nsstring & name);

	nsplugin * plugin(uint32 id);

	nsplugin * plugin(nsplugin * plg);

	template<class T>
	bool destroy_plugin(const T & name)
	{
		return destroy_plugin(plugin(name));
	}

	bool destroy_plugin(nsplugin * plug);

	template<class res_type>
	res_type * resource(const uivec2 & resID)
	{
		uint32 hashed_type = type_to_hash(res_type);
		return static_cast<res_type*>(_resource(hashed_type,resID));
	}

	template<class res_type,class T1, class T2>
	res_type * resource(const T1 & plg, const T2 & res)
	{
		nsplugin * plug = plugin(plg);
		return static_cast<res_type*>(resource(type_to_hash(res_type),plug,res));
	}

	nsresource * resource(uint32 res_typeid, nsplugin * plg, uint32 resid);

	nsresource * resource(uint32 res_typeid, nsplugin * plg, const nsstring & resname);
	
	/*!
	Overload of Propagate name change
	*/
	void name_change(const uivec2 & oldid, const uivec2 newid);

	const nsstring & res_dir();

	nsstring guid(uint32 hash);

	nsstring guid(std::type_index type);

	uint32 manager_id(uint32 res_id);

	uint32 manager_id(std::type_index res_type);

	uint32 manager_id(const nsstring & res_guid);

	uint32 type_id(std::type_index type);
	
	void set_active(const nsstring & plugname);

	void set_active(nsplugin * plug);

	void set_active(uint32 plugid);

	void set_current_scene(const nsstring & scn, bool newSceneOverwriteFile = false, bool saveprevious = false);

	void set_current_scene(nsscene * scn, bool newSceneOverwriteFile = false, bool saveprevious = false);

	void set_current_scene(uint32 scn, bool newSceneOverwriteFile = false, bool saveprevious = false);

	bool make_current(uint32 cID);

	const nsstring & import_dir();

	void set_import_dir(const nsstring & dir);

	void set_res_dir(const nsstring & dir);

	void shutdown();

	void start();

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

	/*!
	Get engine instance.. if this is the first call then a new instance is created (static)
	*/
	static nsengine & inst();

private:

	template<class obj_type>
	bool _add_factory(nsfactory * fac)
	{
		if (fac == NULL)
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
			return NULL;
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

	void _init_systems();
	void _init_factories();
	void _remove_sys(uint32 type_id);

	nsresource * _resource(uint32 restype_id, const uivec2 & resid);
	
	nsstring m_res_dir;
	nsstring m_import_dir;
	
	sys_priority_map m_sys_update_order;
	sys_priority_map m_sys_draw_order;

	type_hash_map m_obj_type_hashes;
	hash_string_map m_obj_type_names;
	hash_factory_map m_factories;
	res_manager_type_map m_res_manager_map;
	
	gl_context_map m_contexts;
	uint32 m_current_context;
	nsstring m_cwd;
};


#endif
