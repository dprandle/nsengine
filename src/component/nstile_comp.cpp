/*!
\file nstilecomp.cpp

\brief Definition file for nstile_comp class

This file contains all of the neccessary definitions for the nstile_comp class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#include <component/nstile_comp.h>
#include <nsentity.h>

nstile_comp::nstile_comp() :
	nscomponent(type_to_hash(nstile_comp))
{}

nstile_comp::nstile_comp(const nstile_comp & copy):
	nscomponent(copy)
{}

nstile_comp::~nstile_comp()
{}

nstile_comp* nstile_comp::copy(const nscomponent * to_copy)
{
	const nstile_comp * comp = dynamic_cast<const nstile_comp*>(to_copy);
	if (comp != nullptr)
		(*this) = *comp;
	return this;
}

void nstile_comp::init()
{}


void nstile_comp::pup(nsfile_pupper * p)
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

nstile_comp & nstile_comp::operator=(nstile_comp rhs_)
{
	nscomponent::operator=(rhs_);
	post_update(true);
	return (*this);
}
