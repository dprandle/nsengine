/*!
\file nsplugin.cpp

\brief Definition file for NSPlugin class

This file contains all of the neccessary definitions for the NSPlugin class.

\author Daniel Randle
\date August 23 2014
\copywrite Earth Banana Games 2013
*/
#include <nsplugin.h>
#include <nsengine.h>
#include <nsresmanager.h>
#include <nsscenemanager.h>
#include <nsfactory.h>
#include <nsentity.h>
#include <nsanimcomp.h>
#include <nsmatmanager.h>
#include <nsanimmanager.h>
#include <nsoccupycomp.h>
#include <nsrendercomp.h>
#include <nsinputcomp.h>
#include <nsterraincomp.h>
#include <nsmeshmanager.h>
#include <nsselcomp.h>
#include <nstilecomp.h>
#include <nspluginmanager.h>

NSPlugin::NSPlugin() : 
	mNotes(),
	mCreator(),
	mCreationDate(),
	mEditDate(),
	mBound(false),
	mManagers(),
	mAddname(true),
	NSResource()
{
	setExtension(DEFAULT_PLUGIN_EXTENSION);
}

NSPlugin::~NSPlugin()
{
	auto iter = mManagers.begin();
	while (iter != mManagers.end())
	{
		delete iter->second;
		++iter;
	}
	mManagers.clear();
}

bool NSPlugin::add(NSResource * res)
{
	if (res == NULL)
		return false;

	NSResManager * rm = manager(nsengine.managerID(res->type()));
	return rm->add(res);
}

void NSPlugin::addNameToResPath(bool add_)
{
	mAddname = add_;
	auto iter = mManagers.begin();
	while (iter != mManagers.end())
	{
		if (mAddname)
			iter->second->setResourceDirectory(mResDir + mName + "/");
		else
			iter->second->setResourceDirectory(mResDir);
		++iter;
	}
}

bool NSPlugin::addingNameToResPath()
{
	return mAddname;
}

bool NSPlugin::addManager(NSResManager * manag)
{
	if (manag == NULL)
		return false;

	auto iter = mManagers.emplace(manag->type(), manag);
	if (iter.second)
	{
		manag->setPlugID(mID);
		if (mAddname)
			manag->setResourceDirectory(mResDir + mName + "/");
		else
			manag->setResourceDirectory(mResDir);
		return true;
	}
	return false;
}

NSResource * NSPlugin::create(uint32 res_typeid, const nsstring & resName)
{
	NSResManager * rm = manager(nsengine.managerID(res_typeid));
	return rm->create(res_typeid, resName);
}

NSEntity * NSPlugin::createCamera(const nsstring & name, float fov, const uivec2 & screenDim, const fvec2 & clipnf)
{
	NSEntity * cament = create<NSEntity>(name);
	if (cament == NULL)
		return NULL;
	NSCamComp * camc = cament->create<NSCamComp>();
	if (camc == NULL)
		return NULL;

	camc->setfov(fov);
	camc->resize(screenDim.w, screenDim.h);
	camc->setPerspNFClip(clipnf);
	camc->setProjectionMode(NSCamComp::Persp);

	NSInputComp *ic = cament->create<NSInputComp>();
	ic->add(CAM_BACKWARD);
	ic->add(CAM_FORWARD);
	ic->add(CAM_LEFT);
	ic->add(CAM_MOVE);
	ic->add(CAM_RIGHT);
	ic->add(CAM_TURN);
	ic->add(CAM_ZOOM);
	return cament;
}

bool NSPlugin::contains(NSResource * res)
{
	NSResManager * rm = manager(nsengine.managerID(res->type()));
	return rm->contains(res);
}

NSEntity * NSPlugin::createCamera(const nsstring & name, const fvec2 & lrclip, const fvec2 & tbclip, const fvec2 & nfclip)
{
	NSEntity * cament = create<NSEntity>(name);
	if (cament == NULL)
		return NULL;

	NSCamComp * camc = cament->create<NSCamComp>();
	if (camc == NULL)
		return NULL;

	camc->setOrthoLRClip(lrclip);
	camc->setOrthoTBClip(tbclip);
	camc->setOrthoNFClip(nfclip);
	camc->setProjectionMode(NSCamComp::Ortho);
	NSInputComp *ic = cament->create<NSInputComp>();
	ic->add(CAM_BACKWARD);
	ic->add(CAM_FORWARD);
	ic->add(CAM_LEFT);
	ic->add(CAM_MOVE);
	ic->add(CAM_RIGHT);
	ic->add(CAM_TURN);
	ic->add(CAM_ZOOM);
	return cament;
}

