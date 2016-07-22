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
#include <ft2build.h>
#include FT_FREETYPE_H

nsfont::nsfont():
	nsasset(type_to_hash(nsfont)),
	m_faces(),
	m_font_file_data(),
	m_cur_pt_size(DEFAULT_IMPORT_PT_SIZE),
	m_cur_dpi(DEFAULT_FONT_DPI)
{
	set_ext(DEFAULT_FONT_EXTENSION);
}

nsfont::nsfont(const nsfont & copy):
	nsasset(copy),
	m_faces(),
	m_font_file_data(copy.m_font_file_data),
	m_cur_pt_size(copy.m_cur_pt_size),
	m_cur_dpi(copy.m_cur_dpi)
{}
	
nsfont::~nsfont()
{}

void nsfont::set_point_size(float pt_size)
{
	m_cur_pt_size = pt_size;
	_update_faces();
}

float nsfont::point_size()
{
	return m_cur_pt_size;
}

void nsfont::set_dpi(uint16 dpi)
{
	m_cur_dpi = dpi;
	_update_faces();
}

uint16 nsfont::dpi()
{
	return m_cur_dpi;
}

void nsfont::_update_faces()
{
	for (uint8 i = 0; i < m_faces.size(); ++i)
	{
		uint8 error = FT_Set_Char_Size(
          m_faces[i].ft_face, /* handle to face object */
          0, /* char_width in 1/64th of points  */
          m_cur_pt_size*64, /* char_height in 1/64th of points */
          m_cur_dpi, /* horizontal device resolution    */
          m_cur_dpi);
		m_faces[i].line_height = m_faces[i].ft_face->size->metrics.height >> 6;
		m_faces[i].max_advance = m_faces[i].ft_face->size->metrics.max_advance >> 6;
		_build_face_texture(i);
	}
}


float nsfont::line_height(uint8 face_index)
{
	if (face_index > m_faces.size())
		return 0.0f;
	return m_faces[face_index].line_height;	
}

float nsfont::max_advance(uint8 face_index)
{
	if (face_index > m_faces.size())
		return 0.0f;
	return m_faces[face_index].max_advance;
}

char_info * nsfont::get_char_info(uint8 face_index, char c)
{
	if (face_index > m_faces.size())
		return nullptr;
	return &m_faces[face_index].ci[c];
}

nstex2d * nsfont::get_atlas(uint8 face_index)
{
	if (face_index > m_faces.size())
		return nullptr;
	return m_faces[face_index].atlas;
}

void nsfont::_build_face_texture(uint8 face_index)
{
	font_face & ff = m_faces[face_index];
	FT_GlyphSlot g = ff.ft_face->glyph;
	ivec2 pad(4,4);
	
	// running total size (for atlas)
	ivec2 size;
	
	for(int i = 0; i < 128; ++i)
	{
		if( FT_Load_Char(ff.ft_face, i, FT_LOAD_RENDER) )
		{
			std::stringstream ss;
			ss << "nsfont::_build_face_texture - Loading character " << char(i) << " failed";
			dprint(ss.str());
			continue;
		}

		ff.ci[i].adv.set(g->advance.x >> 6,g->advance.y >> 6);
		ff.ci[i].bm_size.set(g->bitmap.width, g->bitmap.rows);
		ff.ci[i].bm_lt.set(g->bitmap_left,g->bitmap_top);
        ff.ci[i].tc.x = size.w + pad.x/2; // width before adding this char's width
		ff.ci[i].bearing.set(g->metrics.horiBearingX >> 6,g->metrics.horiBearingY >> 6);

		// add to width and take the largest height
		size.w += g->bitmap.width + pad.x;
		size.h = ivec2(size.h, g->bitmap.rows).max();

        ff.ci[i].tc.y = pad.y/2;
        ff.ci[i].tc.z = size.w - pad.x/2; // width after adding this char's width
        ff.ci[i].tc.w = g->bitmap.rows + pad.y/2;
	}
	size.h += pad.y;
	ff.atlas->resize(size, true);
	uint8 * dat = ff.atlas->data();
	for (uint8 i = 0; i < 128; ++i)
	{
		if( FT_Load_Char(ff.ft_face, i, FT_LOAD_RENDER) )
			continue;
		
        int32 x_offset = int32(ff.ci[i].tc.x);
        int32 y_offset = int32(ff.ci[i].tc.y);
		
		ivec2 bm_size(ff.ci[i].bm_size);
        for (int32 row = 0; row < bm_size.h; ++row)
		{
			for (uint32 j = 0; j < bm_size.w; ++j)
                dat[(row+y_offset) * ff.atlas->size().w + j + x_offset] = g->bitmap.buffer[(bm_size.h- row - 1) * g->bitmap.width + j];
		}
        ff.ci[i].tc.x = (ff.ci[i].tc.x) / size.w;
        ff.ci[i].tc.y = (ff.ci[i].tc.y) / size.h;
		ff.ci[i].tc.z = (ff.ci[i].tc.z) / size.w;
		ff.ci[i].tc.w = (ff.ci[i].tc.w) / size.h;
	}
    ff.atlas->video_update();
}

void nsfont::init()
{}

uivec3_vector nsfont::resources()
{
	return uivec3_vector();
}

void nsfont::name_change(const uivec2 & oldid, const uivec2 newid)
{
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
	return *this;
}
