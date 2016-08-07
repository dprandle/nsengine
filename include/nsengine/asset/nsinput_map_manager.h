#ifndef NSINPUT_MAP_MANAGER_H
#define NSINPUT_MAP_MANAGER_H

#include "nsasset_manager.h"
#include "nsinput_map.h"

class nsinput_map_manager : public nsasset_manager
{

  public:

	MANAGER_TEMPLATES
	
	nsinput_map_manager();
	~nsinput_map_manager();
	
};

#endif
