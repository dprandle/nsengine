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

#include <nstform_comp.h>
#include <nsresource.h>
#include <nsengine.h>

class nscomponent;
class nsentity : public nsresource
{
public:
	typedef std::unordered_map<uint32, nscomponent*> comp_set;

	template <class PUPer>
	friend void pup(PUPer & p, nsentity & ent);

	nsentity();

	
	~nsentity();

	bool add(nscomponent * pComp);

	comp_set::iterator begin();
	comp_set::iterator end();

	void clear();

	bool copy_comp(nscomponent * toCopy, bool overwrite = true);

	bool copy(nsresource * to_copy);

	template<class comp_type>
	comp_type * create()
	{
		uint32 tid = nse.type_id(std::type_index(typeid(comp_type)));
		return static_cast<comp_type*>(create(tid));
	}

	nscomponent * create(const nsstring & guid);

	nscomponent * create(uint32 type_id);

	template<class comp_type>
	bool del()
	{
		uint32 tid = nse.type_id(std::type_index(typeid(comp_type)));
		return del(tid);
	}

	bool del(uint32 type_id);

	bool del(const nsstring & guid);

	template<class comp_type>
	comp_type * get()
	{
		uint32 tid = nse.type_id(std::type_index(typeid(comp_type)));
		return static_cast<comp_type*>(get(tid));
	}

	nscomponent * get(uint32 type_id);

	nscomponent * get(const nsstring & guid);

	uint32 count();

	virtual void name_change(const uivec2 & oldid, const uivec2 newid);

	/*!
	Get the other resources that this Entity uses. This is given by all the components attached to the entity.
	\return Map of resource ID to resource type containing all used resources
	*/
	virtual uivec3_vector resources();
	
	template<class comp_type>
	bool has()
	{
		uint32 tid = nse.type_id(std::type_index(typeid(comp_type)));
		return (has(tid));
	}

	bool has(uint32 type_id);

	bool has(const nsstring & guid);

	void init();

	template<class comp_type>
	comp_type * remove()
	{
		uint32 tid = nse.type_id(std::type_index(typeid(comp_type)));
		return static_cast<comp_type*>(remove(tid));
	}

	// LEFT OFF HERE
	nscomponent * remove(uint32 type_id);

	nscomponent * remove(const nsstring & guid);
	
	void post_update_all(bool pUpdate);

	template<class comp_type>
	void post_update(bool update)
	{
		post_update(comp_type::getTypeString(), update);
	}

	void post_update(const nsstring & compType, bool update);

	virtual void pup(nsfile_pupper * p);

	void update_scene();

	template<class comp_type>
	bool update_posted()
	{
		return update_posted(comp_type::getTypeString());
	}

	bool update_posted(const nsstring & compType);

	

private:
	comp_set m_components;
};

template <class PUPer>
void pup(PUPer & p, nsentity & ent)
{
	uint32 size = ent.count();

	if (ent.has<nstform_comp>())
		size -= 1;

	pup(p, size, "size");

	if (p.mode() == PUP_IN)
	{
		for (uint32 i = 0; i < size; ++i)
		{
			nsstring k; nscomponent * v = NULL;
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
			uint32 tform_typeid = nse.type_id(std::type_index(typeid(nstform_comp)));
			if (iter->first != tform_typeid)
			{
				nsstring k = nse.guid(iter->first);
				pup(p, k, "comptype");
				iter->second->pup(&p);
			}
			++iter;
		}
	}
}

#endif
