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

#include <nsresource.h>
#include <nsentity_manager.h>
#include <nstile_grid.h>
#include <nsoccupy_comp.h>

class nsrender_comp;
class nsrender_system;

class nsscene : public nsresource
{
public:
	typedef std::map<uint32, entity_ptr_set> entities_by_comp;

	template<class PUPer>
	friend void pup(PUPer & p, nsscene & sc);

	nsscene();
	~nsscene();

	uint32 add(
		nsentity * ent_,
		const fvec3 & pos_ = fvec3(),
		const fquat & rot_ = fquat(),
		const fvec3 & scale_ = fvec3(1.0f, 1.0f, 1.0f)
		);

	template<class T1, class T2>
	uint32 add(
		const T1 & plug_,
		const T2 & ref_,
		const fvec3 & pos_ = fvec3(),
		const fquat & rot_ = fquat(),
		const fvec3 & scale_ = fvec3(1.0f, 1.0f, 1.0f)
		)
	{
		return add(nse.resource<nsentity>(plug_, ref_), pos_, rot_, scale_);
	}

	uint32 add_gridded(
		nsentity * ent_, 
		const ivec3 & bounds_, 
		const fvec3 & starting_pos_=fvec3(),
		const fquat & rot_ = fquat(),
		const fvec3 & scale_ = fvec3(1.0f,1.0f,1.0f)
		);

	template<class T1, class T2>
	uint32 add_gridded(
		const T1 & plug_,
		const T2 & ref_,
		const ivec3 & bounds_,
		const fvec3 & starting_pos_ = fvec3(),
		const fquat & rot_ = fquat(),
		const fvec3 & scale_ = fvec3(1.0f, 1.0f, 1.0f)
		)
	{
		return add_gridded(nse.resource<nsentity>(plug_, ref_), bounds_, starting_pos_, rot_, scale_);
	}

	void change_max_players(int32 amount_);

	void clear();

	void enable_show_bit(bool pEnable);

	template<class T1, class T2>
	nsentity * entity(const T1 & plug_, const T2 & res_) const
	{
		nsentity * ent = nse.resource<nsentity>(plug_, res_);
		if (ent == NULL || !ent->has<nstform_comp>())
			return NULL;
		return ent;
	}

	nsentity * entity(const uivec2 & id) const { return entity(id.x, id.y); }

	uivec3 ref_id(const fvec3 & pWorldPos) const;

	const fvec3 & bg_color() const;

	nsentity * camera() const;

	bool has_dir_light() const;

	uint32 max_players() const;

	const nsstring & notes() const;

	uint32 ref_count() const;

	nsentity * skydome() const;

	const entity_ptr_set & entities() const;

	virtual void pup(nsfile_pupper * p);

	void init();

	template<class CompType>
	const entity_ptr_set & entities() const
	{
		uint32 type_id = nse.type_id(std::type_index(typeid(CompType)));
		auto fiter = m_ents_by_comp_type.find(type_id);
		if (fiter != m_ents_by_comp_type.end())
			return fiter->second;
		return m_dummyret;
	}

	/*!
	Get the other resources that this Scene uses. This is given by all the Entities that currently exist in the scene.
	\return Map of resource ID to resource type containing all used resources
	*/
	virtual uivec2_vector resources();

	nstile_grid & grid();

	void hide_layer(int32 pLayer, bool pHide);

	void hide_layers_above(int32 pBaseLayer, bool pHide);

	void hide_layers_below(int32 pTopLayer, bool pHide);

	bool show_bit() const;

	/*!
	This should be called if there was a name change to a resource - will check if the resource is used by this component and if is
	is then it will update the handle
	*/
	virtual void name_change(const uivec2 & oldid, const uivec2 newid);

	uint32 replace(nsentity * oldent, uint32 tformID, nsentity * newent);

	template<class T1, class T2>
	uint32 replace(const T1 & oldplug, const T2 & oldent, uint32 tformID, nsentity * newent)
	{
		return replace(entity(oldplug, oldent), tformID, newent);
	}

	template<class T3, class T4>
	uint32 replace(nsentity * oldent, uint32 tformID, const T3 & newplug, const T4 & newent)
	{
		return replace(oldent, tformID, entity(newplug, newent));
	}

