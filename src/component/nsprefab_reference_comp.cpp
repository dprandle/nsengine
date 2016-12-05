#include <component/nsprefab_reference_comp.h>
#include <asset/nsentity.h>
#include <nsengine.h>
#include <component/nsprefab_comp.h>

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
	return get_asset<nsentity>(ent_id);
}

nsentity * nsprefab_reference_comp::get_source_prefab()
{
	return get_asset<nsentity>(prefab_id);
}

nscomponent * nsprefab_reference_comp::get_source_comp(int tid)
{
    if (tid == type_to_hash(nsprefab_comp) || tid == type_to_hash(nstform_comp))
		return nullptr;
	
	nsentity * ent = get_source_ent();
	return ent->get(tid);
}

uivec3_vector nsprefab_reference_comp::resources()
{
	return uivec3_vector();
}

void nsprefab_reference_comp::pup(nsfile_pupper * p)
{
		
}
