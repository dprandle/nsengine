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

#include <nsresource.h>
#include <nsvector.h>
#include <nsgl_object.h>
#include <nssignal.h>

class nstexture_inst;

class nstexture : public nsresource, public nsgl_object
{
  public:
	struct image_data
	{
		image_data(uint8 * data_=nullptr, uint32 size_=0, uint32 bpp_=0);
		~image_data();
		image_data(const image_data & copy_);
		image_data & operator=(image_data rhs);
		
		uint8 * data;
		uint32 size;
		uint32 bpp;
	};

	enum tex_type {
		tex_1d = GL_TEXTURE_1D,
		tex_1d_array = GL_TEXTURE_1D_ARRAY,
		tex_2d = GL_TEXTURE_2D,
		tex_2d_array = GL_TEXTURE_2D_ARRAY,
		tex_3d = GL_TEXTURE_3D,
		tex_rectangle = GL_TEXTURE_RECTANGLE,
		tex_buffer = GL_TEXTURE_BUFFER,
		tex_cubemap = GL_TEXTURE_CUBE_MAP,
		tex_cubemap_array = GL_TEXTURE_CUBE_MAP_ARRAY,
		tex_2d_multisample = GL_TEXTURE_2D_MULTISAMPLE,
		tex_2d_multisample_array = GL_TEXTURE_2D_MULTISAMPLE_ARRAY
	};

	enum tex_parameter {
		depth_stencil_texture_mode = GL_DEPTH_STENCIL_TEXTURE_MODE,
		base_level = GL_TEXTURE_BASE_LEVEL,
		max_level = GL_TEXTURE_MAX_LEVEL,
		compare_func = GL_TEXTURE_COMPARE_FUNC,
		compare_mode = GL_TEXTURE_COMPARE_MODE,
		LOD_bias = GL_TEXTURE_LOD_BIAS,
		min_filter = GL_TEXTURE_MIN_FILTER,
		mag_filter = GL_TEXTURE_MAG_FILTER,
		min_LOD = GL_TEXTURE_MIN_LOD,
		max_LOD = GL_TEXTURE_MAX_LOD,
		swizzle_r = GL_TEXTURE_SWIZZLE_R,
		swizzle_g = GL_TEXTURE_SWIZZLE_G,
		swizzle_b = GL_TEXTURE_SWIZZLE_B,
		swizzle_a = GL_TEXTURE_SWIZZLE_A,
		wrap_s = GL_TEXTURE_WRAP_S,
		wrap_t = GL_TEXTURE_WRAP_T,
		wrap_r = GL_TEXTURE_WRAP_R };

	enum get_tex_param 
	{
		tex_width = GL_TEXTURE_WIDTH, 
		tex_height = GL_TEXTURE_HEIGHT,
		tex_depth = GL_TEXTURE_DEPTH,
		tex_internal_format = GL_TEXTURE_INTERNAL_FORMAT, 
		tex_border = GL_TEXTURE_BORDER, 
		tex_size_red = GL_TEXTURE_RED_SIZE, 
		tex_size_green = GL_TEXTURE_GREEN_SIZE, 
		tex_size_blue = GL_TEXTURE_BLUE_SIZE, 
		tex_size_alpha = GL_TEXTURE_ALPHA_SIZE, 
		tex_size_luminance = GL_TEXTURE_LUMINANCE_SIZE,
		tex_size_intensity = GL_TEXTURE_INTENSITY_SIZE, 
		tex_size_depth = GL_TEXTURE_DEPTH_SIZE, 
		tex_compressed = GL_TEXTURE_COMPRESSED,
		tex_size_compressed = GL_TEXTURE_COMPRESSED_IMAGE_SIZE
	};

	template <class PUPer>
	friend void pup(PUPer & p, nstexture & sm);

	nstexture(tex_type type);

	nstexture(const nstexture & copy_);

	~nstexture();

	nstexture & operator=(nstexture_inst rhs);
	
	void init_gl();

	void bind() const;

	/*
	  Get the number of bytes per pixel using opengl format and type parameters
	*/
	uint32 bpp() const;

	uint32 channels() const;

	virtual bool allocate(const void * data) = 0;

	void set_allocated(bool alloc);

	bool allocated();

	virtual image_data data() const = 0;

	virtual void data(uint8 * array_, uint32 size_) const = 0;

	virtual void data(ui8_vector * array_) const = 0;

	int32 format() const;

