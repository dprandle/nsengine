/*!
\file nstilegrid.cpp

\brief Definition file for NSTileGrid class

This file contains all of the neccessary definitions for the NSTileGrid class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#include <nstile_grid.h>
#include <nstform_comp.h>

NSTileGrid::NSTileGrid()
{
	mMap.resize(8);

	for (uint32 i = 0; i < QUADRANT_COUNT; ++i)
	{
		mMap[i].resize(64);
		for (uint32 j = 0; j < 64; ++j)
		{
			mMap[i][j].resize(64);
			for (uint32 k = 0; k < 64; ++k)
				mMap[i][j][k].resize(64);
		}
	}
}

NSTileGrid::~NSTileGrid()
{}

bool NSTileGrid::add(const uivec3 & pItem, const fvec3 & pPos)
{
	return add(pItem, ivec3(), pPos);
}

bool NSTileGrid::add(const uivec3 & pItem, const ivec3 & pSpace, const fvec3 & pOrigin)
{
	if (occupied(pSpace, pOrigin))
		return false;

	ivec3 space = pSpace + grid(pOrigin);
	Index ind = index(space);

	// If space is out of bounds, allocate more memory to accomadate
	if (!_checkBounds(ind))
		_resizeForSpace(ind);

	mMap[ind.mQuad][ind.mIndex.z][ind.mIndex.y][ind.mIndex.x] = pItem;
	return true;
}

bool NSTileGrid::add(const uivec3 pItem, const ivec3array & pSpaces, const fvec3 & pOrigin)
{
	for (uint32 i = 0; i < pSpaces.size(); ++i)
	{
		if (!add(pItem, pSpaces[i], pOrigin))
			return false;
	}
	return true;
}

const uivec3 & NSTileGrid::at(const Index & pSpace) const
{
	if (!_checkBounds(pSpace))
		throw(std::exception());

	return mMap[pSpace.mQuad][pSpace.mIndex.z][pSpace.mIndex.y][pSpace.mIndex.x];
}

uivec3 NSTileGrid::get(const fvec3 & pPos) const
{
	return get(ivec3(), pPos);
}

uivec3 NSTileGrid::get(const ivec3 & pSpace, const fvec3 & pOrigin) const
{
	if (!occupied(pSpace, pOrigin)) // also will check bounds here and return false if out of bounds
		return uivec3();

	ivec3 space = pSpace + grid(pOrigin);
	Index ind = index(space);

	// safe to raw index access because we know the indices are in bounds of the quadrant
	return mMap[ind.mQuad][ind.mIndex.z][ind.mIndex.y][ind.mIndex.x];
}

int32 NSTileGrid::minLayer()
{
	int32 minVal = 0;
	for (uint32 i = 3; i < 8; ++i)
	{
		int32 size = static_cast<int32>(mMap[i].size());
		size *= -1;
		if (size < minVal)
			minVal = size;
	}
	return minVal;
}

int32 NSTileGrid::maxLayer()
{
	int32 maxVal = 0;
	for (uint32 i = 0; i < 4; ++i)
	{
		int32 size = static_cast<int32>(mMap[i].size());
		size -= 1;
		if (size > maxVal)
			maxVal = size;
	}
	return maxVal;
}

int32 NSTileGrid::minY()
{
	int32 minY = 0;

	for (uint32 i = 0; i < mMap[2].size(); ++i)
	{
		int32 lowestIndex = static_cast<int32>(mMap[2][i].size()) * -1;
		if (lowestIndex < minY)
			minY = lowestIndex;
	}

	for (uint32 i = 0; i < mMap[3].size(); ++i)
	{
		int32 lowestIndex = static_cast<int32>(mMap[3][i].size()) * -1;
		if (lowestIndex < minY)
			minY = lowestIndex;
	}

	for (uint32 i = 0; i < mMap[6].size(); ++i)
	{
		int32 lowestIndex = static_cast<int32>(mMap[6][i].size()) * -1;
		if (lowestIndex < minY)
			minY = lowestIndex;
	}

	for (uint32 i = 0; i < mMap[7].size(); ++i)
	{
		int32 lowestIndex = static_cast<int32>(mMap[7][i].size()) * -1;
		if (lowestIndex < minY)
			minY = lowestIndex;
	}
	return minY;
}

int32 NSTileGrid::maxY()
{
	int32 maxY = 0;

	for (uint32 i = 0; i < mMap[0].size(); ++i)
	{
		int32 highestIndex = static_cast<int32>(mMap[0][i].size()) - 1;
		if (highestIndex > maxY)
			maxY = highestIndex;
	}

	for (uint32 i = 0; i < mMap[1].size(); ++i)
	{
		int32 highestIndex = static_cast<int32>(mMap[1][i].size()) - 1;
		if (highestIndex > maxY)
			maxY = highestIndex;
	}

	for (uint32 i = 0; i < mMap[4].size(); ++i)
	{
		int32 highestIndex = static_cast<int32>(mMap[4][i].size()) - 1;
		if (highestIndex > maxY)
			maxY = highestIndex;
	}

	for (uint32 i = 0; i < mMap[5].size(); ++i)
	{
		int32 highestIndex = static_cast<int32>(mMap[5][i].size()) - 1;
		if (highestIndex > maxY)
			maxY = highestIndex;
	}
	return maxY;
}

int32 NSTileGrid::minX()
{
	int32 minX = 0;

	for (uint32 i = 0; i < mMap[1].size(); ++i)
	{
		for (uint32 j = 0; j < mMap[1][i].size(); ++j)
		{
			int32 lowestIndex = static_cast<int32>(mMap[1][i][j].size()) * -1;
			if (lowestIndex < minX)
				minX = lowestIndex;
		}
	}

	for (uint32 i = 0; i < mMap[3].size(); ++i)
	{
		for (uint32 j = 0; j < mMap[3][i].size(); ++j)
		{
			int32 lowestIndex = static_cast<int32>(mMap[3][i][j].size()) * -1;
			if (lowestIndex < minX)
				minX = lowestIndex;
		}
	}

	for (uint32 i = 0; i < mMap[5].size(); ++i)
	{
		for (uint32 j = 0; j < mMap[5][i].size(); ++j)
		{
			int32 lowestIndex = static_cast<int32>(mMap[5][i][j].size()) * -1;
			if (lowestIndex < minX)
				minX = lowestIndex;
		}
	}

	for (uint32 i = 0; i < mMap[7].size(); ++i)
	{
		for (uint32 j = 0; j < mMap[7][i].size(); ++j)
		{
			int32 lowestIndex = static_cast<int32>(mMap[7][i][j].size()) * -1;
			if (lowestIndex < minX)
				minX = lowestIndex;
		}
	}
	return minX;
}

int32 NSTileGrid::maxX()
{
	int32 maxX = 0;

	for (uint32 i = 0; i < mMap[0].size(); ++i)
	{
		for (uint32 j = 0; j < mMap[0][i].size(); ++j)
		{
			int32 highestIndex = static_cast<int32>(mMap[0][i][j].size()) - 1;
			if (highestIndex > maxX)
				maxX = highestIndex;
		}
	}

	for (uint32 i = 0; i < mMap[2].size(); ++i)
	{
		for (uint32 j = 0; j < mMap[2][i].size(); ++j)
		{
			int32 highestIndex = static_cast<int32>(mMap[2][i][j].size()) - 1;
			if (highestIndex > maxX)
				maxX = highestIndex;
		}
	}

	for (uint32 i = 0; i < mMap[4].size(); ++i)
	{
		for (uint32 j = 0; j < mMap[4][i].size(); ++j)
		{
			int32 highestIndex = static_cast<int32>(mMap[4][i][j].size()) - 1;
			if (highestIndex > maxX)
				maxX = highestIndex;
		}
	}

	for (uint32 i = 0; i < mMap[6].size(); ++i)
	{
		for (uint32 j = 0; j < mMap[6][i].size(); ++j)
		{
			int32 highestIndex = static_cast<int32>(mMap[6][i][j].size()) - 1;
			if (highestIndex > maxX)
				maxX = highestIndex;
		}
	}
	return maxX;
}

NSTileGrid::GridBounds NSTileGrid::occupiedGridBounds()
{
	GridBounds g;
	for (uint32 i = 0; i < 8; ++i)
	{
		for (uint32 z = 0; z < mMap[i].size(); ++z)
		{
			for (uint32 y = 0; y < mMap[i][z].size(); ++y)
			{
				for (uint32 x = 0; x < mMap[i][z][y].size(); ++x)
				{
					if (mMap[i][z][y][x] != uivec3())
					{
						ivec3 gridPos = grid(Index(i, x, y, z));
						if (gridPos.x > g.maxSpace.x)
							g.maxSpace.x = gridPos.x;
						if (gridPos.y > g.maxSpace.y)
							g.maxSpace.y = gridPos.y;
						if (gridPos.z > g.maxSpace.z)
							g.maxSpace.z = gridPos.z;

						if (gridPos.x < g.minSpace.x)
							g.minSpace.x = gridPos.x;
						if (gridPos.y < g.minSpace.y)
							g.minSpace.y = gridPos.y;
						if (gridPos.z < g.minSpace.z)
							g.minSpace.z = gridPos.z;
					}
				}
			}
		}
	}
	return g;
}

bool NSTileGrid::occupied(const fvec3 & pPos) const
{
	return occupied(ivec3(), pPos);
}

bool NSTileGrid::occupied(const ivec3 & pSpace, const fvec3 & pOrigin) const
{
	ivec3 space = pSpace + grid(pOrigin);
	Index ind = index(space);

	if (!_checkBounds(ind))
		return false;

	return (mMap[ind.mQuad][ind.mIndex.z][ind.mIndex.y][ind.mIndex.x] != uivec3());
}

bool NSTileGrid::occupied(const ivec3array & pSpaces, const fvec3 & pOrigin) const
{
	for (uint32 i = 0; i < pSpaces.size(); ++i)
	{
		if (occupied(pSpaces[i], pOrigin))
			return true;
	}
	return false;
}

bool NSTileGrid::remove(const fvec3 & pPos)
{
	return remove(ivec3(), pPos);
}

bool NSTileGrid::remove(const ivec3 & pSpace, const fvec3 & pOrigin)
{
	if (!occupied(pSpace, pOrigin))
		return false;

	ivec3 space = pSpace + grid(pOrigin);
	Index ind = index(space);

	// Set the space to (0,0) which signifies no item
	mMap[ind.mQuad][ind.mIndex.z][ind.mIndex.y][ind.mIndex.x] = uivec3();
	return true;
}

/*!
Go through entire grid and remove any occurances that have this entity ID
This is fairly slow so should not be used on a per-frame basis
*/
void NSTileGrid::remove(const uint32 plugid)
{
	// Go through entire grid and remove any entrees with entity ID equal to above
	GridBounds g;
	for (uint32 i = 0; i < 8; ++i)
	{
		for (uint32 z = 0; z < mMap[i].size(); ++z)
		{
			for (uint32 y = 0; y < mMap[i][z].size(); ++y)
			{
				for (uint32 x = 0; x < mMap[i][z][y].size(); ++x)
				{
					if (mMap[i][z][y][x].x == plugid)
						mMap[i][z][y][x] = uivec3();
				}
			}
		}
	}
}

