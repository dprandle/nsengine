#include <nsui_tform_ent_chunk.h>
#include <asset/nsprefab.h>

nsui_tform_ent_chunk::nsui_tform_ent_chunk(const nsstring & name):
	m_name(name),
	m_id(hash_id(name)),
	m_plug_id(0)
{}

nsui_tform_ent_chunk::nsui_tform_ent_chunk(const nsui_tform_ent_chunk & copy)
{
}

nsui_tform_ent_chunk::~nsui_tform_ent_chunk()
{}

nsentity * nsui_tform_ent_chunk::create_entity(
	const nsstring & name,
	tform_info * tf_info,
	bool tform_is_world_space)
{
	dprint("Creating entity " + name + " in chunk " + m_name);
	nsentity * ent = new nsentity;
	ent->rename(name);
	if (add_entity(ent,tf_info,tform_is_world_space) == nullptr)
	{
		delete ent;
		return nullptr;
	}
	return ent;	
}

void nsui_tform_ent_chunk::set_process_flag(uint32 new_flag)
{
	m_process_flag = new_flag;
}

uint32 nsui_tform_ent_chunk::process_flag()
{
	return m_process_flag;
}

bool nsui_tform_ent_chunk::process_flag_set(process_flag_bit flag)
{
	return (m_process_flag & flag) == flag;
}

void nsui_tform_ent_chunk::set_process_flag_bits(uint32 flags)
{
	m_process_flag |= flags;
}

void nsui_tform_ent_chunk::add_prefab(nsprefab * prefab)
{
	prefab->create_references(this);
}

void nsui_tform_ent_chunk::_on_ent_rename(const uivec2 & ids)
{
	nsentity * ent = find_entity(ids.x);
	if (ent != nullptr)
	{
		nsentity * changed_to_name = find_entity(ids.y);
		if (changed_to_name != nullptr)
		{
			nsstring new_name = ent->name() + std::to_string(ids.x);
			dprint("Had to edit name in chunk " + m_name + " during name change from old ent id " + std::to_string(ids.x) + ") to proposed name " + ent->name() + " (id " + std::to_string(ent->id()) + ") as it is not unique - changing to " + new_name + " (id " + std::to_string(hash_id(new_name)) + ")");
			ent->rename(new_name);
		}
		else
		{			
			dprint("Successfully changed entity name change in chunk " + m_name + " from old ent id " + std::to_string(ids.x) + " to new id " + std::to_string(ids.y));
			m_owned_ents.erase(ids.x);
			m_owned_ents[ids.y] = ent;
		}
	}
}

nstform_comp * nsui_tform_ent_chunk::add_entity(
	nsentity * ent,
	tform_info * tf_info,
	bool tform_is_world_space)
{
	auto ret = m_owned_ents.emplace(ent->id(), ent);
	if (!ret.second)
		return nullptr;

	nstform_comp * tf_comp = ent->get<nstform_comp>();
	
	// If there is no tranform component, then make one otherwise cancel adding and return nullptr
	if (tf_comp == nullptr)
	{
		tf_comp = ent->create<nstform_comp>();
		tf_comp->m_owning_chunk = this;
		if (tf_info != nullptr)
		{
			dprint("Entity " + ent->name() + " is being added to chunk " + m_name + " with " + tf_info->to_string());
			tf_comp->set_tf_info(*tf_info, tform_is_world_space);				
		}
		else
		{
			dprint("Entity " + ent->name() + " is being added to chunk " + m_name + " with null tf_info");
		}
	}
	else
	{
		dprint("Entity " + ent->name() + " already has tform_comp and cannot be added to chunk " + m_name);
		m_owned_ents.erase(ent->id());
		return nullptr;
	}

	_add_all_comp_entries(ent);
	sig_connect(ent->component_added, nsui_tform_ent_chunk::_on_comp_add);
	sig_connect(ent->component_removed, nsui_tform_ent_chunk::_on_comp_remove);
	tf_comp->post_update(true);
	emit_sig entity_added(ent);
	return tf_comp;
}

nsentity * nsui_tform_ent_chunk::find_entity(uint32 id) const
{
	auto fiter = m_owned_ents.find(id);
	if (fiter != m_owned_ents.end())
		return fiter->second;
	return nullptr;
}

