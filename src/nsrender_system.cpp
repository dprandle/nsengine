/*! 
  \file nsrendersystem.cpp
	
  \brief Definition file for nsrender_system class

  This file contains all of the neccessary definitions for the nsrender_system class.

  \author Daniel Randle
  \date November 23 2013
  \copywrite Earth Banana Games 2013
*/

#include <nsplugin.h>
#include <nsrender_system.h>
#include <nsui_comp.h>
#include <nslight_comp.h>
#include <nssel_comp.h>
#include <nsentity.h>
#include <nsengine.h>
#include <nsentity_manager.h>
#include <nsshader_manager.h>
#include <nstex_manager.h>
#include <nsmesh_manager.h>
#include <nstimer.h>
#include <nsmat_manager.h>
#include <nsevent_dispatcher.h>
#include <nsscene.h>
#include <nsterrain_comp.h>
#include <nsshadowbuf_object.h>
#include <nsgbuf_object.h>
#include <nsxfb_object.h>
#include <nsrender_comp.h>
#include <nsanim_comp.h>
#include <nstform_comp.h>
#include <nsbuffer_object.h>
#include <nsfb_object.h>
#include <nsshader.h>
#include <nscam_comp.h>
#include <nsbuffer_object.h>
#include <nsparticle_comp.h>

translucent_buffers::translucent_buffers():
	atomic_counter(new nsbuffer_object(nsbuffer_object::atomic_counter, nsbuffer_object::storage_mutable)),
	header(new nsbuffer_object(nsbuffer_object::shader_storage, nsbuffer_object::storage_mutable)),
	fragments(new nsbuffer_object(nsbuffer_object::shader_storage, nsbuffer_object::storage_mutable)),
	header_clr_data(DEFAULT_ACCUM_BUFFER_RES_X*DEFAULT_ACCUM_BUFFER_RES_Y, -1)
{
	atomic_counter->init_gl();
	header->init_gl();
	fragments->init_gl();
}

translucent_buffers::~translucent_buffers()
{
	atomic_counter->release();
	header->release();
	fragments->release();
	delete atomic_counter;
	delete header;
	delete fragments;
}

void translucent_buffers::bind_buffers()
{
	atomic_counter->bind(0);
	header->bind(0);
	fragments->bind(1);
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
	uint32 * ptr = atomic_counter->map<uint32>(0, 1, nsbuffer_object::access_map_range(nsbuffer_object::map_write));
	ptr[0] = 0;
	atomic_counter->unmap();
	atomic_counter->unbind();
}

light_vectors::light_vectors()
{
	dir_lights.reserve(MAX_DIR_LIGHT);
	point_lights.reserve(MAX_POINT_LIGHT);
	spot_lights.reserve(MAX_SPOT_LIGHT);		
}

void light_vectors::clear()
{
	dir_lights.resize(0);
	point_lights.resize(0);
	spot_lights.resize(0);
}

instanced_draw_call::instanced_draw_call():
	submesh(nullptr),
	transform_buffer(nullptr),
	transform_id_buffer(nullptr),
	anim_transforms(nullptr),
	proj_cam(),
	height_minmax(),
	entity_id(0),
	plugin_id(0),
	transform_count(0),
	casts_shadows(false),
	transparent_picking(false)
{}

void instanced_draw_call::render()
{
	// Check to make sure each buffer is allocated before setting the shader attribute : un-allocated buffers
	// are fairly common because not every mesh has tangents for example.. or normals.. or whatever
	submesh->m_vao.bind();

	if (transform_buffer != nullptr)
	{
		transform_buffer->bind();
		for (uint32 tfInd = 0; tfInd < 4; ++tfInd)
		{
			submesh->m_vao.add(transform_buffer, nsshader::loc_instance_tform + tfInd);
			submesh->m_vao.vertex_attrib_ptr(nsshader::loc_instance_tform + tfInd, 4, GL_FLOAT, GL_FALSE, sizeof(fmat4), sizeof(fvec4)*tfInd);
			submesh->m_vao.vertex_attrib_div(nsshader::loc_instance_tform + tfInd, 1);
		}
	}

	if (transform_id_buffer != nullptr)
	{
		transform_id_buffer->bind();
		submesh->m_vao.add(transform_id_buffer, nsshader::loc_ref_id);
		submesh->m_vao.vertex_attrib_I_ptr(nsshader::loc_ref_id, 1, GL_UNSIGNED_INT, sizeof(uint32), 0);
		submesh->m_vao.vertex_attrib_div(nsshader::loc_ref_id, 1);	
	}
	
	gl_err_check("instanced_geometry_draw_call::render pre");
	glDrawElementsInstanced(submesh->m_prim_type,
							static_cast<GLsizei>(submesh->m_indices.size()),
							GL_UNSIGNED_INT,
							0,
							transform_count);
	gl_err_check("instanced_geometry_draw_call::render post");	

	if (transform_buffer != nullptr)
	{
		transform_buffer->bind();
		submesh->m_vao.remove(transform_buffer);
	}

	if (transform_id_buffer != nullptr)
	{
		transform_id_buffer->bind();
		submesh->m_vao.remove(transform_id_buffer);
	}
	submesh->m_vao.unbind();
}

void light_draw_call::render()
{
	gl_err_check("pre dir_light_pass::render");
	if (draw_point != nullptr)
	{
		draw_point->bind();
		glDrawArrays(GL_POINTS, 0, 1);
	}
	else
	{
		submesh->m_vao.bind();
		glDrawElements(submesh->m_prim_type,
					   static_cast<GLsizei>(submesh->m_indices.size()),
					   GL_UNSIGNED_INT,
					   0);
		submesh->m_vao.unbind();
	}
	gl_err_check("post dir_light_pass::render");
}

instanced_draw_call::~instanced_draw_call()
{}

render_shaders::render_shaders() :
	deflt(nullptr),
	deflt_wireframe(nullptr),
	deflt_translucent(nullptr),
	dir_light(nullptr),
	point_light(nullptr),
	spot_light(nullptr),
	light_stencil(nullptr),
	shadow_cube(nullptr),
	shadow_2d(nullptr),
	frag_sort(nullptr),
	deflt_particle(nullptr),
	sel_shader(nullptr)
{}

bool render_shaders::error()
{
	return (
		deflt->error() != nsshader::error_none ||
		deflt_wireframe->error() != nsshader::error_none ||
		deflt_translucent->error() != nsshader::error_none ||
		dir_light->error() != nsshader::error_none ||
		point_light->error() != nsshader::error_none ||
		spot_light->error() != nsshader::error_none ||
		light_stencil->error() != nsshader::error_none ||
		shadow_cube->error() != nsshader::error_none ||
		shadow_2d->error() != nsshader::error_none ||
		frag_sort->error() != nsshader::error_none ||
		deflt_particle->error() != nsshader::error_none ||
		sel_shader->error() != nsshader::error_none);
}

bool render_shaders::valid()
{
	return (
		deflt != nullptr &&
		deflt_wireframe != nullptr &&
		deflt_translucent != nullptr &&
		dir_light != nullptr &&
		point_light != nullptr &&
		spot_light != nullptr &&
		light_stencil != nullptr &&
		shadow_cube != nullptr &&
		shadow_2d != nullptr &&
		frag_sort != nullptr &&
		deflt_particle != nullptr &&
		sel_shader != nullptr);
}

