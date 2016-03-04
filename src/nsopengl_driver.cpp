/*!
  \file nsopengl_driver.cpp

  \brief Definition file for nsopengl_driver class

  This file contains all of the neccessary definitions for the nsopengl_driver class.

  \author Daniel Randle
  \date Feb 23 2016
  \copywrite Earth Banana Games 2013
*/

#include <nsanim_comp.h>
#include <nsterrain_comp.h>
#include <nssel_comp.h>
#include <nsplugin.h>
#include <nsrender_system.h>
#include <nsopengl_driver.h>
#include <nsshader.h>
#include <nsshadowbuf_object.h>
#include <nsgbuf_object.h>
#include <nsmaterial.h>
#include <nsscene.h>
#include <nscam_comp.h>
#include <nsshader_manager.h>
#include <nslight_comp.h>
#include <nsrender_comp.h>

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

instanced_draw_call::instanced_draw_call():
	submesh(nullptr),
	transform_buffer(nullptr),
	transform_id_buffer(nullptr),
	anim_transforms(nullptr),
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

void render_pass::setup_pass()
{
	nsfb_object * tgt = (nsfb_object*)ren_target;
	tgt->set_target(nsfb_object::fb_draw);
	tgt->bind();

	if (use_vp_size && vp != nullptr)
	{
		gl_state.current_viewport = ivec4(vp->bounds.xy(), vp->bounds.zw() - vp->bounds.xy());
	}
	else
		gl_state.current_viewport = ivec4(0,0,tgt->size());
	
	nsopengl_driver::set_gl_state(gl_state);
}

void render_pass::render()
{
	for (uint32 i = 0; i < draw_calls->size(); ++i)
	{
		draw_call * dc = (*draw_calls)[i];
		dc->shdr->bind();
		dc->shdr->set_viewport(gl_state.current_viewport);
		if (vp->camera != nullptr)
			dc->shdr->set_proj_cam_mat(vp->camera->get<nscam_comp>()->proj_cam());
		dc->shdr->set_for_draw_call(dc);
		if (dc->mat != nullptr)
		{
			nsopengl_driver::enable_culling(dc->mat->culling());
			nsopengl_driver::set_cull_face(dc->mat->cull_mode());
			nsopengl_driver::bind_textures(dc->mat);
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
		if (vp->camera != nullptr)
			dc->shdr->set_proj_cam_mat(vp->camera->get<nscam_comp>()->proj_cam());
		dc->shdr->set_for_draw_call(dc);
		((nsfb_object*)ren_target)->set_draw_buffer(nsfb_object::att_none);
		dc->render();
		
		glPolygonMode(GL_FRONT, GL_LINE);
		glLineWidth(4.0f);
		nsopengl_driver::set_stencil_func(GL_NOTEQUAL, 1, -1);
		((nsfb_object*)ren_target)->set_draw_buffer(nsfb_object::att_color);
		((nsselection_shader*)dc->shdr)->set_border_color(dc->sel_color.rgb());
		dc->render();
		
		glLineWidth(1.0f);
		glPolygonMode(GL_FRONT, GL_FILL);
		nsopengl_driver::enable_depth_test(false);
		nsopengl_driver::set_stencil_func(GL_EQUAL, 1, 0);
		((nsselection_shader*)dc->shdr)->set_main_color(dc->sel_color);
		dc->render();
	}	
}

void gbuffer_render_pass::render()
{
	render_pass::render();
	nsopengl_driver::bind_gbuffer_textures((nsfb_object*)ren_target);
}

void oit_render_pass::render()
{
	((nsfb_object*)ren_target)->set_draw_buffer(nsfb_object::att_none);
	for (uint32 i = 0; i < draw_calls->size(); ++i)
	{
		draw_call * dc = (*draw_calls)[i];
		dc->shdr->bind();
		dc->shdr->set_viewport(gl_state.current_viewport);
		if (vp->camera != nullptr)
			dc->shdr->set_proj_cam_mat(vp->camera->get<nscam_comp>()->proj_cam());
		dc->shdr->set_for_draw_call(dc);
		if (dc->mat != nullptr)
		{
			nsopengl_driver::enable_culling(dc->mat->culling());
			nsopengl_driver::set_cull_face(dc->mat->cull_mode());
			nsopengl_driver::bind_textures(dc->mat);
		}
		tbuffers->bind_buffers();
		dc->render();
	}
	((nsfb_object*)ren_target)->update_draw_buffers();
	nsopengl_driver::enable_depth_test(false);
	nsopengl_driver::enable_culling(true);
	nsopengl_driver::set_cull_face(GL_BACK);
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
			nsopengl_driver::enable_culling(dc->mat->culling());
			nsopengl_driver::set_cull_face(dc->mat->cull_mode());
			dc->render();
		}
	}
}

