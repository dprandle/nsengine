/*!
\file nsengine.cpp

\brief Definition file for NSEngine class

This file contains all of the neccessary definitions for the NSEngine class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#include <nsglobal.h>
#include <nsengine.h>
#include <nsrendersystem.h>
#include <nsscene.h>
#include <nsinputmanager.h>
#include <nsentitymanager.h>
#include <nsanimmanager.h>
#include <nsmeshmanager.h>
#include <nstexmanager.h>
#include <nsmatmanager.h>
#include <nseventhandler.h>
#include <nsshadermanager.h>
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

NSEngine::NSEngine():
mTimer(new NSTimer()),
mInput(new NSInputManager())
#ifdef NSDEBUG
, mDebug(new NSDebug())
#endif
{
	mInput->setDirectory(LOCAL_INPUT_DIR_DEFAULT);

#ifdef NSDEBUG
	mDebug->clearLog();
#endif
}

NSEngine::~NSEngine()
{
	delete mTimer;
	delete mInput;
#ifdef NSDEBUG
	delete mDebug;
#endif
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
	auto it = gc->systems->emplace(pSystem->typeString(), pSystem);

	if (!it.second)
		return false;

	nsfloat drawPriority = pSystem->drawPriority();

	// Only add systems with a draw priority
	if (drawPriority != NO_DRAW_PR)
		mSystemDrawOrder[drawPriority] = pSystem->typeString();

	mSystemUpdateOrder[pSystem->updatePriority()] = pSystem->typeString();
	return true;
}

bool NSEngine::addResource(NSResource * res)
{
	return addResource(active(), res);
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

NSPlugin * NSEngine::loadPlugin(const nsstring & fname, bool appendDirs)
{
	return current()->plugins->load<NSPlugin>(fname, appendDirs);
}

bool NSEngine::save()
{
	return current()->plugins->save(true);
}

bool NSEngine::save(const nsstring & plugname)
{
	return current()->plugins->save(plugname);
}

bool NSEngine::save(nsuint plugid)
{
	return current()->plugins->save(plugid);
}

bool NSEngine::save(NSPlugin * plugtosave)
{
	return current()->plugins->save(plugtosave);
}

bool NSEngine::savecore()
{
	return engplug()->save();
}

bool NSEngine::delFactory(const nsstring & objtype)
{
	NSFactory * fac = removeFactory(objtype);
	if (fac == NULL)
		return false;
	delete fac;
	return true;
}


bool NSEngine::delSystem(const nsstring & systype)
{
	NSSystem * sys = removeSystem(systype);
	if (sys == NULL)
		return false;
	delete sys;
	return true;
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
	NSPlugin * plug = current()->plugins->create<NSPlugin>(plugname);
	if (plug != NULL && makeactive)
		current()->plugins->setActive(plug);
	return plug;
}

NSSystem * NSEngine::createSystem(const nsstring & systype)
{
	NSSystem * system = factory<NSSysFactory>(systype)->create();
	if (!addSystem(system))
	{
		delete system;
		return NULL;
	}
	system->init();
	return system;
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
	return mTimer;
}

NSInputManager * NSEngine::input()
{
	return mInput;
}

#ifdef NSDEBUG
void NSEngine::debugPrint(const nsstring & str)
{
	mDebug->print(str);
}
#endif

NSEventHandler * NSEngine::events()
{
	return current()->mEvents;
}

NSSystem * NSEngine::system(const nsstring & pTypeName)
{
	GLContext * cont = current();
	auto iter = cont->systems->find(pTypeName);
	if (iter == cont->systems->end())
		return NULL;
	return iter->second;
}

const nsstring & NSEngine::resourceDirectory()
{
	return mResourceDirectory;
}

nsbool NSEngine::hasFactory(const nsstring & pObjType)
{
	return (mFactories.find(pObjType) != mFactories.end());
}

nsbool NSEngine::hasSystem(const nsstring & pTypeString)
{
	GLContext * cont = current();
	return (cont->systems->find(pTypeString) != cont->systems->end());
}

bool NSEngine::loadInput(nsstring pFileName, nsuint plugid, bool prefixdir)
{
	NSPlugin * plug = NULL;
	if (plugid == 0)
		plug = engplug();
	else
		plug = plugin(plugid);

	if (plug == NULL)
		return false;

	if (prefixdir)
	{
		if (plug->addingNameToResPath())
			pFileName = plug->resourceDirectory() + plug->name() + "/" + mInput->directory() + pFileName;
		else
			pFileName = plug->resourceDirectory() + mInput->directory() + pFileName;
	}

	nsfstream fIn;
	fIn.open(pFileName, std::ios::in);
	if (!fIn.is_open())
		return false;

	NSTextFilePUPer p(fIn, PUP_IN);

	nsstring rt;

	pup(p, rt, "type");
	if (rt != "inputconfig")
	{
		dprint("NSEngine::loadInput Attempted to load input config from file that is not of that type: " + pFileName);
		fIn.close();
		return false;
	}

	pup(p, *mInput);
	
	saveInput(pFileName, 0, false);
	fIn.close();
	return true;
}

NSEntity * NSEngine::loadModel(const nsstring & entname, const nsstring & fname, bool prefixWithImportDir, const nsstring & meshname, bool flipuv)
{
	return loadModel(active(), entname, fname, prefixWithImportDir, meshname, flipuv);
}

bool NSEngine::loadModelResources(const nsstring & fname, bool prefixWithImportDir, const nsstring & meshname, bool flipuv)
{
	return loadModelResources(active(), fname, prefixWithImportDir, meshname, flipuv);
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

bool NSEngine::saveInput(nsstring pFileName, nsuint plugid, bool prefixdir)
{
	NSPlugin * plug = NULL;
	if (plugid == 0)
		plug = engplug();
	else
		plug = plugin(plugid);

	if (plug == NULL)
		return false;

	if (prefixdir)
	{
		if (plug->addingNameToResPath())
			pFileName = plug->resourceDirectory() + plug->name() + "/" + mInput->directory() + pFileName;
		else
			pFileName = plug->resourceDirectory() + mInput->directory() + pFileName;
	}

	nsfstream fout;
	fout.open(pFileName, std::ios::out);
	if (!fout.is_open())
		return false;

	NSTextFilePUPer p(fout, PUP_OUT);
	pup(p, nsstring("inputconfig"), "type");
	pup(p, *mInput);
	fout.close();
	return true;
}

nsuint NSEngine::createContext(bool add)
{
	static nsuint id = 1; // forever increasing id
	auto iter = mContexts.emplace(id, new GLContext());
	if (!iter.second)
		return -1;
	if (add)
		_initDefaultFactories();
	mCurrentContext = id;
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

NSSystem * NSEngine::removeSystem(const nsstring & sysType)
{
	NSSystem * sys = system(sysType);
	if (sys == NULL)
		return NULL;
	current()->systems->erase(sysType);
	_removeSys(sysType);
	return sys;
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
	srand(static_cast <unsigned> (time(0)));
	int i = ilInit();
	i = iluInit();
	i = ilutInit();

	setResourceDir(DEFAULT_RESOURCE_DIR);
	setImportDir(DEFAULT_IMPORT_DIR);
	setPluginDirectory(LOCAL_PLUGIN_DIR_DEFAULT);
	engplug()->init();
	engplug()->bind();
	engplug()->setResourceDirectory("core/");
	engplug()->addNameToResPath(false);
	loadInput(DEFAULT_ENGINE_INPUT, 0);
	mInput->pushContext(DEFAULT_INPUT_CONTEXT);
	current()->compositeBuf = createFramebuffer();
	_initSystems();
	_initShaders();
	_initMaterials();
	_initMeshes();
	_initEntities();
	mTimer->start();
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
	if (name == nsstring(ENGINE_PLUG))
		return engplug();
	return current()->plugins->get(name);
}

NSPlugin * NSEngine::plugin(nsuint id)
{
	if (id == 0)
		return active();
	if (id == engplug()->id())
		return engplug();
	return current()->plugins->get(id);
}

NSPlugin * NSEngine::plugin(NSPlugin * plug)
{
	if (plug == NULL)
		return NULL;
	return plugin(plug->id());
}

NSFactory * NSEngine::factory(const nsstring & typeString)
{
	auto fIter = mFactories.find(typeString);
	if (fIter == mFactories.end())
		return NULL;
	return fIter->second;
}

NSFactory * NSEngine::removeFactory(const nsstring & objtype)
{
	NSFactory * f = factory(objtype);
	if (f == NULL)
		return NULL;
	mFactories.erase(objtype);
	return f;
}

nsbool NSEngine::unloadResource(NSResource * res)
{
	NSPlugin * plug = plugin(res->plugid());
	if (plug != NULL)
		return plug->unload(res);
	return false;
}

void NSEngine::update()
{
	mt.lock();
	mTimer->update();
	
	while (mTimer->lag() >= mTimer->fixed())
	{
		// Go through each system and update
		auto sysUpdateIter = mSystemUpdateOrder.begin();
		while (sysUpdateIter != mSystemUpdateOrder.end())
		{
			NSSystem * sys = system(sysUpdateIter->second);
			sys->update();
			++sysUpdateIter;
		}
		mTimer->lag() -= mTimer->fixed();
	}

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

void NSEngine::_initShaders()
{
	NSShaderManager * mShaders = engplug()->manager<NSShaderManager>();
	NSRenderSystem::RenderShaders renShaders;
	renShaders.mDefaultShader = mShaders->load<NSMaterialShader>(nsstring(DEFAULT_GBUFFER_SHADER) + nsstring(DEFAULT_SHADER_EXTENSION), true);
	renShaders.mEarlyZShader = mShaders->load<NSEarlyZShader>(nsstring(DEFAULT_EARLYZ_SHADER) + nsstring(DEFAULT_SHADER_EXTENSION), true);
	renShaders.mLightStencilShader = mShaders->load<NSLightStencilShader>(nsstring(DEFAULT_LIGHTSTENCIL_SHADER) + nsstring(DEFAULT_SHADER_EXTENSION), true);
	renShaders.mDirLightShader = mShaders->load<NSDirLightShader>(nsstring(DEFAULT_DIRLIGHT_SHADER) + nsstring(DEFAULT_SHADER_EXTENSION), true);
	renShaders.mPointLightShader = mShaders->load<NSPointLightShader>(nsstring(DEFAULT_POINTLIGHT_SHADER) + nsstring(DEFAULT_SHADER_EXTENSION), true);
	renShaders.mSpotLightShader = mShaders->load<NSSpotLightShader>(nsstring(DEFAULT_SPOTLIGHT_SHADER) + nsstring(DEFAULT_SHADER_EXTENSION), true);
	renShaders.mPointShadowShader = mShaders->load<NSPointShadowMapShader>(nsstring(DEFAULT_POINTSHADOWMAP_SHADER) + nsstring(DEFAULT_SHADER_EXTENSION), true);
	renShaders.mSpotShadowShader = mShaders->load<NSSpotShadowMapShader>(nsstring(DEFAULT_SPOTSHADOWMAP_SHADER) + nsstring(DEFAULT_SHADER_EXTENSION), true);
	renShaders.mDirShadowShader = mShaders->load<NSDirShadowMapShader>(nsstring(DEFAULT_DIRSHADOWMAP_SHADER) + nsstring(DEFAULT_SHADER_EXTENSION), true);
	renShaders.mGBufDefaultXFB = mShaders->load<NSXFBShader>(nsstring(DEFAULT_XFBGBUFFER_SHADER) + nsstring(DEFAULT_SHADER_EXTENSION), true);
	renShaders.mGBufDefaultXBFRender = mShaders->load<NSRenderXFBShader>(nsstring(DEFAULT_XFBGBUFFER_RENDER_SHADER) + nsstring(DEFAULT_SHADER_EXTENSION), true);
	renShaders.mXFBEarlyZ = mShaders->load<NSEarlyZXFBShader>(nsstring(DEFAULT_XFBEARLYZ_SHADER) + nsstring(DEFAULT_SHADER_EXTENSION), true);
	renShaders.mXFBDirShadowMap = mShaders->load<NSDirShadowMapXFBShader>(nsstring(DEFAULT_XFBDIRSHADOWMAP_SHADER) + nsstring(DEFAULT_SHADER_EXTENSION), true);
	renShaders.mXFBPointShadowMap = mShaders->load<NSPointShadowMapXFBShader>(nsstring(DEFAULT_XFBPOINTSHADOWMAP_SHADER) + nsstring(DEFAULT_SHADER_EXTENSION), true);
	renShaders.mXFBSpotShadowMap = mShaders->load<NSSpotShadowMapXFBShader>(nsstring(DEFAULT_XFBSPOTSHADOWMAP_SHADER) + nsstring(DEFAULT_SHADER_EXTENSION), true);
	NSParticleProcessShader * xfsparticle = mShaders->load<NSParticleProcessShader>(nsstring(DEFAULT_PROCESS_PARTICLE_SHADER) + nsstring(DEFAULT_SHADER_EXTENSION), true);
	NSParticleRenderShader * renderparticle = mShaders->load<NSParticleRenderShader>(nsstring(DEFAULT_RENDER_PARTICLE_SHADER) + nsstring(DEFAULT_SHADER_EXTENSION), true);
	NSSelectionShader * selshader = mShaders->load<NSSelectionShader>(nsstring(DEFAULT_SELECTION_SHADER) + nsstring(DEFAULT_SHADER_EXTENSION), true);
	NSSkyboxShader * skysh = mShaders->load<NSSkyboxShader>(nsstring(DEFAULT_SKYBOX_SHADER) + nsstring(DEFAULT_SHADER_EXTENSION), true);
	system<NSRenderSystem>()->setShaders(renShaders);
	system<NSSelectionSystem>()->setShader(selshader);
	system<NSParticleSystem>()->setShader(xfsparticle);
	mShaders->compile();
	mShaders->link();
	mShaders->initUniforms();
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

void NSEngine::_removeSys(const nsstring & systype)
{
	auto iter1 = mSystemDrawOrder.begin();
	while (iter1 != mSystemDrawOrder.end())
	{
		if (iter1->second == systype)
		{
			mSystemDrawOrder.erase(iter1);
			return;
		}
		++iter1;
	}

	auto iter2 = mSystemUpdateOrder.begin();
	while (iter2 != mSystemUpdateOrder.end())
	{
		if (iter2->second == systype)
		{
			mSystemUpdateOrder.erase(iter2);
			return;
		}
		++iter2;
	}
}

void NSEngine::_initMaterials()
{
	NSTexture * tex = engplug()->load<NSTex2D>(nsstring(DEFAULT_MATERIAL_DIFFUSE) + nsstring(DEFAULT_TEX_EXTENSION), true);
	NSMaterial * def = engplug()->load<NSMaterial>(nsstring(DEFAULT_MATERIAL_NAME) + nsstring(DEFAULT_MAT_EXTENSION),true);
	system<NSRenderSystem>()->setDefaultMat(def);
}

void NSEngine::_initMeshes()
{
	NSMesh * msh = engplug()->load<NSMesh>(nsstring(MESH_FULL_TILE) + nsstring(DEFAULT_MESH_EXTENSION), true);
	msh->bakeNodeRotation(orientation(fvec4(1, 0, 0, -90.0f)));
	engplug()->load<NSMesh>(nsstring(MESH_TERRAIN) + nsstring(DEFAULT_MESH_EXTENSION), true);
	engplug()->load<NSMesh>(nsstring(MESH_HALF_TILE) + nsstring(DEFAULT_MESH_EXTENSION), true);
	engplug()->load<NSMesh>(nsstring(MESH_POINTLIGHT_BOUNDS) + nsstring(DEFAULT_MESH_EXTENSION), true);
	engplug()->load<NSMesh>(nsstring(MESH_SPOTLIGHT_BOUNDS) + nsstring(DEFAULT_MESH_EXTENSION), true);
	engplug()->load<NSMesh>(nsstring(MESH_DIRLIGHT_BOUNDS) + nsstring(DEFAULT_MESH_EXTENSION), true);
	engplug()->load<NSMesh>(nsstring(MESH_SKYDOME) + nsstring(DEFAULT_MESH_EXTENSION), true);
}

void NSEngine::_initSystems()
{
	auto fiter = nsengine.beginFac();
	while (fiter != nsengine.endFac())
	{
		if (fiter->second->type() == NSFactory::System)
			createSystem(fiter->first);
		++fiter;
	}
	system<NSRenderSystem>()->setFinalfbo(current()->compositeBuf);
	system<NSSelectionSystem>()->setFinalfbo(current()->compositeBuf);
	system<NSSelectionSystem>()->setPickfbo(system<NSRenderSystem>()->gbufferfbo());
	system<NSParticleSystem>()->setFinalfbo(current()->compositeBuf);
}

void NSEngine::makeCurrent(nsuint cID)
{
	mCurrentContext = cID;
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

#ifdef NSDEBUG
NSDebug * NSEngine::debug()
{
	return mDebug;
}
#endif

nsbool NSEngine::delResource(NSResource * res)
{
	if (res == NULL)
		return false;

	NSPlugin * plug = plugin(res->plugid());
	if (plug == NULL)
		return false;
	
	return plug->del(res);
}

bool NSEngine::delContext(nsuint cID)
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

NSResource * NSEngine::removeResource(NSResource * res)
{
	if (res == NULL)
		return NULL;

	NSPlugin * plug = plugin(res->plugid());
	if (plug == NULL)
		return NULL;

	return plug->remove(res);
}

void NSEngine::_initDefaultFactories()
{
	createComponentFactory<NSAnimComp>();
	createComponentFactory<NSCamComp>();
	createComponentFactory<NSInputComp>();
	createComponentFactory<NSLightComp>();
	createComponentFactory<NSOccupyComp>();
	createComponentFactory<NSParticleComp>();
	createComponentFactory<NSRenderComp>();
	createComponentFactory<NSSelComp>();
	createComponentFactory<NSTFormComp>();
	createComponentFactory<NSTileBrushComp>();
	createComponentFactory<NSTileComp>();
	createComponentFactory<NSTerrainComp>();
	createSystemFactory<NSAnimSystem>();
	createSystemFactory<NSBuildSystem>();
	createSystemFactory<NSCameraSystem>();
	createSystemFactory<NSInputSystem>();
	createSystemFactory<NSMovementSystem>();
	createSystemFactory<NSParticleSystem>();
	createSystemFactory<NSRenderSystem>();
	createSystemFactory<NSSelectionSystem>();
	createResourceFactory<NSAnimSet>();
	createResourceFactory<NSEntity>();
	createResourceFactory<NSMaterial>();
	createResourceFactory<NSMesh>();
	createResourceFactory<NSPlugin>();
	createResourceFactory<NSScene>();
	createResourceFactory<NSTex1D>();
	createResourceFactory<NSTex1DArray>();
	createResourceFactory<NSTex2D>();
	createResourceFactory<NSTex2DArray>();
	createResourceFactory<NSTex3D>();
	createResourceFactory<NSTexRectangle>();
	createResourceFactory<NSTexBuffer>();
	createResourceFactory<NSTexCubeMap>();
	createResourceFactory<NSTexCubeMapArray>();
	createResourceFactory<NSTex2DMultisample>();
	createResourceFactory<NSTex2DMultisampleArray>();
	createResourceFactory<NSShader>();
	createResourceFactory<NSDirLightShader>();
	createResourceFactory<NSSpotLightShader>();
	createResourceFactory<NSPointLightShader>();
	createResourceFactory<NSMaterialShader>();
	createResourceFactory<NSParticleProcessShader>();
	createResourceFactory<NSParticleRenderShader>();
	createResourceFactory<NSDirShadowMapShader>();
	createResourceFactory<NSPointShadowMapShader>();
	createResourceFactory<NSSpotShadowMapShader>();
	createResourceFactory<NSEarlyZShader>();
	createResourceFactory<NSDirShadowMapXFBShader>();
	createResourceFactory<NSPointShadowMapXFBShader>();
	createResourceFactory<NSSpotShadowMapXFBShader>();
	createResourceFactory<NSEarlyZXFBShader>();
	createResourceFactory<NSRenderXFBShader>();
	createResourceFactory<NSXFBShader>();
	createResourceFactory<NSLightStencilShader>();
	createResourceFactory<NSSkyboxShader>();
	createResourceFactory<NSTransparencyShader>();
	createResourceFactory<NSSelectionShader>();
	createResourceManagerFactory<NSAnimManager>();
	createResourceManagerFactory<NSEntityManager>();
	createResourceManagerFactory<NSMatManager>();
	createResourceManagerFactory<NSMeshManager>();
	createResourceManagerFactory<NSSceneManager>();
	createResourceManagerFactory<NSShaderManager>();
	createResourceManagerFactory<NSTexManager>();
}

GLContext::GLContext() :glewContext(new GLEWContext()), engplug(new NSPlugin()), systems(new SystemMap()), plugins(new NSPluginManager()),mEvents(new NSEventHandler()), compositeBuf(0)
{
	engplug->rename(ENGINE_PLUG);
}

GLContext::~GLContext()
{
	delete plugins;
	delete systems;
	delete engplug;
	delete mEvents;
	delete glewContext;
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

nsuint HashedStringID(const nsstring & string)
{
	nsuint hash = 5381;
	nsint c;
	const char * str = string.c_str();
	while (c = *str++)
		hash = ((hash << 5) + hash) + c;

	return hash;
}