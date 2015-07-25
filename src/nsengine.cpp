/*!
\file nsengine.cpp

\brief Definition file for NSEngine class

This file contains all of the neccessary definitions for the NSEngine class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#include <hash/crc32.h>
#include <nsfileos.h>
#include <nsglobal.h>
#include <nsengine.h>
#include <nsrendersystem.h>
#include <nsscene.h>
#include <nsinputmapmanager.h>
#include <nsentitymanager.h>
#include <nsanimmanager.h>
#include <nsmeshmanager.h>
#include <nstexmanager.h>
#include <nsmatmanager.h>
#include <nseventdispatcher.h>
#include <nsshadermanager.h>
#include <nsinputmapmanager.h>
#include <nsinputmap.h>
#include <nspupper.h>
#include <nsscenemanager.h>
#include <nsselcomp.h>
#include <nsmovementsystem.h>
#include <nsterraincomp.h>
#include <nsanimsystem.h>
#include <nsinputcomp.h>
#include <IL/il.h>
#include <nsinputsystem.h>
#include <IL/ilu.h>
#include <IL/ilut.h>
#include <nsparticlecomp.h>
#include <nstimer.h>
#include <nsentity.h>
#include <nscamerasystem.h>
#include <nsselectionsystem.h>
#include <nsevent.h>
#include <nspluginmanager.h>
#include <nstilebrushcomp.h>
#include <nstilecomp.h>
#include <nsoccupycomp.h>
#include <nsbuildsystem.h>
#include <nsparticlecomp.h>
#include <nsparticlesystem.h>
#include <nslogfile.h>
#include <nsfactory.h>
#include <nssystem.h>
#include <nsrendersystem.h>

#ifdef NSDEBUG
#include <nsdebug.h>
#endif

NSEngine::NSEngine()
{
	srand(static_cast <unsigned> (time(0)));
    ilInit();
	iluInit();
	ilutInit();

	cwd = nsfileio::cwd() + nsstring("/");	
}

NSEngine::~NSEngine()
{
}

NSPlugin * NSEngine::active()
{
	return current()->plugins->active();
}

bool NSEngine::addPlugin(NSPlugin * plugin)
{
	if (plugin == NULL)
		return false;

	return current()->plugins->add(plugin);
}

void NSEngine::setCurrentScene(const nsstring & scn, bool newSceneOverwriteFile, bool saveprevious)
{
	if (active() == NULL)
		return;
	manager<NSSceneManager>(active())->setCurrent(scn, newSceneOverwriteFile, saveprevious);
}

void NSEngine::setCurrentScene(NSScene * scn, bool newSceneOverwriteFile, bool saveprevious)
{
	if (active() == NULL)
		return;
	manager<NSSceneManager>(active())->setCurrent(scn, newSceneOverwriteFile, saveprevious);
}

void NSEngine::setCurrentScene(nsuint scn, bool newSceneOverwriteFile, bool saveprevious)
{
	if (active() == NULL)
		return;
	manager<NSSceneManager>(active())->setCurrent(scn, newSceneOverwriteFile, saveprevious);
}

void NSEngine::setActive(const nsstring & plugname)
{
	plugins()->setActive(plugname);
}

void NSEngine::setActive(NSPlugin * plug)
{
	plugins()->setActive(plug);
}

void NSEngine::setActive(nsuint plugid)
{
	plugins()->setActive(plugid);
}

nsbool NSEngine::addSystem(NSSystem * pSystem)
{
	if (pSystem == NULL)
		return false;

	GLContext * gc = current();
	std::type_index ti(typeid(*pSystem));
	nsuint type_hash = nsengine.typeID(ti);

	if (type_hash == 0)
	{
		dprint(nsstring("NSEngine::addSystem - Could not find type id for type: ") + ti.name());
		return false;
	}
	auto it = gc->systems->emplace(type_hash, pSystem);

	if (!it.second)
		return false;

	nsint drawPriority = pSystem->drawPriority();

	// Only add systems with a draw priority
	if (drawPriority != NO_DRAW_PR)
		mSystemDrawOrder[drawPriority] = type_hash;

	mSystemUpdateOrder[pSystem->updatePriority()] = type_hash;
	return true;
}

NSResource * NSEngine::resource(nsuint res_typeid, NSPlugin * plg, nsuint resid)
{
	if (plg == NULL)
		return NULL;
	return plg->get(res_typeid, resid);
}

NSResource * NSEngine::resource(nsuint res_typeid, NSPlugin * plg, const nsstring & resname)
{
	if (plg == NULL)
		return NULL;
	return plg->get(res_typeid, resname);
}

nsuint NSEngine::createFramebuffer()
{
	NSFrameBuffer * fb = new NSFrameBuffer();
	fb->initGL();
	current()->fbmap.emplace(fb->glid(), fb);
	return fb->glid();
}

bool NSEngine::delFramebuffer(nsuint fbid)
{
	NSFrameBuffer * obj = framebuffer(fbid);
	if (obj == NULL)
		return false;
	obj->release();
	delete obj;
	current()->fbmap.erase(fbid);
	return true;
}

NSFrameBuffer * NSEngine::framebuffer(nsuint id)
{
	auto iter = current()->fbmap.find(id);
	if (iter != current()->fbmap.end())
		return iter->second;
	return NULL;
}

NSPlugin * NSEngine::loadPlugin(const nsstring & fname)
{
	return current()->plugins->load(fname);
}

void NSEngine::savePlugins(nsbool saveOwnedResources, NSSaveResCallback * scallback)
{
	current()->plugins->saveAll("", scallback);

	if (saveOwnedResources)
	{
		auto plugiter = current()->plugins->begin();
		while (plugiter != current()->plugins->end())
		{
			NSPlugin * plg = plugin(plugiter->first);
			plg->saveAll("",scallback);
			++plugiter;
		}
	}
}

bool NSEngine::savePlugin(NSPlugin * plg, bool saveOwnedResources,  NSSaveResCallback * scallback)
{
	if (plg == NULL)
		return false;
	
	bool ret = current()->plugins->save(plg);
	if (saveOwnedResources)
		plg->saveAll("",scallback);
	return ret;
}

void NSEngine::saveCore(NSSaveResCallback * scallback)
{
	engplug()->saveAll("", scallback);
}

bool NSEngine::destroyFactory(nsuint hashid)
{
	NSFactory * fac = removeFactory(hashid);
	if (fac == NULL)
		return false;
	delete fac;
	return true;
}

NSResManager * NSEngine::manager(nsuint manager_typeid, NSPlugin * plg)
{
	return plg->manager(manager_typeid);
}

NSResManager * manager(const nsstring & manager_guid, NSPlugin * plg)
{
	return plg->manager(manager_guid);		
}

bool NSEngine::delPlugin(NSPlugin * plg)
{
	return current()->plugins->del(plg);
}

bool NSEngine::destroySystem(nsuint type_id)
{
	NSSystem * sys = removeSystem(type_id);
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

NSPlugin * NSEngine::createPlugin(const nsstring & plugname, bool makeactive)
{
	NSPlugin * plug = current()->plugins->create(plugname);
	if (plug != NULL && makeactive)
		current()->plugins->setActive(plug);
	return plug;
}

NSSystem * NSEngine::createSystem(nsuint type_id)
{
	NSSystem * system = factory<NSSysFactory>(type_id)->create();
	if (!addSystem(system))
	{
		delete system;
		return NULL;
	}
	system->init();
	return system;
}

NSSystem * NSEngine::createSystem(const nsstring & guid_)
{
	return createSystem(hash_id(guid_));
}

NSScene * NSEngine::currentScene()
{
	NSPlugin * plug = active();
	if (plug == NULL)
		return NULL;
	return plug->currentScene();
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

NSSystem * NSEngine::system(nsuint type_id)
{
	GLContext * cont = current();
	auto iter = cont->systems->find(type_id);
	if (iter == cont->systems->end())
		return NULL;
	return iter->second;
}

bool NSEngine::hasPlugin(NSPlugin * plg)
{
	return current()->plugins->contains(plg);
}

NSSystem * NSEngine::system(const nsstring & guid_)
{
	return system(hash_id(guid_));
}

const nsstring & NSEngine::resourceDirectory()
{
	return mResourceDirectory;
}

nsbool NSEngine::hasSystem(nsuint type_id)
{
	GLContext * cont = current();
	return (cont->systems->find(type_id) != cont->systems->end());
}

nsbool NSEngine::hasSystem(const nsstring & guid_)
{
	return hasSystem(hash_id(guid_));
}

/*!
Overload of Propagate name change
*/
void NSEngine::nameChange(const uivec2 & oldid, const uivec2 newid)
{
	if (oldid.y == engplug()->id())
	{
		dprint("NSEngine::nameChange Cannot change engine default plugin name");
		return;
	}
	if (oldid.x == engplug()->id())
		engplug()->nameChange(oldid, newid);
	plugins()->nameChange(oldid, newid);
}