void light_pass::render()
{
	for (uint32 i = 0; i < draw_calls->size(); ++i)
	{
		light_draw_call * dc = (light_draw_call*)(*draw_calls)[i];
		if (dc->cast_shadows && vp->dir_light_shadows)
		{
			rpass->ldc = dc;
			rpass->setup_pass();
			rpass->render();
			nsfb_object::attachment * attch = ((nsfb_object*)rpass->ren_target)->att(nsfb_object::att_depth);
			nsopengl_driver::set_active_texture_unit(attch->m_tex_unit);
			attch->m_texture->bind();
			render_pass::setup_pass();
		}
		((nsfb_object*)ren_target)->set_draw_buffer(nsfb_object::att_color);
		dc->shdr->bind();
		dc->shdr->set_proj_cam_mat(vp->camera->get<nscam_comp>()->proj_cam());
		dc->shdr->set_viewport(gl_state.current_viewport);
		dc->shdr->set_for_draw_call(dc);
		dc->shdr->set_uniform("camWorldPos", vp->camera->get<nstform_comp>()->wpos());
		dc->shdr->set_uniform("fog_factor", vp->m_fog_nf);
		dc->shdr->set_uniform("fog_color", vp->m_fog_color);
		dc->shdr->set_uniform("lightingEnabled", vp->dir_lights);
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
		if (dc->cast_shadows && vp->dir_light_shadows)
		{
			rpass->ldc = dc;
			rpass->setup_pass();
			rpass->render();
			nsfb_object::attachment * attch = ((nsfb_object*)rpass->ren_target)->att(nsfb_object::att_depth);
			nsopengl_driver::set_active_texture_unit(attch->m_tex_unit);
			attch->m_texture->bind();
			render_pass::setup_pass();
		}
		((nsfb_object*)ren_target)->set_draw_buffer(nsfb_object::att_none);
		stencil_shdr->bind();
		stencil_shdr->set_proj_cam_mat(vp->camera->get<nscam_comp>()->proj_cam());
		stencil_shdr->set_viewport(gl_state.current_viewport);
		stencil_shdr->set_for_draw_call(dc);
		dc->render();
		
		nsopengl_driver::enable_depth_test(false);
		nsopengl_driver::enable_culling(true);
		nsopengl_driver::set_cull_face(GL_FRONT);
		nsopengl_driver::set_stencil_func(GL_NOTEQUAL, 0, 0xFF);
		((nsfb_object*)ren_target)->set_draw_buffer(nsfb_object::att_color);
		dc->shdr->bind();
		dc->shdr->set_viewport(gl_state.current_viewport);
		dc->shdr->set_proj_cam_mat(vp->camera->get<nscam_comp>()->proj_cam());
		dc->shdr->set_uniform("fog_factor", vp->m_fog_nf);
		dc->shdr->set_uniform("fog_color", vp->m_fog_color);		
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
		vp->bounds.x,
		vp->bounds.y,
		vp->bounds.z,
		vp->bounds.w,
		vp->bounds.x,
		vp->bounds.y,
		vp->bounds.z,
		vp->bounds.w,
		GL_COLOR_BUFFER_BIT,
		GL_NEAREST);

	read_buffer->set_read_buffer(nsfb_object::att_none);
	read_buffer->unbind();	
}

nsopengl_driver::nsopengl_driver() :
	nsvideo_driver(),
	m_tbuffers(nullptr),
	m_single_point(nullptr)
{
	m_all_draw_calls.reserve(MAX_INSTANCED_DRAW_CALLS);
	m_light_draw_calls.reserve(MAX_LIGHTS_IN_SCENE);
}

