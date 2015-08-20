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

#include <nscomponent.h>
#include <nsunordered_set.h>

class nsshader;
class nstimer;

class nssel_comp : public nscomponent
{
public:
	template <class PUPer>
	friend void pup(PUPer & p, nssel_comp & sc);

	nssel_comp();
	~nssel_comp();

	bool add(uint32 tform_id_);

	ui_uset::iterator begin();

	bool contains(uint32 tform_id_);

	nssel_comp * copy(const nscomponent* copy_);

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

	virtual void pup(nsfile_pupper * p);

	void set_default_sel_color(const fvec4 & col_);

	void set_mask_alpha(const float & alpha_);

	void set_selected(bool selected_);

	void set_color(const fvec4 & col_);

	nssel_comp & operator=(const nssel_comp & rhs_);

private:
	fvec4 m_default_sel_color;
	fvec4 m_sel_color;
	float m_mask_alpha;
	bool m_selected;
	bool m_draw_enabled;
	bool m_move_with_input;
	ui_uset m_selection;
};

template <class PUPer>
void pup(PUPer & p, nssel_comp & sc)
{
	pup(p, sc.m_default_sel_color, "default_sel_color");
	pup(p, sc.m_sel_color, "sel_color");
	pup(p, sc.m_mask_alpha, "mask_alpha");
	pup(p, sc.m_draw_enabled, "draw_enabled");
}

#endif
