#ifndef NSPREFAB_H
#define NSPREFAB_H

#include <nsengine.h>
#include <asset/nsasset.h>
#include <nsrouter.h>

class nstform_ent_chunk;
class nsentity;
class nsprefab_reference_comp;

struct reference_entry
{
	reference_entry():
		plugin_id(0),
		chunk_id(0),
		ref_comp(nullptr)
	{}
	
	uint32 plugin_id; // Will be zero when reference is in a world chunk
	uint32 chunk_id; // Same as prefab id when the reference is in another prefab chunk
	nsprefab_reference_comp * ref_comp;
};
	
class nsprefab : public nsasset
{

	SLOT_OBJECT
	
  public:

	template <class PUPer>
	friend void pup(PUPer & p, nsprefab & shader);

	nsprefab();

	nsprefab(const nsprefab & copy_);

	~nsprefab();

	nsprefab & operator=(nsprefab rhs);

	void init();

	virtual void pup(nsfile_pupper * p);

	nstform_ent_chunk * prefab_entities();

	void create_references(nstform_ent_chunk * chunk);

	template<class T>
	void add_copy_comp(nsentity * ent)
	{
		add_copy_comp(type_to_hash(T), ent);
	}

	void add_copy_comp(uint32 comp_id, nsentity * ent);

	template<class T>
	void remove_copy_comp(nsentity * ent)
	{
		remove_copy_comp(type_to_hash(T), ent);
	}

	void remove_copy_comp(uint32 comp_id, nsentity * ent);

  private:

	void on_ref_ent_comp_removed(nscomponent * comp);

	// This is entity -> set<comp_hashed_type>. It is a set of component types that should
	// be copied from the prefab entity whenever references are made for that entity
	// If an ent with prefab_ref_comp doesn't have some component, it searches the prefab
	// for that component whenever the comp is requested. By copying the comp the reference will
	// take the values from the prefab initially, but will not be linked after because it has its
	// own version of the comp
	std::unordered_map<uint32, std::set<uint32>> copy_on_create_only;

	std::unordered_map<uint32, std::vector<reference_entry>> m_refs;
	
	nstform_ent_chunk * m_ents;
};

template <class PUPer>
void pup(PUPer & p, nsprefab & canvas)
{
}


#endif