	int32 pixel_data_type() const;

	int32 internal_format() const;

	uint32 border() const;

	float parameter_f(get_tex_param p);

	int32 parameter_i(get_tex_param p);

	/*!
	  Enable mipmaps for this texture - level of 0 means use the default max mip map level
	  If you want to disable mipmaps you need to create a new texture with them disabled
	  If the texture has been allocated then this will call generate mip map function
	  If it has not, it will call generate mip map function when it is allocated
	  \param level how many mip map levels to include
	*/
	void enable_mipmaps(int32 level = 0);

	virtual void pup(nsfile_pupper * p);

	/*
	  Release the opengl image name
	*/
	void release();

	void set_border(uint32 border);

	tex_type texture_type() const;

//	virtual void set_data(image_data) = 0;

	/*
	  Set the format - see opengl specs for which formats are allowable - this determines how pixel data is read in
	  when we allocate or how it is returned when we get the texture data from the GPU

	  \param OpenGL format used when uploading or downloading pixel data from a stored image on the GPU
	*/
	void set_format(int32 format);

	/*
	  Set the texture internal format - it is not gaurenteed however that the image will be stored in this format - but it
	  has a good inlfuence on what the driver will do.

	  As per the specs, image data can be stored internally as unsigned or signed integers, floats, or unsigned/signed normalized integers.
	  Floats resolve in the shader to a vector of floats, matching exactly what they are internally. uint32egers will resolve in to a vector of uint32egers
	  in the shader, and signed integers will resolve in to a vector of signed integers. Normalized uint32egers will resolve in to a vector of floats
	  between 0.0 and 1.0 in the shader. Each integer value is divided by the max possible integer value for that component's bitdepth. For signed normalized,
	  it will resolve to a vector of floats between -1.0 and 1.0 with each positive value being divided my the max signed integer value, and each negative value
	  being divided by the most negative integer value for that component's bitdepth. Usually the ditdepth is the same for all components (where r g b and a are considered
	  components).

	  You dont have to store all of the components of the texture internally - if a texture missing certain components is sampled in a shader, the missing components
	  will resolve to 0 except for alpha which will resolve to 1. The sampled color value will always be a 4 element vector (rgba).

	  GL_DEPTH_COMPONENT_16, _24, _32, and _32F are an example of where only one component is stored internally - the cool thing about these texture formats is that
	  they can be sampled with shadow samplers. The integer formats (all without the F) are all normalized.

	  This function will do nothing if the texture data has already been allocated

	  \param internalFormat The opengl acceptable image format
	*/
	void set_internal_format(int32 intformat);

	void set_parameter_f(tex_parameter param, float pValue);

	void set_parameter_i(tex_parameter param, int32 pValue);

	void set_parameter_fv(tex_parameter param, const fvec4 & val);

	void set_parameter_iv(tex_parameter param, const ivec4 & val);

	void set_parameter_Iiv(tex_parameter param, const ivec4 & val);

	void set_parameter_Iuiv(tex_parameter param, const uivec4 & val);

	void set_gl_name(uint32 glid);

	/*
	  Set the pixel data type - see opengl doc for what types are acceptable
	  The is the type for each component given in format - if a single type such as GL_UNSIGNED_BYTE is specified then all of the components
	  given in the format are of that type. Sometimes images have different bit sizes for each component however, such as 16 bit RGB where the
	  red and blue channels are each 5 bits and the green channel is 6 bits. In this case the format would be GL_RGB and the type would be
	  GL_UNSIGNED_SHORT_5_6_5

	  The specification is GL_[base type]_[size1]_[size2]_[size3]_[size4](_REV)

	  The _REV reversed the order the components are put in the format.. ie if the previous example was GL_UNSIGNED_SHORT_5_5_6_REV with format GL_RGB, this would
	  be equivalent to GL_UNSIGNED_SHORT_5_5_6 with format as GL_BGR

	  The base type must be some type that is large enough to store all of the 4 components
	*/
	void set_pixel_data_type(int32 pType);

	void unbind() const;

	// signals

	ns::signal<image_data*> tex_data_changed;

  protected:
	
	tex_type m_tex_type;
	bool m_allocated;
	GLint m_format;
	GLint m_internal_format;
	GLint m_pixel_data_type;
	int32 m_mipmap_level;
	bool m_generate_mipmaps;
	uint32 m_border;
	image_data m_data;
};

