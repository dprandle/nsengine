#include <component/nsprefab_reference_comp.h>
#include <component/nsprefab_comp.h>
#include <nsengine.h>
#include <asset/nsentity.h>
#include <asset/nsplugin.h>
#include <asset/nsplugin_manager.h>

nsprefab_comp::nsprefab_comp():
	nscomponent(type_to_hash(nsprefab_comp))
{
		
}

nsprefab_comp::nsprefab_comp(const nsprefab_comp & copy):
	nscomponent(copy.m_hashed_type)
{
		
}

nsprefab_comp::~nsprefab_comp()
{
		
}

void nsprefab_comp::create_reference(nsplugin * create_in, const uivec2 & source_ent_id, packed_ent_tform & ref)
{
	create_reference(create_in, find_entity_index(source_ent_id), ref);
}

void nsprefab_comp::create_references(nsplugin * create_in, std::vector<packed_ent_tform> & refs)
{
    refs.reserve(1000);
	for (uint32 i = 0; i < m_ent_ids.size(); ++i)
	{
		refs.resize(refs.size()+1);
		packed_ent_tform & pent = refs.back();
		create_reference(create_in, i, pent);
	}
}

void nsprefab_comp::init()
{
	nstform_comp * tf = m_owner->create<nstform_comp>();
	m_ent_ids.push_back(prefab_ent_info(m_owner->full_id()));
	assert(tf!=nullptr);
}

void nsprefab_comp::release()
{}

void nsprefab_comp::finalize()
{
		
}

void nsprefab_comp::name_change(const uivec2 &, const uivec2)
{
		
}

uivec3_vector nsprefab_comp::resources()
{
	return uivec3_vector();
}

void nsprefab_comp::pup(nsfile_pupper * p)
{
		
}

nstform_comp * nsprefab_comp::add_entity(
	const uivec2 & ent_id,
	tform_info * tf_info,
	bool data_in_world_space)
{
	nstform_comp * our_tfc = m_owner->get<nstform_comp>();
	
	nsentity * ent = get_asset<nsentity>(ent_id);
	if (ent == nullptr || find_entity(ent_id) != nullptr)
		return nullptr;

	// create tform comp if one doesn't exist - if it does then return 0
	nstform_comp * to_add_tfc = ent->get<nstform_comp>();
	if (to_add_tfc != nullptr)
		return nullptr;

	to_add_tfc = ent->create<nstform_comp>();
		
	if (tf_info != nullptr)
	{
		make_valid(tf_info);
		if (tf_info->m_parent == uivec2(0))
			tf_info->m_parent = m_owner->full_id();
		to_add_tfc->set_tf_info(*tf_info, data_in_world_space);
	}
	else
	{
		to_add_tfc->set_parent(our_tfc, true);
	}
		
	m_ent_ids.push_back(prefab_ent_info(ent_id));
	emit_sig entity_added(ent);
	sig_connect(ent->component_removed, nsprefab_comp::on_tform_remove);
	return to_add_tfc;
}

void nsprefab_comp::make_valid(tform_info * tf)
{
	if (find_entity(tf->m_parent) == nullptr)
	{
		dprint("nsprefab_comp::make_valid Parent " + tf->m_parent.to_string() + " was invalid");
		tf->m_parent = uivec2(0);
	}
	auto iter = tf->m_children.begin();
	while (iter != tf->m_children.end())
	{
		if (find_entity(*iter) == nullptr)
		{
			dprint("nsprefab_comp::make_valid Child " + iter->to_string() + " was invalid");
			iter = tf->m_children.erase(iter);
		}
		else
			++iter;
	}
}

nsentity * nsprefab_comp::remove_entity(const uivec2 & ent_id)
{
	nsentity * ret = nullptr;
	auto iter = m_ent_ids.begin();
	while (iter != m_ent_ids.end())
	{
		if (iter->id == ent_id)
		{
			ret = get_asset<nsentity>(ent_id);
			sig_disconnect(ret->component_removed);
			ret->destroy<nstform_comp>();
			break;
		}
		++iter;
	}
	return ret;
}

