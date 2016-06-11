/*--------------------------------------------- Noble Steed Engine-------------------------------------*
Authors : Daniel Randle, Alex Bourne
Date Created : Mar 7 2013

File:
	nstexture.cpp

Description:
	This file contains the definition for the nstexture class functions along with any other helper
	functions
*-----------------------------------------------------------------------------------------------------*/


#include <nslog_file.h>
#include <nstexture.h>
#include <nstex_manager.h>
#include <nsvideo_driver.h>

nstexture::nstexture() :
	nsresource(),
	compress(false),
	m_auto_gen_mipmaps(true),
	m_raw_data(nullptr),
	m_format(tex_rgba),
	m_data_type(tex_u8),
	m_compressed_size(0),
	m_vid_tex(nullptr)
{
	set_ext(DEFAULT_TEX_EXTENSION);
}

nstexture::nstexture(const nstexture & copy_):
	nsresource(copy_),
	compress(copy_.compress),
	m_auto_gen_mipmaps(copy_.m_auto_gen_mipmaps),
	m_raw_data(new uint8[copy_.pixel_count()]),
	m_format(copy_.m_format),
	m_data_type(copy_.m_data_type),
	m_compressed_size(copy_.m_compressed_size),
	m_vid_tex(nullptr)
{
	for (uint32 i = 0; i < copy_.pixel_count(); ++i)
		m_raw_data[i] = copy_.m_raw_data[i];
}

nstexture::~nstexture()
{
	delete [] m_raw_data;
}

nstexture & nstexture::operator=(nstexture_inst rhs)
{
	nsresource::operator=(rhs);
	std::swap(compress, rhs.compress);
	std::swap(m_auto_gen_mipmaps, rhs.m_auto_gen_mipmaps);
	std::swap(m_raw_data, rhs.m_raw_data);
	std::swap(m_format, rhs.m_format);
	std::swap(m_data_type, rhs.m_data_type);
	std::swap(m_compressed_size, rhs.m_compressed_size);
	return *this;
}

void nstexture::enable_compress_on_upload(bool enable)
{
	compress = enable;
}

void nstexture::init()
{}

void nstexture::release()
{}

bool nstexture::compress_on_upload()
{
	return compress;
}

void nstexture::video_context_init()
{
   	video_context_release();
	uint8 context_id = nse.video_driver()->current_context()->context_id;
	ctxt_objs[context_id] = nse.factory<nsvid_obj_factory>(MESH_VID_OBJ_GUID)->create();
}

void nstexture::enable_mipmap_autogen(bool enable)
{
	m_auto_gen_mipmaps = enable;
}

bool nstexture::mipmap_autogen()
{
	return m_auto_gen_mipmaps;
}

uint8 nstexture::channels() const
{
	if (m_format <= tex_ired || m_format == tex_depth)
		return 1;
	else if ((m_format <= tex_irg) || m_format == tex_depth_stencil)
		return 2;
	else if (m_format <= tex_ibgr)
		return 3;
	else if (m_format <= tex_ibgra)
		return 4;
	else
		return 0;
}

uint8 nstexture::bytes_per_channel() const
{
	if (m_data_type <= tex_u8)
		return 1;
	else if (m_data_type <= tex_u16)
		return 2;
	else if (m_data_type <= tex_float)
		return 4;
	else
		return 0;	
}

uint8 nstexture::bytes_per_pixel() const
{
	return channels() * bytes_per_channel();
}

void nstexture::copy_data(uint8 * to_copy, uint32 write_offset_in_bytes)
{
	for (uint32 i = write_offset_in_bytes; i < pixel_count()*bytes_per_pixel(); ++i)
		m_raw_data[i] = to_copy[i-write_offset_in_bytes];
	m_compressed_size = 0;	
}