class nstexture_inst : public nstexture
{
	nstexture_inst(const nstexture & rhs):nstexture(rhs) {}
	void init() {std::terminate();}
 	void pup(nsfile_pupper *) {std::terminate();}
	bool allocate(const void *) {std::terminate();}
	image_data data() const {std::terminate();}
	void data(uint8 *, uint32) const {std::terminate();}
	void data(ui8_vector *) const {std::terminate();}
};


template <class PUPer>
void pup(PUPer & p, nstexture::image_data & dat, const nsstring & var_name)
{
	pup(p, dat.bpp, var_name + ".bpp");
	pup(p, dat.size, var_name + ".size");
	for (uint32 i = 0; i < dat.size; ++i)
		pup(p, dat.data[i], ".data[" + std::to_string(i) + "]");
}

template<class PUPer>
void pup(PUPer & p, nstexture::tex_type & en, const nsstring & var_name)
{
	uint32 in = static_cast<uint32>(en);
	pup(p, in, var_name);
	en = static_cast<nstexture::tex_type>(in);
}

template <class PUPer>
void pup(PUPer & p, nstexture & sm)
{
	pup(p, sm.m_tex_type,"tex_type");
	pup(p, sm.m_format,"format");
	pup(p, sm.m_internal_format,"internal_format");
	pup(p, sm.m_pixel_data_type,"pixel_data_type");
	pup(p, sm.m_mipmap_level,"mipmap_level");
	pup(p, sm.m_generate_mipmaps,"generate_mipmaps");
	pup(p, sm.m_border,"border");
	if (p.mode() == PUP_IN)
	{
		if (sm.m_gl_name == 0)
			sm.init_gl();
		sm.bind();
		sm.allocate(NULL);
		sm.unbind();
	}
	nstexture::image_data i_dat(sm.data());
	pup(p, i_dat, "data");
}

template <class PUPer>
void pup(PUPer & p, nstexture * sm)
{
	pup(p, *sm);
}

// Save using raw data
class nstex1d : public nstexture
{
  public:
	template <class PUPer>
	friend void pup(PUPer & p, nstex1d & sm);

	nstex1d();

	nstex1d(const nstex1d & copy_);

	~nstex1d();

	nstex1d & operator=(nstex1d rhs);

/*
	  Using width, internal format, pixel data type, format, and mip map level this function
	  will allocate new texture data to the currently bound texture name. Be sure to bind the texture
	  before calling allocate.
	*/
	bool allocate(const void * data);

	/*
	  Allocate new space for a texture and copy screen pixels from whatever read buffer is currently bound
	  Read buffer must be bound (glReadBuffer) in order to copy screen pixels
	  \param lowerLeft x and y screen coordinates
	  \param dim width and height starting from lowerLeft
	*/
	bool allocate_from_screen(const uivec2 & lowerLeft, const uivec2 dimensions);

	bool compressed() const;

	void init();

	bool immutable() const;

	virtual image_data data() const;

	virtual void data(uint8 * array_, uint32 size_) const;

	virtual void data(ui8_vector * array_) const;

	virtual void pup(nsfile_pupper * p);

	virtual void resize(uint32 w);

	bool set_compressed(uint32 byteSize);

	/*
	  Set image data starting from the offset and going until offset + dimensions
	  If data is bound to PIXEL_UNPACK_BUFFER then data will be used as a byte offset in to
	  the store data
	  \param offset x and y offset in to the image
	  \param dimensions width and height of the sub image
	  \data Pixel data for the image
	*/
	bool set_data(const void * data, uint32 xoffset, uint32 width);

	/*
	  Copy pixels from currently bound read buffer to existing texture - does not allocate space
	  \param offset the offset in elements 
	*/
	bool set_data_from_screen(uint32 xoffset, const uivec2 & lowerLeft, uint32 width);

	void set_immutable(bool immutable);

	uint32 size();
	
  private:
	uint32 m_width;
	uint32 m_comp_byte_size;
	bool m_immutable;
};

template <class PUPer>
void pup(PUPer & p, nstex1d & sm)
{
	pup(p, sm.m_width,"width");
	pup(p, sm.m_comp_byte_size, "comp_byte_size");
	pup(p, sm.m_immutable, "immutable");
	pup(p, static_cast<nstexture*>(&sm));
}

