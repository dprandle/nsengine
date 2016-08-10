// h file

#include <nssystem.h>

#define CUSTOM_SYS_TYPE_UPDATE_PR 150000

class custom_system_type : public nssystem
{
	custom_system_type();
	
	~custom_system_type();

	void init();

	void setup_input_map(nsinput_map * im, const nsstring & gctxt);

	void release();

	void update();

	int32 update_priority();	
};


// c file

//#include <custom_system_type.h>
#include <nsengine.h>

custom_system_type::custom_system_type():
	nssystem(type_to_hash(custom_system_type))
{
		
}
	
custom_system_type::~custom_system_type()
{
		
}

void custom_system_type::init()
{
}

void custom_system_type::setup_input_map(nsinput_map * im, const nsstring & gctxt)
{
}

void custom_system_type::release()
{
		
}

void custom_system_type::update()
{
}

int32 custom_system_type::update_priority()
{
	return CUSTOM_SYS_TYPE_UPDATE_PR;
}
