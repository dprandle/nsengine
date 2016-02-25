/*!
\file nsengine.cpp

\brief Definition file for nsengine class

This file contains all of the neccessary definitions for the nsengine class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

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
#include <nsgl_context.h>
#include <nsfb_object.h>
#include <nsanim_comp.h>
#include <nslight_comp.h>
#include <nscam_comp.h>
#include <nsrender_comp.h>
#include <nsui_comp.h>
#include <nsfont_manager.h>

#ifdef NSDEBUG
#include <nsdebug.h>
#endif

nsengine::nsengine()
{
	srand(static_cast <unsigned> (time(0)));
    ilInit();
	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
	m_cwd = nsfile_os::cwd();
	nsfile_os::create_dir(m_cwd + "logs/blabla.txt");
}

nsengine::~nsengine()
{
}

nsplugin * nsengine::active()
{
	return current()->plugins->active();
}

bool nsengine::add_plugin(nsplugin * plugin)
{
	if (plugin == NULL)
		return false;

	return current()->plugins->add(plugin);
}

void nsengine::set_current_scene(const nsstring & scn, bool newSceneOverwriteFile, bool saveprevious)
{
	if (active() == NULL)
    {
        dprint("nsengine::set_current_scene No active plugin set");
		return;
    }
	manager<nsscene_manager>(active())->set_current(scn, newSceneOverwriteFile, saveprevious);
}

void nsengine::set_current_scene(nsscene * scn, bool newSceneOverwriteFile, bool saveprevious)
{
	if (active() == NULL)
		return;
	manager<nsscene_manager>(active())->set_current(scn, newSceneOverwriteFile, saveprevious);
}

void nsengine::set_current_scene(uint32 scn, bool newSceneOverwriteFile, bool saveprevious)
{
	if (active() == NULL)
		return;
	manager<nsscene_manager>(active())->set_current(scn, newSceneOverwriteFile, saveprevious);
}

void nsengine::set_active(const nsstring & plugname)
{
	plugins()->set_active(plugname);
}

void nsengine::set_active(nsplugin * plug)
{
	plugins()->set_active(plug);
}

void nsengine::set_active(uint32 plugid)
{
	plugins()->set_active(plugid);
}

bool nsengine::add_system(nssystem * pSystem)
{
	if (pSystem == NULL)
		return false;

	gl_ctxt * gc = current();
	std::type_index ti(typeid(*pSystem));
	uint32 type_hash = nse.type_id(ti);

	if (type_hash == 0)
	{
		dprint(nsstring("nsengine::addSystem - Could not find type id for type: ") + ti.name());
		return false;
	}
	auto it = gc->systems->emplace(type_hash, pSystem);

	if (!it.second)
		return false;

	m_sys_update_order[pSystem->update_priority()] = type_hash;
	return true;
}

nsresource * nsengine::resource(uint32 res_typeid, const uivec2 & resid)
{
	return resource(res_typeid, plugin(resid.x), resid.y);
}

nsresource * nsengine::resource(uint32 res_typeid, nsplugin * plg, uint32 resid)
{
	if (plg == NULL)
		return NULL;
	return plg->get(res_typeid, resid);
}

nsresource * nsengine::resource(uint32 res_typeid, nsplugin * plg, const nsstring & resname)
{
	if (plg == NULL)
		return NULL;
	return plg->get(res_typeid, resname);
}

nsplugin * nsengine::load_plugin(const nsstring & fname)
{
	return current()->plugins->load(fname);
}

void nsengine::save_plugins(bool saveOwnedResources, nssave_resouces_callback * scallback)
{
	current()->plugins->save_all("", scallback);

	if (saveOwnedResources)
	{
		auto plugiter = current()->plugins->begin();
		while (plugiter != current()->plugins->end())
		{
			nsplugin * plg = plugin(plugiter->first);
			plg->save_all("",scallback);
			++plugiter;
		}
	}
}

bool nsengine::save_plugin(nsplugin * plg, bool saveOwnedResources,  nssave_resouces_callback * scallback)
{
	if (plg == NULL)
		return false;
	
	bool ret = current()->plugins->save(plg);
	if (saveOwnedResources)
		plg->save_all("",scallback);
	return ret;
}

void nsengine::save_core(nssave_resouces_callback * scallback)
{
	core()->save_all("", scallback);
}

bool nsengine::_destroy_factory(uint32 hashid)
{
	nsfactory * fac = _remove_factory(hashid);
	if (fac == NULL)
		return false;
	delete fac;
	return true;
}

nsres_manager * nsengine::manager(uint32 manager_typeid, nsplugin * plg)
{
	return plg->manager(manager_typeid);
}

nsres_manager * manager(const nsstring & manager_guid, nsplugin * plg)
{
	return plg->manager(manager_guid);		
}

bool nsengine::del_plugin(nsplugin * plg)
{
	return current()->plugins->del(plg);
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
	return current()->systems->begin();
}

nsplugin * nsengine::create_plugin(const nsstring & plugname, bool makeactive)
{
	nsplugin * plug = current()->plugins->create(plugname);
	if (plug != NULL && makeactive)
		current()->plugins->set_active(plug);
	return plug;
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

nsscene * nsengine::current_scene()
{
	nsplugin * plug = active();
	if (plug == NULL)
		return NULL;
	return plug->current_scene();
}

hash_factory_map::iterator nsengine::end_factory()
{
	return m_factories.end();
}

system_hash_map::iterator nsengine::end_system()
{
	gl_ctxt * gc = current();
	return current()->systems->end();
}

nstimer * nsengine::timer()
{
	return current()->timer;
}

#ifdef NSDEBUG
void nsengine::debug_print(const nsstring & str)
{
	current()->deb->print("context-" + std::to_string(m_current_context) + " " + str);
}
#endif

nsevent_dispatcher * nsengine::event_dispatch()
{
	return current()->event_disp;
}

nssystem * nsengine::system(uint32 type_id)
{
	gl_ctxt * cont = current();
	auto iter = cont->systems->find(type_id);
	if (iter == cont->systems->end())
		return NULL;
	return iter->second;
}

bool nsengine::has_plugin(nsplugin * plg)
{
	return current()->plugins->contains(plg);
}

nssystem * nsengine::system(const nsstring & guid_)
{
	return system(hash_id(guid_));
}

const nsstring & nsengine::res_dir()
{
	return m_res_dir;
}

bool nsengine::has_system(uint32 type_id)
{
	gl_ctxt * cont = current();
	return (cont->systems->find(type_id) != cont->systems->end());
}

bool nsengine::has_system(const nsstring & guid_)
{
	return has_system(hash_id(guid_));
}

/*!
Overload of Propagate name change
*/
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

