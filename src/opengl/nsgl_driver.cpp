/*!
  \file nsopengl_driver.cpp

  \brief Definition file for nsopengl_driver class

  This file contains all of the neccessary definitions for the nsopengl_driver class.

  \author Daniel Randle
  \date Feb 23 2016
  \copywrite Earth Banana Games 2013
*/

#include <opengl/nsgl_vid_objs.h>
#include <opengl/nsgl_render_passes.h>
#include <opengl/nsgl_draw_calls.h>
#include <opengl/nsgl_driver.h>
#include <opengl/nsgl_driver.h>
#include <opengl/nsgl_shadow_framebuffer.h>
#include <opengl/nsgl_gbuffer.h>
#include <opengl/nsgl_texture.h>
#include <opengl/nsgl_buffer.h>
#include <opengl/nsgl_shader.h>
#include <opengl/nsgl_vao.h>

#include <component/nssprite_comp.h>
#include <system/nsselection_system.h>
#include <nsworld_data.h>
#include <asset/nsmaterial.h>
#include <asset/nsshader.h>
#include <component/nsui_button_comp.h>
#include <component/nsui_canvas_comp.h>
#include <asset/nsmesh.h>
#include <component/nsui_comp.h>
#include <asset/nsfont.h>
#include <nsworld_data.h>
#include <component/nscam_comp.h>
#include <component/nslight_comp.h>
#include <component/nsrender_comp.h>
#include <component/nsanim_comp.h>
#include <component/nsterrain_comp.h>
#include <component/nssel_comp.h>
#include <asset/nsplugin.h>
#include <nsengine.h>
#include <system/nsui_system.h>
#include <nstimer.h>

render_shaders::render_shaders() :
	deflt(nullptr),
	deflt_wireframe(nullptr),
	deflt_translucent(nullptr),
	deflt_instanced(nullptr),
	deflt_instanced_wireframe(nullptr),
	deflt_instanced_translucent(nullptr),
	dir_light(nullptr),
	point_light(nullptr),
	spot_light(nullptr),
	light_stencil(nullptr),
	shadow_cube(nullptr),
	shadow_2d(nullptr),
	shadow_cube_instanced(nullptr),
	shadow_2d_instanced(nullptr),
	frag_sort(nullptr),
	deflt_particle(nullptr),
	sel_shader(nullptr)
{}

bool render_shaders::error() const
{
	return (
		deflt->video_obj<nsgl_shader_obj>()->gl_shdr->error_state != nsgl_shader::error_none ||
		deflt_wireframe->video_obj<nsgl_shader_obj>()->gl_shdr->error_state != nsgl_shader::error_none ||
		deflt_instanced->video_obj<nsgl_shader_obj>()->gl_shdr->error_state != nsgl_shader::error_none ||
		deflt_instanced_wireframe->video_obj<nsgl_shader_obj>()->gl_shdr->error_state != nsgl_shader::error_none ||
		dir_light->video_obj<nsgl_shader_obj>()->gl_shdr->error_state != nsgl_shader::error_none ||
		point_light->video_obj<nsgl_shader_obj>()->gl_shdr->error_state != nsgl_shader::error_none ||
		spot_light->video_obj<nsgl_shader_obj>()->gl_shdr->error_state != nsgl_shader::error_none ||
		light_stencil->video_obj<nsgl_shader_obj>()->gl_shdr->error_state != nsgl_shader::error_none ||
		shadow_cube->video_obj<nsgl_shader_obj>()->gl_shdr->error_state != nsgl_shader::error_none ||
		shadow_2d->video_obj<nsgl_shader_obj>()->gl_shdr->error_state != nsgl_shader::error_none ||
		shadow_cube_instanced->video_obj<nsgl_shader_obj>()->gl_shdr->error_state != nsgl_shader::error_none ||
		shadow_2d_instanced->video_obj<nsgl_shader_obj>()->gl_shdr->error_state != nsgl_shader::error_none ||
#ifdef ORDER_INDEPENDENT_TRANSLUCENCY
		deflt_translucent->video_obj<nsgl_shader_obj>()->gl_shdr->error_state != nsgl_shader::error_none ||
		deflt_instanced_translucent->video_obj<nsgl_shader_obj>()->gl_shdr->error_state != nsgl_shader::error_none ||
		frag_sort->video_obj<nsgl_shader_obj>()->gl_shdr->error_state != nsgl_shader::error_none ||
#endif
		deflt_particle->video_obj<nsgl_shader_obj>()->gl_shdr->error_state != nsgl_shader::error_none ||
		sel_shader->video_obj<nsgl_shader_obj>()->gl_shdr->error_state != nsgl_shader::error_none);
}

bool render_shaders::valid() const
{
	return (
		deflt != nullptr &&
		deflt_wireframe != nullptr &&
		deflt_instanced != nullptr &&
		deflt_instanced_wireframe != nullptr &&
		dir_light != nullptr &&
		point_light != nullptr &&
		spot_light != nullptr &&
		light_stencil != nullptr &&
		shadow_cube != nullptr &&
		shadow_2d != nullptr &&
		shadow_cube_instanced != nullptr &&
		shadow_2d_instanced != nullptr &&
#ifdef ORDER_INDEPENDENT_TRANSLUCENCY
		deflt_translucent != nullptr &&
		deflt_instanced_translucent != nullptr &&
		frag_sort != nullptr &&
#endif
		deflt_particle != nullptr &&
		sel_shader != nullptr);
}

#ifdef ORDER_INDEPENDENT_TRANSLUCENCY
translucent_buffers::translucent_buffers():
	atomic_counter(new nsgl_buffer()),
	header(new nsgl_buffer()),
	fragments(new nsgl_buffer()),
	header_clr_data(DEFAULT_ACCUM_BUFFER_RES_X*DEFAULT_ACCUM_BUFFER_RES_Y, -1)
{}

void translucent_buffers::init()
{
	atomic_counter->target = nsgl_buffer::atomic_counter;
	atomic_counter->target_index = 0;
	atomic_counter->init();
	header->target = nsgl_buffer::shader_storage;
	header->target_index = 0;
	header->init();
	fragments->target = nsgl_buffer::shader_storage;
	fragments->target_index = 1;
	fragments->init();	
}

void translucent_buffers::release()
{
	atomic_counter->release();
	header->release();
	fragments->release();	
}

translucent_buffers::~translucent_buffers()
{
	delete atomic_counter;
	delete header;
	delete fragments;
}

void translucent_buffers::bind_buffers()
{
	atomic_counter->bind();
	header->bind();
	fragments->bind();
}

void translucent_buffers::unbind_buffers()
{
	atomic_counter->unbind();
	header->unbind();	
}

void translucent_buffers::reset_atomic_counter()
{
	uint32 data = 0;
	atomic_counter->bind();
	uint32 * ptr = atomic_counter->map_range<uint32>(0, 1, nsgl_buffer::map_write);
	ptr[0] = 0;
	atomic_counter->unmap();
	atomic_counter->unbind();
}
#endif

GLEWContext * glewGetContext()
{
	nsgl_driver * driver = static_cast<nsgl_driver*>(nse.video_driver());
	return driver->current_context()->glew_context;
}

gl_ctxt::gl_ctxt(uint32 id) :
	vid_ctxt(id),
	glew_context(new GLEWContext()),
#ifdef ORDER_INDEPENDENT_TRANSLUCENCY
	m_tbuffers(new translucent_buffers()),
#endif
	m_default_target(new nsgl_framebuffer()),
	m_single_point(new nsgl_buffer()),
	driver(nullptr)
{}

gl_ctxt::~gl_ctxt()
{
	while (render_targets.begin() != render_targets.end())
	{
		delete render_targets.begin()->second;
		render_targets.erase(render_targets.begin());
	}
	delete m_single_point;	
	delete m_default_target;
	#ifdef ORDER_INDEPENDENT_TRANSLUCENCY
	delete m_tbuffers;
	#endif
	delete glew_context;
}

void gl_ctxt::init()
{
    glewExperimental = GL_TRUE;

    // Initialize the glew extensions - if this fails we want a crash because there is nothing
	// useful the program can do without these initialized
	GLenum cont = glewInit();
	if (cont != GLEW_OK)
	{
		dprint("GLEW extensions unable to initialize");
		return;
	}
	initialized = true;

	// GL default setup
    gl_err_check("GL_INIT Normal Enum Wierdness");

	glFrontFace(GL_CW);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(false);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);

	// Create the default framebuffer
	m_default_target->gl_id = 0;

	#ifdef ORDER_INDEPENDENT_TRANSLUCENCY
	m_tbuffers->init();
	uint32 data_ = 0;
	uint32 sz = DEFAULT_ACCUM_BUFFER_RES_X*DEFAULT_ACCUM_BUFFER_RES_Y;
	i_vector header_data(sz, -1);

	m_tbuffers->atomic_counter->bind();
	m_tbuffers->atomic_counter->allocate(1, &data_, nsgl_buffer::mutable_dynamic_draw);
	m_tbuffers->atomic_counter->unbind();
	m_tbuffers->header->bind();
	m_tbuffers->header->allocate(header_data, nsgl_buffer::mutable_dynamic_draw);
	m_tbuffers->fragments->bind();
	m_tbuffers->fragments->allocate(
		sz*2, sizeof(packed_fragment_data), nullptr, nsgl_buffer::mutable_dynamic_draw);
	m_tbuffers->fragments->unbind();
	#endif
	
	fvec3 point;
    // Get single point from render system and initialize it
    m_single_point->target = nsgl_buffer::array;
    m_single_point->init();
	m_single_point->bind();
	m_single_point->allocate(1, &point, nsgl_buffer::mutable_dynamic_draw);
	m_single_point->unbind();

	all_single_draw_calls.reserve(MAX_SINGLE_DRAW_CALLS);
	all_light_draw_calls.reserve(MAX_LIGHTS_IN_SCENE);
	all_ui_draw_calls.reserve(MAX_UI_DRAW_CALLS);	
}

