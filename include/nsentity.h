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
	typedef std::unordered_map<nsuint, NSComponent*> CompSet;

	template <class PUPer>
	friend void pup(PUPer & p, NSEntity & ent);

	friend void swap(const NSEntity & first, const NSEntity & second);

	NSEntity();
	~NSEntity();

	NSEntity(NSEntity & copy);

	nsbool add(NSComponent * pComp);

	CompSet::iterator begin();
	CompSet::iterator end();

	void clear();

	nsbool copy(NSComponent * toCopy, bool overwrite = true);

	template<class CompType>
	CompType * create()
	{
		nsuint tid = nsengine.typeID(std::type_index(typeid(CompType)));
		return static_cast<CompType*>(create(tid));
	}

	NSComponent * create(const nsstring & guid);

	NSComponent * create(nsuint type_id);

	template<class CompType>
	bool del()
	{
		nsuint tid = nsengine.typeID(std::type_index(typeid(CompType)));
		return del(tid);
	}

	bool del(nsuint type_id);

	bool del(const nsstring & guid);

	template<class CompType>
	CompType * get()
	{
		nsuint tid = nsengine.typeID(std::type_index(typeid(CompType)));
		return static_cast<CompType*>(get(tid));
	}

	NSComponent * get(nsuint type_id);

	NSComponent * get(const nsstring & guid);

	nsuint count();

	virtual void nameChange(const uivec2 & oldid, const uivec2 newid);

	/*!
	Get the other resources that this Entity uses. This is given by all the components attached to the entity.
	\return Map of resource ID to resource type containing all used resources
	*/
	virtual uivec2array resources();
	
	template<class CompType>
	bool has()
	{
		nsuint tid = nsengine.typeID(std::type_index(typeid(CompType)));
		return (has(tid));
	}

	bool has(nsuint type_id);

	bool has(const nsstring & guid);

	void init();

	template<class CompType>
	CompType * remove()
	{
		nsuint tid = nsengine.typeID(std::type_index(typeid(CompType)));
		return static_cast<CompType*>(remove(tid));
	}

	// LEFT OFF HERE
	NSComponent * remove(nsuint type_id);

	NSComponent * remove(const nsstring & guid);
	
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
	nsuint size = ent.count();

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
			nsuint tform_typeid = nsengine.typeID(std::type_index(typeid(NSTFormComp)));
			if (iter->first != tform_typeid)
			{
				nsstring k = nsengine.guid(iter->first);
				pup(p, k, "comptype");
				iter->second->pup(&p);
			}
			++iter;
		}
	}
}

#endif
