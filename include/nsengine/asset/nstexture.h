/*! 
  \file nstexture.h
	
  \brief Header file for nstexture class

  This file contains all of the neccessary declarations for the nstexture class.

  \author Daniel Randle
  \date November 23 2013
  \copywrite Earth Banana Games 2013
*/

// Internal Format : Bytes per pixel(ie 3 or 4)
// Format : enum specifying order of those byes - ie RGBA or RBGA etc
// Pixel Data Type : the data type assigned to each component of the pixe(ie unsigned byte)


#ifndef NSTEXTURE_H
#define NSTEXTURE_H

#define BASE_TEX_UNIT GL_TEXTURE0
#define BASE_CUBEMAP_FACE GL_TEXTURE_CUBE_MAP_POSITIVE_X

#include <nsasset.h>
#include <nsvector.h>
#include <nsvideo_driver.h>
#include <nssignal.h>

class nstexture_inst;

enum tex_format
{
	tex_red,
	tex_ired,
	tex_rg,
	tex_irg,
	tex_rgb,
	tex_bgr,
	tex_irgb,
	tex_ibgr,
	tex_rgba,
	tex_bgra,
	tex_irgba,
	tex_ibgra,
	tex_depth,
	tex_depth_stencil
};

enum pixel_component_type
{
	tex_s8,
	tex_u8,
	tex_s16,
	tex_u16,
	tex_s32,
	tex_u32,
	tex_float
};

enum tex_min_filter
{
	tmin_nearest,
	tmin_linear,
	tmin_nearest_mipmap_nearest,
	tmin_nearest_mipmap_linear,
	tmin_linear_mipmap_nearest,
	tmin_linear_mipmap_linear
};

enum tex_mag_filter
{
	tmag_nearest,
	tmag_linear
};

enum tex_edge_behavior
{
	te_repeat,
	te_repeat_mirror,
	te_clamp,
	te_clamp_mirror
};

enum tex_depth_mode
{
	tex_dm_none,
	tex_dm_compare
};

enum tex_depth_compare_func
{
	tex_dc_lequal,
	tex_dc_gequal,
	tex_dc_less,
	tex_dc_greater,
	tex_dc_equal,
	tex_dc_not_equal,
	tex_dc_always,
	tex_dc_never
};

struct tex_params
{
	tex_params():
		min_filter(tmin_linear_mipmap_linear),
		mag_filter(tmag_linear),
		edge_behavior(te_repeat,te_repeat,te_repeat),
		depth_mode(tex_dm_none),
		depth_func(tex_dc_lequal),
		anistropic_filtering(0.0f)
	{}
	
	tex_min_filter min_filter;
	tex_mag_filter mag_filter;
	tex_depth_mode depth_mode;
	tex_depth_compare_func depth_func;
	nsvec3<tex_edge_behavior> edge_behavior;
	float anistropic_filtering;
};

class nstexture : public nsasset, public nsvideo_object
{
  public:
	template <class PUPer>
	friend void pup(PUPer & p, nstexture & sm);

	nstexture(uint32 hashed_type);

	nstexture(const nstexture & copy_);

	~nstexture();

	nstexture & operator=(nstexture_inst rhs);
	
	void video_context_init();

	uint8 bytes_per_pixel() const;

	uint8 bytes_per_channel() const;

	virtual int32 pixel_count() const = 0;

	uint8 channels() const;

	void enable_mipmap_autogen(bool enable);

	bool mipmap_autogen();

	void enable_compress_on_upload(bool enable);

	bool compress_on_upload();

	uint32 compressed_size();

	void set_data(uint8 * image_data);

	virtual void copy_data(uint8 * data, uint32 pixel_offset);
	
	virtual void copy_data(uint8 * data, uint32 data_size, uint32 pixel_offset, bool repeat);

	uint8 * data();

	void set_format(tex_format tf);

	void set_parameters(tex_params texp);

	tex_params parameters();

	void set_component_data_type(pixel_component_type pt);

	tex_format format();

	pixel_component_type component_data_type();

	void unbind() const;

	virtual void init();

	virtual void release();
	
  protected:
	bool compress;
	bool m_auto_gen_mipmaps;
	uint8 * m_raw_data;
	tex_format m_format;
	pixel_component_type m_data_type;
	uint32 m_compressed_size;
	tex_params m_params;
};

class nstexture_inst : public nstexture
{
	nstexture_inst(const nstexture & rhs):nstexture(rhs) {}
	void init() {std::terminate();}
 	void pup(nsfile_pupper *) {std::terminate();}
	int32 pixel_count() const {std::terminate();}
};

template<class PUPer>
void pup(PUPer & p, pixel_component_type & en, const nsstring & var_name_)
{
	uint32 in = static_cast<uint32>(en);
	pup(p, in, var_name_);
	en = static_cast<pixel_component_type>(in);
}

template<class PUPer>
void pup(PUPer & p, tex_format & en, const nsstring & var_name_)
{
	uint32 in = static_cast<uint32>(en);
	pup(p, in, var_name_);
	en = static_cast<tex_format>(in);
}

template<class PUPer>
void pup(PUPer & p, tex_mag_filter & en, const nsstring & var_name_)
{
	uint32 in = static_cast<uint32>(en);
	pup(p, in, var_name_);
	en = static_cast<tex_mag_filter>(in);
}

