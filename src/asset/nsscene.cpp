/*! 
	\file nsscene.cpp
	
	\brief Definition file for nsscene class

	This file contains all of the neccessary definitions for the nsscene class.

	\author Daniel Randle
	\date November 23 2013
	\copywrite Earth Banana Games 2013
*/

#include <system/nsselection_system.h>
#include <component/nstile_brush_comp.h>
#include <asset/nsscene.h>
#include <asset/nsscene_manager.h>
#include <asset/nsentity.h>
#include <component/nsrender_comp.h>
#include <component/nsanim_comp.h>
#include <component/nstform_comp.h>
#include <nstile_grid.h>
#include <system/nstform_system.h>
#include <component/nscam_comp.h>
#include <asset/nsasset_manager.h>
#include <component/nssel_comp.h>
#include <nsengine.h>
#include <asset/nsentity_manager.h>
#include <component/nsoccupy_comp.h>
#include <asset/nsplugin_manager.h>
#include <component/nslight_comp.h>
#include <nscube_grid.h>
#include <component/nssprite_comp.h>

nsscene::nsscene():
	nsasset(type_to_hash(nsscene)),
	m_skydome(),
	m_max_players(0),
	m_bg_color(),
	m_notes(),
	m_creator(),
	m_enabled(false),
	m_tile_grid(new nstile_grid()),
	cube_grid(new nscube_grid()),
	m_ents_by_comp(),
	m_unloaded_tforms()
{
	set_ext(DEFAULT_SCENE_EXTENSION);
}


nsscene::nsscene(const nsscene & copy_):
	nsasset(copy_),
	m_skydome(copy_.m_skydome),
	m_max_players(copy_.m_max_players),
	m_bg_color(copy_.m_bg_color),
	m_notes(copy_.m_notes),
	m_creator(copy_.m_creator),
	m_enabled(copy_.m_enabled),
	m_tile_grid(new nstile_grid()),
	cube_grid(new nscube_grid()),
	m_ents_by_comp(),
	m_unloaded_tforms()
{}

nsscene::~nsscene()
{
    if (m_enabled)
        enable(false);
	delete m_tile_grid;
	delete cube_grid;
}

nsscene & nsscene::operator=(nsscene rhs)
{
	nsasset::operator=(rhs);
	std::swap(m_skydome,rhs.m_skydome);
	std::swap(m_max_players,rhs.m_max_players);
	std::swap(m_bg_color,rhs.m_bg_color);
	std::swap(m_notes,rhs.m_notes);
	std::swap(m_router, rhs.m_router);
	std::swap(m_creator,rhs.m_creator);
	std::swap(m_tile_grid, rhs.m_tile_grid);
	std::swap(m_ents_by_comp,rhs.m_ents_by_comp);
	return *this;
}

void nsscene::clear()
{
	if (!m_enabled)
	{
		dprint("nsscene::clear Cannot clear disabled scene " + m_name);
		return;
	}
	
	auto ents = entities_in_scene();
	if (ents == nullptr)
		return;

	while (ents->begin() != ents->end())
		remove(*(ents->begin()), true);
	
	m_unloaded_tforms.clear();
}


nstform_comp * nsscene::add(
	nsentity * ent,
	const tform_info & tf_info,
	bool preserve_world_tform)
{
	if (!m_enabled)
	{
		dprint("nsscene::add cannot add entity to disabled scene " + m_name);
		return nullptr;
	}

	if (ent == nullptr)
	{
		dprint("nsscene::add unable to add null entity to scene");
		return nullptr;
	}

	nstform_comp * tf_comp = ent->get<nstform_comp>();
	nsoccupy_comp * occ_comp = ent->get<nsoccupy_comp>();
	
	// If there is no tranform component, then make one
	if (tf_comp != nullptr)
	{
		dprint("nsscene::add entity " + ent->name() + " already has a tform comp cannot add to scene " + m_name);
		return nullptr;
	}
	
	tf_comp = ent->create<nstform_comp>();
	tf_comp->m_scene = this;
	tf_comp->set_tf_info(tf_info, preserve_world_tform);
	
	if (occ_comp != nullptr)
	{
		if (!m_tile_grid->add(ent->full_id(), occ_comp->spaces(), tf_comp->world_position()))
		{
			ent->destroy<nstform_comp>();
			return nullptr;
		}
	}

	_add_all_comp_entries(ent);
	sig_connect(ent->component_added, nsscene::_on_comp_add);
	sig_connect(ent->component_removed, nsscene::_on_comp_remove);
	nse.event_dispatch()->push<scene_ent_added>(ent->full_id(),full_id());
	tf_comp->post_update(true);
	return tf_comp;
}