NSEntity * NSPlugin::createDirLight(const nsstring & name,
	float diffuse,
	float ambient,
	const fvec3 & color,
	bool castshadows,
	float shadowdarkness,
	int32 shadowsamples)
{
	NSMesh * bounds = nsengine.engplug()->get<NSMesh>(MESH_DIRLIGHT_BOUNDS);
	if (bounds == NULL)
		return NULL;
	NSEntity * lt = create<NSEntity>(name);
	if (lt == NULL)
		return NULL;
	NSLightComp * lc = lt->create<NSLightComp>();
	if (lc == NULL)
	{
		destroy(lt);
		return NULL;
	}
	lc->setMeshID(bounds->plugid(), bounds->id());
	lc->setType(NSLightComp::Directional);
	lc->setIntensity(diffuse, ambient);
	lc->setColor(color);
	lc->setCastShadows(castshadows);
	lc->setShadowClipping(fvec2(DEFAULT_Z_NEAR, DEFAULT_Z_FAR));
	lc->setShadowDarkness(shadowdarkness);
	lc->setShadowSamples(shadowsamples);
	return lt;
}

NSEntity * NSPlugin::createPointLight(const nsstring & name,
	float diffuse,
	float ambient,
	float distance,
	const fvec3 & color,
	bool castshadows,
	float shadowdarkness,
	int32 shadowsamples)
{
	NSMesh * bounds = nsengine.engplug()->get<NSMesh>(MESH_POINTLIGHT_BOUNDS);
	if (bounds == NULL)
		return NULL;
	NSEntity * lt = create<NSEntity>(name);
	if (lt == NULL)
		return NULL;
	NSLightComp * lc = lt->create<NSLightComp>();
	if (lc == NULL)
	{
		destroy(lt);
		return NULL;
	}
	lc->setMeshID(bounds->plugid(), bounds->id());
	lc->setType(NSLightComp::Point);
	lc->setIntensity(diffuse, ambient);
	lc->setColor(color);
	lc->setDistance(distance);
	lc->setCastShadows(castshadows);
	lc->setShadowClipping(fvec2(DEFAULT_Z_NEAR, DEFAULT_Z_FAR));
	lc->setShadowDarkness(shadowdarkness);
	lc->setShadowSamples(shadowsamples);

	NSInputComp * inComp = lt->create<NSInputComp>();
	inComp->add(SELECT);
	inComp->add(ROTATE_X);
	inComp->add(ROTATE_Y);
	inComp->add(ROTATE_Z);
	inComp->add(MULTI_SELECT);
	inComp->add(PAINT_SELECT);
	inComp->add(DRAG_OBJECT_XY);
	inComp->add(DRAG_OBJECT_XZ);
	inComp->add(DRAG_OBJECT_YZ);
	lt->create<NSSelComp>();

	NSRenderComp * rc = lt->create<NSRenderComp>();
	rc->setMeshID(bounds->fullid());
	rc->setCastShadow(false);
	return lt;
}

NSEntity * NSPlugin::createSpotLight(const nsstring & name,
float diffuse,
float ambient,
float distance,
float radius,
const fvec3 & color,
bool castshadows,
float shadowdarkness,
int32 shadowsamples)
{
	NSMesh * bounds = nsengine.engplug()->get<NSMesh>(MESH_SPOTLIGHT_BOUNDS);
	if (bounds == NULL)
		return NULL;
	NSEntity * lt = create<NSEntity>(name);
	if (lt == NULL)
		return NULL;
	NSLightComp * lc = lt->create<NSLightComp>();
	if (lc == NULL)
	{
		destroy(lt);
		return NULL;
	}
	lc->setMeshID(bounds->plugid(), bounds->id());
	lc->setType(NSLightComp::Spot);
	lc->setIntensity(diffuse, ambient);
	lc->setColor(color);
	lc->setCastShadows(castshadows);
	lc->setShadowClipping(fvec2(DEFAULT_Z_NEAR, DEFAULT_Z_FAR));
	lc->setShadowDarkness(shadowdarkness);
	lc->setShadowSamples(shadowsamples);
	lc->setDistance(distance);
	lc->setRadius(radius);

	NSRenderComp * rc = lt->create<NSRenderComp>();
	rc->setMeshID(bounds->fullid());
	rc->setCastShadow(false);

	NSInputComp * inComp = lt->create<NSInputComp>();
	inComp->add(SELECT);
	inComp->add(ROTATE_X);
	inComp->add(ROTATE_Y);
	inComp->add(ROTATE_Z);
	inComp->add(MULTI_SELECT);
	inComp->add(PAINT_SELECT);
	inComp->add(DRAG_OBJECT_XY);
	inComp->add(DRAG_OBJECT_XZ);
	inComp->add(DRAG_OBJECT_YZ);
	lt->create<NSSelComp>();

	return lt;
}

