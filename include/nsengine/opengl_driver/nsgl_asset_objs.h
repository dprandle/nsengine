#ifndef NSGL_ASSET_OBJ_H
#define NSGL_ASSET_OBJ_H

#include <nsvideo_driver.h>

struct nsgl_shader;
struct nsgl_buffer;
struct nsgl_vao;
struct nsgl_xfb;
struct nsgl_texture;

struct nsgl_shader_ctxt_obj : public nsvid_ctxt_obj
{
	nsgl_shader * gl_shdr;
};

struct nsgl_texture_ctxt_obj : public nsvid_ctxt_obj
{
	nsgl_texture * gl_tex;
};

struct nsgl_submesh_ctxt_obj : public nsvid_ctxt_obj
{
	nsgl_vao * gl_vao;
	nsgl_buffer * gl_vert_buf;
	nsgl_buffer * gl_tex_buf;
	nsgl_buffer * gl_norm_buf;
	nsgl_buffer * gl_tang_buf;
	nsgl_buffer * gl_indice_buf;
	nsgl_buffer * gl_joint_buf;
};

struct nsgl_tform_comp_ctxt_obj : public nsvid_ctxt_obj
{
	nsgl_buffer * gl_tform_buffer;
	nsgl_buffer * gl_tform_id_buffer;
};

struct nsgl_sel_comp_ctxt_obj : public nsvid_ctxt_obj
{
	nsgl_buffer * gl_tform_buffer;
	nsgl_buffer * gl_tform_id_buffer;
};

struct nsgl_particle_comp_ctxt_obj : public nsvid_ctxt_obj
{
	uint8 buf_index;
	nsgl_xfb * gl_xfbs[2];
	nsgl_vao * gl_vaos[2];
	nsgl_buffer * gl_front_buffer;
	nsgl_buffer * gl_back_buffer;
};

#endif