nsopengl_driver::~nsopengl_driver()
{}

void nsopengl_driver::create_default_render_targets()
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
}

void nsopengl_driver::create_default_render_passes()
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
	gbuf_pass->use_vp_size = true;
	gbuf_pass->gl_state.clear_mask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;

	// setup gbuffer geometry stage
	oit_render_pass * oit_pass = new oit_render_pass;
	oit_pass->tbuffers = m_tbuffers;
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
	dir_pass->tbuffers = m_tbuffers;
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
	spot_pass->tbuffers = m_tbuffers;
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
	point_pass->tbuffers = m_tbuffers;
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
	final_pass->ren_target = m_default_target;
	final_pass->read_buffer = static_cast<nsfb_object*>(render_target(ACCUM_TARGET));
	final_pass->gl_state.depth_test = false;
	final_pass->gl_state.depth_write = false;
	final_pass->gl_state.culling = false;
	final_pass->gl_state.blending = false;
	final_pass->gl_state.stencil_test = false;
	final_pass->gl_state.cull_face = GL_BACK;
	final_pass->use_vp_size = true;

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

void nsopengl_driver::create_default_render_queues()
{
	create_queue(SCENE_OPAQUE_QUEUE)->reserve(MAX_GBUFFER_DRAWS);
	create_queue(SCENE_TRANSLUCENT_QUEUE)->reserve(MAX_GBUFFER_DRAWS);
	create_queue(SCENE_SELECTION_QUEUE)->reserve(MAX_GBUFFER_DRAWS);
	create_queue(DIR_LIGHT_QUEUE)->reserve(MAX_LIGHTS_IN_SCENE);
	create_queue(SPOT_LIGHT_QUEUE)->reserve(MAX_LIGHTS_IN_SCENE);
	create_queue(POINT_LIGHT_QUEUE)->reserve(MAX_LIGHTS_IN_SCENE);	
}

void nsopengl_driver::release()
{
	m_single_point->release();
	delete m_single_point;
	m_single_point = nullptr;
	nsvideo_driver::release();
	m_initialized = false;
}
	
void nsopengl_driver::push_scene(nsscene * scn)
{
	_prepare_tforms(scn);
	m_mat_shader_ids.clear();
	m_all_draw_calls.resize(0);
	m_light_draw_calls.resize(0);
	_add_draw_calls_from_scene(scn);
	_add_lights_from_scene(scn);
}

void nsopengl_driver::render(nsrender::viewport * vp)
{
	if (!_valid_check())
		return;

	m_render_passes[oit]->enabled = vp->order_independent_transparency;
	//m_render_passes[dir_shadow]->enabled = vp->dir_light_shadows;
	m_render_passes[dir_light]->enabled = vp->dir_lights;
	//m_render_passes[spot_shadow]->enabled = vp->spot_light_shadows;
	m_render_passes[spot_light]->enabled = vp->spot_lights;
	//m_render_passes[point_shadow]->enabled = vp->point_light_shadows;
	m_render_passes[point_light]->enabled = vp->point_lights;
	m_render_passes[selection]->enabled = vp->picking_enabled;
	nsvideo_driver::render(vp);
	m_tbuffers->reset_atomic_counter();
}

void nsopengl_driver::set_gl_state(const opengl_state & state)
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

void nsopengl_driver::set_blend_eqn(int32 eqn)
{
	if (eqn == 0)
		return;
	
	glBlendEquation(eqn);
	m_gl_state.blend_eqn = eqn;
	gl_err_check("nsopengl_driver::set_blend_eqn");	
}

