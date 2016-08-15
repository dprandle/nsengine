#include <component/nsphysic_comp.h>
#include <nsengine.h>

nsphysic_comp::nsphysic_comp():
	nscomponent(type_to_hash(nsphysic_comp)),
	col_type(col_cube),
	sphere(),
	obb(),
    mass(80.0f),
	gravity(true),
	dynamic(false),
	bounciness(0.0f)
{
		
}

nsphysic_comp::nsphysic_comp(const nsphysic_comp & copy):
	nscomponent(copy.m_hashed_type)
{
		
}

nsphysic_comp::~nsphysic_comp()
{
		
}

void nsphysic_comp::init()
{
}

void nsphysic_comp::release()
{
		
}

void nsphysic_comp::finalize()
{
		
}

void nsphysic_comp::name_change(const uivec2 &, const uivec2)
{
		
}

uivec3_vector nsphysic_comp::resources()
{
	return uivec3_vector();
}

void nsphysic_comp::pup(nsfile_pupper * p)
{
	
}