/*!
Remove multiple tiles from the grid - each space will be shifted by the grid position of the origin
*/
bool NSTileGrid::remove(const ivec3array & pSpaces, const fvec3 & pOrigin)
{
	for (uint32 i = 0; i < pSpaces.size(); ++i)
	{
		if (!remove(pSpaces[i], pOrigin))
			return false;
	}
	return true;
}

/*!
Go through entire grid and remove any occurances that have this transform ID
*/
void NSTileGrid::remove(const uivec2 & id)
{
	// Go through entire grid and remove any entrees with transform ID equal to pTFormID
	GridBounds g;
	for (uint32 i = 0; i < 8; ++i)
	{
		for (uint32 z = 0; z < mMap[i].size(); ++z)
		{
			for (uint32 y = 0; y < mMap[i][z].size(); ++y)
			{
				for (uint32 x = 0; x < mMap[i][z][y].size(); ++x)
				{
					if (mMap[i][z][y][x].x == id.x && mMap[i][z][y][x].y == id.y)
						mMap[i][z][y][x] = uivec3();
				}
			}
		}
	}
}

void NSTileGrid::name_change(const uivec2 & oldid, const uivec2 newid)
{
	// Go through entire grid and remove any entrees with entity ID equal to above
	GridBounds g;
	for (uint32 i = 0; i < 8; ++i)
	{
		for (uint32 z = 0; z < mMap[i].size(); ++z)
		{
			for (uint32 y = 0; y < mMap[i][z].size(); ++y)
			{
				for (uint32 x = 0; x < mMap[i][z][y].size(); ++x)
				{
					if (mMap[i][z][y][x].x == oldid.x)
					{
						mMap[i][z][y][x].x = newid.x;
						if (mMap[i][z][y][x].y == oldid.y)
							mMap[i][z][y][x].y = newid.y;
					}
				}
			}
		}
	}
}