void render_pass::setup_pass()
{
	ren_target->set_target(nsfb_object::fb_draw);
	ren_target->bind();
	ivec2 sz = ren_target->size();
	gl_state.current_viewport = ivec4(norm_viewport.x * sz.x, norm_viewport.y * sz.y, norm_viewport.z * sz.x, norm_viewport.w * sz.y);
	nsrender_system::set_gl_state(gl_state);
}

void render_pass::render()
{
	for (uint32 i = 0; i < draw_calls->size(); ++i)
	{
		draw_call * dc = (*draw_calls)[i];
		dc->shdr->bind(); 
		dc->shdr->set_viewport(gl_state.current_viewport);
		dc->shdr->set_for_draw_call(dc);
		if (dc->mat != nullptr)
		{
			nsrender_system::enable_culling(dc->mat->culling());
			nsrender_system::set_cull_face(dc->mat->cull_mode());
			nsrender_system::bind_textures(dc->mat);
		}
		dc->render();
	}	
}

void selection_render_pass::render()
{
	for (uint32 i = 0; i < draw_calls->size(); ++i)
	{
		instanced_draw_call * dc = (instanced_draw_call*)(*draw_calls)[i];
		
		dc->shdr->bind(); 
		dc->shdr->set_for_draw_call(dc);
		ren_target->set_draw_buffer(nsfb_object::att_none);
		dc->render();
		
		glPolygonMode(GL_FRONT, GL_LINE);
		glLineWidth(4.0f);
		nsrender_system::set_stencil_func(GL_NOTEQUAL, 1, -1);
		ren_target->set_draw_buffer(nsfb_object::att_color);
		((nsselection_shader*)dc->shdr)->set_border_color(dc->sel_color.rgb());
		dc->render();
		
		glLineWidth(1.0f);
		glPolygonMode(GL_FRONT, GL_FILL);
		nsrender_system::enable_depth_test(false);
		nsrender_system::set_stencil_func(GL_EQUAL, 1, 0);
		((nsselection_shader*)dc->shdr)->set_main_color(dc->sel_color);
		dc->render();
	}	
}

void gbuffer_render_pass::render()
{
	render_pass::render();
	nsrender_system::bind_gbuffer_textures(ren_target);
}

void oit_render_pass::render()
{
	ren_target->set_draw_buffer(nsfb_object::att_none);
	for (uint32 i = 0; i < draw_calls->size(); ++i)
	{
		draw_call * dc = (*draw_calls)[i];
		dc->shdr->bind(); 
		dc->shdr->set_viewport(gl_state.current_viewport);
		dc->shdr->set_for_draw_call(dc);
		if (dc->mat != nullptr)
		{
			nsrender_system::enable_culling(dc->mat->culling());
			nsrender_system::set_cull_face(dc->mat->cull_mode());
			nsrender_system::bind_textures(dc->mat);
		}
		tbuffers->bind_buffers();
		dc->render();
	}
	ren_target->update_draw_buffers();
	nsrender_system::enable_depth_test(false);
	nsrender_system::enable_culling(true);
	nsrender_system::set_cull_face(GL_BACK);
	sort_shader->bind();
	sort_shader->set_viewport(gl_state.current_viewport);
	single_point->bind();
	tbuffers->bind_buffers();
	glDrawArrays(GL_POINTS, 0, 1);	
	tbuffers->unbind_buffers();
}

void light_shadow_pass::render()
{
	shdr->bind();
	shdr->set_for_light_draw_call(ldc);
	shdr->set_viewport(gl_state.current_viewport);
	for (uint32 i = 0; i < draw_calls->size(); ++i)
	{
		instanced_draw_call * dc = (instanced_draw_call*)(*draw_calls)[i];
		if (dc->casts_shadows)
		{
			shdr->set_for_draw_call(dc);
			nsrender_system::enable_culling(dc->mat->culling());
			nsrender_system::set_cull_face(dc->mat->cull_mode());
			dc->render();
		}
	}
}

void light_pass::render()
{
	for (uint32 i = 0; i < draw_calls->size(); ++i)
	{
		light_draw_call * dc = (light_draw_call*)(*draw_calls)[i];
		if (dc->cast_shadows)
		{
			rpass->ldc = dc;
			rpass->setup_pass();
			rpass->render();
			nsfb_object::attachment * attch = rpass->ren_target->att(nsfb_object::att_depth);
			nsrender_system::set_active_texture_unit(attch->m_tex_unit);
			attch->m_texture->bind();
			render_pass::setup_pass();
		}
		ren_target->set_draw_buffer(nsfb_object::att_color);
		dc->shdr->bind();
		dc->shdr->set_viewport(gl_state.current_viewport);
		dc->shdr->set_for_draw_call(dc);
		tbuffers->bind_buffers();
		dc->render();
		tbuffers->unbind_buffers();
	}
}

void culled_light_pass::render()
{
	for (uint32 i = 0; i < draw_calls->size(); ++i)
	{
		light_draw_call * dc = (light_draw_call*)(*draw_calls)[i];
		if (dc->cast_shadows)
		{
			rpass->ldc = dc;
			rpass->setup_pass();
			rpass->render();
			nsfb_object::attachment * attch = rpass->ren_target->att(nsfb_object::att_depth);
			nsrender_system::set_active_texture_unit(attch->m_tex_unit);
			attch->m_texture->bind();
			render_pass::setup_pass();
		}
		ren_target->set_draw_buffer(nsfb_object::att_none);
		stencil_shdr->bind();
		stencil_shdr->set_viewport(gl_state.current_viewport);
		stencil_shdr->set_for_draw_call(dc);
		dc->render();
		
		nsrender_system::enable_depth_test(false);
		nsrender_system::enable_culling(true);
		nsrender_system::set_cull_face(GL_FRONT);
		nsrender_system::set_stencil_func(GL_NOTEQUAL, 0, 0xFF);
		ren_target->set_draw_buffer(nsfb_object::att_color);
		dc->shdr->bind();
		dc->shdr->set_viewport(gl_state.current_viewport);
		dc->shdr->set_for_draw_call(dc);
		tbuffers->bind_buffers();
		dc->render();
		tbuffers->unbind_buffers();
	}
}

void final_render_pass::render()
{
	read_buffer->set_target(nsfb_object::fb_read);
	read_buffer->bind();
	read_buffer->set_read_buffer(nsfb_object::att_color);
	glBlitFramebuffer(
		read_vp.x,
		read_vp.y,
		read_vp.z,
		read_vp.w,
		gl_state.current_viewport.x,
		gl_state.current_viewport.y,
		gl_state.current_viewport.z,
		gl_state.current_viewport.w,
		GL_COLOR_BUFFER_BIT,
		GL_NEAREST);
	read_buffer->set_read_buffer(nsfb_object::att_none);
	read_buffer->unbind();	
}

