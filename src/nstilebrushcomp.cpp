/*!
\file nstilebrushcomp.cpp

\brief Definition file for NSTileBrushComp class

This file contains all of the neccessary definitions for the NSTileBrushComp class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/


#include <nstilebrushcomp.h>
#include <nsentity.h>

NSTileBrushComp::NSTileBrushComp() :
mBrush(),
mHeight(1),
NSComponent()
{}

NSTileBrushComp::~NSTileBrushComp()
{}

bool NSTileBrushComp::add(int x, int y)
{
	return add(ivec2(x, y));
}

bool NSTileBrushComp::add(const ivec2 & pGridSpace)
{
	if (contains(pGridSpace))
		return false;

	mBrush.push_back(pGridSpace);
	return true;
}

ivec2array::iterator NSTileBrushComp::begin()
{
	return mBrush.begin();
}

void NSTileBrushComp::changeHeight(const nsint & pAmount)
{
	mHeight += pAmount;
}

bool NSTileBrushComp::contains(int x, int y)
{
	return contains(ivec2(x, y));
}

bool NSTileBrushComp::contains(const ivec2 & pGridSpace)
{
	for (nsuint i = 0; i < mBrush.size(); ++i)
	{
		if (pGridSpace == mBrush[i])
			return true;
	}
	return false;
}

NSTileBrushComp* NSTileBrushComp::copy(const NSComponent * pToCopy)
{
	if (pToCopy == NULL)
		return NULL;
	const NSTileBrushComp * comp = (const NSTileBrushComp*)pToCopy;
	(*this) = (*comp);
	return this;
}

ivec2array::iterator NSTileBrushComp::end()
{
	return mBrush.end();
}

void NSTileBrushComp::pup(NSFilePUPer * p)
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

nsint NSTileBrushComp::height() const
{
	return mHeight;
}

void NSTileBrushComp::init()
{}

bool NSTileBrushComp::remove(int x, int y)
{
	return remove(ivec2(x, y));
}

/*!
Removes first occurance of pGridPos - should only be 1 occurance as the add function doesn't add if the grid space
that is being added already exists in the brush.
*/
bool NSTileBrushComp::remove(const ivec2 & pGridPos)
{
	auto iter = mBrush.begin();
	while (iter != mBrush.end())
	{
		if (*iter == pGridPos)
		{
			mBrush.erase(iter);
			return true;
		}
		++iter;
	}
	return false;
}

void NSTileBrushComp::setHeight(const nsint & pHeight)
{
	mHeight = pHeight;
}

NSTileBrushComp & NSTileBrushComp::operator=(const NSTileBrushComp & pRHSComp)
{
	mHeight = pRHSComp.mHeight;
	mBrush.resize(pRHSComp.mBrush.size());
	for (nsuint i = 0; i < mBrush.size(); ++i)
		mBrush[i] = pRHSComp.mBrush[i];

	postUpdate(true);
	return (*this);
}

nsstring NSTileBrushComp::getTypeString()
{
	return TILEBRUSH_COMP_TYPESTRING;
}