void gl_ctxt::release()
{
	auto iter = render_targets.begin();
	while (iter != render_targets.end())
	{
		iter->second->release();
		++iter;
	}
	m_single_point->release();
	m_default_target->release();
#ifdef ORDER_INDEPENDENT_TRANSLUCENCY
	m_tbuffers->release();
#endif
	vid_ctxt::release();
}

uivec3 gl_ctxt::pick(const fvec2 & mouse_pos)
{
	nsgl_framebuffer * pck = (nsgl_framebuffer*)render_target(ACCUM_TARGET);
	if (pck == NULL)
		return uivec3();

	ivec2 winsize = window_size();
	uivec3 index = pck->pick(mouse_pos.x * winsize.x, mouse_pos.y * winsize.y, 1);
	return index;
}

void gl_ctxt::create_default_render_targets()
{
	// Create all of the framebuffers
	nsgl_gbuffer * gbuffer = new nsgl_gbuffer;
    gbuffer->resize(DEFAULT_GBUFFER_RES_X, DEFAULT_GBUFFER_RES_Y);
	gbuffer->init();
	add_render_target(GBUFFER_TARGET, gbuffer);

	tex_params tp;
	tp.min_filter = tmin_linear;
	tp.mag_filter = tmag_linear;
	
	// Accumulation buffer
	nsgl_framebuffer * accum_buffer = new nsgl_framebuffer;
	accum_buffer->resize(DEFAULT_ACCUM_BUFFER_RES_X, DEFAULT_ACCUM_BUFFER_RES_Y);
	accum_buffer->target = nsgl_framebuffer::fb_draw;
    accum_buffer->init();
	accum_buffer->bind();
	accum_buffer->create_texture_attachment<nstex2d>(
		"rendered_frame",
		nsgl_framebuffer::attach_point(nsgl_framebuffer::att_color),
		FINAL_TEX_UNIT,
		tex_rgba,
		tex_float,
		tp);
	accum_buffer->add(gbuffer->depth());
	tp.min_filter = tmin_nearest;
	tp.mag_filter = tmag_nearest;
	accum_buffer->create_texture_attachment<nstex2d>(
		"final_picking",
		nsgl_framebuffer::attach_point(nsgl_framebuffer::att_color + nsgl_gbuffer::col_picking),
		G_PICKING_TEX_UNIT,
		tex_irgb,
		tex_u32,
		tp);
	accum_buffer->update_draw_buffers();
	add_render_target(ACCUM_TARGET, accum_buffer);

	// 2d shadow map targets
	nsshadow_tex2d_target * dir_shadow_target = new nsshadow_tex2d_target;
	dir_shadow_target->resize(DEFAULT_DIR_LIGHT_SHADOW_W, DEFAULT_DIR_LIGHT_SHADOW_H);
	dir_shadow_target->init("direction_light_shadow");
	add_render_target(DIR_SHADOW2D_TARGET, dir_shadow_target);
	
	nsshadow_tex2d_target * spot_shadow_target = new nsshadow_tex2d_target;
	spot_shadow_target->resize(DEFAULT_SPOT_LIGHT_SHADOW_W, DEFAULT_SPOT_LIGHT_SHADOW_H);
	spot_shadow_target->init("spot_light_shadow");
	add_render_target(SPOT_SHADOW2D_TARGET, spot_shadow_target);
	
	nsshadow_tex_cubemap_target * point_shadow_target = new nsshadow_tex_cubemap_target;
	point_shadow_target->resize(DEFAULT_POINT_LIGHT_SHADOW_W, DEFAULT_POINT_LIGHT_SHADOW_H);
	point_shadow_target->init("point_light_shadow");
	add_render_target(POINT_SHADOW_TARGET, point_shadow_target);	
}

void gl_ctxt::create_default_render_queues()
{
	create_queue(SCENE_OPAQUE_QUEUE)->reserve(MAX_GBUFFER_DRAWS);
	create_queue(SCENE_TRANSLUCENT_QUEUE)->reserve(MAX_GBUFFER_DRAWS);
	create_queue(SCENE_SELECTION_QUEUE)->reserve(MAX_GBUFFER_DRAWS);
	create_queue(DIR_LIGHT_QUEUE)->reserve(MAX_LIGHTS_IN_SCENE);
	create_queue(SPOT_LIGHT_QUEUE)->reserve(MAX_LIGHTS_IN_SCENE);
	create_queue(POINT_LIGHT_QUEUE)->reserve(MAX_LIGHTS_IN_SCENE);
	create_queue(UI_RENDER_QUEUE)->reserve(MAX_UI_DRAW_CALLS);
}

void gl_ctxt::setup_default_rendering()
{
	create_default_render_queues();
	create_default_render_targets();
	create_default_render_passes();
}

void gl_ctxt::destroy_all_render_targets()
{
	while (render_targets.begin() != render_targets.end())
		destroy_render_target(render_targets.begin()->first);
	render_targets.clear();
}

bool gl_ctxt::add_render_target(const nsstring & name, nsgl_framebuffer * rt)
{
	return render_targets.emplace(name, rt).second;
}

nsgl_framebuffer * gl_ctxt::remove_render_target(const nsstring & name)
{
	nsgl_framebuffer * fb = nullptr;
	auto iter = render_targets.find(name);
	if (iter != render_targets.end())
	{
		fb = iter->second;
		render_targets.erase(iter);
	}
	return fb;
}

nsgl_framebuffer * gl_ctxt::render_target(const nsstring & name)
{
	auto iter = render_targets.find(name);
	if (iter != render_targets.end())
		return iter->second;
	return nullptr;
}

void gl_ctxt::destroy_render_target(const nsstring & name)
{
	nsgl_framebuffer * rt = remove_render_target(name);
	rt->release();
	delete rt;
}

