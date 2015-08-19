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
#include <IL/ilu.h>
#include <IL/ilut.h>
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
	mCwd = nsfileio::cwd();	
}

NSEngine::~NSEngine()
{
}

nsplugin * NSEngine::active()
{
	return current()->plugins->active();
}

bool NSEngine::addPlugin(nsplugin * plugin)
{
	if (plugin == NULL)
		return false;

	return current()->plugins->add(plugin);
}

void NSEngine::setCurrentScene(const nsstring & scn, bool newSceneOverwriteFile, bool saveprevious)
{
	if (active() == NULL)
		return;
	manager<nsscene_manager>(active())->set_current(scn, newSceneOverwriteFile, saveprevious);
}

void NSEngine::setCurrentScene(nsscene * scn, bool newSceneOverwriteFile, bool saveprevious)
{
	if (active() == NULL)
		return;
	manager<nsscene_manager>(active())->set_current(scn, newSceneOverwriteFile, saveprevious);
}

void NSEngine::setCurrentScene(uint32 scn, bool newSceneOverwriteFile, bool saveprevious)
{
	if (active() == NULL)
		return;
	manager<nsscene_manager>(active())->set_current(scn, newSceneOverwriteFile, saveprevious);
}

void NSEngine::setActive(const nsstring & plugname)
{
	plugins()->set_active(plugname);
}

void NSEngine::setActive(nsplugin * plug)
{
	plugins()->set_active(plug);
}

void NSEngine::setActive(uint32 plugid)
{
	plugins()->set_active(plugid);
}

