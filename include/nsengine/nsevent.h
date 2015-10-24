/*!
\file nsevent.h

\brief Header file for NSEvent class

This file contains all of the neccessary declarations for the NSEvent class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#include <nsmath.h>
#include <nsinput_map.h>

#ifndef NSEVENT_H
#define NSEVENT_H

#include <iostream>

class nsevent
{
public:
	nsevent():
		ref_count(0)
	{}
    virtual ~nsevent() {}

	uint32 ref_count;
};

class nskey_event : public nsevent
{
public:
	nskey_event(nsinput_map::key_val key_, bool pressed_) :
		nsevent(),
		pressed(pressed_),
		key(key_)
	{}

	bool pressed;
	nsinput_map::key_val key;
};

class nsmouse_button_event : public nsevent
{
public:
	nsmouse_button_event(nsinput_map::mouse_button_val mb_, bool pressed_, const fvec2 & normalized_mpos_) :
		nsevent(),
		mb(mb_),
		pressed(pressed_),
		normalized_mpos(normalized_mpos_)
	{}

	nsinput_map::mouse_button_val mb;
	bool pressed;
	fvec2 normalized_mpos;
};

class nsmouse_move_event : public nsevent
{
public:
	nsmouse_move_event(const fvec2 & normalized_mpos_) :
		nsevent(),
		normalized_mpos(normalized_mpos_)
	{}

	fvec2 normalized_mpos;
};

class nsmouse_scroll_event : public nsevent
{
public:
	nsmouse_scroll_event(float scroll_delta_, const fvec2 & normalized_mpos_) :
		nsevent(),
		scroll_delta(scroll_delta_),
		normalized_mpos(normalized_mpos_)
	{}

	float scroll_delta;
	fvec2 normalized_mpos;
};

class nsaction_event : public nsevent
{
public:
	nsaction_event(uint32 trigger_hashid_):
		nsevent(),
		trigger_hash_name(trigger_hashid_)
	{}

	virtual ~nsaction_event() {}

	uint32 trigger_hash_name;
	nsinput_map::axis_map axes;
};

class nsstate_event : public nsaction_event
{
public:
	nsstate_event(uint32 trigger_hashid_, bool toggle_):
		nsaction_event(trigger_hashid_),
		toggle(toggle_)
	{}

	bool toggle;
};

class nssel_focus_event : public nsevent
{
  public:
	
	nssel_focus_event(const uivec3 & focid_):
		nsevent(),
		focus_id(focid_)
	{}

	uivec3 focus_id;
};

class nscam_change_event : public nsevent
{
  public:
	nscam_change_event()
	{}
};

#endif
