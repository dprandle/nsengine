/*! 
	\file nsfont.cpp
	
	\brief Header file for nsfont class

	This file contains all of the neccessary definitions for the nsfont class.

	\author Daniel Randle
	\date Feb 17 2016
	\copywrite Earth Banana Games 2013
*/

#include <nsfont.h>
#include <nsengine.h>
#include <nstexture.h>

font_info::font_info():
	face(),
	pt(0),
	bold(false),
	italic(false),
	charset(),
	unicode(false),
	stretch(0),
	smooth(false),
	super_sampling(0),
	padding(0),
	spacing(0),
	line_height(0),
	base(0),
	page_count(0),
	packed(false)
{
}

char_info::char_info():
	rect(-1),
	offset(0),
	xadvance(-1),
	page_index(-1),
	channel(-1)
{}

bool char_info::valid()
{
	return !(
		rect == uivec4(-1) &&
		offset == ivec2(0) &&
		xadvance == uint8(-1) &&
		page_index == uint8(-1) &&
		channel == uint8(-1)
		);
}

nsfont::nsfont():
	nsasset(type_to_hash(nsfont)),
	material_id(0),
	m_fi()
{
	m_pages.reserve(256); // max page index
	m_chars.resize(256); // should initialize to "invalid" char_info
	set_ext(DEFAULT_FONT_EXTENSION);
}

nsfont::nsfont(const nsfont & copy):
	nsasset(copy),
	material_id(copy.material_id),
	m_fi(copy.m_fi),
	m_chars(copy.m_chars),
	m_pages(copy.m_pages)
{}
	
nsfont::~nsfont()
{}

void nsfont::init()
{}

uivec3_vector nsfont::resources()
{
	uivec3_vector ret;
	// add all texture maps that are available
	auto iter = m_pages.begin();
	while (iter != m_pages.end())
	{
		nstexture * _tex_ = get_resource<nstexture>(*iter);
		if (_tex_ != NULL)
		{
			uivec3_vector tmp = _tex_->resources();
			ret.insert(ret.end(), tmp.begin(), tmp.end());
			ret.push_back(uivec3(_tex_->full_id(), type_to_hash(nstexture)));
		}
		++iter;
	}
	return ret;
}

void nsfont::name_change(const uivec2 & oldid, const uivec2 newid)
{
	auto iter = m_pages.begin();
	while (iter != m_pages.end())
	{
		if (iter->x == oldid.x)
		{
			iter->x = newid.x;
			if (iter->y == oldid.y)
				iter->y = newid.y;
		}
		++iter;
	}	
}

char_info & nsfont::get_char_info(char c)
{
	return m_chars[uint8(c)];
}

font_info & nsfont::get_font_info()
{
	return m_fi;
}

uivec2 & nsfont::texture_id(uint8 page_index)
{
	return m_pages[page_index];
}

void nsfont::push_page(const uivec2 & page)
{
	m_pages.push_back(page);
}

void nsfont::pop_page()
{
	m_pages.pop_back();
}

void nsfont::pup(nsfile_pupper * p)
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

nsfont & nsfont::operator=(nsfont rhs)
{
	nsasset::operator=(rhs);
	std::swap(material_id, rhs.material_id);
	std::swap(m_fi,rhs.m_fi);
	std::swap(m_chars,rhs.m_chars);
	std::swap(m_pages,rhs.m_pages);
	return *this;
}
