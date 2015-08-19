/*!
\file nsinputcomp.cpp

\brief Definition file for NSInputComp class

This file contains all of the neccessary definitions for the NSInputComp class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/


#include <nsinput_comp.h>
#include <nsentity.h>

NSInputComp::NSInputComp() :NSComponent()
{}

NSInputComp::~NSInputComp()
{}

bool NSInputComp::add(const nsstring & pTriggerName)
{
	auto insert = mActions.emplace(pTriggerName, Action());
	return insert.second;
}

void NSInputComp::clear()
{
	mActions.clear();
}

NSInputComp* NSInputComp::copy(const NSComponent * pToCopy)
{
	if (pToCopy == NULL)
		return NULL;
	const NSInputComp * comp = (const NSInputComp*)pToCopy;
	(*this) = (*comp);
	return this;
}

bool NSInputComp::contains(const nsstring & pTriggerName)
{
	return (mActions.find(pTriggerName) != mActions.end());
}

void NSInputComp::init()
{}

NSInputComp::Action * NSInputComp::action(const nsstring & pTriggerName)
{
	auto iter = mActions.find(pTriggerName);
	if (iter == mActions.end())
		return NULL;
	return &iter->second;
}

bool NSInputComp::remove(const nsstring & pTriggerName)
{
	size_t erased = mActions.erase(pTriggerName);
	return erased && 1;
}

void NSInputComp::setActivated(const nsstring & pTriggerName, bool pActivate)
{
	auto iter = mActions.find(pTriggerName);
	if (iter != mActions.end())
		iter->second.mActivated = pActivate;
}

void NSInputComp::setActivated(bool pActivate)
{
	auto iter = mActions.begin();
	while (iter != mActions.end())
	{
		iter->second.mActivated = pActivate;
		++iter;
	}
}

void NSInputComp::pup(nsfile_pupper * p)
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

NSInputComp & NSInputComp::operator=(const NSInputComp & pRHSComp)
{
	clear();
	auto iter = pRHSComp.mActions.begin();
	while (iter != pRHSComp.mActions.end())
	{
		add(iter->first);
		++iter;
	}
	post_update(true);
	return (*this);
}
