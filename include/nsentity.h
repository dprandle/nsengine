/*! 
	\file nsentity.h
	
	\brief Header file for NSEntity class

	This file contains all of the neccessary declarations for the NSEntity class.

	\author Daniel Randle
	\date November 23 2013
	\copywrite Earth Banana Games 2013
*/

#ifndef NSENTITY_H
#define NSENTITY_H
#include <nsglobal.h>
#include <nstformcomp.h>
#include <nsanimcomp.h>
#include <nscamcomp.h>
#include <nsrendercomp.h>
#include <nslightcomp.h>
#include <nsresource.h>
#include <nsengine.h>
#include <nspupper.h>

class NSComponent;
class NSEntity : public NSResource
{
public:
	typedef std::unordered_map<nsstring, NSComponent*> CompSet;

	template <class PUPer>
	friend void pup(PUPer & p, NSEntity & ent);

	friend void swap(const NSEntity & first, const NSEntity & second);

	NSEntity();
	~NSEntity();

	NSEntity(NSEntity & copy);

	nsbool add(NSComponent * pComp);

	CompSet::iterator compBegin();
	CompSet::iterator compEnd();

	void clear();

	nsbool copy(NSComponent * toCopy, bool overwrite = true);

	template<class CompType>
	CompType * create()
	{
		CompType * comp = new CompType();
		comp->init();
		if (!add(comp))
		{
			dprint("NSEntity::createComponent - Failed adding comp type " + CompType::getTypeString());
			delete comp;
			return NULL;
		}
		return comp;
	}

	NSComponent * create(const nsstring & compType);

	template<class CompType>
	bool del()
	{
		return del(CompType::getTypeString());
	}

	bool del(const nsstring & compType);

	template<class CompType>
	CompType * get()
	{
		CompSet::iterator iter = mComponents.find(CompType::getTypeString());
		if (iter != mComponents.end()) // check to make sure it has the comp or else the cast could mess up memory
			return (CompType*)iter->second;
		return NULL;
	}

	NSComponent * get(const nsstring & compType);

	nsuint compCount();

	virtual void nameChange(const uivec2 & oldid, const uivec2 newid);

	/*!
	Get the other resources that this Entity uses. This is given by all the components attached to the entity.
	\return Map of resource ID to resource type containing all used resources
	*/
	virtual uivec2array resources();
	
	template<class CompType>
	bool has()
	{
		return (has(CompType::getTypeString()));
	}

	bool has(const nsstring & compType);

	void init();

	template<class CompType>
	CompType * remove()
	{
		NSComponent * comp = remove(CompType::getTypeString());
		if (comp != NULL)
			return static_cast<CompType*>(comp);
		return comp;
	}

	NSComponent * remove(const nsstring & compType);

	void postUpdateAll(nsbool pUpdate);

	template<class CompType>
	void postUpdate(nsbool update)
	{
		postUpdate(CompType::getTypeString(), update);
	}

	void postUpdate(const nsstring & compType, nsbool update);

	virtual void pup(NSFilePUPer * p);

	void updateScene();

	template<class CompType>
	nsbool updatePosted()
	{
		return updatePosted(CompType::getTypeString());
	}

	nsbool updatePosted(const nsstring & compType);

	virtual nsstring typeString() { return getTypeString(); }

	nsstring managerTypeString() { return getManagerTypeString(); }

	static nsstring getTypeString() { return ENTITY_TYPESTRING; }

	static nsstring getManagerTypeString() { return ENTITY_MANAGER_TYPESTRING; }

	NSEntity & operator=(NSEntity rhs);

private:
	CompSet mComponents;
};

void swap(NSEntity & first, NSEntity & second);

template <class PUPer>
void pup(PUPer & p, NSEntity & ent)
{
	nsuint size = ent.compCount();

	if (ent.has<NSTFormComp>())
		size -= 1;

	pup(p, size, "size");

	if (p.mode() == PUP_IN)
	{
		for (nsuint i = 0; i < size; ++i)
		{
			nsstring k; NSComponent * v = NULL;
			pup(p, k, "comptype");
			v = ent.create(k);
			v->pup(&p);
		}
	}
	else
	{
		auto iter = ent.mComponents.begin();
		while (iter != ent.mComponents.end())
		{
			if (iter->first != NSTFormComp::getTypeString())
			{
				nsstring k = iter->first;
				pup(p, k, "comptype");
				iter->second->pup(&p);
			}
			++iter;
		}
	}
}

#endif