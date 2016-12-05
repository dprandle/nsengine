#include <component/nscomponent.h>
#include <nsengine.h>

class nsprefab_comp;
class nsentity;

class nsprefab_reference_comp : public nscomponent
{
  public:

	friend nsprefab_comp;
	
	nsprefab_reference_comp();

	nsprefab_reference_comp(const nsprefab_reference_comp & copy);

	~nsprefab_reference_comp();

	void init();

	void release();

	void finalize();

	void name_change(const uivec2 &, const uivec2);

	uivec3_vector resources();

	void pup(nsfile_pupper * p);

	nsentity * get_source_ent();

	nsentity * get_source_prefab();

	template<class T>
	T * get_source_comp()
	{
		uint32 tid = nse.type_id(std::type_index(typeid(T)));
		return static_cast<T*>(get_source_comp(tid));
	}

	nscomponent * get_source_comp(int tid);

	ns::signal<uint32, uint32> reference_id_changed;

  private:


	uint32 ref_id;
	uivec2 prefab_id;
	uivec2 ent_id;
};
