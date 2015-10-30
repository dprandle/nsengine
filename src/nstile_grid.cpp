/*!
\file nstilegrid.cpp

\brief Definition file for nstile_grid class

This file contains all of the neccessary definitions for the nstile_grid class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#include <nstile_grid.h>
#include <nstform_comp.h>

nstile_grid::nstile_grid()
{
	m_world_map.resize(8);

	for (uint32 i = 0; i < QUADRANT_COUNT; ++i)
	{
		m_world_map[i].resize(64);
		for (uint32 j = 0; j < 64; ++j)
		{
			m_world_map[i][j].resize(64);
			for (uint32 k = 0; k < 64; ++k)
				m_world_map[i][j][k].resize(64);
		}
	}
}

nstile_grid::~nstile_grid()
{}

bool nstile_grid::add(const uivec3 & pItem, const fvec3 & pPos)
{
	return add(pItem, ivec3(), pPos);
}

bool nstile_grid::add(const uivec3 & pItem, const ivec3 & pSpace, const fvec3 & pOrigin)
{
	if (occupied(pSpace, pOrigin))
		return false;

	ivec3 space = pSpace + grid(pOrigin);
	map_index ind = index(space);

	// If space is out of bounds, allocate more memory to accomadate
	if (!_check_bounds(ind))
		_resize_for_space(ind);

	m_world_map[ind.quad_index][ind.raw_index.z][ind.raw_index.y][ind.raw_index.x] = pItem;
	return true;
}

bool nstile_grid::add(const uivec3 pItem, const ivec3_vector & pSpaces, const fvec3 & pOrigin)
{
	for (uint32 i = 0; i < pSpaces.size(); ++i)
	{
		// If any of the spaces fail we need to go back and remove all previously added spaces
		if (!add(pItem, pSpaces[i], pOrigin))
		{
            for (uint32 j = 0; j < i; ++j)
				remove(pSpaces[j],pOrigin);
			return false;
		}
	}
	return true;
}

const uivec3 & nstile_grid::at(const map_index & pSpace) const
{
	if (!_check_bounds(pSpace))
		throw(std::exception());

	return m_world_map[pSpace.quad_index][pSpace.raw_index.z][pSpace.raw_index.y][pSpace.raw_index.x];
}

uivec3 nstile_grid::get(const fvec3 & pPos) const
{
	return get(ivec3(), pPos);
}

uivec3 nstile_grid::get(const ivec3 & pSpace, const fvec3 & pOrigin) const
{
	if (!occupied(pSpace, pOrigin)) // also will check bounds here and return false if out of bounds
		return uivec3();

	ivec3 space = pSpace + grid(pOrigin);
	map_index ind = index(space);

	// safe to raw index access because we know the indices are in bounds of the quadrant
	return m_world_map[ind.quad_index][ind.raw_index.z][ind.raw_index.y][ind.raw_index.x];
}

int32 nstile_grid::min_layer()
{
	int32 minVal = 0;
	for (uint32 i = 3; i < 8; ++i)
	{
		int32 size = static_cast<int32>(m_world_map[i].size());
		size *= -1;
		if (size < minVal)
			minVal = size;
	}
	return minVal;
}

int32 nstile_grid::max_layer()
{
	int32 maxVal = 0;
	for (uint32 i = 0; i < 4; ++i)
	{
		int32 size = static_cast<int32>(m_world_map[i].size());
		size -= 1;
		if (size > maxVal)
			maxVal = size;
	}
	return maxVal;
}

int32 nstile_grid::min_y()
{
	int32 minY = 0;

	for (uint32 i = 0; i < m_world_map[2].size(); ++i)
	{
		int32 lowestIndex = static_cast<int32>(m_world_map[2][i].size()) * -1;
		if (lowestIndex < minY)
			minY = lowestIndex;
	}

	for (uint32 i = 0; i < m_world_map[3].size(); ++i)
	{
		int32 lowestIndex = static_cast<int32>(m_world_map[3][i].size()) * -1;
		if (lowestIndex < minY)
			minY = lowestIndex;
	}

	for (uint32 i = 0; i < m_world_map[6].size(); ++i)
	{
		int32 lowestIndex = static_cast<int32>(m_world_map[6][i].size()) * -1;
		if (lowestIndex < minY)
			minY = lowestIndex;
	}

	for (uint32 i = 0; i < m_world_map[7].size(); ++i)
	{
		int32 lowestIndex = static_cast<int32>(m_world_map[7][i].size()) * -1;
		if (lowestIndex < minY)
			minY = lowestIndex;
	}
	return minY;
}

int32 nstile_grid::max_y()
{
	int32 maxY = 0;

	for (uint32 i = 0; i < m_world_map[0].size(); ++i)
	{
		int32 highestIndex = static_cast<int32>(m_world_map[0][i].size()) - 1;
		if (highestIndex > maxY)
			maxY = highestIndex;
	}

	for (uint32 i = 0; i < m_world_map[1].size(); ++i)
	{
		int32 highestIndex = static_cast<int32>(m_world_map[1][i].size()) - 1;
		if (highestIndex > maxY)
			maxY = highestIndex;
	}

	for (uint32 i = 0; i < m_world_map[4].size(); ++i)
	{
		int32 highestIndex = static_cast<int32>(m_world_map[4][i].size()) - 1;
		if (highestIndex > maxY)
			maxY = highestIndex;
	}

	for (uint32 i = 0; i < m_world_map[5].size(); ++i)
	{
		int32 highestIndex = static_cast<int32>(m_world_map[5][i].size()) - 1;
		if (highestIndex > maxY)
			maxY = highestIndex;
	}
	return maxY;
}

int32 nstile_grid::min_x()
{
	int32 minX = 0;

	for (uint32 i = 0; i < m_world_map[1].size(); ++i)
	{
		for (uint32 j = 0; j < m_world_map[1][i].size(); ++j)
		{
			int32 lowestIndex = static_cast<int32>(m_world_map[1][i][j].size()) * -1;
			if (lowestIndex < minX)
				minX = lowestIndex;
		}
	}

	for (uint32 i = 0; i < m_world_map[3].size(); ++i)
	{
		for (uint32 j = 0; j < m_world_map[3][i].size(); ++j)
		{
			int32 lowestIndex = static_cast<int32>(m_world_map[3][i][j].size()) * -1;
			if (lowestIndex < minX)
				minX = lowestIndex;
		}
	}

	for (uint32 i = 0; i < m_world_map[5].size(); ++i)
	{
		for (uint32 j = 0; j < m_world_map[5][i].size(); ++j)
		{
			int32 lowestIndex = static_cast<int32>(m_world_map[5][i][j].size()) * -1;
			if (lowestIndex < minX)
				minX = lowestIndex;
		}
	}

	for (uint32 i = 0; i < m_world_map[7].size(); ++i)
	{
		for (uint32 j = 0; j < m_world_map[7][i].size(); ++j)
		{
			int32 lowestIndex = static_cast<int32>(m_world_map[7][i][j].size()) * -1;
			if (lowestIndex < minX)
				minX = lowestIndex;
		}
	}
	return minX;
}

int32 nstile_grid::max_x()
{
	int32 maxX = 0;

	for (uint32 i = 0; i < m_world_map[0].size(); ++i)
	{
		for (uint32 j = 0; j < m_world_map[0][i].size(); ++j)
		{
			int32 highestIndex = static_cast<int32>(m_world_map[0][i][j].size()) - 1;
			if (highestIndex > maxX)
				maxX = highestIndex;
		}
	}

	for (uint32 i = 0; i < m_world_map[2].size(); ++i)
	{
		for (uint32 j = 0; j < m_world_map[2][i].size(); ++j)
		{
			int32 highestIndex = static_cast<int32>(m_world_map[2][i][j].size()) - 1;
			if (highestIndex > maxX)
				maxX = highestIndex;
		}
	}

	for (uint32 i = 0; i < m_world_map[4].size(); ++i)
	{
		for (uint32 j = 0; j < m_world_map[4][i].size(); ++j)
		{
			int32 highestIndex = static_cast<int32>(m_world_map[4][i][j].size()) - 1;
			if (highestIndex > maxX)
				maxX = highestIndex;
		}
	}

	for (uint32 i = 0; i < m_world_map[6].size(); ++i)
	{
		for (uint32 j = 0; j < m_world_map[6][i].size(); ++j)
		{
			int32 highestIndex = static_cast<int32>(m_world_map[6][i][j].size()) - 1;
			if (highestIndex > maxX)
				maxX = highestIndex;
		}
	}
	return maxX;
}

nstile_grid::grid_bounds nstile_grid::occupied_bounds()
{
	grid_bounds g;
	for (uint32 i = 0; i < 8; ++i)
	{
		for (uint32 z = 0; z < m_world_map[i].size(); ++z)
		{
			for (uint32 y = 0; y < m_world_map[i][z].size(); ++y)
			{
				for (uint32 x = 0; x < m_world_map[i][z][y].size(); ++x)
				{
					if (m_world_map[i][z][y][x] != uivec3())
					{
						ivec3 gridPos = grid(map_index(i, x, y, z));
						if (gridPos.x > g.max_space.x)
							g.max_space.x = gridPos.x;
						if (gridPos.y > g.max_space.y)
							g.max_space.y = gridPos.y;
						if (gridPos.z > g.max_space.z)
							g.max_space.z = gridPos.z;

						if (gridPos.x < g.min_space.x)
							g.min_space.x = gridPos.x;
						if (gridPos.y < g.min_space.y)
							g.min_space.y = gridPos.y;
						if (gridPos.z < g.min_space.z)
							g.min_space.z = gridPos.z;
					}
				}
			}
		}
	}
	return g;
}

bool nstile_grid::occupied(const fvec3 & pPos) const
{
	return occupied(ivec3(), pPos);
}

bool nstile_grid::occupied(const ivec3 & pSpace, const fvec3 & pOrigin) const
{
	ivec3 space = pSpace + grid(pOrigin);
	map_index ind = index(space);

	if (!_check_bounds(ind))
		return false;

	return (m_world_map[ind.quad_index][ind.raw_index.z][ind.raw_index.y][ind.raw_index.x] != uivec3());
}

bool nstile_grid::occupied(const ivec3_vector & pSpaces, const fvec3 & pOrigin) const
{
	for (uint32 i = 0; i < pSpaces.size(); ++i)
	{
		if (occupied(pSpaces[i], pOrigin))
			return true;
	}
	return false;
}

bool nstile_grid::remove(const fvec3 & pPos)
{
	return remove(ivec3(), pPos);
}

bool nstile_grid::remove(const ivec3 & pSpace, const fvec3 & pOrigin)
{
	if (!occupied(pSpace, pOrigin))
		return false;

	ivec3 space = pSpace + grid(pOrigin);
	map_index ind = index(space);

	// Set the space to (0,0) which signifies no item
	m_world_map[ind.quad_index][ind.raw_index.z][ind.raw_index.y][ind.raw_index.x] = uivec3();
	return true;
}

/*!
Go through entire grid and remove any occurances that have this entity ID
This is fairly slow so should not be used on a per-frame basis
*/
void nstile_grid::remove(const uint32 plugid)
{
	// Go through entire grid and remove any entrees with entity ID equal to above
	grid_bounds g;
	for (uint32 i = 0; i < 8; ++i)
	{
		for (uint32 z = 0; z < m_world_map[i].size(); ++z)
		{
			for (uint32 y = 0; y < m_world_map[i][z].size(); ++y)
			{
				for (uint32 x = 0; x < m_world_map[i][z][y].size(); ++x)
				{
					if (m_world_map[i][z][y][x].x == plugid)
						m_world_map[i][z][y][x] = uivec3();
				}
			}
		}
	}
}