nstform_comp * nsscene::add(
	nsentity * ent,
	const fvec3 & pos,
	const fquat & orient,
	const fvec3 & scale)
{
	tform_info tfi(uivec2(0), pos, orient, scale);
	return add(ent, tfi, false);
}

void nsscene::change_max_players(int32 pAmount)
{
	if ((m_max_players + pAmount) > SCENE_MAX_PLAYERS || (m_max_players + pAmount) < 2)
	{
		dprint("nsscene::changeMaxPlayers Players must be in max player range");
		return;
	}

	m_max_players += pAmount;
}

nsentity * nsscene::find_entity(const uivec2 & id) const
{
	auto ents = entities_in_scene();
	if (ents == nullptr)
		return nullptr;
	auto iter = ents->begin();
	while (iter != ents->end())
	{
		if ((*iter)->full_id() == id)
			return *iter;
		++iter;
	}
	return nullptr;
}

nsentity * nsscene::find_entity(uint32 plug_id, uint32 res_id) const
{
	return find_entity(uivec2(plug_id,res_id));
}

bool nsscene::has_dir_light() const
{
	auto ents = entities_in_scene();
	if (ents == nullptr)
		return false;
	
	auto iter = ents->begin();
	while (iter != ents->end())
	{
		nslight_comp * lc = (*iter)->get<nslight_comp>();
		if (lc != nullptr)
		{
			if (lc->get_light_type() == nslight_comp::l_dir)
				return true;
		}
		++iter;
	}
	return false;
}

const fvec4 & nsscene::bg_color() const
{
	return m_bg_color;
}

const nsstring & nsscene::creator() const
{
	return m_creator;
}

void nsscene::pup(nsfile_pupper * p)
{
	if (p->type() == nsfile_pupper::pup_binary)
	{
		nsbinary_file_pupper * bf = static_cast<nsbinary_file_pupper *>(p);
		::pup(*bf, *this);
	}
	else
	{
		nstext_file_pupper * tf = static_cast<nstext_file_pupper *>(p);
		::pup(*tf, *this);
	}
}

uint32 nsscene::max_players() const
{
	return m_max_players;
}

uivec2 nsscene::ref_id(const fvec3 & pWorldPos) const
{
	return m_tile_grid->get(pWorldPos);
}

nsentity * nsscene::skydome() const
{
	return find_entity(m_skydome);
}

/*!
Get the other resources that this Scene uses. This is given by all the Entities that currently exist in the scene.
*/
uivec3_vector nsscene::resources()
{
	auto ents = entities_in_scene();
	uivec3_vector ret;

	if (ents == nullptr)
		return ret;

	auto iter = ents->begin();
	while (iter != ents->end())
	{
		uivec3_vector tmp = (*iter)->resources();
		ret.insert(ret.end(), tmp.begin(), tmp.end());
		++iter;
	}
	return ret;
}

const nsstring & nsscene::notes() const
{
	return m_notes;
}

nstile_grid & nsscene::grid()
{
	return *m_tile_grid;
}

const entity_set * nsscene::entities_with_comp(uint32 comp_type_id) const
{
	auto fiter = m_ents_by_comp.find(comp_type_id);
	if (fiter != m_ents_by_comp.end())
		return &fiter->second;
	return nullptr;
}

