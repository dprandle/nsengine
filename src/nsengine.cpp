/*!
\file nsengine.cpp

\brief Definition file for NSEngine class

This file contains all of the neccessary definitions for the NSEngine class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#include <hash/crc32.h>
#include <nsfile_os.h>
#include <nsglobal.h>
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
#include <nsmovement_system.h>
#include <nsterrain_comp.h>
#include <nsanim_system.h>
#include <nsinput_comp.h>
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

#ifdef NSDEBUG
#include <nsdebug.h>
#endif


NSEngine::NSEngine()
{
	srand(static_cast <unsigned> (time(0)));
    ilInit();
	GL_INVALID_ENUM;
	m_cwd = nsfile_os::cwd();	
}

NSEngine::~NSEngine()
{
}

nsplugin * NSEngine::active()
{
	return current()->plugins->active();
}

bool NSEngine::add_plugin(nsplugin * plugin)
{
	if (plugin == NULL)
		return false;

	return current()->plugins->add(plugin);
}

void NSEngine::set_current_scene(const nsstring & scn, bool newSceneOverwriteFile, bool saveprevious)
{
	if (active() == NULL)
		return;
	manager<nsscene_manager>(active())->set_current(scn, newSceneOverwriteFile, saveprevious);
}

void NSEngine::set_current_scene(nsscene * scn, bool newSceneOverwriteFile, bool saveprevious)
{
	if (active() == NULL)
		return;
	manager<nsscene_manager>(active())->set_current(scn, newSceneOverwriteFile, saveprevious);
}

void NSEngine::set_current_scene(uint32 scn, bool newSceneOverwriteFile, bool saveprevious)
{
	if (active() == NULL)
		return;
	manager<nsscene_manager>(active())->set_current(scn, newSceneOverwriteFile, saveprevious);
}

void NSEngine::set_active(const nsstring & plugname)
{
	plugins()->set_active(plugname);
}

void NSEngine::set_active(nsplugin * plug)
{
	plugins()->set_active(plug);
}

void NSEngine::set_active(uint32 plugid)
{
	plugins()->set_active(plugid);
}

bool NSEngine::add_system(nssystem * pSystem)
{
	if (pSystem == NULL)
		return false;

	gl_ctxt * gc = current();
	std::type_index ti(typeid(*pSystem));
	uint32 type_hash = nsengine.type_id(ti);

	if (type_hash == 0)
	{
		dprint(nsstring("NSEngine::addSystem - Could not find type id for type: ") + ti.name());
		return false;
	}
	auto it = gc->systems->emplace(type_hash, pSystem);

	if (!it.second)
		return false;

	int32 drawPriority = pSystem->draw_priority();

	// Only add systems with a draw priority
	if (drawPriority != NO_DRAW_PR)
		m_sys_draw_order[drawPriority] = type_hash;

	m_sys_update_order[pSystem->update_priority()] = type_hash;
	return true;
}

nsresource * NSEngine::resource(uint32 res_typeid, nsplugin * plg, uint32 resid)
{
	if (plg == NULL)
		return NULL;
	return plg->get(res_typeid, resid);
}

nsresource * NSEngine::resource(uint32 res_typeid, nsplugin * plg, const nsstring & resname)
{
	if (plg == NULL)
		return NULL;
	return plg->get(res_typeid, resname);
}

uint32 NSEngine::create_framebuffer()
{
	nsfb_object * fb = new nsfb_object();
	fb->init_gl();
	current()->fb_map.emplace(fb->gl_id(), fb);
	return fb->gl_id();
}

bool NSEngine::del_framebuffer(uint32 fbid)
{
	nsfb_object * obj = framebuffer(fbid);
	if (obj == NULL)
		return false;
	obj->release();
	delete obj;
	current()->fb_map.erase(fbid);
	return true;
}

nsfb_object * NSEngine::framebuffer(uint32 id)
{
	auto iter = current()->fb_map.find(id);
	if (iter != current()->fb_map.end())
		return iter->second;
	return NULL;
}

nsplugin * NSEngine::load_plugin(const nsstring & fname)
{
	return current()->plugins->load(fname);
}

void NSEngine::save_plugins(bool saveOwnedResources, nssave_resouces_callback * scallback)
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

bool NSEngine::save_plugin(nsplugin * plg, bool saveOwnedResources,  nssave_resouces_callback * scallback)
{
	if (plg == NULL)
		return false;
	
	bool ret = current()->plugins->save(plg);
	if (saveOwnedResources)
		plg->save_all("",scallback);
	return ret;
}

void NSEngine::save_core(nssave_resouces_callback * scallback)
{
	core()->save_all("", scallback);
}

bool NSEngine::_destroy_factory(uint32 hashid)
{
	nsfactory * fac = _remove_factory(hashid);
	if (fac == NULL)
		return false;
	delete fac;
	return true;
}

nsres_manager * NSEngine::manager(uint32 manager_typeid, nsplugin * plg)
{
	return plg->manager(manager_typeid);
}

nsres_manager * manager(const nsstring & manager_guid, nsplugin * plg)
{
	return plg->manager(manager_guid);		
}

bool NSEngine::del_plugin(nsplugin * plg)
{
	return current()->plugins->del(plg);
}

bool NSEngine::destroy_system(uint32 type_id)
{
	nssystem * sys = remove_system(type_id);
	if (sys == NULL)
		return false;
	delete sys;
	return true;
}

bool NSEngine::destroy_system(const nsstring & guid_)
{
	return destroy_system(hash_id(guid_));
}

hash_factory_map::iterator NSEngine::begin_factory()
{
	return m_factories.begin();
}

system_hash_map::iterator NSEngine::begin_system()
{
	return current()->systems->begin();
}

nsplugin * NSEngine::create_plugin(const nsstring & plugname, bool makeactive)
{
	nsplugin * plug = current()->plugins->create(plugname);
	if (plug != NULL && makeactive)
		current()->plugins->set_active(plug);
	return plug;
}

nssystem * NSEngine::create_system(uint32 type_id)
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

nssystem * NSEngine::create_system(const nsstring & guid_)
{
	return create_system(hash_id(guid_));
}

nsscene * NSEngine::current_scene()
{
	nsplugin * plug = active();
	if (plug == NULL)
		return NULL;
	return plug->current_scene();
}

hash_factory_map::iterator NSEngine::end_factory()
{
	return m_factories.end();
}

system_hash_map::iterator NSEngine::end_system()
{
	gl_ctxt * gc = current();
	return current()->systems->end();
}

nstimer * NSEngine::timer()
{
	return current()->timer;
}

#ifdef NSDEBUG
void NSEngine::debug_print(const nsstring & str)
{
	current()->deb->print(str);
}
#endif

nsevent_dispatcher * NSEngine::event_dispatch()
{
	return current()->event_disp;
}

nssystem * NSEngine::system(uint32 type_id)
{
	gl_ctxt * cont = current();
	auto iter = cont->systems->find(type_id);
	if (iter == cont->systems->end())
		return NULL;
	return iter->second;
}

bool NSEngine::has_plugin(nsplugin * plg)
{
	return current()->plugins->contains(plg);
}

nssystem * NSEngine::system(const nsstring & guid_)
{
	return system(hash_id(guid_));
}

const nsstring & NSEngine::res_dir()
{
	return m_res_dir;
}

bool NSEngine::has_system(uint32 type_id)
{
	gl_ctxt * cont = current();
	return (cont->systems->find(type_id) != cont->systems->end());
}

bool NSEngine::has_system(const nsstring & guid_)
{
	return has_system(hash_id(guid_));
}

/*!
Overload of Propagate name change
*/
void NSEngine::name_change(const uivec2 & oldid, const uivec2 newid)
{
	if (oldid.y == core()->id())
	{
		dprint("NSEngine::name_change Cannot change engine default plugin name");
		return;
	}
	if (oldid.x == core()->id())
		core()->name_change(oldid, newid);
	plugins()->name_change(oldid, newid);
}