/*!
Remove multiple tiles from the grid - each space will be shifted by the grid position of the origin
*/
bool nstile_grid::remove(const ivec3_vector & pSpaces, const fvec3 & pOrigin)
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
void nstile_grid::remove(const uivec2 & id)
{
	// Go through entire grid and remove any entrees with transform ID equal to pTFormID
	grid_bounds g;
	for (uint32 i = 0; i < 8; ++i)
	{
		for (uint32 z = 0; z < m_world_map[i].size(); ++z)
		{
			for (uint32 y = 0; y < m_world_map[i][z].size(); ++y)
			{
				for (uint32 x = 0; x < m_world_map[i][z][y].size(); ++x)
				{
					if (m_world_map[i][z][y][x].x == id.x && m_world_map[i][z][y][x].y == id.y)
						m_world_map[i][z][y][x] = uivec3();
				}
			}
		}
	}
}

void nstile_grid::name_change(const uivec2 & oldid, const uivec2 newid)
{
	// Go through entire grid and remove any entrees with entity ID equal to above
	grid_bounds g;
	for (uint32 i = 0; i < 8; ++i)
	{
		for (uint32 z = 0; z < m_world_map[i].size(); ++z)
		{
			for (uint32 y = 0; y < m_world_map[i][z].size(); ++y)
			{
				for (uint32 x = 0; x < m_world_map[i][z][y].size(); ++x)
				{
					if (m_world_map[i][z][y][x].x == oldid.x)
					{
						m_world_map[i][z][y][x].x = newid.x;
						if (m_world_map[i][z][y][x].y == oldid.y)
							m_world_map[i][z][y][x].y = newid.y;
					}
				}
			}
		}
	}
}


