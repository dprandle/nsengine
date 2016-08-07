/*! 
	\file nsentity_manager.h
	
	\brief Header file for nsentity_manager class

	This file contains all of the neccessary declarations for the NSEnTempManager class.

	\author Daniel Randle
	\date November 23 2013
	\copywrite Earth Banana Games 2013
*/

#ifndef NSENTITY_MANAGER_H
#define NSENTITY_MANAGER_H

#include "nsasset_manager.h"
#include "nsentity.h"

nsentity * get_entity(const uivec2 & id);

typedef std::set<nsentity*> entity_ptr_set;

class nsentity_manager : public nsasset_manager
{
  public:

	MANAGER_TEMPLATES
	
	nsentity_manager();
	~nsentity_manager();

	template<class comp_type>
	entity_ptr_set entities()
	{
		uint32 hashed_type = nse.type_id(std::type_index(typeid(comp_type)));
		return entities(hashed_type);
	}

	entity_ptr_set entities(uint32 comp_type_id);

	entity_ptr_set entities(const nsstring & comp_guid);
};

#endif



