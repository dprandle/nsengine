#ifndef NSPHYSIC_COMP_H
#define NSPHYSIC_COMP_H

#include <component/nscomponent.h>

struct instance_tform;

enum collider_type
{
	col_cube,
	col_sphere
};

class nsphysic_comp : public nscomponent
{
  public:
	
	nsphysic_comp();

	nsphysic_comp(const nsphysic_comp & copy);

	~nsphysic_comp();

	void init();

	void release();

	void finalize();

	void name_change(const uivec2 &, const uivec2);

	uivec3_vector resources();

	void pup(nsfile_pupper * p);

	int col_type;

	fvec4 sphere;
	fbox obb;
	
	float mass;
	bool gravity;
	bool dynamic;
	float bounciness; // ie 0 means completely inelastic and 1 means perfectly elastic

	std::vector<instance_tform*> frame_collisions;
};

#endif
