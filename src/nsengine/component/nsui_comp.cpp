/*!
\file nsui_comp.cpp

\brief Definition file for nsui_comp class

This file contains all of the neccessary definitions for the nsui_comp class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#include <nsui_comp.h>
#include <nsentity.h>

nsui_comp::nsui_comp() :
	nscomponent()
{}

nsui_comp::nsui_comp(const nsui_comp & copy):
	nscomponent(copy)
{}

nsui_comp::~nsui_comp()
{}

nsui_comp* nsui_comp::copy(const nscomponent * to_copy)
{
	const nsui_comp * comp = dynamic_cast<const nsui_comp*>(to_copy);
	if (comp != nullptr)
		(*this) = *comp;
	return this;
}

void nsui_comp::init()
{}

void nsui_comp::pup(nsfile_pupper * p)
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

nsui_comp & nsui_comp::operator=(nsui_comp rhs_)
{
	nscomponent::operator=(rhs_);
	post_update(true);
	return (*this);
}
