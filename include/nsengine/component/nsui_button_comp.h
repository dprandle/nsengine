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
	ns::signal<bool> toggled;

	struct button_state
	{
		button_state():
			mat_color(1.0f),
			mat_color_mult(1.0f),
			mat_color_add(0.0f),
			mat_tex_coord_rect(0.0f,0.0f,1.0f,1.0f),
			border_color(1.0f),
			border_color_mult(1.0f),
			border_color_add(0.0f),
			top_border_radius(0.0f),
			bottom_border_radius(0.0f),
			border_tex_coord_rect(0.0f,0.0f,1.0f,1.0f),
			border_size(1.0f),
			text_color(1.0f),
			text_color_mult(1.0f),
			text_color_add(0.0f),
			text_tex_coord_rect(0.0f,0.0f,1.0f,1.0f)
		{}
		
		fvec4 mat_color;
		fvec4 mat_color_mult;
		fvec4 mat_color_add;
		fvec4 mat_tex_coord_rect;

		fvec4 border_color;
		fvec4 border_color_add;
		fvec4 border_color_mult;
		fvec4 border_tex_coord_rect;
		fvec4 border_size;
		fvec4 top_border_radius;
		fvec4 bottom_border_radius;


		fvec4 text_color;
		fvec4 text_color_add;
		fvec4 text_color_mult;
		fvec4 text_tex_coord_rect;
	};
		
	bool is_pressed;
	bool is_hover;
	bool is_toggled;
	bool is_enabled;
	
	bool toggle_enabled;

	button_state button_states[4];
	button_state toggled_button_states[4];

  private:
	bool m_mat_color_mode;
};

template <class PUPer>
void pup(PUPer & p, nsui_button_comp & tc)
{
	// do nothing for now
}

#endif