void nstexture::copy_data(uint8 * to_copy, uint32 buffer_max_size_in_bytes, uint32 write_offset_in_bytes)
{
	buffer_max_size_in_bytes = write_offset_in_bytes + buffer_max_size_in_bytes;
	if (buffer_max_size_in_bytes > pixel_count()*bytes_per_pixel())
		buffer_max_size_in_bytes = pixel_count()*bytes_per_pixel();
	for (uint32 i = write_offset_in_bytes; i < buffer_max_size_in_bytes; ++i)
		m_raw_data[i] = to_copy[i-write_offset_in_bytes];
	m_compressed_size = 0;
}

void nstexture::set_parameters(tex_params texp)
{
	m_params = texp;
}

tex_params nstexture::parameters()
{
	return m_params;
}

uint8 * nstexture::data()
{
	return m_raw_data;
}

void nstexture::set_data(uint8 * image_data)
{
	if (m_raw_data != nullptr)
		delete [] m_raw_data;
	m_raw_data = image_data;
	m_compressed_size = 0;
}

void nstexture::set_format(tex_format tf)
{
	m_format = tf;
}

void nstexture::set_component_data_type(pixel_component_type pt)
{
	m_data_type = pt;
}

tex_format nstexture::format()
{
	return m_format;
}

pixel_component_type nstexture::component_data_type()
{
	return m_data_type;
}

nstex1d::nstex1d():
	nstexture(),
	m_width(0),
	m_offset(0)
{}

nstex1d::nstex1d(const nstex1d & copy_):
	nstexture(copy_),
	m_width(copy_.m_width),
	m_offset(copy_.m_offset)
{}


nstex1d::~nstex1d()
{}

nstex1d & nstex1d::operator=(nstex1d rhs)
{
	nstexture::operator=(rhs);
	std::swap(m_width, rhs.m_width);
	std::swap(m_offset, rhs.m_offset);
	return *this;
}

int32 nstex1d::pixel_count() const
{
	return m_width;
}

void nstex1d::set_offset(int32 offset)
{
	m_offset = offset;
}

int32 nstex1d::offset() const
{
	return m_offset;
}

int32 nstex1d::size() const
{
	return m_width;
}

void nstex1d::pup(nsfile_pupper * p)
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

void nstex1d::resize(int32 w, bool resize_data)
{
	if (!resize_data)
	{
		m_width = w;
		return;
	}
	
	if (m_width >= w)
		m_width = w;
	else
	{
		uint8 * tmp = m_raw_data;
		m_raw_data = new uint8[w*bytes_per_pixel()];
		for (uint32 i = 0; i < w*bytes_per_pixel(); ++i)
		{
			if (i < m_width*bytes_per_pixel())
				m_raw_data[i] = tmp[i];
			else
				m_raw_data[i] = 0;
		}
		delete [] tmp;
		m_width = w;
	}
}

nstex2d::nstex2d() :
	nstexture(),
	m_size(),
	m_offset()
{}

nstex2d::nstex2d(const nstex2d & copy_):
	nstexture(copy_),
	m_size(copy_.m_size),
	m_offset(copy_.m_offset)
{}

nstex2d::~nstex2d()
{
}

nstex2d & nstex2d::operator=(nstex2d rhs)
{
	nstexture::operator=(rhs);
	std::swap(m_size, rhs.m_size);
	std::swap(m_offset, rhs.m_offset);
	return *this;
}

int32 nstex2d::pixel_count() const
{
	return m_size.w * m_size.h;
}

void nstex2d::set_offset(const ivec2 & offset)
{
	m_offset = offset;
}

const ivec2 & nstex2d::offset() const
{
	return m_offset;
}

const ivec2 & nstex2d::size() const
{
	return m_size;
}

void nstex2d::pup(nsfile_pupper * p)
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

void nstex2d::resize(const ivec2 & size, bool resize_data)
{
	if (!resize_data)
	{
		m_size = size;
		return;
	}

	uint32 new_size = size.x * size.y * bytes_per_pixel();
	if (pixel_count()*bytes_per_pixel() >= new_size)
		m_size = size;
	else
	{
		uint8 * tmp = m_raw_data;
		m_raw_data = new uint8[new_size];
		for (uint32 i = 0; i < new_size; ++i)
		{
			if (i < pixel_count()*bytes_per_pixel())
				m_raw_data[i] = tmp[i];
			else
				m_raw_data[i] = 0;
		}
		delete [] tmp;
		m_size = size;
	}
}