template <class PUPer>
void pup(PUPer & p, nstex1d * sm)
{
	pup(p, *sm);
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

/*
	  Using width, internal format, pixel data type, format, and mip map level this function
	  will allocate new texture data to the currently bound texture name. Be sure to bind the texture
	  before calling allocate.
	*/
	bool allocate(const void * data);

	/*
	  Allocate new space for a texture and copy screen pixels from whatever read buffer is currently bound
	  Read buffer must be bound (glReadBuffer) in order to copy screen pixels
	  \param lowerLeft x and y screen coordinates
	  \param dimensions width and height starting from lowerLeft
	*/
	bool allocate_from_screen(const uivec2 & lowerLeft, const uivec2 dimensions);

	bool compressed() const;

	virtual image_data data() const;

	virtual void data(uint8 * array_, uint32 size_) const;

	virtual void data(ui8_vector * array_) const;

	const uivec2 & size() const;

	bool immutable() const;

	void init();

	virtual void pup(nsfile_pupper * p);

	virtual void resize(uint32 w, uint32 h);

	void resize(const uivec2 & sz);

	bool set_compressed(uint32 byteSize);

	/*
	  Set image data starting from the offset and going until offset + dimensions
	  If data is bound to PIXEL_UNPACK_BUFFER then data will be used as a byte offset in to
	  the store data
	  \param offset x and y offset in to the image
	  \param dimensions width and height of the sub image
	  \data Pixel data for the image
	*/
	bool set_data(const void * data, const uivec2 & offset, const uivec2 & dimensions);

	void set_immutable(bool immutable);

	/*
	  Copy pixels from currently bound read buffer to existing texture - does not allocate space
	  \param offset the offset in elements
	*/
	bool set_data_from_screen(const uivec2 & offset, const uivec2 & lowerLeft, const uivec2 & dimensions);

	const uivec2 & size();

  private:
	uivec2 m_size;
	uint32 m_comp_byte_size;
	bool m_immutable;
};

template <class PUPer>
void pup(PUPer & p, nstex2d & sm)
{
	pup(p, sm.m_size, "size");
	pup(p, sm.m_comp_byte_size, "comp_byte_size");
	pup(p, sm.m_immutable, "immutable");
	pup(p, static_cast<nstexture*>(&sm));
}

template <class PUPer>
void pup(PUPer & p, nstex2d * sm)
{
	pup(p, *sm);
}

class nstex1d_array : public nstex2d
{
  public:
	nstex1d_array();
	~nstex1d_array();
};

class nstex3d : public nstexture
{
  public:
	template <class PUPer>
	friend void pup(PUPer & p, nstex3d & sm);

	nstex3d();
	
	nstex3d(const nstex3d & copy_);

	~nstex3d();

	nstex3d & operator=(nstex3d rhs);

/*
	  Using width, internal format, pixel data type, format, and mip map level this function
	  will allocate new texture data to the currently bound texture name. Be sure to bind the texture
	  before calling allocate.
	*/
	bool allocate(const void * data);

	bool compressed() const;

	virtual image_data data() const;

	virtual void data(uint8 * array_, uint32 size_) const;

	virtual void data(ui8_vector * array_) const;

	const uivec3 & size() const;

	bool immutable() const;

	void init();

	virtual void pup(nsfile_pupper * p);

	virtual void resize(uint32 w, uint32 h, uint32 layers);

	void resize(const uivec3 & size);

	bool set_compressed(uint32 byteSize);

	/*
	  Set image data starting from the offset and going until offset + dimensions
	  If data is bound to PIXEL_UNPACK_BUFFER then data will be used as a byte offset in to
	  the store data
	  \param offset x and y offset in to the image
	  \param dimensions width and height of the sub image
	  \data Pixel data for the image
	*/
	bool set_data(const void * data, const uivec3 & offset, const uivec3 & dimensions);

	/*
	  Copy pixels from currently bound read buffer to existing texture - does not allocate space
	  \param offset the offset in elements
	*/
	bool set_data_from_screen(const uivec3 & offset, const uivec2 & lowerLeft, const uivec2 & dimensions);

	void set_immutable(bool immutable);

	const uivec3 & size();

  private:
	uivec3 m_size;
	uint32 m_comp_byte_size;
	bool m_immutable;
};

template <class PUPer>
void pup(PUPer & p, nstex3d & sm)
{
	pup(p, sm.m_size, "size");
	pup(p, sm.m_comp_byte_size, "comp_byte_size");
	pup(p, sm.m_immutable, "immutable");
	pup(p, static_cast<nstexture*>(&sm));
}

