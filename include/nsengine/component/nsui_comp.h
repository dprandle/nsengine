/*!
\file nsui_comp.h

\brief Header file for nsui_comp class

This file contains all of the neccessary declarations for the nsui_comp class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#ifndef NSUI_COMP_H
#define NSUI_COMP_H

#include <nscomponent.h>
#include <nsstring.h>
#include <nsrouter.h>

class nsmaterial;
class nsfont;
class nsui_comp;

class nsui_comp : public nscomponent
{
public:

	struct widget_content
	{
		fvec2 nsize;
		uivec2 mat_id;
		fvec4 tex_coords_rect; // to remove in a bit
		uivec2 font_id;
		nsstring text;
	};

	struct widget_outer
	{
		uivec2 mat_id;
		uivec4 padding; // pixels
		uivec4 border; // pixels
		uivec4 margin; // pixels
		fvec4 border_color;
		fvec4 padding_tex_coords_rect; // to remove in a bit
	};

	template <class PUPer>
	friend void pup(PUPer & p, nsui_comp & tc);

	nsui_comp();
	nsui_comp(const nsui_comp & rhs);

	virtual ~nsui_comp();

	virtual nsui_comp * copy(const nscomponent * comp_);

	virtual void init();

	virtual void pup(nsfile_pupper * p);

	nsui_comp & operator=(nsui_comp rhs_);

	void add_child(nsentity * child);
	void remove_child(nsentity * child);

	bool is_child(nsentity * child);
	void set_parent(nsentity * parent);
	const uivec2 & parent();

	widget_content content_properties;
	widget_outer outer_properties;
	
	uivec2 content_shader_id;
	uivec2 border_shader_id;

	bool show;
	fvec2 center_npos;
	bool resize_with_parent;
	int32 layer;	

	ns::signal<> clicked;
	ns::signal<> pressed;
	ns::signal<> released;
	
	bool is_pressed;
	bool is_hover;
  protected:
	uivec2 m_parent;
	uivec2_vector m_children;
};

template <class PUPer>
void pup(PUPer & p, nsui_comp & tc)
{
	// do nothing for now
}

#endif
