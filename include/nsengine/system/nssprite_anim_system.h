#ifndef NSSPRITE_ANIM_SYSTEM_H
#define NSSPRITE_ANIM_SYSTEM_H

#include <system/nssystem.h>

class nssprite_anim_system : public nssystem
{
  public:
	
	nssprite_anim_system();
	
	~nssprite_anim_system();

	void init();

	void release();

	void update();

	int32 update_priority();
	
};

#endif