nsuint NSEngine::createContext(bool add)
{
	static nsuint id = 1; // forever increasing id
	auto iter = mContexts.emplace(id, new GLContext(id));
	if (!iter.second)
		return -1;
	mCurrentContext = id;
	if (add)
		_initDefaultFactories();
	return id++;
}

NSPlugin * NSEngine::engplug()
{
	return current()->engplug;
}

NSPluginManager * NSEngine::plugins()
{
	return current()->plugins;
}

NSSystem * NSEngine::removeSystem(nsuint type_id)
{
	NSSystem * sys = system(type_id);
	if (sys == NULL)
		return NULL;
	current()->systems->erase(type_id);
	_removeSys(type_id);
	return sys;
}

NSSystem * NSEngine::removeSystem(const nsstring & guid_)
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
	engplug()->setImportDir(mImportDirectory);
	auto iter = mContexts.begin();
	while (iter != mContexts.end())
	{
		auto iter2 = iter->second->plugins->begin();
		while (iter2 != iter->second->plugins->end())
		{
			NSPlugin * plug = static_cast<NSPlugin*>(iter2->second);
			plug->setImportDir(mImportDirectory);
			++iter2;
		}
		++iter;
	}
}

void NSEngine::setResourceDir(const nsstring & dir)
{
	mResourceDirectory = dir;
	auto iter = mContexts.begin();
	while (iter != mContexts.end())
	{
		iter->second->plugins->setResourceDirectory(dir);
		++iter;
	}
}

