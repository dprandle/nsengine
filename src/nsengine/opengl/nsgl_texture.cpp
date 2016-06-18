/*! 
  \file nsgl_texture.h
	
  \brief Header file for nsgl_texture class

  This file contains all of the neccessary declarations for the nsgl_texture class.

  \author Daniel Randle
  \date November 23 2013
  \copywrite Earth Banana Games 2013
*/

#include <nsgl_texture.h>
#include <nsgl_driver.h>
#include <nsengine.h>

nsgl_texture::nsgl_texture():
	target(0)
{}

nsgl_texture::~nsgl_texture()
{}

void nsgl_texture::init()
{
	if (gl_id != 0)
	{
		dprint("nsgl_texture::init Error trying to initialize already initialized texture");
		return;
	}
	glGenTextures(1, &gl_id);
	gl_err_check("nsgl_texture::init");
}

void nsgl_texture::bind()
{
	glBindTexture(target, gl_id);
	gl_err_check("nsgl_texture::bind");
}

bool nsgl_texture::allocate_1d(
	const void * data,
	tex_format format,
	pixel_component_type pt,
	int32 width,
	bool compress,
	uint32 compressed_size,
	uint32 layer)
{
	if (compressed_size > 0)
	{
		glCompressedTexImage1D(
			target,
			layer,
			map_tex_internal_format(format, pt, true), 
			width,
			0,
			compressed_size,
			data);			
	}
	else
	{
		glTexImage1D(
			target,
			layer,
			map_tex_internal_format(format, pt, compress), 
			width,
			0,
			map_tex_format(format),
			map_tex_pixel_data_type(pt),
			data);
	}
	return gl_err_check("nsgl_texture::allocate_1d");	
}

bool nsgl_texture::allocate_2d(
	const void * data,
	tex_format format,
	pixel_component_type pt,
	const ivec2 & size,
	bool compress,
	uint32 compressed_size,
	uint32 layer)
{
	uint32 tget = target;
	if (target == tex_cubemap)
	{
		tget = BASE_CUBEMAP_FACE + layer;
		layer = 0;
	}
	if (compressed_size > 0)
	{
		glCompressedTexImage2D(
			tget,
			layer,
			map_tex_internal_format(format, pt, true), 
			size.x,
			size.y,
			0,
			compressed_size,
			data);			
	}
	else
	{
		glTexImage2D(
			tget,
			layer,
			map_tex_internal_format(format, pt, compress), 
			size.x,
			size.y,
			0,
			map_tex_format(format),
			map_tex_pixel_data_type(pt),
			data);
	}
	return gl_err_check("nsgl_texture::allocate_2d");
}

bool nsgl_texture::allocate_3d(
	const void * data,
	tex_format format,
	pixel_component_type pt,
	const ivec3 & size,
	bool compress,
	uint32 compressed_size,
	uint32 layer)
{
	
	if (compressed_size > 0)
	{
		glCompressedTexImage3D(
			target,
			layer,
			map_tex_internal_format(format, pt, true), 
			size.x,
			size.y,
			size.z,
			0,
			compressed_size,
			data);			
	}
	else
	{
		glTexImage3D(
			target,
			layer,
			map_tex_internal_format(format, pt, compress), 
			size.x,
			size.y,
			size.z,
			0,
			map_tex_format(format),
			map_tex_pixel_data_type(pt),
			data);
	}
	return gl_err_check("nsgl_texture::allocate_1d");		
}

bool nsgl_texture::upload_1d(
	const void * data,
	tex_format format,
	pixel_component_type pt,
	int32 offset,
	int32 size,
	bool compress,
	uint32 compressed_size,
	uint32 layer)
{
	
	if (compressed_size > 0)
	{
		glCompressedTexSubImage1D(
			target,
			0,
			offset,
			size,
			map_tex_internal_format(format, pt, true),
			compressed_size,
			data);
	}
	else
	{
		glTexSubImage1D(
			target,
			0,
			offset,
			size,
			map_tex_format(format),
			map_tex_pixel_data_type(pt),
			data);
	}
	return gl_err_check("nsgl_texture::allocate_1d");			
}
	