entity_set * nsscene::entities_with_comp(uint32 comp_type_id)
{
	auto fiter = m_ents_by_comp.find(comp_type_id);
	if (fiter != m_ents_by_comp.end())
		return &fiter->second;
	return nullptr;		
}

const entity_set * nsscene::entities_in_scene() const
{
	return entities_with_comp<nstform_comp>();
}

entity_set * nsscene::entities_in_scene()
{
	return entities_with_comp<nstform_comp>();		
}

void nsscene::hide_layer(int32 pLayer, bool pHide)
{
	nstile_grid::grid_bounds g = m_tile_grid->occupied_bounds();
	for (int32 y = g.min_space.y; y <= g.max_space.y; ++y)
	{
		for (int32 x = g.min_space.x; x <= g.max_space.x; ++x)
		{
			uivec2 id = m_tile_grid->get(ivec3(x, y, -pLayer));
			if (id != uivec2())
			{
				nsentity * ent = find_entity(id.x, id.y);
				if (ent != nullptr)
				{
					nstform_comp * tc = ent->get<nstform_comp>();
					if (pHide)
						tc->set_hidden_state(tc->hidden_state() | nstform_comp::hide_layer);
					else
						tc->set_hidden_state(tc->hidden_state() & ~nstform_comp::hide_layer);
				}
			}
		}
	}
}

void nsscene::hide_layers_above(int32 pBaseLayer, bool pHide)
{
	nstile_grid::grid_bounds g = m_tile_grid->occupied_bounds();
	pBaseLayer *= -1;

	for (int32 z = pBaseLayer-1; z >= g.min_space.z; --z)
	{
		for (int32 y = g.min_space.y; y <= g.max_space.y; ++y)
		{
			for (int32 x = g.min_space.x; x <= g.max_space.x; ++x)
			{
				uivec2 id = m_tile_grid->get(ivec3(x, y, z));
				if (id != uivec2())
				{
					nsentity * ent = find_entity(id.x, id.y);
					if (ent != nullptr)
					{
						nstform_comp * tc = ent->get<nstform_comp>();
						if (pHide)
							tc->set_hidden_state(tc->hidden_state() | nstform_comp::hide_layer);
						else
							tc->set_hidden_state(tc->hidden_state() & ~nstform_comp::hide_layer);
					}
				}
			}
		}
	}

	if (pHide)
	{
		for (int32 y = g.min_space.y; y <= g.max_space.y; ++y)
		{
			for (int32 x = g.min_space.x; x <= g.max_space.x; ++x)
			{
				uivec2 id = m_tile_grid->get(ivec3(x, y, pBaseLayer));
				if (id != uivec2())
				{
					nsentity * ent = find_entity(id.x, id.y);
					if (ent != nullptr)
					{
						nstform_comp * tc = ent->get<nstform_comp>();
						tc->set_hidden_state(tc->hidden_state() & ~nstform_comp::hide_layer);
					}
				}
			}
		}
	}
}

void nsscene::hide_layers_below(int32 pTopLayer, bool pHide)
{
	nstile_grid::grid_bounds g = m_tile_grid->occupied_bounds();
	pTopLayer *= -1;

	for (int32 z = pTopLayer+1; z <= g.max_space.z; ++z)
	{
		for (int32 y = g.min_space.y; y <= g.max_space.y; ++y)
		{
			for (int32 x = g.min_space.x; x <= g.max_space.x; ++x)
			{
				uivec2 id = m_tile_grid->get(ivec3(x, y, z));
				if (id != uivec2())
				{
					nsentity * ent = find_entity(id.x, id.y);
					if (ent != nullptr)
					{
						nstform_comp * tc = ent->get<nstform_comp>();
						if (pHide)
							tc->set_hidden_state(tc->hidden_state() | nstform_comp::hide_layer);
						else
							tc->set_hidden_state(tc->hidden_state() & ~nstform_comp::hide_layer);					}
				}
			}
		}
	}

	if (pHide)
	{
		for (int32 y = g.min_space.y; y <= g.max_space.y; ++y)
		{
			for (int32 x = g.min_space.x; x <= g.max_space.x; ++x)
			{
				uivec2 id = m_tile_grid->get(ivec3(x, y, pTopLayer));
				if (id != uivec2())
				{
					nsentity * ent = find_entity(id.x, id.y);
					if (ent != nullptr)
					{
						nstform_comp * tc = ent->get<nstform_comp>();
						tc->set_hidden_state(tc->hidden_state() & ~nstform_comp::hide_layer);
					}
				}
			}
		}
	}
}

