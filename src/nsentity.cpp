/*! 
	\file nsentity.cpp
	
	\brief Definition file for nsentity class

	This file contains all of the neccessary definitions for the nsentity class.

	\author Daniel Randle
	\date November 23 2013
	\copywrite Earth Banana Games 2013
*/

#include <nsentity.h>
#include <component/nssel_comp.h>
#include <nsfactory.h>
#include <nsengine.h>
#include <asset/nsplugin_manager.h>
#include <component/nsprefab_reference_comp.h>
#include <nsworld_data.h>

nsentity::nsentity() :
	m_name(),
	m_id(0),
	m_components()
{
	m_components.clear();
}

nsentity::nsentity(const nsentity & copy):
	m_name(copy.m_name),
	m_id(copy.m_id),
	m_components()
{
	// deep copy the components
	auto iter = copy.m_components.begin();
	while (iter != copy.m_components.end())
	{
		create(iter->second);
		++iter;
	}
}

nsentity::~nsentity()
{
	destroy_all();
}

void nsentity::rename(const nsstring & new_name)
{
	uivec2 names(m_id, hash_id(new_name));
	m_name = new_name;
	m_id = names.y;
	emit_sig ent_rename(names);
}

nsentity & nsentity::operator=(nsentity rhs)
{
	std::swap(m_name, rhs.m_name);
	std::swap(m_id, rhs.m_id);
	std::swap(m_components, rhs.m_components);
	return *this;
}

const nsstring & nsentity::name() const
{
	return m_name;
}

uint32 nsentity::id() const
{
	return m_id;
}

bool nsentity::add(nscomponent * cmp)
{
	if (cmp == nullptr)
		return false;

	uint32 hashed_type = cmp->type();
	if (hashed_type == 0)
	{
		dprint(nsstring("Cannot add component with type ") + nse.guid(hashed_type) + nsstring(" to Entity ") + m_name +
			   nsstring(": No hash_id found"));
		return false;
	}
	
	auto ret = m_components.emplace(hashed_type, cmp);
	if (ret.second)
	{
		cmp->set_owner(this);
		emit_sig component_added(cmp);
	}
	return ret.second;
}

void nsentity::finalize()
{
	auto iter = m_components.begin();
	while (iter != m_components.end())
	{
		iter->second->finalize();
		++iter;
	}
}

void nsentity::get_comp_set(std::set<uint32> & ret)
{
	auto iter = m_components.begin();
	while (iter != m_components.end())
	{
		ret.emplace(iter->first);
		++iter;
	}
	
	// If have prefab comp try to add these babies too
	nsprefab_reference_comp * pf = get<nsprefab_reference_comp>();
	if (pf != nullptr)
		pf->get_source_comp_set(ret);
}

void nsentity::destroy_all()
{
	dprint("nsentity::destroy_all destroying all components in " + m_name);
	while (m_components.begin() != m_components.end())
		destroy(m_components.begin()->first);
}

nscomponent * nsentity::create(uint32 type_id)
{
	nscomponent * comp_t = nse.factory<nscomp_factory>(type_id)->create();
	if (!add(comp_t))
	{
		dprint(nsstring("nsentity::create - Failed adding comp_t type ") + nse.guid(type_id) +
			   nsstring(" to Entity ") + m_name);
		delete comp_t;
		return nullptr;
	}
	comp_t->init();
	return comp_t;
}

nscomponent * nsentity::create(nscomponent * to_copy)
{
	if (to_copy == nullptr)
		return nullptr;
	
	nscomponent * comp_t = nse.factory<nscomp_factory>(to_copy->type())->create(to_copy);
	if (!add(comp_t))
	{
		dprint(nsstring("nsentity::create - Failed copying comp_t type ") + nse.guid(to_copy->type()) +
			   nsstring(" to Entity ") + m_name);
		delete comp_t;
		return nullptr;
	}
	comp_t->init();
	return comp_t;	
}

nscomponent * nsentity::create(const nsstring & guid)
{
	return create(hash_id(guid));
}

bool nsentity::destroy(const nsstring & guid)
{
	return destroy(hash_id(guid));
}

