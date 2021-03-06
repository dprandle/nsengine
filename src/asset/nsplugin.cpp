/*!
  \file nsplugin.cpp

  \brief Definition file for nsplugin class

  This file contains all of the neccessary definitions for the nsplugin class.

  \author Daniel Randle
  \date August 23 2014
  \copywrite Earth Banana Games 2013
*/

#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include <nsengine.h>
#include <nsfactory.h>

#include <system/nstform_system.h>
#include <system/nscamera_system.h>
#include <system/nsselection_system.h>
#include <system/nsinput_system.h>
#include <system/nsui_system.h>

#include <asset/nsplugin.h>
#include <nsentity.h>
#include <asset/nstexture.h>
#include <asset/nsshader.h>
#include <component/nsui_canvas_comp.h>
#include <asset/nsfont.h>

#include <asset/nsfont_manager.h>
#include <asset/nsasset_manager.h>
#include <asset/nsmat_manager.h>
#include <asset/nsanim_manager.h>
#include <asset/nsmesh_manager.h>
#include <asset/nsplugin_manager.h>
#include <asset/nstex_manager.h>

#include <component/nscam_comp.h>
#include <component/nsanim_comp.h>
#include <component/nsoccupy_comp.h>
#include <component/nsrender_comp.h>
#include <component/nsterrain_comp.h>
#include <component/nssel_comp.h>
#include <component/nstile_comp.h>
#include <component/nslight_comp.h>

#include <nsworld_data.h>

// SHOULD GO AWAY WHEN DOING MAT THING
#include <opengl/nsgl_driver.h>

nsplugin::nsplugin():
	nsasset(hash_id("nsplugin")),
	m_notes(),
	m_creator(),
	m_creation_date(),
	m_edit_date(),
	m_enabled(false),
	m_managers(),
	m_add_name(DEFAULT_ADD_NAME_TO_RES_DIR)
{
	set_ext(DEFAULT_PLUGIN_EXTENSION);
}

nsplugin::nsplugin(const nsplugin & copy_):
	nsasset(copy_),
	m_notes(copy_.m_notes),
	m_creator(copy_.m_creator),
	m_creation_date(copy_.m_creation_date),
	m_edit_date(copy_.m_edit_date),
	m_enabled(false),
	m_managers(),
	m_parents(copy_.m_parents),
	m_resmap(copy_.m_resmap),
	m_unloaded(),
	m_add_name(copy_.m_add_name)
{}

nsplugin::~nsplugin()
{
}

nsplugin & nsplugin::operator=(nsplugin rhs)
{
	nsasset::operator=(rhs);
	std::swap(m_notes,rhs.m_notes);
	std::swap(m_creator,rhs.m_creator);
	std::swap(m_creation_date,rhs.m_creation_date);
	std::swap(m_edit_date,rhs.m_edit_date);
	std::swap(m_parents,rhs.m_parents);
	std::swap(m_resmap,rhs.m_resmap);
	std::swap(m_add_name,rhs.m_add_name);
	return *this;
}

bool nsplugin::add(nsasset * res)
{
	if (res == nullptr)
		return false;

	if (!m_enabled)
	{
		dprint("nsplugin::add The plugin " + m_name + " is not enabled - it must be enabled to add stuff");
		return false;
	}

	nsasset_manager * rm = manager(nse.manager_id(res->type()));
	return rm->add(res);
}

bool nsplugin::add_manager(nsasset_manager * manag)
{
	if (manag == nullptr)
		return false;

	auto iter = m_managers.emplace(manag->type(), manag);
	if (iter.second)
	{
		manag->set_plugin_id(m_id);
		nsstring res_dir = nse.cwd() + nsstring(DEFAULT_RESOURCE_DIR);
		if (m_add_name)
			manag->set_res_dir(res_dir + m_name + "/");
		else
			manag->set_res_dir(res_dir);
		return true;
	}
	return false;
}

