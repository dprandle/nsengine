#ifndef NSPREFAB_MANAGER_H
#define NSPREFAB_MANAGER_H

#include <asset/nsasset_manager.h>
#include <asset/nsprefab.h>

class nsprefab_manager : public nsasset_manager
{
public:

	MANAGER_TEMPLATES
	
	nsprefab_manager();
	~nsprefab_manager();
};

#endif
