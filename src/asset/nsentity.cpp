/*! 
	\file nsentity.cpp
	
	\brief Definition file for nsentity class

	This file contains all of the neccessary definitions for the nsentity class.

	\author Daniel Randle
	\date November 23 2013
	\copywrite Earth Banana Games 2013
*/

#include <asset/nsentity.h>
#include <component/nssel_comp.h>
#include <nsfactory.h>
#include <nsengine.h>
#include <asset/nsplugin_manager.h>

nsentity::nsentity() :
	nsasset(type_to_hash(nsentity)),
	m_components()
{
	m_components.clear();
	set_ext(DEFAULT_ENTITY_EXTENSION);
}

nsentity::nsentity(const nsentity & copy):
	nsasset(copy),
	m_components()
{
	auto iter = copy.m_components.begin();
	while (iter != copy.m_components.end())
	{
		// Copy all components except for tform comp
		if (iter->first != type_to_hash(nstform_comp))
			create(iter->second);
		++iter;
	}
}

nsentity::~nsentity()
{}

nsentity & nsentity::operator=(nsentity rhs)
{
	nsasset::operator=(rhs);
	std::swap(m_components, rhs.m_components);
	return *this;
}

bool nsentity::add(nscomponent * pComp)
{
	if (pComp == NULL)
		return false;

	uint32 hashed_type = pComp->type();
	if (hashed_type == 0)
	{
		dprint(nsstring("Cannot add component with type ") + nse.guid(hashed_type) + nsstring(" to Entity ") + m_name +
			   nsstring(": No hash_id found"));
		return false;
	}
	
	auto ret = m_components.emplace(hashed_type, pComp);
	if (ret.second)
	{
		pComp->set_owner(this);
		component_added(pComp);
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

void nsentity::destroy_all()
{
	auto cur_comp = m_components.begin();
	while (cur_comp != m_components.end())
	{
		cur_comp->second->release();
		delete cur_comp->second;
		++cur_comp;
	}
	m_components.clear();
}

nsentity::comp_set::iterator nsentity::begin()
{
	return m_components.begin();
}

nsentity::comp_set::iterator nsentity::end()
{
	return m_components.end();
}

nscomponent * nsentity::create(uint32 type_id)
{
	nscomponent * comp_t = nse.factory<nscomp_factory>(type_id)->create();
	if (!add(comp_t))
	{
		dprint(nsstring("nsentity::create - Failed adding comp_t type ") + nse.guid(type_id) +
			   nsstring(" to Entity ") + m_name);
		delete comp_t;
		return NULL;
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
		return NULL;
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
	if (cmp != NULL) // Log delete
	{
		delete cmp;
		dprint("nsentity::del - Deleting \"" + nse.guid(type_id) + "\" from Entity " + m_name + "\"");
		return true;
	}
	dprint("nsentity::del - Component type \"" + nse.guid(type_id) + "\" was not part of Entity \"" + m_name + "\"");
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
	return NULL;
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
	return (m_components.find(type_id) != m_components.end());
}

void nsentity::init()
{
	// do nothing
}

void nsentity::release()
{
	destroy_all();
}

nscomponent * nsentity::remove(uint32 type_id)
{
	nscomponent * comp_t = NULL;
	auto iter = m_components.find(type_id);
	if (iter != m_components.end())
	{
		comp_t = iter->second;
		comp_t->release();
		m_components.erase(iter);
		component_removed(comp_t);
		comp_t->set_owner(NULL);
		dprint("nsentity::remove - Removing \"" + nse.guid(type_id) + "\" from Entity " + m_name + "\"");
	}
	else
	{
		dprint("nsentity::remove - Component type \"" + nse.guid(type_id) + "\" was not part of Entity \"" + m_name + "\"");
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
	if (comp_t != NULL)
		comp_t->post_update(update);
}

bool nsentity::update_posted(const nsstring & compType)
{
	nscomponent * comp_t = get(compType);
	if (comp_t != NULL)
		return comp_t->update_posted();
	return false;
}
