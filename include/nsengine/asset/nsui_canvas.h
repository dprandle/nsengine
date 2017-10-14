#ifndef NSUI_CANVAS_H
#define NSUI_CANVAS_H

#include <nsengine.h>
#include <asset/nsasset.h>
#include <nsrouter.h>

class nsui_tform_ent_chunk;
class nsentity;
class nsui_canvas_reference_comp;

struct ui_reference_entry
{
	ui_reference_entry():
		plugin_id(0),
		chunk_id(0),
		ref_comp(nullptr)
	{}
	
	uint32 plugin_id; // Will be zero when reference is in a ui chunk
	uint32 chunk_id; // Same as canvas id when the reference is in another canvas chunk
	nsui_canvas_reference_comp * ref_comp;
};
	
class nsui_canvas : public nsasset
{

	SLOT_OBJECT
	
  public:

	template <class PUPer>
	friend void pup(PUPer & p, nsui_canvas & shader);

	nsui_canvas();

	nsui_canvas(const nsui_canvas & copy_);

	~nsui_canvas();

	nsui_canvas & operator=(nsui_canvas rhs);

	void init();

	virtual void pup(nsfile_pupper * p);

	nsui_tform_ent_chunk * entities();

	void create_references(nsui_tform_ent_chunk * chunk);

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

	void on_ref_ent_comp_removed(nsentity * ent, nscomponent * comp);

	std::unordered_map<uint32, std::set<uint32>> copy_on_create_only;

	std::unordered_map<uint32, std::vector<ui_reference_entry>> m_refs;
	
	nsui_tform_ent_chunk * m_ents;
};

template <class PUPer>
void pup(PUPer & p, nsui_canvas & canvas)
{
}

#endif
