#include <component/nsprefab_reference_comp.h>
#include <nsentity.h>
#include <asset/nsprefab.h>
#include <nsengine.h>
#include <nsworld_data.h>

nsprefab_reference_comp::nsprefab_reference_comp():
	nscomponent(type_to_hash(nsprefab_reference_comp)),
	ref_id(0)
{
}

nsprefab_reference_comp::nsprefab_reference_comp(const nsprefab_reference_comp & copy):
	nscomponent(copy.m_hashed_type)
{
}

nsprefab_reference_comp::~nsprefab_reference_comp()
{
		
}

void nsprefab_reference_comp::init()
{
}

void nsprefab_reference_comp::release()
{
		
}

void nsprefab_reference_comp::finalize()
{
		
}

void nsprefab_reference_comp::name_change(const uivec2 &, const uivec2)
{
		
}

nsentity * nsprefab_reference_comp::get_source_ent()
{
	nsprefab * pf = get_asset<nsprefab>(prefab_id);
	return pf->prefab_entities()->find_entity(ent_id);
}

nsprefab * nsprefab_reference_comp::get_source_prefab()
{
	return get_asset<nsprefab>(prefab_id);
}

void nsprefab_reference_comp::add_restricted_type(uint32 t)
{
    m_restricted_types.emplace(t);
}

void nsprefab_reference_comp::remove_restricted_type(uint32 t)
{
    m_restricted_types.erase(t);
}

bool nsprefab_reference_comp::type_allowed(uint32 t)
{
	return m_restricted_types.find(t) == m_restricted_types.end();
}

nscomponent * nsprefab_reference_comp::get_source_comp(uint32 tid)
{
	if (!type_allowed(tid))
		return nullptr;
	
	nsentity * ent = get_source_ent();
	if (ent != nullptr)
		return ent->get(tid);
	return nullptr;
}

void nsprefab_reference_comp::get_source_comp_set(std::set<uint32> & ret)
{
	std::set<uint32> source_comps;
	nsentity * ent = get_source_ent();
	if (ent == nullptr)
		return;

	ent->get_comp_set(source_comps);

	auto iter = source_comps.begin();
	while (iter != source_comps.end())
	{
		if (!type_allowed(*iter))
			iter = source_comps.erase(iter);
		else
			++iter;
	}
	ret.insert(source_comps.begin(),source_comps.end());
}

uivec3_vector nsprefab_reference_comp::resources()
{
	return uivec3_vector();
}

void nsprefab_reference_comp::pup(nsfile_pupper * p)
{
		
}
