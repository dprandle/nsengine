/*!
  \file nsgl_render_passes.h

  \brief Definition file for nsgl_render_passes class

  This file contains all of the neccessary definitions for the nsgl_render_passes class.

  \author Daniel Randle
  \date Feb 23 2016
  \copywrite Earth Banana Games 2013
*/

#ifndef NSGL_RENDER_PASSES_H
#define NSGL_RENDER_PASSES_H

#include <nsfile_os.h>
#include <nsmath.h>
#include <nsvideo_driver.h>

struct nsgl_driver;
struct nsgl_framebuffer;
struct translucent_buffers;
struct light_draw_call;

struct opengl_state
{
	opengl_state():
		depth_write(false),
		depth_test(false),
		stencil_test(false),
		blending(false),
		culling(false),
		cull_face(0),
		clear_mask(0),
		blend_func(),
		blend_eqn(0),
		stencil_func(),
		stencil_op_back(),
		stencil_op_front()
	{}
	
	bool depth_write; // material
	bool depth_test; // material
	bool stencil_test; // material
	bool blending; // renderer
	bool culling; // material
	int32 cull_face; // material
	int32 clear_mask;
	ivec2 blend_func; // renderer
	int32 blend_eqn;
	ivec3 stencil_func; // submesh
	ivec3 stencil_op_back; // submesh
	ivec3 stencil_op_front;
	ivec4 current_viewport;
};

struct gl_render_pass : public render_pass
{
	gl_render_pass():
		render_pass(),
		ren_target(nullptr),
		use_vp_size(true)
	{}

	virtual ~gl_render_pass() {}

	virtual void setup();

	virtual void render() = 0;

	virtual void finish() {}

	bool use_vp_size;
	nsgl_framebuffer * ren_target;
	nsgl_driver * driver;
	opengl_state gl_state;
};

struct gbuffer_render_pass : public gl_render_pass
{
	virtual void render();
};

#ifdef ORDER_INDEPENDENT_TRANSLUCENCY
struct oit_render_pass : public gl_render_pass
{
	virtual void render();
	translucent_buffers * tbuffers;
};
#endif

struct sorted_translucency_render_pass : public gl_render_pass
{
	virtual void render() {}
};

struct light_shadow_pass : public gl_render_pass
{
	virtual void render();
	light_draw_call * ldc;
};

struct light_pass : public gl_render_pass
{
	virtual void render();
	light_shadow_pass * rpass;
	translucent_buffers * tbuffers;
};

struct culled_light_pass : public light_pass
{
	virtual void render();
};

struct final_render_pass : public gl_render_pass
{
	virtual void render();
	nsgl_framebuffer * read_buffer;
};

struct selection_render_pass : public gl_render_pass
{
	virtual void render();
};

struct ui_render_pass : public gl_render_pass
{
	virtual void render();
};

#endif