void gl_ctxt::create_default_render_passes()
{
	// setup gbuffer geometry stage
	gbuffer_render_pass * gbuf_pass = new gbuffer_render_pass;
	gbuf_pass->rq = queue(SCENE_OPAQUE_QUEUE);
	gbuf_pass->ren_target = render_target(GBUFFER_TARGET);
	gbuf_pass->gl_state.depth_test = true;
	gbuf_pass->gl_state.depth_write = true;
	gbuf_pass->gl_state.culling = true;
	gbuf_pass->gl_state.cull_face = GL_BACK;
	gbuf_pass->gl_state.blending = false;
	gbuf_pass->gl_state.stencil_test = false;
	gbuf_pass->driver_ctxt = this;
	gbuf_pass->gl_state.clear_mask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;

#ifdef ORDER_INDEPENDENT_TRANSLUCENCY
	oit_render_pass * oit_pass = new oit_render_pass;
	oit_pass->tbuffers = m_tbuffers;
	oit_pass->rq = queue(SCENE_TRANSLUCENT_QUEUE);
	oit_pass->ren_target = render_target(ACCUM_TARGET);
	oit_pass->gl_state.depth_test = true;
	oit_pass->gl_state.depth_write = false;
	oit_pass->gl_state.culling = true;
	oit_pass->gl_state.cull_face = GL_BACK;
	oit_pass->gl_state.blending = false;
	oit_pass->gl_state.stencil_test = false;
	oit_pass->driver_ctxt = this;
#else
	sorted_translucency_render_pass * st_pass = new sorted_translucency_render_pass;
	st_pass->rq = queue(SCENE_TRANSLUCENT_QUEUE);
	st_pass->ren_target = render_target(ACCUM_TARGET);
	st_pass->gl_state.depth_test = true;
	st_pass->gl_state.depth_write = false;
	st_pass->gl_state.culling = false;
	st_pass->gl_state.blending = true;
	st_pass->gl_state.stencil_test = false;
	st_pass->driver_ctxt = this;	
#endif

	// setup dir light shadow stage
	light_shadow_pass * dir_shadow_pass = new light_shadow_pass;
	dir_shadow_pass->rq = queue(SCENE_OPAQUE_QUEUE);
	dir_shadow_pass->ren_target = render_target(DIR_SHADOW2D_TARGET);
	dir_shadow_pass->gl_state.depth_test = true;
	dir_shadow_pass->gl_state.depth_write = true;
	dir_shadow_pass->gl_state.culling = true;
	dir_shadow_pass->gl_state.cull_face = GL_BACK;
	dir_shadow_pass->gl_state.blending = false;
	dir_shadow_pass->gl_state.stencil_test = false;
	dir_shadow_pass->gl_state.clear_mask = GL_DEPTH_BUFFER_BIT;
	dir_shadow_pass->enabled = false;
	dir_shadow_pass->driver_ctxt = this;

	// setup dir light stage
	light_pass * dir_pass = new light_pass;
	dir_pass->rq = queue(DIR_LIGHT_QUEUE);
	dir_pass->ren_target = render_target(ACCUM_TARGET);
	dir_pass->gl_state.depth_test = false;
	dir_pass->gl_state.depth_write = false;
	dir_pass->gl_state.culling = true;
	dir_pass->gl_state.cull_face = GL_BACK;
	dir_pass->gl_state.blending = false;
	dir_pass->gl_state.stencil_test = false;
#ifdef ORDER_INDEPENDENT_TRANSLUCENCY
	dir_pass->tbuffers = m_tbuffers;
#endif
	dir_pass->rpass = dir_shadow_pass;
	dir_pass->driver_ctxt = this;
	
	light_shadow_pass * spot_shadow_pass = new light_shadow_pass;
	spot_shadow_pass->rq = queue(SCENE_OPAQUE_QUEUE);
	spot_shadow_pass->ren_target = render_target(SPOT_SHADOW2D_TARGET);
	spot_shadow_pass->enabled = false;
	spot_shadow_pass->gl_state.depth_test = true;
	spot_shadow_pass->gl_state.depth_write = true;
	spot_shadow_pass->gl_state.culling = true;
	spot_shadow_pass->gl_state.cull_face = GL_BACK;
	spot_shadow_pass->gl_state.blending = false;
	spot_shadow_pass->gl_state.stencil_test = false;
	spot_shadow_pass->gl_state.clear_mask = GL_DEPTH_BUFFER_BIT;
	spot_shadow_pass->driver_ctxt = this;

	culled_light_pass * spot_pass = new culled_light_pass;
	spot_pass->rq = queue(SPOT_LIGHT_QUEUE);
	spot_pass->ren_target = render_target(ACCUM_TARGET);
#ifdef ORDER_INDEPENDENT_TRANSLUCENCY
	spot_pass->tbuffers = m_tbuffers;
#endif
	spot_pass->rpass = spot_shadow_pass;
	spot_pass->gl_state.depth_test = true;
	spot_pass->gl_state.depth_write = false;
	spot_pass->gl_state.culling = false;
	spot_pass->gl_state.blending = true;
	spot_pass->gl_state.blend_eqn = GL_FUNC_ADD;
	spot_pass->gl_state.blend_func = ivec2(GL_ONE, GL_ONE);
	spot_pass->gl_state.stencil_test = true;
	spot_pass->gl_state.stencil_func = ivec3(GL_ALWAYS, 0, 0);
	spot_pass->gl_state.stencil_op_back = ivec3(GL_KEEP, GL_INCR_WRAP, GL_KEEP);
	spot_pass->gl_state.stencil_op_front = ivec3(GL_KEEP, GL_DECR_WRAP, GL_KEEP);
	spot_pass->gl_state.clear_mask = GL_STENCIL_BUFFER_BIT;
	spot_pass->driver_ctxt = this;

	light_shadow_pass * point_shadow_pass = new light_shadow_pass;
	point_shadow_pass->rq = queue(SCENE_OPAQUE_QUEUE);
	point_shadow_pass->ren_target = render_target(POINT_SHADOW_TARGET);
	point_shadow_pass->gl_state.depth_test = true;
	point_shadow_pass->gl_state.depth_write = true;
	point_shadow_pass->gl_state.culling = true;
	point_shadow_pass->gl_state.cull_face = GL_BACK;
	point_shadow_pass->gl_state.blending = false;
	point_shadow_pass->gl_state.stencil_test = false;
	point_shadow_pass->gl_state.clear_mask = GL_DEPTH_BUFFER_BIT;
	point_shadow_pass->enabled = false;
	point_shadow_pass->driver_ctxt = this;

	culled_light_pass * point_pass = new culled_light_pass;
	point_pass->rq = queue(POINT_LIGHT_QUEUE);
	point_pass->ren_target = render_target(ACCUM_TARGET);
#ifdef ORDER_INDEPENDENT_TRANSLUCENCY
	point_pass->tbuffers = m_tbuffers;
#endif
	point_pass->rpass = point_shadow_pass;
	point_pass->gl_state.depth_test = true;
	point_pass->gl_state.depth_write = false;
	point_pass->gl_state.culling = false;
	point_pass->gl_state.blending = true;
	point_pass->gl_state.blend_func = ivec2(GL_ONE, GL_ONE);
	point_pass->gl_state.stencil_test = true;
	point_pass->gl_state.stencil_func = ivec3(GL_ALWAYS, 0, 0);
	point_pass->gl_state.blend_eqn = GL_FUNC_ADD;
	point_pass->gl_state.stencil_op_back = ivec3(GL_KEEP, GL_INCR_WRAP, GL_KEEP);
	point_pass->gl_state.stencil_op_front = ivec3(GL_KEEP, GL_DECR_WRAP, GL_KEEP);
	point_pass->gl_state.clear_mask = GL_STENCIL_BUFFER_BIT;
	point_pass->driver_ctxt = this;

	selection_render_pass * sel_pass_opaque = new selection_render_pass;
	sel_pass_opaque->rq = queue(SCENE_SELECTION_QUEUE);
	sel_pass_opaque->ren_target = render_target(ACCUM_TARGET);
	sel_pass_opaque->gl_state.blending = true;
	sel_pass_opaque->gl_state.blend_func = ivec2(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	sel_pass_opaque->gl_state.blend_eqn = GL_FUNC_ADD;
	sel_pass_opaque->gl_state.culling = true;
	sel_pass_opaque->gl_state.cull_face = GL_BACK;
	sel_pass_opaque->gl_state.clear_mask = GL_STENCIL_BUFFER_BIT;
	sel_pass_opaque->gl_state.depth_test = false;
	sel_pass_opaque->gl_state.depth_write = true;
	sel_pass_opaque->gl_state.stencil_test = true;
	sel_pass_opaque->gl_state.stencil_op_back = ivec3(GL_KEEP, GL_KEEP, GL_REPLACE);
	sel_pass_opaque->gl_state.stencil_op_front = ivec3(GL_KEEP, GL_KEEP, GL_REPLACE);
	sel_pass_opaque->gl_state.stencil_func = ivec3(GL_ALWAYS, 1, -1);
	sel_pass_opaque->driver_ctxt = this;

	ui_render_pass * ui_pass = new ui_render_pass;
	ui_pass->rq = queue(UI_RENDER_QUEUE);
	ui_pass->ren_target = render_target(ACCUM_TARGET);
	ui_pass->gl_state.blending = true;
	ui_pass->gl_state.blend_func = ivec2(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	ui_pass->gl_state.blend_eqn = GL_FUNC_ADD;
	ui_pass->gl_state.culling = true;
	ui_pass->gl_state.cull_face = GL_BACK;
	ui_pass->gl_state.clear_mask = 0;
	ui_pass->gl_state.depth_test = false;
	ui_pass->gl_state.depth_write = false;
	ui_pass->gl_state.stencil_test = false;
	ui_pass->driver_ctxt = this;

	final_render_pass * final_pass = new final_render_pass;
	final_pass->ren_target = m_default_target;
    final_pass->read_buffer = render_target(ACCUM_TARGET);
	final_pass->gl_state.depth_test = false;
	final_pass->gl_state.depth_write = false;
	final_pass->gl_state.culling = false;
	final_pass->gl_state.blending = false;
	final_pass->gl_state.stencil_test = false;
	final_pass->gl_state.cull_face = GL_BACK;
	final_pass->driver_ctxt = this;

	render_passes.push_back(gbuf_pass);
#ifdef ORDER_INDEPENDENT_TRANSLUCENCY
    render_passes.push_back(oit_pass);
#endif
    render_passes.push_back(dir_shadow_pass);
    render_passes.push_back(dir_pass);
    render_passes.push_back(spot_shadow_pass);
    render_passes.push_back(spot_pass);
    render_passes.push_back(point_shadow_pass);
    render_passes.push_back(point_pass);
#ifndef ORDER_INDEPENDENT_TRANSLUCENCY
    //render_passes.push_back(st_pass);
#endif
    render_passes.push_back(sel_pass_opaque);
    render_passes.push_back(ui_pass);
	render_passes.push_back(final_pass);
}

void gl_ctxt::window_resized(const ivec2 & new_size)
{
	m_default_target->resize(new_size);
	vid_ctxt::window_resized(new_size);
}

const ivec2 & gl_ctxt::window_size()
{
	return m_default_target->size;
}

void gl_ctxt::push_chunk(nstform_ent_chunk * chnk)
{
	// now go through second time after recursive update
    if (chnk == nullptr)
        return;

	auto ents = chnk->all_entities();

	if (ents == nullptr)
		return;

	top_mat_id = 0;
	_add_instanced_draw_calls_from_chunk(chnk);
	_add_draw_calls_from_chunk(chnk);
	_add_selection_draw_calls(chnk);
	_add_lights_from_chunk(chnk);
}

void gl_ctxt::_add_instanced_draw_calls_from_chunk(nstform_ent_chunk * chunk)
{
	nsentity * camera = focused_vp->camera;

	// update render components and the draw call list
	render_queue * chunk_dcq = queue(SCENE_OPAQUE_QUEUE);
	render_queue * chunk_tdcq = queue(SCENE_TRANSLUCENT_QUEUE);
	nscam_comp * cc = camera->get<nscam_comp>();
	fvec2 terh;
	nsrender_comp * rComp = nullptr;
	nsanim_comp * animComp = nullptr;
	nsmesh * currentMesh = nullptr;

	auto iter = instance_objs.begin();
	while (iter != instance_objs.end())
	{
		if ((*iter)->shared_geom_tforms.size() < 2)
		{
			for (uint32 j = 0; j < (*iter)->shared_geom_tforms.size(); ++j)
				(*iter)->shared_geom_tforms[j]->inst_obj = nullptr;

			dprint("gl_ctxt::_add_instanced_draw_calls_from_chunk removing shared instance obj as no more shared mesh/material combos left");
			(*iter)->video_context_release();
			delete (*iter);
			iter = instance_objs.erase(iter);
		}
		else
		{
			terh.set(0.0f, 1.0f);
			rComp = (*iter)->shared_geom_tforms[0]->owner()->get<nsrender_comp>();
			animComp = (*iter)->shared_geom_tforms[0]->owner()->get<nsanim_comp>();
			currentMesh = get_asset<nsmesh>(rComp->mesh_id());
			
			if (currentMesh == nullptr)
			{
				++iter;
				continue;
			}

			nsmesh::submesh * mSMesh = nullptr;
			nsmaterial * mat = nullptr;
			fmat4_vector * fTForms = nullptr;
			nsshader * shader = nullptr;
			for (uint32 i = 0; i < currentMesh->count(); ++i)
			{
				mSMesh = currentMesh->sub(i);
			
				mat = get_asset<nsmaterial>(rComp->material_id(i));
				if (mat == nullptr)
					mat = nse.video_driver<nsgl_driver>()->default_mat();

				shader = get_asset<nsshader>(mat->shader_id());
				if (shader == nullptr)
				{
					if (mat->wireframe())
						shader = nse.core()->get<nsshader>(GBUFFER_INSTANCED_WF_SHADER);
					else if (mat->alpha_blend())
						shader = nse.core()->get<nsshader>(GBUFFER_INSTANCED_TRANS_SHADER);
					else
						shader = nse.core()->get<nsshader>(GBUFFER_INSTANCED_SHADER);
				}

				if (animComp != nullptr && !animComp->final_transforms()->empty())
					fTForms = animComp->final_transforms();

				all_single_draw_calls.resize(all_single_draw_calls.size()+1);
				geometry_draw_call * dc = &all_single_draw_calls[all_single_draw_calls.size()-1];
				
				dc->submesh = mSMesh;
				dc->anim_transforms = fTForms;
				dc->height_minmax = terh;
				dc->chunk_id = (*iter)->shared_geom_tforms[0]->chunk_id();
				dc->casts_shadows = rComp->cast_shadow();
				dc->transparent_picking = false;
				dc->mat_index = top_mat_id;
				dc->mat = mat;
				dc->shdr = shader;
				dc->instanced = true;

				dc->instanced_dci.tform_buffer = (*iter)->video_obj<nsgl_tform_comp_obj>()->gl_tform_buffer;
				dc->instanced_dci.tform_id_buffer = (*iter)->video_obj<nsgl_tform_comp_obj>()->gl_tform_id_buffer;
				dc->instanced_dci.transform_count = (*iter)->visible_count;

				auto inserted = mat_shader_ids.emplace(mat, top_mat_id);
				dc->mat_index = inserted.first->second;
				if (inserted.second)
					++top_mat_id;


				if (mat->alpha_blend())
				{
					dc->transparent_picking = rComp->transparent_picking;
					chunk_tdcq->push_back(dc);
				}
				else
				{
					chunk_dcq->push_back(dc);
				}
			}
			++iter;
		}
	}
}

void gl_ctxt::push_viewport_ui(viewport * vp)
{
	render_queue * dc_dq = queue(UI_RENDER_QUEUE);
	for (uint32 i = 0; i < vp->ui_canvases.size(); ++i)
	{
		nsui_canvas_comp * uicc = vp->ui_canvases[i]->get<nsui_canvas_comp>();
		for (uint32 i = 0; i < uicc->m_ordered_ents.size(); ++i)
		{
			nsentity * cur_ent = uicc->m_ordered_ents[i];

			nsui_material_comp * uimat = cur_ent->get<nsui_material_comp>();
			nsrect_tform_comp * tuic = cur_ent->get<nsrect_tform_comp>();
			nsui_button_comp * uib = cur_ent->get<nsui_button_comp>();
			nsui_text_comp * uitxt = cur_ent->get<nsui_text_comp>();
			nsui_text_input_comp * uitxt_input = cur_ent->get<nsui_text_input_comp>();
			
			all_ui_draw_calls.resize(all_ui_draw_calls.size()+1);
			ui_draw_call * uidc = &all_ui_draw_calls[all_ui_draw_calls.size()-1];

			uidc->entity_id = uivec3(cur_ent->full_id(),0);
			uidc->content_wscale = tuic->content_world_scale(uicc);
			uidc->content_tform = tuic->content_transform(uicc);

			if (uimat != nullptr)
			{
				uidc->mat = get_asset<nsmaterial>(uimat->mat_id);
				if (uidc->mat == nullptr)
					uidc->mat = nse.core()->get<nsmaterial>(DEFAULT_MATERIAL);
				uidc->shdr = get_asset<nsshader>(uimat->mat_shader_id);
				if (uidc->shdr == nullptr)
					uidc->shdr = nse.core()->get<nsshader>(UI_SHADER);
				uidc->top_border_radius = uimat->top_border_radius;
				uidc->bottom_border_radius = uimat->bottom_border_radius;
				uidc->border_pix = uimat->border_size;
				uidc->border_mat = get_asset<nsmaterial>(uimat->border_mat_id);
			}
			
			// If there is a ui text component copy that stuff over
			if (uitxt != nullptr)
			{
				uidc->text_shader = get_asset<nsshader>(uitxt->text_shader_id);
				uidc->text = uitxt->text;
				uidc->fnt = get_asset<nsfont>(uitxt->font_id);
				uidc->fnt_material = get_asset<nsmaterial>(uitxt->font_material_id);
				if (uidc->fnt_material == nullptr)
					uidc->fnt_material = nse.core()->get<nsmaterial>(DEFAULT_MATERIAL);
				uidc->text_line_sizes = uitxt->text_line_sizes;
				uidc->text_margins = uitxt->margins;
				uidc->alignment = uitxt->text_alignment;
			}

			if (uitxt_input != nullptr)
			{
				int mod_ = int(nse.timer()->elapsed()*1000.0f / uitxt_input->cursor_blink_rate_ms);
				uidc->text_editable = (mod_ % 2 == 0)
					&& (uitxt->owner() == nse.system<nsui_system>()->focused_ui_element());
				uidc->cursor_color = uitxt_input->cursor_color;
				uidc->cursor_pixel_width = uitxt_input->cursor_pixel_width;
				uidc->cursor_offset = uitxt_input->cursor_offset;
			}
			
			dc_dq->push_back(uidc);
		}
	}	
}

void gl_ctxt::clear_render_queues()
{
	vid_ctxt::clear_render_queues();
	mat_shader_ids.clear();
	all_single_draw_calls.resize(0);
	all_light_draw_calls.resize(0);
	all_ui_draw_calls.resize(0);
}
	
void gl_ctxt::render_to_viewport(viewport * vp)
{
	if (!nse.video_driver<nsgl_driver>()->shaders_are_valid())
		return;

    //render_passes[oit]->enabled = vp->order_independent_transparency;
    //render_passes[dir_shadow]->enabled = vp->dir_light_shadows;
   // render_passes[dir_light]->enabled = vp->dir_lights;
    //render_passes[spot_shadow]->enabled = vp->spot_light_shadows;
    //render_passes[spot_light]->enabled = vp->spot_lights;
    //render_passes[point_shadow]->enabled = vp->point_light_shadows;
    //render_passes[point_light]->enabled = vp->point_lights;
   // render_passes[selection]->enabled = vp->picking_enabled;

	for (uint32 i = 0; i < render_passes.size(); ++i)
	{
		gl_render_pass * rp = (gl_render_pass *)render_passes[i];
		if (rp == nullptr || rp->ren_target == nullptr)
		{
			dprint("nsvideo_driver::_render_pass - pass " + std::to_string(i) + " is not correctly setup");
		}
		else if (rp->enabled)
		{
			rp->vp = vp;
			rp->setup();
			rp->render();
			rp->finish();
		}
	}
	#ifdef ORDER_INDEPENDENT_TRANSLUCENCY
	m_tbuffers->reset_atomic_counter();
	#endif
}

void gl_ctxt::render_to_all_viewports()
{
	static bool vperr = false;
    if (vp_list.empty())
	{
        if (!vperr)
            dprint("nsrender_system::render Warning - no viewports - will render nothing");
		vperr = true;
        return;
	}
    vperr = false;

	auto iter = vp_list.begin();
	while (iter != vp_list.end())
	{
		all_ui_draw_calls.resize(0);
		queue(UI_RENDER_QUEUE)->resize(0);
		push_viewport_ui(iter->vp);
		render_to_viewport(iter->vp);
		++iter;
	}	
}

void gl_ctxt::set_gl_state(const opengl_state & state)
{
	set_gl_viewport(state.current_viewport);
	enable_depth_test(state.depth_test);
	enable_depth_write(state.depth_write);
	enable_stencil_test(state.stencil_test);
	enable_blending(state.blending);
	enable_culling(state.culling);

	if (state.stencil_test)
	{
		set_stencil_func(state.stencil_func);
		set_stencil_op_back(state.stencil_op_back);
		set_stencil_op_front(state.stencil_op_front);
	}

	if (state.culling)
		set_cull_face(state.cull_face);

	if (state.blending)
	{
		set_blend_func(state.blend_func);
		set_blend_eqn(state.blend_eqn);
	}

	if (state.clear_mask != 0)
		clear_framebuffer(state.clear_mask);
}

void gl_ctxt::set_blend_eqn(int32 eqn)
{
	if (eqn == 0)
		return;
	
	glBlendEquation(eqn);
	m_gl_state.blend_eqn = eqn;
	gl_err_check("nsopengl_driver::set_blend_eqn");	
}


void gl_ctxt::enable_depth_write(bool enable)
{
	glDepthMask(enable);
	m_gl_state.depth_write = enable;
	gl_err_check("nsopengl_driver::enable_depth_write");
}

void gl_ctxt::enable_depth_test(bool enable)
{
	if (enable)
	{
		glEnable(GL_DEPTH_TEST);
	}
	else
	{
		glDisable(GL_DEPTH_TEST);
		m_gl_state.depth_write = enable;
	}
    gl_err_check("nsopengl_driver::enable_depth_test");
	m_gl_state.depth_test = enable;
}

void gl_ctxt::enable_stencil_test(bool enable)
{
	if (enable)
		glEnable(GL_STENCIL_TEST);
	else
		glDisable(GL_STENCIL_TEST);
	gl_err_check("nsopengl_driver::enable_stencil_test");
	m_gl_state.stencil_test = enable;
}

void gl_ctxt::enable_blending(bool enable)
{
	if (enable)
		glEnable(GL_BLEND);
	else
		glDisable(GL_BLEND);
	m_gl_state.blending = enable;
	gl_err_check("nsopengl_driver::enable_blending");
}

void gl_ctxt::enable_culling(bool enable)
{
	if (enable)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);
	m_gl_state.culling = enable;
	gl_err_check("nsopengl_driver::enable_culling");
}

void gl_ctxt::set_cull_face(int32 cull_face)
{
	if (cull_face != m_gl_state.cull_face)
	{
		glCullFace(cull_face);
		m_gl_state.cull_face = cull_face;
		gl_err_check("nsopengl_driver::set_cull_face");
	}	
}

void gl_ctxt::set_blend_func(int32 sfactor, int32 dfactor)
{
	set_blend_func(ivec2(sfactor,dfactor));
}

void gl_ctxt::set_blend_func(const ivec2 & blend_func)
{
	glBlendFunc(blend_func.x, blend_func.y);
	m_gl_state.blend_func = blend_func;
	gl_err_check("nsopengl_driver::set_blend_func");
}

void gl_ctxt::set_stencil_func(int32 func, int32 ref, int32 mask)
{
	set_stencil_func(ivec3(func,ref,mask));
}

void gl_ctxt::set_stencil_func(const ivec3 & stencil_func)
{
	glStencilFunc(stencil_func.x, stencil_func.y, stencil_func.z);
	m_gl_state.stencil_func = stencil_func;
	gl_err_check("nsopengl_driver::set_stencil_func");
}

void gl_ctxt::set_stencil_op(int32 sfail, int32 dpfail, int32 dppass)
{
	set_stencil_op(ivec3( sfail, dpfail, dppass));
}

void gl_ctxt::set_stencil_op(const ivec3 & stencil_op)
{
	glStencilOp(stencil_op.x, stencil_op.y, stencil_op.z);
	m_gl_state.stencil_op_back = stencil_op;
	m_gl_state.stencil_op_front = stencil_op;
	gl_err_check("nsopengl_driver::set_stencil_op");
}

void gl_ctxt::set_stencil_op_back(int32 sfail, int32 dpfail, int32 dppass)
{
	set_stencil_op_back(ivec3(sfail, dpfail, dppass));	
}
	
void gl_ctxt::set_stencil_op_back(const ivec3 & stencil_op)
{
	glStencilOpSeparate(GL_BACK, stencil_op.x, stencil_op.y, stencil_op.z);
	m_gl_state.stencil_op_back = stencil_op;
	gl_err_check("nsopengl_driver::set_stencil_op_back");
}

void gl_ctxt::set_stencil_op_front(int32 sfail, int32 dpfail, int32 dppass)
{
	set_stencil_op_front(ivec3(sfail, dpfail, dppass));		
}
	
void gl_ctxt::set_stencil_op_front(const ivec3 & stencil_op)
{
	glStencilOpSeparate(GL_FRONT, stencil_op.x, stencil_op.y, stencil_op.z);
	m_gl_state.stencil_op_front = stencil_op;
	gl_err_check("nsopengl_driver::set_stencil_op_front");
}

uint32 gl_ctxt::bound_fbo()
{
	GLint params;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &params);
	return params;
}

