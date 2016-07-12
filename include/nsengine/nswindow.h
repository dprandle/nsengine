/*! 
	\file nswindow.h
	
	\brief Header file for NSDebug class

	This file contains all of the neccessary declarations for the nswindow class.

	\author Daniel Randle
	\date November 7 2013
	\copywrite Earth Banana Games 2013
*/

#ifndef NSWINDOW_H
#define NSWINDOW_H

#include <nsmath.h>

struct window_params
{
	bool fullscreen;
};

struct full_screen_video_mode
{
	ivec2 resolution;
	int32 refresh_rate;
	ivec3 rgb_bit_depth;
};

enum window_state
{
	window_minimized,
	window_maximized,
	window_full_screen,
	window_restored
};

class nstex2d;

enum window_creation_hint
{
	win_hint_resizable = 0x01,
	win_hint_decorated = 0x02,
	win_hint_always_on_top = 0x04
};

struct vid_ctxt;

void window_poll_input();

class nswindow
{
  public:
	
	nswindow(
		const ivec2 & window_size,
		const nsstring & win_title,
		const nsstring & win_icon_path,
		uint8 creation_hint,
		window_state state_,
		const ivec2 & position_,
		bool visible_,
		bool focused);
	
	virtual ~nswindow();

	virtual void close();

	virtual bool is_open();


	virtual void update() = 0;

	virtual void make_current() = 0;
	

	virtual void set_visible(bool visible) = 0;

	virtual void set_cursor_pos(const fvec2 & norm_cpos) = 0;

	virtual void set_state(window_state ws) = 0;

	virtual void resize(const ivec2 & sz) = 0;

	virtual void set_position(const ivec2 & pos) = 0;

	virtual void focus() = 0;

	virtual void set_icon(nstex2d * icon) = 0;

	virtual void set_title(const nsstring & title) = 0;

	virtual bool visible();

	virtual const fvec2 & cursor_pos();

	virtual window_state get_state();

	virtual const ivec2 & size();

	virtual const ivec2 & position();

	virtual bool focused();

	virtual nstex2d * icon();

	virtual const nsstring & icon_path();

	virtual const nsstring & title();

	virtual bool user_resizable();

	virtual bool always_on_top();

	virtual bool decorated();

  protected: // non-copyable or deleteable by anything except engine

	nswindow(const nswindow & copy);
	nswindow & operator=(const nswindow & copy);

	ivec2 m_size;
	ivec2 m_position;
	uint8 m_creation_hint;
	fvec2 m_norm_cpos;
	bool m_visible;
	window_state m_state;
	bool m_open;
	bool m_focused;
	nsstring m_title;
	nstex2d * m_icon;
	nsstring m_icon_path;
	std::vector<full_screen_video_mode> m_vid_modes;
	uint32 m_vid_mode_index;
};


#endif
