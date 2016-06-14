/*! 
	\file nsscene.cpp
	
	\brief Definition file for nsscene class

	This file contains all of the neccessary definitions for the nsscene class.

	\author Daniel Randle
	\date November 23 2013
	\copywrite Earth Banana Games 2013
*/

#include <nsselection_system.h>
#include <nstile_brush_comp.h>
#include <nsscene.h>
#include <nsscene_manager.h>
#include <nsentity.h>
#include <nsrender_comp.h>
#include <nsanim_comp.h>
#include <nstform_comp.h>
#include <nstile_grid.h>
#include <nsrender_system.h>
#include <nscam_comp.h>
#include <nsres_manager.h>
#include <nssel_comp.h>
#include <nsengine.h>
#include <nsentity_manager.h>
#include <nsoccupy_comp.h>
#include <nsplugin_manager.h>
#include <nslight_comp.h>

nsscene::nsscene():
	nsresource(type_to_hash(nsscene)),
	m_skydome(nullptr),
	m_max_players(0),
	m_bg_color(),
	m_notes(),
	m_creator(),
	m_tile_grid(new nstile_grid()),
	m_ents_by_comp(),
	m_unloaded()
{
	set_ext(DEFAULT_SCENE_EXTENSION);
}


nsscene::nsscene(const nsscene & copy_):
	nsresource(copy_),
	m_skydome(copy_.m_skydome),
	m_max_players(copy_.m_max_players),
	m_bg_color(copy_.m_bg_color),
	m_notes(copy_.m_notes),
	m_creator(copy_.m_creator),
	m_tile_grid(new nstile_grid()),
	m_ents_by_comp(),
	m_unloaded()
{
	auto ents = copy_.entities_in_scene();
	if (ents != nullptr)
	{
		auto iter = ents->begin();
		while (iter != ents->end())
		{
			nstform_comp * tfc = (*iter)->get<nstform_comp>();
			for (uint32 i = 0; i < tfc->instance_count(&copy_); ++i)
			{
				instance_tform & itf = *tfc->instance_transform(&copy_, i);
				if (itf.parent() == nullptr) // if root scene node
					add(*iter, nullptr, *tfc->instance_transform(&copy_, i), true); // add children too
			}
			++iter;
		}
	}
}

nsscene::~nsscene()
{
	delete m_tile_grid;
}

nsscene & nsscene::operator=(nsscene rhs)
{
	nsresource::operator=(rhs);
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
	auto ents = entities_in_scene();
	if (ents == nullptr)
		return;

	while (ents->begin() != ents->end())
		remove(*(ents->begin()), true);
	
	m_unloaded.clear();
}


uint32 nsscene::add(
	nsentity * ent,
	instance_tform * parent,
	const instance_tform & tf_info,
	bool add_children)
{
	nstform_comp * tComp = ent->get<nstform_comp>();
	nsoccupy_comp * occComp = ent->get<nsoccupy_comp>();

	bool make_tform = false;
	
	// If there is no tranform component, then make one
	if (tComp == nullptr)
	{
		tComp = ent->create<nstform_comp>();
		make_tform = true;
	}

	auto fiter = tComp->m_scenes_info.emplace(
		this,
		new tform_per_scene_info(tComp,this)).first;
	tform_per_scene_info & pse = *fiter->second;
	
	uint32 tfid = pse.m_tforms.size();
	pse.m_tforms.resize(tfid + 1);
	instance_tform & tf = pse.m_tforms[tfid];
	tf = tf_info;
	tf.m_scene = this;
	tf.m_owner = tComp;
	tf.set_parent(parent, true);

	// If there is an occupy component, attemp at inserting it in to the map. It will fail if the space
	// is already occupied therefore we need to remove the tranform component and set the entities scene
	// ID to 0 again if the add fails, since that means no other instances of the entity are in the scene
	if (occComp != nullptr)
	{
		// Get the transform ID that would result if we inserted it in to the scene
		// We don't want to insert it yet because we first want to check if the space is open
		uint32 pID = tComp->instance_count(this);
		if (!m_tile_grid->add(uivec3(ent->full_id(), pID), occComp->spaces(), tf_info.world_position()))
		{
			tf.set_parent(nullptr, true);
			pse.m_tforms.resize(tfid);

			if (pse.m_tforms.empty())
				tComp->m_scenes_info.erase(fiter);

			if (tComp->m_scenes_info.empty())
				ent->del<nstform_comp>();
			
			return -1;
		}
	}

	if (add_children)
	{
		for (uint32 i = 0; i < tf_info.m_children.size(); ++i)
		{
			if (add(tf_info.m_children[i]->owner()->owner(), &tf, *tf_info.m_children[i], add_children) == -1)
			{
				dprint("nsscene::add Could not add child of " + ent->name() + " tformid " + std::to_string(tfid) + " because occupy_comp did not allow it");
			}
		}
	}

	if (make_tform)
	{
		_add_all_comp_entries(ent);
		sig_connect(ent->component_added, nsscene::_on_comp_add);
		sig_connect(ent->component_removed, nsscene::_on_comp_remove);
	}
	
	tComp->post_update(true);
	pse.m_buffer_resized = true;
	return tfid;
}