uint32 nsengine::create_context(bool add)
{
	static uint32 id = 1; // forever increasing id
	auto iter = m_contexts.emplace(id, new gl_ctxt(id));
	if (!iter.second)
		return -1;
	m_current_context = id;
	if (add)
		_init_factories();
	return id++;
}

nsplugin * nsengine::core()
{
	return current()->core_plugin;
}

nsplugin_manager * nsengine::plugins()
{
	return current()->plugins;
}

nssystem * nsengine::remove_system(uint32 type_id)
{
	nssystem * sys = system(type_id);
	if (sys == NULL)
		return NULL;
	current()->systems->erase(type_id);
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
	core()->set_import_dir(m_import_dir);
	auto iter = m_contexts.begin();
	while (iter != m_contexts.end())
	{
		auto iter2 = iter->second->plugins->begin();
		while (iter2 != iter->second->plugins->end())
		{
			nsplugin * plug = static_cast<nsplugin*>(iter2->second);
			plug->set_import_dir(m_import_dir);
			++iter2;
		}
		++iter;
	}
}

void nsengine::set_res_dir(const nsstring & dir)
{
	m_res_dir = dir;
	auto iter = m_contexts.begin();
	while (iter != m_contexts.end())
	{
		iter->second->plugins->set_res_dir(dir);
		++iter;
	}
}

void nsengine::start(const ivec2 & screen_size)
{
	nsfile_os::platform_init();
	if (current() == NULL)
		return;

    glewExperimental = true;

	// Initialize the glew extensions - if this fails we want a crash because there is nothing
	// useful the program can do without these initialized
	GLenum cont = glewInit();
	if (cont != GLEW_OK)
	{
		nslog_file("GLEW extensions unable to initialize");
		return;
	}
	set_res_dir(m_cwd + nsstring(DEFAULT_RESOURCE_DIR));
	set_import_dir(m_cwd + nsstring(DEFAULT_IMPORT_DIR));
	set_plugin_dir(m_cwd + nsstring(LOCAL_PLUGIN_DIR_DEFAULT));
	

	nsplugin * plg = core();	
	plg->init();
	plg->bind();
	plg->set_res_dir(m_cwd + DEFAULT_CORE_DIR);
	plg->add_name_to_res_path(false);
	
	_init_systems();
	timer()->start();
}

void nsengine::shutdown()
{
	auto iter = current()->systems->begin();
	while (iter != current()->systems->end())
	{
		delete iter->second;
		iter->second = NULL;
		++iter;
	}
	current()->systems->clear();
	core()->unbind();
}