void nsprefab_comp::on_tform_remove(nscomponent * comp)
{
	if (comp->type() == type_to_hash(nstform_comp))
	{
		nsentity * ent = comp->owner();
		
		auto iter = m_ent_ids.begin();
		while (iter != m_ent_ids.end())
		{
			if (iter->id == ent->full_id())
				iter = m_ent_ids.erase(iter);
			else
				++iter;
		}
		sig_disconnect(ent->component_removed);
	}
}

nsentity * nsprefab_comp::find_entity(const uivec2 & ent_id)
{
	int i = find_entity_index(ent_id);
	
	if (i == -1)
		return nullptr;
	else
		return get_asset<nsentity>(m_ent_ids[i].id);
}

int32 nsprefab_comp::find_entity_index(const uivec2 & ent_id)
{
	for (int32 i = 0; i < m_ent_ids.size(); ++i)
	{
		if (ent_id == m_ent_ids[i].id)
			return i;
	}
	return -1;
}

void nsprefab_comp::create_reference(nsplugin * create_in, int32 ent_ind, packed_ent_tform & ref)
{
	if (ent_ind == -1)
		return;

	prefab_ent_info & ent_inf = m_ent_ids[ent_ind];
	
	nsentity * source = get_asset<nsentity>(ent_inf.id);

	nstform_comp * tfc = source->get<nstform_comp>();
	nsprefab_reference_comp * refc = source->get<nsprefab_reference_comp>();
	if (refc != nullptr)
	{
        // this means the source references another prefab
		// need to create another ref from the source prefab
		nsentity * src_prefab = refc->get_source_prefab();
		nsentity * src_ent = refc->get_source_ent();
		
		nsprefab_comp * pc = src_prefab->get<nsprefab_comp>();
		pc->create_reference(create_in, src_ent->full_id(), ref);
		return;
	}

	// Create the reference entity and add it to the ent info for this prefab
    nsentity * ent = create_in->create<nsentity>(source->name() + "_" + std::to_string(ent_inf.refs.size()));

	// Now create the prefab_reference component and set it to reference this ent as the prefab
	// and source as the entitiy
	refc = ent->create<nsprefab_reference_comp>();
	refc->ref_id = ent_inf.refs.size();
	refc->prefab_id = m_owner->full_id();
	refc->ent_id = source->full_id();

	// Add the newly created ref as a reference
	ent_inf.refs.push_back(ent->full_id());

	// Fill out the ref struct
	ref.ent_id = ent->full_id();
	ref.tf_info = tfc->tf_info();

	// hook up signal to remove ref if removed
	sig_connect(ent->component_removed, nsprefab_comp::on_ref_ent_comp_removed);
}

void nsprefab_comp::on_ref_ent_comp_removed(nscomponent * comp)
{
	if (comp->type() == type_to_hash(nsprefab_reference_comp))
	{
		sig_disconnect(comp->owner()->component_removed);
		nsprefab_reference_comp * rcomp = static_cast<nsprefab_reference_comp*>(comp);

		nsentity * prefab_ent = rcomp->get_source_prefab();
		assert(this == prefab_ent);
		
		int index = find_entity_index(rcomp->get_source_ent()->full_id());
		prefab_ent_info & ent_inf = m_ent_ids[index];

		int new_id = 0;
		auto ref_iter = ent_inf.refs.begin();
		while (ref_iter != ent_inf.refs.end())
		{
			if (*ref_iter == rcomp->owner()->full_id())
				ref_iter = ent_inf.refs.erase(ref_iter);
			else
			{
				nsentity * ent = get_asset<nsentity>(*ref_iter);
				nsprefab_reference_comp * refc = ent->get<nsprefab_reference_comp>();
				int old_id = refc->ref_id;
				if (new_id != old_id)
				{
					refc->ref_id = new_id;
					emit_sig refc->reference_id_changed(old_id, new_id);
				}
				++new_id;
				++ref_iter;
			}
		}
	}
}

