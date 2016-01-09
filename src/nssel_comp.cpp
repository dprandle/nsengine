/*!
\file nsselcomp.cpp

\brief Definition file for nssel_comp class

This file contains all of the neccessary definitions for the nssel_comp class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/


#include <nssel_comp.h>
#include <nsentity.h>
#include <nssel_comp.h>
#include <nstimer.h>

nssel_comp::nssel_comp() :
	nscomponent(),
	m_default_sel_color(DEFAULT_SEL_R, DEFAULT_SEL_G, DEFAULT_SEL_B, DEFAULT_SEL_A),
	m_sel_color(m_default_sel_color),
	m_mask_alpha(DEFAULT_SEL_MASK_A),
	m_selected(false),
	m_draw_enabled(true),
	m_selection(),
	m_move_with_input(true),
	m_transparent_picking_enabled(false)
{}

nssel_comp::nssel_comp(const nssel_comp & copy):
	nscomponent(copy),
	m_default_sel_color(copy.m_default_sel_color),
	m_sel_color(copy.m_sel_color),
	m_mask_alpha(copy.m_mask_alpha),
	m_selected(false),
	m_draw_enabled(copy.m_draw_enabled),
	m_move_with_input(copy.m_move_with_input),
	m_selection(copy.m_selection),
	m_transparent_picking_enabled(copy.m_transparent_picking_enabled)
{
	
}

nssel_comp::~nssel_comp()
{}

bool nssel_comp::add(uint32 pTransformID)
{
	m_selection.insert(pTransformID);
	set_selected(true);
	return true;
}

ui_uset::iterator nssel_comp::begin()
{
	return m_selection.begin();
}

void nssel_comp::clear()
{
	m_selection.clear();
	set_selected(false);
}

bool nssel_comp::contains(uint32 pTransformID)
{
	return (m_selection.find(pTransformID) != m_selection.end());
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

ui_uset::iterator nssel_comp::end()
{
	return m_selection.end();
}

const fvec4 & nssel_comp::default_color()
{
	return m_default_sel_color;
}

const float & nssel_comp::mask_alpha()
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

const bool & nssel_comp::move_enabled()
{
	return m_move_with_input;
}

bool nssel_comp::selected()
{
	return m_selected;
}

bool nssel_comp::remove(uint32 pTransformID)
{
	if (contains(pTransformID))
	{
		m_selection.erase(pTransformID);
		return true;
	}
	return false;
}

bool nssel_comp::set(uint32 pTransformID)
{
	clear();
	return add(pTransformID);
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

void nssel_comp::set_selected(bool pSelected)
{
	m_selected = pSelected;
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

uint32 nssel_comp::count()
{
	return static_cast<uint32>(m_selection.size());
}

bool nssel_comp::empty()
{
	return m_selection.empty();
}

nssel_comp & nssel_comp::operator=(nssel_comp rhs_)
{
	nscomponent::operator=(rhs_);
	std::swap(m_default_sel_color, rhs_.m_default_sel_color);
	std::swap(m_sel_color, rhs_.m_sel_color);
	std::swap(m_mask_alpha, rhs_.m_mask_alpha);
	std::swap(m_selected, rhs_.m_selected);
	std::swap(m_draw_enabled, rhs_.m_draw_enabled);
	std::swap(m_move_with_input, rhs_.m_move_with_input);
	std::swap(m_selection, rhs_.m_selection);
	std::swap(m_transparent_picking_enabled, rhs_.m_transparent_picking_enabled);
	post_update(true);
	return (*this);
}
