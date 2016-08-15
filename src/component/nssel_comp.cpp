/*!
\file nsselcomp.cpp

\brief Definition file for nssel_comp class

This file contains all of the neccessary definitions for the nssel_comp class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/


#include <component/nssel_comp.h>
#include <asset/nsentity.h>
#include <component/nssel_comp.h>
#include <nstimer.h>
#include <opengl/nsgl_buffer.h>
#include <asset/nsscene.h>

nssel_comp::nssel_comp() :
	nscomponent(type_to_hash(nssel_comp)),
	m_default_sel_color(DEFAULT_SEL_R, DEFAULT_SEL_G, DEFAULT_SEL_B, DEFAULT_SEL_A),
	m_sel_color(m_default_sel_color),
	m_mask_alpha(DEFAULT_SEL_MASK_A),
	m_draw_enabled(true),
	m_move_with_input(true),
	m_transparent_picking_enabled(false)
{}

nssel_comp::nssel_comp(const nssel_comp & copy):
	nscomponent(copy),
	m_default_sel_color(copy.m_default_sel_color),
	m_sel_color(copy.m_sel_color),
	m_mask_alpha(copy.m_mask_alpha),
	m_draw_enabled(copy.m_draw_enabled),
	m_move_with_input(copy.m_move_with_input),
	m_transparent_picking_enabled(copy.m_transparent_picking_enabled)
{}

nssel_comp::~nssel_comp()
{
	m_scene_selection.clear();
}

void nssel_comp::clear_all_selections()
{
	auto iter = m_scene_selection.begin();
	while (iter != m_scene_selection.end())
	{
		iter->second->m_selection.clear();
		iter->second->m_selected = false;
		++iter;
	}
	m_update = true;
}

void nssel_comp::enable_draw(bool pEnable)
{
	m_draw_enabled = pEnable;
}

void nssel_comp::pup(nsfile_pupper * p)
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

void nssel_comp::enable_move(const bool & pEnable)
{
	m_move_with_input = pEnable;
}

const fvec4 & nssel_comp::default_color()
{
	return m_default_sel_color;
}

float nssel_comp::mask_alpha()
{
	return m_mask_alpha;
}

const fvec4 & nssel_comp::color()
{
	return m_sel_color;
}

void nssel_comp::init()
{}

bool nssel_comp::draw_enabled()
{
	return m_draw_enabled;
}

bool nssel_comp::move_enabled()
{
	return m_move_with_input;
}

bool nssel_comp::selected(nsscene * scn)
{
	auto fiter = m_scene_selection.find(scn);
	if (fiter != m_scene_selection.end())
		return fiter->second->m_selected;
	return false;
}

ui_uset * nssel_comp::selection(nsscene * scn)
{
	auto fiter = m_scene_selection.find(scn);
	if (fiter != m_scene_selection.end())
		return &fiter->second->m_selection;
	return nullptr;
}

void nssel_comp::set_default_sel_color(const fvec4 & pColor)
{
	m_default_sel_color = pColor;
	m_sel_color = pColor;
}

void nssel_comp::set_mask_alpha(const float & pAlpha)
{
	m_mask_alpha = pAlpha;
}

void nssel_comp::set_selected(nsscene * scn, bool pSelected)
{
	auto fiter = m_scene_selection.find(scn);
	if (fiter != m_scene_selection.end())
		fiter->second->m_selected = pSelected;
}

void nssel_comp::set_color(const fvec4 & pColor)
{
	m_sel_color = pColor;
}

void nssel_comp::enable_transparent_picking(bool enable)
{
	m_transparent_picking_enabled = enable;
}

bool nssel_comp::transparent_picking_enabled() const
{
	return m_transparent_picking_enabled;
}

nssel_comp & nssel_comp::operator=(nssel_comp rhs_)
{
	nscomponent::operator=(rhs_);
	std::swap(m_default_sel_color, rhs_.m_default_sel_color);
	std::swap(m_sel_color, rhs_.m_sel_color);
	std::swap(m_mask_alpha, rhs_.m_mask_alpha);
	std::swap(m_draw_enabled, rhs_.m_draw_enabled);
	std::swap(m_move_with_input, rhs_.m_move_with_input);
	std::swap(m_transparent_picking_enabled, rhs_.m_transparent_picking_enabled);
	post_update(true);
	return (*this);
}

sel_per_scene_info::sel_per_scene_info(nssel_comp * owner_, nsscene * scn):
	nsvideo_object(),
	owner(owner_),
	scene(scn),
	m_selection(),
	m_selected(false)
{
	video_context_init();
}

sel_per_scene_info * nssel_comp::scene_info(nsscene * scn)
{
	auto fiter = m_scene_selection.find(scn);
	if (fiter != m_scene_selection.end())
		return fiter->second;
	return nullptr;
}

void sel_per_scene_info::video_context_init()
{
	vid_ctxt * vc = nse.video_driver()->current_context();
	if (vc != nullptr)
	{
		if (ctxt_objs[vc->context_id] == nullptr)
		{
			ctxt_objs[vc->context_id] = nse.factory<nsvid_obj_factory>(SEL_VID_OBJ_GUID)->create(this);
		}
		else
		{
			dprint("sel_per_scene_info::video_context_init - Context has already been initialized for sel_psi " + owner->owner()->name() + " in scene " + scene->name() + " in ctxtid " + std::to_string(vc->context_id));
		}
	}
}

sel_per_scene_info::~sel_per_scene_info()
{
	m_selection.clear();
}