bool nsgl_texture::upload_2d(
	const void * data,
	tex_format format,
	pixel_component_type pt,
	const ivec2 & offset,
	const ivec2 & size,
	bool compress,
	uint32 compressed_size,
	uint32 layer)
{
	
	if (compressed_size > 0)
	{
		glCompressedTexSubImage2D(
			target,
			0,
			offset.x,
			offset.y,
			size.x,
			size.y,
			map_tex_internal_format(format, pt, true),
			compressed_size,
			data);
	}
	else
	{
		glTexSubImage2D(
			target,
			0,
			offset.x,
			offset.y,
			size.x,
			size.y,
			map_tex_format(format),
			map_tex_pixel_data_type(pt),
			data);
	}
	return gl_err_check("nsgl_texture::allocate_1d");			
}

bool nsgl_texture::upload_3d(
	const void * data,
	tex_format format,
	pixel_component_type pt,
	const ivec3 & offset,
	const ivec3 & size,
	bool compress,
	uint32 compressed_size,
	uint32 layer)
{
	
	if (compressed_size > 0)
	{
		glCompressedTexSubImage3D(
			target,
			0,
			offset.x,
			offset.y,
			offset.z,
			size.x,
			size.y,
			size.z,
			map_tex_internal_format(format, pt, true),
			compressed_size,
			data);
	}
	else
	{
		glTexSubImage3D(
			target,
			0,
			offset.x,
			offset.y,
			offset.z,
			size.x,
			size.y,
			size.z,
			map_tex_format(format),
			map_tex_pixel_data_type(pt),
			data);
	}
	return gl_err_check("nsgl_texture::allocate_1d");				
}
	
void nsgl_texture::download_data(uint8 * array_, tex_format format, pixel_component_type type, uint16 level)
{
	uint target = target;
	if (target == tex_cubemap)
	{
		target = BASE_CUBEMAP_FACE + level;
		level = 0;
	}
	glGetTexImage(
		target,
		level,
		map_tex_format(format),
		map_tex_pixel_data_type(type),
		array_);
	gl_err_check("nsgl_texture::download_data");
}

uint32 nsgl_texture::download_data_compressed(uint8 * array_, uint16 level)
{
	glGetCompressedTexImage(
		target,
		0,
		array_);
	gl_err_check("nsgl_texture::download_data");	
	return parameter_i(texp_size_compressed);
}

float nsgl_texture::parameter_f(get_tex_param p)
{
	GLfloat f;
	glGetTexParameterfv(target, p, &f);
	gl_err_check("nsgl_texture::parameter_f");
	return f;

}

int32 nsgl_texture::parameter_i(get_tex_param p)
{
	GLint i;
	glGetTexParameteriv(target, p, &i);
	gl_err_check("nsgl_texture::parameter_i");
	return i;	
}

void nsgl_texture::generate_mipmaps()
{
	glGenerateMipmap(target);
	gl_err_check("nsgl_texture::generate_mipmaps");
}

void nsgl_texture::release()
{
	glDeleteTextures(1,&gl_id);
	gl_id = 0;
	gl_err_check("nsgl_texture::release");	
}

void nsgl_texture::set_parameter_f(tex_parameter pParam, float pValue)
{
	glTexParameterf(target, pParam, pValue);
	gl_err_check("nsgl_texture::set_parameter_f");
}

void nsgl_texture::set_parameter_i(tex_parameter pParam, int32 pValue)
{
	glTexParameteri(target, pParam, pValue);
	gl_err_check("nsgl_texture::set_parameter_i");
}

void nsgl_texture::set_parameter_fv(tex_parameter param, const fvec4 & val)
{
	glTexParameterfv(target, param, &val[0]);
	gl_err_check("nsgl_texture::set_parameter_fv");
}

void nsgl_texture::set_parameter_iv(tex_parameter param, const ivec4 & val)
{
	glTexParameteriv(target, param, &val[0]);
	gl_err_check("nsgl_texture::set_parameter_iv");
}

void nsgl_texture::set_parameter_Iiv(tex_parameter param, const ivec4 & val)
{
	glTexParameterIiv(target, param, &val[0]);
	gl_err_check("nsgl_texture::set_parameter_Iiv");
}

