#include <nsfactory.h>
#include <nsresource.h>
#include <nscomponent.h>
#include <nssystem.h>
#include <nsres_manager.h>

void NSResFactory::setid(NSResource * res)
{
	res->mHashedType = type_id;
}

void NSResManagerFactory::setid(NSResManager * manager)
{
	manager->mHashedType = type_id;
}

void NSCompFactory::setid(NSComponent * comp)
{
	comp->mHashedType = type_id;
}

void NSSysFactory::setid(NSSystem * sys)
{
	sys->mHashedType = type_id;
}