uint32 gl_ctxt::active_tex_unit()
{
	GLint act_un;
	glGetIntegerv(GL_ACTIVE_TEXTURE, &act_un);
	return act_un - BASE_TEX_UNIT;
}

void gl_ctxt::set_active_texture_unit(uint32 tex_unit)
{
	glActiveTexture(BASE_TEX_UNIT + tex_unit);
	gl_err_check("nsopengl_driver::set_active_texture_unit");
}

void gl_ctxt::set_gl_viewport(const ivec4 & val)
{
	if (m_gl_state.current_viewport != val)
	{
		glViewport(val.x, val.y, val.z, val.w);
		m_gl_state.current_viewport = val;
		gl_err_check("nsopengl_driver::set_viewport");
	}
}

void gl_ctxt::clear_framebuffer(uint32 clear_mask)
{
	glClear(clear_mask);
	gl_err_check("nsopengl_driver::clear_framebuffer");
}

void gl_ctxt::bind_textures(nsmaterial * material)
{
	nsmaterial::texmap_map_const_iter cIter = material->begin();
	while (cIter != material->end())
	{
		nstexture * t = get_asset<nstexture>(cIter->second.tex_id);
		if (t != nullptr)
		{
			set_active_texture_unit(cIter->first);
			t->video_obj<nsgl_texture_obj>()->gl_tex->bind();
		}
		++cIter;
	}	
}

