/*!
\file nstile_grid.h

\brief Header file for nstile_grid class

This file contains all of the neccessary declarations for the nstile_grid class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#ifndef NSTILEGRID_H
#define NSTILEGRID_H

#define X_GRID 0.86f
#define Y_GRID 1.49f
#define Z_GRID 0.45f
#define ROUND_FACTOR 0.5f

#define DEFAULT_GRID_SIZE 64
#define QUADRANT_COUNT 8
#define TILE_GRID_RESIZE_PAD 16


#include <nsvector.h>
#include <nsunordered_map.h>
#include <nsunordered_set.h>

class nstile_grid
{
public:
	typedef std::vector<uivec2_vector> map_layer;
	typedef std::vector<map_layer> map_quadrant;
	typedef std::vector<map_quadrant> map_world;

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

	struct grid_bounds
	{
		ivec3 min_space;
		ivec3 max_space;
	};

	struct map_index
	{
		map_index(uint32 quadrant_ = 0, uint32 x_ = 0, uint32 y_ = 0, uint32 z_ = 0) :
		quad_index(quadrant_index(quadrant_)),
		raw_index(x_, y_, z_)
		{}

		map_index(uint32 quadrant_, const uivec3 & pInd) :
			quad_index(quadrant_index(quadrant_)),
			raw_index(pInd)
		{}


		quadrant_index quad_index;
		uivec3 raw_index;
	};

	nstile_grid();

	nstile_grid(const nstile_grid & copy_);
	
	~nstile_grid();

	nstile_grid & operator=(nstile_grid rhs);
	
	bool add(const uivec2 & item_, const fvec3 & pos_);

	bool add(const uivec2 & item_, const ivec3 & space_, const fvec3 & origin_ = fvec3());

	bool add(const uivec2 & item_, const ivec3_vector & spaces_, const fvec3 & origin_ = fvec3());

	const uivec2 & at(const map_index & space_) const;

	void init();

	void release();

	uivec2 get(const fvec3 & pos_) const;

	uivec2 get(const ivec3 & space_, const fvec3 & origin_ = fvec3()) const;

	uivec2_vector bounded_set(const fvec3 & point1_, const fvec3 & point2_);

	grid_bounds occupied_bounds();

	int32 min_layer();

	int32 max_layer();

	int32 min_y();

	int32 max_y();

	int32 min_x();

	int32 max_x();

	bool occupied(const fvec3 & pos_) const;

	bool occupied(const ivec3 & space_, const fvec3 & origin_ = fvec3()) const;

	bool occupied(const ivec3_vector & spaces_, const fvec3 & origin_ = fvec3()) const;

	bool remove(const fvec3 & pos_);

	bool remove(const ivec3 & space_, const fvec3 & origin_ = fvec3());

	/*!
	Go through entire grid_ and remove any occurances that have this entity ID
	*/
	void remove(const uivec2 & id);

	/*!
	Go through entire grid_ and remove any occurances that have this plugin ID
	*/
	void remove(uint32 plug_id_);

	bool remove(const ivec3_vector & spaces_, const fvec3 & origin_ = fvec3());

	void name_change(const uivec2 & old_id_, const uivec2 new_id_);

	static int32 index_x(float x_, bool offset_);
	static int32 index_y(float y_);
	static int32 index_z(float z_);

	static ivec3 grid(const fvec3 & world_);
	static ivec3 grid(const map_index & index_);

	static map_index index(const ivec3 & grid_);
	static map_index index(const fvec3 & world_);

	static fvec3 world(const map_index & index_);
	static fvec3 world(const ivec3 & grid_, const fvec3 & origin_ = fvec3());

	static void snap(fvec3 & world_);

private:
	bool _check_bounds(const map_index & index_) const;
	void _resize_for_space(const map_index & index_);
	const uivec2 & _get_id(const map_index & index_);

	map_world m_world_map;
};


#endif
