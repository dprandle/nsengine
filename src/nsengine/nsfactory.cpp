#include <nsfactory.h>
#include <nsresource.h>
#include <nscomponent.h>
#include <nssystem.h>
#include <nsres_manager.h>

void nsres_factory::set_id(nsresource * res)
{
	res->m_hashed_type = type_id;
}

void nsmanager_factory::set_id(nsres_manager * manager)
{
	manager->m_hashed_type = type_id;
}

void nscomp_factory::set_id(nscomponent * comp)
{
	comp->m_hashed_type = type_id;
}

void nssys_factory::set_id(nssystem * sys)
{
	sys->m_hashed_type = type_id;
}
