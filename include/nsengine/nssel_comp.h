/*!
\file nssel_comp.h

\brief Header file for nssel_comp class

This file contains all of the neccessary declarations for the nssel_comp class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#ifndef NSSELCOMP_H
#define NSSELCOMP_H

#define DEFAULT_SEL_R 0.0f
#define DEFAULT_SEL_G 1.0f
#define DEFAULT_SEL_B 1.0f
#define DEFAULT_SEL_A 0.5f
#define DEFAULT_SEL_MASK_A 0.2f

#include <nscomponent.h>
#include <nsunordered_set.h>

class nsshader;
class nstimer;
class nsbuffer_object;

class nssel_comp : public nscomponent
{
public:
	template <class PUPer>
	friend void pup(PUPer & p, nssel_comp & sc);

	nssel_comp();

	nssel_comp(const nssel_comp & copy);

	~nssel_comp();

	bool add(uint32 tform_id_);

	ui_uset::iterator begin();

	bool contains(uint32 tform_id_);

	uint32 count();

	void clear();

	void enable_draw(bool enable_);

	void enable_move(const bool & enable_);

	ui_uset::iterator end();

	bool empty();

	const fvec4 & default_color();

	const float & mask_alpha();

	const fvec4 & color();

	virtual void init();

	bool draw_enabled();

	const bool & move_enabled();

	bool selected();

	bool remove(uint32 tform_id_);

	bool set(uint32 tform_id_);

	void enable_transparent_picking(bool enable);

	bool transparent_picking_enabled() const;

	nsbuffer_object * transform_buffer();

	virtual void pup(nsfile_pupper * p);

	void set_default_sel_color(const fvec4 & col_);

	void set_mask_alpha(const float & alpha_);

	void set_selected(bool selected_);

	void set_color(const fvec4 & col_);

	nssel_comp & operator=(nssel_comp rhs_);

private:
	nsbuffer_object * m_tform_buffer;
	fvec4 m_default_sel_color;
	fvec4 m_sel_color;
	float m_mask_alpha;
	bool m_selected;
	bool m_draw_enabled;
	bool m_move_with_input;
	bool m_transparent_picking_enabled;
	ui_uset m_selection;
};

template <class PUPer>
void pup(PUPer & p, nssel_comp & sc)
{
	pup(p, sc.m_default_sel_color, "default_sel_color");
	pup(p, sc.m_sel_color, "sel_color");
	pup(p, sc.m_mask_alpha, "mask_alpha");
	pup(p, sc.m_draw_enabled, "draw_enabled");
	pup(p, sc.m_transparent_picking_enabled, "transparent_picking_enabled");
}

#endif
