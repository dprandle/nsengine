#include <component/nsui_canvas_reference_comp.h>
#include <nsentity.h>
#include <asset/nsui_canvas.h>
#include <component/nsrect_tform_comp.h>
#include <nsengine.h>
#include <asset/nsplugin_manager.h>
#include <asset/nsplugin.h>
#include <nsui_tform_ent_chunk.h>

nsui_canvas_reference_comp::nsui_canvas_reference_comp():
	nscomponent(type_to_hash(nsui_canvas_reference_comp)),
	m_restricted_types(),
	ref_id(0),
	prefab_id(),
	ent_id(0)
{}

nsui_canvas_reference_comp::nsui_canvas_reference_comp(const nsui_canvas_reference_comp & copy):
	nscomponent(copy.m_hashed_type),
	m_restricted_types(copy.m_restricted_types),
	ref_id(copy.ref_id),
	prefab_id(copy.prefab_id),
	ent_id(copy.ent_id)
{
	link_to_source();
}

nsui_canvas_reference_comp & nsui_canvas_reference_comp::operator=(nsui_canvas_reference_comp rhs)
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

nsui_canvas_reference_comp::~nsui_canvas_reference_comp()
{}

void nsui_canvas_reference_comp::init()
{
	m_restricted_types.insert(type_to_hash(nstform_comp));
	m_restricted_types.insert(type_to_hash(nsrect_tform_comp));
}

void nsui_canvas_reference_comp::release()
{
	unlink_from_source();		
}

void nsui_canvas_reference_comp::on_source_comp_add(nsentity * source, nscomponent * comp)
{
	emit_sig m_owner->component_added(m_owner, comp);
}

void nsui_canvas_reference_comp::on_source_comp_remove(nsentity * source, nscomponent * comp)
{
	emit_sig m_owner->component_removed(m_owner, comp);
}

void nsui_canvas_reference_comp::on_source_comp_edit(nsentity * source, nscomponent * comp)
{
	emit_sig m_owner->component_edited(m_owner, comp);
}

void nsui_canvas_reference_comp::link_to_source()
{
	nsentity * src = get_source_ent();

	if (src == nullptr)
		return;
	
	sig_connect(src->component_added, nsui_canvas_reference_comp::on_source_comp_add);
	sig_connect(src->component_removed, nsui_canvas_reference_comp::on_source_comp_remove);
	sig_connect(src->component_edited, nsui_canvas_reference_comp::on_source_comp_edit);

}

void nsui_canvas_reference_comp::unlink_from_source()
{
	nsentity * src = get_source_ent();
	if (src == nullptr)
		return;
	sig_disconnect(src->component_added);
	sig_disconnect(src->component_removed);
	sig_disconnect(src->component_edited);
}

void nsui_canvas_reference_comp::finalize()
{}

void nsui_canvas_reference_comp::name_change(const uivec2 &, const uivec2)
{}

nsentity * nsui_canvas_reference_comp::get_source_ent()
{
	nsprefab * pf = get_asset<nsprefab>(prefab_id);

	if (pf == nullptr)
		return nullptr;
	
	return pf->prefab_entities()->find_entity(ent_id);
}

nsprefab * nsui_canvas_reference_comp::get_source_prefab()
{
	return get_asset<nsprefab>(prefab_id);
}

void nsui_canvas_reference_comp::add_restricted_type(uint32 t)
{
    m_restricted_types.emplace(t);
}

void nsui_canvas_reference_comp::remove_restricted_type(uint32 t)
{
    m_restricted_types.erase(t);
}

bool nsui_canvas_reference_comp::type_allowed(uint32 t)
{
	return m_restricted_types.find(t) == m_restricted_types.end();
}

nscomponent * nsui_canvas_reference_comp::get_source_comp(uint32 tid)
{
	if (!type_allowed(tid))
		return nullptr;
	
	nsentity * ent = get_source_ent();
	if (ent != nullptr)
		return ent->get(tid);
	return nullptr;
}

void nsui_canvas_reference_comp::get_source_comp_set(std::set<uint32> & ret)
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

uivec3_vector nsui_canvas_reference_comp::resources()
{
	return uivec3_vector();
}

void nsui_canvas_reference_comp::pup(nsfile_pupper * p)
{
		
}