bool NSEngine::addSystem(nssystem * pSystem)
{
	if (pSystem == NULL)
		return false;

	GLContext * gc = current();
	std::type_index ti(typeid(*pSystem));
	uint32 type_hash = nsengine.typeID(ti);

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
		mSystemDrawOrder[drawPriority] = type_hash;

	mSystemUpdateOrder[pSystem->update_priority()] = type_hash;
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

uint32 NSEngine::createFramebuffer()
{
	nsfb_object * fb = new nsfb_object();
	fb->init_gl();
	current()->fbmap.emplace(fb->gl_id(), fb);
	return fb->gl_id();
}

bool NSEngine::delFramebuffer(uint32 fbid)
{
	nsfb_object * obj = framebuffer(fbid);
	if (obj == NULL)
		return false;
	obj->release();
	delete obj;
	current()->fbmap.erase(fbid);
	return true;
}

nsfb_object * NSEngine::framebuffer(uint32 id)
{
	auto iter = current()->fbmap.find(id);
	if (iter != current()->fbmap.end())
		return iter->second;
	return NULL;
}

nsplugin * NSEngine::loadPlugin(const nsstring & fname)
{
	return current()->plugins->load(fname);
}

void NSEngine::savePlugins(bool saveOwnedResources, NSSaveResCallback * scallback)
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

bool NSEngine::savePlugin(nsplugin * plg, bool saveOwnedResources,  NSSaveResCallback * scallback)
{
	if (plg == NULL)
		return false;
	
	bool ret = current()->plugins->save(plg);
	if (saveOwnedResources)
		plg->save_all("",scallback);
	return ret;
}

void NSEngine::saveCore(NSSaveResCallback * scallback)
{
	engplug()->save_all("", scallback);
}

bool NSEngine::destroyFactory(uint32 hashid)
{
	NSFactory * fac = removeFactory(hashid);
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

bool NSEngine::delPlugin(nsplugin * plg)
{
	return current()->plugins->del(plg);
}

bool NSEngine::destroySystem(uint32 type_id)
{
	nssystem * sys = removeSystem(type_id);
	if (sys == NULL)
		return false;
	delete sys;
	return true;
}

bool NSEngine::destroySystem(const nsstring & guid_)
{
	return destroySystem(hash_id(guid_));
}

FactoryMap::iterator NSEngine::beginFac()
{
	return mFactories.begin();
}

SystemMap::iterator NSEngine::beginSys()
{
	return current()->systems->begin();
}

nsplugin * NSEngine::createPlugin(const nsstring & plugname, bool makeactive)
{
	nsplugin * plug = current()->plugins->create(plugname);
	if (plug != NULL && makeactive)
		current()->plugins->set_active(plug);
	return plug;
}

nssystem * NSEngine::createSystem(uint32 type_id)
{
	nssystem * system = factory<NSSysFactory>(type_id)->create();
	if (!addSystem(system))
	{
		delete system;
		return NULL;
	}
	system->init();
	return system;
}

nssystem * NSEngine::createSystem(const nsstring & guid_)
{
	return createSystem(hash_id(guid_));
}

nsscene * NSEngine::currentScene()
{
	nsplugin * plug = active();
	if (plug == NULL)
		return NULL;
	return plug->current_scene();
}

FactoryMap::iterator NSEngine::endFac()
{
	return mFactories.end();
}

SystemMap::iterator NSEngine::endSys()
{
	GLContext * gc = current();
	return current()->systems->end();
}

NSTimer * NSEngine::timer()
{
	return current()->timer;
}

#ifdef NSDEBUG
void NSEngine::debugPrint(const nsstring & str)
{
	current()->mDebug->print(str);
}
#endif

NSEventDispatcher * NSEngine::eventDispatch()
{
	return current()->mEvents;
}

nssystem * NSEngine::system(uint32 type_id)
{
	GLContext * cont = current();
	auto iter = cont->systems->find(type_id);
	if (iter == cont->systems->end())
		return NULL;
	return iter->second;
}

bool NSEngine::hasPlugin(nsplugin * plg)
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

bool NSEngine::hasSystem(uint32 type_id)
{
	GLContext * cont = current();
	return (cont->systems->find(type_id) != cont->systems->end());
}

bool NSEngine::hasSystem(const nsstring & guid_)
{
	return hasSystem(hash_id(guid_));
}

/*!
Overload of Propagate name change
*/
void NSEngine::name_change(const uivec2 & oldid, const uivec2 newid)
{
	if (oldid.y == engplug()->id())
	{
		dprint("NSEngine::name_change Cannot change engine default plugin name");
		return;
	}
	if (oldid.x == engplug()->id())
		engplug()->name_change(oldid, newid);
	plugins()->name_change(oldid, newid);
}

uint32 NSEngine::createContext(bool add)
{
	static uint32 id = 1; // forever increasing id
	auto iter = mContexts.emplace(id, new GLContext(id));
	if (!iter.second)
		return -1;
	mCurrentContext = id;
	if (add)
		_initDefaultFactories();
	return id++;
}

nsplugin * NSEngine::engplug()
{
	return current()->engplug;
}

nsplugin_manager * NSEngine::plugins()
{
	return current()->plugins;
}

nssystem * NSEngine::removeSystem(uint32 type_id)
{
	nssystem * sys = system(type_id);
	if (sys == NULL)
		return NULL;
	current()->systems->erase(type_id);
	_removeSys(type_id);
	return sys;
}

nssystem * NSEngine::removeSystem(const nsstring & guid_)
{
	return removeSystem(hash_id(guid_));
}


const nsstring & NSEngine::importdir()
{
	return mImportDirectory;
}

void NSEngine::setImportDir(const nsstring & dir)
{
	mImportDirectory = dir;
	engplug()->set_import_dir(mImportDirectory);
	auto iter = mContexts.begin();
	while (iter != mContexts.end())
	{
		auto iter2 = iter->second->plugins->begin();
		while (iter2 != iter->second->plugins->end())
		{
			nsplugin * plug = static_cast<nsplugin*>(iter2->second);
			plug->set_import_dir(mImportDirectory);
			++iter2;
		}
		++iter;
	}
}

void NSEngine::setResourceDir(const nsstring & dir)
{
	m_res_dir = dir;
	auto iter = mContexts.begin();
	while (iter != mContexts.end())
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

	setResourceDir(mCwd + nsstring(DEFAULT_RESOURCE_DIR));
	setImportDir(mCwd + nsstring(DEFAULT_IMPORT_DIR));
	setPluginDirectory(mCwd + nsstring(LOCAL_PLUGIN_DIR_DEFAULT));

	nsplugin * plg = engplug();
	
	plg->init();
	plg->bind();
	plg->set_res_dir(mCwd + nsstring("core/"));
	plg->add_name_to_res_path(false);
	
	current()->compositeBuf = createFramebuffer();
	_initSystems();
	_initShaders();
	_initMaterials();
	_initMeshes();
	_initEntities();
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

	auto iterfb = current()->fbmap.begin();
	while (iterfb != current()->fbmap.end())
	{
		iterfb->second->release();
		delete iterfb->second;
		iterfb->second = NULL;
		++iterfb;
	}
	current()->fbmap.clear();

	engplug()->unbind();
}

nsplugin * NSEngine::plugin(const nsstring & name)
{
	if (name == engplug()->name())
		return engplug();
	return current()->plugins->get(name);
}

nsplugin * NSEngine::plugin(uint32 id)
{
	if (id == engplug()->id())
		return engplug();
	return current()->plugins->get(id);
}

nsplugin * NSEngine::plugin(nsplugin * plg)
{
	if (plg == engplug())
		return engplug();
	return current()->plugins->get(plg);
}

NSFactory * NSEngine::factory(uint32 hash_id)
{
	auto fIter = mFactories.find(hash_id);
	if (fIter == mFactories.end())
		return NULL;
	return fIter->second;
}

nsstring NSEngine::guid(std::type_index type)
{
	auto fiter = mObjTypeHashes.find(type);
	if (fiter == mObjTypeHashes.end())
		return "";
	return guid(fiter->second);
}

nsstring NSEngine::guid(uint32 hash)
{
	auto fiter = mObjTypeNames.find(hash);
	if (fiter != mObjTypeNames.end())
		return fiter->second;
	return "";
}

uint32 NSEngine::typeID(std::type_index type)
{
	auto fiter = mObjTypeHashes.find(type);
	if (fiter != mObjTypeHashes.end())
		return fiter->second;
	return 0;
}

NSFactory * NSEngine::removeFactory(uint32 hash_id)
{
	NSFactory * f = factory(hash_id);
	if (f == NULL)
		return NULL;
	mFactories.erase(hash_id);
	return f;
}

void NSEngine::update()
{
	timer()->update();
	
	while (timer()->lag() >= timer()->fixed())
	{
		// Go through each system and update
		auto sysUpdateIter = mSystemUpdateOrder.begin();
		while (sysUpdateIter != mSystemUpdateOrder.end())
		{
			nssystem * sys = system(sysUpdateIter->second);
			eventDispatch()->process(sys);
			sys->update();
			++sysUpdateIter;
		}
		timer()->lag() -= timer()->fixed();
	}

	// Go through each system and draw
	auto sysDrawIter = mSystemDrawOrder.begin();
	while (sysDrawIter != mSystemDrawOrder.end())
	{
		nssystem * sys = system(sysDrawIter->second);
		sys->draw();
		++sysDrawIter;
	}
	system<nsrender_system>()->blit_final_frame();
}

void NSEngine::_initInputMaps()
{
	nsinput_map * inmap = engplug()->load<nsinput_map>(DEFAULT_ENGINE_INPUT);
	system<nsinput_system>()->set_input_map(inmap->full_id());
	system<nsinput_system>()->push_context(DEFAULT_INPUT_CONTEXT);	
}

void NSEngine::_initShaders()
{
	nsshader_manager * mShaders = engplug()->manager<nsshader_manager>();
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

void NSEngine::_initEntities()
{
	nsentity * objBrush = engplug()->create<nsentity>(ENT_OBJECT_BRUSH);
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

void NSEngine::_removeSys(uint32 type_id)
{
	auto iter1 = mSystemDrawOrder.begin();
	while (iter1 != mSystemDrawOrder.end())
	{
		if (iter1->second == type_id)
		{
			mSystemDrawOrder.erase(iter1);
			return;
		}
		++iter1;
	}

	auto iter2 = mSystemUpdateOrder.begin();
	while (iter2 != mSystemUpdateOrder.end())
	{
		if (iter2->second == type_id)
		{
			mSystemUpdateOrder.erase(iter2);
			return;
		}
		++iter2;
	}
}

void NSEngine::_initMaterials()
{
	nstexture * tex = engplug()->load<nstex2d>(nsstring(DEFAULT_MATERIAL_DIFFUSE) + nsstring(DEFAULT_TEX_EXTENSION));
	nsmaterial * def = engplug()->load<nsmaterial>(nsstring(DEFAULT_MATERIAL_NAME) + nsstring(DEFAULT_MAT_EXTENSION));
	system<nsrender_system>()->set_default_mat(def);
}

void NSEngine::_initMeshes()
{
	nsmesh * msh = engplug()->load<nsmesh>(nsstring(MESH_FULL_TILE) + nsstring(DEFAULT_MESH_EXTENSION));
	//msh->bake_node_rotation(orientation(fvec4(1, 0, 0, -90.0f)));
	engplug()->load<nsmesh>(nsstring(MESH_TERRAIN) + nsstring(DEFAULT_MESH_EXTENSION));
	engplug()->load<nsmesh>(nsstring(MESH_HALF_TILE) + nsstring(DEFAULT_MESH_EXTENSION));
	engplug()->load<nsmesh>(nsstring(MESH_POINTLIGHT_BOUNDS) + nsstring(DEFAULT_MESH_EXTENSION));
	engplug()->load<nsmesh>(nsstring(MESH_SPOTLIGHT_BOUNDS) + nsstring(DEFAULT_MESH_EXTENSION));
	engplug()->load<nsmesh>(nsstring(MESH_DIRLIGHT_BOUNDS) + nsstring(DEFAULT_MESH_EXTENSION));
	engplug()->load<nsmesh>(nsstring(MESH_SKYDOME) + nsstring(DEFAULT_MESH_EXTENSION));
}

void NSEngine::_initSystems()
{
	auto fiter = nsengine.beginFac();
	while (fiter != nsengine.endFac())
	{
		if (fiter->second->type() == NSFactory::System)
			createSystem(guid(fiter->first));
		++fiter;
	}
	system<nsrender_system>()->set_final_fbo(current()->compositeBuf);
	system<nsselection_system>()->set_final_fbo(current()->compositeBuf);
	system<nsselection_system>()->set_picking_fbo(system<nsrender_system>()->gbuffer_fbo());
	system<nsparticle_system>()->set_final_fbo(current()->compositeBuf);
}

bool NSEngine::makeCurrent(uint32 cID)
{
	auto fIter = mContexts.find(cID);
	if (fIter == mContexts.end())
		return false;
	mCurrentContext = cID;
	return true;
}

GLContext * NSEngine::current()
{
	auto fIter = mContexts.find(mCurrentContext);
	if (fIter != mContexts.end())
		return (fIter->second);
	return NULL;
}

uint32 NSEngine::currentid()
{
	return mCurrentContext;
}

bool NSEngine::destroyPlugin(nsplugin * plug)
{
	return current()->plugins->destroy(plug);
}

#ifdef NSDEBUG
NSDebug * NSEngine::debug()
{
	return current()->mDebug;
}
#endif

bool NSEngine::destroyContext(uint32 cID)
{
	auto fiter = mContexts.find(cID);
	if (fiter == mContexts.end())
		return false;
	delete fiter->second; fiter->second = NULL;
	mContexts.erase(fiter);

	if (mCurrentContext == cID)
		mCurrentContext = -1;

	return true;
}

void NSEngine::setPluginDirectory(const nsstring & plugdir)
{
	current()->plugins->set_plugin_dir(plugdir);
}

const nsstring & NSEngine::pluginDirectory()
{
	return current()->plugins->plugin_dir();
}

uint32 NSEngine::managerID(uint32 res_id)
{
	auto iter = mResManagerMap.find(res_id);
	if (iter != mResManagerMap.end())
		return iter->second;
	return 0;
}

uint32 NSEngine::managerID(std::type_index res_type)
{
	uint32 hashed_type = typeID(res_type);
	return managerID(hashed_type);
}

uint32 NSEngine::managerID(const nsstring & res_guid)
{
	return managerID(hash_id(res_guid));
}

const nsstring & NSEngine::cwd()
{
	return mCwd;
}

nsplugin * NSEngine::removePlugin(nsplugin * plg)
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

void NSEngine::_initDefaultFactories()
{
	registerComponentType<NSAnimComp>("NSAnimComp");
	registerComponentType<NSCamComp>("NSCamComp");
	registerComponentType<NSInputComp>("NSInputComp");
	registerComponentType<NSLightComp>("NSLightComp");
	registerComponentType<NSOccupyComp>("NSOccupyComp");
	registerComponentType<NSParticleComp>("NSParticleComp");
	registerComponentType<NSRenderComp>("NSRenderComp");
	registerComponentType<NSSelComp>("NSSelComp");
	registerComponentType<NSTFormComp>("NSTFormComp");
	registerComponentType<NSTileBrushComp>("NSTileBrushComp");
	registerComponentType<NSTileComp>("NSTileComp");
	registerComponentType<NSTerrainComp>("NSTerrainComp");

	registerSystemType<nsanim_system>("nsanim_system");
	registerSystemType<nsbuild_system>("nsbuild_system");
	registerSystemType<nscamera_system>("nscamera_system");
	registerSystemType<nsinput_system>("nsinput_system");
	registerSystemType<nsmovement_system>("nsmovement_system");
	registerSystemType<nsparticle_system>("nsparticle_system");
	registerSystemType<nsrender_system>("nsrender_system");
	registerSystemType<nsselection_system>("nsselection_system");
	
	
	registerResourceManagerType<nsanim_manager>("nsanim_manager");
	registerResourceManagerType<nsentity_manager>("nsentity_manager");
	registerResourceManagerType<nsmat_manager>("nsmat_manager");
	registerResourceManagerType<nsmesh_manager>("nsmesh_manager");
	registerResourceManagerType<nsscene_manager>("nsscene_manager");
	registerResourceManagerType<nsshader_manager>("nsshader_manager");
	registerResourceManagerType<nstex_manager>("nstex_manager");
	registerResourceManagerType<nsinput_map_manager>("nsinput_map_manager");
	registerResourceManagerType<nsplugin_manager>("nsplugin_manager");
	
	registerResourceType<nsanim_set, nsanim_manager>("nsanim_set");
	registerResourceType<nsentity, nsentity_manager>("nsentity");
	registerResourceType<nsmaterial, nsmat_manager>("nsmaterial");
	registerResourceType<nsmesh, nsmesh_manager>("nsmesh");
	registerResourceType<nsplugin, nsplugin_manager>("nsplugin");
	registerResourceType<nsscene, nsscene_manager>("nsscene");

	registerAbstractResourceType<nstexture, nstex_manager>("nstexture");
	registerResourceType<nstex1d, nstex_manager>("nstex1d");
	registerResourceType<nstex1d_array, nstex_manager>("nstex1d_array");
	registerResourceType<nstex2d, nstex_manager>("nstex2d");
	registerResourceType<nstex2d_array, nstex_manager>("nstex2d_array");
	registerResourceType<nstex3d, nstex_manager>("nstex3d");
	registerResourceType<nstex_rectangle, nstex_manager>("nstex_rectangle");
	registerResourceType<nstex_buffer, nstex_manager>("nstex_buffer");
	registerResourceType<nstex_cubemap, nstex_manager>("nstex_cubemap");
	registerResourceType<nstex_cubemap_array, nstex_manager>("nstex_cubemap_array");
	registerResourceType<nstex2d_multisample, nstex_manager>("nstex2d_multisample");
	registerResourceType<nstex2d_multisample_array, nstex_manager>("nstex2d_multisample_array");

	registerResourceType<nsshader, nsshader_manager>("nsshader");
	registerResourceType<nsdir_light_shader, nsshader_manager>("nsdir_light_shader");
	registerResourceType<nsspot_light_shader, nsshader_manager>("nsspot_light_shader");
	registerResourceType<nspoint_light_shader, nsshader_manager>("nspoint_light_shader");
	registerResourceType<nsmaterial_shader, nsshader_manager>("nsmaterial_shader");
	registerResourceType<nsparticle_process_shader, nsshader_manager>("nsparticle_process_shader");
	registerResourceType<nsparticle_render_shader, nsshader_manager>("nsparticle_render_shader");
	registerResourceType<nsdir_shadowmap_shader, nsshader_manager>("nsdir_shadowmap_shader");
	registerResourceType<nspoint_shadowmap_shader, nsshader_manager>("nspoint_shadowmap_shader");
	registerResourceType<nsspot_shadowmap_shader, nsshader_manager>("nsspot_shadowmap_shader");
	registerResourceType<nsearlyz_shader, nsshader_manager>("nsearlyz_shader");
	registerResourceType<nsdir_shadowmap_xfb_shader, nsshader_manager>("nsdir_shadowmap_xfb_shader");
	registerResourceType<nspoint_shadowmap_xfb_shader, nsshader_manager>("nspoint_shadowmap_xfb_shader");
	registerResourceType<nsspot_shadowmap_xfb_shader, nsshader_manager>("nsspot_shadowmap_xfb_shader");
	registerResourceType<nsearlyz_xfb_shader, nsshader_manager>("nsearlyz_xfb_shader");
	registerResourceType<nsrender_xfb_shader, nsshader_manager>("nsrender_xfb_shader");
	registerResourceType<nsxfb_shader, nsshader_manager>("nsxfb_shader");
	registerResourceType<nslight_stencil_shader, nsshader_manager>("nslight_stencil_shader");
	registerResourceType<nsskybox_shader, nsshader_manager>("nsskybox_shader");
	registerResourceType<nstransparency_shader, nsshader_manager>("nstransparency_shader");
	registerResourceType<nsselection_shader, nsshader_manager>("nsselection_shader");
	registerResourceType<nsinput_map, nsinput_map_manager>("nsinput_map");
}

GLContext::GLContext(uint32 id) :
	glewContext(new GLEWContext()),
	engplug(new nsplugin()),
	systems(new SystemMap()),
	plugins(new nsplugin_manager()),
	mEvents(new NSEventDispatcher()),
	fbmap(),
	timer(new NSTimer()),
	compositeBuf(0),
	context_id(id)
#ifdef NSDEBUG
	,mDebug(new NSDebug())
#endif
{
	engplug->rename(ENGINE_PLUG);
#ifdef NSDEBUG
	mDebug->setLogFile("engine_ctxt" + std::to_string(context_id) + ".log");
	mDebug->setLogDir(nsengine.cwd() + "logs");
	mDebug->clearLog();
#endif
}

GLContext::~GLContext()
{
	delete plugins;
	delete systems;
	delete engplug;
	delete mEvents;
	delete glewContext;
	delete timer;
#ifdef NSDEBUG
	delete mDebug;
#endif
}

NSEngine & NSEngine::inst()
{
	static NSEngine eng;
	return eng;
}

GLEWContext * glewGetContext()
{
	return nsengine.current()->glewContext;
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
