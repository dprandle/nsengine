/*! 
	\file nsentitymanager.cpp
	
	\brief Definition file for nsentity_manager class

	This file contains all of the neccessary definitions for the nsentity_manager class.

	\author Daniel Randle
	\date November 23 2013
	\copywrite Earth Banana Games 2013
*/


#include <nsentity_manager.h>
#include <nsengine.h>
#include <nsscene_manager.h>
#include <nsscene.h>
#include <nsengine.h>

nsentity_manager::nsentity_manager() : nsres_manager()
{
	set_local_dir(LOCAL_ENTITY_DIR_DEFAULT);
	set_save_mode(nsres_manager::text);
}

nsentity_manager::~nsentity_manager()
{}

nspentityset nsentity_manager::entities(uint32 comp_type_id)
{
	nspentityset ret;
	auto iter = m_id_resmap.begin();
	while (iter != m_id_resmap.end())
	{
		nsentity * curEnt = get(iter->first);
		if (curEnt->has(comp_type_id))
			ret.emplace(curEnt);
		++iter;
	}
	return ret;	
}

nspentityset nsentity_manager::entities(const nsstring & comp_guid)
{
	return entities(hash_id(comp_guid));
}
