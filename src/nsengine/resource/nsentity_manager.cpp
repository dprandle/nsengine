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
#include <nsplugin_manager.h>

nsentity * get_entity(const uivec2 & id)
{
	nsplugin * plg = nsep.get(id.x);
	if (plg == nullptr)
		return nullptr;
	return plg->get<nsentity>(id.y);
}

nsentity_manager::nsentity_manager() :
	nsres_manager(type_to_hash(nsentity_manager))
{
	set_local_dir(LOCAL_ENTITY_DIR_DEFAULT);
	set_save_mode(nsres_manager::text);
}

nsentity_manager::~nsentity_manager()
{}

entity_ptr_set nsentity_manager::entities(uint32 comp_type_id)
{
	entity_ptr_set ret;
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

entity_ptr_set nsentity_manager::entities(const nsstring & comp_guid)
{
	return entities(hash_id(comp_guid));
}
