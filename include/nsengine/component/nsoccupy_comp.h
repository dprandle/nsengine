/*!
\file nsoccupy_comp.h

\brief Header file for nsoccupy_comp class

This file contains all of the neccessary declarations for the nsoccupy_comp class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#ifndef NSOCCUPY_COMP_H
#define NSOCCUPY_COMP_H

#define R_FACTOR 0.25f

#include "nscomponent.h"

class nsoccupy_comp : public nscomponent
{
public:

	template <class PUPer>
	friend void pup(PUPer & p, nsoccupy_comp & oc);

	nsoccupy_comp();

	nsoccupy_comp(const nsoccupy_comp & copy);

	virtual ~nsoccupy_comp();

	bool add(int32 x_, int32 y_, int32 z_);

	bool add(const ivec3 & grid_pos_);

	ivec3_vector::iterator begin();

	void build(const nsbounding_box & box_);

	bool contains(int32 x_, int32 y_, int32 z_);

	bool contains(const ivec3 & grid_pos_);

	void clear();

	void enable_draw(bool enable_);

	ivec3_vector::iterator end();

	const ivec3_vector & spaces() const;

	virtual void init();

	bool draw_enabled();

	const uivec2 & material_id();

	const uivec2 & mesh_id();

	virtual void name_change(uint32 plug_id_, uint32 old_id_, uint32 new_id_);

	virtual void pup(nsfile_pupper * p);

	bool remove(int32 x_, int32 y_, int32 z_);

	bool remove(const ivec3 & grid_pos_);

	virtual uivec3_vector resources();

	void set_mesh_id(const uivec2 & mesh_id_);

	void set_material_id(const uivec2 & mat_id_);

	nsoccupy_comp & operator=(nsoccupy_comp rhs_);

private:
	ivec3_vector m_spaces;
	uivec2 m_mesh_id;
	uivec2 m_mat_id;
	bool m_draw_enabled;
};

template <class PUPer>
void pup(PUPer & p, nsoccupy_comp & oc)
{
	pup(p, oc.m_spaces, "spaces");
	pup(p, oc.m_mesh_id, "mesh_id");
	pup(p, oc.m_mat_id, "mat_id");
	pup(p, oc.m_draw_enabled, "draw_enabled");
}

#endif
