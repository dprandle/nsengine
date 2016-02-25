/*!
  \file nsopengl_driver.cpp

  \brief Definition file for nsopengl_driver class

  This file contains all of the neccessary definitions for the nsopengl_driver class.

  \author Daniel Randle
  \date Feb 23 2016
  \copywrite Earth Banana Games 2013
*/

#include <nsopengl_driver.h>
#include <nsshader.h>

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