void gl_ctxt::bind_gbuffer_textures(nsgl_framebuffer * fb)
{
	nsgl_gbuffer * obj = dynamic_cast<nsgl_gbuffer*>(fb);

	if (obj == nullptr)
		return;

	for (uint32 i = 0; i < nsgl_gbuffer::attrib_count; ++i)
	{
		nsgl_framebuffer::attachment * att = obj->color(i);
		set_active_texture_unit(att->m_tex_unit);
		att->m_texture->video_obj<nsgl_texture_obj>()->gl_tex->bind();
	}
}

void gl_ctxt::render_geometry_dc(geometry_draw_call * idc, nsgl_shader * bound_shader)
{
	nsgl_submesh_obj * so = idc->submesh->video_obj<nsgl_submesh_obj>();
	
	so->gl_vao->bind();
	if (!idc->instanced)
	{
        if (!idc->mat->alpha_blend() || idc->transparent_picking)
			bound_shader->set_uniform("entityID", idc->single_dci.entity_id);
		else
            bound_shader->set_uniform("entityID", uint32(0));
		bound_shader->set_uniform("transform", idc->single_dci.transform);
		bound_shader->validate();
		gl_err_check("instanced_geometry_draw_call::render pre");
		glDrawElements(get_gl_prim_type(idc->submesh->m_prim_type),
					   static_cast<GLsizei>(idc->submesh->m_indices.size()),
					   GL_UNSIGNED_INT,
					   0);
		gl_err_check("instanced_geometry_draw_call::render post");
		so->gl_vao->unbind();
	}
	else
	{	
		idc->instanced_dci.tform_buffer->bind();
		for (uint32 tfInd = 0; tfInd < 4; ++tfInd)
		{
			so->gl_vao->add(idc->instanced_dci.tform_buffer, nsgl_shader::loc_instance_tform + tfInd);
			so->gl_vao->vertex_attrib_ptr(nsgl_shader::loc_instance_tform + tfInd, 4, GL_FLOAT, GL_FALSE, sizeof(fmat4), sizeof(fvec4)*tfInd);
			so->gl_vao->vertex_attrib_div(nsgl_shader::loc_instance_tform + tfInd, 1);
		}

		idc->instanced_dci.tform_id_buffer->bind();
		so->gl_vao->add(idc->instanced_dci.tform_id_buffer, nsgl_shader::loc_ref_id);
		so->gl_vao->vertex_attrib_I_ptr(nsgl_shader::loc_ref_id, 1, GL_UNSIGNED_INT, sizeof(uint32), 0);
		so->gl_vao->vertex_attrib_div(nsgl_shader::loc_ref_id, 1);	

		bound_shader->validate();
		gl_err_check("instanced_geometry_draw_call::render pre");
		glDrawElementsInstanced(get_gl_prim_type(idc->submesh->m_prim_type),
								static_cast<GLsizei>(idc->submesh->m_indices.size()),
								GL_UNSIGNED_INT,
								0,
								idc->instanced_dci.transform_count);
		gl_err_check("instanced_geometry_draw_call::render post");
		idc->instanced_dci.tform_buffer->bind();
		so->gl_vao->remove(idc->instanced_dci.tform_buffer);

		idc->instanced_dci.tform_id_buffer->bind();
		so->gl_vao->remove(idc->instanced_dci.tform_id_buffer);
	}
	so->gl_vao->unbind();
}