NSResManager * NSPlugin::createManager(const nsstring & manager_guid)
{
	return createManager(hash_id(manager_guid));
}

NSResManager * NSPlugin::createManager(uint32 manager_typeid)
{
	NSResManagerFactory * factory = nsengine.factory<NSResManagerFactory>(manager_typeid);
	NSResManager * man = factory->create();
	if (!addManager(man))
	{
		delete man;
		return NULL;
	}
	return man;
}

NSEntity * NSPlugin::createTile(const nsstring & name,
	const nsstring & difftex,
	const nsstring & normtex,
	fvec3 m_col,
	float s_pwr,
	float s_int32,
	fvec3 s_col,
	bool collides,
	Tile_t type)
{
	NSMaterial * mat = create<NSMaterial>(name);
	if (mat == NULL)
		return NULL;

	mat->setCullMode(GL_BACK);
	mat->enableCulling(true);
	mat->setSpecularColor(s_col);
	mat->setColor(m_col);
	mat->setSpecularPower(s_pwr);
	mat->setSpecularIntensity(s_int32);
	NSTexture * tdiff = load<NSTex2D>(difftex);
	NSTexture * tnorm = load<NSTex2D>(normtex);
	if (tdiff != NULL)
		mat->setMapTextureID(NSMaterial::Diffuse, tdiff->fullid());
	if (tnorm != NULL)
		mat->setMapTextureID(NSMaterial::Normal, tnorm->fullid());
	if (tdiff == NULL && tnorm == NULL)
		mat->setColorMode(true);
	return createTile(name, mat, collides, type);
}

NSEntity * NSPlugin::createTile(const nsstring & name,
	NSMaterial * mat, bool collides, Tile_t type)
{
	NSEntity * ent = create<NSEntity>(name);
	if (ent == NULL)
		return NULL;

	NSRenderComp * rc = ent->create<NSRenderComp>();
	NSInputComp * ic = ent->create<NSInputComp>();
	NSSelComp * sc = ent->create<NSSelComp>();
	NSTileComp * tc = ent->create<NSTileComp>();
	if (rc == NULL || ic == NULL || sc == NULL || tc == NULL)
	{
		destroy(ent);
		return NULL;
	}

	NSMesh * msh = NULL;
	if (type == Full)
		msh = nsengine.engplug()->get<NSMesh>(MESH_FULL_TILE);
	else
		msh = nsengine.engplug()->get<NSMesh>(MESH_HALF_TILE);

	if (collides)
	{
		NSOccupyComp * oc = ent->create<NSOccupyComp>();
		oc->build(msh->aabb());
		oc->setMeshID(msh->fullid());
	}

	rc->setMeshID(msh->fullid());
	if (mat != NULL)
		rc->setMaterial(0, mat->fullid());

	ic->add(SELECT);
	ic->add(ROTATE_X);
	ic->add(ROTATE_Z);
	ic->add(MULTI_SELECT);
	ic->add(PAINT_SELECT);
	ic->add(DRAG_OBJECT_XY);
	ic->add(DRAG_OBJECT_XZ);
	ic->add(DRAG_OBJECT_YZ);
	ic->add(XZ_MOVE_END);
	ic->add(YZ_MOVE_END);
	ic->add(SHIFT_DONE);
	return ent;
}

NSEntity * NSPlugin::createTile(const nsstring & name,
	const nsstring & matname, bool collides, Tile_t type)
{
	return createTile(name, get<NSMaterial>(matname), collides, type);
}

NSEntity * NSPlugin::createTile(const nsstring & name,
	uint32 matid, bool collides, Tile_t type)
{
	return createTile(name, get<NSMaterial>(matid), collides, type);
}

