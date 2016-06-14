/*!
\file nsui_button_comp.cpp

\brief Definition file for nsui_button_comp class

This file contains all of the neccessary definitions for the nsui_button_comp class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#include <nsui_button_comp.h>
#include <nsentity.h>

nsui_button_comp::nsui_button_comp() :
	nscomponent(type_to_hash(nsui_button_comp)),
	is_pressed(false),
	is_hover(false),
	is_toggled(false),
	is_enabled(true),
	toggle_enabled(false),
	button_states(),
	toggled_button_states(),
	m_mat_color_mode(false)
{}

nsui_button_comp::nsui_button_comp(const nsui_button_comp & copy):
	nscomponent(copy),
	is_pressed(false),
	is_hover(false),
	is_toggled(copy.is_toggled),
	is_enabled(copy.is_enabled),
	toggle_enabled(copy.toggle_enabled),
	m_mat_color_mode(false)
{
	for (uint8 i =0; i < 4; ++i)
	{
		button_states[i] = copy.button_states[i];
		toggled_button_states[i] = copy.toggled_button_states[i];
	}
}

nsui_button_comp::~nsui_button_comp()
{}

nsui_button_comp* nsui_button_comp::copy(const nscomponent * to_copy)
{
	const nsui_button_comp * comp = dynamic_cast<const nsui_button_comp*>(to_copy);
	if (comp != nullptr)
		(*this) = *comp;
	return this;
}

void nsui_button_comp::init()
{}

void nsui_button_comp::pup(nsfile_pupper * p)
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

nsui_button_comp & nsui_button_comp::operator=(nsui_button_comp rhs_)
{
	nscomponent::operator=(rhs_);
	post_update(true);
	return (*this);
}
