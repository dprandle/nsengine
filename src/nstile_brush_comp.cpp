/*!
\file nstilebrushcomp.cpp

\brief Definition file for NSTileBrushComp class

This file contains all of the neccessary definitions for the NSTileBrushComp class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/


#include <nstile_brush_comp.h>
#include <nsentity.h>

NSTileBrushComp::NSTileBrushComp() :
mBrush(),
mHeight(1),
NSComponent()
{}

NSTileBrushComp::~NSTileBrushComp()
{}

bool NSTileBrushComp::add(int32 x, int32 y)
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

void NSTileBrushComp::changeHeight(const int32 & pAmount)
{
	mHeight += pAmount;
}

bool NSTileBrushComp::contains(int32 x, int32 y)
{
	return contains(ivec2(x, y));
}

bool NSTileBrushComp::contains(const ivec2 & pGridSpace)
{
	for (uint32 i = 0; i < mBrush.size(); ++i)
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

void NSTileBrushComp::pup(nsfile_pupper * p)
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

int32 NSTileBrushComp::height() const
{
	return mHeight;
}

void NSTileBrushComp::init()
{}

bool NSTileBrushComp::remove(int32 x, int32 y)
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

void NSTileBrushComp::setHeight(const int32 & pHeight)
{
	mHeight = pHeight;
}

NSTileBrushComp & NSTileBrushComp::operator=(const NSTileBrushComp & pRHSComp)
{
	mHeight = pRHSComp.mHeight;
	mBrush.resize(pRHSComp.mBrush.size());
	for (uint32 i = 0; i < mBrush.size(); ++i)
		mBrush[i] = pRHSComp.mBrush[i];

	post_update(true);
	return (*this);
}