nsrender_system::nsrender_system() :
	nssystem(),
	m_debug_draw(false),
	m_lighting_enabled(true),
	m_fog_nf(DEFAULT_FOG_FACTOR_NEAR, DEFAULT_FOG_FACTOR_FAR),
	m_fog_color(1),
	m_default_fbo(nullptr),
	m_default_mat(nullptr),
	m_single_point(new nsbuffer_object(nsbuffer_object::array, nsbuffer_object::storage_mutable))
{
	m_single_point->init_gl();
	m_all_draw_calls.reserve(MAX_INSTANCED_DRAW_CALLS);
	m_light_draw_calls.reserve(MAX_LIGHTS_IN_SCENE);
}

nsrender_system::~nsrender_system()
{
	m_single_point->release();
	delete m_single_point;
	delete m_default_fbo;
	clear_render_passes();
	clear_render_targets();
	clear_render_queues();
}

void nsrender_system::clear_render_queues()
{
	while (m_render_queues.begin() != m_render_queues.end())
		destroy_queue(m_render_queues.begin()->first);
	m_render_queues.clear();	
}

void nsrender_system::clear_render_targets()
{
	while (m_render_targets.begin() != m_render_targets.end())
		destroy_render_target(m_render_targets.begin()->first);
	m_render_targets.clear();	
}

void nsrender_system::clear_render_passes()
{
	while (m_render_passes.begin() != m_render_passes.end())
	{
		delete m_render_passes.back();
		m_render_passes.pop_back();
	}	
}

void nsrender_system::create_default_render_queues()
{
	create_queue(SCENE_OPAQUE_QUEUE)->reserve(MAX_GBUFFER_DRAWS);
	create_queue(SCENE_TRANSLUCENT_QUEUE)->reserve(MAX_GBUFFER_DRAWS);
	create_queue(SCENE_SELECTION_QUEUE)->reserve(MAX_GBUFFER_DRAWS);
	create_queue(DIR_LIGHT_QUEUE)->reserve(MAX_LIGHTS_IN_SCENE);
	create_queue(SPOT_LIGHT_QUEUE)->reserve(MAX_LIGHTS_IN_SCENE);
	create_queue(POINT_LIGHT_QUEUE)->reserve(MAX_LIGHTS_IN_SCENE);
}

void nsrender_system::create_default_render_targets(uint32 screen_fbo)
{
	// Create all of the framebuffers
	nsgbuf_object * gbuffer = new nsgbuf_object;
	gbuffer->init_gl();
	gbuffer->resize(DEFAULT_GBUFFER_RES_X, DEFAULT_GBUFFER_RES_Y);
	gbuffer->init();
	add_render_target(GBUFFER_TARGET, gbuffer);

	// Accumulation buffer
	nsfb_object * accum_buffer = new nsfb_object;
	accum_buffer->init_gl();
	accum_buffer->resize(DEFAULT_ACCUM_BUFFER_RES_X, DEFAULT_ACCUM_BUFFER_RES_Y);
	accum_buffer->init();
	accum_buffer->set_target(nsfb_object::fb_draw);
	accum_buffer->bind();
	accum_buffer->create<nstex2d>(
		"rendered_frame",
		nsfb_object::att_color,
		FINAL_TEX_UNIT,
		GL_RGBA8,
		GL_RGBA,
		GL_FLOAT);
	accum_buffer->add(gbuffer->depth());
	nsfb_object::attachment * att = accum_buffer->create<nstex2d>(
		"final_picking",
		nsfb_object::attach_point(nsfb_object::att_color + nsgbuf_object::col_picking),
		G_PICKING_TEX_UNIT,
		GL_RGB32UI,
		GL_RGB_INTEGER,
		GL_UNSIGNED_INT);
	if (att != nullptr)
	{
		att->m_texture->set_parameter_i(nstexture::min_filter, GL_NEAREST);
		att->m_texture->set_parameter_i(nstexture::mag_filter, GL_NEAREST);
	}
	accum_buffer->update_draw_buffers();
	add_render_target(ACCUM_TARGET, accum_buffer);

	// 2d shadow map targets
	nsshadow_tex2d_target * dir_shadow_target = new nsshadow_tex2d_target;
	dir_shadow_target->init_gl();
	dir_shadow_target->resize(DEFAULT_DIR_LIGHT_SHADOW_W, DEFAULT_DIR_LIGHT_SHADOW_H);
	dir_shadow_target->init();
	add_render_target(DIR_SHADOW2D_TARGET, dir_shadow_target);
	
	nsshadow_tex2d_target * spot_shadow_target = new nsshadow_tex2d_target;
	spot_shadow_target->init_gl();
	spot_shadow_target->resize(DEFAULT_SPOT_LIGHT_SHADOW_W, DEFAULT_SPOT_LIGHT_SHADOW_H);
	spot_shadow_target->init();
	add_render_target(SPOT_SHADOW2D_TARGET, spot_shadow_target);
	
	nsshadow_tex_cubemap_target * point_shadow_target = new nsshadow_tex_cubemap_target;
	point_shadow_target->init_gl();
	point_shadow_target->resize(DEFAULT_POINT_LIGHT_SHADOW_W, DEFAULT_POINT_LIGHT_SHADOW_H);
	point_shadow_target->init();
	add_render_target(POINT_SHADOW_TARGET, point_shadow_target);
	
	// final fbo
	m_default_fbo->set_gl_id(screen_fbo);
}

