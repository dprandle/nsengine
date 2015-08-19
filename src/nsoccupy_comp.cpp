/*!
\file nsoccupycomp.cpp

\brief Definition file for NSOccupyComp class

This file contains all of the neccessary definitions for the NSOccupyComp class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/


#include <nsoccupy_comp.h>
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
	nsmesh * occ = nsengine.core()->get<nsmesh>(MESH_FULL_TILE);
	if (occ == NULL)
	{
		dprint("NSOccupyComp::NSOccupyComp Could not get occupy mesh");
	}
	else
	{
		mMeshID = occ->full_id();
	}
}

NSOccupyComp::~NSOccupyComp()
{}

bool NSOccupyComp::add(int32 x, int32 y, int32 z)
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
void NSOccupyComp::build(const nsbounding_box & pBox)
{
	int32 zCountUp = int32(abs(pBox.mMax.z) / Z_GRID);
	if (fmod(abs(pBox.mMax.z), Z_GRID) > 0.01)
		++zCountUp;

	int32 zCountDown = int32(abs(pBox.mMin.z) / Z_GRID);
	if (fmod(abs(pBox.mMin.z), Z_GRID) > 0.01)
		++zCountDown;

	int32 xCountRight = int32(abs(pBox.mMax.x) / X_GRID);
	if (fmod(abs(pBox.mMax.x), X_GRID) > 0.01)
		++xCountRight;

	int32 xCountLeft = int32(abs(pBox.mMin.x) / X_GRID);
	if (fmod(abs(pBox.mMin.x), X_GRID) > 0.01)
		++xCountLeft;

	int32 yCountForward = int32(abs(pBox.mMax.y) / Y_GRID);
	if (fmod(abs(pBox.mMax.y), Y_GRID) > 0.01)
		++yCountForward;

	int32 yCountBackward = int32(abs(pBox.mMin.y) / Y_GRID);
	if (fmod(abs(pBox.mMin.z), Y_GRID) > 0.01)
		++yCountBackward;



	for (int32 i = 0; i < zCountUp; ++i)
	{
		for (int32 j = 0; j < yCountForward; ++j)
		{
			int32 mod = j % 2;
			for (int32 k = 0; k < xCountRight; ++k)
			{
				int32 mod2 = k % 2;
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
			for (int32 k = 1; k < xCountLeft; ++k)
			{
				int32 mod2 = k % 2;
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
		for (int32 j = 1; j < yCountBackward; ++j)
		{
			int32 mod = j % 2;
			for (int32 k = 0; k < xCountRight; ++k)
			{
				int32 mod2 = k % 2;
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
			for (int32 k = 1; k < xCountLeft; ++k)
			{
				int32 mod2 = k % 2;
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

	for (int32 i = 1; i < zCountDown; ++i)
	{
		for (int32 j = 0; j < yCountForward; ++j)
		{
			int32 mod = j % 2;
			for (int32 k = 0; k < xCountRight; ++k)
			{
				int32 mod2 = k % 2;
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
			for (int32 k = 1; k < xCountLeft; ++k)
			{
				int32 mod2 = k % 2;
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
		for (int32 j = 1; j < yCountBackward; ++j)
		{
			int32 mod = j % 2;
			for (int32 k = 0; k < xCountRight; ++k)
			{
				int32 mod2 = k % 2;
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
			for (int32 k = 1; k < xCountLeft; ++k)
			{
				int32 mod2 = k % 2;
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

void NSOccupyComp::pup(nsfile_pupper * p)
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

bool NSOccupyComp::contains(int32 x, int32 y, int32 z)
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

void NSOccupyComp::name_change(uint32 plugID, uint32 oldID, uint32 newID)
{
	if (mMeshID == uivec2(plugID, oldID))
		mMeshID.y = newID;
	if (mMatID == uivec2(plugID, oldID))
		mMatID.y = newID;
}

bool NSOccupyComp::remove(int32 x, int32 y, int32 z)
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
	post_update(true);
}

void NSOccupyComp::setMatID(const uivec2 & mat)
{
	mMatID = mat;
	post_update(true);
}

NSOccupyComp & NSOccupyComp::operator=(const NSOccupyComp & pRHSComp)
{
	mSpaces.resize(pRHSComp.mSpaces.size());
	for (uint32 i = 0; i < mSpaces.size(); ++i)
		mSpaces[i] = pRHSComp.mSpaces[i];
	mMeshID = pRHSComp.mMeshID;
	mMatID = pRHSComp.mMatID;
	mDrawEnabled = pRHSComp.mDrawEnabled;
	post_update(true);
	return (*this);
}