entities_removed nsui_tform_ent_chunk::remove_entity(nsentity * ent, bool remove_children)
{
	entities_removed ret;
	ret.root = nullptr;
	
	size_t cnt = m_owned_ents.erase(ent->id());
	if (cnt == 0)
	{
		dprint("Could not remove " + ent->name() + " from chunk " + m_name + " as the entity was not found in the chunk");
		return ret;
	}
	dprint("Successfully removed " + ent->name() + " from chunk " + m_name);
	ret.root = ent; // successfully erased from owned ents so set the return.root value
	
	nstform_comp * tf_comp = ent->get<nstform_comp>();
	nstform_comp * parent = tf_comp->parent();

	// If removing children go through and delete them all - otherwise re-assign children's parent
	// to our parent (which could be null if we are root item)
	if (remove_children)
	{
		while (tf_comp->child_count() > 0)
		{
			entities_removed rem_ents = remove_entity(tf_comp->child(0)->owner(),true); // append the removed child, if it was removed
			if (rem_ents.root != nullptr)
				ret.children.push_back(rem_ents.root);
			
			// Insert all removed children in to our removed children list.. so that for example
			// if we have 2 children, each having 5 children, if all are successfully removed then
			// ret.children would have a total of 12 entities in it
			ret.children.insert(ret.children.end(), rem_ents.children.begin(), rem_ents.children.end());
		}
	}
	else
	{
		for (uint32 i = 0; i < tf_comp->child_count(); ++i)
			tf_comp->child(i)->set_parent(parent, true); // true because we want everything to stay put
	}

	tf_comp->set_parent(nullptr, true); // remove ourself from parent if not already root

	// Disconnect from the add/remove comp signals for this ent
	sig_disconnect(tf_comp->owner()->component_added);
	sig_disconnect(tf_comp->owner()->component_removed);

	// Also get rid of all component entries
	_remove_all_comp_entries(tf_comp->owner());

	// Finally destroy the component
	ent->destroy<nstform_comp>();
	
	emit_sig entity_removed(ent);
	
	// Ret should effectively contain all removed ents now
	return ret;
}

uint32 nsui_tform_ent_chunk::destroy(nsentity * ent, bool destroy_children)
{
	entities_removed rem = remove_entity(ent, destroy_children);
	uint32 cnt = 0;

	// Destroy the main entity being passed in if it was successfully removed from chunk
	if (rem.root != nullptr)
	{
		delete rem.root;
		++cnt;
	}
	
	// Destroy all of the children that were removed from the chunk
	while (rem.children.begin() != rem.children.end())
	{
		++cnt;
		delete rem.children.back();
		rem.children.pop_back();
	}

	dprint("Destroyed " + std::to_string(cnt) + " entity/ies in chunk " + m_name);
	return cnt;
}
	
nsentity * nsui_tform_ent_chunk::find_entity(const nsstring & name) const
{
	return find_entity(hash_id(name));	
}

void nsui_tform_ent_chunk::rename(const nsstring & new_name)
{
	nsstring tmp(m_name);
	uint32 tmp_id(m_id);
	
	m_name = new_name;
	m_id = hash_id(m_name);
	emit_sig renamed(tmp,tmp_id,m_name,m_id);
}

const nsstring & nsui_tform_ent_chunk::chunk_name()
{
	return m_name;
}

uint32 nsui_tform_ent_chunk::chunk_id()
{
	return m_id;
}

const entity_set * nsui_tform_ent_chunk::entities_with_comp(uint32 comp_type_id) const
{
	auto fiter = m_ents_by_comp.find(comp_type_id);
	if (fiter != m_ents_by_comp.end())
		return &fiter->second;
	return nullptr;
}

entity_set * nsui_tform_ent_chunk::entities_with_comp(uint32 comp_type_id)
{
	auto fiter = m_ents_by_comp.find(comp_type_id);
	if (fiter != m_ents_by_comp.end())
		return &fiter->second;
	return nullptr;		
}

entity_set * nsui_tform_ent_chunk::all_entities()
{
	return entities_with_comp<nstform_comp>();
}

// This adds the component from the comp by type list in the scene
void nsui_tform_ent_chunk::_on_comp_add(nsentity * ent, nscomponent * comp_t)
{
	auto fiter = m_ents_by_comp.emplace(comp_t->type(), std::unordered_set<nsentity*>());
	fiter.first->second.emplace(ent);
	dprint("Added component " + hash_to_guid(comp_t->type()) + " to " + ent->name() + " in chunk " + m_name);
}

// This removes the component from the comp by type list in the scene
void nsui_tform_ent_chunk::_on_comp_remove(nsentity * ent, nscomponent * comp_t)
{
	auto fiter = m_ents_by_comp.find(comp_t->type());
	fiter->second.erase(ent);
	dprint("Removed component " + hash_to_guid(comp_t->type()) + " from " + ent->name() + " in chunk " + m_name);
}

void nsui_tform_ent_chunk::_remove_all_comp_entries(nsentity * ent)
{
	std::set<uint32> comps;
	ent->get_comp_set(comps);
	auto iter = comps.begin();
	while (iter != comps.end())
	{
		_on_comp_remove(ent, ent->get(*iter));
		++iter;
	}
}

void nsui_tform_ent_chunk::_add_all_comp_entries(nsentity * ent)
{
	std::set<uint32> comps;
	ent->get_comp_set(comps);
	auto iter = comps.begin();
	while (iter != comps.end())
	{
		_on_comp_add(ent, ent->get(*iter));
		++iter;
	}
}

