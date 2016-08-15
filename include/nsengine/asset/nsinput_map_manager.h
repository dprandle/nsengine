#ifndef NSINPUT_MAP_MANAGER_H
#define NSINPUT_MAP_MANAGER_H

#include <asset/nsasset_manager.h>
#include <asset/nsinput_map.h>

class nsinput_map_manager : public nsasset_manager
{

  public:

	MANAGER_TEMPLATES
	
	nsinput_map_manager();
	~nsinput_map_manager();
	
};

#endif
