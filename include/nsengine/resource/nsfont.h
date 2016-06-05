/*! 
	\file nsfont.h
	
	\brief Header file for nsfont class

	This file contains all of the neccessary declarations for the nsfont class.

	\author Daniel Randle
	\date February 17 2016
	\copywrite Earth Banana Games 2013
*/

#ifndef NSFONT_H
#define NSFONT_H

#include <nsresource.h>

struct font_info
{
	font_info();

	nsstring face;
	uint8 pt;
   	bool bold;
	bool italic;
	nsstring charset;
	bool unicode;
	uint8 stretch;
	bool smooth;
	uint8 super_sampling;
	ivec4 padding; // up, right, down, left
	ivec2 spacing; // horizontal, vertical
	int16 line_height;
	int16 base;
	uint8 page_count;
	bool packed;
};


struct char_info
{
	char_info();

	bool valid();

	uivec4 rect;
	ivec2 offset;
	uint8 xadvance;
	uint8 page_index;
	uint8 channel;
};

class nsfont : public nsresource
{
  public:

	template<class PUPer>
	friend void pup(PUPer & p, nsfont & aset);

	nsfont();
	nsfont(const nsfont & copy);
	
	virtual ~nsfont();

	virtual void init();

	virtual uivec3_vector resources();

	virtual void name_change(const uivec2 & oldid, const uivec2 newid);

	virtual void pup(nsfile_pupper * p);

	nsfont & operator=(nsfont rhs);

	char_info & get_char_info(char c);

	uivec2 & texture_id(uint8 page_index);

	void push_page(const uivec2 & page);

	void pop_page();

	font_info & get_font_info();

	uivec2 material_id;
	
  private:

	font_info m_fi;
	std::vector<char_info> m_chars;
	uivec2_vector m_pages;
};

template<class PUPer>
void pup(PUPer & p, char_info & ci, const nsstring & val_name)
{
	pup(p, ci.rect, val_name + ".rect");
	pup(p, ci.offset, val_name + ".offset");
	pup(p, ci.xadvance, val_name + ".xadvance");
	pup(p, ci.page_index, val_name + ".page_index");
	pup(p, ci.channel, val_name + ".channel");
}

template<class PUPer>
void pup(PUPer & p, font_info & fonti, const nsstring & val_name)
{
	pup(p, fonti.face, val_name + ".face");
	pup(p, fonti.pt, val_name + ".pt");
	pup(p, fonti.bold, val_name + ".bold");
	pup(p, fonti.italic, val_name + ".italic");
	pup(p, fonti.charset, val_name + ".charset");
	pup(p, fonti.unicode, val_name + ".unicode");
	pup(p, fonti.stretch, val_name + ".stretch");
	pup(p, fonti.smooth, val_name + ".smooth");
	pup(p, fonti.super_sampling, val_name + ".super_sampling");
	pup(p, fonti.spacing, val_name + ".spacing");
	pup(p, fonti.line_height, val_name + ".line_height");
	pup(p, fonti.base, val_name + ".base");
	pup(p, fonti.page_count, val_name + ".page_count");
	pup(p, fonti.packed, val_name + ".packed");
}

template<class PUPer>
void pup(PUPer & p, nsfont & font)
{
	pup(p, font.m_fi, "fi");
	pup(p, font.m_chars, "chars");
	pup(p, font.m_pages, "pages");
}

#endif
