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

#include <nscomponent.h>

class nsui_canvas_comp;

struct per_canvas_info
{
	per_canvas_info():
		anchor_rect(0.0,0.0,1.0,1.0),
		pixel_offset_rect(0,0,0,0),
		angle(0.0f),
		pivot(0,0),
		layer(0)
	{}
		
	fvec4 anchor_rect;
	fvec4 pixel_offset_rect;
	float angle;
	fvec2 pivot;
	int32 layer;
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

	void finalize();

	virtual void pup(nsfile_pupper * p);

	nsrect_tform_comp & operator=(nsrect_tform_comp rhs_);

	void add_child(nsrect_tform_comp * child);

	void remove_child(nsrect_tform_comp * child);

	bool has_child(nsrect_tform_comp * child);

	void set_parent(nsui_canvas_comp * canvas, nsrect_tform_comp * parent);

	nsrect_tform_comp * parent(nsui_canvas_comp * canvas);

	per_canvas_info * canvas_info(nsui_canvas_comp * canvas);

	const fmat3 & content_transform(nsui_canvas_comp * canvas);

	const fmat3 & border_transform(nsui_canvas_comp * canvas);
	
	void update_recursively(nsui_canvas_comp * canvas, const fvec2 & pscreen_size);
	
  private:
	
	struct per_canvas_settings
	{
		per_canvas_settings():
			pci(),
			m_parent(nullptr),
			m_children()
		{}
		
		per_canvas_info pci;

		fmat3 content_world_tform;
		fmat3 border_world_tform;
		
		nsrect_tform_comp * m_parent;
		std::vector<nsrect_tform_comp*> m_children;		
	};
	
	std::unordered_map<nsui_canvas_comp *, per_canvas_settings> m_canvas_settings;
};

template<class PUPer>
void pup(PUPer & p, per_canvas_info & pci, const nsstring & var_name)
{
	pup(p, pci.anchor_rect, var_name + ".anchor_rect");
	pup(p, pci.pixel_offset_rect, var_name + ".pixel_offset_rect");
	pup(p, pci.angle, var_name + ".angle");
	pup(p, pci.pivot, var_name + ".pivot");
	pup(p, pci.layer, var_name + ".layer");
}
#endif
