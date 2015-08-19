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

nstexture::nstexture(tex_type type) :
	m_tex_type(type),
	m_allocated(false),
	m_locked(false),
	m_format(0),
	m_internal_format(0),
	m_pixel_data_type(0),
	m_mipmap_level(0),
	m_generate_mipmaps(true),
	m_border(0),
	m_data(NULL, 0, 0)
{
	set_ext(DEFAULT_TEX_EXTENSION);
}

nstexture::~nstexture()
{
	release();
}

void nstexture::init_gl()
{
	glGenTextures(1, &m_gl_name);
	GLError("nstexture::initGL");
}

void nstexture::bind()
{
	glBindTexture(m_tex_type, m_gl_name);
	GLError("nstexture::bind");
}

uint32 nstexture::border() const
{
	return m_border;
}

void nstexture::set_allocated(bool alloc)
{
	m_allocated = alloc;
}

bool nstexture::allocated()
{
	return m_allocated;
}

/*!
Enable mipmaps for this texture - level of 0 means use the default max mip map level
If you want to disable mipmaps you need to create a new texture with them disabled
If the texture has been allocated then this will call generate mip map function
If it has not, it will call generate mip map function when it is allocated
*/
void nstexture::enable_mipmaps(int32 level)
{
	m_mipmap_level = level;
	m_generate_mipmaps = true;
	if (m_allocated)
	{
		if (level != 0)
			set_parameter_i(max_level, m_mipmap_level);
		glGenerateMipmap(m_tex_type);
		GLError("nstexture::enableMipMaps");
	}
}

void nstexture::set_border(uint32 border)
{
	if (m_allocated)
		return;
	m_border = border;
}

uint32 nstexture::channels()
{
	if (m_format == GL_RGBA || m_format == GL_RGBA_INTEGER || GL_BGRA || GL_BGRA_INTEGER)
		return 4;
	else if (m_format == GL_RGB || m_format == GL_RGB_INTEGER || m_format == GL_BGR || m_format == GL_BGR_INTEGER)
		return 3;
	else if (m_format == GL_RG || m_format == GL_RG_INTEGER || m_format == GL_DEPTH_STENCIL)
		return 2;
	else if (m_format == GL_BLUE || m_format == GL_GREEN || m_format == GL_RED ||
		m_format == GL_BLUE_INTEGER || m_format == GL_GREEN_INTEGER || m_format == GL_RED_INTEGER ||
		m_format == GL_STENCIL_INDEX || m_format == GL_DEPTH_COMPONENT)
		return 1;
	else
		return 0;
}

uint32 nstexture::bpp()
{
	uint8 chans; uint8 bpc;

	chans = channels();

	switch (m_pixel_data_type)
	{
	case(GL_UNSIGNED_BYTE) :
		bpc = 1;
		break;
	case(GL_BYTE) :
		bpc = 1;
		break;
	case(GL_UNSIGNED_SHORT) :
		bpc = 2;
		break;
	case(GL_SHORT) :
		bpc = 2;
		break;
	case(GL_UNSIGNED_INT) :
		bpc = 4;
		break; 
	case(GL_INT) :
		bpc = 4;
		break; 
	case(GL_HALF_FLOAT) :
		bpc = 2;
		break; 
	case(GL_FLOAT) :
		bpc = 4;
		break; 
	case(GL_UNSIGNED_BYTE_3_3_2) :
		bpc = 1;
		break; 
	case(GL_UNSIGNED_BYTE_2_3_3_REV) : 
		bpc = 1;
		break; 
	case(GL_UNSIGNED_SHORT_5_6_5) : 
		bpc = 2;
		break; 
	case(GL_UNSIGNED_SHORT_5_6_5_REV) : 
		bpc = 2;
		break; 
	case(GL_UNSIGNED_SHORT_4_4_4_4) : 
		bpc = 2;
		break;
	case(GL_UNSIGNED_SHORT_4_4_4_4_REV) : 
		bpc = 2;
		break; 
	case(GL_UNSIGNED_SHORT_5_5_5_1) : 
		bpc = 2;
		break; 
	case(GL_UNSIGNED_SHORT_1_5_5_5_REV) : 
		bpc = 2;
		break; 
	case(GL_UNSIGNED_INT_8_8_8_8) :
		bpc = 4;
		break; 
	case(GL_UNSIGNED_INT_8_8_8_8_REV) : 
		bpc = 4;
		break; 
	case(GL_UNSIGNED_INT_10_10_10_2) : 
		bpc = 4;
		break; 
	case(GL_UNSIGNED_INT_2_10_10_10_REV) : 
		bpc = 4;
		break; 
	case(GL_UNSIGNED_INT_24_8) : 
		bpc = 4;
		break; 
	case(GL_UNSIGNED_INT_10F_11F_11F_REV) : 
		bpc = 4;
		break; 
	case(GL_UNSIGNED_INT_5_9_9_9_REV) :
		bpc = 4;
		break; 
	case(GL_FLOAT_32_UNSIGNED_INT_24_8_REV) :
		bpc = 4;
		break;
	default:
		bpc = 0;
	}
	return chans * bpc;
}