void nsgl_texture::set_parameter_Iuiv(tex_parameter param, const uivec4 & val)
{
	glTexParameterIuiv(target, param, &val[0]);
	gl_err_check("nsgl_texture::set_parameter_Iiuv");
}

void nsgl_texture::set_parameters(tex_params texp)
{
	switch(texp.min_filter)
	{
	  case(tmin_nearest):
		  set_parameter_i(min_filter, GL_NEAREST);
		  break;
	  case(tmin_linear):
		  set_parameter_i(min_filter, GL_LINEAR);
		  break;
	  case(tmin_nearest_mipmap_nearest):
		  set_parameter_i(min_filter, GL_NEAREST_MIPMAP_NEAREST);
		  break;
	  case(tmin_nearest_mipmap_linear):
		  set_parameter_i(min_filter, GL_NEAREST_MIPMAP_LINEAR);
		  break;
	  case(tmin_linear_mipmap_nearest):
		  set_parameter_i(min_filter, GL_LINEAR_MIPMAP_NEAREST);
		  break;
	  case(tmin_linear_mipmap_linear):
		  set_parameter_i(min_filter, GL_LINEAR_MIPMAP_LINEAR);
		  break;
	  default:
		  dprint("nsgl_texture::set_parameters Invalid min filtering setting");
		  break;
	}

	if (texp.mag_filter == tmag_nearest)
	{
		set_parameter_i(mag_filter, GL_NEAREST);
	}
	else if (texp.mag_filter == tmag_linear)
	{
		set_parameter_i(mag_filter, GL_LINEAR);
	}
	else
	{
		dprint("nsgl_texture::set_parameters Invalid mag filtering setting");
	}

	switch(texp.edge_behavior.x)
	{
	  case(te_repeat):
		  set_parameter_i(wrap_s, GL_REPEAT);
		  break;
	  case(te_repeat_mirror):
		  set_parameter_i(wrap_s, GL_MIRRORED_REPEAT);
		  break;
	  case(te_clamp):
		  set_parameter_i(wrap_s, GL_CLAMP_TO_EDGE);
		  break;
	  case(te_clamp_mirror):
		  set_parameter_i(wrap_s, GL_MIRROR_CLAMP_TO_EDGE);
		  break;
	  default:
		  dprint("nsgl_texture::set_parameters Invalid edge behaviour setting for s");
	}

	switch(texp.edge_behavior.y)
	{
	  case(te_repeat):
		  set_parameter_i(wrap_t, GL_REPEAT);
		  break;
	  case(te_repeat_mirror):
		  set_parameter_i(wrap_t, GL_MIRRORED_REPEAT);
		  break;
	  case(te_clamp):
		  set_parameter_i(wrap_t, GL_CLAMP_TO_EDGE);
		  break;
	  case(te_clamp_mirror):
		  set_parameter_i(wrap_t, GL_MIRROR_CLAMP_TO_EDGE);
		  break;
	  default:
		  dprint("nsgl_texture::set_parameters Invalid edge behaviour setting for t");
	}
	
	switch(texp.edge_behavior.z)
	{
	  case(te_repeat):
		  set_parameter_i(wrap_r, GL_REPEAT);
		  break;
	  case(te_repeat_mirror):
		  set_parameter_i(wrap_r, GL_MIRRORED_REPEAT);
		  break;
	  case(te_clamp):
		  set_parameter_i(wrap_r, GL_CLAMP_TO_EDGE);
		  break;
	  case(te_clamp_mirror):
		  set_parameter_i(wrap_r, GL_MIRROR_CLAMP_TO_EDGE);
		  break;
	  default:
		  dprint("nsgl_texture::set_parameters Invalid edge behaviour setting for r");
	}

	if (texp.anistropic_filtering > 0.01f)
		set_parameter_f(anistropic_filter, texp.anistropic_filtering);

	if (texp.depth_mode == tex_dm_compare)
	{
		set_parameter_i(compare_mode, GL_COMPARE_REF_TO_TEXTURE);
		GLint comp_mode;
		switch (texp.depth_func)
		{
		  case(tex_dc_lequal):
			  comp_mode = GL_LEQUAL;
			  break;
		  case(tex_dc_gequal):
			  comp_mode = GL_GEQUAL;
			  break;
		  case(tex_dc_less):
			  comp_mode = GL_LESS;
			  break;
		  case(tex_dc_greater):
			  comp_mode = GL_GREATER;
			  break;
		  case(tex_dc_equal):
			  comp_mode = GL_EQUAL;
			  break;
		  case(tex_dc_not_equal):
			  comp_mode = GL_NOTEQUAL;
			  break;
		  case(tex_dc_always):
			  comp_mode = GL_ALWAYS;
			  break;
		  case(tex_dc_never):
			  comp_mode = GL_NEVER;
			  break;
		  default:
			  dprint("nsgl_texture::set_parameters Invalid depth compare mode for texture");			  
		}
		set_parameter_i(compare_func, comp_mode);
	}
}

