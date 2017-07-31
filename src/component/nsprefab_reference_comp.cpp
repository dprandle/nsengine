#include <component/nsprefab_reference_comp.h>
#include <nsentity.h>
#include <asset/nsprefab.h>
#include <nsengine.h>
#include <nsworld_data.h>
#include <component/nsrect_tform_comp.h>
#include <asset/nsplugin_manager.h>
#include <asset/nsplugin.h>
#include <nstform_ent_chunk.h>

nsprefab_reference_comp::nsprefab_reference_comp():
	nscomponent(type_to_hash(nsprefab_reference_comp)),
	m_restricted_types(),
	ref_id(0),
	prefab_id(),
	ent_id(0)
{}

nsprefab_reference_comp::nsprefab_reference_comp(const nsprefab_reference_comp & copy):
	nscomponent(copy.m_hashed_type),
	m_restricted_types(copy.m_restricted_types),
	ref_id(copy.ref_id),
	prefab_id(copy.prefab_id),
	ent_id(copy.ent_id)
{
	link_to_source();
}

nsprefab_reference_comp & nsprefab_reference_comp::operator=(nsprefab_reference_comp rhs)
{
	nscomponent::operator=(rhs);
	std::swap(m_restricted_types, rhs.m_restricted_types);
	std::swap(ref_id, rhs.ref_id);
	std::swap(prefab_id, rhs.prefab_id);
	std::swap(ent_id, rhs.ent_id);
	link_to_source();
	post_update(true);
	return *this;
}

nsprefab_reference_comp::~nsprefab_reference_comp()
{}

void nsprefab_reference_comp::init()
{
	m_restricted_types.insert(type_to_hash(nstform_comp));
	m_restricted_types.insert(type_to_hash(nsrect_tform_comp));
}

void nsprefab_reference_comp::release()
{
	unlink_from_source();		
}

void nsprefab_reference_comp::on_source_comp_add(nsentity * source, nscomponent * comp)
{
	emit_sig m_owner->component_added(m_owner, comp);
}

void nsprefab_reference_comp::on_source_comp_remove(nsentity * source, nscomponent * comp)
{
	emit_sig m_owner->component_removed(m_owner, comp);
}

void nsprefab_reference_comp::on_source_comp_edit(nsentity * source, nscomponent * comp)
{
	emit_sig m_owner->component_edited(m_owner, comp);
}

void nsprefab_reference_comp::link_to_source()
{
	nsentity * src = get_source_ent();

	if (src == nullptr)
		return;
	
	sig_connect(src->component_added, nsprefab_reference_comp::on_source_comp_add);
	sig_connect(src->component_removed, nsprefab_reference_comp::on_source_comp_remove);
	sig_connect(src->component_edited, nsprefab_reference_comp::on_source_comp_edit);

}

void nsprefab_reference_comp::unlink_from_source()
{
	nsentity * src = get_source_ent();
	if (src == nullptr)
		return;
	sig_disconnect(src->component_added);
	sig_disconnect(src->component_removed);
	sig_disconnect(src->component_edited);
}

void nsprefab_reference_comp::finalize()
{}

void nsprefab_reference_comp::name_change(const uivec2 &, const uivec2)
{}

nsentity * nsprefab_reference_comp::get_source_ent()
{
	nsprefab * pf = get_asset<nsprefab>(prefab_id);

	if (pf == nullptr)
		return nullptr;
	
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
