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
#include <typeindex>

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

typedef std::unordered_map<nsuint, NSSystem*> SystemMap;
typedef std::unordered_map<std::type_index, nsuint> TypeHashMap;
typedef std::unordered_map<nsuint, nsstring> HashMap;
typedef std::unordered_map<nsuint, NSFactory*> FactoryMap;
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
	typedef std::map<nsint, nsuint> SystemPriorityMap;

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

	NSPlugin * createPlugin(const nsstring & plugname, bool makeactive=true);

	template<class SysType>
	SysType * createSystem()
	{
		nsuint tid = typeID(std::type_index(typeid(SysType)));
		return static_cast<SysType*>(createSystem(tid));
	}

	NSSystem * createSystem(nsuint type_id);
	
	NSSystem * createSystem(const nsstring & guid_);

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

	bool delFramebuffer(nsuint fbid);

	template<class SysType>
	bool destroySystem()
	{
		nsuint tid = typeID(std::type_index(typeid(SysType)));
		return destroySystem(tid);
	}

	bool destroySystem(nsuint type_id);

	bool destroySystem(const nsstring & guid_);

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

	NSEventHandler * events();

	template<class T>
	bool hasPlugin(const T & name)
	{
		return current()->plugins->contains(name);
	}

	template<class ObjType>
	NSFactory * factory()
	{
		nsuint hashed_type = typeID(std::type_index(typeid(ObjType)));
		return factory(hashed_type);
	}

	NSFactory * factory(nsuint hashid);

	template<class BaseFacType>
	BaseFacType * factory(nsuint hashid)
	{
		return static_cast<BaseFacType*>(factory(hashid));
	}

	template<class BaseFacType>
	BaseFacType * factory(const nsstring & guid_)
	{
		return factory<BaseFacType>(hash_id(guid_)) ;
	}


	template<class BaseFacType, class ObjType>
	BaseFacType * factory()
	{
		nsuint hashed_type = typeID(std::type_index(typeid(ObjType)));
		return static_cast<BaseFacType*>(factory(hashed_type));
	}

	NSFrameBuffer * framebuffer(nsuint id);

	template<class SysType>
	nsbool hasSystem()
	{
		nsuint hashed_type = typeID(std::type_index(typeid(SysType)));
		return hasSystem(hashed_type);
	}

	nsbool hasSystem(nsuint type_id);
	
	nsbool hasSystem(const nsstring & guid_);

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

	template<class CompType>
	bool registerComponentType(const nsstring & guid_)
	{
		nsuint hashed = hash_id(guid_);
		auto ret = mObjTypeNames.emplace(hashed, guid_);
		
		if (!ret.second)
		{
			dprint(nsstring("registerComponentType - Could not generate unique hash from ") + guid_);
			return false;
		}

		std::type_index ti(typeid(CompType));
		auto check = mObjTypeHashes.emplace(ti, hashed);

		if (!check.second)
		{
			dprint(nsstring("registerComponentType - Could not generate unique type_index from ") + ti.name());
			return false;
		}

		return (createFactory<NSCompFactoryType<CompType>,CompType>() != NULL);		
	}

	template<class SysType>
	bool registerSystemType(const nsstring & guid_)
	{
		nsuint hashed = hash_id(guid_);
		auto ret = mObjTypeNames.emplace(hashed, guid_);
		
		if (!ret.second)
		{
			dprint(nsstring("registerSystemType - Could not generate unique hash from ") + guid_);
			return false;
		}

		std::type_index ti(typeid(SysType));
		auto check = mObjTypeHashes.emplace(ti, hashed);

		if (!check.second)
		{
			dprint(nsstring("registerSystemType - Could not generate unique type_index from ") + ti.name());
			return false;
		}

		return (createFactory<NSSysFactoryType<SysType>,SysType>() != NULL);
	}

	template<class ResType, class ManagerType>
	bool registerResourceType(const nsstring & guid_)
	{
		nsuint hashed = hash_id(guid_);
		auto ret = mObjTypeNames.emplace(hashed, guid_);
		
		if (!ret.second)
		{
			dprint(nsstring("registerResourceType - Could not generate unique hash from ") + guid_);
			return false;
		}

		std::type_index ti(typeid(ResType));
		auto check = mObjTypeHashes.emplace(ti, hashed);

		if (!check.second)
		{
			dprint(nsstring("registerResourceType - Could not generate unique type_index from ") + ti.name());
			return false;
		}

		std::type_index tim(typeid(ManagerType));
		auto fiter = mObjTypeHashes.find(tim);

		if (fiter == mObjTypeHashes.end())
		{
			dprint(nsstring("registerResourceType - Could not find hash_id for ") + tim.name() + nsstring(" - did you forget to register the manager first?"));
			return false;
		}


		NSResFactoryType<ResType> * rf = createFactory<NSResFactoryType<ResType>, ResType>();
		if (rf == NULL)
			return false;
		
		rf->mManagerID = fiter->second;
		return true;
	}

	template<class ManagerType>
	bool registerResourceManagerType(const nsstring & guid_)
	{
		nsuint hashed = hash_id(guid_);
		auto ret = mObjTypeNames.emplace(hashed, guid_);
		
		if (!ret.second)
		{
			dprint(nsstring("registerResourceManagerType - Could not generate unique hash from ") + guid_);
			return false;
		}

		std::type_index ti(typeid(ManagerType));
		auto check = mObjTypeHashes.emplace(ti, hashed);

		if (!check.second)
		{
			dprint(nsstring("registerResourceManagerType - Could not generate unique type_index from ") + ti.name());
			return false;
		}

		return (createFactory<NSResManagerFactoryType<ManagerType>,ManagerType>() != NULL);
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


	template<class SysType>
	SysType * removeSystem()
	{
		nsuint hashed_type = typeID(std::type_index(typeid(SysType)));
		return static_cast<SysType*>(removeSystem(hashed_type));
	}

	NSSystem * removeSystem(nsuint type_id);

	NSSystem * removeSystem(const nsstring & gui);

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

	nsstring guid(nsuint hash);

	nsstring guid(std::type_index type);

	nsuint typeID(std::type_index type);
	
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
		nsuint hashed_type = typeID(std::type_index(typeid(SysType)));
		return static_cast<SysType*>(system(hashed_type));
	}

	NSSystem * system(nsuint type_id);

	NSSystem * system(const nsstring & guid_);

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

	template<class ObjType>
	bool addFactory(NSFactory * fac)
	{
		if (fac == NULL)
			return false;

		std::type_index ti = std::type_index(typeid(ObjType));
		auto iter = mObjTypeHashes.find(ti);

		if (iter == mObjTypeHashes.end())
			return false;
		
		auto it = mFactories.emplace(iter->second, fac);
		return it.second;
	}
	
	template<class BaseFacType, class ObjType>
	BaseFacType * createFactory()
	{
		BaseFacType * fac = new BaseFacType;
		if (!addFactory<ObjType>(fac))
		{
			delete fac;
			return NULL;
		}
		return fac;
	}
	
	template<class ObjType>
	bool destroyFactory()
	{
		std::type_index ti = std::type_index(typeid(ObjType));
		auto iter = mObjTypeHashes.find(ti);

		if (iter == mObjTypeHashes.end())
			return false;

		return destroyFactory(iter.second);
	}

	bool destroyFactory(nsuint hashid);

	NSFactory * removeFactory(nsuint hashid);
	
	template<class BaseFacType,class ObjType>
	BaseFacType * removeFactory()
	{
		std::type_index ti = std::type_index(typeid(ObjType));
		auto iter = mObjTypeHashes.find(ti);

		if (iter == mObjTypeHashes.end())
			return false;

		return static_cast<BaseFacType*>(removeFactory(iter.second));
	}


	void _initSystems();
	void _initShaders();
	void _initMaterials();
	void _initMeshes();
	void _initInputMaps();
	void _initEntities();
	void _initDefaultFactories();
	void _removeSys(nsuint type_id);
	nsstring mResourceDirectory;
	nsstring mImportDirectory;
	
	SystemPriorityMap mSystemUpdateOrder;
	SystemPriorityMap mSystemDrawOrder;

	TypeHashMap mObjTypeHashes;
	HashMap mObjTypeNames;
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