bool nsopengl_driver::_valid_check()
{
	static bool shadererr = false;
	static bool shadernull = false;

	if (!m_shaders.valid())
	{
		if (!shadernull)
		{
			dprint("nsopengl_driver::_validCheck: Error - one of the rendering shaders is nullptr ");
			shadernull = true;
		}
		return false;
	}
	shadernull = false;

	if (m_shaders.error())
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

//void nsopengl_driver::render_scene_particles(nsscene * scene, nsentity * cam)
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

opengl_state nsopengl_driver::m_gl_state = opengl_state();

void nsopengl_driver::enable_depth_write(bool enable)
{
	glDepthMask(enable);
	m_gl_state.depth_write = enable;
	gl_err_check("nsopengl_driver::enable_depth_write");
}

void nsopengl_driver::enable_depth_test(bool enable)
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

void nsopengl_driver::enable_stencil_test(bool enable)
{
	if (enable)
		glEnable(GL_STENCIL_TEST);
	else
		glDisable(GL_STENCIL_TEST);
	gl_err_check("nsopengl_driver::enable_stencil_test");
	m_gl_state.stencil_test = enable;
}

void nsopengl_driver::enable_blending(bool enable)
{
	if (enable)
		glEnable(GL_BLEND);
	else
		glDisable(GL_BLEND);
	m_gl_state.blending = enable;
	gl_err_check("nsopengl_driver::enable_blending");
}

void nsopengl_driver::enable_culling(bool enable)
{
	if (enable)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);
	m_gl_state.culling = enable;
	gl_err_check("nsopengl_driver::enable_culling");
}

void nsopengl_driver::set_cull_face(int32 cull_face)
{
	if (cull_face != m_gl_state.cull_face)
	{
		glCullFace(cull_face);
		m_gl_state.cull_face = cull_face;
		gl_err_check("nsopengl_driver::set_cull_face");
	}	
}

opengl_state nsopengl_driver::current_gl_state()
{
	return m_gl_state;
}

void nsopengl_driver::set_blend_func(int32 sfactor, int32 dfactor)
{
	set_blend_func(ivec2(sfactor,dfactor));
}

void nsopengl_driver::set_blend_func(const ivec2 & blend_func)
{
	glBlendFunc(blend_func.x, blend_func.y);
	m_gl_state.blend_func = blend_func;
	gl_err_check("nsopengl_driver::set_blend_func");
}

void nsopengl_driver::set_stencil_func(int32 func, int32 ref, int32 mask)
{
	set_stencil_func(ivec3(func,ref,mask));
}

void nsopengl_driver::set_stencil_func(const ivec3 & stencil_func)
{
	glStencilFunc(stencil_func.x, stencil_func.y, stencil_func.z);
	m_gl_state.stencil_func = stencil_func;
	gl_err_check("nsopengl_driver::set_stencil_func");
}

void nsopengl_driver::set_stencil_op(int32 sfail, int32 dpfail, int32 dppass)
{
	set_stencil_op(ivec3( sfail, dpfail, dppass));
}

void nsopengl_driver::set_stencil_op(const ivec3 & stencil_op)
{
	glStencilOp(stencil_op.x, stencil_op.y, stencil_op.z);
	m_gl_state.stencil_op_back = stencil_op;
	m_gl_state.stencil_op_front = stencil_op;
	gl_err_check("nsopengl_driver::set_stencil_op");
}

void nsopengl_driver::set_stencil_op_back(int32 sfail, int32 dpfail, int32 dppass)
{
	set_stencil_op_back(ivec3(sfail, dpfail, dppass));	
}
	
void nsopengl_driver::set_stencil_op_back(const ivec3 & stencil_op)
{
	glStencilOpSeparate(GL_BACK, stencil_op.x, stencil_op.y, stencil_op.z);
	m_gl_state.stencil_op_back = stencil_op;
	gl_err_check("nsopengl_driver::set_stencil_op_back");
}

void nsopengl_driver::set_stencil_op_front(int32 sfail, int32 dpfail, int32 dppass)
{
	set_stencil_op_front(ivec3(sfail, dpfail, dppass));		
}
	
void nsopengl_driver::set_stencil_op_front(const ivec3 & stencil_op)
{
	glStencilOpSeparate(GL_FRONT, stencil_op.x, stencil_op.y, stencil_op.z);
	m_gl_state.stencil_op_front = stencil_op;
	gl_err_check("nsopengl_driver::set_stencil_op_front");
}

void nsopengl_driver::_prepare_tforms(nsscene * scene)
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

uint32 nsopengl_driver::bound_fbo()
{
	GLint params;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &params);
	return params;
}