nstex3d::nstex3d() :
	nstexture(),
	m_size(),
	m_offset()
{}

nstex3d::nstex3d(const nstex3d & copy_):
	nstexture(copy_),
	m_size(copy_.m_size),
	m_offset(copy_.m_offset)
{}

nstex3d::~nstex3d()
{
}

nstex3d & nstex3d::operator=(nstex3d rhs)
{
	nstexture::operator=(rhs);
	std::swap(m_size, rhs.m_size);
	std::swap(m_offset, rhs.m_offset);
	return *this;
}

const ivec3 & nstex3d::size() const
{
	return m_size;
}

void nstex3d::pup(nsfile_pupper * p)
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

void nstex3d::resize(const ivec3 & size, bool resize_data)
{
	if (!resize_data)
	{
		m_size = size;
		return;
	}
		
	uint32 new_size = size.x * size.y * size.z * bytes_per_pixel();
	if (pixel_count()*bytes_per_pixel() >= new_size)
		m_size = size;
	else
	{
		uint8 * tmp = m_raw_data;
		m_raw_data = new uint8[new_size];
		for (uint32 i = 0; i < new_size; ++i)
		{
			if (i < pixel_count()*bytes_per_pixel())
				m_raw_data[i] = tmp[i];
			else
				m_raw_data[i] = 0;
		}
		delete [] tmp;
		m_size = size;
	}
}

int32 nstex3d::pixel_count() const
{
	return m_size.x * m_size.y * m_size.z;
}

void nstex3d::set_offset(const ivec3 & offset)
{
	m_offset = offset;
}

const ivec3 & nstex3d::offset() const
{
	return m_offset;
}

nstex_cubemap::nstex_cubemap() :
	nstexture(),
	m_size(),
	m_offset()
{}

nstex_cubemap::nstex_cubemap(const nstex_cubemap & copy_):
	nstexture(copy_),
	m_size(copy_.m_size),
	m_offset(copy_.m_offset)
{}

nstex_cubemap::~nstex_cubemap()
{
}

nstex_cubemap & nstex_cubemap::operator=(nstex_cubemap rhs)
{
	nstexture::operator=(rhs);
	std::swap(m_size, rhs.m_size);
	std::swap(m_offset, rhs.m_offset);
	return *this;
}

const ivec2 & nstex_cubemap::size() const
{
	return m_size;
}

void nstex_cubemap::resize(const ivec2 & size, bool resize_data)
{
	if (!resize_data)
	{
		m_size = size;
		return;
	}

	uint32 new_size = size.x * size.y * 6 * bytes_per_pixel();
	if (pixel_count()*bytes_per_pixel() >= new_size)
		m_size = size;
	else
	{
		uint8 * tmp = m_raw_data;
		m_raw_data = new uint8[new_size];
		for (uint32 i = 0; i < new_size; ++i)
		{
			if (i < pixel_count()*bytes_per_pixel())
				m_raw_data[i] = tmp[i];
			else
				m_raw_data[i] = 0;
		}
		delete [] tmp;
		m_size = size;
	}
}

void nstex_cubemap::copy_data(uint8 * to_copy, uint8 cube_face)
{
	if (cube_face > 5)
		return;
	nstexture::copy_data(to_copy, cube_face*m_size.w*m_size.h*bytes_per_pixel());
}

uint8 * nstex_cubemap::data(uint8 cube_face)
{
	if (cube_face > 5)
		return nullptr;
	return m_raw_data + cube_face * m_size.x * m_size.y * bytes_per_pixel();
}

void nstex_cubemap::pup(nsfile_pupper * p)
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

int32 nstex_cubemap::pixel_count() const
{
	return m_size.x * m_size.y * 6;
}

void nstex_cubemap::set_offset(const ivec2 & offset)
{
	m_offset = offset;
}

const ivec2 & nstex_cubemap::offset() const
{
	return m_offset;
}
