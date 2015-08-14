/*!
\file nsoccupycomp.cpp

\brief Definition file for NSOccupyComp class

This file contains all of the neccessary definitions for the NSOccupyComp class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/


#include <nsoccupycomp.h>
#include <nsentity.h>
#include <nsplugin.h>
#include <cmath>

NSOccupyComp::NSOccupyComp() :
mDrawEnabled(false),
mMeshID(),
mMatID(),
NSComponent()
{
	add(0, 0, 0);
	NSMesh * occ = nsengine.engplug()->get<NSMesh>(MESH_FULL_TILE);
	if (occ == NULL)
	{
		dprint("NSOccupyComp::NSOccupyComp Could not get occupy mesh");
	}
	else
	{
		mMeshID = occ->fullid();
	}
}

NSOccupyComp::~NSOccupyComp()
{}

bool NSOccupyComp::add(int x, int y, int z)
{
	//if (contains(x, y, z))
		//return false;

	return add(ivec3(x, y, z));
}

bool NSOccupyComp::add(const ivec3 & pGridPos)
{
	mSpaces.push_back(pGridPos);
	return true;
}

ivec3array::iterator NSOccupyComp::begin()
{
	return mSpaces.begin();
}

/*!
This function builds the occupied spaces from the bounding box around the object, assuming that the
origin is the center of the object. This should be true for any object imported in to the engine.
This calculation is very basic and the actual tiles occupied should be refined - That is the algorithm
uses the bounding box of the entire mesh.
*/
void NSOccupyComp::build(const NSBoundingBox & pBox)
{
	nsint zCountUp = nsint(abs(pBox.mMax.z) / Z_GRID);
	if (fmod(abs(pBox.mMax.z), Z_GRID) > 0.01)
		++zCountUp;

	nsint zCountDown = nsint(abs(pBox.mMin.z) / Z_GRID);
	if (fmod(abs(pBox.mMin.z), Z_GRID) > 0.01)
		++zCountDown;

	nsint xCountRight = nsint(abs(pBox.mMax.x) / X_GRID);
	if (fmod(abs(pBox.mMax.x), X_GRID) > 0.01)
		++xCountRight;

	nsint xCountLeft = nsint(abs(pBox.mMin.x) / X_GRID);
	if (fmod(abs(pBox.mMin.x), X_GRID) > 0.01)
		++xCountLeft;

	nsint yCountForward = nsint(abs(pBox.mMax.y) / Y_GRID);
	if (fmod(abs(pBox.mMax.y), Y_GRID) > 0.01)
		++yCountForward;

	nsint yCountBackward = nsint(abs(pBox.mMin.y) / Y_GRID);
	if (fmod(abs(pBox.mMin.z), Y_GRID) > 0.01)
		++yCountBackward;



	for (nsint i = 0; i < zCountUp; ++i)
	{
		for (nsint j = 0; j < yCountForward; ++j)
		{
			nsint mod = j % 2;
			for (nsint k = 0; k < xCountRight; ++k)
			{
				nsint mod2 = k % 2;
				if (!mod) // if mod == 0 - basically if j is even
				{
					if (!mod2) // if k is even
						add(k/2, j, i);
				}
				else // if j is odd
				{
					if (mod2) // if k is odd
						add((k)/2, j, i);
				}
			}
			for (nsint k = 1; k < xCountLeft; ++k)
			{
				nsint mod2 = k % 2;
				if (!mod) // if mod == 0 - basically if j is even
				{
					if (!mod2) // if k is even
						add(-k/2, j, i);
				}
				else // if j is odd
				{
					if (mod2) // if k is odd
						add((-k-1)/2, j, i);
				}
			}
		}
		for (nsint j = 1; j < yCountBackward; ++j)
		{
			nsint mod = j % 2;
			for (nsint k = 0; k < xCountRight; ++k)
			{
				nsint mod2 = k % 2;
				if (!mod) // if mod == 0 - basically if j is even
				{
					if (!mod2) // if k is even
						add(k/2, -j, i);
				}
				else // if j is odd
				{
					if (mod2) // if k is odd
						add((k)/2, -j, i);
				}
			}
			for (nsint k = 1; k < xCountLeft; ++k)
			{
				nsint mod2 = k % 2;
				if (!mod) // if mod == 0 - basically if j is even
				{
					if (!mod2) // if k is even
						add(-k/2, -j, i);
				}
				else // if j is odd
				{
					if (mod2) // if k is odd
						add((-k-1)/2, -j, i);
				}
			}
		}
	}

	for (nsint i = 1; i < zCountDown; ++i)
	{
		for (nsint j = 0; j < yCountForward; ++j)
		{
			nsint mod = j % 2;
			for (nsint k = 0; k < xCountRight; ++k)
			{
				nsint mod2 = k % 2;
				if (!mod) // if mod == 0 - basically if j is even
				{
					if (!mod2) // if k is even
						add(k/2, j, -i);
				}
				else // if j is odd
				{
					if (mod2) // if k is odd
						add((k)/2, j, -i);
				}
			}
			for (nsint k = 1; k < xCountLeft; ++k)
			{
				nsint mod2 = k % 2;
				if (!mod) // if mod == 0 - basically if j is even
				{
					if (!mod2) // if k is even
						add(-k/2, j, -i);
				}
				else // if j is odd
				{
					if (mod2) // if k is odd
						add((-k-1)/2, j, -i);
				}
			}
		}
		for (nsint j = 1; j < yCountBackward; ++j)
		{
			nsint mod = j % 2;
			for (nsint k = 0; k < xCountRight; ++k)
			{
				nsint mod2 = k % 2;
				if (!mod) // if mod == 0 - basically if j is even
				{
					if (!mod2) // if k is even
						add(k/2, -j, -i);
				}
				else // if j is odd
				{
					if (mod2) // if k is odd
						add((k)/2, -j, -i);
				}
			}
			for (nsint k = 1; k < xCountLeft; ++k)
			{
				nsint mod2 = k % 2;
				if (!mod) // if mod == 0 - basically if j is even
				{
					if (!mod2) // if k is even
						add(-k/2, -j, -i);
				}
				else // if j is odd
				{
					if (mod2) // if k is odd
						add((-k-1)/2, -j, -i);
				}
			}
		}
	}
}