NSEntity * NSPlugin::createTerrain(const nsstring & name, 
	float hmin, 
	float hmax, 
	const nsstring & hmfile, 
	const nsstring & dmfile, 
	const nsstring & nmfile,
	bool importdir)
{
	NSEntity * terr = create<NSEntity>(name);
	nullchkn(terr);
	NSTerrainComp * tc = terr->create<NSTerrainComp>();
	tc->setHeightBounds(hmin, hmax);
	NSRenderComp * rc = terr->create<NSRenderComp>();
	rc->setCastShadow(true);
	rc->setMeshID(nsengine.engplug()->get<NSMesh>(MESH_TERRAIN)->fullid());
	
	NSMaterial * termat = create<NSMaterial>(name);
	if (termat == NULL)
	{
		destroy(terr);
		return NULL;
	}
	termat->enableCulling(false);
	rc->setMaterial(0, termat->fullid(), true);

	NSTex2D * hm = NULL, * dm = NULL, * nm = NULL;
	if (importdir)
		hm = load<NSTex2D>(mImportDir + hmfile);
	else
		hm = load<NSTex2D>(hmfile);

	if (hm == NULL)
	{
		destroy(termat);
		destroy(terr);
		return NULL;
	}

	hm->setParameteri(NSTexture::WrapS, GL_REPEAT);
	hm->setParameteri(NSTexture::WrapT, GL_REPEAT);

	if (!dmfile.empty())
	{
		
		if (importdir)
			dm = load<NSTex2D>(mImportDir + dmfile);
		else
			dm = load<NSTex2D>(dmfile);

		if (dm == NULL)
		{
			destroy(hm);
			destroy(termat);
			destroy(terr);
			return NULL;
		}
		dm->enableMipMaps();
	}

	if (!nmfile.empty())
	{
		if (importdir)
			nm = load<NSTex2D>(mImportDir + nmfile);
		else
			nm = load<NSTex2D>(nmfile);

		if (nm == NULL)
		{
			destroy(dm);
			destroy(hm);
			destroy(termat);
			destroy(terr);
			return NULL;
		}
	}

	termat->setMapTextureID(NSMaterial::Height, hm->fullid(), true);
	if (dm != NULL)
		termat->setMapTextureID(NSMaterial::Diffuse, dm->fullid(), true);
	if (nm != NULL)
		termat->setMapTextureID(NSMaterial::Normal, nm->fullid(), true);

	
	return terr;
}

NSScene * NSPlugin::currentScene()
{
	NSSceneManager * sm = manager<NSSceneManager>();
	return sm->current();
}

bool NSPlugin::destroyManager(const nsstring & manager_guid)
{
	return destroyManager(hash_id(manager_guid));
}

bool NSPlugin::destroyManager(uint32 manager_typeid)
{
	NSResManager * resman = removeManager(manager_typeid);
	delete resman;
	return true;
}

bool NSPlugin::del(NSResource * res)
{
	NSResManager * rm = manager(nsengine.managerID(res->type()));
	return rm->del(res);
}

bool NSPlugin::hasParent(const nsstring & pname)
{
	auto fiter = mParents.find(pname);
	return (fiter != mParents.end());
}

void NSPlugin::nameChange(const uivec2 & oldid, const uivec2 newid)
{
	auto iter = mManagers.begin();
	while (iter != mManagers.end())
	{
		// If the plugin part of id is zero it means the plugin itself had a name change.. therefor
		// propagate a plugin name change through all resources using the get id that was passed in
		// Otherwise propagate name change like normal
		if (oldid.x == 0)
			iter->second->nameChange(uivec2(oldid.yx()), uivec2(newid.yx()));
		else
			iter->second->nameChange(oldid, newid);
		++iter;
	}
}

NSResource * NSPlugin::get(uint32 res_typeid, uint32 resid)
{
	NSResManager * rm = manager(nsengine.managerID(res_typeid));
	return rm->get(resid);
}

NSResource * NSPlugin::get(uint32 res_typeid, const nsstring & resName)
{
	NSResManager * rm = manager(nsengine.managerID(res_typeid));
	return rm->get(resName);
}

void NSPlugin::init()
{
	auto fiter = nsengine.beginFac();
	NSResManager * rm = NULL;
	while (fiter != nsengine.endFac())
	{
		NSFactory * plug_man_fac = nsengine.factory<NSPluginManager>();
		if (fiter->second->type() == NSFactory::ResManager && fiter->second != plug_man_fac)
		{
			rm = createManager(nsengine.guid(fiter->first));

			if (mAddname)
				rm->setResourceDirectory(mResDir + mName + "/");
			else
				rm->setResourceDirectory(mResDir);
		}

		++fiter;
	}
}