void gl_ctxt::render_light_dc(light_draw_call * idc, nsgl_shader * bound_shader)
{
    nsmesh::submesh * sub = nullptr;

	if (idc->submesh == nullptr)
        sub = nse.core()->get<nsmesh>(MESH_DIRLIGHT_BOUNDS)->sub(0);
	else
        sub = idc->submesh;

    nsgl_submesh_obj * so = sub->video_obj<nsgl_submesh_obj>();
    so->gl_vao->bind();
    bound_shader->validate();
    gl_err_check("pre render_light_dc::render pre");
    glDrawElements(get_gl_prim_type(sub->m_prim_type),
                   static_cast<GLsizei>(sub->m_indices.size()),
                   GL_UNSIGNED_INT,
                   0);
    gl_err_check("post render_light_dc::render post");
    so->gl_vao->unbind();
}

void gl_ctxt::render_ui_dc(ui_draw_call * idc, nsgl_shader * bound_shader)
{
    nsmesh::submesh * sub = nse.core()->get<nsmesh>(MESH_DIRLIGHT_BOUNDS)->sub(0);
    nsgl_submesh_obj * so = sub->video_obj<nsgl_submesh_obj>();
    so->gl_vao->bind();
    bound_shader->validate();
    gl_err_check("pre render_light_dc::render pre");
    glDrawElements(get_gl_prim_type(sub->m_prim_type),
                   static_cast<GLsizei>(sub->m_indices.size()),
                   GL_UNSIGNED_INT,
                   0);
    gl_err_check("post render_light_dc::render post");
    so->gl_vao->unbind();
}

void gl_ctxt::_add_lights_from_chunk(nstform_ent_chunk * chunk)
{
	auto ents = chunk->entities_with_comp<nslight_comp>();
	if (ents == nullptr)
		return;
	
	render_queue * dc_dq = queue(DIR_LIGHT_QUEUE);
	render_queue * dc_sq = queue(SPOT_LIGHT_QUEUE);
	render_queue * dc_pq = queue(POINT_LIGHT_QUEUE);

	auto l_iter = ents->begin();
	while (l_iter != ents->end())
	{
		nslight_comp * lcomp = (*l_iter)->get<nslight_comp>();
		nstform_comp * tcomp = (*l_iter)->get<nstform_comp>();
		nsmesh * boundingMesh = get_asset<nsmesh>(lcomp->mesh_id());
		
		all_light_draw_calls.resize(all_light_draw_calls.size()+1);

		fmat4 proj;
		if (lcomp->get_light_type() == nslight_comp::l_spot)
			proj = perspective(2*lcomp->angle(), 1.0f, lcomp->shadow_clipping().x, lcomp->shadow_clipping().y);
		else if(lcomp->get_light_type() == nslight_comp::l_point)
			proj = perspective(90.0f, 1.0f, lcomp->shadow_clipping().x, lcomp->shadow_clipping().y);
		else
			proj = ortho(-100.0f,100.0f,100.0f,-100.0f,100.0f,-100.0f);

		
		light_draw_call * ldc = &all_light_draw_calls[all_light_draw_calls.size()-1];
			
		ldc->submesh = nullptr;
		ldc->direction = tcomp->world_orientation().target();
		ldc->diffuse_intensity = lcomp->intensity().x;
		ldc->ambient_intensity = lcomp->intensity().y;
		ldc->cast_shadows = lcomp->cast_shadows();
		ldc->light_color = lcomp->color();
		ldc->light_type = lcomp->get_light_type();
		ldc->shadow_samples = lcomp->shadow_samples();
		ldc->shadow_darkness = lcomp->shadow_darkness();
		ldc->material_ids = &mat_shader_ids;
		ldc->spot_atten = lcomp->atten();
		ldc->light_pos = tcomp->world_position();
		ldc->light_transform = tcomp->world_tf() % lcomp->scaling();
		ldc->max_depth = lcomp->shadow_clipping().y - lcomp->shadow_clipping().x;
		if (lcomp->get_light_type() == nslight_comp::l_dir)
		{
			ldc->proj_light_mat = proj * tcomp->world_inv_tf();
			ldc->shdr = nse.core()->get<nsshader>(DIR_LIGHT_SHADER);
			dc_dq->push_back(ldc);
		}
		else if (lcomp->get_light_type() == nslight_comp::l_spot)
		{
			ldc->shdr = nse.core()->get<nsshader>(SPOT_LIGHT_SHADER);
			ldc->proj_light_mat = proj * tcomp->world_inv_tf();
			if (boundingMesh != nullptr)
			{
				for (uint32 i = 0; i < boundingMesh->count(); ++i)
				{
					ldc->submesh = boundingMesh->sub(i);
					dc_sq->push_back(ldc);
				}
			}
		}
		else
		{
			ldc->shdr = nse.core()->get<nsshader>(POINT_LIGHT_SHADER);
			ldc->proj_light_mat = proj;
			if (boundingMesh != nullptr)
			{
				for (uint32 i = 0; i < boundingMesh->count(); ++i)
				{
					ldc->submesh = boundingMesh->sub(i);
					dc_pq->push_back(ldc);
				}
			}
		}
		++l_iter;
	}
}

void gl_ctxt::_add_draw_calls_from_chunk(nstform_ent_chunk * chunk)
{
	if (focused_vp == nullptr)
		return;

	nsentity * camera = focused_vp->camera;
	if (camera == NULL)
		return;

	auto ents = chunk->all_entities();
	if (ents == nullptr)
		return;

	// update render components and the draw call list
	render_queue * chunk_dcq = queue(SCENE_OPAQUE_QUEUE);
	render_queue * chunk_tdcq = queue(SCENE_TRANSLUCENT_QUEUE);

	nscam_comp * cc = camera->get<nscam_comp>();	
	fvec2 terh;
	nsrender_comp * rComp = nullptr;
	nstform_comp * tComp = nullptr;
	nsanim_comp * animComp = nullptr;
	nsterrain_comp * terComp = nullptr;
	nsmesh * currentMesh = nullptr;
	nslight_comp * lc = nullptr;
	nssel_comp * sc = nullptr;

	auto iter = ents->begin();
	while (iter != ents->end())
	{
		terh.set(0.0f, 1.0f);
		rComp = (*iter)->get<nsrender_comp>();
		tComp = (*iter)->get<nstform_comp>();

		// if instance obj skip
		if (tComp->inst_obj != nullptr || rComp == nullptr)
		{
			++iter;
			continue;
		}
		
		lc = (*iter)->get<nslight_comp>();
		sc = (*iter)->get<nssel_comp>();
		animComp = (*iter)->get<nsanim_comp>();
		terComp = (*iter)->get<nsterrain_comp>();
		currentMesh = get_asset<nsmesh>(rComp->mesh_id());

		if (lc != nullptr)
		{
			if (lc->update_posted())
				lc->post_update(false);
		}

		if (rComp->update_posted())
			rComp->post_update(false);

		if (currentMesh == nullptr)
		{
			++iter;
			continue;
		}
		
		nsmesh::submesh * mSMesh = nullptr;
		nsmaterial * mat = nullptr;
		fmat4_vector * fTForms = nullptr;
		nsshader * shader = nullptr;

		for (uint32 i = 0; i < currentMesh->count(); ++i)
		{
			mSMesh = currentMesh->sub(i);
			mat = get_asset<nsmaterial>(rComp->material_id(i));

			if (mat == nullptr)
				mat = nse.video_driver<nsgl_driver>()->default_mat();

			shader = get_asset<nsshader>(mat->shader_id());
			fTForms = nullptr;

			if (shader == nullptr)
			{
				if (mat->wireframe())
					shader = nse.core()->get<nsshader>(GBUFFER_WF_SHADER);
				else if (mat->alpha_blend())
					shader = nse.core()->get<nsshader>(GBUFFER_TRANS_SHADER);
				else
					shader = nse.core()->get<nsshader>(GBUFFER_SHADER);
			}

			if (animComp != nullptr && !animComp->final_transforms()->empty())
				fTForms = animComp->final_transforms();

			if (terComp != nullptr)
				terh = terComp->height_bounds();
			
			
			all_single_draw_calls.resize(all_single_draw_calls.size()+1);
			geometry_draw_call * dc = &all_single_draw_calls[all_single_draw_calls.size()-1];
			dc->submesh = mSMesh;
			dc->anim_transforms = fTForms;
			dc->height_minmax = terh;
			dc->chunk_id = (*iter)->chunk_id();
			dc->casts_shadows = rComp->cast_shadow();
			dc->transparent_picking = false;
			dc->mat = mat;
			dc->shdr = shader;
			dc->single_dci.entity_id = (*iter)->id();
			dc->single_dci.transform = tComp->world_tf();

			auto inserted = mat_shader_ids.emplace(mat, top_mat_id);
			dc->mat_index = inserted.first->second;
			if (inserted.second)
				++top_mat_id;

			if (mat->alpha_blend())
			{
				dc->transparent_picking = rComp->transparent_picking;
				chunk_tdcq->push_back(dc);
			}
			else
			{
				chunk_dcq->push_back(dc);
			}
 		}
		
		++iter;
	}
}

