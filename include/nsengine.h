/*!
\file nsengine.h

\brief Header file for NSEngine class

This file contains all of the neccessary declartations for the NSEngine class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#ifndef NSENGINE_H
#define NSENGINE_H

#include <map>
#include <nsglobal.h>
#include <nsplugin.h>
#include <nsfactory.h>
#include <mutex>

class NSScene;
class NSRenderSystem;
class NSAnimManager;
class NSMeshManager;
class NSTexManager;
class NSMatManager;
class NSShaderManager;
class NSInputManager;
class NSEventHandler;
class NSAnimSystem;
class NSMovementSystem;
class NSTimer;
class NSPluginManager;
class NSSystem;

#ifdef NSDEBUG
class NSDebug;
#endif

struct GLContext;
class NSFrameBuffer;

typedef std::unordered_map<nsstring, NSSystem*> SystemMap;
typedef std::unordered_map<nsstring, NSFactory*> FactoryMap;
typedef std::unordered_map<nsuint, GLContext*> ContextMap;
typedef std::unordered_map<nsuint, NSFrameBuffer*> FramebufferMap;

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
class NSEngine
{
public:
	NSEngine();
	~NSEngine();
	typedef std::map<nsfloat, nsstring> SystemPriorityMap;

	template<class ObjType>
	bool addFactory(NSFactory * fac)
	{
		if (fac == NULL)
			return false;

		auto it = mFactories.emplace(ObjType::getTypeString(), fac);
		return it.second;
	}

	bool addPlugin(NSPlugin * plug);

	/* Add a system to the current context */
	bool addSystem(NSSystem * pSystem);

	template<class T>
	bool addResource(const T & plug_, NSResource * res)
	{
		NSPlugin * plug = plugin(plug_);
		if (plug != NULL)
			return plug->add(res);
		return false;
	}

	bool addResource(NSResource * res);

	NSPlugin * active();

	FactoryMap::iterator beginFac();

	SystemMap::iterator beginSys();

	template<class ResType, class T1, class T2>
	bool containsResource(const T1 & plug_, const T2 & res_)
	{
		NSResource * res = nsengine.resource<ResType>(plug_, res_);
		return (res != NULL);
	}

	/*!
	Create a GLContext and return a unique id - this id can be used to set the current context and
	delete the context later

	This does not initialize the context - call start/shutdown to initialize and release opengl and all
	resources associated with the context

	The newly created context will be set as the current context.
	*/
	nsuint createContext(nsbool addDefaultFactories = true //<! Add the normal system/component/resource/resource manager factories
		);

	nsuint createFramebuffer();

	template<class ObjType>
	NSFactory * createResourceManagerFactory()
	{
		NSFactory * fac = new NSResManagerFactoryType<ObjType>();
		if (!addFactory<ObjType>(fac))
		{
			delete fac;
			return NULL;
		}
		return fac;
	}

	template<class ObjType>
	NSFactory * createResourceFactory()
	{
		NSFactory * fac = new NSResFactoryType<ObjType>();
		if (!addFactory<ObjType>(fac))
		{
			delete fac;
			return NULL;
		}
		return fac;
	}

	template<class ObjType>
	NSFactory * createSystemFactory()
	{
		NSFactory * fac = new NSSysFactoryType<ObjType>();
		if (!addFactory<ObjType>(fac))
		{
			delete fac;
			return NULL;
		}
		return fac;
	}

	template<class ObjType>
	NSFactory * createComponentFactory()
	{
		NSFactory * fac = new NSCompFactoryType<ObjType>();
		if (!addFactory<ObjType>(fac))
		{
			delete fac;
			return NULL;
		}
		return fac;
	}

	NSPlugin * createPlugin(const nsstring & plugname, bool makeactive=true);

	template<class SysType>
	SysType * createSystem()
	{
		SysType * system = new SysType();
		system->init();
		if (!addSystem(system))
		{
			delete system;
			return NULL;
		}
		return system;
	}

	NSSystem * createSystem(const nsstring & systype);

	template<class ResType, class T>
	ResType * createResource(const T & plug_, const nsstring & resName)
	{
		NSPlugin * plug = plugin(plug_);
		if (plug != NULL)
			return plug->create<ResType>(resName);
		return NULL;
	}

	template<class ResType>
	ResType * createResource(const nsstring & resName)
	{
		NSPlugin * plug = active();
		return createResource<ResType>(plug, resName);
	}

	GLContext * current();

	nsuint currentid();

	NSScene * currentScene();

	/*!
	Delete the context with ID cID
	Make sure shutdown has been called with the context to be deleted set as the current context
	*/
	bool delContext(nsuint cID);

	template<class T>
	bool delPlugin(const T & name)
	{
		return current()->plugins->del(name);
	}

	template<class ObjType>
	bool delFactory()
	{
		return delFactory(ObjType::getTypeString());
	}

	bool delFactory(const nsstring & objtype);

	bool delFramebuffer(nsuint fbid);

	template<class SysType>
	bool delSystem()
	{
		return delSystem(SysType::getTypeString());
	}

	bool delSystem(const nsstring & systype);

	template<class ResType, class T1, class T2>
	nsbool delResource(const T1 & plug, const T2 & res)
	{
		return delResource(resource<ResType>(plug, res));
	}

	nsbool delResource(NSResource * res);