uivec3_vector nstile_grid::bounded_set(const fvec3 & pPoint1, const fvec3 & pPoint2)
{
	uivec3_vector retSet;
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
				map_index ind = index(minGrid);
				const uivec3 & id = _get_id(ind);
				if (id == 0)
					continue;
				retSet.push_back(id);
			}
		}
	}
	return retSet;
}

int32 nstile_grid::index_x(float pX, bool pOffset)
{
	if (pOffset)
		return int32(floor(((pX - X_GRID) / (2 * X_GRID)) + ROUND_FACTOR));

	return int32(floor((pX / (2 * X_GRID)) + ROUND_FACTOR));
}

int32 nstile_grid::index_y(float pY)
{
	return int32(floor((pY / Y_GRID) + ROUND_FACTOR));
}

int32 nstile_grid::index_z(float pZ)
{
	return int32(floor((pZ / Z_GRID) + ROUND_FACTOR));
}

nstile_grid::map_index nstile_grid::index(const ivec3 & pSpace)
{
	map_index index;
	if (pSpace.z < 0)
	{
		index.quad_index = quadrant_index(uint32(index.quad_index) + 4);
		index.raw_index.z = -1 * pSpace.z;
		index.raw_index.z -= 1;
	}
	else
		index.raw_index.z = pSpace.z;
	if (pSpace.y < 0)
	{
		index.quad_index = quadrant_index(uint32(index.quad_index) + 2);
		index.raw_index.y = -1 * pSpace.y;
		index.raw_index.y -= 1;
	}
	else
		index.raw_index.y = pSpace.y;
	if (pSpace.x < 0)
	{
		index.quad_index = quadrant_index(uint32(index.quad_index) + 1);
		index.raw_index.x = -1 * pSpace.x;
		index.raw_index.x -= 1;
	}
	else
		index.raw_index.x = pSpace.x;
	return index;
}