	template<class T1, class T2, class T3, class T4>
	uint32 replace(const T1 & oldplug, const T2 & oldent, uint32 tformID, const T3 & newplug, const T4 & newent)
	{
		return replace(entity(oldplug, oldent), tformID, entity(newplug, newent));
	}

	bool replace(nsentity * oldent, nsentity * newent);

	template<class T1, class T2>
	uint32 replace(const T1 & oldplug, const T2 & oldent, nsentity * newent)
	{
		return replace(entity(oldplug, oldent), newent);
	}

	template<class T3, class T4>
	uint32 replace(nsentity * oldent, const T3 & newplug, const T4 & newent)
	{
		return replace(oldent, entity(newplug, newent));
	}

	template<class T1, class T2, class T3, class T4>
	uint32 replace(const T1 & oldplug, const T2 & oldent, const T3 & newplug, const T4 & newent)
	{
		return replace(entity(oldplug, oldent), entity(newplug, newent));
	}

	template<class T1, class T2>
	bool remove(const T1 & plug_, const T2 & res_, uint32 tformid)
	{
		return remove(entity(plug_, res_), tformid);
	}

	bool remove(fvec3 & pWorldPos);

	bool remove(nsentity * ent);

	template<class T1, class T2>
	bool remove(const T1 & plug_, const T2 & res_)
	{
		return remove(entity(plug_, res_));
	}

	bool remove(nsentity * entity, uint32 tformid);

	void set_bg_color(const fvec3 & pColor);

	void set_creator(const nsstring & pCreator);

	const nsstring & creator() const;

	void set_camera(nsentity * cam, bool addToSceneIfNeeded = true);

	template<class T1, class T2>
	void set_camera(const T1 & plug_, const T2 & camid, bool addToSceneIfNeeded = true)
	{
		set_camera(nse.resource<nsentity>(plug_, camid), addToSceneIfNeeded);
	}

	void set_max_players(uint32 pMaxPlayers);

	void set_notes(const nsstring & pNotes);

	template<class T1, class T2>
	void set_skydome(const T1 & plug_, const T2 & skyid, bool addToSceneIfNeeded = true)
	{
		set_skydome(nse.resource<nsentity>(plug_, skyid), addToSceneIfNeeded);
	}

	void set_skydome(nsentity * skydome, bool addToSceneIfNeeded = true);

	void update_comp_maps(uint32 plugid, uint32 entid);

	uivec2_vector & unloaded();

private:

	uivec2 m_camera_id;
	uivec2 m_skydome_id;
	uint32 m_max_players;
	fvec3 m_bg_color;
	entities_by_comp m_ents_by_comp_type;
	nsstring m_notes;
	nsstring m_creator;
	nstile_grid * m_tile_grid;
	bool m_show_bit;

	uivec2_vector m_unloaded;
	entity_ptr_set m_dummyret;
};


template<class PUPer>
void pup(PUPer & p, nsscene & sc)
{
	pup(p, sc.m_camera_id, "camera_id");
	pup(p, sc.m_skydome_id, "skydome_id");
	pup(p, sc.m_max_players, "max_players");
	pup(p, sc.m_bg_color, "bg_color");
	pup(p, sc.m_notes, "notes");
	pup(p, sc.m_creator, "creator");
	pup(p, sc.m_show_bit, "show_bit");

	sc.unloaded().clear();
	uivec2_vector entids;


	auto iter = sc.entities().begin();
	while (iter != sc.entities().end())
	{
		entids.push_back(uivec2((*iter)->plugin_id(), (*iter)->id()));
		++iter;
	}

	pup(p, entids, "entids");

	for (uint32 i = 0; i < entids.size(); ++i)
	{
		nsentity * ent = nse.resource<nsentity>(entids[i]);

		if (ent == NULL)
		{
			sc.unloaded().push_back(entids[i]);
			continue;
		}

		nstform_comp * tc = ent->get<nstform_comp>();
		if (tc == NULL)
			tc = ent->create<nstform_comp>();
		tc->pup(&p);
		if (p.mode() == PUP_IN)
		{
			nsoccupy_comp * oc = ent->get<nsoccupy_comp>();
			if (oc != NULL)
			{
				for (uint32 i = 0; i < tc->count(); ++i)
					sc.grid().add(uivec3(ent->plugin_id(), ent->id(), i), oc->spaces(), tc->wpos(i));
			}
		}
	}
}

#endif