uint32 NSEngine::create_context(bool add)
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

nsplugin * NSEngine::core()
{
	return current()->core_plugin;
}

nsplugin_manager * NSEngine::plugins()
{
	return current()->plugins;
}

nssystem * NSEngine::remove_system(uint32 type_id)
{
	nssystem * sys = system(type_id);
	if (sys == NULL)
		return NULL;
	current()->systems->erase(type_id);
	_remove_sys(type_id);
	return sys;
}

nssystem * NSEngine::remove_system(const nsstring & guid_)
{
	return remove_system(hash_id(guid_));
}


const nsstring & NSEngine::import_dir()
{
	return m_import_dir;
}

void NSEngine::set_import_dir(const nsstring & dir)
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

void NSEngine::set_res_dir(const nsstring & dir)
{
	m_res_dir = dir;
	auto iter = m_contexts.begin();
	while (iter != m_contexts.end())
	{
		iter->second->plugins->set_res_dir(dir);
		++iter;
	}
}

void NSEngine::start()
{
	if (current() == NULL)
		return;

    glewExperimental = true;

	// Initialize the glew extensions - if this fails we want a crash because there is nothing
	// useful the program can do without these initialized
	GLenum cont = glewInit();
	if (cont != GLEW_OK)
	{
		NSLogFile("GLEW extensions unable to initialize");
		return;
	}

	set_res_dir(m_cwd + nsstring(DEFAULT_RESOURCE_DIR));
	set_import_dir(m_cwd + nsstring(DEFAULT_IMPORT_DIR));
	set_plugin_dir(m_cwd + nsstring(LOCAL_PLUGIN_DIR_DEFAULT));

	nsplugin * plg = core();
	
	plg->init();
	plg->bind();
	plg->set_res_dir(m_cwd + nsstring("core/"));
	plg->add_name_to_res_path(false);
	
	current()->composite_buf = create_framebuffer();
	_init_systems();
	_init_shaders();
	_init_materials();
	_init_meshes();
	_init_entities();
	//_initInputMaps();
	timer()->start();
}