void NSOccupyComp::pup(NSFilePUPer * p)
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

bool NSOccupyComp::contains(int x, int y, int z)
{
	return contains(ivec3(x, y, z));
}

bool NSOccupyComp::contains(const ivec3 & pGridPos)
{
	auto iter = mSpaces.begin();
	while (iter != mSpaces.end())
	{
		if (*iter == pGridPos)
			return true;
		++iter;
	}
	return false;
}

NSOccupyComp* NSOccupyComp::copy(const NSComponent * pToCopy)
{
	if (pToCopy == NULL)
		return NULL;
	const NSOccupyComp * comp = (const NSOccupyComp*)pToCopy;
	(*this) = (*comp);
	return this;
}

void NSOccupyComp::enableDraw(bool pEnable)
{
	mDrawEnabled = pEnable;
}

ivec3array::iterator NSOccupyComp::end()
{
	return mSpaces.end();
}

const ivec3array & NSOccupyComp::spaces() const
{
	return mSpaces;
}

void NSOccupyComp::init()
{
}

bool NSOccupyComp::drawEnabled()
{
	return mDrawEnabled;
}

const uivec2 & NSOccupyComp::meshid()
{
	return mMeshID;
}

const uivec2 & NSOccupyComp::matid()
{
	return mMeshID;
}

void NSOccupyComp::nameChange(nsuint plugID, nsuint oldID, nsuint newID)
{
	if (mMeshID == uivec2(plugID, oldID))
		mMeshID.y = newID;
	if (mMatID == uivec2(plugID, oldID))
		mMatID.y = newID;
}

bool NSOccupyComp::remove(int x, int y, int z)
{
	return remove(ivec3(x,y,z));
}

bool NSOccupyComp::remove(const ivec3 & pGridPos)
{
	auto iter = mSpaces.begin();
	while (iter != mSpaces.end())
	{
		if (*iter == pGridPos)
		{
			iter = mSpaces.erase(iter);
			return true;
		}
		++iter;
	}
	return false;
}

uivec2array NSOccupyComp::resources()
{
	uivec2array ret;
	if (mMatID != 0)
		ret.push_back(mMatID);
	if (mMeshID != 0)
		ret.push_back(mMeshID);
	return ret;
}

void NSOccupyComp::setMeshID(const uivec2 & mesh)
{
	mMeshID = mesh;
	postUpdate(true);
}

void NSOccupyComp::setMatID(const uivec2 & mat)
{
	mMatID = mat;
	postUpdate(true);
}

NSOccupyComp & NSOccupyComp::operator=(const NSOccupyComp & pRHSComp)
{
	mSpaces.resize(pRHSComp.mSpaces.size());
	for (nsuint i = 0; i < mSpaces.size(); ++i)
		mSpaces[i] = pRHSComp.mSpaces[i];
	mMeshID = pRHSComp.mMeshID;
	mMatID = pRHSComp.mMatID;
	mDrawEnabled = pRHSComp.mDrawEnabled;
	postUpdate(true);
	return (*this);
}
