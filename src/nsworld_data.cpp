#include <nsworld_data.h>
#include <nstform_ent_chunk.h>
#include <asset/nsprefab.h>

// #include <nsrouter.h>
// #include <nsentity.h>
// #include <nsevent.h>


nstform_ent_chunk * nsworld_data::create_chunk(const nsstring & name)
{
	nstform_ent_chunk * nc = new nstform_ent_chunk(name);
	auto iter = m_chunks.emplace(nc->chunk_id(), nc);
	if (!iter.second)
	{
		delete nc;
		return nullptr;
	}
	emit_sig chunk_added(nc);
	return nc;
}

nstform_ent_chunk * nsworld_data::create_chunk_from_prefab(nsprefab * pf, const nsstring & name_wanted_if_different_than_prefab)
{
	nsstring name = pf->name();
	if (name_wanted_if_different_than_prefab != "")
		name = name_wanted_if_different_than_prefab;
	
	nstform_ent_chunk * chnk = create_chunk(name);
	if (chnk == nullptr)
		return nullptr;
	chnk->add_prefab(pf);
	return chnk;
}

bool nsworld_data::save_chunk_to_prefab(const nsstring & name, nsprefab * pf)
{
	return save_chunk_to_prefab(chunk(name), pf);
}

bool nsworld_data::save_chunk_to_prefab(uint32 chunk_id, nsprefab * pf)
{
	return save_chunk_to_prefab(chunk(chunk_id), pf);
}

bool nsworld_data::save_chunk_to_prefab(nstform_ent_chunk * chnk, nsprefab * pf)
{
	entity_set * all_ents = chnk->all_entities();
	nstform_ent_chunk * pf_chnk = pf->prefab_entities();	

	if (all_ents == nullptr)
		return false;
	
	// First check to make sure there are no ents in prefab with same name as ents in chunk
	auto ent_iter = all_ents->begin();
	while (ent_iter != all_ents->end())
	{
		nsentity * ent = pf_chnk->find_entity((*ent_iter)->id());
		if (ent != nullptr)
		{
			dprint("Cannot save chunk " + chnk->chunk_name() + " to prefab " + pf->name() + " because the prefab already contains entities with the same id as those that are in the chunk (" + ent->name() + ")");
			return false;
		}
		++ent_iter;
	}

	// Now go through and add all ents ignoring scene graph - except remembering that we need
	// to add children once finished
	std::map<uint32, std::vector<uint32>> need_to_add_children;
	auto iter = all_ents->begin();
	while (iter != all_ents->end())
	{
		nsentity * copy = new nsentity(*(*iter));
		tform_info tfi = (*iter)->get<nstform_comp>()->tf_info();

		// This will work since the new entity in the prefab will have the same id as the ent in the chunk
		for (int i = 0; i < tfi.m_children.size(); ++i)
			need_to_add_children[copy->id()].push_back(tfi.m_children[i]);

		tfi.m_parent = 0;
		tfi.m_children.clear();
		
		pf_chnk->add_entity(copy, &tfi, false);
		++iter;
	}

	// Now go through and assign the children to restore the scene graph in the prefab
	auto add_children = need_to_add_children.begin();
	while (add_children != need_to_add_children.end())
	{
		nstform_comp * tfc = pf_chnk->find_entity(add_children->first)->get<nstform_comp>();
		for (uint i = 0; i < add_children->second.size(); ++i)
		{
			nsentity * child = pf_chnk->find_entity(add_children->second[i]);
			tfc->add_child(child->get<nstform_comp>(), false);
		}
		++add_children;
	}
	return true;
}

	
int nsworld_data::destroy_chunk(const nsstring & name)
{
	auto iter = m_chunks.find(hash_id(name));
	if (iter != m_chunks.end())
	{
		nstform_ent_chunk * chnk = iter->second;
		iter = m_chunks.erase(iter);
		emit_sig chunk_removed(chnk);
		delete chnk;
		return 1;
	}
	return 0;
}

nstform_ent_chunk * nsworld_data::chunk(const nsstring & name)
{
	return chunk(hash_id(name));
}

nstform_ent_chunk * nsworld_data::chunk(uint32 id)
{
	auto fiter = m_chunks.find(id);
	if (fiter != m_chunks.end())
		return fiter->second;
	return nullptr;	
}

chunk_map::iterator nsworld_data::begin()
{
	return m_chunks.begin();
}

chunk_map::iterator nsworld_data::end()
{
	return m_chunks.end();
}