template <class PUPer>
void pup(PUPer & p, nstex3d * sm)
{
	pup(p, *sm);
}

class nstex2d_array : public nstex3d
{
  public:
	nstex2d_array();
	~nstex2d_array();
};

class nstex_cubemap : public nstexture
{
  public:

	enum cube_face {
		pos_x = GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		neg_x = GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
		pos_y = GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
		neg_y = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
		pos_z = GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
		neg_z = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
	};

	template <class PUPer>
	friend void pup(PUPer & p, nstex_cubemap & sm);

	nstex_cubemap();

	nstex_cubemap(const nstex_cubemap & copy_);

	~nstex_cubemap();

	nstex_cubemap & operator=(nstex_cubemap rhs);

	/*
	  Using width, internal format, pixel data type, format, and mip map level this function
	  will allocate new texture data to the currently bound texture name. Be sure to bind the texture
	  before calling allocate.
	*/
	bool allocate(const void * data);

	bool allocate(cube_face f, const void * data);


	bool allocate_from_screen(const uivec2 & lowerLeft, const uivec2 dim);
	/*
	  Allocate new space for a texture and copy screen pixels from whatever read buffer is currently bound
	  Read buffer must be bound (glReadBuffer) in order to copy screen pixels
	  \param lowerLeft x and y screen coordinates
	  \param dimensions width and height starting from lowerLeft
	*/
	bool allocate_from_screen(cube_face f, const uivec2 & lowerLeft, const uivec2 dimensions);

	bool compressed() const;

	virtual image_data data() const;

	virtual void data(uint8 * array_, uint32 size_) const;

	virtual void data(ui8_vector * array_) const;
	
	virtual image_data data(cube_face f) const;

	virtual void data(cube_face f, uint8 * array_, uint32 size_) const;

	virtual void data(cube_face f, ui8_vector * array_) const;

	const uivec2 & size() const;

	void init();

	virtual void pup(nsfile_pupper * p);

	virtual void resize(uint32 w, uint32 h);
	
	void resize(const uivec2 & size);

	bool set_compressed(uint32 byteSize);

	/*
	  Set image data starting from the offset and going until offset + dimensions
	  If data is bound to PIXEL_UNPACK_BUFFER then data will be used as a byte offset in to
	  the store data
	  \param offset x and y offset in to the image
	  \param dimensions width and height of the sub image
	  \data Pixel data for the image
	*/
	bool set_data(const void * data, const uivec2 & offset, const uivec2 & dimensions);

	bool set_data(cube_face f, const void * data, const uivec2 & offset, const uivec2 & dimensions);

	/*
	  Copy pixels from currently bound read buffer to existing texture - does not allocate space
	  \param offset the offset in elements
	*/
	bool set_data_from_screen(const uivec2 & offset, const uivec2 & lowerLeft, const uivec2 & dimensions);

	bool set_data_from_screen(cube_face f, const uivec2 & offset, const uivec2 & lowerLeft, const uivec2 & dimensions);

  private:
	uivec2 m_size;
	uint32 m_comp_byte_size;
};

template <class PUPer>
void pup(PUPer & p, nstex_cubemap & sm)
{
	pup(p, sm.m_size, "size");
	pup(p, sm.m_comp_byte_size, "comp_byte_size");
	pup(p, static_cast<nstexture*>(&sm));
}


template <class PUPer>
void pup(PUPer & p, nstex_cubemap * sm)
{
	pup(p, *sm);
}

class nstex_rectangle : public nstex2d
{
  public:
	nstex_rectangle();
	~nstex_rectangle();
};

class nstex_cubemap_array : public nstex3d
{
  public:
	nstex_cubemap_array();
	~nstex_cubemap_array();
};

class nstex2d_multisample : public nstex2d
{
  public:
	nstex2d_multisample();
	~nstex2d_multisample();
};

class nstex2d_multisample_array : public nstex3d
{
  public:
	nstex2d_multisample_array();
	~nstex2d_multisample_array();
};

class nstex_buffer : public nstexture
{
  public:

	nstex_buffer();
	
	nstex_buffer(const nstex_buffer & copy_);

	~nstex_buffer();

	nstex_buffer & operator=(nstex_buffer rhs);

	bool allocate(const void * data);

	virtual image_data data() const;

	virtual void data(uint8 * array_, uint32 size_) const;

	virtual void data(ui8_vector * array_) const;

	void init();
};

#endif
