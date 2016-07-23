/*!
\file nssprite_comp.cpp

\brief Definition file for nssprite_comp class

This file contains all of the neccessary definitions for the nssprite_comp class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#include <nssprite_comp.h>
#include <nsentity.h>

nssprite_sheet_comp::nssprite_sheet_comp() :
	nscomponent(type_to_hash(nssprite_sheet_comp))
{}

nssprite_sheet_comp::nssprite_sheet_comp(const nssprite_sheet_comp & copy):
	nscomponent(copy)
{}

nssprite_sheet_comp::~nssprite_sheet_comp()
{}

nssprite_sheet_comp* nssprite_sheet_comp::copy(const nscomponent * to_copy)
{
	const nssprite_sheet_comp * comp = dynamic_cast<const nssprite_sheet_comp*>(to_copy);
	if (comp != nullptr)
		(*this) = *comp;
	return this;
}

void nssprite_sheet_comp::init()
{}


void nssprite_sheet_comp::pup(nsfile_pupper * p)
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

nssprite_sheet_comp & nssprite_sheet_comp::operator=(nssprite_sheet_comp rhs_)
{
	nscomponent::operator=(rhs_);
	post_update(true);
	return (*this);
}