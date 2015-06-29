/*!
\file nsinputcomp.cpp

\brief Definition file for NSInputComp class

This file contains all of the neccessary definitions for the NSInputComp class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/


#include <nsinputcomp.h>
#include <nsentity.h>

NSInputComp::NSInputComp() :NSComponent()
{}

NSInputComp::~NSInputComp()
{}

nsbool NSInputComp::add(const nsstring & pTriggerName)
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

nsbool NSInputComp::contains(const nsstring & pTriggerName)
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

nsbool NSInputComp::remove(const nsstring & pTriggerName)
{
	nsuint erased = mActions.erase(pTriggerName);
	return erased && 1;
}

void NSInputComp::setActivated(const nsstring & pTriggerName, nsbool pActivate)
{
	auto iter = mActions.find(pTriggerName);
	if (iter != mActions.end())
		iter->second.mActivated = pActivate;
}

void NSInputComp::setActivated(nsbool pActivate)
{
	auto iter = mActions.begin();
	while (iter != mActions.end())
	{
		iter->second.mActivated = pActivate;
		++iter;
	}
}

void NSInputComp::pup(NSFilePUPer * p)
{
	if (p->type() == NSFilePUPer::Binary)
	{
		NSBinFilePUPer * bf = static_cast<NSBinFilePUPer *>(p);
		::pup(*bf, *this);
	}
	else
	{
		NSTextFilePUPer * tf = static_cast<NSTextFilePUPer *>(p);
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
	postUpdate(true);
	return (*this);
}

nsstring NSInputComp::getTypeString()
{
	return INPUT_COMP_TYPESTRING;
}