void nsgl_texture::unbind()
{
	glBindTexture(target, 0);
	gl_err_check("nsgl_texture::unbind");
}

uint32 nsgl_texture::map_tex_format(tex_format fmt)
{
	switch (fmt)
	{
	  case(tex_red):
		  return GL_RED;
	  case(tex_rg):
		  return GL_RG;
	  case(tex_rgb):
		  return GL_RGB;
	  case(tex_bgr):
		  return GL_BGR;
	  case(tex_rgba):
		  return GL_RGBA;
	  case(tex_bgra):
		  return GL_BGRA;
	  case(tex_ired):
		  return GL_RED_INTEGER;
	  case(tex_irg):
		  return GL_RG_INTEGER;
	  case(tex_irgb):
		  return GL_RGB_INTEGER;
	  case(tex_ibgr):
		  return GL_BGR_INTEGER;
	  case(tex_irgba):
		  return GL_RGBA_INTEGER;
	  case(tex_ibgra):
		  return GL_BGRA_INTEGER;
	  case(tex_depth):
		  return GL_DEPTH_COMPONENT;
	  case(tex_depth_stencil):
		  return GL_DEPTH_STENCIL;
	  default:
		  return 0;
	}		
}

uint32 nsgl_texture::map_tex_pixel_data_type(pixel_component_type pt)
{
	switch (pt)
	{
	  case(tex_s8):
		  return GL_BYTE;
	  case(tex_u8):
		  return GL_UNSIGNED_BYTE;
	  case(tex_s16):
		  return GL_SHORT;
	  case(tex_u16):
		  return GL_UNSIGNED_SHORT;
	  case(tex_s32):
		  return GL_INT;
	  case(tex_u32):
		  return GL_UNSIGNED_INT;
	  case(tex_float):
		  return GL_FLOAT;
	}
}