float nstexture::parameter_f(get_tex_param p)
{
	GLfloat f;

	if (m_tex_type == tex_cubemap)
		glGetTexParameterfv(BASE_CUBEMAP_FACE, p, &f);
	else
		glGetTexParameterfv(m_tex_type, p, &f);
	
	GLError("nstexture::getParameterf");
	return f;
}

int32 nstexture::parameter_i(get_tex_param p)
{
	GLint i;

	if (m_tex_type == tex_cubemap)
		glGetTexParameteriv(BASE_CUBEMAP_FACE, p, &i);
	else
		glGetTexParameteriv(m_tex_type, p, &i);

	GLError("nstexture::getParameteri");
	return i;
}

void nstexture::disable(uint32 pTexUnit)
{
	glActiveTexture(BASE_TEX_UNIT + pTexUnit);
	GLError("nstexture::disable");
	unbind();
}

void nstexture::enable(uint32 pTexUnit)
{
	glActiveTexture(BASE_TEX_UNIT + pTexUnit);
	GLError("nstexture::enable");
	bind();
}

nstexture::image_data nstexture::data()
{
	return m_data;
}

bool nstexture::locked() const
{
	return m_locked;
}

int32 nstexture::internal_format() const
{
	return m_internal_format;
}

int32 nstexture::format() const
{
	return m_format;
}

int32 nstexture::pixel_data_type() const
{
	return m_pixel_data_type;
}

void nstexture::release()
{
	glDeleteTextures(1,&m_gl_name);
	m_gl_name = 0;
	m_allocated = false;
}

void nstexture::set_gl_name(uint32 glid)
{
	if (glid != 0)
	{
		if (m_gl_name != 0)
			release();
		m_gl_name = glid;
	}
}

void nstexture::set_internal_format(int32 intformat)
{
	if (m_allocated)
		return;

	m_internal_format = intformat;
}

void nstexture::pup(nsfile_pupper * p)
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

void nstexture::set_format(int32 pFormat)
{
	m_format = pFormat;
}

void nstexture::set_pixel_data_type(int32 pDataType)
{
	m_pixel_data_type = pDataType;
}

void nstexture::set_parameter_f(tex_parameter pParam, float pValue)
{
	if (pParam == GL_TEXTURE_BUFFER)
		return;
	glTexParameterf(m_tex_type, pParam, pValue);
	GLError("nstexture::setParameterf");
}

void nstexture::set_parameter_i(tex_parameter pParam, int32 pValue)
{
	if (pParam == GL_TEXTURE_BUFFER)
		return;
	glTexParameteri(m_tex_type, pParam, pValue);
	GLError("nstexture::setParameteri");
}

void nstexture::set_parameter_fv(tex_parameter param, const fvec4 & val)
{
	if (param == GL_TEXTURE_BUFFER)
		return;
	glTexParameterfv(m_tex_type, param, &val[0]);
	GLError("nstexture::setParameterfv");
}

void nstexture::set_parameter_iv(tex_parameter param, const ivec4 & val)
{
	if (param == GL_TEXTURE_BUFFER)
		return;
	glTexParameteriv(m_tex_type, param, &val[0]);
	GLError("nstexture::setParameteriv");
}

void nstexture::set_parameter_Iiv(tex_parameter param, const ivec4 & val)
{
	if (param == GL_TEXTURE_BUFFER)
		return;
	glTexParameterIiv(m_tex_type, param, &val[0]);
	GLError("nstexture::setParameterIiv");
}

void nstexture::set_parameter_Iuiv(tex_parameter param, const uivec4 & val)
{
	if (param == GL_TEXTURE_BUFFER)
		return;
	glTexParameterIuiv(m_tex_type, param, &val[0]);
	GLError("nstexture::setParameterIiuv");
}

void nstexture::unbind()
{
	glBindTexture(m_tex_type, 0);
}

