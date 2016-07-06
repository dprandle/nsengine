/*! 
	\file nsscene.h
	
	\brief Header file for nsscene class

	This file contains all of the neccessary declarations for the nsscene class.

	\author Daniel Randle
	\date November 23 2013
	\copywrite Earth Banana Games 2013
*/

#ifndef NSSCENE_H
#define NSSCENE_H

#define SCENE_MAX_PLAYERS 16

#include <nsasset.h>
#include <nsentity.h>
#include <nstile_grid.h>
#include <nsoccupy_comp.h>

class nsrender_comp;
class nstform_system;

class nsscene : public nsasset
{
	SLOT_OBJECT
	public:

	template<class PUPer>
	friend void pup(PUPer & p, nsscene & sc);

	nsscene();

	nsscene(const nsscene & copy_);

	~nsscene();
	
	nsscene & operator=(nsscene rhs);

	uint32 add(
		nsentity * ent,
		instance_tform * parent,
		const instance_tform & tf_info,
		bool add_chilren);

	uint32 add(
		nsentity * ent_,
		instance_tform * parent = nullptr,
		bool snap_to_grid = true,
		const fvec3 & pos_ = fvec3(),
		const fquat & rot_ = fquat(),
		const fvec3 & scale_ = fvec3(1.0f, 1.0f, 1.0f)
		);


	void add_gridded(
		nsentity * ent_, 
		const ivec3 & bounds_, 
		const fvec3 & starting_pos_=fvec3(),
		const fquat & rot_ = fquat(),
		const fvec3 & scale_ = fvec3(1.0f,1.0f,1.0f)
		);

	void change_max_players(int32 amount_);

	void clear();

	nsentity * find_entity(uint32 plug_id, uint32 res_id) const;

	nsentity * find_entity(const uivec2 & id) const;

	uivec3 ref_id(const fvec3 & pWorldPos) const;

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

	bool remove(instance_tform * itform, bool remove_children);

	bool remove(nsentity * entity, uint32 tformid, bool remove_children);

    void set_bg_color(const fvec4 & bg_color);

	void set_creator(const nsstring & pCreator);

	const nsstring & creator() const;

	void set_max_players(uint32 pMaxPlayers);

	void set_notes(const nsstring & pNotes);

	void set_skydome(nsentity * skydome, bool addToSceneIfNeeded = true);

	uivec2_vector & unloaded();

private:

	void _remove_all_comp_entries(nsentity * ent);
	void _add_all_comp_entries(nsentity * ent);
	void _on_comp_remove(nscomponent * comp_t);
	void _on_comp_add(nscomponent * comp_t);
	void _populate_pup_vec();
	struct instance_tform_info
	{
		instance_tform_info(nsscene * scn, const instance_tform & it);

		uivec2 ent_id;
		bool render_update;
		int32 hidden_state;
		fquat orient;
		fvec3 position;
		fvec3 scaling;
		fmat4 world_tform;
		fmat4 world_inv_tform;
		fmat4 local_tform;
		fmat4 local_inv_tform;

		uivec3 parent;
		std::vector<uivec3> children;
	};
	
	nsentity *  m_skydome;
	uint32 m_max_players;
    fvec4 m_bg_color;
	nsstring m_notes;
	nsstring m_creator;
	bool m_enabled;
	nstile_grid * m_tile_grid;
	std::unordered_map<uint32, std::unordered_set<nsentity*>> m_ents_by_comp;
	uivec2_vector m_unloaded;

	std::vector<instance_tform_info> m_pupped_tforms;
};


template<class PUPer>
void pup(PUPer & p, nsscene & sc)
{
//	sc.m_pupped_tforms.clear();
//	sc._populate_pup_vec();
//	pup(p, sc.m_pupped_tforms, "scene_tforms");

	// pup(p, sc.m_max_players, "max_players");
	// pup(p, sc.m_bg_color, "bg_color");
	// pup(p, sc.m_notes, "notes");
	// pup(p, sc.m_creator, "creator");
}

#endif
