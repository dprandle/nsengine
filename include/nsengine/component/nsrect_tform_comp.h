/*!
\file nsrect_tform_comp.h

\brief Header file for nsrect_tform_comp class

This file contains all of the neccessary declarations for the nsrect_tform_comp class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#ifndef NSRECT_TFORM_COMP_H
#define NSRECT_TFORM_COMP_H

#include <component/nscomponent.h>

class nsui_canvas_comp;

struct rect_tform_info
{
	rect_tform_info():
		anchor_rect(0.0,0.0,1.0,1.0),
		pixel_offset_rect(0,0,0,0),
		angle(0.0f),
		pivot(0,0),
		layer(0)
	{}

	fvec2 scale;
	fmat3 content_world_tform;	
	fvec4 anchor_rect;
	fvec4 pixel_offset_rect;
	float angle;
	fvec2 pivot;
	int32 layer;
	uint32 m_parent;
	ui_vector m_children;
};

class nsrect_tform_comp : public nscomponent
{
public:

	friend class nsui_canvas_comp;
	
	nsrect_tform_comp();
	nsrect_tform_comp(const nsrect_tform_comp & rhs);

	virtual ~nsrect_tform_comp();

	virtual nsrect_tform_comp * copy(const nscomponent * comp_);

	virtual void init();

	virtual void pup(nsfile_pupper * p);

	nsrect_tform_comp & operator=(nsrect_tform_comp rhs_);

	void add_child(nsrect_tform_comp * child);

	void remove_child(nsrect_tform_comp * child);

	bool has_child(nsrect_tform_comp * child);

	void set_parent(nsui_canvas_comp * canvas, nsrect_tform_comp * parent);

	nsrect_tform_comp * parent();

	rect_tform_info * tf_info();

	const fmat3 & content_transform(nsui_canvas_comp * canvas);

	const fvec2 & content_world_scale(nsui_canvas_comp * canvas);
	
	void update_recursively(nsui_canvas_comp * canvas, const fvec2 & pscreen_size);
	
  private:
	
	rect_tform_info pci;
};

template<class PUPer>
void pup(PUPer & p, rect_tform_info & pci, const nsstring & var_name)
{
	pup(p, pci.anchor_rect, var_name + ".anchor_rect");
	pup(p, pci.pixel_offset_rect, var_name + ".pixel_offset_rect");
	pup(p, pci.angle, var_name + ".angle");
	pup(p, pci.pivot, var_name + ".pivot");
	pup(p, pci.layer, var_name + ".layer");
}
#endif
