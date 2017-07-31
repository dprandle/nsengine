#ifndef NSWORLD_DATA_H
#define NSWORLD_DATA_H

#include <nsunordered_map.h>
#include <nssignal.h>

class nstform_ent_chunk;
class nsprefab;

typedef std::unordered_map<uint32,nstform_ent_chunk*> chunk_map;

class nsworld_data
{
  public:

	nsworld_data() {}
	~nsworld_data() {}

	void init() {}

	chunk_map::iterator begin();

	chunk_map::iterator end();

	nstform_ent_chunk * create_chunk(const nsstring & name);

	nstform_ent_chunk * create_chunk_from_prefab(nsprefab * pf, const nsstring & name_wanted_if_different_than_prefab = "");

	nstform_ent_chunk * chunk(const nsstring & name);

	nstform_ent_chunk * chunk(uint32 id);

	bool save_chunk_to_prefab(const nsstring & name, nsprefab * pf);

	bool save_chunk_to_prefab(uint32 chunk_id, nsprefab * pf);

	bool save_chunk_to_prefab(nstform_ent_chunk * chnk, nsprefab * pf);
	
	int destroy_chunk(const nsstring & name);

	void release() {}

	ns::signal<nstform_ent_chunk*> chunk_added;
	ns::signal<nstform_ent_chunk*> chunk_removed;	

  private:

	chunk_map m_chunks;
	
};

#endif
