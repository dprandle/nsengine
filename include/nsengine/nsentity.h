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
	typedef std::unordered_map<uint32, NSComponent*> CompSet;

	template <class PUPer>
	friend void pup(PUPer & p, NSEntity & ent);

	friend void swap(const NSEntity & first, const NSEntity & second);

	NSEntity();
	~NSEntity();

	NSEntity(NSEntity & copy);

	bool add(NSComponent * pComp);

	CompSet::iterator begin();
	CompSet::iterator end();

	void clear();

	bool copy(NSComponent * toCopy, bool overwrite = true);

	template<class CompType>
	CompType * create()
	{
		uint32 tid = nsengine.typeID(std::type_index(typeid(CompType)));
		return static_cast<CompType*>(create(tid));
	}

	NSComponent * create(const nsstring & guid);

	NSComponent * create(uint32 type_id);

	template<class CompType>
	bool del()
	{
		uint32 tid = nsengine.typeID(std::type_index(typeid(CompType)));
		return del(tid);
	}

	bool del(uint32 type_id);

	bool del(const nsstring & guid);

	template<class CompType>
	CompType * get()
	{
		uint32 tid = nsengine.typeID(std::type_index(typeid(CompType)));
		return static_cast<CompType*>(get(tid));
	}

	NSComponent * get(uint32 type_id);

	NSComponent * get(const nsstring & guid);

	uint32 count();

	virtual void nameChange(const uivec2 & oldid, const uivec2 newid);

	/*!
	Get the other resources that this Entity uses. This is given by all the components attached to the entity.
	\return Map of resource ID to resource type containing all used resources
	*/
	virtual uivec2array resources();
	
	template<class CompType>
	bool has()
	{
		uint32 tid = nsengine.typeID(std::type_index(typeid(CompType)));
		return (has(tid));
	}

	bool has(uint32 type_id);

	bool has(const nsstring & guid);

	void init();

	template<class CompType>
	CompType * remove()
	{
		uint32 tid = nsengine.typeID(std::type_index(typeid(CompType)));
		return static_cast<CompType*>(remove(tid));
	}

	// LEFT OFF HERE
	NSComponent * remove(uint32 type_id);

	NSComponent * remove(const nsstring & guid);
	
	void postUpdateAll(bool pUpdate);

	template<class CompType>
	void postUpdate(bool update)
	{
		postUpdate(CompType::getTypeString(), update);
	}

	void postUpdate(const nsstring & compType, bool update);

	virtual void pup(NSFilePUPer * p);

	void updateScene();

	template<class CompType>
	bool updatePosted()
	{
		return updatePosted(CompType::getTypeString());
	}

	bool updatePosted(const nsstring & compType);

	NSEntity & operator=(NSEntity rhs);

private:
	CompSet mComponents;
};

template <class PUPer>
void pup(PUPer & p, NSEntity & ent)
{
	uint32 size = ent.count();

	if (ent.has<NSTFormComp>())
		size -= 1;

	pup(p, size, "size");

	if (p.mode() == PUP_IN)
	{
		for (uint32 i = 0; i < size; ++i)
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
			uint32 tform_typeid = nsengine.typeID(std::type_index(typeid(NSTFormComp)));
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
