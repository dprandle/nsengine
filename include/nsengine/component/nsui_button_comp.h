/*!
\file nsui_button_comp.h

\brief Header file for nsui_button_comp class

This file contains all of the neccessary declarations for the nsui_button_comp class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#ifndef NSUI_BUTTON_COMP_H
#define NSUI_BUTTON_COMP_H

#include <nscomponent.h>
#include <nsstring.h>
#include <nsrouter.h>

class nsui_button_comp;

enum button_change
{
	tex_coord_rect,
	change_color,
	color_multiplier
};

class nsui_button_comp : public nscomponent
{
public:
	template <class PUPer>
	friend void pup(PUPer & p, nsui_button_comp & tc);

	friend class nsui_system;

	nsui_button_comp();
	nsui_button_comp(const nsui_button_comp & rhs);

	virtual ~nsui_button_comp();

	virtual nsui_button_comp * copy(const nscomponent * comp_);

	virtual void init();

	virtual void pup(nsfile_pupper * p);

	nsui_button_comp & operator=(nsui_button_comp rhs_);
	
	ns::signal<> clicked;
	ns::signal<> pressed;
	ns::signal<> released;

	fvec4 colors[3];
	fvec4 tex_coord_rects[3];
	
	bool is_pressed;
	bool is_hover;

	button_change change_button_using;

  private:
	fvec4 m_mat_norm_color;
	bool m_mat_color_mode;
	fvec4 m_mat_norm_tex_rect;
};

template <class PUPer>
void pup(PUPer & p, nsui_button_comp & tc)
{
	// do nothing for now
}

#endif
