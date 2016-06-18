#ifndef NSGL_VID_OBJS_H
#define NSGL_VID_OBJS_H

#include <nsvideo_driver.h>

struct nsgl_shader;
struct nsgl_buffer;
struct nsgl_vao;
struct nsgl_xfb;
struct nsgl_texture;

class nsshader;
class nstexture;
#include <nsmesh.h>
struct tform_per_scene_info;
struct sel_per_scene_info;
struct nsparticle_comp;

struct nsgl_shader_obj : public nsvid_obj
{
	nsgl_shader_obj(nsvideo_object * parent_);
	
	~nsgl_shader_obj();
	
	void update();
	
	nsgl_shader * gl_shdr;
};

struct nsgl_texture_obj : public nsvid_obj
{
	nsgl_texture_obj(nsvideo_object * parent_);
	
	~nsgl_texture_obj();
	
	void update();
	
	nsgl_texture * gl_tex;
};

struct nsgl_submesh_obj : public nsvid_obj
{
	nsgl_submesh_obj(nsvideo_object * parent_);
	
	~nsgl_submesh_obj();
	
	void update();

	nsgl_vao * gl_vao;

	nsgl_buffer * gl_vert_buf;

	nsgl_buffer * gl_tex_buf;

	nsgl_buffer * gl_norm_buf;

	nsgl_buffer * gl_tang_buf;

	nsgl_buffer * gl_indice_buf;

	nsgl_buffer * gl_joint_buf;
};

struct nsgl_tform_comp_obj : public nsvid_obj
{
	nsgl_tform_comp_obj(nsvideo_object * parent_);
	
	~nsgl_tform_comp_obj();
	
	void update();

	nsgl_buffer * gl_tform_buffer;

	nsgl_buffer * gl_tform_id_buffer;
};

struct nsgl_sel_comp_obj : public nsvid_obj
{
	nsgl_sel_comp_obj(nsvideo_object * parent_);
	
	~nsgl_sel_comp_obj();
	
	void update();

	nsgl_buffer * gl_tform_buffer;
};

struct nsgl_particle_comp_obj : public nsvid_obj
{
	nsgl_particle_comp_obj(nsvideo_object * parent_);
	
	~nsgl_particle_comp_obj();
	
	void update();

	uint32 last_size;

	uint32 buffer_index;
	
	nsgl_xfb * gl_xfbs[2];

	nsgl_vao * gl_vaos[2];

	nsgl_buffer * gl_front_buffer;

	nsgl_buffer * gl_back_buffer;
};

#endif
