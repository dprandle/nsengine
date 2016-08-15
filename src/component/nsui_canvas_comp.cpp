/*!
\file nsui_canvas_comp.cpp

\brief Definition file for nsui_canvas_comp class

This file contains all of the neccessary definitions for the nsui_canvas_comp class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#include <component/nsui_canvas_comp.h>
#include <asset/nsentity.h>

nsui_canvas_comp::nsui_canvas_comp() :
	nscomponent(type_to_hash(nsui_canvas_comp)),
	m_unloaded_ents(),
	m_ents_by_comp(),
    m_pupped_rects(),
    m_enabled(false)
{}

nsui_canvas_comp::nsui_canvas_comp(const nsui_canvas_comp & copy):
	nscomponent(copy),
	m_unloaded_ents(),
	m_ents_by_comp(),
	m_pupped_rects(),
	m_enabled(false)
{
	std::vector<rect_info> pprects;
	if (copy.m_enabled)
	{
		
	}
//	auto ents = copy.entities_in_canvas();
}

nsui_canvas_comp::~nsui_canvas_comp()
{
}

nsui_canvas_comp* nsui_canvas_comp::copy(const nscomponent * to_copy)
{
	const nsui_canvas_comp * comp = dynamic_cast<const nsui_canvas_comp*>(to_copy);
	if (comp != nullptr)
		(*this) = *comp;
	return this;
}

void nsui_canvas_comp::init()
{
}

void nsui_canvas_comp::release()
{
    if (m_enabled)
        enable(false);
}

void nsui_canvas_comp::pup(nsfile_pupper * p)
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

void nsui_canvas_comp::update_rects(const fvec2 & pscreen_size)
{
	if (!m_enabled)
	{
		dprint("nsui_canvas_comp::update Cannot update disabled canvas " + m_owner->name());
		return;
	}

	m_owner->get<nsrect_tform_comp>()->update_recursively(this, pscreen_size);
}

void nsui_canvas_comp::enable(bool enbl)
{
    if (enbl)
    {
		if (m_enabled)
			return;

		
		m_enabled = true;

		add(m_owner);

        // Go through all saved rects and load them
        for (uint32 i = 0; i < m_pupped_rects.size(); ++i)
        {

            // Load the current rect
            nsentity * ent = get_asset<nsentity>(m_pupped_rects[i].this_ent);
            if (ent == nullptr)
            {
                m_unloaded_ents.push_back(m_pupped_rects[i].this_ent);
                dprint("nsui_canvas_comp::finalize - Could not load ent with id " +
                       m_pupped_rects[i].this_ent.to_string());
                continue;
            }

			if (m_owner != ent)
				add(ent);

            // Set all the canvas per canvas info in the rect comp to match the pupped rect comp
            nsrect_tform_comp * tuic = ent->get<nsrect_tform_comp>();
            tuic->m_canvas_settings.find(this)->second.pci = m_pupped_rects[i].pci;
        }

        // Now that all rect comps have been loaded, set all parents and children accordingly
        for (uint32 i = 0; i < m_pupped_rects.size(); ++i)
        {
			nsentity * cur_ent = get_asset<nsentity>(m_pupped_rects[i].this_ent);
            nsrect_tform_comp * rtc = cur_ent->get<nsrect_tform_comp>();
			
            // Try to load the current rect's parent
            if (m_pupped_rects[i].pupped_parent != uivec2(0))
            {
                nsentity * parent_ent = get_asset<nsentity>(m_pupped_rects[i].pupped_parent);
                if (parent_ent == nullptr)
                {
                    dprint("nsui_canvas_comp::finalize - Could not load parent ent with id " +
                           m_pupped_rects[i].pupped_parent.to_string());
                }
                else
                {
                    nsrect_tform_comp * tuic_parent = parent_ent->get<nsrect_tform_comp>();
                    rtc->set_parent(this, tuic_parent);
                }
            }
        }
        m_pupped_rects.clear();
    }
    else
    {
		if (!m_enabled)
			return;

		m_pupped_rects.clear();
		_populate_pup_vec();
		clear();
		m_enabled = false;
    }
}

void nsui_canvas_comp::clear()
{
	if (!m_enabled)
	{
		dprint("nsui_canvas_comp::clear Cannot clear disabled canvas " + m_owner->name());
		return;
	}
	
//	auto ents = entities_in_canvas();
//	if (ents == nullptr)
//		return;

//	while (ents->begin() != ents->end())
//		remove(*(ents->begin()), true);

    remove(m_owner,true);
	
	m_unloaded_ents.clear();	
}

void nsui_canvas_comp::add(nsentity * to_add)
{
	if (!m_enabled)
	{
		dprint("nsui_canvas_comp::add Cannot add to disabled canvas " + m_owner->name());
		return;
	}

	nsrect_tform_comp * tuic = to_add->get<nsrect_tform_comp>();
	if (tuic == nullptr)
		tuic = to_add->create<nsrect_tform_comp>();

	auto emp_iter = tuic->m_canvas_settings.emplace(this, nsrect_tform_comp::per_canvas_settings());
	if (!emp_iter.second)
	{
		dprint("nsui_canvas_comp::add Cannot add entity " + tuic->owner()->name() + " to canvas " + owner()->name() + " because it is already in the canvas");
		return;
	}
	if (to_add != m_owner)
		m_owner->get<nsrect_tform_comp>()->add_child(tuic);
	_add_all_comp_entries(to_add);
	sig_connect(to_add->component_added, nsui_canvas_comp::_on_comp_add);
	sig_connect(to_add->component_removed, nsui_canvas_comp::_on_comp_remove);	
}

void nsui_canvas_comp::remove(nsentity * to_remove, bool remove_children)
{
	if (!m_enabled)
	{
		dprint("nsui_canvas_comp::remove Cannot remove from disabled canvas " + m_owner->name());
		return;
	}

	nsrect_tform_comp * tuic = to_remove->get<nsrect_tform_comp>();
	auto fiter = tuic->m_canvas_settings.find(this);

	for (uint32 i = 0; i < fiter->second.m_children.size(); ++i)
	{
		if (remove_children)
			remove(fiter->second.m_children[i]->owner(), true);
		else
		{
			auto child_fiter = fiter->second.m_children[i]->m_canvas_settings.find(this);
			child_fiter->second.m_parent = fiter->second.m_parent;
		}
	}
	
	sig_disconnect(to_remove->component_added);
	sig_disconnect(to_remove->component_removed);
	_remove_all_comp_entries(to_remove);
	tuic->m_canvas_settings.erase(this);
	if (tuic->m_canvas_settings.empty())
		to_remove->destroy<nsrect_tform_comp>();
}

void nsui_canvas_comp::_remove_all_comp_entries(nsentity * ent)
{
	auto citer = ent->begin();
	while (citer != ent->end())
	{
		_on_comp_remove(citer->second);
		++citer;
	}
}

void nsui_canvas_comp::_add_all_comp_entries(nsentity * ent)
{
	auto citer = ent->begin();
	while (citer != ent->end())
	{
		_on_comp_add(citer->second);
		++citer;
	}	
}

nsui_canvas_comp & nsui_canvas_comp::operator=(nsui_canvas_comp rhs_)
{
	nscomponent::operator=(rhs_);
	std::swap(m_router, rhs_.m_router);
	std::swap(m_unloaded_ents, rhs_.m_unloaded_ents);
	std::swap(m_ents_by_comp, rhs_.m_ents_by_comp);
	std::swap(m_pupped_rects, rhs_.m_pupped_rects);
	post_update(true);
	return (*this);
}

entity_set * nsui_canvas_comp::entities_with_comp(uint32 comp_type_id)
{
	auto fiter = m_ents_by_comp.find(comp_type_id);
	if (fiter != m_ents_by_comp.end())
		return &fiter->second;
	return nullptr;
}

entity_set * nsui_canvas_comp::entities_in_canvas()
{
	return entities_with_comp<nsrect_tform_comp>();
}

bool nsui_canvas_comp::is_enabled()
{
    return m_enabled;
}

void nsui_canvas_comp::_populate_pup_vec()
{
	entity_set * ents = entities_in_canvas();

	if (ents == nullptr)
		return;
	
	auto iter = ents->begin();
	while (iter != ents->end())
	{
		rect_info ri;
		nsrect_tform_comp * tuic = (*iter)->get<nsrect_tform_comp>();

        ri.this_ent = (*iter)->full_id();
		auto fiter = tuic->m_canvas_settings.find(this);
		ri.pci = fiter->second.pci;

		if (fiter->second.m_parent != nullptr)
			ri.pupped_parent = fiter->second.m_parent->owner()->full_id();

		for (uint32 i = 0; i < fiter->second.m_children.size(); ++i)
			ri.m_pupped_children.push_back(fiter->second.m_children[i]->owner()->full_id());

        m_pupped_rects.push_back(ri);
		++iter;
	}
}

void nsui_canvas_comp::_on_comp_add(nscomponent * comp_t)
{
	auto fiter = m_ents_by_comp.emplace(comp_t->type(), std::unordered_set<nsentity*>());
	fiter.first->second.emplace(comp_t->owner());
	dprint("nsui_canvas_comp::_on_comp_add Added component " + hash_to_guid(comp_t->type()) + " to entity " + comp_t->owner()->name() + " in canvas " + m_owner->name());
}

void nsui_canvas_comp::_on_comp_remove(nscomponent * comp_t)
{
	auto fiter = m_ents_by_comp.find(comp_t->type());
	if (fiter != m_ents_by_comp.end())
	{
		fiter->second.erase(comp_t->owner());
		dprint("nsui_canvas_comp::_on_comp_remove Removed component " + hash_to_guid(comp_t->type()) + " from entity " + comp_t->owner()->name() + " in canvas " + m_owner->name());
	}
	else
	{
		dprint("nsui_canvas_comp::_on_comp_remove - Trying to remove comp that has not been added apparently");
	}
}
