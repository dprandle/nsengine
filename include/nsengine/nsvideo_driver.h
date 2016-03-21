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

// shader types
class nsrender_shader;
class nsrender_target;
class nsmaterial_shader;
class nslight_stencil_shader;
class nsdir_light_shader;
class nspoint_light_shader;
class nsspot_light_shader;
class nsshadow_cubemap_shader;
class nsfragment_sort_shader;
class nsshadow_2dmap_shader;
class nsbuffer_object;
class nsselection_shader;
class nsparticle_render_shader;

struct render_shaders
{
	render_shaders();
	bool error();
	bool valid();
	
	nsmaterial_shader * deflt;
	nsmaterial_shader * deflt_wireframe;
	nsmaterial_shader * deflt_translucent;
	nsdir_light_shader * dir_light;
	nspoint_light_shader * point_light;
	nsspot_light_shader * spot_light;
	nslight_stencil_shader * light_stencil;
	nsshadow_cubemap_shader * shadow_cube;
	nsshadow_2dmap_shader * shadow_2d;
	nsfragment_sort_shader * frag_sort;
	nsparticle_render_shader * deflt_particle;
	nsselection_shader * sel_shader;
};

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

	void set_render_shaders(render_shaders rs);

	render_shaders & get_render_shaders();

	virtual void push_scene(nsscene * scn) = 0;

	virtual void render(nsrender::viewport * vp) = 0;

	virtual void init_texture(nstexture * tex) = 0;

	virtual void release_texture(nstexture * tex) = 0;
	
  protected:
	
	render_shaders m_shaders;
	bool m_initialized;
};

struct nsvid_obj
{
	nsvid_obj() {}
	virtual ~nsvid_obj() {}
	
	virtual void video_init() =0;

	virtual void video_release()=0;	
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