bool nsentity::destroy(uint32 type_id)
{
	nscomponent * cmp = remove(type_id);
	if (cmp != nullptr) // Log delete
	{
		dprint("nsentity::destroy - destroying \"" + nse.guid(type_id) + "\" from entity " + m_name + "\"");
		delete cmp;
		return true;
	}
	dprint("nsentity::destroy - component type \"" + nse.guid(type_id) + "\" was not part of entity \"" + m_name + "\"");
	return false;
}

void nsentity::pup(nsfile_pupper * p)
{
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

uint32 nsentity::count()
{
	return static_cast<uint32>(m_components.size());
}

nscomponent * nsentity::get(const nsstring & guid)
{
	return get(hash_id(guid));
}

nscomponent * nsentity::get(uint32 type_id)
{
	comp_set::iterator iter = m_components.find(type_id);
	if (iter != m_components.end())
		return iter->second;

	comp_set::iterator prefab_iter = m_components.find(type_to_hash(nsprefab_reference_comp));
	if (prefab_iter != m_components.end())
	{
		nsprefab_reference_comp * ref = static_cast<nsprefab_reference_comp*>(prefab_iter->second);
		return ref->get_source_comp(type_id);
	}
	return nullptr;
}

void nsentity::name_change(const uivec2 & oldid, const uivec2 newid)
{
	// Go through each component and call name change
	auto iter = m_components.begin();
	while (iter != m_components.end())
	{
		iter->second->name_change(oldid, newid);
		++iter;
	}
}

/*!
  Get the other resources that this Entity uses. This is given by all the components attached to the entity.
*/
uivec3_vector nsentity::resources()
{
	uivec3_vector ret;

	// Go through each component and insert all used resources from each comp_t
	auto iter = m_components.begin();
	while (iter != m_components.end())
	{
		uivec3_vector tmp = iter->second->resources();
		ret.insert(ret.end(), tmp.begin(), tmp.end() );
		++iter;
	}
	return ret;
}

bool nsentity::has(const nsstring & guid)
{
	return has(hash_id(guid));
}

bool nsentity::has(uint32 type_id)
{
	nscomponent * comp = get(type_id);
	return comp != nullptr;
}

nscomponent * nsentity::remove(uint32 type_id)
{
	nscomponent * comp_t = nullptr;
	auto iter = m_components.find(type_id);
	if (iter != m_components.end())
	{
		comp_t = iter->second;
		comp_t->release();
		m_components.erase(iter);
		dprint("nsentity::remove - removing \"" + nse.guid(type_id) + "\" from entity " + m_name + "\"");
		emit_sig component_removed(comp_t);
		comp_t->set_owner(nullptr);
	}
	else
	{
		dprint("nsentity::remove - component type \"" + nse.guid(type_id) + "\" was not part of Entity \"" + m_name + "\"");
	}

	return comp_t;
	
}

nscomponent * nsentity::remove(const nsstring & guid)
{
	return remove(hash_id(guid));
}

void nsentity::post_update_all(bool pUpdate)
{
	auto iter = m_components.begin();
	while (iter != m_components.end())
	{
		iter->second->post_update(pUpdate);
		++iter;
	}
}

void nsentity::post_update(const nsstring & compType, bool update)
{
	nscomponent * comp_t = get(compType);
	if (comp_t != nullptr)
		comp_t->post_update(update);
}

bool nsentity::update_posted(const nsstring & compType)
{
	nscomponent * comp_t = get(compType);
	if (comp_t != nullptr)
		return comp_t->update_posted();
	return false;
}

uint32 nsentity::chunk_id()
{
	nstform_comp * tfc = get<nstform_comp>();
	if (tfc != nullptr)
		return tfc->chunk_id();
	return 0;
}

uivec2 nsentity::full_id()
{
	nstform_comp * tfc = get<nstform_comp>();
	if (tfc != nullptr)
		return uivec2(tfc->chunk_id(), m_id);
	return uivec2(0, m_id);
}

nsentity * get_entity(const uivec2 & chunk_ent_id)
{
	nstform_ent_chunk * chnk = nse.world()->chunk(chunk_ent_id.x);
	if (chnk == nullptr)
		return nullptr;
	return chnk->find_entity(chunk_ent_id.y);
}