void NSEngine::shutdown()
{
	auto iter = current()->systems->begin();
	while (iter != current()->systems->end())
	{
		delete iter->second;
		iter->second = NULL;
		++iter;
	}
	current()->systems->clear();

	auto iterfb = current()->fb_map.begin();
	while (iterfb != current()->fb_map.end())
	{
		iterfb->second->release();
		delete iterfb->second;
		iterfb->second = NULL;
		++iterfb;
	}
	current()->fb_map.clear();

	core()->unbind();
}

nsplugin * NSEngine::plugin(const nsstring & name)
{
	if (name == core()->name())
		return core();
	return current()->plugins->get(name);
}

nsplugin * NSEngine::plugin(uint32 id)
{
	if (id == core()->id())
		return core();
	return current()->plugins->get(id);
}

nsplugin * NSEngine::plugin(nsplugin * plg)
{
	if (plg == core())
		return core();
	return current()->plugins->get(plg);
}

nsfactory * NSEngine::factory(uint32 hash_id)
{
	auto fIter = m_factories.find(hash_id);
	if (fIter == m_factories.end())
		return NULL;
	return fIter->second;
}

nsstring NSEngine::guid(std::type_index type)
{
	auto fiter = m_obj_type_hashes.find(type);
	if (fiter == m_obj_type_hashes.end())
		return "";
	return guid(fiter->second);
}

nsstring NSEngine::guid(uint32 hash)
{
	auto fiter = m_obj_type_names.find(hash);
	if (fiter != m_obj_type_names.end())
		return fiter->second;
	return "";
}