void nsrender_system::create_default_render_passes()
{
	// setup gbuffer geometry stage
	gbuffer_render_pass * gbuf_pass = new gbuffer_render_pass;
	gbuf_pass->draw_calls = queue(SCENE_OPAQUE_QUEUE);
	gbuf_pass->ren_target = render_target(GBUFFER_TARGET);
	gbuf_pass->gl_state.depth_test = true;
	gbuf_pass->gl_state.depth_write = true;
	gbuf_pass->gl_state.culling = true;
	gbuf_pass->gl_state.cull_face = GL_BACK;
	gbuf_pass->gl_state.blending = false;
	gbuf_pass->gl_state.stencil_test = false;
	gbuf_pass->gl_state.clear_mask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;

	// setup gbuffer geometry stage
	oit_render_pass * oit_pass = new oit_render_pass;
	oit_pass->tbuffers = &m_tbuffers;
	oit_pass->single_point = m_single_point;
	oit_pass->sort_shader = m_shaders.frag_sort;
	oit_pass->draw_calls = queue(SCENE_TRANSLUCENT_QUEUE);
	oit_pass->ren_target = render_target(ACCUM_TARGET);
	oit_pass->gl_state.depth_test = true;
	oit_pass->gl_state.depth_write = false;
	oit_pass->gl_state.culling = true;
	oit_pass->gl_state.cull_face = GL_BACK;
	oit_pass->gl_state.blending = false;
	oit_pass->gl_state.stencil_test = false;

	// setup dir light shadow stage
	light_shadow_pass * dir_shadow_pass = new light_shadow_pass;
	dir_shadow_pass->draw_calls = queue(SCENE_OPAQUE_QUEUE);
	dir_shadow_pass->ren_target = render_target(DIR_SHADOW2D_TARGET);
	dir_shadow_pass->shdr = m_shaders.shadow_2d;
	dir_shadow_pass->gl_state.depth_test = true;
	dir_shadow_pass->gl_state.depth_write = true;
	dir_shadow_pass->gl_state.culling = true;
	dir_shadow_pass->gl_state.cull_face = GL_BACK;
	dir_shadow_pass->gl_state.blending = false;
	dir_shadow_pass->gl_state.stencil_test = false;
	dir_shadow_pass->gl_state.clear_mask = GL_DEPTH_BUFFER_BIT;
	dir_shadow_pass->enabled = false;

	// setup dir light stage
	light_pass * dir_pass = new light_pass;
	dir_pass->draw_calls = queue(DIR_LIGHT_QUEUE);
	dir_pass->ren_target = render_target(ACCUM_TARGET);
	dir_pass->gl_state.depth_test = false;
	dir_pass->gl_state.depth_write = false;
	dir_pass->gl_state.culling = true;
	dir_pass->gl_state.cull_face = GL_BACK;
	dir_pass->gl_state.blending = false;
	dir_pass->gl_state.stencil_test = false;
	dir_pass->tbuffers = &m_tbuffers;
	dir_pass->rpass = dir_shadow_pass;
	
	light_shadow_pass * spot_shadow_pass = new light_shadow_pass;
	spot_shadow_pass->draw_calls = queue(SCENE_OPAQUE_QUEUE);
	spot_shadow_pass->ren_target = render_target(SPOT_SHADOW2D_TARGET);
	spot_shadow_pass->shdr = m_shaders.shadow_2d;
	spot_shadow_pass->enabled = false;
	spot_shadow_pass->gl_state.depth_test = true;
	spot_shadow_pass->gl_state.depth_write = true;
	spot_shadow_pass->gl_state.culling = true;
	spot_shadow_pass->gl_state.cull_face = GL_BACK;
	spot_shadow_pass->gl_state.blending = false;
	spot_shadow_pass->gl_state.stencil_test = false;
	spot_shadow_pass->gl_state.clear_mask = GL_DEPTH_BUFFER_BIT;

	culled_light_pass * spot_pass = new culled_light_pass;
	spot_pass->draw_calls = queue(SPOT_LIGHT_QUEUE);
	spot_pass->ren_target = render_target(ACCUM_TARGET);
	spot_pass->stencil_shdr = m_shaders.light_stencil;
	spot_pass->tbuffers = &m_tbuffers;
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

	light_shadow_pass * point_shadow_pass = new light_shadow_pass;
	point_shadow_pass->draw_calls = queue(SCENE_OPAQUE_QUEUE);
	point_shadow_pass->ren_target = render_target(POINT_SHADOW_TARGET);
	point_shadow_pass->shdr = m_shaders.shadow_cube;
	point_shadow_pass->gl_state.depth_test = true;
	point_shadow_pass->gl_state.depth_write = true;
	point_shadow_pass->gl_state.culling = true;
	point_shadow_pass->gl_state.cull_face = GL_BACK;
	point_shadow_pass->gl_state.blending = false;
	point_shadow_pass->gl_state.stencil_test = false;
	point_shadow_pass->gl_state.clear_mask = GL_DEPTH_BUFFER_BIT;
	point_shadow_pass->enabled = false;

	culled_light_pass * point_pass = new culled_light_pass;
	point_pass->draw_calls = queue(POINT_LIGHT_QUEUE);
	point_pass->ren_target = render_target(ACCUM_TARGET);
	point_pass->stencil_shdr = m_shaders.light_stencil;
	point_pass->tbuffers = &m_tbuffers;
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

	selection_render_pass * sel_pass_opaque = new selection_render_pass;
	sel_pass_opaque->draw_calls = queue(SCENE_SELECTION_QUEUE);
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

	final_render_pass * final_pass = new final_render_pass;
	final_pass->ren_target = m_default_fbo;
	final_pass->read_buffer = render_target(ACCUM_TARGET);
	final_pass->read_vp = ivec4(0,0,render_target(ACCUM_TARGET)->size());
	final_pass->gl_state.depth_test = false;
	final_pass->gl_state.depth_write = false;
	final_pass->gl_state.culling = false;
	final_pass->gl_state.blending = false;
	final_pass->gl_state.stencil_test = false;
	final_pass->gl_state.cull_face = GL_BACK;
	
	m_render_passes.push_back(gbuf_pass);
	m_render_passes.push_back(oit_pass);
	m_render_passes.push_back(dir_shadow_pass);
	m_render_passes.push_back(dir_pass);
	m_render_passes.push_back(spot_shadow_pass);
	m_render_passes.push_back(spot_pass);
	m_render_passes.push_back(point_shadow_pass);
	m_render_passes.push_back(point_pass);
	m_render_passes.push_back(sel_pass_opaque);
	m_render_passes.push_back(final_pass);
}

render_pass_vector * nsrender_system::render_passes()
{
	return &m_render_passes;
}

bool nsrender_system::add_render_target(const nsstring & name, nsfb_object * rt)
{
	return m_render_targets.emplace(name, rt).second;
}

nsfb_object * nsrender_system::remove_render_target(const nsstring & name)
{
	nsfb_object * fb = nullptr;
	auto iter = m_render_targets.find(name);
	if (iter != m_render_targets.end())
	{
		fb = iter->second;
		m_render_targets.erase(iter);
	}
	return fb;
}

nsfb_object * nsrender_system::render_target(const nsstring & name)
{
	auto iter = m_render_targets.find(name);
	if (iter != m_render_targets.end())
		return iter->second;
	return nullptr;
}

void nsrender_system::destroy_render_target(const nsstring & name)
{
	nsfb_object * rt = remove_render_target(name);
	rt->release();
	delete rt;
}

// system takes ownership and will delete on shutdown
bool nsrender_system::add_queue(const nsstring & name, drawcall_queue * q)
{
	return m_render_queues.emplace(name, q).second;	
}

drawcall_queue * nsrender_system::create_queue(const nsstring & name)
{
	drawcall_queue * q = new drawcall_queue;
	if (!add_queue(name,q))
	{
		delete q;
		q = nullptr;
	}
	return q;
}

drawcall_queue * nsrender_system::queue(const nsstring & name)
{
	auto iter = m_render_queues.find(name);
	if (iter != m_render_queues.end())
		return iter->second;
	return nullptr;	
}

drawcall_queue * nsrender_system::remove_queue(const nsstring & name)
{
	drawcall_queue * q = nullptr;
	auto iter = m_render_queues.find(name);
	if (iter != m_render_queues.end())
	{
		q = iter->second;
		m_render_queues.erase(iter);
	}
	return q;
}

void nsrender_system::destroy_queue(const nsstring & name)
{
	drawcall_queue * q = remove_queue(name);
	delete q;		
}

void nsrender_system::enable_lighting(bool pEnable)
{
	m_lighting_enabled = pEnable;
}

void nsrender_system::render()
{
	if (!_valid_check())
		return;

	for (uint32 i = 0; i < m_render_passes.size(); ++i)
	{
		render_pass * rp = m_render_passes[i];

		if (rp == nullptr || rp->ren_target == nullptr)
		{
			dprint("nsrender_system::render - Warning render pass is not correctly setup");
			continue;
		}
		if (rp->enabled)
		{
			rp->setup_pass();
			rp->render();
		}
	}
	m_tbuffers.reset_atomic_counter();
}