uint32 nsscene::add(
	nsentity * ent,
	instance_tform * parent,
	bool snap_to_grid,
	const fvec3 & pPos,
	const fquat & orient_,
	const fvec3 & scaling_)
{
	instance_tform tf;
	tf.snap_to_grid = snap_to_grid;
	tf.m_position = pPos;
	tf.m_orient = orient_;
	tf.m_scaling = scaling_;
	return add(ent, parent, tf, false);
}

void nsscene::add_gridded(
	nsentity * ent,
	const ivec3 & bounds,
	const fvec3 & starting_pos,
	const fquat & orient_,
	const fvec3 & scaling_
	)
{
	nstform_comp * tComp = ent->get<nstform_comp>();
    bool added_tform = false;
	if (tComp == nullptr)
    {
		tComp = ent->create<nstform_comp>();
        added_tform = true;
    }

	auto fiter = tComp->m_scenes_info.emplace(
		this,
		new tform_per_scene_info(tComp,this)).first;
	tform_per_scene_info & pse = *fiter->second;

	// Figure out the total number of transforms needed and allocate that 
	// much memory (addTransforms does this)
	uint32 addSize = bounds.x * bounds.y * bounds.z;
	pse.m_tforms.reserve(pse.m_tforms.size() + addSize);
	instance_tform itf;
	for (int32 z = 0; z < bounds.z; ++z)
	{
		for (int32 y = 0; y < bounds.y; ++y)
		{
			for (int32 x = 0; x < bounds.x; ++x)
			{
				float xP = X_GRID * x * 2.0f;
				float yP = Y_GRID * y;
				float zP = Z_GRID * z;
				if (y % 2 != 0)
					xP += X_GRID;
				fvec3 pos(xP, yP, zP);
				pos += starting_pos;

				itf.m_position = pos;
				itf.m_orient = orient_;
				itf.m_scaling = scaling_;
				itf.snap_to_grid = true;
				if (add(ent, nullptr, itf, false) == -1)
				{
					dprint("nsscene::add_gridded Could not add grid position " + ivec3(x,y,z).to_string() + " to entity " + ent->name() + " because occupy component would not allow it");
				}
			}
		}
	}

    if (added_tform)
    {
        _add_all_comp_entries(ent);
        sig_connect(ent->component_added, nsscene::_on_comp_add);
        sig_connect(ent->component_removed, nsscene::_on_comp_remove);
    }
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
		ret;

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
					instance_tform * itf = tc->instance_transform(this, id.z);
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
						instance_tform * itf = tc->instance_transform(this, id.z);
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
						instance_tform * itf = tc->instance_transform(this, id.z);
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
						instance_tform * itf = tc->instance_transform(this, id.z);
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
						instance_tform * itf = tc->instance_transform(this, id.z);
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
	nstform_comp * tComp = itform->owner();
	nsentity * entity = tComp->owner();
	
	auto fiter = tComp->m_scenes_info.find(this);
	if (fiter == tComp->m_scenes_info.end())
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
		itform->owner()->post_update(true);
	}

    uint32 tformid = (itform - &pse->m_tforms[0]);
	nssel_comp * sel_cmp = entity->get<nssel_comp>();
	if (sel_cmp != nullptr)
	{
		auto selection = sel_cmp->selection(this);
		
		// Since remove replaces the tform at tformid with a copy of the last tform and then pops
		// the back, we need to replace the index appropriately in the selection component
		if (selection->erase(pse->m_tforms.size()) != 1)
			selection->erase(tformid);
	}
	
	nsoccupy_comp * occComp = entity->get<nsoccupy_comp>();
	if (occComp != nullptr)
	{
		m_tile_grid->remove(occComp->spaces(), pos);

		// This should take care of updating the reference ID for the space that got switched..
		if (pse->m_tforms.size() != 0)
		{
			fvec3 newPos = pse->m_tforms[tformid].world_position();
			m_tile_grid->remove(occComp->spaces(), newPos);
			m_tile_grid->add(uivec3(entity->full_id(), tformid), occComp->spaces(), newPos);
		}
	}

	if (pse->m_tforms.size() == 0)
	{
		sig_disconnect(tComp->owner()->component_added);
		sig_disconnect(tComp->owner()->component_removed);
		_remove_all_comp_entries(tComp->owner());

		delete pse;
		tComp->m_scenes_info.erase(fiter);		
		if (tComp->m_scenes_info.empty())
			entity->del<nstform_comp>();
		
		if (m_skydome == entity)
			m_skydome = nullptr;
	}
	
	return true;
}