uint32 NSEngine::type_id(std::type_index type)
{
	auto fiter = m_obj_type_hashes.find(type);
	if (fiter != m_obj_type_hashes.end())
		return fiter->second;
	return 0;
}

nsfactory * NSEngine::_remove_factory(uint32 hash_id)
{
	nsfactory * f = factory(hash_id);
	if (f == NULL)
		return NULL;
	m_factories.erase(hash_id);
	return f;
}

void NSEngine::update()
{
	timer()->update();
	
	while (timer()->lag() >= timer()->fixed())
	{
		// Go through each system and update
		auto sysUpdateIter = m_sys_update_order.begin();
		while (sysUpdateIter != m_sys_update_order.end())
		{
			nssystem * sys = system(sysUpdateIter->second);
			event_dispatch()->process(sys);
			sys->update();
			++sysUpdateIter;
		}
		timer()->lag() -= timer()->fixed();
	}

	// Go through each system and draw
	auto sysDrawIter = m_sys_draw_order.begin();
	while (sysDrawIter != m_sys_draw_order.end())
	{
		nssystem * sys = system(sysDrawIter->second);
		sys->draw();
		++sysDrawIter;
	}
	system<nsrender_system>()->blit_final_frame();
}

void NSEngine::_init_input_maps()
{
	nsinput_map * inmap = core()->load<nsinput_map>(DEFAULT_ENGINE_INPUT);
	system<nsinput_system>()->set_input_map(inmap->full_id());
	system<nsinput_system>()->push_context(DEFAULT_INPUT_CONTEXT);	
}

void NSEngine::_init_shaders()
{
	nsshader_manager * mShaders = core()->manager<nsshader_manager>();
	nsrender_system::RenderShaders renShaders;
	nsstring shext = nsstring(DEFAULT_SHADER_EXTENSION);
	renShaders.deflt = mShaders->load<nsmaterial_shader>(nsstring(DEFAULT_GBUFFER_SHADER) + shext);
	renShaders.early_z = mShaders->load<nsearlyz_shader>(nsstring(DEFAULT_EARLYZ_SHADER) + shext);
	renShaders.light_stencil = mShaders->load<nslight_stencil_shader>(nsstring(DEFAULT_LIGHTSTENCIL_SHADER) + shext);
	renShaders.dir_light = mShaders->load<nsdir_light_shader>(nsstring(DEFAULT_DIRLIGHT_SHADER) + shext);
	renShaders.point_light = mShaders->load<nspoint_light_shader>(nsstring(DEFAULT_POINTLIGHT_SHADER) + shext);
	renShaders.spot_light = mShaders->load<nsspot_light_shader>(nsstring(DEFAULT_SPOTLIGHT_SHADER) + shext);
	renShaders.point_shadow = mShaders->load<nspoint_shadowmap_shader>(nsstring(DEFAULT_POINTSHADOWMAP_SHADER) + shext);
	renShaders.spot_shadow = mShaders->load<nsspot_shadowmap_shader>(nsstring(DEFAULT_SPOTSHADOWMAP_SHADER) + shext);
	renShaders.dir_shadow = mShaders->load<nsdir_shadowmap_shader>(nsstring(DEFAULT_DIRSHADOWMAP_SHADER) + shext);
	renShaders.xfb_default = mShaders->load<nsxfb_shader>(nsstring(DEFAULT_XFBGBUFFER_SHADER) + shext);
	renShaders.xfb_render = mShaders->load<nsrender_xfb_shader>(nsstring(DEFAULT_XFBGBUFFER_RENDER_SHADER) + shext);
	renShaders.xfb_earlyz = mShaders->load<nsearlyz_xfb_shader>(nsstring(DEFAULT_XFBEARLYZ_SHADER) + shext);
	renShaders.xfb_dir_shadow = mShaders->load<nsdir_shadowmap_xfb_shader>(nsstring(DEFAULT_XFBDIRSHADOWMAP_SHADER) + shext);
	renShaders.xfb_point_shadow = mShaders->load<nspoint_shadowmap_xfb_shader>(nsstring(DEFAULT_XFBPOINTSHADOWMAP_SHADER) + shext);
	renShaders.xfb_spot_shadow = mShaders->load<nsspot_shadowmap_xfb_shader>(nsstring(DEFAULT_XFBSPOTSHADOWMAP_SHADER) + shext);
	nsparticle_process_shader * xfsparticle = mShaders->load<nsparticle_process_shader>(nsstring(DEFAULT_PROCESS_PARTICLE_SHADER) + shext);
	nsparticle_render_shader * renderparticle = mShaders->load<nsparticle_render_shader>(nsstring(DEFAULT_RENDER_PARTICLE_SHADER) + shext);
	nsselection_shader * selshader = mShaders->load<nsselection_shader>(nsstring(DEFAULT_SELECTION_SHADER) + shext);
	nsskybox_shader * skysh = mShaders->load<nsskybox_shader>(nsstring(DEFAULT_SKYBOX_SHADER) + shext);
	system<nsrender_system>()->set_shaders(renShaders);
	system<nsselection_system>()->set_shader(selshader);
	system<nsparticle_system>()->set_process_shader(xfsparticle);
	mShaders->compile_all();
	mShaders->link_all();
	mShaders->init_uniforms_all();
}

