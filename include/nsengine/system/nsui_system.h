/*!
\file nsui_system.h

\brief Header file for nsui_system class

This file contains all of the neccessary declarations for the nsui_system class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/


#ifndef NSUI_SYSTEM_H
#define NSUI_SYSTEM_H

#define START_MAX_ENTS 1024
#define UI_MOUSE_PRESS "ui_mouse_press"
#define UI_MOUSE_RELEASE "ui_mouse_release"

#include "nssystem.h"
#include "nsrender_system.h"

class nsui_canvas_comp;
class nsrect_tform_comp;
class nsui_button_comp;
class nsentity;
struct viewport;

class nsui_system : public nssystem
{
  public:
	
	nsui_system();

	~nsui_system();

	virtual void init();

	void setup_input_map(nsinput_map * imap, const nsstring & global_imap_ctxt_name);

	virtual void release();

	nsentity * focused_ui_element();

	virtual void update();

	int32 update_priority();

	bool mpos_over_element(const fvec2 & norm_mpos, viewport * vp);

  private:
	void _sort_ents(nsui_canvas_comp * uicc);

	bool _handle_mouse_event(nsmouse_move_event * evnt);
	bool _handle_mouse_press(nsaction_event * evnt);
	bool _handle_mouse_release(nsaction_event * evnt);	
	bool _handle_key_press(nskey_event * evnt);
	
	void _recursive_update(nsrect_tform_comp * child);

	nsui_button_comp * m_pressed_button;
	nsentity * m_focused_ui_ent;
	uivec2 imap_id;
};

#endif