void nsscene::init()
{
}

void nsscene::name_change(const uivec2 & oldid, const uivec2 newid)
{
	m_tile_grid->name_change(oldid, newid);
}

bool nsscene::remove(nsentity * ent, bool remove_children)
{
	if (!m_enabled)
	{
		dprint("nsscene::remove Cannot remove entity in disabled scene " + m_name);
		return false;
	}

	nstform_comp * tf_comp = ent->get<nstform_comp>();
	nstform_comp * parent = tf_comp->parent();

	for (uint32 i = 0; i < tf_comp->child_count(); ++i)
	{
		nstform_comp * chld = tf_comp->child(i);
		if (chld != nullptr)
		{
			if (remove_children)
				remove(chld->owner(), remove_children);
			else
				chld->set_parent(parent, true);
		}
	}

	tf_comp->set_parent(nullptr, true); // remove itform from parent

	fvec3 pos = tf_comp->world_position();	
	nsoccupy_comp * occ_comp = ent->get<nsoccupy_comp>();
	if (occ_comp != nullptr)
		m_tile_grid->remove(occ_comp->spaces(), pos);

	sig_disconnect(tf_comp->owner()->component_added);
	sig_disconnect(tf_comp->owner()->component_removed);
	_remove_all_comp_entries(tf_comp->owner());

	ent->destroy<nstform_comp>();
	nse.event_dispatch()->push<scene_ent_removed>(ent->full_id(),full_id());

	if (m_skydome == ent->full_id())
		m_skydome = uivec2(0);

	return true;
}

bool nsscene::remove(fvec3 & pWorldPos, bool remove_children)
{
	nsentity * ent = find_entity(m_tile_grid->get(pWorldPos));
	if (ent == nullptr)
		return false;
	return remove(ent, remove_children);
}

void nsscene::set_bg_color(const fvec4 & bg_color)
{
    m_bg_color = bg_color;
}

void nsscene::set_creator(const nsstring & pCreator)
{
	m_creator = pCreator;
}

void nsscene::set_max_players(uint32 pMaxPlayers)
{
	if (pMaxPlayers > SCENE_MAX_PLAYERS || pMaxPlayers < 2)
	{
		dprint("nsscene::set_max_players players must be in max player range");
		return;
	}
	m_max_players = pMaxPlayers;
}

void nsscene::set_notes(const nsstring & notes_)
{
	m_notes = notes_;
}

void nsscene::set_skydome(nsentity * skydome, bool add_to_scene_if_needed)
{
	if (skydome == nullptr)
		return;

	nstform_comp * skytf = skydome->get<nstform_comp>();

	if (skytf != nullptr)
	{
		m_skydome = skydome->full_id();
		dprint("nsscene::set_skydome - Map \"" + m_name + "\"'s skydome set to \"" + skydome->name() + "\"");
	}
	else
	{
		if (add_to_scene_if_needed)
		{
			if (add(skydome) != nullptr)
				set_skydome(skydome, false);
		}
	}
}

bool nsscene::is_enabled()
{
	return m_enabled;
}