void NSEngine::_init_entities()
{
	nsentity * objBrush = core()->create<nsentity>(ENT_OBJECT_BRUSH);
	NSSelComp * sc = objBrush->create<NSSelComp>();
	sc->setDefaultColor(fvec4(0.0f, 1.0f, 0.0f, 1.0f));
	sc->setColor(fvec4(0.0f, 1.0f, 0.0f, 1.0f));
	sc->setMaskAlpha(0.2f);
	sc->enableDraw(true);
	sc->enableMove(true);
	NSInputComp * ic = objBrush->create<NSInputComp>();
	ic->add(DRAG_OBJECT_XY);
	ic->add(DRAG_OBJECT_XZ);
	ic->add(DRAG_OBJECT_YZ);
	ic->add(INSERT_OBJECT);
	ic->add(SHIFT_DONE);
	ic->add(XZ_MOVE_END);
	ic->add(YZ_MOVE_END);
	system<nsbuild_system>()->set_object_brush(objBrush);
}

void NSEngine::_remove_sys(uint32 type_id)
{
	auto iter1 = m_sys_draw_order.begin();
	while (iter1 != m_sys_draw_order.end())
	{
		if (iter1->second == type_id)
		{
			m_sys_draw_order.erase(iter1);
			return;
		}
		++iter1;
	}

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

void NSEngine::_init_materials()
{
	nstexture * tex = core()->load<nstex2d>(nsstring(DEFAULT_MATERIAL_DIFFUSE) + nsstring(DEFAULT_TEX_EXTENSION));
	nsmaterial * def = core()->load<nsmaterial>(nsstring(DEFAULT_MATERIAL_NAME) + nsstring(DEFAULT_MAT_EXTENSION));
	system<nsrender_system>()->set_default_mat(def);
}

void NSEngine::_init_meshes()
{
	nsmesh * msh = core()->load<nsmesh>(nsstring(MESH_FULL_TILE) + nsstring(DEFAULT_MESH_EXTENSION));
	//msh->bake_node_rotation(orientation(fvec4(1, 0, 0, -90.0f)));
	core()->load<nsmesh>(nsstring(MESH_TERRAIN) + nsstring(DEFAULT_MESH_EXTENSION));
	core()->load<nsmesh>(nsstring(MESH_HALF_TILE) + nsstring(DEFAULT_MESH_EXTENSION));
	core()->load<nsmesh>(nsstring(MESH_POINTLIGHT_BOUNDS) + nsstring(DEFAULT_MESH_EXTENSION));
	core()->load<nsmesh>(nsstring(MESH_SPOTLIGHT_BOUNDS) + nsstring(DEFAULT_MESH_EXTENSION));
	core()->load<nsmesh>(nsstring(MESH_DIRLIGHT_BOUNDS) + nsstring(DEFAULT_MESH_EXTENSION));
	core()->load<nsmesh>(nsstring(MESH_SKYDOME) + nsstring(DEFAULT_MESH_EXTENSION));
}

void NSEngine::_init_systems()
{
	auto fiter = nsengine.begin_factory();
	while (fiter != nsengine.end_factory())
	{
		if (fiter->second->type() == nsfactory::f_system)
			create_system(guid(fiter->first));
		++fiter;
	}
	system<nsrender_system>()->set_final_fbo(current()->composite_buf);
	system<nsselection_system>()->set_final_fbo(current()->composite_buf);
	system<nsselection_system>()->set_picking_fbo(system<nsrender_system>()->gbuffer_fbo());
	system<nsparticle_system>()->set_final_fbo(current()->composite_buf);
}

bool NSEngine::make_current(uint32 cID)
{
	auto fIter = m_contexts.find(cID);
	if (fIter == m_contexts.end())
		return false;
	m_current_context = cID;
	return true;
}

gl_ctxt * NSEngine::current()
{
	auto fIter = m_contexts.find(m_current_context);
	if (fIter != m_contexts.end())
		return (fIter->second);
	return NULL;
}

uint32 NSEngine::current_id()
{
	return m_current_context;
}

bool NSEngine::destroy_plugin(nsplugin * plug)
{
	return current()->plugins->destroy(plug);
}

#ifdef NSDEBUG
nsdebug * NSEngine::debug()
{
	return current()->deb;
}
#endif

bool NSEngine::destroy_context(uint32 cID)
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

void NSEngine::set_plugin_dir(const nsstring & plugdir)
{
	current()->plugins->set_plugin_dir(plugdir);
}

const nsstring & NSEngine::plugin_dir()
{
	return current()->plugins->plugin_dir();
}

uint32 NSEngine::manager_id(uint32 res_id)
{
	auto iter = m_res_manager_map.find(res_id);
	if (iter != m_res_manager_map.end())
		return iter->second;
	return 0;
}

uint32 NSEngine::manager_id(std::type_index res_type)
{
	uint32 hashed_type = type_id(res_type);
	return manager_id(hashed_type);
}

uint32 NSEngine::manager_id(const nsstring & res_guid)
{
	return manager_id(hash_id(res_guid));
}

const nsstring & NSEngine::cwd()
{
	return m_cwd;
}

nsplugin * NSEngine::remove_plugin(nsplugin * plg)
{
	return current()->plugins->remove(plg);
}

nsresource * NSEngine::_resource(uint32 restype_id, const uivec2 & resid)
{
	nsplugin * plg = plugin(resid.x);
	if (plg == NULL)
		return NULL;
	return plg->get(restype_id, resid.y);
}

void NSEngine::_init_factories()
{
	register_component<NSAnimComp>("NSAnimComp");
	register_component<nscam_comp>("nscam_comp");
	register_component<NSInputComp>("NSInputComp");
	register_component<NSLightComp>("NSLightComp");
	register_component<NSOccupyComp>("NSOccupyComp");
	register_component<NSParticleComp>("NSParticleComp");
	register_component<NSRenderComp>("NSRenderComp");
	register_component<NSSelComp>("NSSelComp");
	register_component<NSTFormComp>("NSTFormComp");
	register_component<NSTileBrushComp>("NSTileBrushComp");
	register_component<NSTileComp>("NSTileComp");
	register_component<NSTerrainComp>("NSTerrainComp");

	register_system<nsanim_system>("nsanim_system");
	register_system<nsbuild_system>("nsbuild_system");
	register_system<nscamera_system>("nscamera_system");
	register_system<nsinput_system>("nsinput_system");
	register_system<nsmovement_system>("nsmovement_system");
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
	
	register_resource<nsanim_set, nsanim_manager>("nsanim_set");
	register_resource<nsentity, nsentity_manager>("nsentity");
	register_resource<nsmaterial, nsmat_manager>("nsmaterial");
	register_resource<nsmesh, nsmesh_manager>("nsmesh");
	register_resource<nsplugin, nsplugin_manager>("nsplugin");
	register_resource<nsscene, nsscene_manager>("nsscene");

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
	register_resource<nsmaterial_shader, nsshader_manager>("nsmaterial_shader");
	register_resource<nsparticle_process_shader, nsshader_manager>("nsparticle_process_shader");
	register_resource<nsparticle_render_shader, nsshader_manager>("nsparticle_render_shader");
	register_resource<nsdir_shadowmap_shader, nsshader_manager>("nsdir_shadowmap_shader");
	register_resource<nspoint_shadowmap_shader, nsshader_manager>("nspoint_shadowmap_shader");
	register_resource<nsspot_shadowmap_shader, nsshader_manager>("nsspot_shadowmap_shader");
	register_resource<nsearlyz_shader, nsshader_manager>("nsearlyz_shader");
	register_resource<nsdir_shadowmap_xfb_shader, nsshader_manager>("nsdir_shadowmap_xfb_shader");
	register_resource<nspoint_shadowmap_xfb_shader, nsshader_manager>("nspoint_shadowmap_xfb_shader");
	register_resource<nsspot_shadowmap_xfb_shader, nsshader_manager>("nsspot_shadowmap_xfb_shader");
	register_resource<nsearlyz_xfb_shader, nsshader_manager>("nsearlyz_xfb_shader");
	register_resource<nsrender_xfb_shader, nsshader_manager>("nsrender_xfb_shader");
	register_resource<nsxfb_shader, nsshader_manager>("nsxfb_shader");
	register_resource<nslight_stencil_shader, nsshader_manager>("nslight_stencil_shader");
	register_resource<nsskybox_shader, nsshader_manager>("nsskybox_shader");
	register_resource<nstransparency_shader, nsshader_manager>("nstransparency_shader");
	register_resource<nsselection_shader, nsshader_manager>("nsselection_shader");
	register_resource<nsinput_map, nsinput_map_manager>("nsinput_map");
}

gl_ctxt::gl_ctxt(uint32 id) :
	glew_context(new GLEWContext()),
	core_plugin(new nsplugin()),
	systems(new system_hash_map()),
	plugins(new nsplugin_manager()),
	event_disp(new nsevent_dispatcher()),
	fb_map(),
	timer(new nstimer()),
	composite_buf(0),
	context_id(id)
#ifdef NSDEBUG
	,deb(new nsdebug())
#endif
{
	core_plugin->rename(ENGINE_PLUG);
#ifdef NSDEBUG
	deb->set_log_file("engine_ctxt" + std::to_string(context_id) + ".log");
	deb->set_log_dir(nsengine.cwd() + "logs");
	deb->clear_log();
#endif
}

gl_ctxt::~gl_ctxt()
{
	delete plugins;
	delete systems;
	delete core_plugin;
	delete event_disp;
	delete glew_context;
	delete timer;
#ifdef NSDEBUG
	delete deb;
#endif
}

NSEngine & NSEngine::inst()
{
	static NSEngine eng;
	return eng;
}

GLEWContext * glewGetContext()
{
	return nsengine.current()->glew_context;
}

bool GLError(nsstring errorMessage)
{
	GLenum err = glGetError();
	if (err != GL_NO_ERROR)
	{
#ifdef NSDEBUG
		nsstringstream ss;
		ss << errorMessage << ": OpenGL Error Code : " << err;
		dprint(ss.str());
#endif
		return true;
	}
	return false;
}

uint32 hash_id(const nsstring & str)
{
	return crc32(str.c_str(),static_cast<int32>(str.size()),0);
}