nsplugin * nsengine::plugin(const nsstring & name)
{
	if (name == core()->name())
		return core();
	return current()->plugins->get(name);
}

nsplugin * nsengine::plugin(uint32 id)
{
	if (id == core()->id())
		return core();
	return current()->plugins->get(id);
}

nsplugin * nsengine::plugin(nsplugin * plg)
{
	if (plg == core())
		return core();
	return current()->plugins->get(plg);
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
	system<nsrender_system>()->render();
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

void nsengine::_init_systems()
{
	auto fiter = nse.begin_factory();
	while (fiter != nse.end_factory())
	{
		if (fiter->second->type() == nsfactory::f_system)
			create_system(guid(fiter->first));
		++fiter;
	}
}

bool nsengine::make_current(uint32 cID)
{
	auto fIter = m_contexts.find(cID);
	if (fIter == m_contexts.end())
		return false;
	m_current_context = cID;
	return true;
}

gl_ctxt * nsengine::current()
{
	auto fIter = m_contexts.find(m_current_context);
	if (fIter != m_contexts.end())
		return (fIter->second);
	return NULL;
}

uint32 nsengine::current_id()
{
	return m_current_context;
}

bool nsengine::destroy_plugin(nsplugin * plug)
{
	return current()->plugins->destroy(plug);
}

#ifdef NSDEBUG
nsdebug * nsengine::debug()
{
	return current()->deb;
}
#endif

bool nsengine::destroy_context(uint32 cID)
{
	auto fiter = m_contexts.find(cID);
	if (fiter == m_contexts.end())
		return false;
	delete fiter->second; fiter->second = NULL;
	m_contexts.erase(fiter);

	if (m_current_context == cID)
		m_current_context = -1;

	return true;
}

void nsengine::set_plugin_dir(const nsstring & plugdir)
{
	current()->plugins->set_plugin_dir(plugdir);
}

const nsstring & nsengine::plugin_dir()
{
	return current()->plugins->plugin_dir();
}

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

nsplugin * nsengine::remove_plugin(nsplugin * plg)
{
	return current()->plugins->remove(plg);
}

void nsengine::_init_factories()
{
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
	register_component<nsui_comp>("nsui_comp");
	
	register_system<nsanim_system>("nsanim_system");
	register_system<nsbuild_system>("nsbuild_system");
	register_system<nscamera_system>("nscamera_system");
	register_system<nsinput_system>("nsinput_system");
	register_system<nsparticle_system>("nsparticle_system");
	register_system<nsrender_system>("nsrender_system");
	register_system<nsselection_system>("nsselection_system");
	
	
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
	register_resource<nstex1d_array, nstex_manager>("nstex1d_array");
	register_resource<nstex2d, nstex_manager>("nstex2d");
	register_resource<nstex2d_array, nstex_manager>("nstex2d_array");
	register_resource<nstex3d, nstex_manager>("nstex3d");
	register_resource<nstex_rectangle, nstex_manager>("nstex_rectangle");
	register_resource<nstex_buffer, nstex_manager>("nstex_buffer");
	register_resource<nstex_cubemap, nstex_manager>("nstex_cubemap");
	register_resource<nstex_cubemap_array, nstex_manager>("nstex_cubemap_array");
	register_resource<nstex2d_multisample, nstex_manager>("nstex2d_multisample");
	register_resource<nstex2d_multisample_array, nstex_manager>("nstex2d_multisample_array");

	register_resource<nsshader, nsshader_manager>("nsshader");
	register_resource<nsdir_light_shader, nsshader_manager>("nsdir_light_shader");
	register_resource<nsspot_light_shader, nsshader_manager>("nsspot_light_shader");
	register_resource<nspoint_light_shader, nsshader_manager>("nspoint_light_shader");
	register_resource<nsrender_shader, nsshader_manager>("nsrender_shader");
	register_resource<nsmaterial_shader, nsshader_manager>("nsmaterial_shader");
	register_resource<nsparticle_process_shader, nsshader_manager>("nsparticle_process_shader");
	register_resource<nsparticle_render_shader, nsshader_manager>("nsparticle_render_shader");
	register_resource<nsfragment_sort_shader, nsshader_manager>("nsfragment_sort_shader");
	register_resource<nsshadow_2dmap_shader, nsshader_manager>("nsshadow_2dmap_shader");
	register_resource<nsshadow_cubemap_shader, nsshader_manager>("nsshadow_cubemap_shader");
	register_resource<nslight_stencil_shader, nsshader_manager>("nslight_stencil_shader");
	register_resource<nsselection_shader, nsshader_manager>("nsselection_shader");

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