void NSEngine::start()
{
	if (current() == NULL)
		return;

	glewExperimental = TRUE;

	// Initialize the glew extensions - if this fails we want a crash because there is nothing
	// useful the program can do without these initialized
	GLenum cont = glewInit();
	if (cont != GLEW_OK)
	{
		NSLogFile("GLEW extensions unable to initialize");
		return;
	}

	setResourceDir(cwd + nsstring(DEFAULT_RESOURCE_DIR));
	setImportDir(cwd + nsstring(DEFAULT_IMPORT_DIR));
	setPluginDirectory(cwd + nsstring(LOCAL_PLUGIN_DIR_DEFAULT));

	engplug()->init();
	engplug()->bind();
	engplug()->setResourceDirectory(cwd + nsstring("core/"));
	engplug()->addNameToResPath(false);

	current()->compositeBuf = createFramebuffer();
	_initSystems();
	_initShaders();
	_initMaterials();
	_initMeshes();
	_initEntities();
	_initInputMaps();
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

NSPlugin * NSEngine::plugin(const nsstring & name)
{
	if (name == engplug()->name())
		return engplug();
	return current()->plugins->get(name);
}

NSPlugin * NSEngine::plugin(nsuint id)
{
	if (id == engplug()->id())
		return engplug();
	return current()->plugins->get(id);
}

NSPlugin * NSEngine::plugin(NSPlugin * plg)
{
	if (plg == engplug())
		return engplug();
	return current()->plugins->get(plg);
}

NSFactory * NSEngine::factory(nsuint hash_id)
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

nsstring NSEngine::guid(nsuint hash)
{
	auto fiter = mObjTypeNames.find(hash);
	if (fiter != mObjTypeNames.end())
		return fiter->second;
	return "";
}

nsuint NSEngine::typeID(std::type_index type)
{
	auto fiter = mObjTypeHashes.find(type);
	if (fiter != mObjTypeHashes.end())
		return fiter->second;
	return 0;
}

NSFactory * NSEngine::removeFactory(nsuint hash_id)
{
	NSFactory * f = factory(hash_id);
	if (f == NULL)
		return NULL;
	mFactories.erase(hash_id);
	return f;
}

void NSEngine::update()
{
	mt.lock();
	timer()->update();
	
//	while (timer()->lag() >= timer()->fixed())
//	{
		// Go through each system and update
		auto sysUpdateIter = mSystemUpdateOrder.begin();
		while (sysUpdateIter != mSystemUpdateOrder.end())
		{
			NSSystem * sys = system(sysUpdateIter->second);
			sys->update();
			++sysUpdateIter;
		}
//		timer()->lag() -= timer()->fixed();
//	}

	// Go through each system and draw
	auto sysDrawIter = mSystemDrawOrder.begin();
	while (sysDrawIter != mSystemDrawOrder.end())
	{
		NSSystem * sys = system(sysDrawIter->second);
		sys->draw();
		++sysDrawIter;
	}
	system<NSRenderSystem>()->blitFinalFrame();
	mt.unlock();
}

void NSEngine::_initInputMaps()
{
	NSInputMap * inmap = engplug()->load<NSInputMap>(DEFAULT_ENGINE_INPUT);
	system<NSInputSystem>()->setInputMap(inmap->fullid());
	system<NSInputSystem>()->pushContext(DEFAULT_INPUT_CONTEXT);	
}

void NSEngine::_initShaders()
{
	NSShaderManager * mShaders = engplug()->manager<NSShaderManager>();
	NSRenderSystem::RenderShaders renShaders;
	nsstring shext = nsstring(DEFAULT_SHADER_EXTENSION);
	renShaders.mDefaultShader = mShaders->load<NSMaterialShader>(nsstring(DEFAULT_GBUFFER_SHADER) + shext);
	renShaders.mEarlyZShader = mShaders->load<NSEarlyZShader>(nsstring(DEFAULT_EARLYZ_SHADER) + shext);
	renShaders.mLightStencilShader = mShaders->load<NSLightStencilShader>(nsstring(DEFAULT_LIGHTSTENCIL_SHADER) + shext);
	renShaders.mDirLightShader = mShaders->load<NSDirLightShader>(nsstring(DEFAULT_DIRLIGHT_SHADER) + shext);
	renShaders.mPointLightShader = mShaders->load<NSPointLightShader>(nsstring(DEFAULT_POINTLIGHT_SHADER) + shext);
	renShaders.mSpotLightShader = mShaders->load<NSSpotLightShader>(nsstring(DEFAULT_SPOTLIGHT_SHADER) + shext);
	renShaders.mPointShadowShader = mShaders->load<NSPointShadowMapShader>(nsstring(DEFAULT_POINTSHADOWMAP_SHADER) + shext);
	renShaders.mSpotShadowShader = mShaders->load<NSSpotShadowMapShader>(nsstring(DEFAULT_SPOTSHADOWMAP_SHADER) + shext);
	renShaders.mDirShadowShader = mShaders->load<NSDirShadowMapShader>(nsstring(DEFAULT_DIRSHADOWMAP_SHADER) + shext);
	renShaders.mGBufDefaultXFB = mShaders->load<NSXFBShader>(nsstring(DEFAULT_XFBGBUFFER_SHADER) + shext);
	renShaders.mGBufDefaultXBFRender = mShaders->load<NSRenderXFBShader>(nsstring(DEFAULT_XFBGBUFFER_RENDER_SHADER) + shext);
	renShaders.mXFBEarlyZ = mShaders->load<NSEarlyZXFBShader>(nsstring(DEFAULT_XFBEARLYZ_SHADER) + shext);
	renShaders.mXFBDirShadowMap = mShaders->load<NSDirShadowMapXFBShader>(nsstring(DEFAULT_XFBDIRSHADOWMAP_SHADER) + shext);
	renShaders.mXFBPointShadowMap = mShaders->load<NSPointShadowMapXFBShader>(nsstring(DEFAULT_XFBPOINTSHADOWMAP_SHADER) + shext);
	renShaders.mXFBSpotShadowMap = mShaders->load<NSSpotShadowMapXFBShader>(nsstring(DEFAULT_XFBSPOTSHADOWMAP_SHADER) + shext);
	NSParticleProcessShader * xfsparticle = mShaders->load<NSParticleProcessShader>(nsstring(DEFAULT_PROCESS_PARTICLE_SHADER) + shext);
	NSParticleRenderShader * renderparticle = mShaders->load<NSParticleRenderShader>(nsstring(DEFAULT_RENDER_PARTICLE_SHADER) + shext);
	NSSelectionShader * selshader = mShaders->load<NSSelectionShader>(nsstring(DEFAULT_SELECTION_SHADER) + shext);
	NSSkyboxShader * skysh = mShaders->load<NSSkyboxShader>(nsstring(DEFAULT_SKYBOX_SHADER) + shext);
	system<NSRenderSystem>()->setShaders(renShaders);
	system<NSSelectionSystem>()->setShader(selshader);
	system<NSParticleSystem>()->setShader(xfsparticle);
	mShaders->compileAll();
	mShaders->linkAll();
	mShaders->initUniformsAll();
}

void NSEngine::_initEntities()
{
	NSEntity * objBrush = engplug()->create<NSEntity>(ENT_OBJECT_BRUSH);
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
	system<NSBuildSystem>()->setObjectBrush(objBrush);
}

void NSEngine::_removeSys(nsuint type_id)
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
	NSTexture * tex = engplug()->load<NSTex2D>(nsstring(DEFAULT_MATERIAL_DIFFUSE) + nsstring(DEFAULT_TEX_EXTENSION));
	NSMaterial * def = engplug()->load<NSMaterial>(nsstring(DEFAULT_MATERIAL_NAME) + nsstring(DEFAULT_MAT_EXTENSION));
	system<NSRenderSystem>()->setDefaultMat(def);
}