nstexture::tex_type nstexture::texture_type() const
{
	return m_tex_type;
}

/*-------------------------------------------------------------------------------------------------------------------------------------------------------------------*/



nstex1d::nstex1d():
m_width(0),
m_comp_byte_size(0),
m_immutable(false),
nstexture(tex_1d)
{}

nstex1d::~nstex1d()
{
	if (m_locked && m_data.data != NULL)
		delete[] m_data.data;
}

/*
Using width, internal format, pixel data type, format, and mip map level this function
will allocate new texture data to the currently bound texture name. Be sure to bind the texture
before calling allocate.
*/
bool nstex1d::allocate(const void * data)
{
	if (m_allocated)
		return false;

	if (m_comp_byte_size == 0)
	{
		if (m_immutable)
			glTexStorage1D(m_tex_type, m_mipmap_level, m_internal_format, m_width);
		else
			glTexImage1D(m_tex_type, 0, m_internal_format, m_width, m_border, m_format, m_pixel_data_type, data); // specify base mip map level
	}
	else
	{
		glCompressedTexImage1D(m_tex_type, 0, m_internal_format, m_width, m_border, m_comp_byte_size, data);
	}

	m_allocated = !GLError("nstex1d::allocate");
	return m_allocated;
}

/*
Allocate new space for a texture and copy screen pixels from whatever read buffer is currently bound
Read buffer must be bound (glReadBuffer) in order to copy screen pixels
\param lowerLeft x and y screen coordinates
\param dimensions width and height starting from lowerLeft
*/
bool nstex1d::allocate_from_screen(const uivec2 & lowerLeft, const uivec2 dimensions)
{
	if (m_comp_byte_size != 0 || m_immutable) // Cannot perform this operation on a compressed textureor immutable texture
		return false;

	glCopyTexImage1D(m_tex_type, 0, m_internal_format, lowerLeft.x, lowerLeft.y, m_width, m_border);
	m_allocated = !GLError("nstexture::allocateFromScreen()");
	return m_allocated;
}


bool nstex1d::compressed() const
{
	return (m_comp_byte_size != 0);
}

bool nstex1d::immutable() const
{
	return m_immutable;
}

