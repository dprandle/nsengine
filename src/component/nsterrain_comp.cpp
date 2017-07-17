/*!
\file nsterraincomp.cpp

\brief Definition file for nsterrain_comp class

This file contains all of the neccessary definitions for the nsterrain_comp class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#include <component/nsterrain_comp.h>
#include <nsentity.h>

nsterrain_comp::nsterrain_comp() :
	nscomponent(type_to_hash(nsterrain_comp)),
	m_minmax(0.0f,1.0f)
{}

nsterrain_comp::nsterrain_comp(const nsterrain_comp & copy):
	nscomponent(copy),
	m_minmax(copy.m_minmax)
{}

nsterrain_comp::~nsterrain_comp()
{}

void nsterrain_comp::init()
{}

void nsterrain_comp::set_height_bounds(float min, float max)
{
	m_minmax.set(min, max);
}

void nsterrain_comp::set_height_bounds(const fvec2 & minmax_)
{
	m_minmax = minmax_;
}

const fvec2 & nsterrain_comp::height_bounds() const
{
	return m_minmax;
}


void nsterrain_comp::pup(nsfile_pupper * p)
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

nsterrain_comp & nsterrain_comp::operator=(nsterrain_comp rhs_)
{
	nscomponent::operator=(rhs_);
	std::swap(m_minmax, rhs_.m_minmax);
	post_update(true);
	return (*this);
}
