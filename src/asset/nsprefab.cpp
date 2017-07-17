#include <asset/nsprefab.h>
#include <nsworld_data.h>
#include <component/nsprefab_reference_comp.h>

nsprefab::nsprefab():
	nsasset(type_to_hash(nsprefab)),
	m_ents(new nstform_ent_chunk(m_name))
{
	
}

nsprefab::nsprefab(const nsprefab & copy_):
	nsasset(copy_)
{
	delete m_ents;	
}

nsprefab::~nsprefab()
{
	
}

nstform_ent_chunk * nsprefab::prefab_entities()
{
	return m_ents;
}

nsprefab & nsprefab::operator=(nsprefab rhs)
{
	return *this;
}

void nsprefab::pup(nsfile_pupper * p)
{
		
}

void nsprefab::init()
{
	m_ents->m_name = m_name;
	m_ents->m_plug_id = m_plugin_id;
}

void nsprefab::create_references(nstform_ent_chunk * chunk)
{
	if (m_ents == chunk) // can't add ourselves to ourselves.. start getting crazy
		return;
	
	entity_set * all_ents = m_ents->all_entities();
	if (all_ents == nullptr)
	{
		dprint("Cannot create references from empty prefab : " + m_name);
		return;
	}

	// Iterate over all the entities in prefab and create a reference to it in the provided
	// chunk - only iterate over
	auto iter = all_ents->begin();
	while (iter != all_ents->end())
	{
		nstform_comp * our_tform = (*iter)->get<nstform_comp>();
		tform_info copy = our_tform->tf_info();

		// Now zero out the parent and children for now - we will have to re-iterate and assign
		// the parent/children with the tform comp of all the new entities.
		copy.m_children.clear();
		copy.m_parent = 0;

		// Now create the entity in the chunk, add a prefab_reference_comp, connect the entities comp
		// removed signal
		uint32 ref_id = m_refs[(*iter)->id()].size();
		nsentity * ref_ent = chunk->create_entity((*iter)->name() + "_" + std::to_string(ref_id));
		nsprefab_reference_comp * ref_comp = ref_ent->create<nsprefab_reference_comp>();
		ref_comp->prefab_id = full_id();
		ref_comp->ref_id = ref_id;
		ref_comp->ent_id = (*iter)->id();

		// Create an entry in our reference map, or increase
		reference_entry refent;
		refent.chunk_id = chunk->m_id;
		refent.plugin_id = chunk->m_plug_id; // this will be 0 for world chunks
		refent.ref_comp = ref_comp;
		m_refs[(*iter)->id()].push_back(refent);
				
		sig_connect(ref_ent->component_removed, nsprefab::on_ref_ent_comp_removed);
		++iter;
	}

	// Go through again - this time set the parent/children relationship
	iter = all_ents->begin();
	while (iter != all_ents->end())
	{
		nstform_comp * our_tform = (*iter)->get<nstform_comp>();
		tform_info copy = our_tform->tf_info();
//		nstform_comp * 
	}

}

void nsprefab::on_ref_ent_comp_removed(nscomponent * comp)
{
	if (comp->type() == type_to_hash(nsprefab_reference_comp))
	{
		sig_disconnect(comp->owner()->component_removed);
		nsprefab_reference_comp * pfcomp = static_cast<nsprefab_reference_comp*>(comp);
		nsentity * prefab = m_ents->find_entity(pfcomp->ent_id);
		std::vector<reference_entry> & refs = m_refs[pfcomp->ent_id];

		if (refs.empty())
		{
			dprint("BUG: Reference vector for prefab " + prefab->name() + " is empty though there still exists an entity that references it: " + pfcomp->owner()->name());
			return;
		}

		// If the ref being removed still has the default name - we need to change it to indicate
		// its no longer referencing any prefabs
		if (pfcomp->owner()->name() == prefab->name() + "_" + std::to_string(pfcomp->ref_id))
			pfcomp->owner()->rename(prefab->name() + "_unlinked");

		// If this is the last element in the reference vector than just pop the back
		// No need for trying to rename the ref ent or exchanging the id
		if (pfcomp->ref_id == refs.size()-1)
		{
			refs.pop_back();

			// if the ref vec is empty now, remove its entry completely
			if (refs.empty())
				m_refs.erase(prefab->id());
			return;
		}

        // Other wise, first move the last reference to this references position in the vector,
		// change the moved reference's name (if still set to the default), then pop the back
		// No need to check if empty - since we only ever remove one ref at a time the above
		// check will catch an empty vec no matter what
		uint32 old_id = refs.back().ref_comp->ref_id;
		uint32 new_id = pfcomp->ref_id;
		assert(old_id == refs.size()-1);
		
		refs[new_id] = refs.back();
		refs[new_id].ref_comp->ref_id = new_id;
		refs[pfcomp->ref_id].ref_comp->reference_id_changed(old_id,new_id);

		nsentity * moved_ref = refs[new_id].ref_comp->owner();

		// Change the name if it is still the default created name for the ref
		if (moved_ref->name() == prefab->name() + "_" + std::to_string(old_id))
			moved_ref->rename(prefab->name() + "_" + std::to_string(new_id));

		// Get rid of the ref!
		refs.pop_back();
	}		
}

void nsprefab::add_copy_comp(uint32 comp_id, nsentity * ent)
{
	copy_on_create_only[comp_id].insert(ent->id());
}

void nsprefab::remove_copy_comp(uint32 comp_id, nsentity * ent)
{
	copy_on_create_only[comp_id].erase(ent->id());
	if (copy_on_create_only[comp_id].empty())
		copy_on_create_only.erase(comp_id);
}