uivec3array NSTileGrid::boundedSet(const fvec3 & pPoint1, const fvec3 & pPoint2)
{
	uivec3array retSet;
	fvec3 min(pPoint1);
	fvec3 max(pPoint2);

	// Make sure we get the minimum and maximum points so that we can loop through
	// from the min to the max without having an infinite loop
	if (min.z > max.z)
	{
		max.z = min.z;
		min.z = pPoint2.z;
	}
	if (min.y > max.y)
	{
		max.y = min.y;
		min.y = pPoint2.y;
	}
	if (min.x > max.x)
	{
		max.x = min.x;
		min.x = pPoint2.x;
	}

	ivec3 minGrid = grid(min);
	ivec3 maxGrid = grid(max);

	// Now loop through the grid and check if occupied - if occupied then add
	// the id to the id set
	for (; minGrid.z <= maxGrid.z; ++minGrid.z)
	{
		for (; minGrid.y <= maxGrid.y; ++minGrid.y)
		{
			for (; minGrid.x <= maxGrid.x; ++minGrid.x)
			{
				Index ind = index(minGrid);
				const uivec3 & id = _getID(ind);
				if (id == 0)
					continue;
				retSet.push_back(id);
			}
		}
	}
	return retSet;
}

int32 NSTileGrid::indexX(float pX, bool pOffset)
{
	if (pOffset)
		return int32(floor(((pX - X_GRID) / (2 * X_GRID)) + ROUND_FACTOR));

	return int32(floor((pX / (2 * X_GRID)) + ROUND_FACTOR));
}

