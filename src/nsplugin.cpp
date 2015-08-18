/*!
\file nsplugin.cpp

\brief Definition file for nsplugin class

This file contains all of the neccessary definitions for the nsplugin class.

\author Daniel Randle
\date August 23 2014
\copywrite Earth Banana Games 2013
*/
#include <nsplugin.h>
#include <nsengine.h>
#include <nsres_manager.h>
#include <nsscene_manager.h>
#include <nsfactory.h>
#include <nsentity.h>
#include <nsanim_comp.h>
#include <nsmat_manager.h>
#include <nsanim_manager.h>
#include <nsoccupy_comp.h>
#include <nsrender_comp.h>
#include <nsinput_comp.h>
#include <nsterrain_comp.h>
#include <nsmesh_manager.h>
#include <nssel_comp.h>
#include <nstile_comp.h>
#include <nsplugin_manager.h>

nsplugin::nsplugin() : 
	m_notes(),
	m_creator(),
	m_creation_date(),
	m_edit_date(),
	m_bound(false),
	m_managers(),
	m_add_name(true),
	nsresource()
{
	set_ext(DEFAULT_PLUGIN_EXTENSION);
}

nsplugin::~nsplugin()
{
	auto iter = m_managers.begin();
	while (iter != m_managers.end())
	{
		delete iter->second;
		++iter;
	}
	m_managers.clear();
}

bool nsplugin::add(nsresource * res)
{
	if (res == NULL)
		return false;

	nsres_manager * rm = manager(nsengine.managerID(res->type()));
	return rm->add(res);
}

void nsplugin::add_name_to_res_path(bool add_)
{
	m_add_name = add_;
	auto iter = m_managers.begin();
	while (iter != m_managers.end())
	{
		if (m_add_name)
			iter->second->set_res_dir(m_res_dir + m_name + "/");
		else
			iter->second->set_res_dir(m_res_dir);
		++iter;
	}
}

bool nsplugin::adding_names_to_res_path()
{
	return m_add_name;
}

bool nsplugin::add_manager(nsres_manager * manag)
{
	if (manag == NULL)
		return false;

	auto iter = m_managers.emplace(manag->type(), manag);
	if (iter.second)
	{
		manag->set_plugin_id(m_id);
		if (m_add_name)
			manag->set_res_dir(m_res_dir + m_name + "/");
		else
			manag->set_res_dir(m_res_dir);
		return true;
	}
	return false;
}

nsresource * nsplugin::create(uint32 res_typeid, const nsstring & resName)
{
	nsres_manager * rm = manager(nsengine.managerID(res_typeid));
	return rm->create(res_typeid, resName);
}

