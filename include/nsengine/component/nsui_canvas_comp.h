/*!
\file nsui_canvas_comp.h

\brief Header file for nsui_canvas_comp class

This file contains all of the neccessary declarations for the nsui_canvas_comp class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#ifndef NSUI_CANVAS_COMP_H
#define NSUI_CANVAS_COMP_H

#include <nscomponent.h>
#include <nsrect_tform_comp.h>
#include <nsentity.h>
#include <nsrouter.h>

class nsui_canvas_comp : public nscomponent
{
	struct rect_info;
	SLOT_OBJECT
public:

	friend class nsui_system;
	
	template <class PUPer>
	friend void pup(PUPer & p, nsui_canvas_comp & tc);

	template <class PUPer>
	friend void pup(PUPer & p, nsui_canvas_comp::rect_info & ri, const nsstring & var_name);
	
	nsui_canvas_comp();
	nsui_canvas_comp(const nsui_canvas_comp & rhs);

	virtual ~nsui_canvas_comp();

	nsui_canvas_comp * copy(const nscomponent * comp_);

	void init();

	void add(nsentity * to_add);

	void remove(nsentity * to_remove, bool remove_children);

	template<class comp_type>
	entity_set * entities_with_comp()
	{
		return entities_with_comp(type_to_hash(comp_type));
	}
	
	entity_set * entities_with_comp(uint32 comp_type_id);

	entity_set * entities_in_canvas();

	void update_rects(const fvec2 & pscreen_size);

	void release();

	void finalize();

	void pup(nsfile_pupper * p);

	nsui_canvas_comp & operator=(nsui_canvas_comp rhs_);

	std::vector<nsentity*> m_ordered_ents;
	
  private:	
	void _populate_pup_vec();
	void _remove_all_comp_entries(nsentity * ent);
	void _add_all_comp_entries(nsentity * ent);
	
	// private slots
	void _on_comp_add(nscomponent * comp_t);
	void _on_comp_remove(nscomponent * comp_t);
	std::vector<uivec2> m_unloaded_ents;
	std::unordered_map<uint32, std::unordered_set<nsentity*>> m_ents_by_comp;
	
	struct rect_info
	{
		per_canvas_info pci;
		uivec2 pupped_parent;
		uivec2 this_ent;
		std::vector<uivec2> m_pupped_children;
	};
	
	std::vector<rect_info> m_pupped_rects;
};

template <class PUPer>
void pup(PUPer & p, nsui_canvas_comp::rect_info & ri, const nsstring & var_name)
{
	pup(p, ri.pci, var_name + ".per_context_info");
	pup(p, ri.pupped_parent, var_name + ".parent");
	pup(p, ri.this_ent, var_name + ".ent_id");
	pup(p, ri.m_pupped_children, var_name + ".children");
}

template <class PUPer>
void pup(PUPer & p, nsui_canvas_comp & tc)
{
	tc.m_pupped_rects.clear();
	tc._populate_pup_vec();
	pup(p, tc.m_pupped_rects, "child_rects");
}

#endif