void nsrender_system::set_gl_state(const opengl_state & state)
{
	set_viewport(state.current_viewport);
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

void nsrender_system::set_blend_eqn(int32 eqn)
{
	if (eqn == 0)
		return;
	
	glBlendEquation(eqn);
	m_gl_state.blend_eqn = eqn;
	gl_err_check("nsrender_system::set_blend_eqn");	
}

void nsrender_system::setup_default_rendering(uint32 final_fbo)
{
	create_default_render_queues();
	create_default_render_targets(final_fbo);
	create_default_render_passes();
}

bool nsrender_system::_valid_check()
{
	static bool shadererr = false;
	static bool shadernull = false;
	static bool defmaterr = false;

	if (!m_shaders.valid())
	{
		if (!shadernull)
		{
			dprint("nsrender_system::_validCheck: Error - one of the rendering shaders is nullptr ");
			shadernull = true;
		}
		return false;
	}
	shadernull = false;

	if (m_default_mat == nullptr)
	{
		if (!defmaterr)
		{
			dprint("nsrender_system::_validCheck Error - deflt material is nullptr - assign deflt material to render system");
			defmaterr = true;
		}
		return false;
	}
	defmaterr = false;

	if (m_shaders.error())
	{
		if (!shadererr)
		{
			dprint("nsrender_system::_validCheck: Error in one of the rendering shaders - check compile log");
			shadererr = true;
		}
		return false;
	}
	shadererr = false;
	return true;
}

//void nsrender_system::render_scene_particles(nsscene * scene, nsentity * cam)
//{
	// nscam_comp * compc = cam->get<nscam_comp>();
	// nstform_comp * camTComp = cam->get<nstform_comp>();

	// auto comps = scene->entities<nsparticle_comp>();
	// auto ent_iter = comps.begin();
	// while (ent_iter != comps.end())
	// {
	// 	nsparticle_comp * comp = (*ent_iter)->get<nsparticle_comp>();
	// 	if (comp->first() || !comp->simulating())
	// 	{
	// 		++ent_iter;
	// 		continue;
	// 	}
		
	// 	nsmaterial * mat = nse.resource<nsmaterial>(comp->material_id());
	// 	if (mat == nullptr)
	// 		mat = m_default_mat;

	// 	nsparticle_render_shader * rshdr = nse.resource<nsparticle_render_shader>(mat->shader_id());
	// 	if (rshdr == nullptr)
	// 		rshdr = m_shaders.deflt_particle;

	// 	nstform_comp * tComp = (*ent_iter)->get<nstform_comp>();
	// 	nstform_comp * camTComp = scene->camera()->get<nstform_comp>();
	// 	nstexture * tex = nse.resource<nstexture>(mat->map_tex_id(nsmaterial::diffuse));
	// 	if (tex != nullptr)
	// 	{
	// 		set_active_texture_unit(nsmaterial::diffuse);
	// 		tex->bind();
	// 	}
	// 	else
	// 		dprint("nsparticle_system::draw() - No random texture set - particles will be lame.");

	// 	if (comp->blend_mode() == nsparticle_comp::b_mix)
	// 		set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// 	else
	// 		set_blend_func(GL_ADD, GL_ADD);

	// 	rshdr->bind();
	// 	rshdr->set_proj_cam_mat(compc->proj_cam());
	// 	rshdr->set_cam_right(camTComp->dvec(nstform_comp::dir_right));
	// 	rshdr->set_cam_up(camTComp->dvec(nstform_comp::dir_up));
	// 	rshdr->set_cam_target(camTComp->dvec(nstform_comp::dir_target));
	// 	rshdr->set_world_up(fvec3(0.0f, 0.0f, 1.0f));
	// 	rshdr->set_diffusemap_enabled(mat->contains(nsmaterial::diffuse));
	// 	rshdr->set_color_mode(mat->color_mode());
	// 	rshdr->set_frag_color_out(mat->color());
	// 	rshdr->set_lifetime(float(comp->lifetime()) / 1000.0f);
	// 	rshdr->set_blend_mode(uint32(comp->blend_mode()));

		
	// 	comp->va_obj()->bind();
	// 	tComp->transform_buffer()->bind();
	// 	for (uint32 tfInd = 0; tfInd < 4; ++tfInd)
	// 	{
	// 		comp->va_obj()->add(tComp->transform_buffer(), 4 + tfInd);
	// 		comp->va_obj()->vertex_attrib_ptr(4 + tfInd, 4, GL_FLOAT, GL_FALSE, sizeof(fmat4), sizeof(fvec4)*tfInd);
	// 		comp->va_obj()->vertex_attrib_div(4 + tfInd, 1);
	// 	}

	// 	glDrawTransformFeedbackInstanced(GL_POINTS, comp->xfb_id(),tComp->count());
	// 	gl_err_check("nsparticle_system::draw in glDrawElementsInstanced");
	// 	tComp->transform_buffer()->bind();
	// 	comp->va_obj()->remove(tComp->transform_buffer());
	// 	comp->va_obj()->unbind();
	// 	rshdr->unbind();
	// 	++ent_iter;
	// }
//}

opengl_state nsrender_system::m_gl_state = opengl_state();

void nsrender_system::enable_depth_write(bool enable)
{
	glDepthMask(enable);
	m_gl_state.depth_write = enable;
	gl_err_check("nsrender_system::enable_depth_write");
}

void nsrender_system::enable_depth_test(bool enable)
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
	gl_err_check("nsrender_system::enable_depth_test");
	m_gl_state.depth_test = enable;
}

void nsrender_system::enable_stencil_test(bool enable)
{
	if (enable)
		glEnable(GL_STENCIL_TEST);
	else
		glDisable(GL_STENCIL_TEST);
	gl_err_check("nsrender_system::enable_stencil_test");
	m_gl_state.stencil_test = enable;
}

void nsrender_system::enable_blending(bool enable)
{
	if (enable)
		glEnable(GL_BLEND);
	else
		glDisable(GL_BLEND);
	m_gl_state.blending = enable;
	gl_err_check("nsrender_system::enable_blending");
}

void nsrender_system::enable_culling(bool enable)
{
	if (enable)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);
	m_gl_state.culling = enable;
	gl_err_check("nsrender_system::enable_culling");
}

void nsrender_system::set_cull_face(int32 cull_face)
{
	if (cull_face != m_gl_state.cull_face)
	{
		glCullFace(cull_face);
		m_gl_state.cull_face = cull_face;
		gl_err_check("nsrender_system::set_cull_face");
	}	
}

opengl_state nsrender_system::current_gl_state()
{
	return m_gl_state;
}

void nsrender_system::set_blend_func(int32 sfactor, int32 dfactor)
{
	set_blend_func(ivec2(sfactor,dfactor));
}

void nsrender_system::set_blend_func(const ivec2 & blend_func)
{
	glBlendFunc(blend_func.x, blend_func.y);
	m_gl_state.blend_func = blend_func;
	gl_err_check("nsrender_system::set_blend_func");
}

void nsrender_system::set_stencil_func(int32 func, int32 ref, int32 mask)
{
	set_stencil_func(ivec3(func,ref,mask));
}

