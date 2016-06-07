/*!
  \file nsvideo_driver.h

  \brief Definition file for nsvideo_driver class

  This file contains all of the neccessary definitions for the nsvideo_driver class.

  \author Daniel Randle
  \date Feb 23 2016
  \copywrite Earth Banana Games 2013
*/

#ifndef NSVIDEO_DRIVER
#define NSVIDEO_DRIVER

#include <nsrender_system.h>
#include <nsstring.h>
#include <nsmath.h>
#include <nsunordered_map.h>
#include <nsmesh.h>
#include <nstexture.h>

class nsvideo_driver
{
  public:

	nsvideo_driver();
	
	virtual ~nsvideo_driver();

	virtual void setup_default_rendering() = 0;

	virtual uivec3 pick(const fvec2 & mouse_pos) = 0;
	
	virtual void init(bool setup_default_rend);

	bool initialized();

	virtual void release();

	virtual void resize_screen(const ivec2 & new_size) = 0;

	virtual void render(nsrender::viewport * vp) = 0;

	virtual void init_texture(nstexture * tex) = 0;

	virtual void release_texture(nstexture * tex) = 0;
	
  protected:
	
	bool m_initialized;
};

struct nsvid_obj
{
	nsvid_obj() {}
	virtual ~nsvid_obj() {}
	
	virtual void video_init() =0;

	virtual void video_release()=0;	
};

struct nsvid_buffer : public nsvid_obj
{
	virtual void allocate(void * data, uint32 data_size);
};

struct nsvid_texture : public nsvid_obj
{
	nsvid_texture():nsvid_obj() {}
	virtual ~nsvid_texture() {}

	virtual void bind() const =0;

	virtual void unbind() const =0;

	virtual	bool allocate_1d(
		const void * data,
		tex_format format,
		pixel_component_type pt,
		int32 width,
		bool compress,
		uint32 compressed_size,
		uint32 layer=0) = 0;

	virtual	bool allocate_2d(
		const void * data,
		tex_format format,
		pixel_component_type pt,
		const ivec2 & size,
		bool compress,
		uint32 compressed_size,
		uint32 layer=0) = 0;

	virtual	bool allocate_3d(
		const void * data,
		tex_format format,
		pixel_component_type pt,
		const ivec3 & size,
		bool compress,
		uint32 compressed_size,
		uint32 layer=0) = 0;

	virtual	bool upload_1d(
		const void * data,
		tex_format format,
		pixel_component_type pt,
		int32 offset,
		int32 size,
		bool compress,
		uint32 compressed_size,
		uint32 layer=0) = 0;
	
	virtual	bool upload_2d(
		const void * data,
		tex_format format,
		pixel_component_type pt,
		const ivec2 & offset,
		const ivec2 & size,
		bool compress,
		uint32 compressed_size,
		uint32 layer=0) = 0;

	virtual	bool upload_3d(
		const void * data,
		tex_format format,
		pixel_component_type pt,
		const ivec3 & offset,
		const ivec3 & size,
		bool compress,
		uint32 compressed_size,
		uint32 layer=0) = 0;

	virtual void set_parameters(tex_params texp) = 0;
	
	virtual void download_data(uint8 * array_, tex_format format, pixel_component_type type, uint16 level) =0;

	virtual uint32 download_data_compressed(uint8 * array_, uint16 level)=0;

	virtual void generate_mipmaps()=0;

};

#endif