nsinput_map * nsplugin::create_global_input_map(const nsstring & imap_name, const nsstring & global_ctxt_name)
{
	nsinput_map * im = create<nsinput_map>(imap_name);
    im->create_context(global_ctxt_name);

	nse.system<nscamera_system>()->setup_input_map(im, global_ctxt_name);
    nse.system<nsselection_system>()->setup_input_map(im, global_ctxt_name);
    nse.system<nsui_system>()->setup_input_map(im, global_ctxt_name);
    nse.system<nsinput_system>()->set_input_map(im->full_id());
    nse.system<nsinput_system>()->push_context(global_ctxt_name);

	nsinput_map::trigger change_vp(VIEWPORT_CHANGE, nsinput_map::t_pressed);
    change_vp.add_key_mod(nsinput_map::key_any);
    im->add_mouse_trigger(global_ctxt_name, nsinput_map::left_button, change_vp);
	return im;
}

nsasset * nsplugin::create(uint32 res_typeid, const nsstring & resName, nsasset * to_copy)
{
	if (!m_enabled)
	{
		dprint("nsplugin::create The plugin " + m_name + " is not enabled - it must be enabled to create stuff");
		return nullptr;
	}

	nsasset_manager * rm = manager(nse.manager_id(res_typeid));
	return rm->create(res_typeid, resName, to_copy);
}


bool nsplugin::contains(nsasset * res)
{
	if (res == nullptr)
		return false;
	
	nsasset_manager * rm = manager(nse.manager_id(res->type()));
	return rm->contains(res);
}

nsasset_manager * nsplugin::create_manager(const nsstring & manager_guid)
{
	return create_manager(hash_id(manager_guid));
}

nsasset_manager * nsplugin::create_manager(uint32 manager_typeid)
{
	nsmanager_factory * factory = nse.factory<nsmanager_factory>(manager_typeid);
	nsasset_manager * man = factory->create();
	if (!add_manager(man))
	{
		delete man;
		return nullptr;
	}
	return man;
}

bool nsplugin::destroy_manager(const nsstring & manager_guid)
{
	if (m_enabled)
		return false;
	
	return destroy_manager(hash_id(manager_guid));
}

bool nsplugin::destroy_manager(uint32 manager_typeid)
{
	if (m_enabled)
		return false;
	
	nsasset_manager * resman = remove_manager(manager_typeid);
	delete resman;
	return true;
}