int32 NSTileGrid::indexY(float pY)
{
	return int32(floor((pY / Y_GRID) + ROUND_FACTOR));
}

int32 NSTileGrid::indexZ(float pZ)
{
	return int32(floor((pZ / Z_GRID) + ROUND_FACTOR));
}

NSTileGrid::Index NSTileGrid::index(const ivec3 & pSpace)
{
	Index index;
	if (pSpace.z < 0)
	{
		index.mQuad = QuadrantIndex(uint32(index.mQuad) + 4);
		index.mIndex.z = -1 * pSpace.z;
		index.mIndex.z -= 1;
	}
	else
		index.mIndex.z = pSpace.z;
	if (pSpace.y < 0)
	{
		index.mQuad = QuadrantIndex(uint32(index.mQuad) + 2);
		index.mIndex.y = -1 * pSpace.y;
		index.mIndex.y -= 1;
	}
	else
		index.mIndex.y = pSpace.y;
	if (pSpace.x < 0)
	{
		index.mQuad = QuadrantIndex(uint32(index.mQuad) + 1);
		index.mIndex.x = -1 * pSpace.x;
		index.mIndex.x -= 1;
	}
	else
		index.mIndex.x = pSpace.x;
	return index;
}

fvec3 NSTileGrid::world(const ivec3 & pSpace, const fvec3 & pOrigin)
{
	ivec3 space = pSpace + grid(pOrigin);
	fvec3 pos(space.x * 2.0f * X_GRID, space.y * Y_GRID, space.z * Z_GRID);
	if (space.y % 2 != 0)
		pos.x += X_GRID;
	return pos;
}