void nsscene::make_ent_instanced_if_needed(nsentity * ent)
{
	entity_set * ents_with_rcomp = entities_with_comp<nsrender_comp>();
	auto fiter = ents_with_rcomp->begin();
	nsanim_comp * acomp = ent->get<nsanim_comp>();
	nssprite_sheet_comp * scomp = ent->get<nssprite_sheet_comp>();
	nstform_comp * tcomp = ent->get<nstform_comp>();
	nsrender_comp * rcomp = ent->get<nsrender_comp>();	
	while (fiter != ents_with_rcomp->end())
	{
		if ((*fiter) != ent)
		{
			nstform_comp * ent_tcomp = (*fiter)->get<nstform_comp>();
			nsrender_comp * ent_rcomp = (*fiter)->get<nsrender_comp>();
			nsanim_comp * ent_acomp = (*fiter)->get<nsanim_comp>();
			nssprite_sheet_comp * ent_scomp = (*fiter)->get<nssprite_sheet_comp>();

			// The render comps must be equivalent (see operator ==) and the anim/sprite comps must have same address
			if (ent_rcomp != nullptr && (*rcomp) == (*ent_rcomp) && acomp == ent_acomp && scomp == ent_scomp)
			{
				if (ent_tcomp->inst_obj != nullptr)
				{
					tcomp->inst_obj = ent_tcomp->inst_obj;
					tcomp->inst_obj->shared_geom_tforms.push_back(tcomp);
					tcomp->inst_obj->needs_update = true;
				}
				else
				{
					tform_per_scene_info * psi = new tform_per_scene_info;
					nse.video_driver()->current_context()->instance_objs.push_back(psi);
					tcomp->inst_obj = psi;
					ent_tcomp->inst_obj = psi;
					psi->shared_geom_tforms.push_back(ent_tcomp);
					psi->shared_geom_tforms.push_back(tcomp);
					psi->needs_update = true;
				}
#ifdef NSDEBUG
				nsstringstream ss;
				ss << "nsscene_make_ent_instanced_if_needed making " + ent->name() + " instanced along with the following ents: \n";
				for (uint32 i = 0; i < tcomp->inst_obj->shared_geom_tforms.size()-1; ++i)
					ss << tcomp->inst_obj->shared_geom_tforms[i]->owner()->name() << "\n";
				dprint(ss.str());
#endif
				return;
			}
		}
		++fiter;
	}
}

void nsscene::make_ent_not_instanced(nsentity * ent)
{
	nstform_comp * tfc = ent->get<nstform_comp>();
	if (tfc == nullptr)
	{
		dprint("nsscene::make_ent_not_instanced render component being removed from scene after tform comp");
		return;
	}

	if (tfc->inst_obj == nullptr)
	{
		dprint("nsscene::make_ent_not_instanced no need to remove instanced obj - not instanced");
		return;		
	}
	
	auto iter = tfc->inst_obj->shared_geom_tforms.begin();
	while (iter != tfc->inst_obj->shared_geom_tforms.end())
	{
		if (tfc == *iter)
		{
			iter = (*iter)->inst_obj->shared_geom_tforms.erase(iter);
			dprint("nsscene_make_ent_not_instanced making " + ent->name() + " no longer instanced");

#ifdef NSDEBUG
			nsstringstream ss;
			ss << "the scene now contains the following instanced ents\n";
			for (uint32 i = 0; i < tfc->inst_obj->shared_geom_tforms.size(); ++i)
				ss << tfc->inst_obj->shared_geom_tforms[i]->owner()->name() << "\n";
			dprint(ss.str());
#endif
			tfc->inst_obj = nullptr;

			// If there is only one entity left - the auto update code should remove that and delete
			// the obj
			return;
		}
		else
		{
			++iter;
		}
	}
	
}

// This adds the component from the comp by type list in the scene
void nsscene::_on_comp_add(nscomponent * comp_t)
{
	auto fiter = m_ents_by_comp.emplace(comp_t->type(), std::unordered_set<nsentity*>());
	fiter.first->second.emplace(comp_t->owner());
	dprint("nsscene::_on_comp_add added component " + hash_to_guid(comp_t->type()) + " in entity " + comp_t->owner()->name() + " to scene " + m_name);
	if (comp_t->type() == type_to_hash(nsrender_comp))
	{
		dprint("nscene::_on_comp_add analyzing render component...");
		make_ent_instanced_if_needed(comp_t->owner());
	}
	if (comp_t->type() == type_to_hash(nsanim_comp) || comp_t->type() == type_to_hash(nssprite_sheet_comp))
	{
		make_ent_not_instanced(comp_t->owner());
		make_ent_instanced_if_needed(comp_t->owner());
	}
}

