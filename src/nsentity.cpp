/*! 
	\file nsentity.cpp
	
	\brief Definition file for NSEntity class

	This file contains all of the neccessary definitions for the NSEntity class.

	\author Daniel Randle
	\date November 23 2013
	\copywrite Earth Banana Games 2013
*/

#include <nsentity.h>
#include <nsscene.h>
#include <nsscenemanager.h>
#include <nsselcomp.h>
#include <nsfactory.h>
#include <nsengine.h>

NSEntity::NSEntity() :mComponents(), NSResource()
{
	mComponents.clear();
	setExtension(DEFAULT_ENTITY_EXTENSION);
}

NSEntity::NSEntity(NSEntity & toCopy) :mComponents(), NSResource()
{
	auto iter = toCopy.begin();
	while (iter != toCopy.end())
	{
		copy(iter->second);
		++iter;
	}
}

NSEntity::~NSEntity()
{
	clear();
}

nsbool NSEntity::add(NSComponent * pComp)
{
	if (pComp == NULL)
		return false;

	std::type_index ti(typeid(*pComp));
	nsuint hashed_type = nsengine.typeID(ti);
	if (hashed_type == 0)
	{
		dprint(nsstring("Cannot add component with type ") + ti.name() + nsstring(" to Entity ") + mName +
			   nsstring(": No hash_id found"));
		return false;
	}
	
	auto ret = mComponents.emplace(hashed_type, pComp);
	if (ret.second)
	{
		pComp->setOwner(this);
		updateScene();
	}
	return ret.second;
}

nsbool NSEntity::copy(NSComponent * toCopy, bool overwrite)
{
	if (toCopy == NULL)
		return false;

	NSComponent * nc = NULL;
	
	std::type_index ti(typeid(*toCopy));
	nsuint type_id = nsengine.typeID(ti);
	if (has(type_id))
	{
		if (!overwrite)
			return false;
	}
	else
		nc = create(type_id);

	if (nc == NULL)
		return false;

	(*nc) = (*toCopy);
	return true;
}

void NSEntity::clear()
{
	auto iter = mComponents.begin();
	while (iter != mComponents.end())
	{
		delete iter->second;
		iter = mComponents.erase(iter);
	}
	updateScene();
}

NSEntity::CompSet::iterator NSEntity::begin()
{
	return mComponents.begin();
}

NSEntity::CompSet::iterator NSEntity::end()
{
	return mComponents.end();
}

NSComponent * NSEntity::create(nsuint type_id)
{
	NSComponent * comp_t = nsengine.factory<NSCompFactory>(type_id)->create();
	comp_t->init();
	if (!add(comp_t))
	{
		dprint(nsstring("NSEntity::createComponent - Failed adding comp_t type ") + nsengine.guid(type_id) +
			   nsstring(" to Entity ") + mName);
		delete comp_t;
		return NULL;
	}
	return comp_t;
}

NSComponent * NSEntity::create(const nsstring & guid)
{
	return create(hash_id(guid));
}

bool NSEntity::del(const nsstring & guid)
{
	return del(hash_id(guid));
}

bool NSEntity::del(nsuint type_id)
{
	NSComponent * cmp = remove(type_id);
	if (cmp != NULL) // Log delete
	{
		delete cmp;
		updateScene();
		dprint("NSEntity::del - Deleting \"" + nsengine.guid(type_id) + "\" from Entity " + mName + "\"");
		return true;
	}
	dprint("NSEntity::del - Component type \"" + nsengine.guid(type_id) + "\" was not part of Entity \"" + mName + "\"");
	return false;
}

void NSEntity::pup(NSFilePUPer * p)
{
	if (p->type() == NSFilePUPer::Binary)
	{
		NSBinFilePUPer * bf = static_cast<NSBinFilePUPer *>(p);
		::pup(*bf, *this);
	}
	else
	{
		NSTextFilePUPer * tf = static_cast<NSTextFilePUPer *>(p);
		::pup(*tf, *this);
	}
}

nsuint NSEntity::count()
{
	return mComponents.size();
}

NSComponent * NSEntity::get(const nsstring & guid)
{
	return get(hash_id(guid));
}

NSComponent * NSEntity::get(nsuint type_id)
{
	CompSet::iterator iter = mComponents.find(type_id);
	if (iter != mComponents.end())
		return iter->second;
	return NULL;
}

void NSEntity::nameChange(const uivec2 & oldid, const uivec2 newid)
{
	// Go through each component and call name change
	auto iter = mComponents.begin();
	while (iter != mComponents.end())
	{
		iter->second->nameChange(oldid, newid);
		++iter;
	}
}

/*!
Get the other resources that this Entity uses. This is given by all the components attached to the entity.
*/
uivec2array NSEntity::resources()
{
	uivec2array ret;

	// Go through each component and insert all used resources from each comp_t
	auto iter = mComponents.begin();
	while (iter != mComponents.end())
	{
		uivec2array tmp = iter->second->resources();
		ret.insert(ret.end(), tmp.begin(), tmp.end() );
		++iter;
	}
	return ret;
}

bool NSEntity::has(const nsstring & guid)
{
	return has(hash_id(guid));
}

bool NSEntity::has(nsuint type_id)
{
	return (mComponents.find(type_id) != mComponents.end());
}

void NSEntity::init()
{
	// do nothing
}

NSComponent * NSEntity::remove(nsuint type_id)
{
	NSComponent * comp_t = NULL;
	auto iter = mComponents.find(type_id);
	if (iter != mComponents.end())
	{
		comp_t = iter->second;
		comp_t->setOwner(NULL);
		mComponents.erase(iter);
		updateScene();
		dprint("NSEntity::remove - Removing \"" + nsengine.guid(type_id) + "\" from Entity " + mName + "\"");
	}
	else
		dprint("NSEntity::remove - Component type \"" + nsengine.guid(type_id) + "\" was not part of Entity \"" + mName + "\"");

	return comp_t;
	
}

NSComponent * NSEntity::remove(const nsstring & guid)
{
	return remove(hash_id(guid));
}

void NSEntity::postUpdateAll(nsbool pUpdate)
{
	auto iter = mComponents.begin();
	while (iter != mComponents.end());
	{
		iter->second->postUpdate(pUpdate);
		++iter;
	}
}

void NSEntity::postUpdate(const nsstring & compType, nsbool update)
{
	NSComponent * comp_t = get(compType);
	if (comp_t != NULL)
		comp_t->postUpdate(update);
}

nsbool NSEntity::updatePosted(const nsstring & compType)
{
	NSComponent * comp_t = get(compType);
	if (comp_t != NULL)
		return comp_t->updatePosted();
	return false;
}

void NSEntity::updateScene()
{
	NSScene * scene = nsengine.currentScene();
	if (scene != NULL)
		scene->updateCompMaps(mPlugID, mID);
}

NSEntity & NSEntity::operator=(NSEntity rhs)
{
	swap(*this, rhs);
	return *this;
}

void swap(NSEntity & first, NSEntity & second)
{
	std::swap(first, second);
}
