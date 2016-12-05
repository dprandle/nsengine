/*! 
	\file nsmap_area.h
	
	\brief Header file for nsmap_area class

	This file contains all of the neccessary declarations for the nsmap_area class.

	\author Daniel Randle
	\date November 23 2013
	\copywrite Earth Banana Games 2013
*/

#ifndef NSSCENE_H
#define NSSCENE_H

#define SCENE_MAX_PLAYERS 16

#include <asset/nsasset.h>
#include <asset/nsentity.h>
//#include <nstile_grid.h>
#include <component/nsoccupy_comp.h>

class nsprefab_comp;
class nstile_grid;
class nscube_grid;
class nsrender_comp;
class nstform_system;
class nstform_comp;

typedef std::vector<packed_ent_tform> packed_tform_vec;

class nsmap_area : public nsasset
{
	SLOT_OBJECT
	public:
	
	template<class PUPer>
	friend void pup(PUPer & p, nsmap_area & sc);

	nsmap_area();

	nsmap_area(const nsmap_area & copy_);

	~nsmap_area();
	
	nsmap_area & operator=(nsmap_area rhs);

	nstform_comp * add_entity(
		nsentity * ent,
		tform_info * tf_info = nullptr,
		bool tform_is_world_space = true);

	void change_max_players(int32 amount_);

	void clear();

	nsentity * find_entity(const uivec2 & id) const;

	uivec2 ref_id(const fvec3 & pWorldPos) const;

    const fvec4 & bg_color() const;

	bool has_dir_light() const;

	uint32 max_players() const;

	const nsstring & notes() const;

	nsentity * skydome() const;

	virtual void pup(nsfile_pupper * p);

	void init();

	void enable(bool to_enable);

	bool is_enabled();

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

	const entity_set * entities_with_comp(uint32 comp_type_id) const;

	entity_set * entities_with_comp(uint32 comp_type_id);

	const entity_set * entities_in_scene() const;

	entity_set * entities_in_scene();

	uivec3_vector resources();

	nstile_grid & grid();

	void hide_layer(int32 pLayer, bool pHide);

	void hide_layers_above(int32 pBaseLayer, bool pHide);

	void hide_layers_below(int32 pTopLayer, bool pHide);

	void name_change(const uivec2 & oldid, const uivec2 newid);

	bool remove(fvec3 & pWorldPos, bool remove_children);

	bool remove(nsentity * ent, bool remove_children);

    void set_bg_color(const fvec4 & bg_color);

	void set_creator(const nsstring & pCreator);

	const nsstring & creator() const;

	void set_max_players(uint32 pMaxPlayers);

	void set_notes(const nsstring & pNotes);

	void set_skydome(nsentity * skydome, bool add_to_scene_if_needed);

	nscube_grid * cube_grid;

	ns::signal<nsentity*> entity_added;
	ns::signal<nsentity*> entity_removed;
	
private:

	nstform_comp * add_from_prefab(nsprefab_comp * pfc,
								   tform_info * tf_info,
								   bool tform_is_world_space);

	void add_from_packed_vec(const packed_tform_vec & tforms, packed_tform_vec & unable_to_add);

	void make_valid(tform_info * tf);

	void make_ent_instanced_if_needed(nsentity * ent);
	void make_ent_not_instanced(nsentity * ent);
	
	void _remove_all_comp_entries(nsentity * ent);
	void _add_all_comp_entries(nsentity * ent);

	void _on_comp_remove(nscomponent * comp_t);
	void _on_comp_add(nscomponent * comp_t);

	void _populate_pup_vec();

	void _add_children_recursively();

	uivec2 m_skydome;
	uint32 m_max_players;
    fvec4 m_bg_color;
	nsstring m_notes;
	nsstring m_creator;
	bool m_enabled;
	nstile_grid * m_tile_grid;
	
	std::unordered_map<uint32, std::unordered_set<nsentity*>> m_ents_by_comp;

	packed_tform_vec m_unloaded_tforms;
	packed_tform_vec m_pupped_tforms;
};

template<class PUPer>
void pup(PUPer & p, packed_ent_tform & pi, const nsstring & var_name)
{
	pup(p, pi.ent_id, var_name + ".ent_id");
	pup(p, pi.tf_info, var_name + ".tf_info");
}

template<class PUPer>
void pup(PUPer & p, nsmap_area & sc)
{
	if (sc.m_enabled)
	{
		if (p.mode() == PUP_IN)
			return;
		sc._populate_pup_vec();
		pup(p, sc.m_pupped_tforms, "scene_tforms");
		sc.m_pupped_tforms.clear();
	}
	else
	{
        pup(p, sc.m_pupped_tforms, "scene_tforms");
	}
	pup(p, sc.m_skydome, "skydome_id");
    pup(p, sc.m_max_players, "max_players");
	pup(p, sc.m_bg_color, "bg_color");
	pup(p, sc.m_notes, "notes");
	pup(p, sc.m_creator, "creator");
}

#endif