void nsrender_system::set_stencil_func(const ivec3 & stencil_func)
{
	glStencilFunc(stencil_func.x, stencil_func.y, stencil_func.z);
	m_gl_state.stencil_func = stencil_func;
	gl_err_check("nsrender_system::set_stencil_func");
}

void nsrender_system::set_stencil_op(int32 sfail, int32 dpfail, int32 dppass)
{
	set_stencil_op(ivec3( sfail, dpfail, dppass));
}

void nsrender_system::set_stencil_op(const ivec3 & stencil_op)
{
	glStencilOp(stencil_op.x, stencil_op.y, stencil_op.z);
	m_gl_state.stencil_op_back = stencil_op;
	m_gl_state.stencil_op_front = stencil_op;
	gl_err_check("nsrender_system::set_stencil_op");
}

void nsrender_system::set_stencil_op_back(int32 sfail, int32 dpfail, int32 dppass)
{
	set_stencil_op_back(ivec3(sfail, dpfail, dppass));	
}
	
void nsrender_system::set_stencil_op_back(const ivec3 & stencil_op)
{
	glStencilOpSeparate(GL_BACK, stencil_op.x, stencil_op.y, stencil_op.z);
	m_gl_state.stencil_op_back = stencil_op;
	gl_err_check("nsrender_system::set_stencil_op_back");
}

void nsrender_system::set_stencil_op_front(int32 sfail, int32 dpfail, int32 dppass)
{
	set_stencil_op_front(ivec3(sfail, dpfail, dppass));		
}
	
void nsrender_system::set_stencil_op_front(const ivec3 & stencil_op)
{
	glStencilOpSeparate(GL_FRONT, stencil_op.x, stencil_op.y, stencil_op.z);
	m_gl_state.stencil_op_front = stencil_op;
	gl_err_check("nsrender_system::set_stencil_op_front");
}

void nsrender_system::_prepare_tforms(nsscene * scene)
{
	auto ent_iter = scene->entities().begin();
	while (ent_iter != scene->entities().end())
	{
		nstform_comp * tForm = (*ent_iter)->get<nstform_comp>();
		if (tForm->update_posted())
		{
			nsbuffer_object & tFormBuf = *tForm->transform_buffer();
			nsbuffer_object & tFormIDBuf = *tForm->transform_id_buffer();

			if (tForm->buffer_resized())
			{
				tFormBuf.bind();
				tFormBuf.allocate<fmat4>(nsbuffer_object::mutable_dynamic_draw, tForm->count());
				tFormIDBuf.bind();
				tFormIDBuf.allocate<uint32>(nsbuffer_object::mutable_dynamic_draw, tForm->count());
			}

			tFormBuf.bind();
			fmat4 * mappedT = tFormBuf.map<fmat4>(0, tForm->count(),
											   nsbuffer_object::access_map_range(nsbuffer_object::map_write));
			tFormIDBuf.bind();
			uint32 * mappedI = tFormIDBuf.map<uint32>(0, tForm->count(), nsbuffer_object::access_map_range(nsbuffer_object::map_write));
			tFormIDBuf.unbind();

			uint32 visibleCount = 0;
			for (uint32 i = 0; i < tForm->count(); ++i)
			{
				if (tForm->transform_update(i))
				{
					uivec3 parentID = tForm->parent_id(i);
					if (parentID != 0)
					{
						nsentity * ent = scene->entity(parentID.x, parentID.y);
						if (ent != nullptr)
						{
							nstform_comp * tComp2 = ent->get<nstform_comp>();
							if (parentID.z < tComp2->count())
								tForm->set_parent(tComp2->transform(parentID.z));
						}
					}

					tForm->compute_transform(i);
					tForm->set_instance_update(false, i);
				}
				int32 state = tForm->hidden_state(i);

				bool layerBit = state & nstform_comp::hide_layer && true;
				bool objectBit = state & nstform_comp::hide_object && true;
				bool showBit = state & nstform_comp::hide_none && true;
				bool hideBit = state & nstform_comp::hide_all && true;

				if (!hideBit && (!layerBit && (showBit || !objectBit)))
				{
					mappedT[visibleCount] = tForm->transform(i);
					mappedI[visibleCount] = i;
					++visibleCount;
				}
			}
			tForm->set_visible_instance_count(visibleCount);


			tFormBuf.bind();
			tFormBuf.unmap();
			tFormIDBuf.bind();
			tFormIDBuf.unmap();
			tFormIDBuf.unbind();
			tForm->post_update(tForm->buffer_resized());
			tForm->set_buffer_resize(false);
		}
		++ent_iter;
	}
}

uint32 nsrender_system::bound_fbo()
{
	GLint params;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &params);
	return params;
}

uint32 nsrender_system::active_tex_unit()
{
	GLint act_un;
	glGetIntegerv(GL_ACTIVE_TEXTURE, &act_un);
	return act_un - BASE_TEX_UNIT;
}

bool nsrender_system::debug_draw()
{
	return m_debug_draw;
}

nsmaterial * nsrender_system::default_mat()
{
	return m_default_mat;
}

