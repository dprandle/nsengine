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
	auto iter = toCopy.compBegin();
	while (iter != toCopy.compEnd())
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

	auto ret = mComponents.emplace(pComp->typeString(), pComp);
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

	if (has(toCopy->typeString()))
	{
		if (!overwrite)
			return false;
	}
	else
		nc = create(toCopy->typeString());

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

NSEntity::CompSet::iterator NSEntity::compBegin()
{
	return mComponents.begin();
}

NSEntity::CompSet::iterator NSEntity::compEnd()
{
	return mComponents.end();
}

NSComponent * NSEntity::create(const nsstring & compType)
{
	NSComponent * comp = nsengine.factory<NSCompFactory>(compType)->create();
	if (!add(comp))
	{
		dprint("NSEntity::createComponent - Failed adding comp type " + compType);
		delete comp;
		return NULL;
	}
	comp->init();
	return comp;
}

bool NSEntity::del(const nsstring & compType)
{
	auto iter = mComponents.find(compType);
	if (iter != mComponents.end())
	{
		delete iter->second;
		mComponents.erase(iter);
		updateScene();
		dprint("NSEntity::del - Deleting \"" + compType + "\" from Entity " + mName + "\"");
		return true;
	}
	dprint("NSEntity::del - Component type \"" + compType + "\" was not part of Entity \"" + mName + "\"");
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

nsuint NSEntity::compCount()
{
	return mComponents.size();
}

NSComponent * NSEntity::get(const nsstring & compType)
{
	CompSet::iterator iter = mComponents.find(compType);
	if (iter != mComponents.end()) // check to make sure it has the comp or else the cast could mess up memory
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

	// Go through each component and insert all used resources from each comp
	auto iter = mComponents.begin();
	while (iter != mComponents.end())
	{
		uivec2array tmp = iter->second->resources();
		ret.insert(ret.end(), tmp.begin(), tmp.end() );
		++iter;
	}
	return ret;
}

bool NSEntity::has(const nsstring & compType)
{
	CompSet::iterator iter = mComponents.find(compType);
	return (iter != mComponents.end());
}

void NSEntity::init()
{
	// do nothing
}

NSComponent * NSEntity::remove(const nsstring & typeName)
{
	NSComponent * comp = NULL;
	auto iter = mComponents.find(typeName);
	if (iter != mComponents.end())
	{
		comp = iter->second;
		comp->setOwner(NULL);
		mComponents.erase(iter);
		updateScene();
		dprint("NSEntity::remove - Removing \"" + typeName + "\" from Entity " + mName + "\"");
	}
	else
		dprint("NSEntity::remove - Component type \"" + typeName + "\" was not part of Entity \"" + mName + "\"");

	return comp;
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
	NSComponent * comp = get(compType);
	if (comp != NULL)
		comp->postUpdate(update);
}

nsbool NSEntity::updatePosted(const nsstring & compType)
{
	NSComponent * comp = get(compType);
	if (comp != NULL)
		return comp->updatePosted();
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