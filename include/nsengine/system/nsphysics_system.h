
#ifndef NSPHYSIC_SYSTEM_H
#define NSPHYSIC_SYSTEM_H

#define GRAVITY_ACCEL 9.81

#include <system/nssystem.h>

#define COLLISION_RECOIL 0.1f

class nsentity;

struct col_stuff
{
	uivec3 space_ind;
	fvec3 norm;
};

class nsphysics_system : public nssystem
{
  public:
	nsphysics_system();

	~nsphysics_system();

	void init();

	void setup_input_map(nsinput_map * im, const nsstring & gctxt);

	void release();

	void draw_tile_grid();

	void update();

	void add_col_items_to_vec(
		std::vector<col_stuff> & vec,
		uivec3_vector * item_ptr,
		const fvec3 & norm);

	fbox calc_final_velocities(const fvec3 & init_vel_a,
								const fvec3 & init_vel_b,
								const fvec3 & normal,
								const fvec2 & masses,
							   float restitution);

	void check_and_resolve_collisions();

	int32 update_priority();

	nsentity * bb_shower;
	fvec3 gravity;
	bool draw_grid;
	fvec3 cam_grid_cube;
	bool handle_toggle_draw_grid(nsaction_event * evnt);
};

#endif
