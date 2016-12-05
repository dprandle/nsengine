#ifndef NSPREFAB_COMP_H
#define NSPREFAB_COMP_H

#include <component/nscomponent.h>
#include <component/nstform_comp.h>

class nsplugin;

struct prefab_ent_info
{
 	prefab_ent_info(const uivec2 & id_ = uivec2()):
		id(id_),
		refs()
	{}
	
	uivec2 id;
	std::vector<uivec2> refs;
};

class nsprefab_comp : public nscomponent
{
	SLOT_OBJECT
	
  public:
	
	nsprefab_comp();

	nsprefab_comp(const nsprefab_comp & copy);

	~nsprefab_comp();

    void create_references(nsplugin * create_in, std::vector<packed_ent_tform> & refs);

	void create_reference(nsplugin * create_in, const uivec2 & source_ent_id, packed_ent_tform & ref);

	void init();

	void release();

	void finalize();

	void name_change(const uivec2 &, const uivec2);

	uivec3_vector resources();

	void pup(nsfile_pupper * p);

	nstform_comp * add_entity(const uivec2 & ent_id,
				   tform_info * tf_info,
				   bool data_in_world_space);

	nsentity * remove_entity(const uivec2 & ent_id);

	nsentity * find_entity(const uivec2 & ent_id);

	ns::signal<nsentity*> entity_added;
	ns::signal<nsentity*> entity_removed;

private:

    void on_ref_ent_comp_removed(nscomponent * comp);

	void create_reference(nsplugin * create_in, int32 ent_ind, packed_ent_tform & ref);

	int32 find_entity_index(const uivec2 & ent_id);

	void make_valid(tform_info * tf);

	void on_tform_remove(nscomponent * comp);

	std::vector<prefab_ent_info> m_ent_ids;

};

#endif
