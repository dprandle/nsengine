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

class nsmaterial;

struct ui_widget
{
	ui_widget * parent;
	nsmaterial * bg_mat;
	fvec2 center_npos;
	fvec2 size;
};

enum layout_dir
{
	layout_horizontal,
	layout_verticle
};

struct ui_layout : ui_widget
{
	layout_dir ldir;
	std::vector<ui_widget*> children;
};

struct ui_button : ui_widget
{
	bool stretch_to_fit;
	nsstring text;
	bool pressed;
	bool hover;
	nsmaterial * hover_mat;
	nsmaterial * pressed_mat;
};

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

  private:
	ui_widget * m_widget;
};

template <class PUPer>
void pup(PUPer & p, nsui_comp & tc)
{
	// do nothing for now
}

#endif
