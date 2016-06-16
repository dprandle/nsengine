/*!
\file nsengine.cpp

\brief Definition file for nsengine class

This file contains all of the neccessary definitions for the nsengine class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#include <nsui_button_comp.h>
#include <nsui_canvas_comp.h>
#include <nsrect_tform_comp.h>
#include <nsui_system.h>
#include <nsvideo_driver.h>
#include <hash/crc32.h>
#include <nsfile_os.h>
#include <nsengine.h>
#include <nsrender_system.h>
#include <nsscene.h>
#include <nsinput_map_manager.h>
#include <nsentity_manager.h>
#include <nsanim_manager.h>
#include <nsmesh_manager.h>
#include <nstex_manager.h>
#include <nsmat_manager.h>
#include <nsevent_dispatcher.h>
#include <nsshader_manager.h>
#include <nsinput_map.h>
#include <nspupper.h>
#include <nsscene_manager.h>
#include <nssel_comp.h>
#include <nsterrain_comp.h>
#include <nsanim_system.h>
#include <IL/il.h>
#include <nsinput_system.h>
#include <nsparticle_comp.h>
#include <nstimer.h>
#include <nsentity.h>
#include <nscamera_system.h>
#include <nsselection_system.h>
#include <nsevent.h>
#include <nsplugin_manager.h>
#include <nstile_brush_comp.h>
#include <nstile_comp.h>
#include <nsoccupy_comp.h>
#include <nsbuild_system.h>
#include <nsparticle_comp.h>
#include <nsparticle_system.h>
#include <nslog_file.h>
#include <nsfactory.h>
#include <nssystem.h>
#include <nsrender_system.h>
#include <nsgl_framebuffer.h>
#include <nsanim_comp.h>
#include <nslight_comp.h>
#include <nscam_comp.h>
#include <nsrender_comp.h>
#include <nsui_comp.h>
#include <nsfont_manager.h>

#ifdef NSDEBUG
#include <nsdebug.h>
#endif

nsengine::nsengine():
	m_systems(new system_hash_map()),
	m_event_disp(new nsevent_dispatcher()),
	m_timer(new nstimer()),
#ifdef NSDEBUG
	m_deb(new nsdebug()),
#endif
	m_cwd(nsfile_os::cwd())
{
	m_import_dir = m_cwd + nsstring(DEFAULT_IMPORT_DIR);
	srand(static_cast<unsigned>(time(0)));
    ilInit();
	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);

#ifdef NSDEBUG
	m_deb->set_log_file("engine_debug.log");
	m_deb->set_log_dir(m_cwd + "logs");
	m_deb->clear_log();
#endif
}

nsengine::~nsengine()
{
	delete m_plugins;
	delete m_systems;
	delete m_event_disp;
	delete m_timer;
#ifdef NSDEBUG
	delete m_deb;
#endif
}

bool nsengine::add_system(nssystem * sys_to_add)
{
	if (sys_to_add == NULL)
		return false;

	std::type_index ti(typeid(*sys_to_add));
	uint32 type_hash = nse.type_id(ti);

	if (type_hash == 0)
	{
		dprint(nsstring("nsengine::add_system - Could not find type id for type: ") + ti.name());
		return false;
	}
	auto it = m_systems->emplace(type_hash, sys_to_add);

	if (!it.second)
		return false;

	m_sys_update_order[sys_to_add->update_priority()] = type_hash;
	return true;
}

bool nsengine::_destroy_factory(uint32 hashid)
{
	nsfactory * fac = _remove_factory(hashid);
	if (fac == NULL)
		return false;
	delete fac;
	return true;
}

bool nsengine::destroy_system(uint32 type_id)
{
	nssystem * sys = remove_system(type_id);
	if (sys == NULL)
		return false;
	delete sys;
	return true;
}

bool nsengine::destroy_system(const nsstring & guid_)
{
	return destroy_system(hash_id(guid_));
}

hash_factory_map::iterator nsengine::begin_factory()
{
	return m_factories.begin();
}

system_hash_map::iterator nsengine::begin_system()
{
	return m_systems->begin();
}

nssystem * nsengine::create_system(uint32 type_id)
{
	nssystem * system = factory<nssys_factory>(type_id)->create();
	if (!add_system(system))
	{
		delete system;
		return NULL;
	}
	system->init();
	return system;
}

nssystem * nsengine::create_system(const nsstring & guid_)
{
	return create_system(hash_id(guid_));
}

hash_factory_map::iterator nsengine::end_factory()
{
	return m_factories.end();
}

system_hash_map::iterator nsengine::end_system()
{
	return m_systems->end();
}

nstimer * nsengine::timer()
{
	return m_timer;
}

#ifdef NSDEBUG
void nsengine::debug_print(const nsstring & str)
{
	m_deb->print(str);
}
#endif

nsevent_dispatcher * nsengine::event_dispatch()
{
	return m_event_disp;
}

nssystem * nsengine::system(uint32 type_id)
{
	auto iter = m_systems->find(type_id);
	if (iter == m_systems->end())
		return nullptr;
	return iter->second;
}

nssystem * nsengine::system(const nsstring & guid_)
{
	return system(hash_id(guid_));
}

bool nsengine::has_system(uint32 type_id)
{
	return (m_systems->find(type_id) != m_systems->end());
}

bool nsengine::has_system(const nsstring & guid_)
{
	return has_system(hash_id(guid_));
}

void nsengine::name_change(const uivec2 & oldid, const uivec2 newid)
{
	if (oldid.y == core()->id())
	{
		dprint("nsengine::name_change Cannot change engine default plugin name");
		return;
	}
	if (oldid.x == core()->id())
		core()->name_change(oldid, newid);
	plugins()->name_change(oldid, newid);
}

nsplugin * nsengine::core()
{
	return m_plugins->get(ENGINE_PLUG);
}

nsplugin_manager * nsengine::plugins()
{
	return m_plugins;
}

nssystem * nsengine::remove_system(uint32 type_id)
{
	nssystem * sys = system(type_id);
	if (sys == NULL)
		return NULL;
	m_systems->erase(type_id);
	_remove_sys(type_id);
	return sys;
}

nssystem * nsengine::remove_system(const nsstring & guid_)
{
	return remove_system(hash_id(guid_));
}

const nsstring & nsengine::import_dir()
{
	return m_import_dir;
}

void nsengine::set_import_dir(const nsstring & dir)
{
	m_import_dir = dir;
}

void nsengine::start(bool create_default_systems)
{
	if (m_driver == nullptr)
	{
		dprint("nsengine::start Cannot initialize engine without video driver created (use nse.create_video_driver<driver_type>()");
		return;
	}
	
	_init_factories();
	nsfile_os::platform_init();

	m_plugins = new nsplugin_manager();
	m_plugins->set_res_dir(m_cwd);

	nsplugin * plg = m_plugins->create(ENGINE_PLUG);
	plg->init();
	plg->enable(true);
	plg->set_managers_res_dir(m_cwd + DEFAULT_CORE_DIR);
	plg->enable_group_save(false);
	m_driver->init();
	if (create_default_systems)
		_create_factory_systems();
	m_timer->start();
}

void nsengine::shutdown()
{
	m_plugins->destroy_all();
	delete m_plugins;
	m_plugins = nullptr;
	
	m_event_disp->clear();
	auto iter = m_systems->begin();
	while (iter != m_systems->end())
	{
		iter->second->release();
		delete iter->second;
		iter->second = NULL;
		++iter;
	}
	m_systems->clear();
	delete m_driver;
	m_driver = nullptr;
	m_timer->pause();
}

nsfactory * nsengine::factory(uint32 hash_id)
{
	auto fIter = m_factories.find(hash_id);
	if (fIter == m_factories.end())
		return NULL;
	return fIter->second;
}

nsstring nsengine::guid(std::type_index type)
{
	auto fiter = m_obj_type_hashes.find(type);
	if (fiter == m_obj_type_hashes.end())
		return "";
	return guid(fiter->second);
}

nsstring nsengine::guid(uint32 hash)
{
	auto fiter = m_obj_type_names.find(hash);
	if (fiter != m_obj_type_names.end())
		return fiter->second;
	return "";
}

uint32 nsengine::type_id(std::type_index type)
{
	auto fiter = m_obj_type_hashes.find(type);
	if (fiter != m_obj_type_hashes.end())
		return fiter->second;
	return 0;
}

nsfactory * nsengine::_remove_factory(uint32 hash_id)
{
	nsfactory * f = factory(hash_id);
	if (f == NULL)
		return NULL;
	m_factories.erase(hash_id);
	return f;
}

void nsengine::set_active_scene(nsscene * active_scene)
{
	auto sys_iter = m_systems->begin();
	while (sys_iter != m_systems->end())
	{
		sys_iter->second->set_active_scene(active_scene);
		++sys_iter;
	}	
}

void nsengine::update()
{
	static double accumulator = 0.0;
	timer()->update();
	accumulator += timer()->dt();
    while (accumulator >= timer()->fixed())
	{
		auto sys_iter = m_sys_update_order.begin();
		while (sys_iter != m_sys_update_order.end())
		{
			nssystem * sys = system(sys_iter->second);
			event_dispatch()->process(sys);
			sys->update();
			++sys_iter;
		}
		accumulator -= timer()->fixed();
	}
}

void nsengine::_remove_sys(uint32 type_id)
{
	auto iter2 = m_sys_update_order.begin();
	while (iter2 != m_sys_update_order.end())
	{
		if (iter2->second == type_id)
		{
			m_sys_update_order.erase(iter2);
			return;
		}
		++iter2;
	}
}

void nsengine::_create_factory_systems()
{
	auto fiter = nse.begin_factory();
	while (fiter != nse.end_factory())
	{
		if (fiter->second->type() == nsfactory::f_system)
			create_system(guid(fiter->first));
		++fiter;
	}
}

#ifdef NSDEBUG
nsdebug * nsengine::debug()
{
	return m_deb;
}
#endif

uint32 nsengine::manager_id(uint32 res_id)
{
	auto iter = m_res_manager_map.find(res_id);
	if (iter != m_res_manager_map.end())
		return iter->second;
	return 0;
}

uint32 nsengine::manager_id(std::type_index res_type)
{
	uint32 hashed_type = type_id(res_type);
	return manager_id(hashed_type);
}

uint32 nsengine::manager_id(const nsstring & res_guid)
{
	return manager_id(hash_id(res_guid));
}

const nsstring & nsengine::cwd()
{
	return m_cwd;
}

void nsengine::_cleanup_driver()
{
	if (m_driver != nullptr)
	{
		delete m_driver;
		m_driver = nullptr;
	}
}

void nsengine::_init_factories()
{
	register_component<nsrect_tform_comp>("nsrect_tform_comp");
	register_component<nsanim_comp>("nsanim_comp");
	register_component<nscam_comp>("nscam_comp");
	register_component<nslight_comp>("nslight_comp");
	register_component<nsoccupy_comp>("nsoccupy_comp");
	register_component<nsparticle_comp>("nsparticle_comp");
	register_component<nsrender_comp>("nsrender_comp");
	register_component<nssel_comp>("nssel_comp");
	register_component<nstform_comp>("nstform_comp");
	register_component<nstile_brush_comp>("nstile_brush_comp");
	register_component<nstile_comp>("nstile_comp");
	register_component<nsterrain_comp>("nsterrain_comp");
	register_component<nsui_material_comp>("nsui_comp");
	register_component<nsui_text_input_comp>("nsui_text_input_comp");
	register_component<nsui_text_comp>("nsui_text_comp");
	register_component<nsui_canvas_comp>("nsui_canvas_comp");
	register_component<nsui_button_comp>("nsui_button_comp");
	
	register_system<nsanim_system>("nsanim_system");
	register_system<nsbuild_system>("nsbuild_system");
	register_system<nscamera_system>("nscamera_system");
	register_system<nsinput_system>("nsinput_system");
	register_system<nsparticle_system>("nsparticle_system");
	register_system<nsrender_system>("nsrender_system");
	register_system<nsselection_system>("nsselection_system");
	register_system<nsui_system>("nsui_system");
	
	register_manager<nsanim_manager>("nsanim_manager");
	register_manager<nsentity_manager>("nsentity_manager");
	register_manager<nsmat_manager>("nsmat_manager");
	register_manager<nsmesh_manager>("nsmesh_manager");
	register_manager<nsscene_manager>("nsscene_manager");
	register_manager<nsshader_manager>("nsshader_manager");
	register_manager<nstex_manager>("nstex_manager");
	register_manager<nsinput_map_manager>("nsinput_map_manager");
	register_manager<nsplugin_manager>("nsplugin_manager");
	register_manager<nsfont_manager>("nsfont_manager");
	
	register_resource<nsanim_set, nsanim_manager>("nsanim_set");
	register_resource<nsentity, nsentity_manager>("nsentity");
	register_resource<nsmaterial, nsmat_manager>("nsmaterial");
	register_resource<nsplugin, nsplugin_manager>("nsplugin");
	register_resource<nsscene, nsscene_manager>("nsscene");
	register_resource<nsmesh, nsmesh_manager>("nsmesh");
	register_resource<nsmesh_plane, nsmesh_manager>("nsmesh_plane");
	register_abstract_resource<nstexture, nstex_manager>("nstexture");
	register_resource<nstex1d, nstex_manager>("nstex1d");
	register_resource<nstex2d, nstex_manager>("nstex2d");
	register_resource<nstex3d, nstex_manager>("nstex3d");
	register_resource<nstex_cubemap, nstex_manager>("nstex_cubemap");
	register_resource<nsshader, nsshader_manager>("nsshader");
	register_resource<nsfont, nsfont_manager>("nsfont");
	register_resource<nsinput_map, nsinput_map_manager>("nsinput_map");
}

nsengine & nsengine::inst()
{
	static nsengine eng;
	return eng;
}

uint32 hash_id(const nsstring & str)
{
	return crc32(str.c_str(),static_cast<int32>(str.size()),0);
}

nsresource * get_resource(uint32 res_type, const uivec2 & res_id)
{
	if (res_type == type_to_hash(nsplugin))
		return nsep.get(res_id.y);

	nsplugin * plg = nsep.get(res_id.x);
	if (plg == nullptr)
		return nullptr;
	
	return plg->get(res_type, res_id.y);	
}