fvec3 nstile_grid::world(const ivec3 & pSpace, const fvec3 & pOrigin)
{
	ivec3 space = pSpace + grid(pOrigin);
	fvec3 pos(space.x * 2.0f * X_GRID, space.y * Y_GRID, space.z * Z_GRID);
	if (space.y % 2 != 0)
		pos.x += X_GRID;
	return pos;
}

ivec3 nstile_grid::grid(const map_index & pIndex)
{
	ivec3 grid;

	if (pIndex.quad_index < 4)
		grid.z = pIndex.raw_index.z;
	else
		grid.z = -1 * (pIndex.raw_index.z + 1);

	if (pIndex.quad_index % 4 < 2)
		grid.y = pIndex.raw_index.y;
	else
		grid.y = -1 * (pIndex.raw_index.y + 1);

	if (pIndex.quad_index % 2 == 0)
		grid.x = pIndex.raw_index.x;
	else
		grid.x = -1 * (pIndex.raw_index.x + 1);

	return grid;
}

fvec3 nstile_grid::world(const map_index & pIndex)
{
	return world(grid(pIndex));
}

void nstile_grid::snap(fvec3 & pToSnap)
{
	pToSnap = world(grid(pToSnap));
}

nstile_grid::map_index nstile_grid::index(const fvec3 & pWorldPos)
{
	return index(grid(pWorldPos));
}

