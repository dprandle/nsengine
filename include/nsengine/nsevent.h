/*!
\file nsevent.h

\brief Header file for NSEvent struct

This file contains all of the neccessary declarations for the NSEvent struct.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#include <nsmath/nsmath.h>
#include <asset/nsinput_map.h>

#ifndef NSEVENT_H
#define NSEVENT_H

#include <iostream>

#define push_asset_destroyed(asset_id) nse.event_dispatch()->push<asset_name_change_event>(asset_id, uivec2(0))
#define push_asset_name_change(old_id,new_id) nse.event_dispatch()->push<asset_name_change_event>(old_id,new_id)

struct nsevent
{

	nsevent(uint32 id_=0):
		ref_count(0),
		id(id_)
	{}
    virtual ~nsevent() {}

	uint32 ref_count;
	uint32 id;
};

struct nswindow_closed_event : public nsevent
{
	nswindow_closed_event(uint32 ctxt_id):
		nsevent(ctxt_id)
	{}
};

struct nswindow_position_event : public nsevent
{
	nswindow_position_event(uint32 ctxt_id, const ivec2 & new_pos):
		nsevent(ctxt_id),
		new_win_pos(new_pos)
	{}
	
	ivec2 new_win_pos;
};

struct nswindow_minimized_event : public nsevent
{
	nswindow_minimized_event(uint32 ctxt_id):
		nsevent(ctxt_id)
	{}
};

struct nswindow_fullscreened_event : public nsevent
{
	nswindow_fullscreened_event(uint32 ctxt_id, const ivec2 & resolution_):
		nsevent(ctxt_id),
		resolution(resolution_)
	{}
	
	ivec2 resolution;
};

struct nswindow_restored_event : public nsevent
{
	nswindow_restored_event(uint32 ctxt_id):
		nsevent(ctxt_id)
	{}
};

struct nswindow_maximized_event : public nsevent
{
	nswindow_maximized_event(uint32 ctxt_id):
		nsevent(ctxt_id)
	{}
};

struct nswindow_focused_event : public nsevent
{
	nswindow_focused_event(uint32 ctxt_id):
		nsevent(ctxt_id)
	{}
};

struct nswindow_unfocused_event : public nsevent
{
	nswindow_unfocused_event(uint32 ctxt_id):
		nsevent(ctxt_id)
	{}
};

struct nskey_event : public nsevent
{

	nskey_event(nsinput_map::key_val key_, bool pressed_) :
		nsevent(),
		pressed(pressed_),
		key(key_)
	{}

	bool pressed;
	nsinput_map::key_val key;
};

struct nsmouse_button_event : public nsevent
{

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

struct nsmouse_move_event : public nsevent
{

	nsmouse_move_event(const fvec2 & normalized_mpos_) :
		nsevent(),
		normalized_mpos(normalized_mpos_)
	{}

	fvec2 normalized_mpos;
};

struct nsmouse_scroll_event : public nsevent
{

	nsmouse_scroll_event(float scroll_delta_, const fvec2 & normalized_mpos_) :
		nsevent(),
		scroll_delta(scroll_delta_),
		normalized_mpos(normalized_mpos_)
	{}

	float scroll_delta;
	fvec2 normalized_mpos;
};

struct nsaction_event : public nsevent
{
	enum state_t
	{
		none=-1,
		end,
		begin
	};
	
	nsaction_event(uint32 trigger_hashid_, state_t current_state=none):
		nsevent(trigger_hashid_),
		cur_state(current_state)
	{}

	virtual ~nsaction_event() {}

	fvec2 norm_mpos;
	fvec2 norm_delta;
	float scroll;
	state_t cur_state;
	int key_code;
};

struct nssel_focus_event : public nsevent
{  	
	nssel_focus_event(const uivec2 & focid_):
		nsevent(),
		focus_id(focid_)
	{}

	uivec2 focus_id;
};

struct nscam_change_event : public nsevent
{  
	nscam_change_event()
	{}
};

struct window_resize_event : public nsevent
{
	window_resize_event(uint32 window_tag_, const ivec2 & new_size_):
		nsevent(),
		window_tag(window_tag_),
		new_size(new_size_)
	{}

	uint32 window_tag;
	ivec2 new_size;
};

struct scene_ent_added : public nsevent
{
	scene_ent_added(const uivec2 & ent_id_, const uivec2 & scene_id):
		nsevent(),
		ent_id(ent_id_),
		scn_id(scene_id)
	{}
	uivec2 ent_id;
	uivec2 scn_id;
};

struct scene_ent_removed : public nsevent
{
	scene_ent_removed(const uivec2 & ent_id_, const uivec2 & scene_id):
		nsevent(),
		ent_id(ent_id_),
		scn_id(scene_id)
	{}
	uivec2 ent_id;
	uivec2 scn_id;
};

struct collision_event : public nsevent
{
	collision_event(const uivec3 & ent_c1_, const uivec3 & ent_c2_):
		nsevent(),
		ent_c1(ent_c1_),
		ent_c2(ent_c2_)
	{}
	
	uivec3 ent_c1;
	uivec3 ent_c2;
};

struct audio_play_event : public nsevent
{
	audio_play_event(const uivec3 & source_id_, uint32 effect_index_):
		nsevent(),
		source_id(source_id_),
		effect_index(effect_index_)
	{}
	
	uivec3 source_id;
	uint32 effect_index;
};

struct audio_start_streaming_event : public nsevent
{
	audio_start_streaming_event(const uivec3 & source_id_, uint32 effect_index_):
		nsevent(),
		source_id(source_id_),
		effect_index(effect_index_)
	{}
	
	uivec3 source_id;
	uint32 effect_index;
};

struct audio_stop_streaming_event : public nsevent
{
	audio_stop_streaming_event(const uivec3 & source_id_, uint32 effect_index_):
		nsevent(),
		source_id(source_id_),
		effect_index(effect_index_)
	{}
	
	uivec3 source_id;
	uint32 effect_index;
};

// A name change to 0,0,0 signifies asset deletion
struct asset_name_change_event : public nsevent
{
	asset_name_change_event(const uivec2 & old, const uivec2 & new_):
		nsevent(),
		old_name(old),
		new_name(new_)
	{}
	
	uivec2 old_name;
	uivec2 new_name;
};

#endif
