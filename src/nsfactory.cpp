#include <nsfactory.h>
#include <nsresource.h>
#include <nscomponent.h>
#include <nssystem.h>
#include <nsres_manager.h>

void NSResFactory::setid(nsresource * res)
{
	res->m_hashed_type = type_id;
}

void nsres_managerFactory::setid(nsres_manager * manager)
{
	manager->m_hashed_type = type_id;
}

void NSCompFactory::setid(NSComponent * comp)
{
	comp->m_hashed_type = type_id;
}

void NSSysFactory::setid(NSSystem * sys)
{
	sys->m_hashed_type = type_id;
}