void nstex1d::init()
{
	// do nothing
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

bool nstex1d::lock()
{
	m_locked = true;

	if (!m_allocated)
		return m_locked;

	if (m_comp_byte_size == 0)
	{
		m_data.bpp = bpp();
		m_data.size = m_data.bpp*m_width;
		m_data.data = new uint8[m_data.size];
		glGetTexImage(m_tex_type, 0, m_format, m_pixel_data_type, static_cast<void*>(m_data.data));
	}
	else
	{
		m_data.size = m_comp_byte_size;
		m_data.data = new uint8[m_data.size];
		glGetCompressedTexImage(m_tex_type, 0, static_cast<void*>(m_data.data));
	}

	m_locked = !GLError("nstex1d::lock");
	if (!m_locked)
	{
		m_data.bpp = 0;
		m_data.size = 0;
		delete[] m_data.data;
		m_data.data = NULL;
	}
	return m_locked;
}

bool nstex1d::unlock()
{
	if (!m_locked)
		return m_locked;

	m_locked = false;
	bool ret = set_data(m_data.data, 0, m_width);
	m_data.bpp = 0;
	m_data.size = 0;
	delete[] m_data.data;
	m_data.data = NULL;
	return ret;
}

/*
Set image data starting from the offset and going until offset + dimensions
If data is bound to PIXEL_UNPACK_BUFFER then data will be used as a byte offset in to
the store data
*/
bool nstex1d::set_data(const void * data, uint32 xoffset, uint32 width)
{
	if (!m_allocated || m_immutable)
		return false;

	if (m_comp_byte_size == 0)
		glTexSubImage1D(m_tex_type, 0, xoffset, width, m_format, m_pixel_data_type, data);
	else
		glCompressedTexSubImage1D(m_tex_type, 0, xoffset, width, m_format, m_comp_byte_size, data);

	return GLError("nstex1d::setData");
}

bool nstex1d::set_compressed(uint32 byteSize)
{
	if (m_allocated)
		return false;
	m_comp_byte_size = byteSize;
	return true;
}

/*
Copy pixels from currently bound read buffer to existing texture - does not allocate space
*/
bool nstex1d::set_data_from_screen(uint32 xoffset, const uivec2 & lowerLeft, uint32 width)
{
	if (!m_allocated || m_comp_byte_size != 0 || m_immutable)
		return false;

	glCopyTexSubImage1D(m_tex_type, 0, xoffset, lowerLeft.x, lowerLeft.y, width);
	return GLError("nstex1d::setDataFromScreen");
}

void nstex1d::set_immutable(bool immutable)
{
	if (m_allocated)
		return;

	m_immutable = immutable;
}

void nstex1d::resize(uint32 w)
{
	if (!m_allocated)
	{
		m_width = w;
		return;
	}
	else if (m_immutable)
		return;

	lock();
	uint32 nsize = w*bpp();
	uint8 * cpy = new uint8[nsize];
	for (uint32 i = 0; i < nsize; ++i)
	{
		if (i < m_data.size)
			cpy[i] = m_data.data[i];
		else
			cpy[i] = 0;
	}
	unlock();

	m_allocated = false;
	m_width = w;
	allocate(cpy);
	delete[] cpy;
}

uint32 nstex1d::size()
{
	return m_width;
}


/*------------------------------------------------------------------------------------------------------------------------------------------*/

nstex1d_array::nstex1d_array() :nstex2d() { m_tex_type = tex_1d_array;}
nstex1d_array::~nstex1d_array(){}

/*------------------------------------------------------------------------------------------------------------------------------------------*/

nstex2d::nstex2d() :
m_size(),
m_comp_byte_size(0),
m_immutable(false),
nstexture(tex_2d)
{}

nstex2d::~nstex2d()
{
	if (m_locked && m_data.data != NULL)
		delete[] m_data.data;
}

/*
Using width, internal format, pixel data type, format, and mip map level this function
will allocate new texture data to the currently bound texture name. Be sure to bind the texture
before calling allocate.
*/
bool nstex2d::allocate(const void * data)
{
	if (m_allocated)
		return false;

	if (m_comp_byte_size == 0)
	{
		if (m_immutable)
			glTexStorage2D(m_tex_type, m_mipmap_level, m_internal_format, m_size.w, m_size.h);
		else
			glTexImage2D(m_tex_type, 0, m_internal_format, m_size.w, m_size.h, m_border, m_format, m_pixel_data_type, data); // specify base mip map level
	}
	else
	{
		glCompressedTexImage2D(m_tex_type, 0, m_internal_format, m_size.w, m_size.h, m_border, m_comp_byte_size, data);
	}

	m_allocated = !GLError("nstex2d::allocate");
	return m_allocated;
}

/*
Allocate new space for a texture and copy screen pixels from whatever read buffer is currently bound
Read buffer must be bound (glReadBuffer) in order to copy screen pixels
\param lowerLeft x and y screen coordinates
\param dimensions width and height starting from lowerLeft
*/
bool nstex2d::allocate_from_screen(const uivec2 & lowerLeft, const uivec2 dimensions)
{
	if (m_allocated || m_immutable)
		return false;

	if (m_comp_byte_size != 0) // Cannot perform this operation on a compressed texture
		return false;

	glCopyTexImage2D(m_tex_type, 0, m_internal_format, lowerLeft.x, lowerLeft.y, m_size.w, m_size.h, m_border);
	m_allocated = !GLError("nstex2d::allocateFromScreen()");
	return m_allocated;
}


bool nstex2d::compressed() const
{
	return (m_comp_byte_size != 0);
}

const uivec2 & nstex2d::size() const
{
	return m_size;
}

bool nstex2d::immutable() const
{
	return m_immutable;
}

void nstex2d::init()
{
	// do nothing
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

bool nstex2d::lock()
{
	m_locked = true;

	if (!m_allocated)
		return m_locked;

	if (m_comp_byte_size == 0)
	{
		m_data.bpp = bpp();
		m_data.size = m_data.bpp*m_size.w*m_size.h;
		m_data.data = new uint8[m_data.size];
		glGetTexImage(m_tex_type, 0, m_format, m_pixel_data_type, static_cast<void*>(m_data.data));
	}
	else
	{
		m_data.size = m_comp_byte_size;
		m_data.data = new uint8[m_data.size];
		glGetCompressedTexImage(m_tex_type, 0, static_cast<void*>(m_data.data));
	}

	m_locked = !GLError("nstex2d::lock");
	if (!m_locked)
	{
		m_data.bpp = 0;
		m_data.size = 0;
		delete[] m_data.data;
		m_data.data = NULL;
	}
	return m_locked;
}

void nstex2d::resize(const uivec2 & size)
{
	resize(size.x,size.y);
}

void nstex2d::resize(uint32 w, uint32 h)
{
	if (!m_allocated)
	{
		m_size.set(w,h);
		return;
	}
	else if (m_immutable)
		return;

	lock();
	uint32 nsize = w*h*bpp();
	uint8 * cpy = new uint8[nsize];
	for (uint32 i = 0; i < nsize; ++i)
	{
		if (i < m_data.size)
			cpy[i] = m_data.data[i];
		else
			cpy[i] = 0;
	}
	unlock();

	m_allocated = false;
	m_size.set(w, h);
	allocate(cpy);
	delete[] cpy;
}

bool nstex2d::unlock()
{
	if (!m_locked) // cannot be locked if not allocated
		return m_locked;

	m_locked = false;
	bool ret = set_data(m_data.data, 0, m_size);
	m_data.bpp = 0;
	m_data.size = 0;
	delete[] m_data.data;
	m_data.data = NULL;
	return ret;
}

/*
Set image data starting from the offset and going until offset + dimensions
If data is bound to PIXEL_UNPACK_BUFFER then data will be used as a byte offset in to
the store data
*/
bool nstex2d::set_data(const void * data, const uivec2 & offset, const uivec2 & dimensions)
{
	if (!m_allocated || m_immutable)
		return false;

	if (m_comp_byte_size == 0)
		glTexSubImage2D(m_tex_type, 0, offset.x, offset.y, dimensions.w, dimensions.h, m_format, m_pixel_data_type, data);
	else
		glCompressedTexSubImage2D(m_tex_type, 0, offset.x, offset.y, dimensions.w, dimensions.h, m_format, m_comp_byte_size, data);

	return !GLError("nstex2d::setData");
}

bool nstex2d::set_compressed(uint32 byteSize)
{
	if (m_allocated)
		return false;
	m_comp_byte_size = byteSize;
	return true;
}

/*
Copy pixels from currently bound read buffer to existing texture - does not allocate space
*/
bool nstex2d::set_data_from_screen(const uivec2 & offset, const uivec2 & lowerLeft, const uivec2 & dimensions)
{
	if (!m_allocated || m_comp_byte_size != 0 || m_immutable)
		return false;

	glCopyTexSubImage2D(m_tex_type, 0, offset.x, offset.y, lowerLeft.x, lowerLeft.y, dimensions.w, dimensions.h);
	return !GLError("nstex2d::setDataFromScreen");
}

void nstex2d::set_immutable(bool immutable)
{
	if (m_allocated)
		return;

	m_immutable = immutable;
}


const uivec2 & nstex2d::size()
{
	return m_size;
}

/*----------------------------------------------------------------------------------------------------------------------------------------*/

nstex2d_array::nstex2d_array() : nstex3d() { m_tex_type = tex_2d_array; }
nstex2d_array::~nstex2d_array(){}

/*----------------------------------------------------------------------------------------------------------------------------------------*/

nstex3d::nstex3d() :
m_size(),
m_comp_byte_size(0),
m_immutable(false),
nstexture(tex_3d)
{}

nstex3d::~nstex3d()
{
	if (m_locked && m_data.data != NULL)
		delete[] m_data.data;
}

/*
Using width, internal format, pixel data type, format, and mip map level this function
will allocate new texture data to the currently bound texture name. Be sure to bind the texture
before calling allocate.
*/
bool nstex3d::allocate(const void * data)
{
	if (m_allocated)
		return false;

	if (m_comp_byte_size == 0)
	{
		if (m_immutable)
			glTexStorage3D(m_tex_type, m_mipmap_level, m_internal_format, m_size.x, m_size.y, m_size.z);
		else
			glTexImage3D(m_tex_type, 0, m_internal_format, m_size.x, m_size.y, m_size.z, m_border, m_format, m_pixel_data_type, data); // specify base mip map level
	}
	else
	{
		glCompressedTexImage3D(m_tex_type, 0, m_internal_format, m_size.x, m_size.y, m_size.z, m_border, m_comp_byte_size, data);
	}

	m_allocated = !GLError("nstex3d::allocate");
	return m_allocated;
}


bool nstex3d::compressed() const
{
	return (m_comp_byte_size != 0);
}

const uivec3 & nstex3d::size() const
{
	return m_size;
}

void nstex3d::init()
{
	// do nothing
}

bool nstex3d::immutable() const
{
	return m_immutable;
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

bool nstex3d::lock()
{
	m_locked = true;

	if (!m_allocated)
		return m_locked;

	if (m_comp_byte_size == 0)
	{
		m_data.bpp = bpp();
		m_data.size = m_data.bpp*m_size.x*m_size.y*m_size.z;
		m_data.data = new uint8[m_data.size];
		glGetTexImage(m_tex_type, 0, m_format, m_pixel_data_type, static_cast<void*>(m_data.data));
	}
	else
	{
		m_data.size = m_comp_byte_size;
		m_data.data = new uint8[m_data.size];
		glGetCompressedTexImage(m_tex_type, 0, static_cast<void*>(m_data.data));
	}

	m_locked = !GLError("nstex3d::lock");
	if (!m_locked)
	{
		m_data.bpp = 0;
		m_data.size = 0;
		delete[] m_data.data;
		m_data.data = NULL;
	}
	return m_locked;
}

void nstex3d::resize(const uivec3 & size)
{
	resize(size.x,size.y,size.z);
}

void nstex3d::resize(uint32 w, uint32 h, uint32 layers)
{
	if (!m_allocated)
	{
		m_size.set(w, h, layers);
		return;
	}
	else if (m_immutable)
		return;

	lock();
	uint32 nsize = w*h*layers*bpp();
	uint8 * cpy = new uint8[nsize];
	for (uint32 i = 0; i < nsize; ++i)
	{
		if (i < m_data.size)
			cpy[i] = m_data.data[i];
		else
			cpy[i] = 0;
	}
	unlock();

	m_allocated = false;
	m_size.set(w, h, layers);
	allocate(cpy);
	delete[] cpy;
}

bool nstex3d::unlock()
{
	if (!m_locked) // cannot be locked if not allocated
		return m_locked;

	m_locked = false;
	bool ret = set_data(m_data.data, 0, m_size);
	m_data.bpp = 0;
	m_data.size = 0;
	delete[] m_data.data;
	m_data.data = NULL;
	return ret;
}

/*
Set image data starting from the offset and going until offset + dimensions
If data is bound to PIXEL_UNPACK_BUFFER then data will be used as a byte offset in to
the store data
*/
bool nstex3d::set_data(const void * data, const uivec3 & offset, const uivec3 & dimensions)
{
	if (!m_allocated || m_immutable)
		return false;

	if (m_comp_byte_size == 0)
		glTexSubImage3D(m_tex_type, 0, offset.x, offset.y, offset.z, dimensions.x, dimensions.y, dimensions.z, m_format, m_pixel_data_type, data);
	else
		glCompressedTexSubImage3D(m_tex_type, 0, offset.x, offset.y, offset.z, dimensions.x, dimensions.y, dimensions.z, m_format, m_comp_byte_size, data);

	return !GLError("nstex3d::setData");
}

bool nstex3d::set_compressed(uint32 byteSize)
{
	if (m_allocated)
		return false;
	m_comp_byte_size = byteSize;
	return true;
}

/*
Copy pixels from currently bound read buffer to existing texture - does not allocate space
*/
bool nstex3d::set_data_from_screen(const uivec3 & offset, const uivec2 & lowerLeft, const uivec2 & dimensions)
{
	if (!m_allocated || m_comp_byte_size != 0 || m_immutable)
		return false;

	glCopyTexSubImage3D(m_tex_type, 0, offset.x, offset.y, offset.z, lowerLeft.x, lowerLeft.y, dimensions.w, dimensions.h);
	return !GLError("nstex3d::setDataFromScreen");
}

void nstex3d::set_immutable(bool immutable)
{
	if (m_allocated)
		return;

	m_immutable = immutable;
}

const uivec3 & nstex3d::size()
{
	return m_size;
}

/*------------------------------------------------------------------------------------------------------------------------------*/

nstex_cubemap::nstex_cubemap() :
m_size(),
m_comp_byte_size(0),
nstexture(tex_cubemap)
{}

nstex_cubemap::~nstex_cubemap()
{
	if (m_locked && m_data.data != NULL)
		delete[] m_data.data;
}

/*
Using width, internal format, pixel data type, format, and mip map level this function
will allocate new texture data to the currently bound texture name. Be sure to bind the texture
before calling allocate.
*/
bool nstex_cubemap::allocate(const void * data)
{
	if (m_allocated)
		return false;

	bool s = true;
	for (uint32 i = pos_x; i <= neg_z; i += 1)
	{
		s = s && allocate(cube_face(i), data);
		m_allocated = false;
	}

	return (m_allocated = s);
}

bool nstex_cubemap::allocate(cube_face f, const void * data)
{
	if (m_allocated)
		return false;

	if (m_comp_byte_size == 0)
		glTexImage2D(f, 0, m_internal_format, m_size.w, m_size.h, m_border, m_format, m_pixel_data_type, data); // specify base mip map level
	else
		glCompressedTexImage2D(f, 0, m_internal_format, m_size.w, m_size.h, m_border, m_comp_byte_size, data);

	m_allocated = !GLError("nstex_cubemap::allocateFace");
	return m_allocated;
}

bool nstex_cubemap::allocate_from_screen(const uivec2 & lowerLeft, const uivec2 dimensions)
{
	if (m_allocated)
		return false;

	if (m_comp_byte_size != 0) // Cannot perform this operation on a compressed texture
		return false;

	bool s = true;
	for (uint32 i = pos_x; i <= neg_z; ++i)
		s = s && allocate_from_screen(cube_face(i), lowerLeft, dimensions);

	return (m_allocated = s);
}

/*
Allocate new space for a texture and copy screen pixels from whatever read buffer is currently bound
Read buffer must be bound (glReadBuffer) in order to copy screen pixels
\param lowerLeft x and y screen coordinates
\param dimensions width and height starting from lowerLeft
*/
bool nstex_cubemap::allocate_from_screen(cube_face f, const uivec2 & lowerLeft, const uivec2 dimensions)
{
	if (m_allocated)
		return false;

	if (m_comp_byte_size != 0) // Cannot perform this operation on a compressed texture
		return false;

	glCopyTexImage2D(f, 0, m_internal_format, lowerLeft.x, lowerLeft.y, m_size.w, m_size.h, m_border);
	m_allocated = !GLError("nstexture::allocateFromScreen()");
	return m_allocated;
}


bool nstex_cubemap::compressed() const
{
	return (m_comp_byte_size != 0);
}

const uivec2 & nstex_cubemap::size() const
{
	return m_size;
}

void nstex_cubemap::init()
{
	// do nothing
}

bool nstex_cubemap::lock()
{
	m_locked = true;

	if (!m_allocated)
		return m_locked;

	if (m_comp_byte_size == 0)
	{
		m_data.bpp = bpp();
		m_data.size = m_data.bpp*m_size.w*m_size.h*6;
		m_data.data = new uint8[m_data.size];
		for (uint32 i = 0; i < 6; ++i)
			glGetTexImage(pos_x + i, 0, m_format, m_pixel_data_type, static_cast<void*>(m_data.data + i*m_data.bpp*m_size.w*m_size.h));
	}
	else
	{
		m_data.size = m_comp_byte_size;
		m_data.data = new uint8[m_data.size];
		for (uint32 i = 0; i < 6; ++i)
			glGetCompressedTexImage(pos_x + i, 0, static_cast<void*>(m_data.data + i*m_data.bpp*m_size.w*m_size.h));
	}

	m_locked = !GLError("nstex_cubemap::lock");
	if (!m_locked)
	{
		m_data.bpp = 0;
		m_data.size = 0;
		delete[] m_data.data;
		m_data.data = NULL;
	}
	return m_locked;
}

bool nstex_cubemap::lock(cube_face f)
{
	if (m_locked || !m_allocated)
		return m_locked;

	if (m_comp_byte_size == 0)
	{
		m_data.bpp = bpp();
		m_data.size = m_data.bpp*m_size.w*m_size.h;
		m_data.data = new uint8[m_data.size];
		glGetTexImage(f, 0, m_format, m_pixel_data_type, static_cast<void*>(m_data.data));
	}
	else
	{
		m_data.size = m_comp_byte_size;
		m_data.data = new uint8[m_data.size];
		glGetCompressedTexImage(f, 0, static_cast<void*>(m_data.data));
	}

	m_locked = !GLError("nstex_cubemap::lock");
	if (!m_locked)
	{
		m_data.bpp = 0;
		m_data.size = 0;
		delete[] m_data.data;
		m_data.data = NULL;
	}
	return m_locked;
}

void nstex_cubemap::resize(const uivec2 & size)
{
	resize(size.w,size.h);
}

void nstex_cubemap::resize(uint32 w, uint32 h)
{
	if (!m_allocated)
	{
		m_size.set(w, h);
		return;
	}

	lock();
	uint32 nsize = w*h*6*bpp();
	uint8 * cpy = new uint8[nsize];
	for (uint32 i = 0; i < nsize; ++i)
	{
		if (i < m_data.size)
			cpy[i] = m_data.data[i];
		else
			cpy[i] = 0;
	}
	unlock();

	m_allocated = false;
	m_size.set(w, h);
	allocate(cpy);
	delete[] cpy;
}

bool nstex_cubemap::unlock()
{
	if (!m_locked) // cannot be locked if not allocated
		return m_locked;

	m_locked = false;

	bool s = true;
	for (uint32 i = 0; i < 6; ++i)
		s = s && set_data(cube_face(pos_x + i), m_data.data + i*(m_data.bpp*m_size.x*m_size.y), 0, m_size);

	m_data.bpp = 0;
	m_data.size = 0;
	delete[] m_data.data;
	m_data.data = NULL;
	return s;
}

bool nstex_cubemap::unlock(cube_face f)
{
	if (!m_locked) // cannot be locked if not allocated
		return m_locked;

	m_locked = false;
	bool ret = set_data(f, m_data.data, 0, m_size);
	m_data.bpp = 0;
	m_data.size = 0;
	delete[] m_data.data;
	m_data.data = NULL;
	return ret;
}

/*
Set image data starting from the offset and going until offset + dimensions
If data is bound to PIXEL_UNPACK_BUFFER then data will be used as a byte offset in to
the store data
*/
bool nstex_cubemap::set_data(const void * data, const uivec2 & offset, const uivec2 & dimensions)
{
	if (!m_allocated)
		return false;

	bool s = true;
	for (uint32 i = pos_x; i <= neg_z; ++i)
		s = s && set_data(cube_face(i), data, offset, dimensions);

	return s;
}

bool nstex_cubemap::set_data(cube_face f, const void * data, const uivec2 & offset, const uivec2 & dimensions)
{
	if (!m_allocated)
		return false;

	if (m_comp_byte_size == 0)
		glTexSubImage2D(f, 0, offset.x, offset.y, dimensions.w, dimensions.h, m_format, m_pixel_data_type, data);
	else
		glCompressedTexSubImage2D(f, 0, offset.x, offset.y, dimensions.w, dimensions.h, m_format, m_comp_byte_size, data);

	return !GLError("nstex_cubemap::setData");
}

bool nstex_cubemap::set_compressed(uint32 byteSize)
{
	if (m_allocated)
		return false;
	m_comp_byte_size = byteSize;
	return true;
}

/*
Copy pixels from currently bound read buffer to existing texture - does not allocate space
*/
bool nstex_cubemap::set_data_from_screen(const uivec2 & offset, const uivec2 & lowerLeft, const uivec2 & dimensions)
{
	if (!m_allocated || m_comp_byte_size != 0)
		return false;

	bool s = true;
	for (uint32 i = pos_x; i <= neg_z; ++i)
		s = s && set_data_from_screen(cube_face(i), offset, lowerLeft, dimensions);

	return s;
}

bool nstex_cubemap::set_data_from_screen(cube_face f, const uivec2 & offset, const uivec2 & lowerLeft, const uivec2 & dimensions)
{
	if (!m_allocated || m_comp_byte_size != 0)
		return false;

	glCopyTexSubImage2D(f, 0, offset.x, offset.y, lowerLeft.x, lowerLeft.y, dimensions.w, dimensions.h);
	return !GLError("nstex_cubemap::setDataFromScreen");
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

/*-------------------------------------------------------------------------------------------------------------------------------------------------*/

nstex_rectangle::nstex_rectangle() { m_tex_type = tex_rectangle; }
nstex_rectangle::~nstex_rectangle() {}

nstex_cubemap_array::nstex_cubemap_array() { m_tex_type = tex_cubemap_array; }
nstex_cubemap_array::~nstex_cubemap_array() {}

nstex2d_multisample::nstex2d_multisample() { m_tex_type = tex_2d_multisample;}
nstex2d_multisample::~nstex2d_multisample() {}

nstex2d_multisample_array::nstex2d_multisample_array() { m_tex_type = tex_2d_multisample_array; }
nstex2d_multisample_array::~nstex2d_multisample_array() {}

nstex_buffer::nstex_buffer():nstexture(tex_buffer) {}
nstex_buffer::~nstex_buffer() {}

bool nstex_buffer::allocate(const void * data)
{
	return false;
}

void nstex_buffer::init()
{
	// do nothing
}

bool nstex_buffer::lock()
{
	return false;
}

bool nstex_buffer::unlock()
{
	return false;
}
