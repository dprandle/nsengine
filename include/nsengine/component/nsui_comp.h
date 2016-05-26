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
	template <class PUPer>
	friend void pup(PUPer & p, nsui_comp & tc);

	nsui_comp();
	nsui_comp(const nsui_comp & rhs);

	virtual ~nsui_comp();

	virtual nsui_comp * copy(const nscomponent * comp_);

	virtual void init();

	virtual void pup(nsfile_pupper * p);

	nsui_comp & operator=(nsui_comp rhs_);

	uivec2 content_shader_id;
	uivec2 border_shader_id;
	uivec2 text_shader_id;
	uivec2 mat_id;
	uivec2 font_id;
	fvec4 border;
	fvec4 border_color;
	nsstring text;
	bool show;
	bool focused;
	bool text_editable;
};

template <class PUPer>
void pup(PUPer & p, nsui_comp & tc)
{
	// do nothing for now
}

#endif
