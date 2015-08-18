/*! 
	\file nsentity.h
	
	\brief Header file for nsentity class

	This file contains all of the neccessary declarations for the nsentity class.

	\author Daniel Randle
	\date November 23 2013
	\copywrite Earth Banana Games 2013
*/

#ifndef NSENTITY_H
#define NSENTITY_H
#include <nsglobal.h>
#include <nstform_comp.h>
#include <nsanim_comp.h>
#include <nscam_comp.h>
#include <nsrender_comp.h>
#include <nslight_comp.h>
#include <nsresource.h>
#include <nsengine.h>
#include <nspupper.h>

class NSComponent;
class nsentity : public nsresource
{
public:
	typedef std::unordered_map<uint32, NSComponent*> comp_set;

	template <class PUPer>
	friend void pup(PUPer & p, nsentity & ent);

	friend void swap(const nsentity & first, const nsentity & second);

	nsentity();
	~nsentity();

	nsentity(nsentity & copy);

	bool add(NSComponent * pComp);

	comp_set::iterator begin();
	comp_set::iterator end();

	void clear();

	bool copy(NSComponent * toCopy, bool overwrite = true);

	template<class comp_type>
	comp_type * create()
	{
		uint32 tid = nsengine.typeID(std::type_index(typeid(comp_type)));
		return static_cast<comp_type*>(create(tid));
	}

	NSComponent * create(const nsstring & guid);

	NSComponent * create(uint32 type_id);

	template<class comp_type>
	bool del()
	{
		uint32 tid = nsengine.typeID(std::type_index(typeid(comp_type)));
		return del(tid);
	}

	bool del(uint32 type_id);

	bool del(const nsstring & guid);

	template<class comp_type>
	comp_type * get()
	{
		uint32 tid = nsengine.typeID(std::type_index(typeid(comp_type)));
		return static_cast<comp_type*>(get(tid));
	}

	NSComponent * get(uint32 type_id);

	NSComponent * get(const nsstring & guid);

	uint32 count();

	virtual void name_change(const uivec2 & oldid, const uivec2 newid);

	/*!
	Get the other resources that this Entity uses. This is given by all the components attached to the entity.
	\return Map of resource ID to resource type containing all used resources
	*/
	virtual uivec2array resources();
	
	template<class comp_type>
	bool has()
	{
		uint32 tid = nsengine.typeID(std::type_index(typeid(comp_type)));
		return (has(tid));
	}

	bool has(uint32 type_id);

	bool has(const nsstring & guid);

	void init();

	template<class comp_type>
	comp_type * remove()
	{
		uint32 tid = nsengine.typeID(std::type_index(typeid(comp_type)));
		return static_cast<comp_type*>(remove(tid));
	}

	// LEFT OFF HERE
	NSComponent * remove(uint32 type_id);

	NSComponent * remove(const nsstring & guid);
	
	void post_update_all(bool pUpdate);

	template<class comp_type>
	void post_update(bool update)
	{
		post_update(comp_type::getTypeString(), update);
	}

	void post_update(const nsstring & compType, bool update);

	virtual void pup(NSFilePUPer * p);

	void update_scene();

	template<class comp_type>
	bool update_posted()
	{
		return update_posted(comp_type::getTypeString());
	}

	bool update_posted(const nsstring & compType);

	nsentity & operator=(nsentity rhs);

private:
	comp_set m_components;
};

template <class PUPer>
void pup(PUPer & p, nsentity & ent)
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
		auto iter = ent.m_components.begin();
		while (iter != ent.m_components.end())
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