NSResource * NSPlugin::load(uint32 res_typeid, const nsstring & fname)
{
	NSResManager * rm = manager(nsengine.managerID(res_typeid));
	return rm->load(res_typeid, fname);
}

NSEntity * NSPlugin::loadModel(const nsstring & entname, nsstring fname, bool prefixWithImportDir, const nsstring & meshname, bool flipuv)
{
	if (prefixWithImportDir)
		fname = mImportDir + fname;

	NSEntity * ent = create<NSEntity>(entname);
	NSRenderComp * renderComp = ent->create<NSRenderComp>();

	nsstring sceneName = meshname;
	if (sceneName.empty())
	{
		sceneName = fname.substr(fname.find_last_of("/\\") + 1);
		sceneName = sceneName.erase(sceneName.find_last_of("."), nsstring::npos);
	}

	nsstring dir = fname.substr(0, fname.find_last_of("/\\") + 1);

	uint32 flag = 0;

	if (flipuv)
	{
		flag = aiProcess_ConvertToLeftHanded;
	}

	Assimp::Importer importer;
	const aiScene * scene = importer.ReadFile(fname.c_str(),
		aiProcess_JoinIdenticalVertices |
		aiProcess_Triangulate |
		aiProcess_SortByPType |
		aiProcess_ImproveCacheLocality |
		flag |
		aiProcess_GenUVCoords |
		aiProcess_TransformUVCoords |
		aiProcess_FindDegenerates |
		aiProcess_ValidateDataStructure |
		aiProcess_OptimizeMeshes |
		aiProcess_OptimizeGraph |
		aiProcess_GenSmoothNormals | // Use this always - if normals included this process wont happen
		aiProcess_RemoveRedundantMaterials |
		aiProcess_FixInfacingNormals |
		aiProcess_FindInvalidData
		// May include optimizing scene etc here
		);


	NSMesh * renderMesh = manager<NSMeshManager>()->assimpLoadMeshFromScene(scene, sceneName);
	renderComp->setMeshID(mID, renderMesh->id());

	if (scene->HasMaterials())
	{
		std::map<uint32, uint32> indexMap;
		for (uint32 i = 0; i < scene->mNumMaterials; ++i)
		{
			nsstringstream ss;
			ss << sceneName << "_" << i;
			const aiMaterial* aiMat = scene->mMaterials[i];
			NSMaterial * mat = manager<NSMatManager>()->assimpLoadMaterial(ss.str(), aiMat, dir);
			if (mat != NULL)
				indexMap[i] = mat->id();
		}

		for (uint32 i = 0; i < scene->mNumMeshes; ++i)
		{
			const aiMesh* mesh = scene->mMeshes[i];
			if (mesh->mPrimitiveTypes != aiPrimitiveType_TRIANGLE)
				continue;

			uint32 mI = mesh->mMaterialIndex;
			auto fIter = indexMap.find(mI);
			if (fIter != indexMap.end())
				renderComp->setMaterial(i, mID, fIter->second);
		}
	}

	if (scene->HasAnimations())
	{
		NSAnimSet * animSet = manager<NSAnimManager>()->assimpLoadAnimationSet(scene, sceneName);
		NSAnimComp * animComp = ent->create<NSAnimComp>();
		animComp->setAnimationSetID(mID, animSet->id());
	}

	NSInputComp * inComp = ent->create<NSInputComp>();
	inComp->add(SELECT);
	inComp->add(ROTATE_X);
	inComp->add(ROTATE_Y);
	inComp->add(ROTATE_Z);
	inComp->add(MULTI_SELECT);
	inComp->add(PAINT_SELECT);
	inComp->add(DRAG_OBJECT_XY);
	inComp->add(DRAG_OBJECT_XZ);
	inComp->add(DRAG_OBJECT_YZ);
	ent->create<NSSelComp>();
	return ent;
}