void NSEngine::_initMeshes()
{
	NSMesh * msh = engplug()->load<NSMesh>(nsstring(MESH_FULL_TILE) + nsstring(DEFAULT_MESH_EXTENSION));
	msh->bakeNodeRotation(orientation(fvec4(1, 0, 0, -90.0f)));
	engplug()->load<NSMesh>(nsstring(MESH_TERRAIN) + nsstring(DEFAULT_MESH_EXTENSION));
	engplug()->load<NSMesh>(nsstring(MESH_HALF_TILE) + nsstring(DEFAULT_MESH_EXTENSION));
	engplug()->load<NSMesh>(nsstring(MESH_POINTLIGHT_BOUNDS) + nsstring(DEFAULT_MESH_EXTENSION));
	engplug()->load<NSMesh>(nsstring(MESH_SPOTLIGHT_BOUNDS) + nsstring(DEFAULT_MESH_EXTENSION));
	engplug()->load<NSMesh>(nsstring(MESH_DIRLIGHT_BOUNDS) + nsstring(DEFAULT_MESH_EXTENSION));
	engplug()->load<NSMesh>(nsstring(MESH_SKYDOME) + nsstring(DEFAULT_MESH_EXTENSION));
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
	system<NSRenderSystem>()->setFinalfbo(current()->compositeBuf);
	system<NSSelectionSystem>()->setFinalfbo(current()->compositeBuf);
	system<NSSelectionSystem>()->setPickfbo(system<NSRenderSystem>()->gbufferfbo());
	system<NSParticleSystem>()->setFinalfbo(current()->compositeBuf);
}