uint32 nsopengl_driver::active_tex_unit()
{
	GLint act_un;
	glGetIntegerv(GL_ACTIVE_TEXTURE, &act_un);
	return act_un - BASE_TEX_UNIT;
}

void nsopengl_driver::init()
{
	glewExperimental = true;
	// Initialize the glew extensions - if this fails we want a crash because there is nothing
	// useful the program can do without these initialized
	GLenum cont = glewInit();
	if (cont != GLEW_OK)
	{
		dprint("GLEW extensions unable to initialize");
		return;
	}
	m_initialized = true;

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
	m_default_target = new nsfb_object;
	((nsfb_object*)m_default_target)->set_gl_id(0);

	m_single_point = new nsbuffer_object(nsbuffer_object::array, nsbuffer_object::storage_mutable);
	m_single_point->init_gl();
	
	uint32 data_ = 0;
	uint32 sz = DEFAULT_ACCUM_BUFFER_RES_X*DEFAULT_ACCUM_BUFFER_RES_Y;
	i_vector header_data(sz, -1);
	m_tbuffers = new translucent_buffers;
	m_tbuffers->atomic_counter->bind();
	m_tbuffers->atomic_counter->allocate(data_, nsbuffer_object::mutable_dynamic_draw);
	m_tbuffers->atomic_counter->unbind();
	m_tbuffers->header->bind();
	m_tbuffers->header->allocate(header_data, nsbuffer_object::mutable_dynamic_draw, header_data.size());
	m_tbuffers->fragments->bind();
	m_tbuffers->fragments->allocate<packed_fragment_data>(nsbuffer_object::mutable_dynamic_draw, sz * 2);
	m_tbuffers->fragments->unbind();

	m_single_point->bind();
	fvec3 point;
	m_single_point->allocate(point, nsbuffer_object::mutable_dynamic_draw);
	m_single_point->unbind();
	nsvideo_driver::init();
}

void nsopengl_driver::set_active_texture_unit(uint32 tex_unit)
{
	glActiveTexture(BASE_TEX_UNIT + tex_unit);
	gl_err_check("nsopengl_driver::set_active_texture_unit");
}

void nsopengl_driver::_add_lights_from_scene(nsscene * scene)
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
//		nstform_comp * camt = scene->camera()->get<nstform_comp>();
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
			ldc->bg_color = scene->bg_color();
			ldc->direction = lcomp->transform(i).target();
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
				ldc->shadow_tex_size = ((nsfb_object*)render_target(DIR_SHADOW2D_TARGET))->size();
				dc_dq->push_back(ldc);
			}
			else if (lcomp->type() == nslight_comp::l_spot)
			{
				ldc->shadow_tex_size = ((nsfb_object*)render_target(SPOT_SHADOW2D_TARGET))->size();
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
				ldc->shadow_tex_size = ((nsfb_object*)render_target(POINT_SHADOW_TARGET))->size();
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

void nsopengl_driver::_add_draw_calls_from_scene(nsscene * scene)
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
				mat = nse.system<nsrender_system>()->default_mat();

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

void nsopengl_driver::set_viewport(const ivec4 & val)
{
	if (m_gl_state.current_viewport != val)
	{
		glViewport(val.x, val.y, val.z, val.w);
		m_gl_state.current_viewport = val;
		gl_err_check("nsopengl_driver::set_viewport");
	}
}

void nsopengl_driver::clear_framebuffer(uint32 clear_mask)
{
	glClear(clear_mask);
	gl_err_check("nsopengl_driver::clear_framebuffer");
}

void nsopengl_driver::bind_textures(nsmaterial * material)
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

uivec3 nsopengl_driver::pick(const fvec2 & mouse_pos)
{
	nsfb_object * pck = (nsfb_object*)render_target(ACCUM_TARGET);
	if (pck == NULL)
		return uivec3();

	fvec2 screen_size = fvec2(m_default_target->size().x, m_default_target->size().y);
	fvec2 accum_size = fvec2(pck->size().x,pck->size().y);
	fvec2 rmpos = mouse_pos % (screen_size / accum_size);
	uivec3 index = pck->pick(rmpos.x, rmpos.y, 1);
	return index;
}

void nsopengl_driver::bind_gbuffer_textures(nsfb_object * fb)
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