ivec3 NSTileGrid::grid(const Index & pIndex)
{
	ivec3 grid;

	if (pIndex.mQuad < 4)
		grid.z = pIndex.mIndex.z;
	else
		grid.z = -1 * (pIndex.mIndex.z + 1);

	if (pIndex.mQuad % 4 < 2)
		grid.y = pIndex.mIndex.y;
	else
		grid.y = -1 * (pIndex.mIndex.y + 1);

	if (pIndex.mQuad % 2 == 0)
		grid.x = pIndex.mIndex.x;
	else
		grid.x = -1 * (pIndex.mIndex.x + 1);

	return grid;
}

fvec3 NSTileGrid::world(const Index & pIndex)
{
	return world(grid(pIndex));
}

void NSTileGrid::snap(fvec3 & pToSnap)
{
	pToSnap = world(grid(pToSnap));
}

NSTileGrid::Index NSTileGrid::index(const fvec3 & pWorldPos)
{
	return index(grid(pWorldPos));
}

ivec3 NSTileGrid::grid(const fvec3 & pWorldPos)
{
	ivec3 gPos;

	gPos.y = indexY(pWorldPos.y);
	bool offset = (gPos.y % 2 != 0);
	gPos.x = indexX(pWorldPos.x, offset);
	gPos.z = indexZ(pWorldPos.z);
	return gPos;
}

bool NSTileGrid::_checkBounds(const Index & pIndex) const
{
	if (pIndex.mQuad >= QUADRANT_COUNT)
		return false;

	if (pIndex.mIndex.z >= mMap[pIndex.mQuad].size())
		return false;

	if (pIndex.mIndex.y >= mMap[pIndex.mQuad][pIndex.mIndex.z].size())
		return false;

	if (pIndex.mIndex.x >= mMap[pIndex.mQuad][pIndex.mIndex.z][pIndex.mIndex.y].size())
		return false;

	return true;
}

void NSTileGrid::_resizeForSpace(const Index & pIndex)
{
	uint32 lastIndex = 0;

	if (pIndex.mIndex.z >= mMap[pIndex.mQuad].size())
	{
		lastIndex = static_cast<int32>(mMap[pIndex.mQuad].size()) - 1;
		mMap[pIndex.mQuad].resize(pIndex.mIndex.z + TILE_GRID_RESIZE_PAD);

		//resize the x and y dimensions for all new layers
		for (uint32 i = lastIndex; i < mMap[pIndex.mQuad].size(); ++i)
		{
			mMap[pIndex.mQuad][i].resize(DEFAULT_GRID_SIZE);
			for (uint32 j = 0; j < DEFAULT_GRID_SIZE; ++j)
				mMap[pIndex.mQuad][i][j].resize(DEFAULT_GRID_SIZE);
		}
	}

	if (pIndex.mIndex.y >= mMap[pIndex.mQuad][pIndex.mIndex.z].size())
	{
		lastIndex = static_cast<int32>(mMap[pIndex.mQuad][pIndex.mIndex.z].size()) - 1;
		mMap[pIndex.mQuad][pIndex.mIndex.z].resize(pIndex.mIndex.y + TILE_GRID_RESIZE_PAD);

		// resize all the x dimensions for the current layer/row
		for (uint32 i = lastIndex; i < mMap[pIndex.mQuad][pIndex.mIndex.z].size(); ++i)
			mMap[pIndex.mQuad][pIndex.mIndex.z][i].resize(DEFAULT_GRID_SIZE);
	}

	if (pIndex.mIndex.x >= mMap[pIndex.mQuad][pIndex.mIndex.z][pIndex.mIndex.y].size())
		mMap[pIndex.mQuad][pIndex.mIndex.z][pIndex.mIndex.y].resize(pIndex.mIndex.x + TILE_GRID_RESIZE_PAD);
}

const uivec3 & NSTileGrid::_getID(const Index & pIndex)
{
	return mMap[pIndex.mQuad][pIndex.mIndex.z][pIndex.mIndex.y][pIndex.mIndex.x];
}