template<class PUPer>
void pup(PUPer & p, tex_min_filter & en, const nsstring & var_name_)
{
	uint32 in = static_cast<uint32>(en);
	pup(p, in, var_name_);
	en = static_cast<tex_min_filter>(in);
}

template<class PUPer>
void pup(PUPer & p, tex_edge_behavior & en, const nsstring & var_name_)
{
	uint32 in = static_cast<uint32>(en);
	pup(p, in, var_name_);
	en = static_cast<tex_edge_behavior>(in);
}

template<class PUPer>
void pup(PUPer & p, tex_params & tp, const nsstring & var_name_)
{
	pup(p, tp.min_filter, var_name_ + ".min_filter");
	pup(p, tp.mag_filter, var_name_ + ".mag_filter");
	pup(p, tp.edge_behavior, var_name_ + ".edge_behavior");
	pup(p, tp.anistropic_filtering, var_name_ + ".anistropic_filtering");
}

template <class PUPer>
void pup(PUPer & p, nstexture & sm)
{
	pup(p, sm.compress,"compress");
	pup(p, sm.m_auto_gen_mipmaps,"auto_gen_mipmaps");
	pup(p, sm.m_format,"format");
	pup(p, sm.m_data_type, "data_type");
	pup(p, sm.m_compressed_size, "compressed_size");
	pup(p, sm.m_params, "parameters");
	uint32 size = sm.pixel_count();
	if (sm.m_compressed_size > 0)
		size = sm.m_compressed_size;
	for (uint32 i = 0; i < size; ++i)
		pup(p, sm.m_raw_data[i], "data[" + std::to_string(i) + "]");
}

class nstex1d : public nstexture
{
  public:
	
	template <class PUPer>
	friend void pup(PUPer & p, nstex1d & sm);

	nstex1d();

	nstex1d(const nstex1d & copy_);

	~nstex1d();

	nstex1d & operator=(nstex1d rhs);

	int32 pixel_count() const;

	void pup(nsfile_pupper * p);

	void resize(int32 w, bool resize_data=true);

	int32 size() const;

	void set_offset(int32 offset);

	int32 offset() const;

  private:
	int32 m_width;
	int32 m_offset;
};

template <class PUPer>
void pup(PUPer & p, nstex1d & sm)
{
	pup(p, sm.m_width,"width");
	pup(p, *(static_cast<nstexture*>(&sm)));
}

class nstex2d : public nstexture
{
  public:
	template <class PUPer>
	friend void pup(PUPer & p, nstex2d & sm);

	nstex2d();
	
	nstex2d(const nstex2d & copy_);

	~nstex2d();

	nstex2d & operator=(nstex2d rhs);

	int32 pixel_count() const;

	void flip_verticle();
	
	virtual void pup(nsfile_pupper * p);

	void resize(const ivec2 & sz, bool resize_data=true);

	const ivec2 & size() const;

	void set_offset(const ivec2 & offset);

	const ivec2 & offset() const;

  private:
	ivec2 m_size;
	ivec2 m_offset;
};

template <class PUPer>
void pup(PUPer & p, nstex2d & sm)
{
	pup(p, sm.m_size, "size");
	pup(p,*(static_cast<nstexture*>(&sm)));
}

class nstex3d : public nstexture
{
  public:
	template <class PUPer>
	friend void pup(PUPer & p, nstex3d & sm);

	nstex3d();
	
	nstex3d(const nstex3d & copy_);

	~nstex3d();

	nstex3d & operator=(nstex3d rhs);

	int32 pixel_count() const;
	
	virtual void pup(nsfile_pupper * p);

	void resize(const ivec3 & size, bool resize_data=true);

	const ivec3 & size() const;

	void set_offset(const ivec3 & offset);

	const ivec3 & offset() const;

  private:
	ivec3 m_offset;
	ivec3 m_size;
};

template <class PUPer>
void pup(PUPer & p, nstex3d & sm)
{
	pup(p, sm.m_size, "size");
	pup(p, *(static_cast<nstexture*>(&sm)));
}

class nstex_cubemap : public nstexture
{
  public:
	template <class PUPer>
	friend void pup(PUPer & p, nstex_cubemap & sm);
	using nstexture::data;
	using nstexture::copy_data;
	
	nstex_cubemap();

	nstex_cubemap(const nstex_cubemap & copy_);

	~nstex_cubemap();

	nstex_cubemap & operator=(nstex_cubemap rhs);

	int32 pixel_count() const;
	
	void pup(nsfile_pupper * p);

	const ivec2 & size() const;
	
	void resize(const ivec2 & size, bool resize_data=true);

	void copy_data(uint8 * data, uint32 cube_face);

	void copy_data(uint8 * data, uint32 data_size, uint32 cube_face, bool repeat);

	uint8 * data(uint8 cube_face);
	
	void set_offset(const ivec2 & offset);

	const ivec2 & offset() const;

  private:
	ivec2 m_size;
	ivec2 m_offset;
};

template <class PUPer>
void pup(PUPer & p, nstex_cubemap & sm)
{
	pup(p, sm.m_size, "size");
	pup(p, *(static_cast<nstexture*>(&sm)));
}

#endif