// This removes the component from the comp by type list in the scene
void nsscene::_on_comp_remove(nscomponent * comp_t)
{
	auto fiter = m_ents_by_comp.find(comp_t->type());
	fiter->second.erase(comp_t->owner());
	dprint("nsscene::_on_comp_remove removed component " + hash_to_guid(comp_t->type()) + " in entity " + comp_t->owner()->name() + " from scene " + m_name);
	if (comp_t->type() == type_to_hash(nssel_comp))
	{
		dprint("nsscene::_on_comp_remove making sure entity is de-selected");
		nse.system<nsselection_system>()->remove_from_selection(comp_t->owner());
	}
	if (comp_t->type() == type_to_hash(nsrender_comp))
	{
		dprint("nscene::_on_comp_remove analyzing render component...");
		make_ent_not_instanced(comp_t->owner());
	}
	if (comp_t->type() == type_to_hash(nsanim_comp)
		|| comp_t->type() == type_to_hash(nssprite_sheet_comp))
	{
		make_ent_not_instanced(comp_t->owner());
		make_ent_instanced_if_needed(comp_t->owner());
	}
}

void nsscene::_remove_all_comp_entries(nsentity * ent)
{
	auto citer = ent->begin();
	while (citer != ent->end())
	{
		_on_comp_remove(citer->second);
		++citer;
	}
}

void nsscene::_add_all_comp_entries(nsentity * ent)
{
	auto citer = ent->begin();
	while (citer != ent->end())
	{
		_on_comp_add(citer->second);
		++citer;
	}	
}

void nsscene::enable(bool to_enable)
{
	if (to_enable)
	{
		if (m_enabled)
			return;

		m_tile_grid->init();
        m_enabled = true;
		
		// If enabling the scene, go through the pupped vec and load - clear the pupped vec after loading
		// Any unloaded tforms are added to the "unloaded" pupped vec
		
		auto add_iter = m_pupped_tforms.begin();
		while (add_iter != m_pupped_tforms.end())
		{
			nsentity * ent = get_asset<nsentity>(add_iter->ent_id);
			tform_info tf_info(
				uivec2(),
				add_iter->tf_info.m_position,
				add_iter->tf_info.m_orient,
				add_iter->tf_info.m_scaling,
				add_iter->tf_info.m_hidden_state);
			nstform_comp * tfc = add(ent,tf_info,false);

			if (tfc == nullptr)
			{
				m_unloaded_tforms.emplace_back(*add_iter);
				add_iter = m_pupped_tforms.erase(add_iter);
				dprint("nsscene::enable - could not load tform - unable to find entity id " + add_iter->ent_id.to_string());
			}
			else
			{
				++add_iter;
			}
		}

		// After having added all tforms, preserving the ids, we should be able to assign parent/children
		// accordingly now
		auto piter = m_pupped_tforms.begin();
		while (piter != m_pupped_tforms.end())
		{
			nsentity * ent = get_asset<nsentity>(piter->ent_id);
			nstform_comp * tfc = ent->get<nstform_comp>();
			tfc->set_tf_info(piter->tf_info, false);
			++piter;
		}

		m_pupped_tforms.clear();
	}
	else
	{
		if (!m_enabled)
			return;
		
		// If disabling, fill the pupped vec and then remove everything from the scene
		m_pupped_tforms.clear();
		_populate_pup_vec();
		clear();
		m_tile_grid->release();
		m_enabled = false;
	}
}


void nsscene::_populate_pup_vec()
{
	auto ents = entities_in_scene();
	auto ent_iter = ents->begin();
	while (ent_iter != ents->end())
	{
        nstform_comp * tfc = (*ent_iter)->get<nstform_comp>();
		if (tfc->save_with_scene)
			m_pupped_tforms.emplace_back(tfc->tf_info());
		++ent_iter;
	}
}


