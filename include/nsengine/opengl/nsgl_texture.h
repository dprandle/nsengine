/*! 
  \file nsgl_texture.h
	
  \brief Header file for nsgl_texture class

  This file contains all of the neccessary declarations for the nsgl_texture class.

  \author Daniel Randle
  \date November 23 2013
  \copywrite Earth Banana Games 2013
*/

// Internal Format : Bytes per pixel(ie 3 or 4)
// Format : enum specifying order of those byes - ie RGBA or RBGA etc
// Pixel Data Type : the data type assigned to each component of the pixe(ie unsigned byte)


#ifndef NSGL_TEXTURE_H
#define NSGL_TEXTURE_H

#define BASE_TEX_UNIT GL_TEXTURE0
#define BASE_CUBEMAP_FACE GL_TEXTURE_CUBE_MAP_POSITIVE_X

#include <asset/nstexture.h>
#include <opengl/nsgl_object.h>
#include <nsvideo_driver.h>

struct nsgl_texture : public nsgl_obj
{
	enum tex_target {
		tex_1d = GL_TEXTURE_1D,
		tex_1d_array = GL_TEXTURE_1D_ARRAY,
		tex_2d = GL_TEXTURE_2D,
		tex_2d_array = GL_TEXTURE_2D_ARRAY,
		tex_3d = GL_TEXTURE_3D,
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
		wrap_r = GL_TEXTURE_WRAP_R,
	    anistropic_filter = GL_TEXTURE_MAX_ANISOTROPY_EXT};

	enum get_tex_param 
	{
		texp_width = GL_TEXTURE_WIDTH, 
		texp_height = GL_TEXTURE_HEIGHT,
		texp_depth = GL_TEXTURE_DEPTH,
		texp_internal_format = GL_TEXTURE_INTERNAL_FORMAT, 
		texp_border = GL_TEXTURE_BORDER, 
		texp_size_red = GL_TEXTURE_RED_SIZE, 
		texp_size_green = GL_TEXTURE_GREEN_SIZE, 
		texp_size_blue = GL_TEXTURE_BLUE_SIZE, 
		texp_size_alpha = GL_TEXTURE_ALPHA_SIZE, 
		texp_size_luminance = GL_TEXTURE_LUMINANCE_SIZE,
		texp_size_intensity = GL_TEXTURE_INTENSITY_SIZE, 
		texp_size_depth = GL_TEXTURE_DEPTH_SIZE, 
		texp_compressed = GL_TEXTURE_COMPRESSED,
		texp_size_compressed = GL_TEXTURE_COMPRESSED_IMAGE_SIZE
	};

	nsgl_texture();
	~nsgl_texture();

	void init();

	void bind();

	bool allocate_1d(
		const void * data,
		tex_format format,
		pixel_component_type pt,
		int32 width,
		bool compress,
		uint32 compressed_size,
		uint32 layer=0);

	bool allocate_2d(
		const void * data,
		tex_format format,
		pixel_component_type pt,
		const ivec2 & size,
		bool compress,
		uint32 compressed_size,
		uint32 layer=0);

	bool allocate_3d(
		const void * data,
		tex_format format,
		pixel_component_type pt,
		const ivec3 & size,
		bool compress,
		uint32 compressed_size,
		uint32 layer=0);

	bool upload_1d(
		const void * data,
		tex_format format,
		pixel_component_type pt,
		int32 offset,
		int32 size,
		bool compress,
		uint32 compressed_size,
		uint32 layer=0);
	
	bool upload_2d(
		const void * data,
		tex_format format,
		pixel_component_type pt,
		const ivec2 & offset,
		const ivec2 & size,
		bool compress,
		uint32 compressed_size,
		uint32 layer=0);

	bool upload_3d(
		const void * data,
		tex_format format,
		pixel_component_type pt,
		const ivec3 & offset,
		const ivec3 & size,
		bool compress,
		uint32 compressed_size,
		uint32 layer=0);
	
	
	void download_data(uint8 * array_, tex_format format, pixel_component_type type, uint16 level);

	uint32 download_data_compressed(uint8 * array_, uint16 level);

	void set_parameters(tex_params texp);

	float parameter_f(get_tex_param p);

	int32 parameter_i(get_tex_param p);

	void generate_mipmaps();

	void release();

	void set_parameter_f(tex_parameter param, float pValue);

	void set_parameter_i(tex_parameter param, int32 pValue);

	void set_parameter_fv(tex_parameter param, const fvec4 & val);

	void set_parameter_iv(tex_parameter param, const ivec4 & val);

	void set_parameter_Iiv(tex_parameter param, const ivec4 & val);

	void set_parameter_Iuiv(tex_parameter param, const uivec4 & val);

	void set_pixel_data_type(int32 pType);

	uint32 map_tex_format(tex_format fmt);

	uint32 map_tex_pixel_data_type(pixel_component_type pt);

	uint32 map_tex_internal_format(tex_format fmt, pixel_component_type pt, bool compress);

	void unbind();

	int32 target;
};

#endif
