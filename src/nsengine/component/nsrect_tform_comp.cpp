/*!
\file nsrect_tform_comp.cpp

\brief Definition file for nsrect_tform_comp class

This file contains all of the neccessary definitions for the nsrect_tform_comp class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#include <nsui_comp.h>
#include <nsrect_tform_comp.h>
#include <nsentity.h>

nsrect_tform_comp::nsrect_tform_comp() :
	nscomponent()
{}

nsrect_tform_comp::nsrect_tform_comp(const nsrect_tform_comp & copy):
	nscomponent(copy)
{		
}

nsrect_tform_comp::~nsrect_tform_comp()
{
}

nsrect_tform_comp* nsrect_tform_comp::copy(const nscomponent * to_copy)
{
	const nsrect_tform_comp * comp = dynamic_cast<const nsrect_tform_comp*>(to_copy);
	if (comp != nullptr)
		(*this) = *comp;
	return this;
}

void nsrect_tform_comp::init()
{}

void nsrect_tform_comp::pup(nsfile_pupper * p)
{
	// purposely do nothing - cannot pup this component its canvases job!
}

nsrect_tform_comp & nsrect_tform_comp::operator=(nsrect_tform_comp rhs_)
{
	nscomponent::operator=(rhs_);
	post_update(true);
	return (*this);
}

bool nsrect_tform_comp::has_child(nsrect_tform_comp * child)
{
	auto iter = m_canvas_settings.begin();
	while (iter != m_canvas_settings.end())
	{
		for (uint32 i = 0; i < iter->second.m_children.size(); ++i)
		{
			if (iter->second.m_children[i] == child || iter->second.m_children[i]->has_child(child))
				return true;
		}
		++iter;
	}	
	return false;
}

void nsrect_tform_comp::update_recursively(nsui_canvas_comp * canvas, const fvec2 & pscreen_size)
{
	auto fiter = m_canvas_settings.find(canvas);
	auto pci = &fiter->second.pci;
	fvec4 screen_rect(pscreen_size % pci->anchor_rect.xy() + pci->pixel_offset_rect.xy(),
					  pscreen_size % pci->anchor_rect.zw() + pci->pixel_offset_rect.zw());
	fvec2 scale = screen_rect.zw() - screen_rect.xy();
	fvec2 pos = screen_rect.xy() + scale % pci->pivot;
	fiter->second.content_world_tform = rotation2d_mat3(pci->angle) % fvec3(scale,1.0f);
	fiter->second.content_world_tform.set_column(2, fvec3(pos, 1.0f));

	nsui_comp * uic = m_owner->get<nsui_comp>();
	if (uic != nullptr)
	{
		fvec2 border_scale = screen_rect.zw() + uic->outer_properties.border.zw() - (screen_rect.xy() + uic->outer_properties.border.xy());
		fiter->second.border_world_tform = rotation2d_mat3(pci->angle) % fvec3(border_scale,1.0f);
		pos.x += uic->outer_properties.border.z - uic->outer_properties.border.x;
		pos.y += uic->outer_properties.border.w - uic->outer_properties.border.y;
		fiter->second.border_world_tform.set_column(2, fvec3(pos, 1.0f));
	}

	if (fiter->second.m_parent != nullptr)
	{
		auto parent_fiter = fiter->second.m_parent->m_canvas_settings.find(canvas);
		fiter->second.content_world_tform = parent_fiter->second.content_world_tform * fiter->second.content_world_tform;
	}

	for (uint32 i = 0; i < fiter->second.m_children.size(); ++i)
		fiter->second.m_children[i]->update_recursively(canvas, pscreen_size);	
}

const fmat3 & nsrect_tform_comp::content_transform(nsui_canvas_comp * canvas)
{
	auto fiter = m_canvas_settings.find(canvas);
	return fiter->second.content_world_tform;
}

const fmat3 & nsrect_tform_comp::border_transform(nsui_canvas_comp * canvas)
{
	auto fiter = m_canvas_settings.find(canvas);
	return fiter->second.border_world_tform;
}

void nsrect_tform_comp::add_child(nsrect_tform_comp * child)
{
	if (has_child(child))
		return;
	
	auto iter = m_canvas_settings.begin();
	while (iter != m_canvas_settings.end())
	{
		auto fiter = child->m_canvas_settings.find(iter->first);
		if (fiter != child->m_canvas_settings.end()) // if the child is not part of the active canvas then return
		{		
			if (fiter->second.m_parent != nullptr)
				fiter->second.m_parent->remove_child(child);
		
			iter->second.m_children.push_back(child);
			fiter->second.m_parent = this;
		}
		++iter;
	}	
}

void nsrect_tform_comp::finalize()
{
}

void nsrect_tform_comp::remove_child(nsrect_tform_comp * child)
{
	auto iter = m_canvas_settings.begin();
	while (iter != m_canvas_settings.end())
	{
		auto child_iter = iter->second.m_children.begin();
		while (child_iter != iter->second.m_children.end())
		{
			if (*child_iter == child)
			{
				iter->second.m_children.erase(child_iter);
				child->m_canvas_settings.find(iter->first)->second.m_parent = nullptr;
				return;
			}
			++child_iter;
		}
		++iter;
	}
}

void nsrect_tform_comp::set_parent(nsui_canvas_comp * canvas, nsrect_tform_comp * parent)
{
	if (parent == nullptr)
	{
		auto fiter = m_canvas_settings.find(canvas);
		if (fiter == m_canvas_settings.end() || fiter->second.m_parent == nullptr)
			return;		
		fiter->second.m_parent->remove_child(this);
	}
	else
		parent->add_child(this);
}

nsrect_tform_comp * nsrect_tform_comp::parent(nsui_canvas_comp * canvas)
{
	auto fiter = m_canvas_settings.find(canvas);
	if (fiter != m_canvas_settings.end())
		return fiter->second.m_parent;
	return nullptr;
}

per_canvas_info * nsrect_tform_comp::canvas_info(nsui_canvas_comp * canvas)
{
	auto fiter = m_canvas_settings.find(canvas);
	if (fiter == m_canvas_settings.end())
		return nullptr;
	return &fiter->second.pci;
}
