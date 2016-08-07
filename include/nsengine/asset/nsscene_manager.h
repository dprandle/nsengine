/*!
\file nsscene_manager.h

\brief Header file for nsscene_manager class

This file contains all of the neccessary declarations for the nsscene_manager class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#ifndef NSSCENE_MANAGER_H
#define NSSCENE_MANAGER_H

#include "nsasset_manager.h"
#include "nsscene.h"

nsscene * get_scene(const uivec2 & id);

class nsscene_manager : public nsasset_manager
{
public:

	MANAGER_TEMPLATES
	
	nsscene_manager();
	~nsscene_manager();
	
};

#endif
