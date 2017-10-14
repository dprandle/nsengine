#include <component/nscomponent.h>
#include <nsengine.h>

class nsentity;
class nsui_canvas;

class nsui_canvas_reference_comp : public nscomponent
{
	SLOT_OBJECT
	
  public:

	friend class nsui_canvas;
	
	nsui_canvas_reference_comp();

	nsui_canvas_reference_comp(const nsui_canvas_reference_comp & copy);

	nsui_canvas_reference_comp & operator=(nsui_canvas_reference_comp rhs);

	~nsui_canvas_reference_comp();

	template<class T>
	void add_restricted_type()
	{
		uint32 tid = nse.type_id(std::type_index(typeid(T)));
		add_restricted_type(tid);
	}

	template<class T>
	void remove_restricted_type()
	{
		uint32 tid = nse.type_id(std::type_index(typeid(T)));
		remove_restricted_type(tid);
	}

	template<class T>
	bool type_allowed()
	{
		uint32 tid = nse.type_id(std::type_index(typeid(T)));
		return type_allowed(tid);
	}


	void add_restricted_type(uint32 t);
	
	bool type_allowed(uint32 t);

	void remove_restricted_type(uint32 t);

	void init();

	void release();

	void finalize();

	void name_change(const uivec2 &, const uivec2);

	uivec3_vector resources();

	void pup(nsfile_pupper * p);

	nsentity * get_source_ent();

	void get_source_comp_set(std::set<uint32> & ret);

	nsui_canvas * get_source_prefab();

	template<class T>
	T * get_source_comp()
	{
		uint32 tid = nse.type_id(std::type_index(typeid(T)));
		return static_cast<T*>(get_source_comp(tid));
	}

	nscomponent * get_source_comp(uint32 tid);
	
	ns::signal<uint32, uint32> reference_id_changed;

  private:

	void link_to_source();

	void unlink_from_source();


	/*
	  This slot will emit the comp edit signal with the sources component as the arguement.
	  This will cause the entitiy to emit a component_edited signal for the sources edited component,
	  allowing the vid driver to re-analyse the instancing situation
	*/
	void on_source_comp_add(nsentity * source, nscomponent * comp);
	void on_source_comp_remove(nsentity * source, nscomponent * comp);
	void on_source_comp_edit(nsentity * source, nscomponent * comp);

	std::set<uint32> m_restricted_types;

	uint32 ref_id;
	uivec2 prefab_id;
	uint32 ent_id;
};