uint32 nsgl_texture::map_tex_internal_format(tex_format fmt, pixel_component_type pt, bool compress)
{
	if (!compress)
	{
		if (fmt <= tex_red)
		{
			switch (pt)
			{
			  case(tex_s8):
				  return GL_R8_SNORM;
			  case(tex_u8):
				  return GL_R8;
			  case(tex_s16):
				  return GL_R16_SNORM;
			  case(tex_u16):
				  return GL_R16;
			  case(tex_s32):
				  return GL_R32F;
			  case(tex_u32):
				  return GL_R32F;
			  case(tex_float):
				  return GL_R32F;
			  default:
				  dprint("nsgl_texture::map_tex_internal_format Unknown type");
				  return 0;
			}
		}
		else if (fmt <= tex_ired)
		{
			switch (pt)
			{
			  case(tex_s8):
				  return GL_R8I;
			  case(tex_u8):
				  return GL_R8UI;
			  case(tex_s16):
				  return GL_R16I;
			  case(tex_u16):
				  return GL_R16UI;
			  case(tex_s32):
				  return GL_R32I;
			  case(tex_u32):
				  return GL_R32UI;
			  default:
				  dprint("nsgl_texture::map_tex_internal_format Cannot have float type with int format");
				  return 0;
			}
		}
		else if (fmt <= tex_rg)
		{
			switch (pt)
			{
			  case(tex_s8):
				  return GL_RG8_SNORM;
			  case(tex_u8):
				  return GL_RG8;
			  case(tex_s16):
				  return GL_RG16_SNORM;
			  case(tex_u16):
				  return GL_RG16;
			  case(tex_s32):
				  return GL_RG32F;
			  case(tex_u32):
				  return GL_RG32F;
			  case(tex_float):
				  return GL_RG32F;
			  default:
				  dprint("nsgl_texture::map_tex_internal_format Unknown type");
				  return 0;
			}
		}
		else if (fmt <= tex_irg)
		{
			switch (pt)
			{
			  case(tex_s8):
				  return GL_RG8I;
			  case(tex_u8):
				  return GL_RG8UI;
			  case(tex_s16):
				  return GL_RG16I;
			  case(tex_u16):
				  return GL_RG16UI;
			  case(tex_s32):
				  return GL_RG32I;
			  case(tex_u32):
				  return GL_RG32UI;
			  default:
				  dprint("nsgl_texture::map_tex_internal_format Cannot have float type with int format");
				  return 0;
			}
		}
		else if (fmt <= tex_bgr)
		{
			switch (pt)
			{
			  case(tex_s8):
				  return GL_RGB8_SNORM;
			  case(tex_u8):
				  return GL_RGB8;
			  case(tex_s16):
				  return GL_RGB16_SNORM;
			  case(tex_u16):
				  return GL_RGB16;
			  case(tex_s32):
				  return GL_RGB32F;
			  case(tex_u32):
				  return GL_RGB32F;
			  case(tex_float):
				  return GL_RGB32F;
			  default:
				  dprint("nsgl_texture::map_tex_internal_format Unknown type");
				  return 0;
			}
		}
		else if (fmt <= tex_ibgr)
		{
			switch (pt)
			{
			  case(tex_s8):
				  return GL_RGB8I;
			  case(tex_u8):
				  return GL_RGB8UI;
			  case(tex_s16):
				  return GL_RGB16I;
			  case(tex_u16):
				  return GL_RGB16UI;
			  case(tex_s32):
				  return GL_RGB32I;
			  case(tex_u32):
				  return GL_RGB32UI;
			  default:
				  dprint("nsgl_texture::map_tex_internal_format Cannot have float type with int format");
				  return 0;
			}
		}
		else if (fmt <= tex_bgra)
		{
			switch (pt)
			{
			  case(tex_s8):
				  return GL_RGBA8_SNORM;
			  case(tex_u8):
				  return GL_RGBA8;
			  case(tex_s16):
				  return GL_RGBA16_SNORM;
			  case(tex_u16):
				  return GL_RGBA16;
			  case(tex_s32):
				  return GL_RGBA32F;
			  case(tex_u32):
				  return GL_RGBA32F;
			  case(tex_float):
				  return GL_RGBA32F;
			  default:
				  dprint("nsgl_texture::map_tex_internal_format Unknown type");
				  return 0;
			}			
		}
		else if (fmt <= tex_ibgra)
		{
			switch (pt)
			{
			  case(tex_s8):
				  return GL_RGBA8I;
			  case(tex_u8):
				  return GL_RGBA8UI;
			  case(tex_s16):
				  return GL_RGBA16I;
			  case(tex_u16):
				  return GL_RGBA16UI;
			  case(tex_s32):
				  return GL_RGBA32I;
			  case(tex_u32):
				  return GL_RGBA32UI;
			  default:
				  dprint("nsgl_texture::map_tex_internal_format Cannot have float type with int format");
				  return 0;
			}
		}
		else if (fmt == tex_depth)
			return GL_DEPTH_COMPONENT;
		else if (fmt == tex_depth_stencil)
			return GL_DEPTH_STENCIL;
		else
			return 0;
	}
	else
	{
		if (fmt <= tex_ired)
			return GL_COMPRESSED_RED_RGTC1;
		else if (fmt <= tex_irg)
			return GL_COMPRESSED_RG_RGTC2;
		else if (fmt <= tex_ibgr)
			return GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT;
		else if (fmt <= tex_ibgra)
			return GL_COMPRESSED_RGBA_BPTC_UNORM;
		else
			return 0;		
	}
}