void nsrender_system::init()
{
	register_handler_func(this, &nsrender_system::_handle_window_resize);

	// GL setup
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
	m_default_fbo = new nsfb_object;
	m_default_fbo->set_gl_id(0);
	
	// Load default shaders
	nsplugin * cplg = nse.core();	
    nsstring shext = nsstring(DEFAULT_SHADER_EXTENSION);
	m_shaders.deflt = cplg->load<nsmaterial_shader>(nsstring(GBUFFER_SHADER) + shext);
	m_shaders.deflt_wireframe = cplg->load<nsmaterial_shader>(nsstring(GBUFFER_WF_SHADER) + shext);
	m_shaders.deflt_translucent = cplg->load<nsmaterial_shader>(nsstring(GBUFFER_TRANS_SHADER) + shext);
	m_shaders.light_stencil = cplg->load<nslight_stencil_shader>(nsstring(LIGHTSTENCIL_SHADER) + shext);
	m_shaders.frag_sort = cplg->load<nsfragment_sort_shader>(nsstring(FRAGMENT_SORT_SHADER) + shext);
	m_shaders.dir_light = cplg->load<nsdir_light_shader>(nsstring(DIR_LIGHT_SHADER) + shext);
	m_shaders.point_light = cplg->load<nspoint_light_shader>(nsstring(POINT_LIGHT_SHADER) + shext);
	m_shaders.spot_light = cplg->load<nsspot_light_shader>(nsstring(SPOT_LIGHT_SHADER) + shext);
	m_shaders.shadow_cube = cplg->load<nsshadow_cubemap_shader>(nsstring(POINT_SHADOWMAP_SHADER) + shext);
	m_shaders.shadow_2d = cplg->load<nsshadow_2dmap_shader>(nsstring(SPOT_SHADOWMAP_SHADER) + shext);
	m_shaders.sel_shader = cplg->load<nsselection_shader>(nsstring(SELECTION_SHADER) + shext);
	m_shaders.deflt_particle = cplg->load<nsparticle_render_shader>(nsstring(RENDER_PARTICLE_SHADER) + shext);
	cplg->load<nsmaterial_shader>(nsstring(SKYBOX_SHADER) + shext);
	cplg->manager<nsshader_manager>()->compile_all();
	cplg->manager<nsshader_manager>()->link_all();
	cplg->manager<nsshader_manager>()->init_uniforms_all();

	// Default material
	nstexture * tex = cplg->load<nstex2d>(nsstring(DEFAULT_MATERIAL) + nsstring(DEFAULT_TEX_EXTENSION));
	m_default_mat = cplg->load<nsmaterial>(nsstring(DEFAULT_MATERIAL) + nsstring(DEFAULT_MAT_EXTENSION));

	// Light bounds, skydome, and tile meshes
    cplg->load<nsmesh>(nsstring(MESH_FULL_TILE) + nsstring(DEFAULT_MESH_EXTENSION));
    cplg->load<nsmesh>(nsstring(MESH_TERRAIN) + nsstring(DEFAULT_MESH_EXTENSION));
    cplg->load<nsmesh>(nsstring(MESH_HALF_TILE) + nsstring(DEFAULT_MESH_EXTENSION));
	cplg->load<nsmesh>(nsstring(MESH_POINTLIGHT_BOUNDS) + nsstring(DEFAULT_MESH_EXTENSION));
	cplg->load<nsmesh>(nsstring(MESH_SPOTLIGHT_BOUNDS) + nsstring(DEFAULT_MESH_EXTENSION));
	cplg->load<nsmesh>(nsstring(MESH_DIRLIGHT_BOUNDS) + nsstring(DEFAULT_MESH_EXTENSION));
	cplg->load<nsmesh>(nsstring(MESH_SKYDOME) + nsstring(DEFAULT_MESH_EXTENSION));

	uint32 data_ = 0;
	uint32 sz = DEFAULT_ACCUM_BUFFER_RES_X*DEFAULT_ACCUM_BUFFER_RES_Y;
	i_vector header_data(sz, -1);
	m_tbuffers.atomic_counter->bind();
	m_tbuffers.atomic_counter->allocate(data_, nsbuffer_object::mutable_dynamic_draw);
	m_tbuffers.atomic_counter->unbind();
	m_tbuffers.header->bind();
	m_tbuffers.header->allocate(header_data, nsbuffer_object::mutable_dynamic_draw, header_data.size());
	m_tbuffers.fragments->bind();
	m_tbuffers.fragments->allocate<packed_fragment_data>(nsbuffer_object::mutable_dynamic_draw, sz * 2);
	m_tbuffers.fragments->unbind();

	m_single_point->bind();
	fvec3 point;
	m_single_point->allocate(point, nsbuffer_object::mutable_dynamic_draw);
	m_single_point->unbind();
}

void nsrender_system::enable_debug_render(bool pDebDraw)
{
	clear_render_passes();
}

void nsrender_system::set_active_texture_unit(uint32 tex_unit)
{
	glActiveTexture(BASE_TEX_UNIT + tex_unit);
	gl_err_check("nsrender_system::set_active_texture_unit");
}

void nsrender_system::set_default_mat(nsmaterial * pDefMat)
{
	m_default_mat = pDefMat;
}

void nsrender_system::toggle_debug_draw()
{
	enable_debug_render(!m_debug_draw);
}

void nsrender_system::update()
{
	nsscene * scene = nse.current_scene();
	if (scene == nullptr)
		return;

	nsentity * cam = scene->camera();
	if (cam == nullptr)
		return;

	_prepare_tforms(scene);

	m_mat_shader_ids.clear();
	m_all_draw_calls.resize(0);
	m_light_draw_calls.resize(0);
	add_draw_calls_from_scene(scene);
	add_lights_from_scene(scene);
}

