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
	m_skydome(nullptr),
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
{
	auto ents = copy_.entities_in_scene();
	if (ents != nullptr)
	{
		auto iter = ents->begin();
		while (iter != ents->end())
		{
			tform_per_scene_info * psi = (*iter)->get<nstform_comp>()->per_scene_info(&copy_);
			for (uint32 i = 0; i < psi->m_tforms.size(); ++i)
			{
				instance_tform & itf = psi->m_tforms[i];
				if (itf.parent() == nullptr) // if root scene node
					add(*iter, nullptr, itf, true); // add children too
			}
			++iter;
		}
	}
}

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
	
	m_unloaded_tforms.pv.clear();
}


nstform_comp * nsscene::add(
	nsentity * ent,
	const tform_info & tf_info,
	bool add_children_recursively)
{
	if (!m_enabled)
	{
		dprint("nsscene::add cannot add entity to disabled scene " + m_name);
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
	
	tf_comp->set_tf_info(tf_info);
	
	tform_per_scene_info & pse = *fiter->second;
	pse.video_context_init();
	pse.scene = this;
	pse.owner = tf_comp;
	uint32 tfid = pse.m_tforms.size();
	pse.m_tforms.resize(tfid + 1);
	instance_tform & tf = pse.m_tforms[tfid];
	tf = tf_info;
    tf.m_owner = &pse;
	tf.set_parent(parent, true);

	// If there is an occupy component, attemp at inserting it in to the map. It will fail if the space
	// is already occupied therefore we need to remove the tranform component and set the entities scene
	// ID to 0 again if the add fails, since that means no other instances of the entity are in the scene
	if (occ_comp != nullptr)
	{
		// Get the transform ID that would result if we inserted it in to the scene
		// We don't want to insert it yet because we first want to check if the space is open
		uint32 pID = pse.m_tforms.size();
		if (!m_tile_grid->add(uivec3(ent->full_id(), pID), occ_comp->spaces(), tf_info.world_position()))
		{
			tf.set_parent(nullptr, true);
			pse.m_tforms.resize(tfid);

			if (pse.m_tforms.empty())
				tf_comp->m_scenes_info.erase(fiter);

			if (tf_comp->m_scenes_info.empty())
				ent->destroy<nstform_comp>();
			
			return -1;
		}
	}

	if (add_children_recursively)
	{
		for (uint32 i = 0; i < tf_info.m_children.size(); ++i)
		{
			instance_handle ihnd = tf_info.m_children[i];
			instance_tform & chld = ihnd.tfc->m_tforms[ihnd.ind];
			
			if (add(chld.owner()->owner->owner(), &tf, chld, add_children_recursively) == -1)
			{
				dprint("nsscene::add Could not add child of " + ent->name() + " tformid " + std::to_string(tfid) + " because occupy_comp did not allow it");
			}
		}
	}

	nse.event_dispatch()->push<scene_tform_added>(uivec3(ent->full_id(),tfid),full_id());

	if (make_tform)
	{
		_add_all_comp_entries(ent);
		sig_connect(ent->component_added, nsscene::_on_comp_add);
		sig_connect(ent->component_removed, nsscene::_on_comp_remove);
		nse.event_dispatch()->push<scene_ent_added>(ent->full_id(),full_id());
	}
	
	tf_comp->post_update(true);
	pse.m_buffer_resized = true;
	return tfid;
}

// void nsscene::add_gridded(
// 	nsentity * ent,
// 	const ivec3 & bounds,
// 	const fvec3 & starting_pos,
// 	const fquat & orient_,
// 	const fvec3 & scaling_
// 	)
// {
// 	if (!m_enabled)
// 	{
// 		dprint("nsscene::add_gridded Cannot add entity to disabled scene " + m_name);
// 		return;
// 	}

// 	nstform_comp * tf_comp = ent->get<nstform_comp>();
//     bool added_tform = false;
// 	if (tf_comp == nullptr)
//     {
// 		tf_comp = ent->create<nstform_comp>();
//         added_tform = true;
//     }

// 	// Create a new per scene tform entry
// 	auto fiter = tf_comp->m_scenes_info.find(this);
//     if (fiter == tf_comp->m_scenes_info.end())
//     {
//         fiter = tf_comp->m_scenes_info.emplace(
//                     this,
//                     new tform_per_scene_info()).first;
//     }
// 	tform_per_scene_info & pse = *fiter->second;
// 	pse.video_context_init();
// 	pse.owner = tf_comp;
// 	pse.scene = this;
// 	// Figure out the total number of transforms needed and allocate that 
// 	// much memory (addTransforms does this)
// 	uint32 addSize = bounds.x * bounds.y * bounds.z;
// 	pse.m_tforms.reserve(pse.m_tforms.size() + addSize);
// 	instance_tform itf;
// 	for (int32 z = 0; z < bounds.z; ++z)
// 	{
// 		for (int32 y = 0; y < bounds.y; ++y)
// 		{
// 			for (int32 x = 0; x < bounds.x; ++x)
// 			{
// 				float xP = X_GRID * x * 2.0f;
// 				float yP = Y_GRID * y;
// 				float zP = Z_GRID * z;
// 				if (y % 2 != 0)
// 					xP += X_GRID;
// 				fvec3 pos(xP, yP, zP);
// 				pos += starting_pos;

// 				itf.m_position = pos;
// 				itf.m_orient = orient_;
// 				itf.m_scaling = scaling_;
// 				itf.snap_to_grid = true;
// 				if (add(ent, nullptr, itf, false) == -1)
// 				{
// 					dprint("nsscene::add_gridded Could not add grid position " + ivec3(x,y,z).to_string() + " to entity " + ent->name() + " because occupy component would not allow it");
// 				}
// 			}
// 		}
// 	}

//     if (added_tform)
//     {
//         _add_all_comp_entries(ent);
//         sig_connect(ent->component_added, nsscene::_on_comp_add);
//         sig_connect(ent->component_removed, nsscene::_on_comp_remove);
//     }
// }

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

uivec3 nsscene::ref_id(const fvec3 & pWorldPos) const
{
	return m_tile_grid->get(pWorldPos);
}

nsentity * nsscene::skydome() const
{
	return m_skydome;
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
			uivec3 id = m_tile_grid->get(ivec3(x, y, -pLayer));
			if (id != uivec3())
			{
				nsentity * ent = find_entity(id.x, id.y);
				if (ent != nullptr)
				{
					nstform_comp * tc = ent->get<nstform_comp>();
					instance_tform * itf = &tc->per_scene_info(this)->m_tforms[id.z];
					if (pHide)
						itf->m_hidden_state |= nstform_comp::hide_layer;
					else
						itf->m_hidden_state &= ~nstform_comp::hide_layer;
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
				uivec3 id = m_tile_grid->get(ivec3(x, y, z));
				if (id != uivec3())
				{
					nsentity * ent = find_entity(id.x, id.y);
					if (ent != nullptr)
					{
						nstform_comp * tc = ent->get<nstform_comp>();
						instance_tform * itf = &tc->per_scene_info(this)->m_tforms[id.z];
						if (pHide)
							itf->m_hidden_state |= nstform_comp::hide_layer;
						else
							itf->m_hidden_state &= ~nstform_comp::hide_layer;
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
				uivec3 id = m_tile_grid->get(ivec3(x, y, pBaseLayer));
				if (id != uivec3())
				{
					nsentity * ent = find_entity(id.x, id.y);
					if (ent != nullptr)
					{
						nstform_comp * tc = ent->get<nstform_comp>();
						instance_tform * itf = &tc->per_scene_info(this)->m_tforms[id.z];
						itf->m_hidden_state &= ~nstform_comp::hide_layer;
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
				uivec3 id = m_tile_grid->get(ivec3(x, y, z));
				if (id != uivec3())
				{
					nsentity * ent = find_entity(id.x, id.y);
					if (ent != nullptr)
					{
						nstform_comp * tc = ent->get<nstform_comp>();
						instance_tform * itf = &tc->per_scene_info(this)->m_tforms[id.z];
						if (pHide)
							itf->m_hidden_state |= nstform_comp::hide_layer;
						else
							itf->m_hidden_state &= ~nstform_comp::hide_layer;
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
				uivec3 id = m_tile_grid->get(ivec3(x, y, pTopLayer));
				if (id != uivec3())
				{
					nsentity * ent = find_entity(id.x, id.y);
					if (ent != nullptr)
					{
						nstform_comp * tc = ent->get<nstform_comp>();
						instance_tform * itf = &tc->per_scene_info(this)->m_tforms[id.z];
						itf->m_hidden_state &= ~nstform_comp::hide_layer;
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

bool nsscene::remove(instance_tform * itform, bool remove_children)
{
	if (!m_enabled)
	{
		dprint("nsscene::remove Cannot remove entity in disabled scene " + m_name);
		return false;
	}

	nstform_comp * tf_comp = itform->owner()->owner;
	nsentity * entity = tf_comp->owner();
	
	auto fiter = tf_comp->m_scenes_info.find(this);
	if (fiter == tf_comp->m_scenes_info.end())
	{
		dprint("nsscene::remove entity " + entity->name() + " not found in scene");
		return false;
	}

	if (remove_children)
	{
		for (uint32 i = 0; i < itform->child_count(); ++i)
			remove(itform->child(i), remove_children);
	}

	tform_per_scene_info * pse = fiter->second; // easier to use pse
	
	// reassign all itform's children's parent to itform's parent
	while (itform->child_count() != 0)
		itform->child(0)->set_parent(itform->parent(), true);

	itform->set_parent(nullptr, true); // remove itform from parent

	fvec3 pos = itform->world_position();	
	
	if (pse->m_tforms.size() == 1)
		pse->m_tforms.pop_back();
	else
	{
		*itform = pse->m_tforms.back();

		pse->m_tforms.pop_back();
		itform->update = true;
		pse->m_buffer_resized = true;
		itform->owner()->owner->post_update(true);
	}

    uint32 old_index = pse->m_tforms.size();
    uint32 tformid = itform->current_tform_id();

    
	if (itform->m_parent.is_valid())
	{
		instance_tform * par_itf = itform->parent();
		if (tformid != -1) // Replace parent's child id with our new one if the new tformid is valid
		{
			for (uint32 i = 0; i < par_itf->m_children.size(); ++i)
			{
				if (par_itf->m_children[i] == instance_handle(itform->owner(), old_index))
					par_itf->m_children[i].ind = tformid;
			}
		}
		else // otherwise remove us from our parent's children
		{
			par_itf->remove_child(itform, false);
		}
    }

    // Replace childrens' parent ids with our new one
    for (uint32 i = 0; i < itform->m_children.size(); ++i)
    {
		instance_tform * chld = &itform->m_children[i].tfc->m_tforms[itform->m_children[i].ind];
		if (tformid != -1)
		{
			chld->m_parent.ind = tformid;
		}
		else
		{
			chld->set_parent(nullptr, true);
		}
    }

	nssel_comp * sel_cmp = entity->get<nssel_comp>();
	if (sel_cmp != nullptr)
	{
		auto selection = sel_cmp->selection(this);
		
		// Since remove replaces the tform at tformid with a copy of the last tform and then pops
		// the back, we need to replace the index appropriately in the selection component
		if (selection->erase(pse->m_tforms.size()) != 1)
			selection->erase(tformid);
	}
	
	nsoccupy_comp * occ_comp = entity->get<nsoccupy_comp>();
	if (occ_comp != nullptr)
	{
		m_tile_grid->remove(occ_comp->spaces(), pos);

		// This should take care of updating the reference ID for the space that got switched..
		if (tformid != -1)
		{
			fvec3 newPos = pse->m_tforms[tformid].world_position();
			m_tile_grid->remove(occ_comp->spaces(), newPos);
			m_tile_grid->add(uivec3(entity->full_id(), tformid), occ_comp->spaces(), newPos);
		}
	}

	nse.event_dispatch()->push<scene_tform_removed>(uivec3(entity->full_id(),tformid),full_id());
	
	if (tformid == -1)
	{
		sig_disconnect(tf_comp->owner()->component_added);
		sig_disconnect(tf_comp->owner()->component_removed);
		_remove_all_comp_entries(tf_comp->owner());

		delete pse;
		tf_comp->m_scenes_info.erase(fiter);		
		if (tf_comp->m_scenes_info.empty())
			entity->destroy<nstform_comp>();

		nse.event_dispatch()->push<scene_ent_removed>(entity->full_id(),full_id());
		
		if (m_skydome == entity)
			m_skydome = nullptr;
	}
	return true;
}

bool nsscene::remove(nsentity * entity, uint32 tformid, bool remove_children)
{
	nstform_comp * tf_comp = entity->get<nstform_comp>();
	if (tf_comp == nullptr)
		return false;
	return remove(&tf_comp->per_scene_info(this)->m_tforms[tformid], remove_children);
}

bool nsscene::remove(fvec3 & pWorldPos, bool remove_children)
{
	uivec3 refid = m_tile_grid->get(pWorldPos);
	if (refid == uivec3(0))
		return false;

	return remove(get_asset<nsentity>(refid.xy()), refid.z, remove_children);
}

bool nsscene::remove(nsentity * ent, bool remove_children)
{
	bool ret = true;
	while (ret)
		ret = remove(ent, 0, remove_children);
	return ret;
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
		dprint("nsscene::setMaxPlayers Players must be in max player range");
		return;
	}
	m_max_players = pMaxPlayers;
}

void nsscene::set_notes(const nsstring & pNotes)
{
	m_notes = pNotes;
}

void nsscene::set_skydome(nsentity * skydome, bool addToSceneIfNeeded)
{
	if (skydome == nullptr)
		return;

	nstform_comp * skytf = skydome->get<nstform_comp>();

	if (skytf != nullptr)
	{
		m_skydome = skydome;
		dprint("nsscene::set_skydome - Map \"" + m_name + "\"'s skydome set to \"" + skydome->name() + "\"");
	}
	else
	{
		if (addToSceneIfNeeded)
		{
			if (add(skydome) != -1) // -1 indicates failure
				set_skydome(skydome);
		}
	}
}

nsscene::pupped_vec & nsscene::unloaded()
{
	return m_unloaded_tforms.pv;
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
	if (comp_t->type() == type_to_hash(nssel_comp))
	{
		dprint("nsscene::_on_comp_add adding scene to selection component");
		nssel_comp * sc = static_cast<nssel_comp*>(comp_t);
		auto emp_iter = sc->m_scene_selection.emplace(this, new sel_per_scene_info(sc,this));
		assert(emp_iter.second);
	}
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
		dprint("nsscene::_on_comp_remove removing scene from selection component");
		nssel_comp * sc = static_cast<nssel_comp*>(comp_t);
		auto emp_iter = sc->m_scene_selection.find(this);
		delete emp_iter->second;
		sc->m_scene_selection.erase(emp_iter);
	}
	if (comp_t->type() == type_to_hash(nsrender_comp))
	{
		dprint("nscene::_on_comp_remove analyzing render component...");
		make_ent_not_instanced(comp_t->owner());
	}
	if (comp_t->type() == type_to_hash(nsanim_comp) || comp_t->type() == type_to_hash(nssprite_sheet_comp))
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

nsscene::pupped_tform_info::pupped_tform_info(uint32 id, const instance_tform & it):
	parent(),
	children(),
	itf(it),
    ent_tform_id()
{
    if (it.m_owner != nullptr)
    {
        ent_tform_id = uivec3(it.m_owner->owner->owner()->full_id(),id);
    }
	if (it.m_parent.is_valid())
	{
        parent = uivec3(it.m_parent.tfc->owner->owner()->full_id(),it.m_parent.ind);
	}
	auto child_iter = it.m_children.begin();
	while (child_iter != it.m_children.end())
	{
        children.push_back(uivec3(child_iter->tfc->owner->owner()->full_id(),child_iter->ind));
		++child_iter;
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
		
		auto add_iter = m_pupped_tforms.pv.begin();
		while (add_iter != m_pupped_tforms.pv.end())
		{
			nsentity * ent = get_asset<nsentity>(add_iter->ent_tform_id.xy());
			if (ent == nullptr)
			{
				m_unloaded_tforms.pv.emplace_back(*add_iter);
				add_iter = m_pupped_tforms.pv.erase(add_iter);
				dprint("nsscene::enable - Could not load entity tform with id " + add_iter->ent_tform_id.to_string());
			}
			else
			{
				uint32 id = add(ent, nullptr, add_iter->itf, false);
				dprint("nsscene::enable - Adding tform " + std::to_string(id) + " to entity " + ent->name());
				++add_iter;
			}
		}

		// After having added all tforms, preserving the ids, we should be able to assign parent/children
		// accordingly now
		auto piter = m_pupped_tforms.pv.begin();
		while (piter != m_pupped_tforms.pv.end())
		{
			nsentity * ent = get_asset<nsentity>(piter->ent_tform_id.xy());
			tform_per_scene_info * tpsi = ent->get<nstform_comp>()->per_scene_info(this);
            instance_tform * itf = &tpsi->m_tforms[piter->ent_tform_id.z];
            nsentity * parent_ent = get_asset<nsentity>(piter->parent.xy());
            if (parent_ent != nullptr)
			{
                itf->m_parent.tfc = parent_ent->get<nstform_comp>()->per_scene_info(this);
				itf->m_parent.ind = piter->parent.z;
			}
			for (uint32 i = 0; i < piter->children.size(); ++i)
			{
				nsentity * cent = get_asset<nsentity>(piter->children[i].xy());
				itf->m_children.push_back(instance_handle(
											  cent->get<nstform_comp>()->per_scene_info(this),
											  piter->children[i].z));
			}
			++piter;
		}

		m_pupped_tforms.pv.clear();
		m_skydome = find_entity(m_pupped_tforms.sky_id);
		m_pupped_tforms.sky_id = uivec2();
	}
	else
	{
		if (!m_enabled)
			return;
		
		// If disabling, fill the pupped vec and then remove everything from the scene
		m_pupped_tforms.pv.clear();

		_populate_pup_vec();
		m_pupped_tforms.sky_id = uivec2();
		if (m_skydome != nullptr)
			m_pupped_tforms.sky_id = m_skydome->full_id();

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
        {
            tform_per_scene_info * psi = tfc->per_scene_info(this);

            for (uint32 i = 0; i < psi->m_tforms.size(); ++i)
            {
                instance_tform & itf = psi->m_tforms[i];
                m_pupped_tforms.pv.emplace_back(pupped_tform_info(i,itf));
                m_pupped_tforms.pv.back().itf.m_children.clear();
				m_pupped_tforms.pv.back().itf.m_parent.invalidate();
            }
        }
		++ent_iter;
	}
}


