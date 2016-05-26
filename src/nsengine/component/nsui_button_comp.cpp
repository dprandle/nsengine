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
	nscomponent(),
	is_pressed(false),
	is_hover(false),
	m_mat_norm_color(fvec4(-1.0f)),
	m_mat_color_mode(false),
	m_mat_norm_tex_rect(fvec4(-1.0f))
{}

nsui_button_comp::nsui_button_comp(const nsui_button_comp & copy):
	nscomponent(copy),
	m_mat_norm_color(fvec4(-1.0f)),
	m_mat_color_mode(false),
	m_mat_norm_tex_rect(fvec4(-1.0f))
{		
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
