// h file

#include <nscomponent.h>

class custom_comp_type_sep : public nscomponent
{
  public:
	
	custom_comp_type_sep();

	custom_comp_type_sep(const custom_comp_type_sep & copy);

	~custom_comp_type_sep();

	void init();

	void release();

	void finalize();

	void name_change(const uivec2 &, const uivec2);

	uivec3_vector resources();

	void pup(nsfile_pupper * p);
};

// c file

// Dont forget to register comp type!!

//#include <custom_comp_type_sep.h>
#include <nsengine.h>

custom_comp_type_sep::custom_comp_type_sep():
	nscomponent(type_to_hash(custom_comp_type_sep))
{
		
}

custom_comp_type_sep::custom_comp_type_sep(const custom_comp_type_sep & copy):
	nscomponent(copy.m_hashed_type)
{
		
}

custom_comp_type_sep::~custom_comp_type_sep()
{
		
}

void custom_comp_type_sep::init()
{
		
}

void custom_comp_type_sep::release()
{
		
}

void custom_comp_type_sep::finalize()
{
		
}

void custom_comp_type_sep::name_change(const uivec2 &, const uivec2)
{
		
}

uivec3_vector custom_comp_type_sep::resources()
{
	return uivec3_vector();
}

void custom_comp_type_sep::pup(nsfile_pupper * p)
{
		
}
