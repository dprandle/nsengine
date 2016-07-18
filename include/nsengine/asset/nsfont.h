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

#define DEFAULT_IMPORT_PT_SIZE 44
#define DEFAULT_FONT_DPI 100.13

#include <nsasset.h>

struct FT_FaceRec_;
class nstex2d;

struct char_info
{
	fvec2 adv; // advance
	fvec2 bm_size; // bitmap width and rows
	fvec2 bm_lt; // bitmap left and top
	fvec2 tc;
};

struct font_face
{
	font_face():
		ft_face(nullptr),
		atlas(nullptr)
	{}
	
	FT_FaceRec_ * ft_face;
	nstex2d * atlas;
	char_info ci[128];
};

struct char_bmp_info
{
	char_bmp_info(uint8 * dat=nullptr, uivec2 sz=0, int pitch_ = 0, uint8 bytes_per_pixel_=1):
		size(sz),
		data(dat),
		pitch(pitch_),
		bytes_per_pixel(bytes_per_pixel_)
	{}
	
	uivec2 size;
	int pitch;
	uint8 bytes_per_pixel;
	uint8 * data;
};

class nsfont : public nsasset
{
  public:

	friend class nsfont_manager;
	
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

	void set_point_size(float pt_size);

	float point_size();

	void set_dpi(uint16 dpi);

	uint16 dpi();

	char_info get_char_info(uint8 face_index, char c);

	nstex2d * get_atlas(uint8 face_index);
	
  private:

	void _update_faces();
	void _build_face_texture(uint8 face_index);

	std::vector<font_face> m_faces;
	std::vector<uint8> m_font_file_data;
	float m_cur_pt_size;
	uint16 m_cur_dpi;
};

template<class PUPer>
void pup(PUPer & p, nsfont & font)
{
	pup(p, font.m_font_file_data, "fi_data");
	pup(p, font.m_cur_pt_size, "cur_pt");
	pup(p, font.m_cur_dpi, "cur_dpi");
}

#endif