bool NSPlugin::loadModelResources(nsstring fname,bool prefixWithImportDir, const nsstring & meshname, bool flipuv)
{
	if (prefixWithImportDir)
		fname = mImportDir + fname;

	nsstring sceneName = meshname;
	if (sceneName.empty())
	{
		sceneName = fname.substr(fname.find_last_of("/\\") + 1);
		sceneName = sceneName.erase(sceneName.find_last_of("."), nsstring::npos);
	}

	nsstring dir = fname.substr(0, fname.find_last_of("/\\") + 1);

	uint32 flag = 0;

	if (flipuv)
	{
		flag = aiProcess_ConvertToLeftHanded;
	}

	Assimp::Importer importer;
	const aiScene * scene = importer.ReadFile(fname.c_str(),
		aiProcess_JoinIdenticalVertices |
		aiProcess_Triangulate |
		aiProcess_SortByPType |
		flag |
		aiProcess_GenUVCoords |
		aiProcess_TransformUVCoords |
		aiProcess_ImproveCacheLocality |
		aiProcess_FindDegenerates |
		aiProcess_ValidateDataStructure |
		aiProcess_OptimizeMeshes |
		aiProcess_OptimizeGraph |
		aiProcess_GenSmoothNormals | // Use this always - if normals included this process wont happen
		aiProcess_RemoveRedundantMaterials |
		aiProcess_FixInfacingNormals | // may give incorrect results sometimes - usually not though
		aiProcess_FindInvalidData
		// May include optimizing scene etc here
		);

	NSMesh * mesh = manager<NSMeshManager>()->assimpLoadMeshFromScene(scene, sceneName);

	if (scene->HasMaterials())
	{
		for (uint32 i = 0; i < scene->mNumMaterials; ++i)
		{
			nsstringstream ss;
			ss << sceneName << "_" << i;
			const aiMaterial* aiMat = scene->mMaterials[i];
			NSMaterial * mat = manager<NSMatManager>()->assimpLoadMaterial(ss.str(), aiMat, dir);
		}
	}

	if (scene->HasAnimations())
		manager<NSAnimManager>()->assimpLoadAnimationSet(scene, sceneName);

	return true;
}

NSResManager * NSPlugin::manager(const nsstring & manager_guid)
{
	return manager(hash_id(manager_guid));
}

NSResManager * NSPlugin::manager(uint32 manager_typeid)
{
	auto iter = mManagers.find(manager_typeid);
	if (iter == mManagers.end())
		return NULL;
	return iter->second;	
}

bool NSPlugin::hasManager(uint32 manager_typeid)
{
	return (mManagers.find(manager_typeid) != mManagers.end());
}

bool NSPlugin::hasManager(const nsstring & manager_guid)
{
	return hasManager(hash_id(manager_guid));
}

bool NSPlugin::bound()
{
	return mBound;
}

const nsstring & NSPlugin::creator()
{
	return mCreator;
}

const nsstring & NSPlugin::notes()
{
	return mNotes;
}

const nsstring & NSPlugin::editDate()
{
	return mEditDate;
}

const nsstring & NSPlugin::creationDate()
{
	return mCreationDate;
}

const nsstringset & NSPlugin::parents()
{
	if (mBound)
		_updateParents();
	return mParents;
}

bool NSPlugin::bind()
{
	if (!parentsLoaded())
		return false;

	auto liter = resmap.begin();
	while (liter != resmap.end())
	{
		NSResManager * rm = manager(liter->first);
		NSResource * r = rm->load(liter->second.first,liter->second.second);
		if (r == NULL)
			unloaded.emplace(liter->first, liter->second);
		++liter;
	}

	return (mBound = true);
}

bool NSPlugin::unbind()
{
	_updateResMap();
	_updateParents();
	_clear();
	unloaded.clear();
	mBound = false;
	return !mBound;
}

void NSPlugin::saveAll(const nsstring & path, NSSaveResCallback * scallback)
{
	auto iter = mManagers.begin();
	while (iter != mManagers.end())
	{
		iter->second->saveAll(path, scallback);
		++iter;
	}
}

void NSPlugin::saveAll(uint32 res_typeid, const nsstring & path, NSSaveResCallback * scallback)
{
	NSResManager * rm = manager(nsengine.managerID(res_typeid));
	return rm->saveAll(path, scallback);	
}

bool NSPlugin::saveAs(NSResource * res, const nsstring & fname)
{
	NSResManager * rm = manager(nsengine.managerID(res->type()));
	return rm->saveAs(res, fname);
}

nsstring NSPlugin::details()
{
	if (mBound)
		_updateResMap();
	
	nsstring ret;
	auto iter = resmap.begin();
	while (iter != resmap.end())
	{
		ret += iter->second.second + "\n";
		++iter;
	}
	return ret;
}

