#include <nswindow.h>

nswindow::nswindow(
	const ivec2 & window_size,
	const nsstring & win_title,
	const nsstring & win_icon_path,
	uint8 creation_hint,
	window_state state_,
	const ivec2 & position_,
	bool visible_,
	bool focused):
	m_size(window_size),
	m_position(position_),
	m_creation_hint(creation_hint),
	m_norm_cpos(),
	m_visible(visible_),
	m_state(state_),
	m_open(true),
	m_focused(focused),
	m_title(win_title),
	m_icon(nullptr),
	m_icon_path(win_icon_path),
	m_vid_modes(),
	m_vid_mode_index(0)
{}

nswindow::~nswindow()
{}

void nswindow::close()
{
	m_open = false;
}

bool nswindow::is_open()
{
	return m_open;
}

bool nswindow::visible()
{
	return m_visible;
}

const fvec2 & nswindow::cursor_pos()
{
	return m_norm_cpos;
}

window_state nswindow::get_state()
{
	return m_state;
}

const ivec2 & nswindow::size()
{
	return m_size;
}

const ivec2 & nswindow::position()
{
	return m_position;
}

bool nswindow::focused()
{
	return m_focused;
}

nstex2d * nswindow::icon()
{
	return m_icon;
}

const nsstring & nswindow::icon_path()
{
	return m_icon_path;
}

const nsstring & nswindow::title()
{
	return m_title;
}

bool nswindow::user_resizable()
{
	return ((m_creation_hint & win_hint_resizable) == win_hint_resizable);
}

bool nswindow::always_on_top()
{
	return ((m_creation_hint & win_hint_always_on_top) == win_hint_always_on_top);
}

bool nswindow::decorated()
{
	return ((m_creation_hint & win_hint_decorated) == win_hint_decorated);
}
