/*!
\file nscube_grid.h

\brief Header file for nscube_grid class

This file contains all of the neccessary declarations for the nscube_grid class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#ifndef NSCUBE_GRID_H
#define NSCUBE_GRID_H

#define CUBE_X_GRID 10.0f
#define CUBE_Y_GRID 10.0f
#define CUBE_Z_GRID 10.0f

#define CUBE_X_GRID_OFFSET 0.0f
#define CUBE_Y_GRID_OFFSET 0.0f
#define CUBE_Z_GRID_OFFSET 0.0f

#define ROUND_FACTOR 0.5f

#define QUADRANT_COUNT 8

#define DEFAULT_X_GRID_PAD 16
#define DEFAULT_Y_GRID_PAD 16
#define DEFAULT_Z_GRID_PAD 4

#define DEFAULT_GRID_X_SIZE 64
#define DEFAULT_GRID_Y_SIZE 64
#define DEFAULT_GRID_Z_SIZE 16


#include <nsvector.h>
#include <nsunordered_map.h>
#include <nsunordered_set.h>
#include <nsmath.h>

class nsentity;
struct instance_tform;

typedef std::vector<std::vector<uivec3_vector>> cube_grid_layer;
typedef std::vector<cube_grid_layer> cube_grid_quadrant;
typedef cube_grid_quadrant cube_grid_world[QUADRANT_COUNT];

enum quadrant_index
{
	top_right_front,
	top_left_front,
	top_right_back,
	top_left_back,
	bottom_right_front,
	bottom_left_front,
	bottom_right_back,
	bottom_left_back
};

class nscube_grid
{
public:

	nscube_grid();

	nscube_grid(const nscube_grid & copy_);
	
	~nscube_grid();

	nscube_grid & operator=(nscube_grid rhs);

	void init();

	void release();

	bool insert(const uivec3 & item_, const ibox & aabb_post_tform);

	bool insert(const uivec3 & item_, const fbox & aabb_post_tform);

	bool insert(instance_tform * itf);

	uivec3_vector * items_at(const fvec3 & pos_);

	uivec3_vector * items_at(const ivec3 & grid_pos_);

	const ibox & grid_bounds();

	// search bounds will limit the search to specified bounds
	void search_and_remove(nsentity * ent, const ibox & search_bounds=ibox());
	void search_and_remove(nsentity * ent, const fbox & aabb);
	void search_and_remove(const uivec2 & ent_id, const fbox & aabb);
	void search_and_remove(const uivec2 & ent_id, const ibox & search_bounds=ibox());

	// search bounds will limit the search to specified bounds
	void search_and_remove(nsentity * ent, uint32 tform_id, const ibox & search_bounds=ibox());
	void search_and_remove(nsentity * ent, uint32 tform_id, const fbox & aabb);
	void search_and_remove(const uivec3 & tf_id, const fbox & aabb);
	void search_and_remove(const uivec3 & tf_id, const ibox & search_bounds=ibox());

	void remove(nsentity * ent);
	void remove(instance_tform * tform);
	
	void name_change(const uivec2 & old_id_, const uivec2 new_id_);

	static ivec3 grid_from(const fvec3 & world_);
	static ibox grid_from(const fbox & aabb);

	static fvec3 world_from(const ivec3 & grid);
	static fbox world_from(const ibox & box);
	
	static void snap_to_grid(fvec3 & world_);

	cube_grid_world world_map;
	
private:
	uivec4 index_from(const ivec3 & grid_);
	uivec4 index_from(const fvec3 & world_);
	ivec3 grid_from(const uivec4 & index);
	fvec3 world_from(const uivec4 & index);
	
	void _resize_if_needed(const ivec3 & grid_pos);
	void _update_bounds();

	bool _index_in_bounds(const uivec4 & ind);
	bool _item_already_there(const uivec4 & index, const uivec3 & item);
	bool _remove_item(const uivec4 & index, const uivec3 & item);
	bool _remove_item(const uivec4 & index, const uivec2 & item);

	ibox m_world_bounds;
};


#endif