void NSPlugin::setCreator(const nsstring & pCreator)
{
	mCreator = pCreator;
}

void NSPlugin::setNotes(const nsstring & pNotes)
{
	mNotes = pNotes;
}

NSResManager * NSPlugin::removeManager(uint32 manager_typeid)
{
	NSResManager * resman = manager(manager_typeid);
	auto iter = mManagers.erase(manager_typeid);
	return resman;		
}

NSResManager * NSPlugin::removeManager(const nsstring & manager_guid)
{
	return removeManager(hash_id(manager_guid));
}

void NSPlugin::setEditDate(const nsstring & pEditDate)
{
	mEditDate = pEditDate;
}

bool NSPlugin::save(NSResource * res, const nsstring & path)
{
	NSResManager * rm = manager(nsengine.managerID(res->type()));
	return rm->save(res, path);
}

void NSPlugin::setCreationDate(const nsstring & pCreationDate)
{
	mCreationDate = pCreationDate;
}

const nsstring & NSPlugin::resourceDirectory()
{
	return mResDir;
}

bool NSPlugin::resourceChanged(NSResource * res)
{
	NSResManager * rm = manager(nsengine.managerID(res->type()));
	return rm->changed(res);
}

uint32 NSPlugin::resourceCount()
{
	if (mBound)
		_updateResMap();
	return static_cast<uint32>(resmap.size());
}

bool NSPlugin::destroy(NSResource * res)
{
	NSResManager * rm = manager(nsengine.managerID(res->type()));
	return rm->destroy(res);
}

void NSPlugin::setResourceDirectory(const nsstring & dir)
{
	mResDir = dir;
	auto iter = mManagers.begin();
	while (iter != mManagers.end())
	{
		if (mAddname)
			iter->second->setResourceDirectory(mResDir + mName + "/");
		else
			iter->second->setResourceDirectory(mResDir);
		++iter;
	}
}

void NSPlugin::pup(NSFilePUPer * p)
{
	_updateParents();
	_updateResMap();
	if (p->type() == NSFilePUPer::Binary)
	{
		NSBinFilePUPer * bf = static_cast<NSBinFilePUPer *>(p);
		::pup(*bf, *this);
	}
	else
	{
		NSTextFilePUPer * tf = static_cast<NSTextFilePUPer *>(p);
		::pup(*tf, *this);
	}
}

NSResource * NSPlugin::remove(NSResource * res)
{
	NSResManager * rm = manager(nsengine.managerID(res->type()));
	return rm->remove(res);
}

void NSPlugin::_updateParents()
{
	// Get all get parents - only do immediate parents (not recursive)
	uivec2array usedResources;
	mParents.clear();

	auto iter = mManagers.begin();
	while (iter != mManagers.end())
	{
		auto iter2 = iter->second->begin();
		while (iter2 != iter->second->end())
		{
			uivec2array ret = iter2->second->resources();
			usedResources.insert(usedResources.end(), ret.begin(), ret.end());
			++iter2;
		}
		++iter;
	}

	auto resIter = usedResources.begin();
	while (resIter != usedResources.end())
	{
		if (resIter->x != mID) // if the owning plugin of this get is not us, then add it to parents
		{
			NSPlugin * plug = nsengine.plugin(resIter->x);
			if (plug != NULL && plug->id() != nsengine.engplug()->id())
				mParents.insert(plug->name());
		}
		++resIter;
	}
}

void NSPlugin::_updateResMap()
{
	resmap.clear();
	auto miter = mManagers.begin();
	while (miter != mManagers.end())
	{
		auto resiter = miter->second->begin();
		while (resiter != miter->second->end())
		{
			std::pair<nsstring, nsstring> rpair;
			rpair.first = type_to_guid(*resiter->second);
			rpair.second = resiter->second->name() + resiter->second->extension();
			resmap.emplace(hash_to_guid(miter->first), rpair);
			++resiter;
		}
		++miter;
	}
}

bool NSPlugin::parentsLoaded()
{
	auto piter = mParents.begin();
	while (piter != mParents.end())
	{
		NSPlugin * parent = nsengine.plugin(*piter);
		if (parent == NULL || !parent->bound())
			return false;
		++piter;
	}
	return true;
}

void NSPlugin::_clear()
{
	auto iter = mManagers.begin();
	while (iter != mManagers.end())
	{
		iter->second->destroyAll();
		++iter;
	}
}

