#ifndef NSTFORM_ENT_CHUNK
#define NSTFORM_ENT_CHUNK

#include <nsentity.h>

struct entities_removed
{
	nsentity * root;
	std::vector<nsentity*> children;
};

class nsprefab;

enum process_flag_bit
{
	pfb_render=1
};

class nstform_ent_chunk
{

	SLOT_OBJECT

	friend class nsworld_data;
	friend class nsprefab;
	
  public:

	nstform_ent_chunk(const nsstring & name);

	nstform_ent_chunk(const nstform_ent_chunk & copy);

	~nstform_ent_chunk();

	/*
	  This function creates the entity - meaning allocates the memory for it - then adds it to the chunk.
	  It is possible to create the entity yourself, call add_entity and the chunk will take ownership,
	  however in the future custom allocators might be used in chunks that are more efficient than simple
	  new - which means its probably a good idea to create entities with this function.
	 */
	nsentity * create_entity(
		const nsstring & name,
		tform_info * tf_info = nullptr,
		bool tform_is_world_space = true);
	

	/*
	  Add the entity to the chunk using the tform info to get the position/orientation/scaling
	  If the tform info has a parent id or children ids assigned, it will try to assign those
	  entities as parent or children respectively. It will only work if those entities are already
	  in the chunk though.

	  The tform_is_world_space tells the chunk whether the position and orientation
	  are world space or local space - ie if tform_is_world_space is false then the pos and orient will
	  be taken as relative to the parent - if there is no parent then this will be the same as world
	  space and the bool doesn't really matter.

	  If tform_is_world_space is true, then the position and orientation will be taken as world space.
	  This means that if there is a parent, the actual position and orientation in the new tform will
	  be adjusted so that the final world space position and orientation after transforming by the
	  parent (and actually the entire parent heirarchy) will be the position/orientation passed in
	  with the tf_info.
	 */
	nstform_comp * add_entity(
		nsentity * ent,
		tform_info * tf_info = nullptr,
		bool tform_is_world_space = true);

	/*
	  This function is really a shortcut - you can just as easily call prefab->create_references(this_chunk);
	  In fact, that's all this function does. The prefab knows how to correctly create all the necessary
	  entities to represent the prefab.
	 */
	void add_prefab(nsprefab * prefab);
	
	/*
	  Find the entity with id in owned ents and return it
	 */
	nsentity * find_entity(uint32 id) const;

	/*
	  Find the entity with id in owned ents and return it
	*/
	nsentity * find_entity(const nsstring & name) const;

	/*
	  Removes the entity ent from the chunk, and its children if bool is true, which also disconnets all
	  signals from the entities. After this call the chunk will no longer own the returned entities
	  given in entities_removed, which means the caller will now own them.
	 */
	entities_removed remove_entity(nsentity * ent, bool remove_children);

	/*
	  Removes the entity (and children if bool is true) from the chunk using the remove function call,
	  then destroys them. Returns the total number of entities destroyed including the root entity.
	  Will be 0 on failure.
	 */
	uint32 destroy(nsentity * ent, bool destroy_children);

	void rename(const nsstring & new_name);

	const nsstring & chunk_name();

	uint32 chunk_id();

	template<class comp_type>
	const entity_set * entities_with_comp() const
	{
		return entities_with_comp(type_to_hash(comp_type));
	}

	template<class comp_type>
	entity_set * entities_with_comp()
	{
		return entities_with_comp(type_to_hash(comp_type));
	}

	void set_process_flag(uint32 new_flag);

	bool process_flag_set(process_flag_bit flag);

	void set_process_flag_bits(uint32 flags);

	uint32 process_flag();

	const entity_set * entities_with_comp(uint32 comp_type_id) const;

	entity_set * entities_with_comp(uint32 comp_type_id);

	entity_set * all_entities();

	// Entity and chunk id
	ns::signal<nsentity*> entity_added;
	ns::signal<nsentity*> entity_removed;
	
	// Format here is old_name, old_id, new_name, new_id
	ns::signal<const nsstring &, uint32, const nsstring &, uint32> renamed;
	

  private:

	void _remove_all_comp_entries(nsentity * ent);
	void _add_all_comp_entries(nsentity * ent);

	void _on_comp_remove(nsentity * ent, nscomponent * comp_t);
	void _on_comp_add(nsentity * ent, nscomponent * comp_t);
	void _on_comp_edit(nsentity * ent, nscomponent * comp_t);
	
	void _on_ent_rename(const uivec2 & ids);

	nsstring m_name;

	uint32 m_process_flag;
	uint32 m_id;
	uint32 m_plug_id;

	std::unordered_map<uint32, nsentity*> m_owned_ents;
	std::unordered_map<uint32, entity_set> m_ents_by_comp;
};

#endif
