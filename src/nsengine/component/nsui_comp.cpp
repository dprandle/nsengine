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
{		
}

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

bool nsui_comp::is_child(nsentity * child)
{
	for (uint32 i = 0; i < m_children.size(); ++i)
	{
		if (m_children[i] == child->full_id())
			return true;
	}
	return false;
}

void nsui_comp::add_child(nsentity * child)
{
	if (is_child(child) || !child->has<nsui_comp>())
		return;
	
	nsui_comp * chld_ui = child->get<nsui_comp>();
	nsentity * childs_parent = get_resource<nsentity>(chld_ui->m_parent);
	if (childs_parent != nullptr)
	{
		nsui_comp * uic = childs_parent->get<nsui_comp>();
		if (uic != nullptr)
			uic->remove_child(child);
	}
		
	m_children.push_back(child->full_id());
	child->get<nsui_comp>()->m_parent = m_owner->full_id();
}

void nsui_comp::remove_child(nsentity * child)
{
	auto iter = m_children.begin();
	while (iter != m_children.end())
	{
		if (*iter == child->full_id())
		{
			nsui_comp * uic = child->get<nsui_comp>();
			if (uic != nullptr)
				uic->m_parent = uivec2();
			m_children.erase(iter);
			return;
		}
		++iter;
	}
}

void nsui_comp::set_parent(nsentity * parent)
{
	nsui_comp * uic = parent->get<nsui_comp>();
	if (uic == nullptr)
		return;
	uic->add_child(m_owner);
}
const uivec2 & nsui_comp::parent()
{
	return m_parent;
}
