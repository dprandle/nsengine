#ifndef NSUI_TFORM_ENT_CHUNK
#define NSUI_TFORM_ENT_CHUNK

#include <nsentity.h>

struct entities_removed
{
	nsentity * root;
	std::vector<nsentity*> children;
};

class nsui_canvas;

enum process_flag_bit
{
	pfb_render=1
};

class nsui_tform_ent_chunk
{

	SLOT_OBJECT

	friend class nsui_canvas;
	
  public:

	nsui_tform_ent_chunk(const nsstring & name);

	nsui_tform_ent_chunk(const nsui_tform_ent_chunk & copy);

	~nsui_tform_ent_chunk();

	nsentity * create_entity(
		const nsstring & name,
		tform_info * tf_info = nullptr,
		bool tform_is_world_space = true);
	

	nstform_comp * add_entity(
		nsentity * ent,
		tform_info * tf_info = nullptr,
		bool tform_is_world_space = true);

	void add_ui_canvas(nsui_canvas * canvas);
	
	nsentity * find_entity(uint32 id) const;

	nsentity * find_entity(const nsstring & name) const;

	entities_removed remove_entity(nsentity * ent, bool remove_children);

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