void gl_ctxt::_add_selection_draw_calls(nstform_ent_chunk * chunk)
{
	// add selection draw calls
	render_queue * chunk_sel = queue(SCENE_SELECTION_QUEUE);

	entity_set & ents = nse.system<nsselection_system>()->current_selection();
	auto sel_ent_iter = ents.begin();
	while (sel_ent_iter != ents.end())
	{
		nsrender_comp * rComp = (*sel_ent_iter)->get<nsrender_comp>();
		nsmesh * currentMesh = get_asset<nsmesh>(rComp->mesh_id());
		if (currentMesh == nullptr)
		{
			++sel_ent_iter;
			continue;
		}

		nstform_comp * tform = (*sel_ent_iter)->get<nstform_comp>();
		nssel_comp * sc = (*sel_ent_iter)->get<nssel_comp>();

		nsterrain_comp * tc = (*sel_ent_iter)->get<nsterrain_comp>();
		fvec2 terh;		
		if (tc != nullptr)
			terh = tc->height_bounds();

		nsanim_comp * ac = (*sel_ent_iter)->get<nsanim_comp>();
		fmat4_vector * fTForms = nullptr;
		if (ac != nullptr && !ac->final_transforms()->empty())
			fTForms = ac->final_transforms();
		
		for (uint32 i = 0; i < currentMesh->count(); ++i)
		{
			all_single_draw_calls.resize(all_single_draw_calls.size()+1);
			geometry_draw_call * sel_dc = &all_single_draw_calls[all_single_draw_calls.size()-1];

			nsmaterial * mat = get_asset<nsmaterial>(rComp->material_id(i));
			if (mat == nullptr)
				mat = nse.video_driver<nsgl_driver>()->default_mat();
			
			sel_dc->submesh = currentMesh->sub(i);
			sel_dc->anim_transforms = fTForms;
			sel_dc->height_minmax = terh;
			sel_dc->shdr = nse.core()->get<nsshader>(SELECTION_SHADER);
			sel_dc->mat = mat;
			sel_dc->sel_color = sc->color();
			sel_dc->single_dci.transform = tform->world_tf();
			chunk_sel->push_back(sel_dc);
		}
		
		++sel_ent_iter;
	}
		
}

nsgl_driver::nsgl_driver() :
	nsvideo_driver(),
	m_default_mat(nullptr)
{
	nse.register_vid_obj_type<nsgl_shader_obj>(SHADER_VID_OBJ_GUID);
    nse.register_vid_obj_type<nsgl_texture_obj>(TEXTURE_VID_OBJ_GUID);
    nse.register_vid_obj_type<nsgl_submesh_obj>(MESH_VID_OBJ_GUID);
    nse.register_vid_obj_type<nsgl_tform_comp_obj>(TFORM_VID_OBJ_GUID);
    nse.register_vid_obj_type<nsgl_particle_comp_obj>(PARTICLE_VID_OBJ_GUID);
}

nsgl_driver::~nsgl_driver()
{}

void nsgl_driver::init()
{
	if (m_initialized)
		return;
	
	nsplugin * cplg = nse.core();
	if (cplg == nullptr)
	{
		dprint("nsgl_driver::nsgl_driver You must call nse.start() before creating the video driver");
		return;
	}
	
	// Default material
    nstexture * tex = cplg->load<nstex2d>(nsstring(DEFAULT_MATERIAL) + nsstring(DEFAULT_TEX_EXTENSION), true);
	m_default_mat = cplg->create<nsmaterial>(nsstring(DEFAULT_MATERIAL));
    m_default_mat->add_tex_map(nsmaterial::diffuse, tex->full_id());
    m_default_mat->set_color(fvec4(0.0f,1.0f,1.0f,1.0f));


	// Rendering shaders
    nsstring shext(DEFAULT_SHADER_EXTENSION);
    rshaders.deflt = cplg->load<nsshader>(nsstring(GBUFFER_SHADER) + shext, true);
	rshaders.deflt_wireframe = cplg->load<nsshader>(nsstring(GBUFFER_WF_SHADER) + shext, true);
    rshaders.deflt_instanced = cplg->load<nsshader>(nsstring(GBUFFER_INSTANCED_SHADER) + shext, true);
	rshaders.deflt_instanced_wireframe = cplg->load<nsshader>(nsstring(GBUFFER_INSTANCED_WF_SHADER) + shext, true);
	rshaders.light_stencil = cplg->load<nsshader>(nsstring(LIGHTSTENCIL_SHADER) + shext, true);
#ifdef ORDER_INDEPENDENT_TRANSLUCENCY
    rshaders.deflt_translucent = cplg->load<nsshader>(nsstring(GBUFFER_TRANS_SHADER) + shext, true);
	rshaders.deflt_instanced_translucent = cplg->load<nsshader>(nsstring(GBUFFER_INSTANCED_TRANS_SHADER) + shext, true);
	rshaders.frag_sort = cplg->load<nsshader>(nsstring(FRAGMENT_SORT_SHADER) + shext, true);
#endif
	rshaders.dir_light = cplg->load<nsshader>(nsstring(DIR_LIGHT_SHADER) + shext, true);
	rshaders.point_light = cplg->load<nsshader>(nsstring(POINT_LIGHT_SHADER) + shext, true);
	rshaders.spot_light = cplg->load<nsshader>(nsstring(SPOT_LIGHT_SHADER) + shext, true);
	rshaders.shadow_cube = cplg->load<nsshader>(nsstring(SHADOW_CUBE_SHADER) + shext, true);
	rshaders.shadow_2d = cplg->load<nsshader>(nsstring(SHADOW_2D_SHADER) + shext, true);
	rshaders.shadow_cube_instanced = cplg->load<nsshader>(nsstring(SHADOW_CUBE_INSTANCED_SHADER) + shext, true);
	rshaders.shadow_2d_instanced = cplg->load<nsshader>(nsstring(SHADOW_2D_INSTANCED_SHADER) + shext, true);
	rshaders.sel_shader = cplg->load<nsshader>(nsstring(SELECTION_SHADER) + shext, true);
	rshaders.deflt_particle = cplg->load<nsshader>(nsstring(RENDER_PARTICLE_SHADER) + shext, true);
	cplg->load<nsshader>(nsstring(SKYBOX_SHADER) + shext, true);
	cplg->load<nsshader>(nsstring(UI_SHADER) + shext, true);
	cplg->load<nsshader>(nsstring(UI_TEXT_SHADER) + shext, true);

    // FIGURE THIS OUT LATER
	// particle transform feedback shader
    //nsgl_shader * ps =
    cplg->load<nsshader>(nsstring(PARTICLE_PROCESS_SHADER) + shext, true);
//	std::vector<nsstring> outLocs2;
//	outLocs2.push_back("gPosOut");
//	outLocs2.push_back("gVelOut");
//	outLocs2.push_back("gScaleAndAngleOut");
//	outLocs2.push_back("gAgeOut");
//	ps->xfb = nsgl_shader::xfb_interleaved;
//	ps->xfb_locs = outLocs2;

	// Light bounds, skydome, and tile meshes
    nsmesh * msh = cplg->load<nsmesh>(nsstring(MESH_FULL_TILE) + nsstring(DEFAULT_MESH_EXTENSION), true);
	msh->flip_uv();
	
    cplg->load<nsmesh>(nsstring(MESH_TERRAIN) + nsstring(DEFAULT_MESH_EXTENSION), true);
    cplg->load<nsmesh>(nsstring(MESH_HALF_TILE) + nsstring(DEFAULT_MESH_EXTENSION), true);
    cplg->load<nsmesh>(nsstring(MESH_POINTLIGHT_BOUNDS) + nsstring(DEFAULT_MESH_EXTENSION), true);
    cplg->load<nsmesh>(nsstring(MESH_SPOTLIGHT_BOUNDS) + nsstring(DEFAULT_MESH_EXTENSION), true);
    cplg->load<nsmesh>(nsstring(MESH_DIRLIGHT_BOUNDS) + nsstring(DEFAULT_MESH_EXTENSION), true);
    cplg->load<nsmesh>(nsstring(MESH_SKYDOME) + nsstring(DEFAULT_MESH_EXTENSION), true);

	sig_connect(nse.world()->chunk_added, nsgl_driver::on_chunk_added);
	sig_connect(nse.world()->chunk_removed, nsgl_driver::on_chunk_removed);
	
	nsvideo_driver::init();
}

void nsgl_driver::release()
{
	if (!m_initialized)
		return;
	
	sig_disconnect(nse.world()->chunk_added);
	sig_disconnect(nse.world()->chunk_removed);

	nsvideo_driver::release();
}

