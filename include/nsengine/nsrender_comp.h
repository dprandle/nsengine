/*! 
	\file nsrender_comp.h
	
	\brief Header file for nsrender_comp class

	This file contains all of the neccessary declarations for the nsrender_comp class.

	\author Daniel Randle
	\date December 17 2013
	\copywrite Earth Banana Games 2013
*/

#ifndef NSRENDER_COMP_H
#define NSRENDER_COMP_H

#include <vector>
#include <nscomponent.h>
#include <map>
#include <nsmath.h>

class nsmesh;
class nstimer;
class nsmaterial;

class nsrender_comp : public NSComponent
{
public:

	template <class PUPer>
	friend void pup(PUPer & p, nsrender_comp & rc);

	nsrender_comp();
	~nsrender_comp();

	void clear_mats();

	nsrender_comp* copy(const NSComponent* copy_);

	bool cast_shadow();

	uivec2 material_id(uint32 submesh_index_);

	const uivec2 & mesh_id();

	virtual void name_change(const uivec2 & old_id_, const uivec2 new_id_);

	/*!
	Get the resources that the component uses. The render comp uses a mesh and possibly multiple materials.
	/return Map of resource ID to resource type containing mesh ID and all material IDs.
	*/
	virtual uivec2array resources();

	bool has_material(uint32 submesh_index_);

	void init();

	uivec2uimap::iterator begin_mat();

	uivec2uimap::const_iterator begin_mat() const;

	uivec2uimap::iterator end_mat();

	uivec2uimap::const_iterator end_mat() const;

	bool remove_material(uint32 submesh_index_);

	bool remove_all_materials(const uivec2 & to_remove_);

	bool replace_material(const uivec2 & old_id_, const uivec2 & new_id_);

	bool replace_material(uint32 old_plug_id_, uint32 old_res_id, uint32 new_plug_id, uint32 new_res_id)
	{
		return replace_material(uivec2(old_plug_id_, old_res_id), uivec2(new_plug_id, new_res_id));
	}

	virtual void pup(nsfile_pupper * p);

	void set_cast_shadow(bool enable_);

	bool set_material(uint32 submesh_index_, const uivec2 & mat_id_, bool replace_ = false);

	bool set_material(uint32 submesh_index_, uint32 mat_plug_id, uint32 mat_id_, bool replace_ = false)
	{
		return set_material(submesh_index_, uivec2(mat_plug_id, mat_id_), replace_);
	}

	void set_mesh_id(const uivec2 & mesh_id_);

	void set_mesh_id(uint32 plug_id_, uint32 res_id_)
	{
		m_mesh_id.x = plug_id_; m_mesh_id.y = res_id_;
		post_update(true);
	}

	nsrender_comp & operator=(const nsrender_comp & rhs_);

private:
	bool m_cast_shadow;
	uivec2 m_mesh_id;
	uivec2uimap m_mats;
};

template <class PUPer>
void pup(PUPer & p, nsrender_comp & rc)
{
	pup(p, rc.m_cast_shadow, "cast_shadow");
	pup(p, rc.m_mesh_id, "mesh_id");
	pup(p, rc.m_mats, "mats");
}

#endif
