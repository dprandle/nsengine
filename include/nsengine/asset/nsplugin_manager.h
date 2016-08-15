/*!
\file nsplugin_manager.h

\brief Header file for nsplugin_manager class

This file contains all of the neccessary declarations for the nsplugin_manager class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#ifndef NSPLUGIN_MANAGER_H
#define NSPLUGIN_MANAGER_H

#include <asset/nsasset_manager.h>
#include <asset/nsplugin.h>

nsplugin * get_plugin(uint32 id);

class nsplugin_manager : public nsasset_manager
{
public:

	MANAGER_TEMPLATES
	
	nsplugin_manager();	
	~nsplugin_manager();
	
};

#define nsep (*nse.plugins())

#endif
