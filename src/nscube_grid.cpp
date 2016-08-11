/*!
\file nscube_grid.cpp

\brief Definition file for nscube_grid class

This file contains all of the neccessary definitions for the nscube_grid class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#include <nscube_grid.h>
#include <nsentity.h>
#include <nstform_comp.h>

nscube_grid::nscube_grid()
{}

nscube_grid::nscube_grid(const nscube_grid & copy_):
	world_map(),
	m_world_bounds(copy_.m_world_bounds)
{
	for (uint8 i = 0; i < QUADRANT_COUNT; ++i)
		world_map[i] = copy_.world_map[i];
}

nscube_grid::~nscube_grid()
{}

void nscube_grid::release()
{
	cube_grid_quadrant q;
	for (uint8 i = 0; i < QUADRANT_COUNT; ++i)
		world_map[i].swap(q);
}

void nscube_grid::init()
{
	for (uint32 i = 0; i < QUADRANT_COUNT; ++i)
	{
		world_map[i].resize(DEFAULT_GRID_Z_SIZE);
		for (uint32 j = 0; j < DEFAULT_GRID_Z_SIZE; ++j)
		{
			world_map[i][j].resize(DEFAULT_GRID_Y_SIZE);
			for (uint32 k = 0; k < DEFAULT_GRID_Y_SIZE; ++k)
				world_map[i][j][k].resize(DEFAULT_GRID_X_SIZE);
		}
	}	
}

nscube_grid & nscube_grid::operator=(nscube_grid rhs)
{
	std::swap(world_map, rhs.world_map);
	std::swap(m_world_bounds, rhs.m_world_bounds);
	return *this;
}

bool nscube_grid::insert(instance_tform * itf)
{
	return insert(uivec3(itf->owner()->owner->owner()->full_id(),itf->current_tform_id()), itf->phys.aabb);
}

bool nscube_grid::insert(const uivec3 & item_, const ibox & aabb_post_tform)
{
	ibox bounds(aabb_post_tform);
	if (aabb_post_tform.max == ivec3(0) && aabb_post_tform.min == ivec3(0))
		bounds = m_world_bounds;
	
	// If space is out of bounds, allocate more memory to accomadate
	uivec4 ind;
	ivec3 space;
	bool ret = true;
	for (int i = aabb_post_tform.min.z; i < aabb_post_tform.max.z; ++i)
	{
		for (int j = aabb_post_tform.min.y; j < aabb_post_tform.max.y; ++j)
		{
			for (int k = aabb_post_tform.min.x; k < aabb_post_tform.max.x; ++k)
			{
				space.set(k,j,i);
				_resize_if_needed(space);
				ind = index_from(space);
				if (!_item_already_there(ind,item_))
				{
					world_map[ind.w][ind.z][ind.y][ind.x].push_back(item_);
				}
				else
				{
					ret = false;
				}
			}
		}

	}
	return ret;	
}

bool nscube_grid::insert(const uivec3 & item_, const fbox & aabb_post_tform)
{
	return insert(item_, grid_from(aabb_post_tform));
}

uivec3_vector * nscube_grid::items_at(const fvec3 & pos_)
{
	return items_at(grid_from(pos_));
}

uivec3_vector * nscube_grid::items_at(const ivec3 & grid_pos_)
{
	uivec4 ind = index_from(grid_pos_);
	if (_index_in_bounds(ind))
		return &world_map[ind.w][ind.z][ind.y][ind.x];
	return nullptr;
}

bool nscube_grid::_index_in_bounds(const uivec4 & ind)
{
	if (ind.z < world_map[ind.w].size())
	{
		if (ind.y < world_map[ind.w][ind.z].size())
		{
			if (ind.x < world_map[ind.w][ind.z][ind.y].size())
				return true;
		}
	}	
}

const ibox & nscube_grid::grid_bounds()
{
	return m_world_bounds;
}

void nscube_grid::search_and_remove(nsentity * ent, uint32 tform_id, const ibox & search_bounds)
{
	search_and_remove(uivec3(ent->full_id(),tform_id), search_bounds);
}

void nscube_grid::search_and_remove(nsentity * ent, uint32 tform_id, const fbox & aabb)
{
	search_and_remove(uivec3(ent->full_id(),tform_id), grid_from(aabb));
}

void nscube_grid::search_and_remove(const uivec3 & tf_id, const fbox & aabb)
{
	search_and_remove(tf_id, grid_from(aabb));
}

void nscube_grid::search_and_remove(const uivec3 & tf_id, const ibox & search_bounds)
{
	ibox bounds(search_bounds);
	if (bounds.min == ivec3(0) && bounds.max == ivec3(0))
		bounds = m_world_bounds;

	ivec3 space;
	uivec4 ind;
	for (int i = bounds.min.z; i < bounds.max.z; ++i)
	{
		for (int j = bounds.min.y; j < bounds.max.y; ++j)
		{
			for (int k = bounds.min.x; k < bounds.max.x; ++k)
			{
				space.set(k,j,i);
				ind = index_from(space);
				_remove_item(ind, tf_id);
			}
		}
	}
}

void nscube_grid::search_and_remove(nsentity * ent, const ibox & search_bounds)
{
	search_and_remove(ent->full_id(), search_bounds);
}

void nscube_grid::search_and_remove(nsentity * ent, const fbox & aabb)
{
	search_and_remove(ent->full_id(), grid_from(aabb));
}

void nscube_grid::search_and_remove(const uivec2 & tf_id, const fbox & aabb)
{
	search_and_remove(tf_id, grid_from(aabb));
}

void nscube_grid::search_and_remove(const uivec2 & tf_id, const ibox & search_bounds)
{
	ibox bounds(search_bounds);
	if (bounds.min == ivec3(0) && bounds.max == ivec3(0))
		bounds = m_world_bounds;

	ivec3 space;
	uivec4 ind;
	for (int i = bounds.min.z; i < bounds.max.z; ++i)
	{
		for (int j = bounds.min.y; j < bounds.max.y; ++j)
		{
			for (int k = bounds.min.x; k < bounds.max.x; ++k)
			{
				space.set(k,j,i);
				ind = index_from(space);
				_remove_item(ind, tf_id);
			}
		}
	}
}

void nscube_grid::remove(nsentity * ent)
{
	search_and_remove(ent->full_id());
}

void nscube_grid::remove(instance_tform * tform)
{
//	search_and_remove(tform->)
}

bool nscube_grid::_remove_item(const uivec4 & ind, const uivec3 & item)
{
	if (_index_in_bounds(ind))
	{
		auto iter = world_map[ind.w][ind.z][ind.y][ind.x].begin();
		while (iter != world_map[ind.w][ind.z][ind.y][ind.x].end())
		{
			if (item == (*iter))
				iter = world_map[ind.w][ind.z][ind.y][ind.x].erase(iter);
			else
				++iter;
		}
	}
}

bool nscube_grid::_remove_item(const uivec4 & ind, const uivec2 & item)
{
	if (_index_in_bounds(ind))
	{
		auto iter = world_map[ind.w][ind.z][ind.y][ind.x].begin();
		while (iter != world_map[ind.w][ind.z][ind.y][ind.x].end())
		{
			if (item == (*iter).xy())
				iter = world_map[ind.w][ind.z][ind.y][ind.x].erase(iter);
			else
				++iter;
		}
	}
}

void nscube_grid::name_change(const uivec2 & oldid, const uivec2 newid)
{
	uivec4 ind;
	for (uint32 i = 0; i < 8; ++i)
	{
		for (uint32 z = 0; z < world_map[i].size(); ++z)
		{
			for (uint32 y = 0; y < world_map[i][z].size(); ++y)
			{
				for (uint32 x = 0; x < world_map[i][z][y].size(); ++x)
				{
					auto iter = world_map[i][z][y][x].begin();
					while (iter != world_map[i][z][y][x].end())
					{
						if (iter->xy() == oldid)
							iter->set(newid.x,newid.y,iter->z);
						++iter;
					}
				}
			}
		}
	}
}


ivec3 nscube_grid::grid_from(const fvec3 & world_pos)
{
	return ivec3(std::round(world_pos.x / CUBE_X_GRID - CUBE_X_GRID_OFFSET),
				 std::round(world_pos.y / CUBE_Y_GRID - CUBE_Y_GRID_OFFSET),
				 std::round(world_pos.z / CUBE_Z_GRID - CUBE_Z_GRID_OFFSET));
}

ibox nscube_grid::grid_from(const fbox & aabb)
{
	return ibox(grid_from(aabb.min),grid_from(aabb.max) + ivec3(1));
}

fvec3 nscube_grid::world_from(const ivec3 & grid_pos)
{
	return fvec3(grid_pos.x * CUBE_X_GRID + CUBE_X_GRID_OFFSET * CUBE_X_GRID,
				 grid_pos.y * CUBE_Y_GRID + CUBE_Y_GRID_OFFSET * CUBE_Y_GRID,
				 grid_pos.z * CUBE_Z_GRID + CUBE_Z_GRID_OFFSET * CUBE_Z_GRID);
}

fbox nscube_grid::world_from(const ibox & box)
{
	return fbox(world_from(box.min),world_from(box.max - ivec3(1)));
}

void nscube_grid::snap_to_grid(fvec3 & snap_)
{
	snap_ = world_from(grid_from(snap_));
}

uivec4 nscube_grid::index_from(const fvec3 & world_)
{
	return index_from(grid_from(world_));
}

uivec4 nscube_grid::index_from(const ivec3 & grid_pos)
{
	uivec4 ind;

	if (grid_pos.z < 0)
	{
		ind.w = 4;
		ind.z = (grid_pos.z * -1) - 1;
	}
	else
	{
		ind.z = grid_pos.z;
	}
	
	if (grid_pos.y < 0)
	{
		ind.w += 2;
		ind.y = (grid_pos.y * -1) - 1;
	}
	else
	{
		ind.y = grid_pos.y;
	}
	
	if (grid_pos.x < 0)
	{
		ind.w += 1;
		ind.x = (grid_pos.x * -1) - 1;
	}
	else
	{
		ind.x = grid_pos.x;
	}
	
	return ind;
}

fvec3 nscube_grid::world_from(const uivec4 & ind)
{
	return world_from(grid_from(ind));
}

ivec3 nscube_grid::grid_from(const uivec4 & ind)
{
	ivec3 grid;

	if (ind.w < 4)
		grid.z = ind.z;
	else
		grid.z = -1 * (ind.z + 1);

	if (ind.w % 4 < 2)
		grid.y = ind.y;
	else
		grid.y = -1 * (ind.y + 1);

	if (ind.w % 2 == 0)
		grid.x = ind.x;
	else
		grid.x = -1 * (ind.x + 1);

	return grid;
}

void nscube_grid::_resize_if_needed(const ivec3 & grid_pos)
{
	uivec4 ind = index_from(grid_pos);
	uint32 last_ind = 0;

	bool update_bounds = false;
	
	if (ind.z >= world_map[ind.w].size())
	{
		last_ind = world_map[ind.w].size() - 1;
		world_map[ind.w].resize(ind.z + DEFAULT_X_GRID_PAD);

		//resize the x and y dimensions for all new layers
		for (uint32 i = last_ind; i < world_map[ind.w].size(); ++i)
		{
			world_map[ind.w][i].resize(DEFAULT_GRID_Y_SIZE);
			for (uint32 j = 0; j < DEFAULT_GRID_Y_SIZE; ++j)
				world_map[ind.w][i][j].resize(DEFAULT_GRID_X_SIZE);
		}
		update_bounds = true;
	}

	if (ind.y >= world_map[ind.w][ind.z].size())
	{
		last_ind = world_map[ind.w][ind.z].size() - 1;
		world_map[ind.w][ind.z].resize(ind.y + DEFAULT_Y_GRID_PAD);

		// resize all the x dimensions for the current layer/row
		for (uint32 i = last_ind; i < world_map[ind.w][ind.z].size(); ++i)
			world_map[ind.w][ind.z][i].resize(DEFAULT_GRID_X_SIZE);

		update_bounds = true;
	}

	if (ind.x >= world_map[ind.w][ind.z][ind.y].size())
	{
		world_map[ind.w][ind.z][ind.y].resize(ind.x + DEFAULT_X_GRID_PAD);
		update_bounds = true;
	}

	if (update_bounds)
		_update_bounds();
}

void nscube_grid::_update_bounds()
{
	m_world_bounds = ibox();
	for (uint32 i = 0; i < 8; ++i)
	{
		for (uint32 z = 0; z < world_map[i].size(); ++z)
		{
			for (uint32 y = 0; y < world_map[i][z].size(); ++y)
			{
				for (uint32 x = 0; x < world_map[i][z][y].size(); ++x)
				{
					ivec3 grid_pos = grid_from(uivec4(x, y, z, i));
					if (grid_pos.x > m_world_bounds.max.x)
						m_world_bounds.max.x = grid_pos.x;
					if (grid_pos.y > m_world_bounds.max.y)
						m_world_bounds.max.y = grid_pos.y;
					if (grid_pos.z > m_world_bounds.max.z)
						m_world_bounds.max.z = grid_pos.z;

					if (grid_pos.x < m_world_bounds.min.x)
						m_world_bounds.min.x = grid_pos.x;
					if (grid_pos.y < m_world_bounds.min.y)
						m_world_bounds.min.y = grid_pos.y;
					if (grid_pos.z < m_world_bounds.min.z)
						m_world_bounds.min.z = grid_pos.z;
						
				}
			}
		}
	}
}


bool nscube_grid::_item_already_there(const uivec4 & ind, const uivec3 & item)
{
	uivec3_vector::iterator iter = world_map[ind.w][ind.z][ind.y][ind.x].begin();
	while (iter != world_map[ind.w][ind.z][ind.y][ind.x].end())
	{
		if ((*iter) == item)
			return true;
		++iter;
	}
	return false;
}