nsentity * nsplugin::create_camera(const nsstring & name, float fov, const uivec2 & screenDim, const fvec2 & clipnf)
{
	nsentity * cament = create<nsentity>(name);
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

bool nsplugin::contains(nsresource * res)
{
	nsres_manager * rm = manager(nsengine.managerID(res->type()));
	return rm->contains(res);
}

nsentity * nsplugin::create_camera(const nsstring & name, const fvec2 & lrclip, const fvec2 & tbclip, const fvec2 & nfclip)
{
	nsentity * cament = create<nsentity>(name);
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

nsentity * nsplugin::create_dir_light(const nsstring & name,
	float diffuse,
	float ambient,
	const fvec3 & color,
	bool castshadows,
	float shadowdarkness,
	int32 shadowsamples)
{
	nsmesh * bounds = nsengine.engplug()->get<nsmesh>(MESH_DIRLIGHT_BOUNDS);
	if (bounds == NULL)
		return NULL;
	nsentity * lt = create<nsentity>(name);
	if (lt == NULL)
		return NULL;
	NSLightComp * lc = lt->create<NSLightComp>();
	if (lc == NULL)
	{
		destroy(lt);
		return NULL;
	}
	lc->setMeshID(bounds->plugin_id(), bounds->id());
	lc->setType(NSLightComp::Directional);
	lc->setIntensity(diffuse, ambient);
	lc->setColor(color);
	lc->setCastShadows(castshadows);
	lc->setShadowClipping(fvec2(DEFAULT_Z_NEAR, DEFAULT_Z_FAR));
	lc->setShadowDarkness(shadowdarkness);
	lc->setShadowSamples(shadowsamples);
	return lt;
}

nsentity * nsplugin::create_point_light(const nsstring & name,
	float diffuse,
	float ambient,
	float distance,
	const fvec3 & color,
	bool castshadows,
	float shadowdarkness,
	int32 shadowsamples)
{
	nsmesh * bounds = nsengine.engplug()->get<nsmesh>(MESH_POINTLIGHT_BOUNDS);
	if (bounds == NULL)
		return NULL;
	nsentity * lt = create<nsentity>(name);
	if (lt == NULL)
		return NULL;
	NSLightComp * lc = lt->create<NSLightComp>();
	if (lc == NULL)
	{
		destroy(lt);
		return NULL;
	}
	lc->setMeshID(bounds->plugin_id(), bounds->id());
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
	rc->setMeshID(bounds->full_id());
	rc->setCastShadow(false);
	return lt;
}

nsentity * nsplugin::create_spot_light(const nsstring & name,
float diffuse,
float ambient,
float distance,
float radius,
const fvec3 & color,
bool castshadows,
float shadowdarkness,
int32 shadowsamples)
{
	nsmesh * bounds = nsengine.engplug()->get<nsmesh>(MESH_SPOTLIGHT_BOUNDS);
	if (bounds == NULL)
		return NULL;
	nsentity * lt = create<nsentity>(name);
	if (lt == NULL)
		return NULL;
	NSLightComp * lc = lt->create<NSLightComp>();
	if (lc == NULL)
	{
		destroy(lt);
		return NULL;
	}
	lc->setMeshID(bounds->plugin_id(), bounds->id());
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
	rc->setMeshID(bounds->full_id());
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

nsres_manager * nsplugin::create_manager(const nsstring & manager_guid)
{
	return create_manager(hash_id(manager_guid));
}

nsres_manager * nsplugin::create_manager(uint32 manager_typeid)
{
	nsres_managerFactory * factory = nsengine.factory<nsres_managerFactory>(manager_typeid);
	nsres_manager * man = factory->create();
	if (!add_manager(man))
	{
		delete man;
		return NULL;
	}
	return man;
}

nsentity * nsplugin::create_tile(const nsstring & name,
	const nsstring & difftex,
	const nsstring & normtex,
	fvec3 m_col,
	float s_pwr,
	float s_int32,
	fvec3 s_col,
	bool collides,
	tile_t type)
{
	nsmaterial * mat = create<nsmaterial>(name);
	if (mat == NULL)
		return NULL;

	mat->set_cull_mode(GL_BACK);
	mat->enable_culling(true);
	mat->set_specular_color(s_col);
	mat->set_color(m_col);
	mat->set_specular_power(s_pwr);
	mat->set_specular_intensity(s_int32);
	nstexture * tdiff = load<nstex2d>(difftex);
	nstexture * tnorm = load<nstex2d>(normtex);
	if (tdiff != NULL)
		mat->set_map_tex_id(nsmaterial::diffuse, tdiff->full_id());
	if (tnorm != NULL)
		mat->set_map_tex_id(nsmaterial::normal, tnorm->full_id());
	if (tdiff == NULL && tnorm == NULL)
		mat->set_color_mode(true);
	return create_tile(name, mat, collides, type);
}

nsentity * nsplugin::create_tile(const nsstring & name,
	nsmaterial * mat, bool collides, tile_t type)
{
	nsentity * ent = create<nsentity>(name);
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

	nsmesh * msh = NULL;
	if (type == tile_full)
		msh = nsengine.engplug()->get<nsmesh>(MESH_FULL_TILE);
	else
		msh = nsengine.engplug()->get<nsmesh>(MESH_HALF_TILE);

	if (collides)
	{
		NSOccupyComp * oc = ent->create<NSOccupyComp>();
		oc->build(msh->aabb());
		oc->setMeshID(msh->full_id());
	}

	rc->setMeshID(msh->full_id());
	if (mat != NULL)
		rc->setMaterial(0, mat->full_id());

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

nsentity * nsplugin::create_tile(const nsstring & name,
	const nsstring & matname, bool collides, tile_t type)
{
	return create_tile(name, get<nsmaterial>(matname), collides, type);
}

nsentity * nsplugin::create_tile(const nsstring & name,
	uint32 matid, bool collides, tile_t type)
{
	return create_tile(name, get<nsmaterial>(matid), collides, type);
}

nsentity * nsplugin::create_terrain(const nsstring & name, 
	float hmin, 
	float hmax, 
	const nsstring & hmfile, 
	const nsstring & dmfile, 
	const nsstring & nmfile,
	bool importdir)
{
	nsentity * terr = create<nsentity>(name);
	nullchkn(terr);
	NSTerrainComp * tc = terr->create<NSTerrainComp>();
	tc->setHeightBounds(hmin, hmax);
	NSRenderComp * rc = terr->create<NSRenderComp>();
	rc->setCastShadow(true);
	rc->setMeshID(nsengine.engplug()->get<nsmesh>(MESH_TERRAIN)->full_id());
	
	nsmaterial * termat = create<nsmaterial>(name);
	if (termat == NULL)
	{
		destroy(terr);
		return NULL;
	}
	termat->enable_culling(false);
	rc->setMaterial(0, termat->full_id(), true);

	nstex2d * hm = NULL, * dm = NULL, * nm = NULL;
	if (importdir)
		hm = load<nstex2d>(m_import_dir + hmfile);
	else
		hm = load<nstex2d>(hmfile);

	if (hm == NULL)
	{
		destroy(termat);
		destroy(terr);
		return NULL;
	}

	hm->set_parameter_i(nstexture::wrap_s, GL_REPEAT);
	hm->set_parameter_i(nstexture::wrap_t, GL_REPEAT);

	if (!dmfile.empty())
	{
		
		if (importdir)
			dm = load<nstex2d>(m_import_dir + dmfile);
		else
			dm = load<nstex2d>(dmfile);

		if (dm == NULL)
		{
			destroy(hm);
			destroy(termat);
			destroy(terr);
			return NULL;
		}
		dm->enable_mipmaps();
	}

	if (!nmfile.empty())
	{
		if (importdir)
			nm = load<nstex2d>(m_import_dir + nmfile);
		else
			nm = load<nstex2d>(nmfile);

		if (nm == NULL)
		{
			destroy(dm);
			destroy(hm);
			destroy(termat);
			destroy(terr);
			return NULL;
		}
	}

	termat->set_map_tex_id(nsmaterial::height, hm->full_id(), true);
	if (dm != NULL)
		termat->set_map_tex_id(nsmaterial::diffuse, dm->full_id(), true);
	if (nm != NULL)
		termat->set_map_tex_id(nsmaterial::normal, nm->full_id(), true);

	
	return terr;
}

nsscene * nsplugin::current_scene()
{
	nsscene_manager * sm = manager<nsscene_manager>();
	return sm->current();
}

bool nsplugin::destroy_manager(const nsstring & manager_guid)
{
	return destroy_manager(hash_id(manager_guid));
}

bool nsplugin::destroy_manager(uint32 manager_typeid)
{
	nsres_manager * resman = remove_manager(manager_typeid);
	delete resman;
	return true;
}

bool nsplugin::del(nsresource * res)
{
	nsres_manager * rm = manager(nsengine.managerID(res->type()));
	return rm->del(res);
}

bool nsplugin::has_parent(const nsstring & pname)
{
	auto fiter = m_parents.find(pname);
	return (fiter != m_parents.end());
}

void nsplugin::name_change(const uivec2 & oldid, const uivec2 newid)
{
	auto iter = m_managers.begin();
	while (iter != m_managers.end())
	{
		// If the plugin part of id is zero it means the plugin itself had a name change.. therefor
		// propagate a plugin name change through all resources using the get id that was passed in
		// Otherwise propagate name change like normal
		if (oldid.x == 0)
			iter->second->name_change(uivec2(oldid.yx()), uivec2(newid.yx()));
		else
			iter->second->name_change(oldid, newid);
		++iter;
	}
}

nsresource * nsplugin::get(uint32 res_typeid, uint32 resid)
{
	nsres_manager * rm = manager(nsengine.managerID(res_typeid));
	return rm->get(resid);
}

nsresource * nsplugin::get(uint32 res_typeid, const nsstring & resName)
{
	nsres_manager * rm = manager(nsengine.managerID(res_typeid));
	return rm->get(resName);
}

void nsplugin::init()
{
	auto fiter = nsengine.beginFac();
	nsres_manager * rm = NULL;
	while (fiter != nsengine.endFac())
	{
		NSFactory * plug_man_fac = nsengine.factory<nsplugin_manager>();
		if (fiter->second->type() == NSFactory::ResManager && fiter->second != plug_man_fac)
		{
			rm = create_manager(nsengine.guid(fiter->first));

			if (m_add_name)
				rm->set_res_dir(m_res_dir + m_name + "/");
			else
				rm->set_res_dir(m_res_dir);
		}

		++fiter;
	}
}

nsresource * nsplugin::load(uint32 res_typeid, const nsstring & fname)
{
	nsres_manager * rm = manager(nsengine.managerID(res_typeid));
	return rm->load(res_typeid, fname);
}

nsentity * nsplugin::load_model(const nsstring & entname, nsstring fname, bool prefixWithImportDir, const nsstring & meshname, bool flipuv)
{
	if (prefixWithImportDir)
		fname = m_import_dir + fname;

	nsentity * ent = create<nsentity>(entname);
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


	nsmesh * renderMesh = manager<nsmesh_manager>()->assimp_load_mesh(scene, sceneName);
	renderComp->setMeshID(m_id, renderMesh->id());

	if (scene->HasMaterials())
	{
		std::map<uint32, uint32> indexMap;
		for (uint32 i = 0; i < scene->mNumMaterials; ++i)
		{
			nsstringstream ss;
			ss << sceneName << "_" << i;
			const aiMaterial* aiMat = scene->mMaterials[i];
			nsmaterial * mat = manager<nsmat_manager>()->assimp_load_material(ss.str(), aiMat, dir);
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
				renderComp->setMaterial(i, m_id, fIter->second);
		}
	}

	if (scene->HasAnimations())
	{
		nsanim_set * animSet = manager<nsanim_manager>()->assimp_load_anim_set(scene, sceneName);
		NSAnimComp * animComp = ent->create<NSAnimComp>();
		animComp->setAnimationSetID(m_id, animSet->id());
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

bool nsplugin::load_model_resources(nsstring fname,bool prefixWithImportDir, const nsstring & meshname, bool flipuv)
{
	if (prefixWithImportDir)
		fname = m_import_dir + fname;

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

	nsmesh * mesh = manager<nsmesh_manager>()->assimp_load_mesh(scene, sceneName);

	if (scene->HasMaterials())
	{
		for (uint32 i = 0; i < scene->mNumMaterials; ++i)
		{
			nsstringstream ss;
			ss << sceneName << "_" << i;
			const aiMaterial* aiMat = scene->mMaterials[i];
			nsmaterial * mat = manager<nsmat_manager>()->assimp_load_material(ss.str(), aiMat, dir);
		}
	}

	if (scene->HasAnimations())
		manager<nsanim_manager>()->assimp_load_anim_set(scene, sceneName);

	return true;
}

nsres_manager * nsplugin::manager(const nsstring & manager_guid)
{
	return manager(hash_id(manager_guid));
}

nsres_manager * nsplugin::manager(uint32 manager_typeid)
{
	auto iter = m_managers.find(manager_typeid);
	if (iter == m_managers.end())
		return NULL;
	return iter->second;	
}

bool nsplugin::has_manager(uint32 manager_typeid)
{
	return (m_managers.find(manager_typeid) != m_managers.end());
}

bool nsplugin::has_manager(const nsstring & manager_guid)
{
	return has_manager(hash_id(manager_guid));
}

bool nsplugin::bound()
{
	return m_bound;
}

const nsstring & nsplugin::creator()
{
	return m_creator;
}

const nsstring & nsplugin::notes()
{
	return m_notes;
}

const nsstring & nsplugin::edit_date()
{
	return m_edit_date;
}

const nsstring & nsplugin::creation_date()
{
	return m_creation_date;
}

const nsstringset & nsplugin::parents()
{
	if (m_bound)
		_update_parents();
	return m_parents;
}

bool nsplugin::bind()
{
	if (!parents_loaded())
		return false;

	auto liter = m_resmap.begin();
	while (liter != m_resmap.end())
	{
		nsres_manager * rm = manager(liter->first);
		nsresource * r = rm->load(liter->second.first,liter->second.second);
		if (r == NULL)
			m_unloaded.emplace(liter->first, liter->second);
		++liter;
	}

	return (m_bound = true);
}

bool nsplugin::unbind()
{
	_update_res_map();
	_update_parents();
	_clear();
	m_unloaded.clear();
	m_bound = false;
	return !m_bound;
}

void nsplugin::save_all(const nsstring & path, NSSaveResCallback * scallback)
{
	auto iter = m_managers.begin();
	while (iter != m_managers.end())
	{
		iter->second->save_all(path, scallback);
		++iter;
	}
}

void nsplugin::save_all(uint32 res_typeid, const nsstring & path, NSSaveResCallback * scallback)
{
	nsres_manager * rm = manager(nsengine.managerID(res_typeid));
	return rm->save_all(path, scallback);	
}

bool nsplugin::save_as(nsresource * res, const nsstring & fname)
{
	nsres_manager * rm = manager(nsengine.managerID(res->type()));
	return rm->save_as(res, fname);
}

nsstring nsplugin::details()
{
	if (m_bound)
		_update_res_map();
	
	nsstring ret;
	auto iter = m_resmap.begin();
	while (iter != m_resmap.end())
	{
		ret += iter->second.second + "\n";
		++iter;
	}
	return ret;
}

void nsplugin::set_creator(const nsstring & pCreator)
{
	m_creator = pCreator;
}

void nsplugin::set_notes(const nsstring & pNotes)
{
	m_notes = pNotes;
}

nsres_manager * nsplugin::remove_manager(uint32 manager_typeid)
{
	nsres_manager * resman = manager(manager_typeid);
	auto iter = m_managers.erase(manager_typeid);
	return resman;		
}

nsres_manager * nsplugin::remove_manager(const nsstring & manager_guid)
{
	return remove_manager(hash_id(manager_guid));
}

void nsplugin::set_edit_date(const nsstring & pEditDate)
{
	m_edit_date = pEditDate;
}

bool nsplugin::save(nsresource * res, const nsstring & path)
{
	nsres_manager * rm = manager(nsengine.managerID(res->type()));
	return rm->save(res, path);
}

void nsplugin::set_creation_date(const nsstring & pCreationDate)
{
	m_creation_date = pCreationDate;
}

const nsstring & nsplugin::res_dir()
{
	return m_res_dir;
}

bool nsplugin::resource_changed(nsresource * res)
{
	nsres_manager * rm = manager(nsengine.managerID(res->type()));
	return rm->changed(res);
}

uint32 nsplugin::resource_count()
{
	if (m_bound)
		_update_res_map();
	return static_cast<uint32>(m_resmap.size());
}

bool nsplugin::destroy(nsresource * res)
{
	nsres_manager * rm = manager(nsengine.managerID(res->type()));
	return rm->destroy(res);
}

void nsplugin::set_res_dir(const nsstring & dir)
{
	m_res_dir = dir;
	auto iter = m_managers.begin();
	while (iter != m_managers.end())
	{
		if (m_add_name)
			iter->second->set_res_dir(m_res_dir + m_name + "/");
		else
			iter->second->set_res_dir(m_res_dir);
		++iter;
	}
}

void nsplugin::pup(NSFilePUPer * p)
{
	_update_parents();
	_update_res_map();
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

nsresource * nsplugin::remove(nsresource * res)
{
	nsres_manager * rm = manager(nsengine.managerID(res->type()));
	return rm->remove(res);
}

void nsplugin::_update_parents()
{
	// Get all get parents - only do immediate parents (not recursive)
	uivec2array usedResources;
	m_parents.clear();

	auto iter = m_managers.begin();
	while (iter != m_managers.end())
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
		if (resIter->x != m_id) // if the owning plugin of this get is not us, then add it to parents
		{
			nsplugin * plug = nsengine.plugin(resIter->x);
			if (plug != NULL && plug->id() != nsengine.engplug()->id())
				m_parents.insert(plug->name());
		}
		++resIter;
	}
}

void nsplugin::_update_res_map()
{
	m_resmap.clear();
	auto miter = m_managers.begin();
	while (miter != m_managers.end())
	{
		auto resiter = miter->second->begin();
		while (resiter != miter->second->end())
		{
			std::pair<nsstring, nsstring> rpair;
			rpair.first = type_to_guid(*resiter->second);
			rpair.second = resiter->second->name() + resiter->second->extension();
			m_resmap.emplace(hash_to_guid(miter->first), rpair);
			++resiter;
		}
		++miter;
	}
}

bool nsplugin::parents_loaded()
{
	auto piter = m_parents.begin();
	while (piter != m_parents.end())
	{
		nsplugin * parent = nsengine.plugin(*piter);
		if (parent == NULL || !parent->bound())
			return false;
		++piter;
	}
	return true;
}

void nsplugin::_clear()
{
	auto iter = m_managers.begin();
	while (iter != m_managers.end())
	{
		iter->second->destroy_all();
		++iter;
	}
}