bool NSEngine::makeCurrent(nsuint cID)
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

nsuint NSEngine::currentid()
{
	return mCurrentContext;
}

bool NSEngine::destroyPlugin(NSPlugin * plug)
{
	return current()->plugins->destroy(plug);
}

#ifdef NSDEBUG
NSDebug * NSEngine::debug()
{
	return current()->mDebug;
}
#endif

bool NSEngine::destroyContext(nsuint cID)
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
	current()->plugins->setPluginDirectory(plugdir);
}

const nsstring & NSEngine::pluginDirectory()
{
	return current()->plugins->pluginDirectory();
}

nsuint NSEngine::managerID(nsuint res_id)
{
	auto iter = mResManagerMap.find(res_id);
	if (iter != mResManagerMap.end())
		return iter->second;
	return 0;
}

nsuint NSEngine::managerID(std::type_index res_type)
{
	nsuint hashed_type = typeID(res_type);
	return managerID(hashed_type);
}

nsuint NSEngine::managerID(const nsstring & res_guid)
{
	return managerID(hash_id(res_guid));
}

NSPlugin * NSEngine::removePlugin(NSPlugin * plg)
{
	return current()->plugins->remove(plg);
}

NSResource * NSEngine::_resource(nsuint restype_id, const uivec2 & resid)
{
	NSPlugin * plg = plugin(resid.x);
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

	registerSystemType<NSAnimSystem>("NSAnimSystem");
	registerSystemType<NSBuildSystem>("NSBuildSystem");
	registerSystemType<NSCameraSystem>("NSCameraSystem");
	registerSystemType<NSInputSystem>("NSInputSystem");
	registerSystemType<NSMovementSystem>("NSMovementSystem");
	registerSystemType<NSParticleSystem>("NSParticleSystem");
	registerSystemType<NSRenderSystem>("NSRenderSystem");
	registerSystemType<NSSelectionSystem>("NSSelectionSystem");
	
	
	registerResourceManagerType<NSAnimManager>("NSAnimManager");
	registerResourceManagerType<NSEntityManager>("NSEntityManager");
	registerResourceManagerType<NSMatManager>("NSMatManager");
	registerResourceManagerType<NSMeshManager>("NSMeshManager");
	registerResourceManagerType<NSSceneManager>("NSSceneManager");
	registerResourceManagerType<NSShaderManager>("NSShaderManager");
	registerResourceManagerType<NSTexManager>("NSTexManager");
	registerResourceManagerType<NSInputMapManager>("NSInputMapManager");
	registerResourceManagerType<NSPluginManager>("NSPluginManager");
	
	registerResourceType<NSAnimSet, NSAnimManager>("NSAnimSet");
	registerResourceType<NSEntity, NSEntityManager>("NSEntity");
	registerResourceType<NSMaterial, NSMatManager>("NSMaterial");
	registerResourceType<NSMesh, NSMeshManager>("NSMesh");
	registerResourceType<NSPlugin, NSPluginManager>("NSPlugin");
	registerResourceType<NSScene, NSSceneManager>("NSScene");

	registerAbstractResourceType<NSTexture, NSTexManager>("NSTexture");
	registerResourceType<NSTex1D, NSTexManager>("NSTex1D");
	registerResourceType<NSTex1DArray, NSTexManager>("NSTex1DArray");
	registerResourceType<NSTex2D, NSTexManager>("NSTex2D");
	registerResourceType<NSTex2DArray, NSTexManager>("NSTex2DArray");
	registerResourceType<NSTex3D, NSTexManager>("NSTex3D");
	registerResourceType<NSTexRectangle, NSTexManager>("NSTexRectangle");
	registerResourceType<NSTexBuffer, NSTexManager>("NSTexBuffer");
	registerResourceType<NSTexCubeMap, NSTexManager>("NSTexCubeMap");
	registerResourceType<NSTexCubeMapArray, NSTexManager>("NSTexCubeMapArray");
	registerResourceType<NSTex2DMultisample, NSTexManager>("NSTex2DMultisample");
	registerResourceType<NSTex2DMultisampleArray, NSTexManager>("NSTex2DMultisampleArray");

	registerResourceType<NSShader, NSShaderManager>("NSShader");
	registerResourceType<NSDirLightShader, NSShaderManager>("NSDirLightShader");
	registerResourceType<NSSpotLightShader, NSShaderManager>("NSSpotLightShader");
	registerResourceType<NSPointLightShader, NSShaderManager>("NSPointLightShader");
	registerResourceType<NSMaterialShader, NSShaderManager>("NSMaterialShader");
	registerResourceType<NSParticleProcessShader, NSShaderManager>("NSParticleProcessShader");
	registerResourceType<NSParticleRenderShader, NSShaderManager>("NSParticleRenderShader");
	registerResourceType<NSDirShadowMapShader, NSShaderManager>("NSDirShadowMapShader");
	registerResourceType<NSPointShadowMapShader, NSShaderManager>("NSPointShadowMapShader");
	registerResourceType<NSSpotShadowMapShader, NSShaderManager>("NSSpotShadowMapShader");
	registerResourceType<NSEarlyZShader, NSShaderManager>("NSEarlyZShader");
	registerResourceType<NSDirShadowMapXFBShader, NSShaderManager>("NSDirShadowMapXFBShader");
	registerResourceType<NSPointShadowMapXFBShader, NSShaderManager>("NSPointShadowMapXFBShader");
	registerResourceType<NSSpotShadowMapXFBShader, NSShaderManager>("NSSpotShadowMapXFBShader");
	registerResourceType<NSEarlyZXFBShader, NSShaderManager>("NSEarlyZXFBShader");
	registerResourceType<NSRenderXFBShader, NSShaderManager>("NSRenderXFBShader");
	registerResourceType<NSXFBShader, NSShaderManager>("NSXFBShader");
	registerResourceType<NSLightStencilShader, NSShaderManager>("NSLightStencilShader");
	registerResourceType<NSSkyboxShader, NSShaderManager>("NSSkyboxShader");
	registerResourceType<NSTransparencyShader, NSShaderManager>("NSTransparencyShader");
	registerResourceType<NSSelectionShader, NSShaderManager>("NSSelectionShader");
	registerResourceType<NSInputMap, NSInputMapManager>("NSInputMap");
}

GLContext::GLContext(nsuint id) :
	glewContext(new GLEWContext()),
	engplug(new NSPlugin()),
	systems(new SystemMap()),
	plugins(new NSPluginManager()),
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

nsuint hash_id(const nsstring & str)
{
	return crc32(str.c_str(),static_cast<int>(str.size()),0);
}
