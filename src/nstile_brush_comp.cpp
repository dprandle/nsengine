/*!
\file nstilebrushcomp.cpp

\brief Definition file for nstile_brush_comp class

This file contains all of the neccessary definitions for the nstile_brush_comp class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/


#include <nstile_brush_comp.h>
#include <nsentity.h>

nstile_brush_comp::nstile_brush_comp() :
	nscomponent(),
	m_brush(),
	m_height(1)
{}


nstile_brush_comp::nstile_brush_comp(const nstile_brush_comp & copy):
	nscomponent(copy),
	m_brush(copy.m_brush),
	m_height(copy.m_height)
{}

nstile_brush_comp::~nstile_brush_comp()
{}

bool nstile_brush_comp::add(int32 x, int32 y)
{
	return add(ivec2(x, y));
}

bool nstile_brush_comp::add(const ivec2 & pGridSpace)
{
	if (contains(pGridSpace))
		return false;

	m_brush.push_back(pGridSpace);
	return true;
}

ivec2_vector::iterator nstile_brush_comp::begin()
{
	return m_brush.begin();
}

void nstile_brush_comp::change_height(const int32 & pAmount)
{
	m_height += pAmount;
}

bool nstile_brush_comp::contains(int32 x, int32 y)
{
	return contains(ivec2(x, y));
}

bool nstile_brush_comp::contains(const ivec2 & pGridSpace)
{
	for (uint32 i = 0; i < m_brush.size(); ++i)
	{
		if (pGridSpace == m_brush[i])
			return true;
	}
	return false;
}

ivec2_vector::iterator nstile_brush_comp::end()
{
	return m_brush.end();
}

void nstile_brush_comp::pup(nsfile_pupper * p)
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

int32 nstile_brush_comp::height() const
{
	return m_height;
}

void nstile_brush_comp::init()
{}

bool nstile_brush_comp::remove(int32 x, int32 y)
{
	return remove(ivec2(x, y));
}

/*!
Removes first occurance of pGridPos - should only be 1 occurance as the add function doesn't add if the grid space
that is being added already exists in the brush.
*/
bool nstile_brush_comp::remove(const ivec2 & pGridPos)
{
	auto iter = m_brush.begin();
	while (iter != m_brush.end())
	{
		if (*iter == pGridPos)
		{
			m_brush.erase(iter);
			return true;
		}
		++iter;
	}
	return false;
}

void nstile_brush_comp::set_height(const int32 & pHeight)
{
	m_height = pHeight;
}

nstile_brush_comp & nstile_brush_comp::operator=(nstile_brush_comp rhs_)
{
	nscomponent::operator=(rhs_);
	std::swap(m_brush, rhs_.m_brush);
	std::swap(m_height, rhs_.m_height);
	post_update(true);
	return (*this);
}
