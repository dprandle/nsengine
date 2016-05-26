/*!
\file nsinput_system.h

\brief Header file for nsinput_system class

This file contains all of the neccessary declarations for the nsinput_system class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#ifndef NSINPUT_SYSTEM_H
#define NSINPUT_SYSTEM_H

#include <nssystem.h>
#include <nsinput_map.h>

fvec2 platform_normalized_mpos();

class nsinput_system : public nssystem
{
public:

	typedef std::vector<nsinput_map::ctxt*> context_stack;

	nsinput_system();

	virtual ~nsinput_system();

	const fvec2 & cursor_pos();

	bool key_event(nskey_event * evnt);

	bool mouse_button_event(nsmouse_button_event * evnt);

	bool mouse_move_event(nsmouse_move_event * evnt);

	bool mouse_scroll_event(nsmouse_scroll_event * evnt);
	
	virtual void init();

	void set_input_map(const uivec2 & resid);

	const uivec2 & input_map();

	virtual void update();

	virtual int32 update_priority();

	void clear_contexts();

	void pop_context();

	void push_context(const nsstring & pName);

	void set_cursor_pos(const fvec2 & cursorPos);

	bool caps_locked();

	bool num_locked();
	
private:

	void _key_press(nsinput_map::key_val pKey);

	void _key_release(nsinput_map::key_val pKey);

	void _mouse_move(const fvec2 & cursorPos);

	void _mouse_press(nsinput_map::mouse_button_val pButton, const fvec2 & mousePos);

	void _mouse_release(nsinput_map::mouse_button_val pButton, const fvec2 & mousePos);

	void _mouse_scroll(float pDelta, const fvec2 & mousePos);

	bool _check_trigger_modifiers(const nsinput_map::trigger & t);

	void _create_action_event(nsinput_map::trigger & trigger);

	void _create_action_state_event(nsinput_map::trigger & trigger, bool toggle);

	void _set_event_from_trigger(nsaction_event * evnt, const nsinput_map::trigger & t);
	
	context_stack m_ctxt_stack;
	nsinput_map::key_modifier_set m_key_modifiers;
	nsinput_map::mouse_modifier_set m_mouse_modifiers;

	fvec2 m_current_pos;
	fvec2 m_last_pos;
	float m_scroll_delta;
	uivec2 m_input_map_id;
	bool m_caps_locked;
	bool m_num_locked;
};


#endif