bool nsscene::remove(nsentity * entity, uint32 tformid, bool remove_children)
{
	nstform_comp * tComp = entity->get<nstform_comp>();
	if (tComp == nullptr)
		return false;
	return remove(tComp->instance_transform(this, tformid), remove_children);
}

bool nsscene::remove(fvec3 & pWorldPos, bool remove_children)
{
	uivec3 refid = m_tile_grid->get(pWorldPos);
	if (refid == uivec3(0))
		return false;

	return remove(get_resource<nsentity>(refid.xy()), refid.z, remove_children);
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

uivec2_vector & nsscene::unloaded()
{
	return m_unloaded;
}

void nsscene::_on_comp_add(nscomponent * comp_t)
{
	auto fiter = m_ents_by_comp.emplace(comp_t->type(), std::unordered_set<nsentity*>());
	fiter.first->second.emplace(comp_t->owner());
	dprint("nsscene::_on_comp_add Added component " + hash_to_guid(comp_t->type()) + " to entity " + comp_t->owner()->name() + " in scene " + m_name);
	if (hash_to_guid(comp_t->type()) == "nssel_comp")
	{
		dprint("nsscene::_on_comp_add Adding scene to selection component");
		nssel_comp * sc = static_cast<nssel_comp*>(comp_t);
		auto emp_iter = sc->m_scene_selection.emplace(this, new sel_per_scene_info(sc,this));
		if (!emp_iter.second)
			dprint("nsscene::_on_comp_add Something in adding the comp went seriously wrong");
	}
}

void nsscene::_on_comp_remove(nscomponent * comp_t)
{
	auto fiter = m_ents_by_comp.find(comp_t->type());
	if (fiter != m_ents_by_comp.end())
	{
		fiter->second.erase(comp_t->owner());
		dprint("nsscene::_on_comp_remove Removed component " + hash_to_guid(comp_t->type()) + " from entity " + comp_t->owner()->name() + " in scene " + m_name);
		if (hash_to_guid(comp_t->type()) == "nssel_comp")
		{
			dprint("nsscene::_on_comp_remove Remocing scene from selection component");
			nssel_comp * sc = static_cast<nssel_comp*>(comp_t);
			auto emp_iter = sc->m_scene_selection.find(this);
			delete emp_iter->second;
			sc->m_scene_selection.erase(emp_iter);
			nse.system<nsselection_system>()->refresh_selection(this);
		}
	}
	else
	{
		dprint("nsscene::_on_comp_remove - Trying to remove comp that has not been added apparently");
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
