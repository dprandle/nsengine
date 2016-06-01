/*!
\file nsui_comp.cpp

\brief Definition file for nsui_comp class

This file contains all of the neccessary definitions for the nsui_comp class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#include <nsui_comp.h>
#include <nsentity.h>

nsui_material_comp::nsui_material_comp() :
	nscomponent(),
	content_shader_id(0),
	border_shader_id(0),
	mat_id(0),
	border(0.0f),
	border_color(0.0f),
	focused(false)
{}

nsui_material_comp::nsui_material_comp(const nsui_material_comp & copy):
	nscomponent(copy),
	content_shader_id(copy.content_shader_id),
	border_shader_id(copy.border_shader_id),
	mat_id(copy.mat_id),
	border(copy.border),
	border_color(copy.border_color),
	focused(false)
{}

nsui_material_comp::~nsui_material_comp()
{}

nsui_material_comp* nsui_material_comp::copy(const nscomponent * to_copy)
{
	const nsui_material_comp * comp = dynamic_cast<const nsui_material_comp*>(to_copy);
	if (comp != nullptr)
		(*this) = *comp;
	return this;
}

void nsui_material_comp::init()
{}

void nsui_material_comp::pup(nsfile_pupper * p)
{
	if (p->type() == nsfile_pupper::pup_binary)
	{
		nsbinary_file_pupper * bf = static_cast<nsbinary_file_pupper *>(p);
		::pup(*bf, *this);
	}
	else
	{
		nstext_file_pupper * tf = static_cast<nstext_file_pupper *>(p);
		::pup(*tf, *this);
	}
}

nsui_material_comp & nsui_material_comp::operator=(nsui_material_comp rhs_)
{
	nscomponent::operator=(rhs_);
	std::swap(content_shader_id,rhs_.content_shader_id);
	std::swap(border_shader_id,rhs_.border_shader_id);
	std::swap(mat_id,rhs_.mat_id);
	std::swap(border,rhs_.border);
	std::swap(border_color,rhs_.border_color);
	std::swap(focused,rhs_.focused);
	post_update(true);
	return (*this);
}



nsui_text_comp::nsui_text_comp() :
	nscomponent(),
	text_shader_id(),
	font_id(),
	margins(0),
	text(),
	text_alignment(top_left),
	text_line_sizes()
{
	text_line_sizes.reserve(128);
}

nsui_text_comp::nsui_text_comp(const nsui_text_comp & copy):
	nscomponent(copy),
	text_shader_id(copy.text_shader_id),
	font_id(copy.font_id),
	margins(copy.margins),
	text(copy.text),
	text_alignment(copy.text_alignment),
	text_line_sizes(copy.text_line_sizes)
{}

nsui_text_comp::~nsui_text_comp()
{}

nsui_text_comp* nsui_text_comp::copy(const nscomponent * to_copy)
{
	const nsui_text_comp * comp = dynamic_cast<const nsui_text_comp*>(to_copy);
	if (comp != nullptr)
		(*this) = *comp;
	return this;
}

void nsui_text_comp::init()
{}

void nsui_text_comp::pup(nsfile_pupper * p)
{
	if (p->type() == nsfile_pupper::pup_binary)
	{
		nsbinary_file_pupper * bf = static_cast<nsbinary_file_pupper *>(p);
		::pup(*bf, *this);
	}
	else
	{
		nstext_file_pupper * tf = static_cast<nstext_file_pupper *>(p);
		::pup(*tf, *this);
	}
}

nsui_text_comp & nsui_text_comp::operator=(nsui_text_comp rhs_)
{
	nscomponent::operator=(rhs_);
	std::swap(text_shader_id,rhs_.text_shader_id);
	std::swap(font_id,rhs_.font_id);
	std::swap(margins,rhs_.margins);
	std::swap(text,rhs_.text);
	std::swap(text_alignment,rhs_.text_alignment);
	std::swap(text_line_sizes,rhs_.text_line_sizes);
	post_update(true);
	return (*this);
}

nsui_text_input_comp::nsui_text_input_comp() :
	nscomponent(),
	cursor_blink_rate_ms(450.0f),
	cursor_pixel_width(3),
	cursor_color(fvec4(1.0f)),
	cursor_offset(0)
{}

nsui_text_input_comp::nsui_text_input_comp(const nsui_text_input_comp & copy):
	nscomponent(copy),
	cursor_blink_rate_ms(copy.cursor_blink_rate_ms),
	cursor_pixel_width(copy.cursor_pixel_width),
	cursor_color(copy.cursor_color),
	cursor_offset(copy.cursor_offset)
{}

nsui_text_input_comp::~nsui_text_input_comp()
{}

nsui_text_input_comp* nsui_text_input_comp::copy(const nscomponent * to_copy)
{
	const nsui_text_input_comp * comp = dynamic_cast<const nsui_text_input_comp*>(to_copy);
	if (comp != nullptr)
		(*this) = *comp;
	return this;
}

void nsui_text_input_comp::init()
{}

void nsui_text_input_comp::pup(nsfile_pupper * p)
{
	if (p->type() == nsfile_pupper::pup_binary)
	{
		nsbinary_file_pupper * bf = static_cast<nsbinary_file_pupper *>(p);
		::pup(*bf, *this);
	}
	else
	{
		nstext_file_pupper * tf = static_cast<nstext_file_pupper *>(p);
		::pup(*tf, *this);
	}
}

nsui_text_input_comp & nsui_text_input_comp::operator=(nsui_text_input_comp rhs_)
{
	nscomponent::operator=(rhs_);
	std::swap(cursor_blink_rate_ms,rhs_.cursor_blink_rate_ms);
	std::swap(cursor_pixel_width,rhs_.cursor_pixel_width);
	std::swap(cursor_color,rhs_.cursor_color);
	std::swap(cursor_offset,rhs_.cursor_offset);
	post_update(true);
	return (*this);
}