ivec3 nstile_grid::grid(const fvec3 & pWorldPos)
{
	ivec3 gPos;

	gPos.y = index_y(pWorldPos.y);
	bool offset = (gPos.y % 2 != 0);
	gPos.x = index_x(pWorldPos.x, offset);
	gPos.z = index_z(pWorldPos.z);
	return gPos;
}

bool nstile_grid::_check_bounds(const map_index & pIndex) const
{
	if (pIndex.quad_index >= QUADRANT_COUNT)
		return false;

	if (pIndex.raw_index.z >= m_world_map[pIndex.quad_index].size())
		return false;

	if (pIndex.raw_index.y >= m_world_map[pIndex.quad_index][pIndex.raw_index.z].size())
		return false;

	if (pIndex.raw_index.x >= m_world_map[pIndex.quad_index][pIndex.raw_index.z][pIndex.raw_index.y].size())
		return false;

	return true;
}

void nstile_grid::_resize_for_space(const map_index & pIndex)
{
	uint32 lastIndex = 0;

	if (pIndex.raw_index.z >= m_world_map[pIndex.quad_index].size())
	{
		lastIndex = static_cast<int32>(m_world_map[pIndex.quad_index].size()) - 1;
		m_world_map[pIndex.quad_index].resize(pIndex.raw_index.z + TILE_GRID_RESIZE_PAD);

		//resize the x and y dimensions for all new layers
		for (uint32 i = lastIndex; i < m_world_map[pIndex.quad_index].size(); ++i)
		{
			m_world_map[pIndex.quad_index][i].resize(DEFAULT_GRID_SIZE);
			for (uint32 j = 0; j < DEFAULT_GRID_SIZE; ++j)
				m_world_map[pIndex.quad_index][i][j].resize(DEFAULT_GRID_SIZE);
		}
	}

	if (pIndex.raw_index.y >= m_world_map[pIndex.quad_index][pIndex.raw_index.z].size())
	{
		lastIndex = static_cast<int32>(m_world_map[pIndex.quad_index][pIndex.raw_index.z].size()) - 1;
		m_world_map[pIndex.quad_index][pIndex.raw_index.z].resize(pIndex.raw_index.y + TILE_GRID_RESIZE_PAD);

		// resize all the x dimensions for the current layer/row
		for (uint32 i = lastIndex; i < m_world_map[pIndex.quad_index][pIndex.raw_index.z].size(); ++i)
			m_world_map[pIndex.quad_index][pIndex.raw_index.z][i].resize(DEFAULT_GRID_SIZE);
	}

	if (pIndex.raw_index.x >= m_world_map[pIndex.quad_index][pIndex.raw_index.z][pIndex.raw_index.y].size())
		m_world_map[pIndex.quad_index][pIndex.raw_index.z][pIndex.raw_index.y].resize(pIndex.raw_index.x + TILE_GRID_RESIZE_PAD);
}

const uivec3 & nstile_grid::_get_id(const map_index & pIndex)
{
	return m_world_map[pIndex.quad_index][pIndex.raw_index.z][pIndex.raw_index.y][pIndex.raw_index.x];
}