bool nsplugin::del(nsasset * res)
{
	if (!m_enabled)
	{
		dprint("nsplugin::del The plugin " + m_name + " is not enabled - it must be enabled to del stuff");
		return false;
	}

	nsasset_manager * rm = manager(nse.manager_id(res->type()));
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

nsasset * nsplugin::get(uint32 res_typeid, uint32 resid)
{
	if (!m_enabled)
	{
		dprint("nsplugin::get The plugin " + m_name + " is not enabled - it must be enabled to get stuff");
		return nullptr;
	}

	nsasset_manager * rm = manager(nse.manager_id(res_typeid));
	return rm->get(resid);
}

nsasset * nsplugin::get(uint32 res_typeid, nsasset * res)
{
	if (!m_enabled)
	{
		dprint("nsplugin::get The plugin " + m_name + " is not enabled - it must be enabled to get stuff");
		return nullptr;
	}

	nsasset_manager * rm = manager(nse.manager_id(res_typeid));
	return rm->get(res);
}

nsasset * nsplugin::get(uint32 res_typeid, const nsstring & resName)
{
	if (!m_enabled)
	{
		dprint("nsplugin::get The plugin " + m_name + " is not enabled - it must be enabled to get stuff");
		return nullptr;
	}

	nsasset_manager * rm = manager(nse.manager_id(res_typeid));
	return rm->get(resName);
}

void nsplugin::init()
{
	auto fiter = nse.begin_factory();
	nsasset_manager * rm = nullptr;
	while (fiter != nse.end_factory())
	{
		nsfactory * plug_man_fac = nse.factory<nsplugin_manager>();
		if (fiter->second->type() == nsfactory::f_res_manager && fiter->second != plug_man_fac)
		{
			rm = create_manager(nse.guid(fiter->first));
		}

		++fiter;
	}
}

void nsplugin::release()
{
	enable(false);
    auto iter = m_managers.begin();
    while (iter != m_managers.end())
    {
        delete iter->second;
        ++iter;
    }
    m_managers.clear();	
}

nsasset * nsplugin::load(uint32 res_typeid, const nsstring & fname, bool finalize_)
{
	if (!m_enabled)
	{
		dprint("nsplugin::load_model The plugin " + m_name + " is not enabled - it must be enabled to load stuff");
		return nullptr;
	}

	nsasset_manager * rm = manager(nse.manager_id(res_typeid));
	return rm->load(res_typeid, fname, finalize_);
}

bool nsplugin::load_model_assets(
	nsstring fname,
	const nsstring & model_name,
	bool flipuv)
{
	if (!m_enabled)
	{
		dprint("nsplugin::load_model_resources The plugin " + m_name + " is not enabled - it must be enabled to load stuff");
		return false;
	}

	nsstring sceneName = model_name;
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

nsmesh * nsplugin::load_model_mesh(
	nsstring fname,
	const nsstring & model_name,
	bool flipuv)
{
	if (!m_enabled)
	{
		dprint("nsplugin::load_model_mesh The plugin " + m_name + " is not enabled - it must be enabled to load stuff");
		return nullptr;
	}

	nsstring sceneName = model_name;
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

	return manager<nsmesh_manager>()->assimp_load_mesh(scene, sceneName);
}

bool nsplugin::load_model_mats(
	nsstring fname,
	const nsstring & model_name,
	bool flipuv)
{
	if (!m_enabled)
	{
		dprint("nsplugin::load_model_mats The plugin " + m_name + " is not enabled - it must be enabled to load stuff");
		return false;
	}

	nsstring sceneName = model_name;
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

	if (scene->HasMaterials())
	{
		for (uint32 i = 0; i < scene->mNumMaterials; ++i)
		{
			nsstringstream ss;
			ss << sceneName << "_" << i;
			const aiMaterial* aiMat = scene->mMaterials[i];
			nsmaterial * mat = manager<nsmat_manager>()->assimp_load_material(ss.str(), aiMat, dir);
		}
		return true;
	}
	else
		return false;
}

nsanim_set * nsplugin::load_model_anim(
	nsstring fname,
	const nsstring & model_name,
	bool flipuv)
{
	if (!m_enabled)
	{
		dprint("nsplugin::load_model_anim The plugin " + m_name + " is not enabled - it must be enabled to load stuff");
		return nullptr;
	}

	nsstring sceneName = model_name;
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

	if (scene->HasAnimations())
		return manager<nsanim_manager>()->assimp_load_anim_set(scene, sceneName);

	return nullptr;		
}

nsasset_manager * nsplugin::manager(const nsstring & manager_guid)
{
	return manager(hash_id(manager_guid));
}

nsasset_manager * nsplugin::manager(uint32 manager_typeid)
{	
	auto iter = m_managers.find(manager_typeid);
	if (iter == m_managers.end())
		return nullptr;
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

bool nsplugin::is_enabled()
{
	return m_enabled;
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

const nsstring_set & nsplugin::parents()
{
	if (m_enabled)
		_update_parents();
	return m_parents;
}

void nsplugin::enable(bool enable_)
{
	if (enable_)
	{
		if (m_enabled)
		{
			dprint("nsplugin::bind Cannot bind plugin " + m_name + " - it is already enabled");
			return;
		}
	
		if (!parents_enabled())
		{
			dprint("nsplugin::bind Cannot load plugin " + m_name + " without parents enabled");
			return;
		}

        m_enabled = true;

		auto liter = m_resmap.begin();
		while (liter != m_resmap.end())
		{
			nsasset_manager * rm = manager(liter->first);
			nsasset * r = rm->load(liter->second.m_res_guid,liter->second.m_res_subdir_and_name, false);

			if (r == nullptr)
				m_unloaded.emplace(liter->first, liter->second);
			else
				r->set_icon_path(liter->second.m_icon_path);
		
			++liter;
		}

		// Finalize loading - the finalize function is for resources that may have dependencies on
		// themselves - or other resources. This allows the engine to save these dependencies as uivec3
		// ids on loading - and once all is loaded - the finalize replaces the ids with pointers
		liter = m_resmap.begin();
		while (liter != m_resmap.end())
		{
			nsasset_manager * rm = manager(liter->first);
			rm->finalize_all();
			++liter;
		}
	}
	else
	{
		if (!m_enabled)
			return;
		
		_update_res_map();
		_update_parents();
		destroy_all();
		m_unloaded.clear();
		m_enabled = false;
	}
}

void nsplugin::save_all_in_plugin(const nsstring & path, nssave_resouces_callback * scallback)
{
	if (!m_enabled)
	{
		dprint("nsplugin::save_all_in_plugin Cannot save all resources while pugin " + m_name + " is not enabled");
		return;
	}

	auto iter = m_managers.begin();
	while (iter != m_managers.end())
	{
		iter->second->save_all(path, scallback);
		++iter;
	}
}

void nsplugin::save_all(uint32 res_typeid, const nsstring & path, nssave_resouces_callback * scallback)
{
	if (!m_enabled)
	{
		dprint("nsplugin::save_all Cannot save all resources type " + hash_to_guid(res_typeid) + " while pugin " + m_name + " is not enabled");
		return;
	}

	nsasset_manager * rm = manager(nse.manager_id(res_typeid));
	return rm->save_all(path, scallback);	
}

bool nsplugin::save_as(nsasset * res, const nsstring & fname)
{
	if (res == nullptr)
		return false;
	
	if (!m_enabled)
	{
		dprint("nsplugin::save_as Trying to save resource " + res->name() + " in plugin " + m_name + " while it is not enabled");
		return false;
	}

	nsasset_manager * rm = manager(nse.manager_id(res->type()));
	return rm->save_as(res, fname);
}

nsstring nsplugin::details()
{
	if (m_enabled)
		_update_res_map();
	
	nsstring ret;
	auto iter = m_resmap.begin();
	while (iter != m_resmap.end())
	{
		ret += iter->second.m_res_subdir_and_name + "\n";
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

nsasset_manager * nsplugin::remove_manager(uint32 manager_typeid)
{
	nsasset_manager * resman = manager(manager_typeid);
	auto iter = m_managers.erase(manager_typeid);
	return resman;		
}

nsasset_manager * nsplugin::remove_manager(const nsstring & manager_guid)
{
	return remove_manager(hash_id(manager_guid));
}

void nsplugin::set_edit_date(const nsstring & pEditDate)
{
	m_edit_date = pEditDate;
}

bool nsplugin::save(nsasset * res, const nsstring & path)
{
	if (res == nullptr)
		return false;
	
	if (!m_enabled)
	{
		dprint("nsplugin::save Trying to save resource " + res->name() + " in plugin " + m_name + " while it is not enabled..");
		return false;
	}
	
	nsasset_manager * rm = manager(nse.manager_id(res->type()));
	return rm->save(res, path);
}

void nsplugin::set_creation_date(const nsstring & pCreationDate)
{
	m_creation_date = pCreationDate;
}

bool nsplugin::resource_changed(nsasset * res)
{
	if (!m_enabled)
	{
		dprint("nsplugin::resource_changed Trying to check plugin " + m_name + " while it is not enabled..");
		return false;
	}

	nsasset_manager * rm = manager(nse.manager_id(res->type()));
	return rm->changed(res);
}

uint32 nsplugin::resource_count()
{
	if (m_enabled)
		_update_res_map();
	return static_cast<uint32>(m_resmap.size());
}

bool nsplugin::destroy(nsasset * res)
{
	if (res == nullptr)
		return false;

	if (!m_enabled)
	{
		dprint("nsplugin::destroy Trying to destroy " + res->name() + " while owning plugin " + m_name + " is not enabled..");
		return false;
	}

	nsasset_manager * rm = manager(nse.manager_id(res->type()));
	return rm->destroy(res);
}

void nsplugin::set_managers_res_dir(const nsstring & dir)
{
	auto iter = m_managers.begin();
	while (iter != m_managers.end())
	{
		iter->second->set_res_dir(dir);
		++iter;
	}
}

void nsplugin::pup(nsfile_pupper * p)
{
	_update_parents();
	_update_res_map();
	if (p->type() == nsfile_pupper::pup_binary)
	{
		nsbinary_file_pupper * bf = static_cast<nsbinary_file_pupper *>(p);
		::pup(*bf, *this);
	}
	else
	{
		nstext_file_pupper * tf = static_cast<nstext_file_pupper *>(p);
		::pup(*tf, *this);
	}
}

nsasset * nsplugin::remove(nsasset * res)
{
	if (res == nullptr)
		return nullptr;
	
	if (!m_enabled)
	{
		dprint("nsplugin::remove Trying to remove " + res->name() + " while owning plugin " + m_name + " is not enabled..");
		return nullptr;
	}
	
	nsasset_manager * rm = manager(nse.manager_id(res->type()));
	return rm->remove(res);
}

void nsplugin::_update_parents()
{
	// Get all get parents - only do immediate parents (not recursive)
	uivec3_vector usedResources;
	m_parents.clear();

	auto iter = m_managers.begin();
	while (iter != m_managers.end())
	{
		auto iter2 = iter->second->begin();
		while (iter2 != iter->second->end())
		{
			uivec3_vector ret = iter2->second->resources();
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
			nsplugin * plug = get_plugin(resIter->x);
			if (plug != nullptr && plug->id() != nse.core()->id())
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
			res_info rinfo;
			rinfo.m_res_guid = hash_to_guid(resiter->second->type());
			rinfo.m_res_subdir_and_name = resiter->second->subdir() + resiter->second->name() + resiter->second->extension();
			rinfo.m_icon_path = resiter->second->icon_path();
			m_resmap.emplace(hash_to_guid(miter->first), rinfo);
			++resiter;
		}
		++miter;
	}
}

bool nsplugin::parents_enabled()
{
	auto piter = m_parents.begin();
	while (piter != m_parents.end())
	{
		nsplugin * parent = nsep.get<nsplugin>(*piter);
		if (parent == nullptr || !parent->is_enabled())
			return false;
		++piter;
	}
	return true;
}

void nsplugin::destroy_all()
{
	auto iter = m_managers.begin();
	while (iter != m_managers.end())
	{
		iter->second->destroy_all();
		++iter;
	}
}

nsplugin::res_info::res_info(
	const nsstring & res_guid,
	const nsstring & subdir_and_name,
	const nsstring & icon_path):
	m_res_guid(res_guid),
	m_res_subdir_and_name(subdir_and_name),
	m_icon_path(icon_path)	
{}

nsentity * create_tile(
	nsplugin * assets,
	const nsstring & name,
	fvec4 m_col,
	float s_pwr,
	float s_int32,
	fvec3 s_col,
	bool collides,
	tile_t type)
{
	return create_tile(assets, name, "","",m_col,s_pwr,s_int32,s_col,collides,type);
}

nsentity * create_tile(
	nsplugin * assets,
	const nsstring & name,
	const nsstring & difftex,
	const nsstring & normtex,
	fvec4 m_col,
	float s_pwr,
	float s_int32,
	fvec3 s_col,
	bool collides,
	tile_t type)
{
	nsmaterial * mat = assets->get<nsmaterial>(name);
	if (mat == nullptr)
		mat = assets->create<nsmaterial>(name);

	if (mat == nullptr)
		return nullptr;

	mat->set_cull_mode(GL_BACK);
	mat->enable_culling(true);
	mat->set_specular_color(s_col);
	mat->set_color(m_col);
	mat->set_specular_power(s_pwr);
	mat->set_specular_intensity(s_int32);

	nstexture * tdiff = assets->get<nstex2d>(nsasset_manager::name_from_filename(difftex));
	if (tdiff == nullptr)
		tdiff = assets->load<nstex2d>(difftex, true);
	
	nstexture * tnorm = assets->get<nstex2d>(nsasset_manager::name_from_filename(normtex));
	if (tnorm == nullptr)
		tnorm = assets->load<nstex2d>(normtex, true);
	
	if (tdiff != nullptr)
		mat->add_tex_map(nsmaterial::diffuse, tdiff->full_id());
	if (tnorm != nullptr)
		mat->add_tex_map(nsmaterial::normal, tnorm->full_id());
	if (tdiff == nullptr && tnorm == nullptr)
	{
		mat->set_color_mode(true);
		if (mat->color().a < 1.0f)
		{
			mat->enable_culling(false);
			mat->set_alpha_blend(true);
		}
	}
	return create_tile(assets, name, mat, collides, type);
}

nsentity * create_tile(
	nsplugin * assets,
	const nsstring & name,
	nsmaterial * mat,
	bool collides,
	tile_t type)
{
	nsentity * ent = ent = new nsentity(name);
	
	if (ent == nullptr)
		return nullptr;

	nsrender_comp * rc = ent->create<nsrender_comp>();
	nssel_comp * sc = ent->create<nssel_comp>();
	nstile_comp * tc = ent->create<nstile_comp>();
	nsmesh * msh = nullptr;
	if (type == tile_full)
		msh = nse.core()->get<nsmesh>(MESH_FULL_TILE);
	else
		msh = nse.core()->get<nsmesh>(MESH_HALF_TILE);

	if (collides)
	{
		nsoccupy_comp * oc = ent->create<nsoccupy_comp>();
		oc->build(msh->aabb());
		oc->set_mesh_id(msh->full_id());
	}

	rc->set_mesh_id(msh->full_id());
	if (mat != nullptr)
		rc->set_material(0, mat->full_id());
	return ent;
}

nsentity * create_tile(
	nsplugin * assets,
	const nsstring & name,
	const nsstring & matname,
	bool collides,
	tile_t type)
{
	return create_tile(assets, name, assets->get<nsmaterial>(matname), collides, type);
}

nsentity * create_tile(
	nsplugin * assets,
	const nsstring & name,
	uint32 matid,
	bool collides,
	tile_t type)
{
	return create_tile(assets, name, assets->get<nsmaterial>(matid), collides, type);
}

nsentity * create_skydome(
	nsplugin * assets,
	const nsstring & name,
	nsstring cubemap_relative_fname,
	const nsstring & image_ext,
	const nsstring & tex_subdir)
{
	nsentity * skybox = new nsentity(name);
	
	nstexture * sky_box = assets->manager<nstex_manager>()->load_cubemap(cubemap_relative_fname, image_ext);
	sky_box->set_subdir(tex_subdir);
	
	nsmaterial * sb_mat = assets->create<nsmaterial>(name);
	sb_mat->add_tex_map(nsmaterial::diffuse, sky_box->full_id());
	sb_mat->set_shader_id(nse.core()->get<nsshader>(SKYBOX_SHADER)->full_id());
	sb_mat->set_cull_mode(GL_FRONT);
	
	nsrender_comp * rc = skybox->create<nsrender_comp>();
	rc->set_cast_shadow(false);
	rc->set_mesh_id(nse.core()->get<nsmesh>(MESH_SKYDOME)->full_id());
	rc->set_material(0, sb_mat->full_id());

	return skybox;
}

nsentity * create_terrain(
	nsplugin * assets,
	const nsstring & name, 
	float hmin, 
	float hmax, 
	const nsstring & hmfile, 
	const nsstring & dmfile, 
	const nsstring & nmfile)
{
	nsentity * terr = new nsentity(name);

	if (terr == nullptr)
		return nullptr;

	nsterrain_comp * tc = terr->create<nsterrain_comp>();
	tc->set_height_bounds(hmin, hmax);
	nsrender_comp * rc = terr->create<nsrender_comp>();
	rc->set_cast_shadow(true);
	rc->set_mesh_id(nse.core()->get<nsmesh>(MESH_TERRAIN)->full_id());
	
	nsmaterial * termat = assets->create<nsmaterial>(name);
	termat->enable_culling(false);
	rc->set_material(0, termat->full_id(), true);

	nstex2d * hm = nullptr, * dm = nullptr, * nm = nullptr;
	hm = assets->load<nstex2d>(hmfile, true);

	if (hm == nullptr)
	{
		assets->destroy(termat);
		delete terr;
		return nullptr;
	}

	if (!dmfile.empty())
	{
		
		dm = assets->load<nstex2d>(dmfile, true);
		if (dm == nullptr)
		{
			assets->destroy(hm);
			assets->destroy(termat);
			delete terr;
			return nullptr;
		}
	}

	if (!nmfile.empty())
	{
		nm = assets->load<nstex2d>(nmfile, true);
		if (nm == nullptr)
		{
			assets->destroy(dm);
			assets->destroy(hm);
			assets->destroy(termat);
			delete terr;
			return nullptr;
		}
	}

	termat->add_tex_map(nsmaterial::height, hm->full_id());
	if (dm != nullptr)
		termat->add_tex_map(nsmaterial::diffuse, dm->full_id());
	if (nm != nullptr)
		termat->add_tex_map(nsmaterial::normal, nm->full_id());
	
	return terr;
}

nsentity * create_sprite(
	nsplugin * assets,
	const nsstring & sprite_name,
	const nsstring & tex_filename,
	bool match_tex_dims,
	bool alpha_enabled)
{
	nstex2d * bg = assets->load<nstex2d>(tex_filename, true);
	bg->flip_horizontal();
	nsmesh_plane * plane = assets->create<nsmesh_plane>(sprite_name);

	if (match_tex_dims)
		plane->bake_scaling(fvec3(bg->size().w/2,bg->size().h/2,1.0f));
	
	nsmaterial * mat = assets->create<nsmaterial>(sprite_name);
	mat->add_tex_map(nsmaterial::diffuse, bg->full_id());

	mat->set_alpha_blend(alpha_enabled);
	
	nsentity * bgent = new nsentity(sprite_name);
	nsrender_comp * rc = bgent->create<nsrender_comp>();
	bgent->create<nssel_comp>();
	rc->set_mesh_id(plane->full_id());
	rc->set_material(0, mat->full_id());
	rc->transparent_picking = true;
	return bgent;
}

nsentity * create_camera(
	const nsstring & name,
	float fov,
	const uivec2 & screenDim,
	const fvec2 & clipnf)
{
	nsentity * cament = new nsentity(name);
	if (cament == nullptr)
		return nullptr;

	nscam_comp * camc = cament->create<nscam_comp>();
	camc->set_fov(fov);
	camc->resize_screen(screenDim.w, screenDim.h);
	camc->set_persp_nf_clip(clipnf);
	camc->set_proj_mode(nscam_comp::proj_persp);
	return cament;
}


nsentity * create_camera(
	const nsstring & name,
	const fvec2 & lrclip,
	const fvec2 & tbclip,
	const fvec2 & nfclip)
{
	nsentity * cament = new nsentity(name);
	if (cament == nullptr)
		return nullptr;

	nscam_comp * camc = cament->create<nscam_comp>();
	camc->set_ortho_lr_clip(lrclip);
	camc->set_ortho_tb_clip(tbclip);
	camc->set_ortho_nf_clip(nfclip);
	camc->set_proj_mode(nscam_comp::proj_ortho);
	return cament;
}

nsentity * create_dir_light(
	const nsstring & name,
	float diffuse,
	float ambient,
	const fvec3 & color,
	bool castshadows,
	float shadowdarkness,
	int32 shadowsamples)
{
	nsmesh * bounds = nse.core()->get<nsmesh>(MESH_DIRLIGHT_BOUNDS);
	if (bounds == nullptr)
		return nullptr;

	nsentity * lt = new nsentity(name);
	if (lt == nullptr)
		return nullptr;
	
	nslight_comp * lc = lt->create<nslight_comp>();
	lc->set_mesh_id(bounds->plugin_id(), bounds->id());
	lc->set_type(nslight_comp::l_dir);
	lc->set_angle(30.0f);
	lc->set_intensity(diffuse, ambient);
	lc->set_color(color);
	lc->set_cast_shadows(castshadows);
	lc->set_shadow_clipping(fvec2(DEFAULT_Z_NEAR, DEFAULT_Z_FAR));
	lc->set_shadow_darkness(shadowdarkness);
	lc->set_shadow_samples(shadowsamples);
	return lt;
}

nsentity * create_point_light(
	const nsstring & name,
	float diffuse,
	float ambient,
	float distance,
	const fvec3 & color,
	bool castshadows,
	float shadowdarkness,
	int32 shadowsamples)
{
	nsmesh * bounds = nse.core()->get<nsmesh>(MESH_POINTLIGHT_BOUNDS);
	if (bounds == nullptr)
		return nullptr;

	nsentity * lt = new nsentity(name);
	if (lt == nullptr)
		return nullptr;

	nslight_comp * lc = lt->create<nslight_comp>();
	lc->set_mesh_id(bounds->plugin_id(), bounds->id());
	lc->set_type(nslight_comp::l_point);
	lc->set_intensity(diffuse, ambient);
	lc->set_color(color);
	lc->set_distance(distance);
	lc->set_cast_shadows(castshadows);
	lc->set_shadow_clipping(fvec2(DEFAULT_Z_NEAR, DEFAULT_Z_FAR));
	lc->set_shadow_darkness(shadowdarkness);
	lc->set_shadow_samples(shadowsamples);
	lt->create<nssel_comp>();
	nsrender_comp * rc = lt->create<nsrender_comp>();
	rc->set_mesh_id(bounds->full_id());
	rc->set_cast_shadow(false);
	return lt;
}

nsentity * create_spot_light(
	const nsstring & name,
	float diffuse,
	float ambient,
	float distance,
	float radius,
	const fvec3 & color,
	bool castshadows,
	float shadowdarkness,
	int32 shadowsamples)
{
	nsmesh * bounds = nse.core()->get<nsmesh>(MESH_SPOTLIGHT_BOUNDS);
	if (bounds == nullptr)
		return nullptr;

	nsentity * lt = new nsentity(name);
	if (lt == nullptr)
		return nullptr;

	nslight_comp * lc = lt->create<nslight_comp>();
	lc->set_mesh_id(bounds->plugin_id(), bounds->id());
	lc->set_type(nslight_comp::l_spot);
	lc->set_intensity(diffuse, ambient);
	lc->set_color(color);
	lc->set_cast_shadows(castshadows);
	lc->set_shadow_clipping(fvec2(DEFAULT_Z_NEAR, DEFAULT_Z_FAR));
	lc->set_shadow_darkness(shadowdarkness);
	lc->set_shadow_samples(shadowsamples);
	lc->set_distance(distance);
	lc->set_radius(radius);

	nsrender_comp * rc = lt->create<nsrender_comp>();
	rc->set_mesh_id(bounds->full_id());
	rc->set_cast_shadow(false);
	lt->create<nssel_comp>();

	return lt;
}


nsentity * create_entity_from_model(
	nsplugin * assets,
	const nsstring & entname,
	nsstring fname,
	const nsstring & model_name,
	bool occupy_comp,
	bool flipuv)
{
	nsentity * ent = new nsentity(entname);
	nsrender_comp * renderComp = ent->create<nsrender_comp>();

	nsstring sceneName = model_name;
	if (sceneName.empty())
	{
		sceneName = fname.substr(fname.find_last_of("/\\") + 1);
		sceneName = sceneName.erase(sceneName.find_last_of("."), nsstring::npos);
	}

	nsstring dir = fname.substr(0, fname.find_last_of("/\\") + 1);

	uint32 flag = 0;

	if (flipuv)
		flag = aiProcess_ConvertToLeftHanded;

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


	nsmesh * renderMesh = assets->get<nsmesh>(sceneName);
	if (renderMesh == nullptr)
		renderMesh = assets->manager<nsmesh_manager>()->assimp_load_mesh(scene, sceneName);
	
	renderComp->set_mesh_id(renderMesh->full_id());

	if (occupy_comp)
	{
		nsoccupy_comp * occ = ent->create<nsoccupy_comp>();
		occ->build(renderMesh->aabb());
	}
	
	if (scene->HasMaterials())
	{
		std::map<uint32, uivec2> indexMap;
		for (uint32 i = 0; i < scene->mNumMaterials; ++i)
		{
			nsstringstream ss;
			ss << sceneName << "_" << i;
			const aiMaterial* aiMat = scene->mMaterials[i];

			nsmaterial * mat = assets->get<nsmaterial>(ss.str());
			if (mat == nullptr)
				mat = assets->manager<nsmat_manager>()->assimp_load_material(ss.str(), aiMat, dir);
			
			if (mat != nullptr)
				indexMap[i] = mat->full_id();
		}

		for (uint32 i = 0; i < scene->mNumMeshes; ++i)
		{
			const aiMesh* mesh = scene->mMeshes[i];
			if (mesh->mPrimitiveTypes != aiPrimitiveType_TRIANGLE)
				continue;

			uint32 mI = mesh->mMaterialIndex;
			auto fIter = indexMap.find(mI);
			if (fIter != indexMap.end())
				renderComp->set_material(i, fIter->second);
		}
	}

	if (scene->HasAnimations())
	{
		nsanim_set * animSet = assets->get<nsanim_set>(sceneName);
		if (animSet == nullptr)
			animSet = assets->manager<nsanim_manager>()->assimp_load_anim_set(scene, sceneName);
		
		nsanim_comp * animComp = ent->create<nsanim_comp>();
		animComp->set_anim_set_id(animSet->full_id());
	}
	ent->create<nssel_comp>();
	return ent;
}