uint8 nsgl_driver::create_context()
{
	for (uint8 id = 0; id < MAX_CONTEXT_COUNT; ++id)
	{
		if (m_contexts[id] == nullptr)
		{
			gl_ctxt * ctxt = new gl_ctxt(id);
			ctxt->driver = this;
			m_contexts[id] = ctxt;
			m_current_context = ctxt;
			m_current_context->init();
			make_context_current(id); // i know already current - but want to call the signal and update objs
			return id;
		}
	}
	dprint("nsgl_driver::creat_context Unable to create context - context slots are all occupied");
	return -1;
}

gl_ctxt * nsgl_driver::current_context()
{
    return static_cast<gl_ctxt*>(m_current_context);
}

gl_ctxt * nsgl_driver::context(uint8 id)
{
	if (id > MAX_CONTEXT_COUNT)
		return nullptr;
	return static_cast<gl_ctxt*>(m_contexts[id]);		
}

nsmaterial * nsgl_driver::default_mat()
{
	return m_default_mat;
}

void nsgl_driver::set_default_mat(nsmaterial * mat)
{
	m_default_mat = mat;
}

bool nsgl_driver::shaders_are_valid()
{
	static bool shadererr = false;
	static bool shadernull = false;

	if (!rshaders.valid())
	{
		if (!shadernull)
		{
			dprint("nsopengl_driver::_validCheck: Error - one of the rendering shaders is nullptr ");
			shadernull = true;
		}
		return false;
	}
	shadernull = false;

	if (rshaders.error())
	{
		if (!shadererr)
		{
			dprint("nsopengl_driver::_validCheck: Error in one of the rendering shaders - check compile log");
			shadererr = true;
		}
		return false;
	}
	shadererr = false;
	return true;
}

int32 get_gl_prim_type(mesh_primitive_type pt)
{
	switch (pt)
	{
	  case(prim_points):
		  return GL_POINTS;
	  case(prim_lines):
		  return GL_LINES;
	  case(prim_line_strip):
		  return GL_LINE_STRIP;
	  case(prim_line_loop):
		  return GL_LINE_LOOP;
	  case(prim_triangles):
		  return GL_TRIANGLES;
	  case(prim_triangle_strip):
		  return GL_TRIANGLE_STRIP;
	  case(prim_triangle_fan):
		  return GL_TRIANGLE_FAN;
	  case(prim_patch):
		  return GL_PATCHES;
	  default:
		  return -1;
	}
}

void nsgl_driver::on_chunk_added(nstform_ent_chunk * chnk)
{
	sig_connect(chnk->entity_added, nsgl_driver::on_chunk_ent_added);
	sig_connect(chnk->entity_removed, nsgl_driver::on_chunk_ent_removed);
}

void nsgl_driver::on_chunk_removed(nstform_ent_chunk * chnk)
{
	sig_disconnect(chnk->entity_added);
	sig_disconnect(chnk->entity_removed);
}

void nsgl_driver::on_chunk_ent_added(nsentity* ent)
{
	make_instanced_if_possible(ent, nse.world()->chunk(ent->get<nstform_comp>()->chunk_id()));
	sig_connect(ent->component_added, nsgl_driver::on_chunk_ent_comp_added);
	sig_connect(ent->component_removed, nsgl_driver::on_chunk_ent_comp_added);
}

void nsgl_driver::on_chunk_ent_removed(nsentity* ent)
{
	sig_disconnect(ent->component_added);
	sig_disconnect(ent->component_removed);
	remove_instancing(ent, nse.world()->chunk(ent->get<nstform_comp>()->chunk_id()));
}

void nsgl_driver::on_chunk_ent_comp_added(nsentity * ent, nscomponent * comp)
{
	nstform_ent_chunk * chnk = nse.world()->chunk(ent->get<nstform_comp>()->chunk_id());
	if (comp->type() == type_to_hash(nsrender_comp))
	{
		make_instanced_if_possible(ent, chnk);
	}
	if (comp->type() == type_to_hash(nsanim_comp) || comp->type() == type_to_hash(nssprite_sheet_comp))
	{
		remove_instancing(ent, chnk);
		make_instanced_if_possible(ent, chnk);
	}
	sig_connect(ent->component_edited, nsgl_driver::on_chunk_ent_comp_edited);
}

void nsgl_driver::on_chunk_ent_comp_removed(nsentity * ent, nscomponent * comp)
{
	sig_disconnect(ent->component_edited);
	nstform_ent_chunk * chnk = nse.world()->chunk(ent->get<nstform_comp>()->chunk_id());
	if (comp->type() == type_to_hash(nsrender_comp))
	{
		remove_instancing(ent, chnk);
	}
	if (comp->type() == type_to_hash(nsanim_comp) || comp->type() == type_to_hash(nssprite_sheet_comp))
	{
		remove_instancing(ent, chnk);
		make_instanced_if_possible(ent, chnk);
	}		
}

void nsgl_driver::on_chunk_ent_comp_edited(nsentity * ent, nscomponent * comp)
{
	nstform_ent_chunk * chnk = nse.world()->chunk(ent->get<nstform_comp>()->chunk_id());
	if (comp->type() == type_to_hash(nsrender_comp))
	{
		dprint("Render comp for " + ent->name() + " changed... analyzing render component");
		remove_instancing(ent, chnk);
		make_instanced_if_possible(ent, chnk);
	}
}

void nsgl_driver::remove_instancing(nsentity * ent, nstform_ent_chunk * chnk)
{
	nstform_comp * tcomp = ent->get<nstform_comp>();
	
	if (tcomp->inst_obj != nullptr)
	{
		auto iter = tcomp->inst_obj->shared_geom_tforms.begin();
		while (iter != tcomp->inst_obj->shared_geom_tforms.end())
		{
			if (tcomp == *iter)
			{
				iter = (*iter)->inst_obj->shared_geom_tforms.erase(iter);
				dprint("Making " + ent->name() + " no longer instanced...");

#ifdef NSDEBUG
				nsstringstream ss;
				ss << "The chunk now contains the following instanced ents\n";
				for (uint32 i = 0; i < tcomp->inst_obj->shared_geom_tforms.size(); ++i)
					ss << tcomp->inst_obj->shared_geom_tforms[i]->owner()->name() << "\n";
				dprint(ss.str());
#endif
				tcomp->inst_obj = nullptr;

				// If there is only one entity left -
				// the auto update code should remove that and delete the obj
				return;
			}
			++iter;
		}
	}		
}
	
void nsgl_driver::make_instanced_if_possible(nsentity * ent, nstform_ent_chunk * chnk)
{
	nstform_comp * tcomp = ent->get<nstform_comp>();
	
	entity_set * ents_with_rcomp = chnk->entities_with_comp<nsrender_comp>();
	if (ents_with_rcomp == nullptr)
		return;
	
	auto fiter = ents_with_rcomp->begin();
	while (fiter != ents_with_rcomp->end())
	{
		if (instancing_candidate(ent, *fiter))
		{
			nstform_comp * ent_tcomp = (*fiter)->get<nstform_comp>();

			if (ent_tcomp->inst_obj != nullptr)
			{
				tcomp->inst_obj = ent_tcomp->inst_obj;
				tcomp->inst_obj->shared_geom_tforms.push_back(tcomp);
				tcomp->inst_obj->needs_update = true;
			}
			else
			{
				tform_per_scene_info * psi = new tform_per_scene_info;
				psi->video_context_init();
				nse.video_driver()->current_context()->instance_objs.push_back(psi);
				tcomp->inst_obj = psi;
				ent_tcomp->inst_obj = psi;
				psi->shared_geom_tforms.push_back(ent_tcomp);
				psi->shared_geom_tforms.push_back(tcomp);
				psi->needs_update = true;
			}
#ifdef NSDEBUG
			nsstringstream ss;
			ss << "Making " + ent->name() + " instanced along with the following ents: \n";
			for (uint32 i = 0; i < tcomp->inst_obj->shared_geom_tforms.size()-1; ++i)
				ss << tcomp->inst_obj->shared_geom_tforms[i]->owner()->name() << "\n";
			dprint(ss.str());
#endif
			return;
		}
		++fiter;
	}
		
}

bool instancing_candidate(nsentity * ent_checking, nsentity * checking_against)
{
	if (ent_checking == checking_against)
		return false;
	
	nsanim_comp * acomp = ent_checking->get<nsanim_comp>();
	nssprite_sheet_comp * scomp = ent_checking->get<nssprite_sheet_comp>();
	nsrender_comp * rcomp = ent_checking->get<nsrender_comp>();	

	nsrender_comp * ent_rcomp = checking_against->get<nsrender_comp>();
	nsanim_comp * ent_acomp = checking_against->get<nsanim_comp>();
	nssprite_sheet_comp * ent_scomp = checking_against->get<nssprite_sheet_comp>();

	if (rcomp == nullptr || ent_rcomp == nullptr)
		return false;

	// The render comps must be equivalent (see operator ==) and the anim/sprite comps must have same address
	return (ent_rcomp != nullptr &&
			(*rcomp) == (*ent_rcomp) &&
			acomp == ent_acomp &&
			scomp == ent_scomp);
}
