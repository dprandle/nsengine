/*!
\file nsinputsystem.cpp

\brief Definition file for nsinput_system class

This file contains all of the neccessary definitions for the nsinput_system class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#include <iostream>

#include <nsinput_system.h>
#include <nsevent.h>
#include <nsevent_dispatcher.h>
#include <nsinput_map.h>
#include <nsscene.h>
#include <nsplugin.h>

nsinput_system::nsinput_system() :
	nssystem(),
	m_scroll_delta(0.0f)
{

}

nsinput_system::~nsinput_system()
{
 
}

const fvec2 & nsinput_system::cursor_pos()
{
	return m_current_pos;
}

bool nsinput_system::key_event(nskey_event * evnt)
{
	if (evnt->pressed)
		_key_press(evnt->key);
	else
		_key_release(evnt->key);
	return true;
}

bool nsinput_system::mouse_button_event(nsmouse_button_event * evnt)
{
	if (evnt->pressed)
		_mouse_press(evnt->mb, evnt->normalized_mpos);
	else
		_mouse_release(evnt->mb, evnt->normalized_mpos);
	return true;
}

bool nsinput_system::mouse_move_event(nsmouse_move_event * evnt)
{
	_mouse_move(evnt->normalized_mpos);
	return true;
}

bool nsinput_system::mouse_scroll_event(nsmouse_scroll_event * evnt)
{
	_mouse_scroll(evnt->scroll_delta, evnt->normalized_mpos);
	return true;
}

void nsinput_system::_key_press(nsinput_map::key_val pKey)
{
	// Add the key to the modifier set (if not already there)..
	nsinput_map * inmap = nse.resource<nsinput_map>(m_input_map_id);
	if (inmap == NULL)
		return;

	// Search context at top of stack for trigger with key -
	// create an event for every key that has matching modifiers
	context_stack::reverse_iterator rIter = m_ctxt_stack.rbegin();
	bool foundInContext = false;
	while (rIter != m_ctxt_stack.rend())
	{
		auto keyIter = (*rIter)->key_map.equal_range(pKey);
		while (keyIter.first != keyIter.second)
		{
			// Send input event to event system if modifiers match the trigger modifiers
			if (_check_trigger_modifiers(keyIter.first->second))
			{
				if (keyIter.first->second.trigger_state == nsinput_map::t_toggle)
					_create_state_event(keyIter.first->second, true);
				else if (keyIter.first->second.trigger_state != nsinput_map::t_released)
					_create_action_event(keyIter.first->second);
				
				foundInContext = keyIter.first->second.overwrite_lower_contexts || foundInContext;
			}
			++keyIter.first;
		}
		if (foundInContext) // If found a match for this key/modifier then search no further
			rIter = m_ctxt_stack.rend();
		else
			++rIter;
	}

	if (inmap->allowed_mod(pKey))
		m_key_modifiers.insert(pKey);
}

void nsinput_system::_create_action_event(nsinput_map::trigger & trigger)
{
	// If the trigger is set to toggle then create a state event - otherwise create an action event
	nsaction_event * evnt = nse.event_dispatch()->push<nsaction_event>(trigger.hash_name);
	_set_event_from_trigger(evnt, trigger);
}

void nsinput_system::_create_state_event(nsinput_map::trigger & trigger, bool toggle)
{
	// If the trigger is set to toggle then create a state event - otherwise create an action event
	nsstate_event * evnt = nse.event_dispatch()->push<nsstate_event>(trigger.hash_name, toggle);
	_set_event_from_trigger(evnt, trigger);
}

void nsinput_system::_key_release(nsinput_map::key_val pKey)
{
	nsinput_map * inmap = nse.resource<nsinput_map>(m_input_map_id);
	if (inmap == NULL)
		return;

	// Remove the key from the modifier set..
	m_key_modifiers.erase(pKey);

	// Check each context for pKey - create an event for each trigger. Since multiple triggers can
	// be assigned to each key sequence, I have to finish going through found triggers and set the iterator
	// to the rend
	context_stack::reverse_iterator rIter = m_ctxt_stack.rbegin();
	bool foundInContext = false;
	while (rIter != m_ctxt_stack.rend())
	{
		auto keyIter = (*rIter)->key_map.equal_range(pKey);
		while (keyIter.first != keyIter.second)
		{
			if (_check_trigger_modifiers(keyIter.first->second))
			{
				if (keyIter.first->second.trigger_state == nsinput_map::t_toggle)
					_create_state_event(keyIter.first->second, false);
				else if (keyIter.first->second.trigger_state != nsinput_map::t_pressed)
					_create_action_event(keyIter.first->second);
				
				foundInContext = keyIter.first->second.overwrite_lower_contexts || foundInContext;
			}
			++keyIter.first;
		}
		if (foundInContext)
			return;
		++rIter;
	}
}

void nsinput_system::set_cursor_pos(const fvec2 & cursorPos)
{
	m_last_pos = m_current_pos;
	m_current_pos = cursorPos;
}

void nsinput_system::_set_event_from_trigger(nsaction_event * evnt, const nsinput_map::trigger & t)
{
	evnt->norm_mpos = m_current_pos;
	evnt->norm_delta = m_current_pos - m_last_pos;
	evnt->scroll = m_scroll_delta;
}

void nsinput_system::_mouse_move(const fvec2 & cursorPos)
{
	set_cursor_pos(cursorPos);

	nsinput_map * inmap = nse.resource<nsinput_map>(m_input_map_id);
	if (inmap == NULL)
		return;

	context_stack::reverse_iterator rIter = m_ctxt_stack.rbegin();
	bool foundInContext = false;
	while (rIter != m_ctxt_stack.rend())
	{
		auto mouseIter = (*rIter)->mousebutton_map.equal_range(nsinput_map::movement);
		while (mouseIter.first != mouseIter.second)
		{
			// Send input event to event system if modifiers match the trigger modifiers
			if (_check_trigger_modifiers(mouseIter.first->second))
			{
				_create_action_event(mouseIter.first->second);
				foundInContext = mouseIter.first->second.overwrite_lower_contexts || foundInContext;
			}
			++mouseIter.first;
		}
		if (foundInContext)
			return;
		++rIter;
	}
}

void nsinput_system::_mouse_press(nsinput_map::mouse_button_val pButton, const fvec2 & mousePos)
{
	set_cursor_pos(mousePos);

	nsinput_map * inmap = nse.resource<nsinput_map>(m_input_map_id);
	if (inmap == NULL)
		return;

	// Go check each context for the key starting from the last context on the stack
	// if it is found there - then return
	context_stack::reverse_iterator rIter = m_ctxt_stack.rbegin();
	bool foundInContext = false;
	while (rIter != m_ctxt_stack.rend())
	{
		auto mouseIter = (*rIter)->mousebutton_map.equal_range(pButton);
		while (mouseIter.first != mouseIter.second)
		{
			// Send input event to event system if modifiers match the trigger modifiers
			if (_check_trigger_modifiers(mouseIter.first->second))
			{
				if (mouseIter.first->second.trigger_state == nsinput_map::t_toggle)
					_create_state_event(mouseIter.first->second, true);
				else if (mouseIter.first->second.trigger_state != nsinput_map::t_released)
					_create_action_event(mouseIter.first->second);

				foundInContext = mouseIter.first->second.overwrite_lower_contexts || foundInContext;
			}
			++mouseIter.first;
		}
		if (foundInContext)
			rIter = m_ctxt_stack.rend();
		else
			++rIter;
	}

	m_mouse_modifiers.insert(pButton);
}

void nsinput_system::_mouse_release(nsinput_map::mouse_button_val pButton, const fvec2 & mousePos)
{
	set_cursor_pos(mousePos);
	
	nsinput_map * inmap = nse.resource<nsinput_map>(m_input_map_id);
	if (inmap == NULL)
		return;

	m_mouse_modifiers.erase(pButton);
	// Go check each context for the key starting from the last context on the stack
	// if it is found there - then return
	context_stack::reverse_iterator rIter = m_ctxt_stack.rbegin();
	bool foundInContext = false;
	while (rIter != m_ctxt_stack.rend())
	{
		auto mouseIter = (*rIter)->mousebutton_map.equal_range(pButton);
		while (mouseIter.first != mouseIter.second)
		{
			if (_check_trigger_modifiers(mouseIter.first->second))
			{
				if (mouseIter.first->second.trigger_state == nsinput_map::t_toggle)
					_create_state_event(mouseIter.first->second, false);
				else if (mouseIter.first->second.trigger_state != nsinput_map::t_pressed)
					_create_action_event(mouseIter.first->second);

				foundInContext = mouseIter.first->second.overwrite_lower_contexts || foundInContext;
			}
			++mouseIter.first;
		}
		if (foundInContext)
			return;
		++rIter;
	}
}

void nsinput_system::_mouse_scroll(float pDelta, const fvec2 & mousePos)
{
	set_cursor_pos(mousePos);
	m_scroll_delta = pDelta;

	nsinput_map * inmap = nse.resource<nsinput_map>(m_input_map_id);
	if (inmap == NULL)
		return;

	context_stack::reverse_iterator rIter = m_ctxt_stack.rbegin();
	bool foundInContext = false;
	while (rIter != m_ctxt_stack.rend())
	{
		auto mouseIter = (*rIter)->mousebutton_map.equal_range(nsinput_map::scrolling);
		while (mouseIter.first != mouseIter.second)
		{
			// Send input event to event system if modifiers match the trigger modifiers
			if (_check_trigger_modifiers(mouseIter.first->second))
			{
				_create_action_event(mouseIter.first->second);
				foundInContext = mouseIter.first->second.overwrite_lower_contexts || foundInContext;
			}
			++mouseIter.first;
		}
		if (foundInContext)
			return;
		++rIter;
	}
}

void nsinput_system::pop_context()
{
	if (m_ctxt_stack.empty())
		return;
	m_ctxt_stack.pop_back();
}

void nsinput_system::push_context(const nsstring & pName)
{
	nsinput_map * inmap = nse.resource<nsinput_map>(m_input_map_id);
	nsinput_map::ctxt * ctxt = inmap->context(pName);

	if (ctxt == NULL)
		return;

	// Make sure the context isnt already on the stack
	for (uint32 i = 0; i < m_ctxt_stack.size(); ++i)
	{
		if (m_ctxt_stack[i] == ctxt)
			return;
	}
	
	m_ctxt_stack.push_back(ctxt);
}


void nsinput_system::init()
{
    register_handler_func(this, &nsinput_system::key_event);
    register_handler_func(this, &nsinput_system::mouse_button_event);
    register_handler_func(this, &nsinput_system::mouse_scroll_event);
    register_handler_func(this, &nsinput_system::mouse_move_event);
	register_handler_func(this, &nsinput_system::_handle_action);
}

bool nsinput_system::_handle_action(nsaction_event * evnt)
{
	if (evnt->trigger_hash_name == hash_id("save_map"))
		nse.active()->save<nsscene>("mainscene");
	return true;
}

int32 nsinput_system::update_priority()
{
	return INP_SYS_UPDATE_PR;
}

void nsinput_system::set_input_map(const uivec2 & resid)
{
	m_input_map_id = resid;
	clear_contexts();
}

const uivec2 & nsinput_system::input_map()
{
	return m_input_map_id;
}

void nsinput_system::update()
{
	nsscene * scene = nse.current_scene();
	if (scene == NULL)
		return;
}

bool nsinput_system::_check_trigger_modifiers(const nsinput_map::trigger & t)
{
	nsinput_map * inmap = nse.resource<nsinput_map>(m_input_map_id);
	
	// If Key_Any is not part of key modifiers than there must be a key for key match
	if ( (t.key_modifiers.find(nsinput_map::key_any) == t.key_modifiers.end()) &&
		 t.key_modifiers.size() != m_key_modifiers.size())
		return false;

	// Same thing with mouse modifiers
	if ( (t.mouse_modifiers.find(nsinput_map::any_button) == t.mouse_modifiers.end()) &&
		 t.mouse_modifiers.size() != m_mouse_modifiers.size())
		return false;


	// Check key modifiers
	auto keyIter = t.key_modifiers.begin();
	while (keyIter != t.key_modifiers.end())
	{
		// If modifier isnt allowed or if it is not in current mod stack then return false
		if ((*keyIter != nsinput_map::key_any) &&
			(!inmap->allowed_mod(*keyIter) || m_key_modifiers.find(*keyIter) == m_key_modifiers.end()))
			return false;			
		++keyIter;
	}

	// Check mouse modifiers
	auto mouseIter = t.mouse_modifiers.begin();
	while (mouseIter != t.mouse_modifiers.end())
	{
		// If modifier isnt allowed or if it is not in current mod stack then return false
		if (*mouseIter != nsinput_map::any_button &&
			m_mouse_modifiers.find(*mouseIter) == m_mouse_modifiers.end())
			return false;
		++mouseIter;
	}

	return true;
}

void nsinput_system::clear_contexts()
{
	m_ctxt_stack.clear();
}