#ifdef NSDEBUG
	void debugPrint(const nsstring & str);
#endif

	FactoryMap::iterator endFac();

	SystemMap::iterator endSys();

	NSPlugin * engplug();

	NSPluginManager * plugins();

	NSFactory * factory(const nsstring & typeString);

	template<class BaseFacType>
	BaseFacType * factory(const nsstring & typeString)
	{
		auto fIter = mFactories.find(typeString);
		if (fIter == mFactories.end())
			return NULL;
		return static_cast<BaseFacType*>(fIter->second);
	}

	NSEventHandler * events();

	template<class ObjType>
	nsbool hasFactory()
	{
		auto fIter = mFactories.find(ObjType::getTypeString());
		return (fIter != mFactories.end());
	}

	nsbool hasFactory(const nsstring & pObjType);

	template<class T>
	bool hasPlugin(const T & name)
	{
		return current()->plugins->contains(name);
	}

	NSFrameBuffer * framebuffer(nsuint id);

	template<class SysType>
	nsbool hasSystem()
	{
		auto fIter = mSystems.find(SysType::getTypeString());
		return (fIter != mSystems.end());
	}

	nsbool hasSystem(const nsstring & pTypeString);

	NSPlugin * loadPlugin(const nsstring & fname, bool appendDirs = true);

	template<class ResType>
	ResType * loadResource(nsuint plugid, const nsstring & fileName, bool appendDirs = true)
	{
		NSPlugin * plug = plugin(plugid);

		if (plug == NULL)
			return NULL;

		return plug->load<ResType>(fileName, appendDirs);
	}

	template<class ResType>
	ResType * loadResource(const nsstring & fileName, bool appendDirs = true)
	{
		NSPlugin * plug = active();

		if (plug == NULL)
			return NULL;

		return plug->load<ResType>(fileName, appendDirs);
	}

	void save(NSSaveResCallback * scallback=NULL);

	bool save(const nsstring & plugname, NSSaveResCallback * scallback=NULL);

	bool save(nsuint plugid, NSSaveResCallback * scallback=NULL);

	bool save(NSPlugin * plugtosave, NSSaveResCallback * scallback=NULL);

	void savecore(NSSaveResCallback * scallback=NULL);

	void setPluginDirectory(const nsstring & plugdir);

	const nsstring & pluginDirectory();

	template < class ManagerType, class T>
	ManagerType * manager(const T & plug_)
	{
		NSPlugin * plug = plugin(plug_);
		if (plug == NULL)
			return NULL;
		return plug->manager<ManagerType>();
	}

	template<class T>
	NSEntity * loadModel(const T & plug_, const nsstring & entname, const nsstring & fname, bool prefixWithImportDir, const nsstring & meshname = "", bool flipuv = true)
	{
		NSPlugin * plug = plugin(plug_);
		if (plug == NULL)
			return NULL;
		return plug->loadModel(entname, fname, prefixWithImportDir, meshname, flipuv);
	}

	NSEntity * loadModel(const nsstring & entname, const nsstring & fname, bool prefixWithImportDir, const nsstring & meshname = "", bool flipuv = true);

	template<class T>
	bool loadModelResources(const T & plug_, const nsstring & fname, bool prefixWithImportDir, const nsstring & meshname = "", bool flipuv = true)
	{
		NSPlugin * plug = plugin(plug_);
		if (plug == NULL)
			return NULL;
		return plug->loadModelResources(fname, prefixWithImportDir, meshname, flipuv);
	}

	bool loadModelResources(const nsstring & fname, bool prefixWithImportDir, const nsstring & meshname = "", bool flipuv = true);

	template<class T>
	NSPlugin * removePlugin(const T & name)
	{
		return current()->plugins->remove<NSPlugin>(name);
	}

	NSFactory * removeFactory(const nsstring & objtype);

	template<class BaseFacType>
	BaseFacType * removeFactory(const nsstring & objtype)
	{
		BaseFacType * fac = factory<BaseFacType>(objtype);
		if (fac == NULL)
			return NULL;
		mFactories.erase(objtype);
		return fac;
	}

	template<class SysType>
	SysType * removeSystem()
	{
		SysType * sys = system<SysType>();
		if (sys == NULL)
			return NULL;
		current()->systems->erase(sys->typeString());
		_removeSys(sys->typeString());
		return sys;
	}

	NSSystem * removeSystem(const nsstring & sysType);

	template<class ResType, class T1, class T2>
	ResType * removeResource(const T1 & plug, const T2 & res)
	{
		NSResource * res = removeResource(resource<ResType>(plug, res));
		if (res != NULL)
			return static_cast<ResType*>(res);
		return NULL;
	}

	NSResource * removeResource(NSResource * res);

	NSPlugin * plugin(const nsstring & name);

	NSPlugin * plugin(nsuint id);

	NSPlugin * plugin(NSPlugin * plug);

	template<class T>
	bool unloadPlugin(const T & name)
	{
		return unloadPlugin(plugin(name));
	}

	bool unloadPlugin(NSPlugin * plug);

	
	template<class ObjType>
	NSFactory * createComponentFactory()
	{
		NSFactory * fac = new NSCompFactoryType<ObjType>();
		if (!addFactory<ObjType>(fac))
		{
			delete fac;
			return NULL;
		}
		return fac;
	}


	template<class ResType>
	ResType * resource(const uivec2 & resID)
	{
		return resource<ResType>(resID.x, resID.y);
	}

	template<class ResType, class T1, class T2>
	ResType * resource(const T1 & plug_, const T2 & res)
	{
		NSPlugin * plug = plugin(plug_);
		if (plug == NULL)
			return NULL;

		return plug->resource<ResType>(res);
	}

	bool resourceChanged(NSResource * res);

	/*!
	Overload of Propagate name change
	*/
	void nameChange(const uivec2 & oldid, const uivec2 newid);

	const nsstring & resourceDirectory();

	template<class ResType, class T1, class T2>
	nsbool saveResource(const T1 & plug_, const T2 & name)
	{
		NSPlugin * plug = plugin(plug_);
		if (plug != NULL)
			return plug->save(name);
		return false;
	}

	void setActive(const nsstring & plugname);

	void setActive(NSPlugin * plug);

	void setActive(nsuint plugid);

	void setCurrentScene(const nsstring & scn, bool newSceneOverwriteFile = false, bool saveprevious = false);

	void setCurrentScene(NSScene * scn, bool newSceneOverwriteFile = false, bool saveprevious = false);

	void setCurrentScene(nsuint scn, bool newSceneOverwriteFile = false, bool saveprevious = false);

	bool makeCurrent(nsuint cID);

	const nsstring & importdir();

	void setImportDir(const nsstring & dir);

	void setResourceDir(const nsstring & dir);

	void shutdown();

	void start();

	template<class SysType>
	SysType * system()
	{
		GLContext * cont = current();
		auto fIter = cont->systems->find(SysType::getTypeString());
		if (fIter == cont->systems->end())
			return NULL;
		return (SysType*)fIter->second;
	}

	NSSystem * system(const nsstring & pTypeName);

	NSTimer * timer();

	template<class ResType, class T1, class T2>
	nsbool unloadResource(const T1 & plug_, const T2 & res)
	{
		NSPlugin * plug = plugin(plug_);
		if (plug != NULL)
			return plug->unload<ResType>(res);
		return false;
	}

	nsbool unloadResource(NSResource * res);

	void update();

#ifdef NSDEBUG
	NSDebug * debug();
#endif

	/*!
	Get engine instance.. if this is the first call then a new instance is created (static)
	*/
	static NSEngine & inst();

private:
	void _initSystems();
	void _initShaders();
	void _initMaterials();
	void _initMeshes();
	void _initInputMaps();
	void _initEntities();
	void _initDefaultFactories();
	void _removeSys(const nsstring & systype);
	nsstring mResourceDirectory;
	nsstring mImportDirectory;
	
	SystemPriorityMap mSystemUpdateOrder;
	SystemPriorityMap mSystemDrawOrder;


	FactoryMap mFactories;
	ContextMap mContexts;
	nsuint mCurrentContext;
	nsstring cwd;
	std::mutex mt;
};

struct GLContext
{
	GLContext(nsuint id);
	~GLContext();
	GLEWContext * glewContext;
	NSPlugin * engplug;
	SystemMap * systems;
	NSPluginManager * plugins;
	NSEventHandler * mEvents;
	FramebufferMap fbmap;
	NSTimer * timer;
	nsuint compositeBuf;
	nsuint context_id;
#ifdef NSDEBUG
	NSDebug * mDebug;
#endif
	
};

nsuint hash_id(const nsstring & str);

#endif