void nsrender_system::add_lights_from_scene(nsscene * scene)
{
	auto l_iter = scene->entities<nslight_comp>().begin();
	drawcall_queue * dc_dq = queue(DIR_LIGHT_QUEUE);
	drawcall_queue * dc_sq = queue(SPOT_LIGHT_QUEUE);
	drawcall_queue * dc_pq = queue(POINT_LIGHT_QUEUE);
	dc_dq->resize(0);
	dc_sq->resize(0);
	dc_pq->resize(0);
	
	while (l_iter != scene->entities<nslight_comp>().end())
	{
		nslight_comp * lcomp = (*l_iter)->get<nslight_comp>();
		nstform_comp * tcomp = (*l_iter)->get<nstform_comp>();
		nstform_comp * camt = scene->camera()->get<nstform_comp>();
		nsmesh * boundingMesh = nse.resource<nsmesh>(lcomp->mesh_id());
		
		m_light_draw_calls.resize(m_light_draw_calls.size()+1);

		fmat4 proj;
		if (lcomp->type() == nslight_comp::l_spot)
			proj = perspective(2*lcomp->angle(), 1.0f, lcomp->shadow_clipping().x, lcomp->shadow_clipping().y);
		else if(lcomp->type() == nslight_comp::l_point)
			proj = perspective(90.0f, 1.0f, lcomp->shadow_clipping().x, lcomp->shadow_clipping().y);
		else
			proj = ortho(-100.0f,100.0f,100.0f,-100.0f,100.0f,-100.0f);

		for (uint32 i = 0; i < lcomp->transform_count(); ++i)
		{
			light_draw_call * ldc = &m_light_draw_calls[m_light_draw_calls.size()-1];

			ldc->submesh = nullptr;
			ldc->draw_point = nullptr;
			ldc->proj_cam_mat = scene->camera()->get<nscam_comp>()->proj_cam();
			ldc->lighting_enabled = m_lighting_enabled;
			ldc->bg_color = scene->bg_color();
			ldc->direction = lcomp->transform(i).target();
			ldc->cam_world_pos = camt->wpos();
			ldc->fog_color = m_fog_color;
			ldc->fog_factor = m_fog_nf;
			ldc->diffuse_intensity = lcomp->intensity().x;
			ldc->ambient_intensity = lcomp->intensity().y;
			ldc->cast_shadows = lcomp->cast_shadows();
			ldc->light_color = lcomp->color();
			ldc->shadow_samples = lcomp->shadow_samples();
			ldc->shadow_darkness = lcomp->shadow_darkness();
			ldc->material_ids = &m_mat_shader_ids;
			ldc->spot_atten = lcomp->atten();
			ldc->light_pos = tcomp->wpos(i);
			ldc->light_transform = lcomp->transform(i);
			ldc->max_depth = lcomp->shadow_clipping().y - lcomp->shadow_clipping().x;
			if (lcomp->type() == nslight_comp::l_dir)
			{
				ldc->draw_point = m_single_point;
				ldc->proj_light_mat = proj * lcomp->pov(i);
				ldc->shdr = m_shaders.dir_light;
				ldc->shadow_tex_size = render_target(DIR_SHADOW2D_TARGET)->size();
				dc_dq->push_back(ldc);
			}
			else if (lcomp->type() == nslight_comp::l_spot)
			{
				ldc->shadow_tex_size = render_target(SPOT_SHADOW2D_TARGET)->size();
				ldc->shdr = m_shaders.spot_light;
				ldc->proj_light_mat = proj * lcomp->pov(i);
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
				ldc->shadow_tex_size = render_target(POINT_SHADOW_TARGET)->size();
				ldc->shdr = m_shaders.point_light;
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
		}
		++l_iter;
	}
}

void nsrender_system::add_draw_calls_from_scene(nsscene * scene)
{
	nsentity * camera = scene->camera();

	// update render components and the draw call list
	drawcall_queue * scene_dcq = queue(SCENE_OPAQUE_QUEUE);
	drawcall_queue * scene_tdcq = queue(SCENE_TRANSLUCENT_QUEUE);
	drawcall_queue * scene_sel = queue(SCENE_SELECTION_QUEUE);
	scene_dcq->resize(0);
	scene_tdcq->resize(0);
	scene_sel->resize(0);
	
	nscam_comp * cc = camera->get<nscam_comp>();	
	fvec2 terh;
	nsrender_comp * rComp = nullptr;
	nstform_comp * tComp = nullptr;
	nsanim_comp * animComp = nullptr;
	nsterrain_comp * terComp = nullptr;
	nsmesh * currentMesh = nullptr;
	nslight_comp * lc = nullptr;
	nssel_comp * sc = nullptr;
	bool transparent_picking = false;
	uint32 mat_id = 0;
	auto iter = scene->entities<nsrender_comp>().begin();
	while (iter != scene->entities<nsrender_comp>().end())
	{
		terh.set(0.0f, 1.0f);
		rComp = (*iter)->get<nsrender_comp>();
		tComp = (*iter)->get<nstform_comp>();
		lc = (*iter)->get<nslight_comp>();
		sc = (*iter)->get<nssel_comp>();
		animComp = (*iter)->get<nsanim_comp>();
		terComp = (*iter)->get<nsterrain_comp>();
		currentMesh = nse.resource<nsmesh>(rComp->mesh_id());
		
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

		if (sc != nullptr)
		{
			transparent_picking = sc->transparent_picking_enabled();
		}
		
		nsmesh::submesh * mSMesh = nullptr;
		nsmaterial * mat = nullptr;
		fmat4_vector * fTForms = nullptr;
		nsmaterial_shader * shader = nullptr;
		for (uint32 i = 0; i < currentMesh->count(); ++i)
		{
			mSMesh = currentMesh->sub(i);
			mat = nse.resource<nsmaterial>(rComp->material_id(i));

			if (mat == nullptr)
				mat = m_default_mat;

			shader = nse.resource<nsmaterial_shader>(mat->shader_id());
			fTForms = nullptr;

			if (shader == nullptr)
			{
				if (mat->wireframe())
					shader = m_shaders.deflt_wireframe;
				else if (mat->alpha_blend())
					shader = m_shaders.deflt_translucent;
				else
					shader = m_shaders.deflt;
			}

			if (animComp != nullptr)
				fTForms = animComp->final_transforms();

			if (terComp != nullptr)
				terh = terComp->height_bounds();

			
			if (tComp != nullptr)
			{
				m_all_draw_calls.resize(m_all_draw_calls.size()+1);
				instanced_draw_call * dc = &m_all_draw_calls[m_all_draw_calls.size()-1];
				dc->submesh = mSMesh;
				dc->transform_buffer = tComp->transform_buffer();
				dc->transform_id_buffer = tComp->transform_id_buffer();
				dc->anim_transforms = fTForms;
				dc->proj_cam = cc->proj_cam();
				dc->height_minmax = terh;
				dc->entity_id = (*iter)->id();
				dc->plugin_id = (*iter)->plugin_id();
				dc->transform_count = tComp->visible_count();
				dc->casts_shadows = rComp->cast_shadow();
				dc->transparent_picking = false;
				dc->mat_index = mat_id;
				dc->mat = mat;
				dc->shdr = shader;
				if (mat->alpha_blend())
				{
					dc->transparent_picking = transparent_picking;
					scene_tdcq->push_back(dc);
				}
				else
				{
					scene_dcq->push_back(dc);
				}

				if (sc != nullptr && sc->selected())
				{
					m_all_draw_calls.resize(m_all_draw_calls.size()+1);
					instanced_draw_call * sel_dc = &m_all_draw_calls[m_all_draw_calls.size()-1];
					sel_dc->submesh = mSMesh;
					sel_dc->transform_buffer = sc->transform_buffer();
					sel_dc->transform_id_buffer = nullptr;
					sel_dc->anim_transforms = fTForms;
					sel_dc->proj_cam = cc->proj_cam();
					sel_dc->height_minmax = terh;
					sel_dc->transform_count = sc->count();
					sel_dc->shdr = m_shaders.sel_shader;
					sel_dc->mat = mat;
					sel_dc->sel_color = sc->color();
					scene_sel->push_back(sel_dc);
				}

				auto inserted = m_mat_shader_ids.emplace(mat, mat_id);
				if (inserted.second)
					++mat_id;
			}
 		}
		
		++iter;
	}
}

nsfb_object * nsrender_system::default_fbo()
{
	return m_default_fbo;
}

void nsrender_system::set_viewport(const ivec4 & val)
{
	if (m_gl_state.current_viewport != val)
	{
		glViewport(val.x, val.y, val.z, val.w);
		m_gl_state.current_viewport = val;
		gl_err_check("nsrender_system::set_viewport");
	}
}

const uivec2 & nsrender_system::fog_factor()
{
	return m_fog_nf;
}

void nsrender_system::set_fog_factor(const uivec2 & near_far)
{
	m_fog_nf = near_far;
}

const fvec4 & nsrender_system::fog_color()
{
	return m_fog_color;
}

void nsrender_system::set_fog_color(const fvec4 & color)
{
	m_fog_color = color;
}

void nsrender_system::clear_framebuffer(uint32 clear_mask)
{
	glClear(clear_mask);
	gl_err_check("nsrender_system::clear_framebuffer");
}

int32 nsrender_system::update_priority()
{
	return RENDER_SYS_UPDATE_PR;
}

void nsrender_system::bind_textures(nsmaterial * material)
{
	nsmaterial::texmap_map_const_iter cIter = material->begin();
	while (cIter != material->end())
	{
		nstexture * t = nse.resource<nstexture>(cIter->second);
		if (t != nullptr)
		{
			set_active_texture_unit(cIter->first);
			t->bind();
		}
		++cIter;
	}	
}

// TODO: Move the gbuffer thing in to a material and use bind material textures instead
void nsrender_system::bind_gbuffer_textures(nsfb_object * fb)
{
	nsgbuf_object * obj = dynamic_cast<nsgbuf_object*>(fb);

	if (obj == nullptr)
		return;

	for (uint32 i = 0; i < nsgbuf_object::attrib_count; ++i)
	{
		nsfb_object::attachment * att = obj->color(i);
		set_active_texture_unit(att->m_tex_unit);
		att->m_texture->bind();
	}
}

bool nsrender_system::_handle_window_resize(window_resize_event * evt)
{
	m_default_fbo->resize(evt->new_size);
	
	nsscene * scn = nse.current_scene();
	if (scn != nullptr)
	{
		nsentity * cam = scn->camera();
		if (cam != nullptr)
		{
			nscam_comp * cc = cam->get<nscam_comp>();
			cc->resize_screen(evt->new_size);
		}
	}
	return true;
}
