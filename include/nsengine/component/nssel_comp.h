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

#include "nscomponent.h"
#include "nsunordered_set.h"
#include "nsvideo_driver.h"

class nsshader;
class nstimer;
class nsgl_buffer;
class nsscene;
class nssel_comp;

struct sel_per_scene_info : public nsvideo_object
{
	sel_per_scene_info(nssel_comp * owner_, nsscene * scn);
	~sel_per_scene_info();

	void video_context_init();

	nssel_comp * owner;
	nsscene * scene;
	ui_uset m_selection;
	bool m_selected;
};

class nssel_comp : public nscomponent
{
public:

	friend nsscene;
	
	template <class PUPer>
	friend void pup(PUPer & p, nssel_comp & sc);

	nssel_comp();

	nssel_comp(const nssel_comp & copy);

	~nssel_comp();
	
	void clear_all_selections();

	void enable_draw(bool enable_);

	void enable_move(const bool & enable_);

	bool empty();

	const fvec4 & default_color();

	float mask_alpha();

	const fvec4 & color();

	virtual void init();

	bool draw_enabled();

	bool move_enabled();

	bool selected(nsscene * scn);

	void enable_transparent_picking(bool enable);

	bool transparent_picking_enabled() const;

	ui_uset * selection(nsscene * scn);

	virtual void pup(nsfile_pupper * p);

	sel_per_scene_info * scene_info(nsscene * scn);

	void set_default_sel_color(const fvec4 & col_);

	void set_mask_alpha(const float & alpha_);

	void set_selected(nsscene * scn, bool selected_);

	void set_color(const fvec4 & col_);

	nssel_comp & operator=(nssel_comp rhs_);

private:

	std::unordered_map<nsscene*, sel_per_scene_info*> m_scene_selection;

	fvec4 m_default_sel_color;
	fvec4 m_sel_color;
	float m_mask_alpha;
	bool m_draw_enabled;
	bool m_move_with_input;
	bool m_transparent_picking_enabled;
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
