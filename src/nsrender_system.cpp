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

translucent_buffers::translucent_buffers():
	atomic_counter(new nsbuffer_object(nsbuffer_object::atomic_counter, nsbuffer_object::storage_mutable)),
	header(new nsbuffer_object(nsbuffer_object::shader_storage, nsbuffer_object::storage_mutable)),
	fragments(new nsbuffer_object(nsbuffer_object::shader_storage, nsbuffer_object::storage_mutable)),
	header_clr_data(DEFAULT_FB_RES_X*DEFAULT_FB_RES_Y, -1)
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

draw_call::draw_call(nsmesh::submesh * pSubMesh, 
					 fmat4_vector * pAnimTransforms,
					 nsbuffer_object * pTransformBuffer,
					 nsbuffer_object * pTransformIDBuffer,
					 const fvec2 & heightMinMax,
					 uint32 pEntID,
					 uint32 plugID,
					 uint32 pNumTransforms, 
					 bool pCastShadows,
					 bool selectable_) :
	submesh(pSubMesh),
	anim_transforms(pAnimTransforms),
	transform_buffer(pTransformBuffer),
	transform_id_buffer(pTransformIDBuffer),
	height_minmax(heightMinMax),
	entity_id(pEntID),
	plugin_id(plugID),
	transform_count(pNumTransforms),
	casts_shadows(pCastShadows),
	transparent_picking(selectable_)
{}

draw_call::~draw_call()
{}

bool draw_call::operator<(const draw_call & rhs) const
{
	if (entity_id < rhs.entity_id)
		return true;
	else if (entity_id == rhs.entity_id)
		return submesh < rhs.submesh;
	return false;
}

bool draw_call::operator<=(const draw_call & rhs) const
{
	return (*this < rhs || *this == rhs);
}

bool draw_call::operator>(const draw_call & rhs) const
{
	return !(*this <= rhs);
}

bool draw_call::operator>=(const draw_call & rhs) const
{
	return !(*this < rhs);
}

bool draw_call::operator==(const draw_call & rhs) const
{
	return (entity_id == rhs.entity_id) && (submesh == rhs.submesh);
}

bool draw_call::operator!=(const draw_call & rhs) const
{
	return !(*this == rhs);
}

render_shaders::render_shaders() :
	deflt(NULL),
	deflt_wireframe(NULL),
	deflt_translucent(NULL),
	light_stencil(NULL),
	frag_sort(NULL),
	dir_light(NULL),
	point_light(NULL),
	spot_light(NULL),
	point_shadow(NULL),
	spot_shadow(NULL),
	sel_shader(NULL),
	dir_shadow(NULL)
{}

bool render_shaders::error()
{
	return (
		deflt->error() != nsshader::error_none ||
		deflt_wireframe->error() != nsshader::error_none ||
		deflt_translucent->error() != nsshader::error_none ||
		light_stencil->error() != nsshader::error_none ||
		frag_sort->error() != nsshader::error_none ||
		dir_light->error() != nsshader::error_none ||
		point_light->error() != nsshader::error_none ||
		spot_light->error() != nsshader::error_none ||
		point_shadow->error() != nsshader::error_none ||
		spot_shadow->error() != nsshader::error_none ||
		sel_shader->error() != nsshader::error_none ||
		dir_shadow->error() != nsshader::error_none );
}

bool render_shaders::valid()
{
	return (
		deflt != NULL &&
		deflt_wireframe != NULL &&
		deflt_translucent != NULL &&
		light_stencil != NULL &&
		frag_sort != NULL &&
		dir_light != NULL &&
		point_light != NULL &&
		spot_light != NULL &&
		point_shadow != NULL &&
		spot_shadow != NULL &&
		sel_shader != NULL &&
		dir_shadow != NULL );
}

nsrender_system::nsrender_system() :
nssystem(),
  m_drawcall_map(),
  m_gbuffer(new nsgbuf_object()),
  m_final_buf(NULL),
  m_shadow_buf(new nsshadowbuf_object()),
  m_shaders(),
  m_debug_draw(false),
  m_lighting_enabled(true),
  m_screen_fbo(0),
m_fog_color(1),
m_fog_nf(DEFAULT_FOG_FACTOR_NEAR, DEFAULT_FOG_FACTOR_FAR),
m_single_point(new nsbuffer_object(nsbuffer_object::array, nsbuffer_object::storage_mutable))
{
	m_single_point->init_gl();
}

nsrender_system::~nsrender_system()
{
	delete m_shadow_buf;
	delete m_gbuffer;
	m_single_point->release();
	delete m_single_point;
}

void nsrender_system::set_screen_fbo(uint32 fbo)
{
	m_screen_fbo = fbo;
}

uint32 nsrender_system::screen_fbo()
{
	return m_screen_fbo;
}

void nsrender_system::blit_final_frame()
{
	if (m_debug_draw)
		return;

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_screen_fbo);
	if (nse.current_scene() != NULL)
	{
		m_final_buf->set_target(nsfb_object::fb_read);
		m_final_buf->bind();
		m_final_buf->set_read_buffer(nsfb_object::att_color);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_STENCIL_TEST);
		glDisable(GL_BLEND);
		glBlitFramebuffer(0, 0, DEFAULT_FB_RES_X, DEFAULT_FB_RES_Y, 0, 0, DEFAULT_FB_RES_X, DEFAULT_FB_RES_Y, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		m_final_buf->set_read_buffer(nsfb_object::att_none);
		m_final_buf->unbind();
	}
	else
	{
		glClearColor(0.11f, 0.11f, 0.11f, 0.11f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
}

void nsrender_system::enable_lighting(bool pEnable)
{
	m_lighting_enabled = pEnable;
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
			dprint("nsrender_system::_validCheck: Error - one of the rendering shaders is NULL ");
			shadernull = true;
		}
		return false;
	}
	shadernull = false;

	if (m_default_mat == NULL)
	{
		if (!defmaterr)
		{
			dprint("nsrender_system::_validCheck Error - deflt material is NULL - assign deflt material to render system");
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


void nsrender_system::draw_opaque(nsentity * camera)
{
	_draw_geometry(m_shader_mat_map, m_drawcall_map, camera);
	_bind_gbuffer_textures();	
}

void nsrender_system::draw_translucent(nsentity * camera)
{
	m_final_buf->set_draw_buffer(nsfb_object::att_none);// picking
	_draw_geometry(m_tshader_mat_map, m_tdrawcall_map, camera);
}

void nsrender_system::draw_pre_lighting_pass()
{
	m_final_buf->update_draw_buffers();
	m_shaders.frag_sort->bind();
	m_shaders.frag_sort->set_screen_size(fvec2(m_final_buf->size().x, m_final_buf->size().y));
	m_single_point->bind();
	glDrawArrays(GL_POINTS, 0, 1);
}

void nsrender_system::draw_selection(nsscene * scene, nsentity * cam)
{
	nscam_comp * camc = cam->get<nscam_comp>();
	m_shaders.sel_shader->bind();
	m_shaders.sel_shader->set_proj_cam_mat(camc->proj_cam());
    m_shaders.sel_shader->set_heightmap_enabled(false);

    // Go through and stencil each selected item
	auto iter = scene->entities<nssel_comp>().begin();
	while (iter != scene->entities<nssel_comp>().end())
	{
		nstform_comp * tc = (*iter)->get<nstform_comp>();
		nssel_comp * sc = (*iter)->get<nssel_comp>();
		nsrender_comp * rc = (*iter)->get<nsrender_comp>();
		nsanim_comp * ac = (*iter)->get<nsanim_comp>();
		nsterrain_comp * trc = (*iter)->get<nsterrain_comp>();

		if (rc == NULL || tc == NULL || sc == NULL)
		{
			++iter;
			continue;
		}

		if (sc->selected() && sc->draw_enabled())
		{
			nsmaterial * mat;
			draw_call dc;
			nsmesh * rc_msh = nse.resource<nsmesh>(rc->mesh_id());
			for (uint32 i = 0; i < rc_msh->count(); ++i)
			{
				dc.submesh = rc_msh->sub(i);

				// make it so the height map is selectable
				nsmaterial * mat = nse.resource<nsmaterial>(rc->material_id(i));
				if (mat != NULL)
				{
					nstexture * tex = nse.resource<nstexture>(mat->map_tex_id(nsmaterial::height));
					if (tex != NULL)
					{
						m_shaders.sel_shader->set_heightmap_enabled(true);
						set_active_texture_unit(nsmaterial::height);
						tex->bind();
					}
				}

				if (dc.submesh->m_node != NULL)
					m_shaders.sel_shader->set_node_transform(dc.submesh->m_node->m_world_tform);
				else
					m_shaders.sel_shader->set_node_transform(fmat4());

				if (ac != NULL)
				{
					m_shaders.sel_shader->set_has_bones(true);
					m_shaders.sel_shader->set_bone_transform(*ac->final_transforms());
				}
				else
					m_shaders.sel_shader->set_has_bones(false);

				if (trc != NULL)
					m_shaders.sel_shader->set_height_minmax(trc->height_bounds());

				dc.transform_buffer = sc->transform_buffer();
				dc.transform_count = sc->count();
				dc.submesh->m_vao.bind();

				dc.transform_buffer->bind();
				for (uint32 tfInd = 0; tfInd < 4; ++tfInd)
				{
					dc.submesh->m_vao.add(dc.transform_buffer,
										  nsshader::loc_instance_tform + tfInd);
					dc.submesh->m_vao.vertex_attrib_ptr(nsshader::loc_instance_tform + tfInd,
						4, GL_FLOAT, GL_FALSE, sizeof(fmat4), sizeof(fvec4)*tfInd);
					dc.submesh->m_vao.vertex_attrib_div(nsshader::loc_instance_tform + tfInd, 1);
				}

				glDepthMask(GL_TRUE);
				glDisable(GL_DEPTH_TEST);
				glStencilFunc(GL_ALWAYS, 1, -1);
				glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

				m_final_buf->set_draw_buffer(nsfb_object::att_none);
				gl_err_check("nsrender_system::draw_selection pre");
				glDrawElementsInstanced(
					dc.submesh->m_prim_type,
					static_cast<GLsizei>(dc.submesh->m_indices.size()),
					GL_UNSIGNED_INT,
					0,
					dc.transform_count);
				gl_err_check("nsrender_system::draw_selection post");	

				glPolygonMode(GL_FRONT, GL_LINE);
				glStencilFunc(GL_NOTEQUAL, 1, -1);
				glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

				fvec4 selCol = sc->color();
				m_shaders.sel_shader->set_frag_color_out(selCol);

				m_final_buf->set_draw_buffer(nsfb_object::att_color);
				gl_err_check("nsrender_system::draw_selection pre");
				glDrawElementsInstanced(
					dc.submesh->m_prim_type,
					static_cast<GLsizei>(dc.submesh->m_indices.size()),
					GL_UNSIGNED_INT,
					0,
					dc.transform_count);
				gl_err_check("nsrender_system::draw_selection post");	

				glPolygonMode(GL_FRONT, GL_FILL);
				glEnable(GL_DEPTH_TEST);
				glStencilFunc(GL_EQUAL, 1, 0);
				selCol.w = sc->mask_alpha();
				
				//if (m_focus_ent.y == (*iter)->id() && m_focus_ent.z == *selIter)
				//	selCol.w = 0.4f;
				m_shaders.sel_shader->set_uniform("fragColOut", selCol);
				gl_err_check("nsrender_system::draw_selection pre");
				glDrawElementsInstanced(
					dc.submesh->m_prim_type,
					static_cast<GLsizei>(dc.submesh->m_indices.size()),
					GL_UNSIGNED_INT,
					0,
					dc.transform_count);
				gl_err_check("nsrender_system::draw_selection post");	
				dc.transform_id_buffer->bind();
				dc.submesh->m_vao.remove(dc.transform_id_buffer);
				dc.submesh->m_vao.unbind();
			}
		}
		++iter;
	}
	glEnable(GL_DEPTH_TEST);
					
}

void nsrender_system::draw()
{
	if (!_valid_check())
		return;

	nsscene * scene = nse.current_scene();
	if (scene == NULL)
		return;

	nsentity * cam = scene->camera();
	if (cam == NULL)
		return;


	m_gbuffer->bind();
	
	if (scene->skydome() == nullptr)
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	else
		glClear(GL_DEPTH_BUFFER_BIT);

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);
	glDepthMask(true);	

	draw_opaque(cam);	

	if (m_debug_draw)
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_screen_fbo);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		m_gbuffer->debug_blit(m_screen_size);	
		return;
	}

	glDepthMask(false);

	m_final_buf->set_target(nsfb_object::fb_draw);
	m_final_buf->bind();
	draw_translucent(cam);

	glDisable(GL_DEPTH_TEST);
	draw_pre_lighting_pass();
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	blend_dir_lights(cam, scene->bg_color());

	glEnable(GL_STENCIL_TEST);
	glCullFace(GL_FRONT);
	
	//blend_spot_lights(cam);
	//blend_point_lights(cam);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glClear(GL_STENCIL_BUFFER_BIT);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glStencilFunc(GL_ALWAYS, 1, -1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	//draw_selection(scene, cam);
	
	blit_final_frame();
}

void nsrender_system::blend_spot_lights(nsentity * camera)
{
	nstform_comp * camt = camera->get<nstform_comp>();
	nscam_comp * camc = camera->get<nscam_comp>();
	float ar = float(m_shadow_buf->size(nsshadowbuf_object::Spot).w) /
		float(m_shadow_buf->size(nsshadowbuf_object::Spot).h);
	fmat4 proj, proj_light_mat;
	
	glCullFace(GL_FRONT);	
	for (uint32 light_index = 0; light_index < m_lights.spot_lights.size(); ++light_index)
	{
		nslight_comp * lc = m_lights.spot_lights[light_index]->get<nslight_comp>();
		nstform_comp * tc = m_lights.spot_lights[light_index]->get<nstform_comp>();
		proj = perspective(lc->angle()*2.0f, ar, lc->shadow_clipping().x, lc->shadow_clipping().y);
		for (uint32 i = 0; i < lc->transform_count(); ++i)
		{
			proj_light_mat = proj * lc->pov(i);
			if (lc->cast_shadows())
			{
				glEnable(GL_DEPTH_TEST);
				glDepthMask(GL_TRUE);
				glViewport(0, 0, m_shadow_buf->size(nsshadowbuf_object::Spot).w,
						   m_shadow_buf->size(nsshadowbuf_object::Spot).h);
				m_shadow_buf->bind(nsshadowbuf_object::Spot);
				glClear(GL_DEPTH_BUFFER_BIT);
				m_shaders.spot_shadow->bind();
				m_shaders.spot_shadow->set_proj_mat(proj_light_mat);
				_draw_scene_to_depth(m_shaders.spot_shadow);
				glViewport(0, 0, camc->screen_size().w, camc->screen_size().h);

				nsfb_object::attachment * attch =
					m_shadow_buf->fb(nsshadowbuf_object::Spot)->att(nsfb_object::att_depth);
				set_active_texture_unit(attch->m_tex_unit);
				attch->m_texture->bind();
			}

			m_final_buf->bind();
			glDepthMask(GL_FALSE);
			glDisable(GL_CULL_FACE);
			glStencilFunc(GL_ALWAYS, 0, 0);
			glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
			glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);
			m_final_buf->set_draw_buffer(nsfb_object::att_none);
			glClear(GL_STENCIL_BUFFER_BIT);
			m_shaders.light_stencil->bind();
			m_shaders.light_stencil->set_proj_cam_mat(camc->proj_cam());
			m_shaders.light_stencil->set_transform(lc->transform(i));
			_stencil_spot_light(lc);

			glDisable(GL_DEPTH_TEST);
			glEnable(GL_CULL_FACE);
			glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
			m_final_buf->set_draw_buffer(nsfb_object::att_color);
			m_shaders.spot_light->bind();
			m_shaders.spot_light->set_cam_world_pos(camt->wpos());
			m_shaders.spot_light->set_position(tc->wpos(i));
			m_shaders.spot_light->set_max_depth(lc->shadow_clipping().y - lc->shadow_clipping().x);
			m_shaders.spot_light->set_transform(lc->transform(i));
			m_shaders.spot_light->set_proj_cam_mat(camc->proj_cam());
			m_shaders.spot_light->set_proj_light_mat(proj_light_mat);
			m_shaders.spot_light->set_direction(tc->dvec(nstform_comp::dir_target, i));
			_blend_spot_light(lc);
		}
	}		
}

void nsrender_system::blend_point_lights(nsentity * camera)
{
	float ar = float(m_shadow_buf->size(nsshadowbuf_object::Point).w) /
		float(m_shadow_buf->size(nsshadowbuf_object::Point).h);
	fmat4 proj;
	fmat4 proj_light_mat;
	fmat4 inv_trans;
	nscam_comp * camc = camera->get<nscam_comp>();
	nstform_comp * camt = camera->get<nstform_comp>();

	glCullFace(GL_FRONT);
	for (uint32 light_index = 0; light_index < m_lights.point_lights.size(); ++light_index)
	{
		nslight_comp * lc = m_lights.point_lights[light_index]->get<nslight_comp>();
		nstform_comp * tc = m_lights.point_lights[light_index]->get<nstform_comp>();
		
		proj = perspective(90.0f, ar, lc->shadow_clipping().x, lc->shadow_clipping().y);
		for (uint32 i = 0; i < lc->transform_count(); ++i)
		{
			proj_light_mat = proj * lc->pov(i);

			if (lc->cast_shadows())
			{
				glEnable(GL_DEPTH_TEST);
				glDepthMask(GL_TRUE);
				glViewport(0, 0, m_shadow_buf->size(nsshadowbuf_object::Point).w, m_shadow_buf->size(nsshadowbuf_object::Point).h);
				m_shadow_buf->bind(nsshadowbuf_object::Point);
				glClear(GL_DEPTH_BUFFER_BIT);
				m_shaders.point_shadow->bind();
				m_shaders.point_shadow->set_light_pos(tc->wpos());
				m_shaders.point_shadow->set_max_depth(lc->shadow_clipping().y - lc->shadow_clipping().x);
				m_shaders.point_shadow->set_proj_mat(proj);
				inv_trans.set_column(3,fvec4(tc->wpos(i)*-1.0f,1.0f));
				m_shaders.point_shadow->set_inverse_trans_mat(inv_trans);
				_draw_scene_to_depth(m_shaders.point_shadow);
				glViewport(0, 0, camc->screen_size().w, camc->screen_size().h);

				// bind the point light shadow fb's attachment for use in the shader
				nsfb_object::attachment * attch =
					m_shadow_buf->fb(nsshadowbuf_object::Point)->att(nsfb_object::att_depth);
				set_active_texture_unit(attch->m_tex_unit);
				attch->m_texture->bind();
			}

			glDepthMask(GL_FALSE);
			glDisable(GL_CULL_FACE);
			glStencilFunc(GL_ALWAYS, 0, 0);
			glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
			glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);
			m_final_buf->bind();
			m_final_buf->set_draw_buffer(nsfb_object::att_none);
			glClear(GL_STENCIL_BUFFER_BIT);
			m_shaders.light_stencil->bind();
			m_shaders.light_stencil->set_proj_cam_mat(camc->proj_cam());
			m_shaders.light_stencil->set_transform(lc->transform(i));
			_stencil_point_light(lc);

			glEnable(GL_CULL_FACE);
			glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
			glDisable(GL_DEPTH_TEST);
			m_final_buf->set_draw_buffer(nsfb_object::att_color);
			m_shaders.point_light->bind();
			m_shaders.point_light->set_cam_world_pos(camt->wpos());
			m_shaders.point_light->set_position(tc->wpos(i));
			m_shaders.point_light->set_max_depth(lc->shadow_clipping().y - lc->shadow_clipping().x);
			m_shaders.point_light->set_transform(lc->transform(i));
			m_shaders.point_light->set_proj_cam_mat(camc->proj_cam());
			_blend_point_light(lc);
		}
	}
}	

void nsrender_system::blend_dir_lights(nsentity * camera, const fvec4 & bg_color)
{
	float ar = float(m_shadow_buf->size(nsshadowbuf_object::Direction).w) /
		float(m_shadow_buf->size(nsshadowbuf_object::Direction).h);
	fmat4 proj;
	fmat4 proj_light_mat;
	nscam_comp * camc = camera->get<nscam_comp>();
	nstform_comp * camt = camera->get<nstform_comp>();	
	for (uint32 light_index = 0; light_index < m_lights.dir_lights.size(); ++light_index)
	{
		nslight_comp * lc = m_lights.dir_lights[light_index]->get<nslight_comp>();
		proj = perspective(160.0f, ar, lc->shadow_clipping().x, lc->shadow_clipping().y);
		for (uint32 i = 0; i < lc->transform_count(); ++i)
		{
			proj_light_mat = proj * lc->pov(i);

			if (lc->cast_shadows() && m_lighting_enabled)
			{
				glEnable(GL_DEPTH_TEST);
				glDepthMask(GL_TRUE);
				glViewport(0,
						   0,
						   m_shadow_buf->size(nsshadowbuf_object::Direction).w,
						   m_shadow_buf->size(nsshadowbuf_object::Direction).h);
				m_shadow_buf->bind(nsshadowbuf_object::Direction);
				glClear(GL_DEPTH_BUFFER_BIT);
				m_shaders.dir_shadow->bind();
				m_shaders.dir_shadow->set_proj_mat(proj_light_mat);
				_draw_scene_to_depth(m_shaders.dir_shadow);
				glViewport(0, 0, camc->screen_size().w, camc->screen_size().h);

				// Bind the shadow buffer's attachment texture for use in the dir light shader
				nsfb_object::attachment * attch =
					m_shadow_buf->fb(nsshadowbuf_object::Direction)->att(nsfb_object::att_depth);
				set_active_texture_unit(attch->m_tex_unit);
				attch->m_texture->bind();
			}

			glDepthMask(GL_FALSE);
			glDisable(GL_DEPTH_TEST);
			m_final_buf->bind();
			m_final_buf->set_draw_buffer(nsfb_object::att_color);
			m_shaders.dir_light->bind();
			m_shaders.dir_light->set_proj_light_mat(proj_light_mat);
			m_shaders.dir_light->set_lighting_enabled(m_lighting_enabled);
			m_shaders.dir_light->set_bg_color(bg_color);
			m_shaders.dir_light->set_direction(lc->transform(i).target());
			m_shaders.dir_light->set_cam_world_pos(camt->wpos());
			m_shaders.dir_light->set_fog_factor(m_fog_nf);
			m_shaders.dir_light->set_fog_color(m_fog_color);
			m_tbuffers.bind_buffers();
			_blend_dir_light(lc);
			m_tbuffers.unbind_buffers();
		}
	}
}

uint32 nsrender_system::bound_fbo()
{
	GLint params;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &params);
	return params;
}

void nsrender_system::resize_screen(const ivec2 & size)
{
	m_screen_size = size;
	nsscene * scn = nse.current_scene();
	if (scn != NULL)
	{
		nsentity * cam = scn->camera();
		if (cam != NULL)
		{
			nscam_comp * cc = cam->get<nscam_comp>();
			cc->resize_screen(m_screen_size);
		}
	}
}

const ivec2 & nsrender_system::screen_size()
{
	return m_screen_size;	
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
	// GL setup
	glFrontFace(GL_CW);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glDepthFunc(GL_LEQUAL);

	// Frambuffer setup
	set_gbuffer_fbo(nse.create_framebuffer());
	set_shadow_fbo(nse.create_framebuffer(), nse.create_framebuffer(), nse.create_framebuffer());
	set_final_fbo(nse.composite_framebuffer());

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
	m_shaders.point_shadow = cplg->load<nspoint_shadowmap_shader>(nsstring(POINT_SHADOWMAP_SHADER) + shext);
	m_shaders.spot_shadow = cplg->load<nsspot_shadowmap_shader>(nsstring(SPOT_SHADOWMAP_SHADER) + shext);
	m_shaders.sel_shader = cplg->load<nsselection_shader>(nsstring(SELECTION_SHADER) + shext);
	m_shaders.dir_shadow = cplg->load<nsdir_shadowmap_shader>(nsstring(DIR_SHADOWMAP_SHADER) + shext);
	cplg->load<nsskybox_shader>(nsstring(SKYBOX_SHADER) + shext);
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
	i_vector header_data(DEFAULT_FB_RES_X*DEFAULT_FB_RES_Y, -1);
	m_tbuffers.atomic_counter->bind();
	m_tbuffers.atomic_counter->allocate(data_, nsbuffer_object::mutable_dynamic_draw);
	m_tbuffers.atomic_counter->unbind();
	m_tbuffers.header->bind();
	m_tbuffers.header->allocate(header_data, nsbuffer_object::mutable_dynamic_draw, header_data.size());
	m_tbuffers.fragments->bind();
	m_tbuffers.fragments->allocate<packed_fragment_data>(nsbuffer_object::mutable_dynamic_draw, DEFAULT_FB_RES_X * DEFAULT_FB_RES_Y * 2);
	m_tbuffers.fragments->unbind();

	m_single_point->bind();
	fvec3 point;
	m_single_point->allocate(point, nsbuffer_object::mutable_dynamic_draw);
	m_single_point->unbind();
}

void nsrender_system::enable_debug_draw(bool pDebDraw)
{
	m_debug_draw = pDebDraw;
}

void nsrender_system::set_active_texture_unit(uint32 tex_unit)
{
	glActiveTexture(BASE_TEX_UNIT + tex_unit);
	gl_err_check("nstexture::enable");
}

void nsrender_system::set_default_mat(nsmaterial * pDefMat)
{
	m_default_mat = pDefMat;
}

void nsrender_system::set_shaders(const render_shaders & pShaders)
{
	m_shaders = pShaders;
}

void nsrender_system::toggle_debug_draw()
{
	enable_debug_draw(!m_debug_draw);
}

void nsrender_system::update()
{
	static bool sceneerr = false;
	static bool camerr = false;

	// Warning message switches (so they dont appear every frame)
	nsscene * scene = nse.current_scene();
	if (scene == NULL)
	{
		if (!sceneerr)
		{
			dprint("nsrender_system::update WARNING No current scene set");
			sceneerr = true;
		}
		return;
	}
	else
		sceneerr = false;

	nsentity * cam = scene->camera();
	if (cam == NULL)
	{
		if (!camerr)
		{
			dprint("nsrender_system::update WARNING No scene camera set");
			camerr = true;
		}
		return;
	}
	else
		camerr = false;

	m_drawcall_map.clear();
	m_shader_mat_map.clear();
	m_tdrawcall_map.clear();
	m_tshader_mat_map.clear();
	m_lights.clear();
	m_tbuffers.reset_atomic_counter();	
	add_draw_calls_from_scene(scene);
	add_lights_from_scene(scene);
	update_material_ids();
}

void nsrender_system::add_lights_from_scene(nsscene * scene)
{
	auto l_iter = scene->entities<nslight_comp>().begin();
	while (l_iter != scene->entities<nslight_comp>().end())
	{
		nslight_comp * lcomp = (*l_iter)->get<nslight_comp>();
		if (lcomp->type() == nslight_comp::l_dir)
			m_lights.dir_lights.push_back(*l_iter);
		else if (lcomp->type() == nslight_comp::l_spot)
			m_lights.spot_lights.push_back(*l_iter);
		else
			m_lights.point_lights.push_back(*l_iter);
		++l_iter;
	}
}

void nsrender_system::add_draw_calls_from_scene(nsscene * scene)
{
	// update render components and the draw call list
	fvec2 terh;
	nsrender_comp * rComp = NULL;
	nstform_comp * tComp = NULL;
	nsanim_comp * animComp = NULL;
	nsterrain_comp * terComp = NULL;
	nsmesh * currentMesh = NULL;
	nslight_comp * lc = NULL;
	nssel_comp * sc = NULL;
	bool transparent_picking = false;
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
		
		if (lc != NULL)
		{
			if (lc->update_posted())
				lc->post_update(false);
		}

		if (rComp->update_posted())
			rComp->post_update(false);

		if (currentMesh == NULL)
		{
			++iter;
			continue;
		}

		if (sc != NULL)
			transparent_picking = sc->transparent_picking_enabled();
		
		nsmesh::submesh * mSMesh = NULL;
		nsmaterial * mat = NULL;
		fmat4_vector * fTForms = NULL;
		nsmaterial_shader * shader = NULL;
		for (uint32 i = 0; i < currentMesh->count(); ++i)
		{
			mSMesh = currentMesh->sub(i);
			mat = nse.resource<nsmaterial>(rComp->material_id(i));

			if (mat == NULL)
				mat = m_default_mat;

			shader = nse.resource<nsmaterial_shader>(mat->shader_id());
			fTForms = NULL;

			if (shader == NULL)
			{
				if (mat->wireframe())
					shader = m_shaders.deflt_wireframe;
				else if (mat->alpha_blend())
					shader = m_shaders.deflt_translucent;
				else
					shader = m_shaders.deflt;
			}

			if (animComp != NULL)
				fTForms = animComp->final_transforms();

			if (terComp != NULL)
				terh = terComp->height_bounds();

			if (tComp != NULL)
			{
				draw_call dc(
						mSMesh,
						fTForms,
						tComp->transform_buffer(),
						tComp->transform_id_buffer(),
						terh,
						(*iter)->id(),
						(*iter)->plugin_id(),
						tComp->visible_count(),
						rComp->cast_shadow(),
						false);
				
				if (mat->alpha_blend())
				{
					dc.transparent_picking = transparent_picking;
					m_tdrawcall_map[mat].emplace(dc);
					m_tshader_mat_map[shader].emplace(mat);
				}
				else
				{
					m_drawcall_map[mat].emplace(dc);
					m_shader_mat_map[shader].emplace(mat);
				}
			}
		}
		
		++iter;
	}	
}

void nsrender_system::update_material_ids()
{
	uint id = 0;
	m_mat_shader_ids.clear();
	auto iter_mat = m_drawcall_map.begin();
	while (iter_mat != m_drawcall_map.end())
	{
		if (m_mat_shader_ids.emplace(iter_mat->first, id).second)
			++id;
		++iter_mat;
	}
	auto iter_tmat = m_tdrawcall_map.begin();
	while (iter_tmat != m_tdrawcall_map.end())
	{
		if (m_mat_shader_ids.emplace(iter_tmat->first, id).second)
			++id;
		++iter_tmat;
	}	
}

void nsrender_system::_blend_dir_light(nslight_comp * pLight)
{
	m_shaders.dir_light->set_ambient_intensity(pLight->intensity().y);
	m_shaders.dir_light->set_cast_shadows(pLight->cast_shadows());
	m_shaders.dir_light->set_diffuse_intensity(pLight->intensity().x);
	m_shaders.dir_light->set_light_color(pLight->color());
	m_shaders.dir_light->set_shadow_samples(pLight->shadow_samples());
	m_shaders.dir_light->set_shadow_darkness(pLight->shadow_darkness());
	m_shaders.dir_light->set_screen_size(fvec2(float(m_final_buf->size().x), float(m_final_buf->size().y)));
	m_shaders.dir_light->set_shadow_tex_size(fvec2(float(m_shadow_buf->size(nsshadowbuf_object::Direction).w), float(m_shadow_buf->size(nsshadowbuf_object::Direction).y)));
	m_shaders.dir_light->set_material_ids(m_mat_shader_ids);

	nsmesh * boundingMesh = nse.resource<nsmesh>(pLight->mesh_id());
	for (uint32 i = 0; i < boundingMesh->count(); ++i)
	{
		nsmesh::submesh * cSub = boundingMesh->sub(i);
		cSub->m_vao.bind();
		GLsizei sz = static_cast<GLsizei>(cSub->m_indices.size());
		glDrawElements(cSub->m_prim_type,
					   sz,
					   GL_UNSIGNED_INT,
					   nullptr);
        gl_err_check("_blendDirectionLight: Draw Error");
		cSub->m_vao.unbind();
	}
}

void nsrender_system::_blend_point_light(nslight_comp * pLight)
{
	m_shaders.point_light->set_ambient_intensity(pLight->intensity().y);
	m_shaders.point_light->set_cast_shadows(pLight->cast_shadows());
	m_shaders.point_light->set_diffuse_intensity(pLight->intensity().x);
	m_shaders.point_light->set_light_color(pLight->color());
	m_shaders.point_light->set_shadow_samples(pLight->shadow_samples());
	m_shaders.point_light->set_shadow_darkness(pLight->shadow_darkness());
	m_shaders.point_light->set_screen_size(fvec2(float(m_final_buf->size().x), float(m_final_buf->size().y)));
	m_shaders.point_light->set_shadow_tex_size(fvec2(float(m_shadow_buf->size(nsshadowbuf_object::Direction).w), float(m_shadow_buf->size(nsshadowbuf_object::Direction).y)));
	m_shaders.point_light->set_const_atten(pLight->atten().x);
	m_shaders.point_light->set_lin_atten(pLight->atten().y);
	m_shaders.point_light->set_exp_atten(pLight->atten().z);
	m_shaders.point_light->set_material_ids(m_mat_shader_ids);

	nsmesh * boundingMesh = nse.resource<nsmesh>(pLight->mesh_id());
	for (uint32 i = 0; i < boundingMesh->count(); ++i)
	{
		nsmesh::submesh * cSub = boundingMesh->sub(i);

		if (cSub->m_node != NULL)
			m_shaders.point_light->set_uniform("nodeTransform", cSub->m_node->m_world_tform);
		else
			m_shaders.point_light->set_uniform("nodeTransform", fmat4());

		cSub->m_vao.bind();
		glDrawElements(cSub->m_prim_type,
					   static_cast<GLsizei>(cSub->m_indices.size()),
					   GL_UNSIGNED_INT,
					   0);
		cSub->m_vao.unbind();
	}
}

void nsrender_system::_blend_spot_light(nslight_comp * pLight)
{
	m_shaders.spot_light->set_ambient_intensity(pLight->intensity().y);
	m_shaders.spot_light->set_cast_shadows(pLight->cast_shadows());
	m_shaders.spot_light->set_diffuse_intensity(pLight->intensity().x);
	m_shaders.spot_light->set_light_color(pLight->color());
	m_shaders.spot_light->set_shadow_samples(pLight->shadow_samples());
	m_shaders.spot_light->set_shadow_darkness(pLight->shadow_darkness());
	m_shaders.spot_light->set_screen_size(fvec2(float(m_final_buf->size().x),
												float(m_final_buf->size().y)));
	m_shaders.spot_light->set_shadow_tex_size(
		fvec2(float(m_shadow_buf->size(nsshadowbuf_object::Direction).w),
			  float(m_shadow_buf->size(nsshadowbuf_object::Direction).y)));
	m_shaders.spot_light->set_const_atten(pLight->atten().x);
	m_shaders.spot_light->set_lin_atten(pLight->atten().y);
	m_shaders.spot_light->set_exp_atten(pLight->atten().z);
	m_shaders.spot_light->set_cutoff(pLight->cutoff());
	m_shaders.spot_light->set_material_ids(m_mat_shader_ids);
	
	nsmesh * boundingMesh = nse.resource<nsmesh>(pLight->mesh_id());
	for (uint32 i = 0; i < boundingMesh->count(); ++i)
	{
		nsmesh::submesh * cSub = boundingMesh->sub(i);

		if (cSub->m_node != NULL)
			m_shaders.spot_light->set_uniform("nodeTransform", cSub->m_node->m_world_tform);
		else
			m_shaders.spot_light->set_uniform("nodeTransform", fmat4());

		cSub->m_vao.bind();
		glDrawElements(cSub->m_prim_type,
					   static_cast<GLsizei>(cSub->m_indices.size()),
					   GL_UNSIGNED_INT,
					   0);
		cSub->m_vao.unbind();
	}
}

uivec3 nsrender_system::shadow_fbo()
{
	nsfb_object * d = m_shadow_buf->fb(nsshadowbuf_object::Direction);
	nsfb_object * s = m_shadow_buf->fb(nsshadowbuf_object::Spot);
	nsfb_object * p = m_shadow_buf->fb(nsshadowbuf_object::Point);
	uivec3 ret;
	if (d != NULL)
		ret.x = d->gl_id();
	if (s != NULL)
		ret.y = s->gl_id();
	if (p != NULL)
		ret.z = s->gl_id();
	return ret;
}

uint32 nsrender_system::final_fbo()
{
	if (m_final_buf != NULL)
		return m_final_buf->gl_id();
	return 0;
}

uint32 nsrender_system::gbuffer_fbo()
{
	nsfb_object * fb = m_gbuffer->fb();
	if (fb != NULL)
		return fb->gl_id();
	return 0;
}

void nsrender_system::set_shadow_fbo(uint32 fbodir, uint32 fbospot, uint32 fbopoint)
{
	m_shadow_buf->set_fb(nse.framebuffer(fbodir), nsshadowbuf_object::Direction);
	m_shadow_buf->set_fb(nse.framebuffer(fbospot), nsshadowbuf_object::Spot);
	m_shadow_buf->set_fb(nse.framebuffer(fbopoint), nsshadowbuf_object::Point);
	m_shadow_buf->resize(nsshadowbuf_object::Direction, DEFAULT_DIRLIGHT_SHADOW_W, DEFAULT_DIRLIGHT_SHADOW_H);
	m_shadow_buf->resize(nsshadowbuf_object::Spot, DEFAULT_SPOTLIGHT_SHADOW_W, DEFAULT_SPOTLIGHT_SHADOW_H);
	m_shadow_buf->resize(nsshadowbuf_object::Point, DEFAULT_POINTLIGHT_SHADOW_W, DEFAULT_POINTLIGHT_SHADOW_H);
	m_shadow_buf->init();
}

void nsrender_system::set_gbuffer_fbo(uint32 fbo)
{
	m_gbuffer->set_fb(nse.framebuffer(fbo));
	m_gbuffer->resize_fb(uivec2(DEFAULT_FB_RES_X, DEFAULT_FB_RES_Y));
	m_gbuffer->init();
}

void nsrender_system::set_final_fbo(uint32 fbo)
{
	m_final_buf = nse.framebuffer(fbo);
	if (m_final_buf == NULL)
		return;

	m_final_buf->resize(DEFAULT_FB_RES_X, DEFAULT_FB_RES_Y);
	m_final_buf->set_target(nsfb_object::fb_read_draw);
	m_final_buf->bind();
	m_final_buf->create<nstex2d>("RenderedFrame", nsfb_object::att_color, FINAL_TEX_UNIT, GL_RGBA8, GL_RGBA, GL_FLOAT);
	m_final_buf->add(m_gbuffer->depth());

	nsfb_object::attachment * att = m_final_buf->create<nstex2d>("FinalPicking", nsfb_object::attach_point(nsfb_object::att_color + nsgbuf_object::col_picking), G_PICKING_TEX_UNIT, GL_RGB32UI, GL_RGB_INTEGER, GL_UNSIGNED_INT);
	if (att != NULL)
	{
		att->m_texture->set_parameter_i(nstexture::min_filter, GL_NEAREST);
		att->m_texture->set_parameter_i(nstexture::mag_filter, GL_NEAREST);
	}
	m_final_buf->update_draw_buffers();
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

void nsrender_system::_draw_call(const draw_call * dc)
{
	// Check to make sure each buffer is allocated before setting the shader attribute : un-allocated buffers
	// are fairly common because not every mesh has tangents for example.. or normals.. or whatever
	dc->submesh->m_vao.bind();

	if (dc->transform_buffer != nullptr)
	{
		dc->transform_buffer->bind();
		for (uint32 tfInd = 0; tfInd < 4; ++tfInd)
		{
			dc->submesh->m_vao.add(dc->transform_buffer, nsshader::loc_instance_tform + tfInd);
			dc->submesh->m_vao.vertex_attrib_ptr(nsshader::loc_instance_tform + tfInd, 4, GL_FLOAT, GL_FALSE, sizeof(fmat4), sizeof(fvec4)*tfInd);
			dc->submesh->m_vao.vertex_attrib_div(nsshader::loc_instance_tform + tfInd, 1);
		}
	}

	if (dc->transform_id_buffer != nullptr)
	{
		dc->transform_id_buffer->bind();
		dc->submesh->m_vao.add(dc->transform_id_buffer, nsshader::loc_ref_id);
		dc->submesh->m_vao.vertex_attrib_I_ptr(nsshader::loc_ref_id, 1, GL_UNSIGNED_INT, sizeof(uint32), 0);
		dc->submesh->m_vao.vertex_attrib_div(nsshader::loc_ref_id, 1);	
	}
	
	gl_err_check("nsrender_system::_draw_call pre");
	glDrawElementsInstanced(dc->submesh->m_prim_type,
							static_cast<GLsizei>(dc->submesh->m_indices.size()),
							GL_UNSIGNED_INT,
							0,
							dc->transform_count);
	gl_err_check("nsrender_system::_draw_call post");	

	if (dc->transform_buffer != nullptr)
	{
		dc->transform_buffer->bind();
		dc->submesh->m_vao.remove(dc->transform_buffer);
	}

	if (dc->transform_id_buffer != nullptr)
	{
		dc->transform_id_buffer->bind();
		dc->submesh->m_vao.remove(dc->transform_id_buffer);
	}
	
	dc->submesh->m_vao.unbind();
}

void nsrender_system::_draw_geometry(const shader_mat_map & sm_map,const mat_drawcall_map & mdc_map, nsentity * camera)
{
	nstform_comp * camTComp = camera->get<nstform_comp>();
	nscam_comp * camc = camera->get<nscam_comp>();
	// Go through each shader in the shader material map.. because each submesh corresponds to exactly one
	// material we can use this material as the key in to the draw call map minimizing shader switches
	shader_mat_map::const_iterator shaderIter = sm_map.begin();
	while (shaderIter != sm_map.end())
	{
		nsmaterial_shader * currentShader = shaderIter->first;
		currentShader->bind();
		currentShader->set_proj_cam_mat(camc->proj_cam());
		currentShader->set_lighting_enabled(m_lighting_enabled);
		// Now go through each material that is under the current shader, use the material as a key in the draw call map
		// to get all the draw calls associated with that material, and then draw everything. If a material for some reason
		// is not in the shader map then some items wont draw
		auto matIter = shaderIter->second.begin();
		while (matIter != shaderIter->second.end())
		{
			uint32 mat_shader_id = m_mat_shader_ids.find(*matIter)->second;

			glCullFace((*matIter)->cull_mode());

			if ((*matIter)->culling())// && !(*matIter)->alpha_blend())
				glEnable(GL_CULL_FACE);
			else
				glDisable(GL_CULL_FACE);

			currentShader->set_diffusemap_enabled((*matIter)->contains(nsmaterial::diffuse));
			currentShader->set_opacitymap_enabled((*matIter)->contains(nsmaterial::opacity));
			currentShader->set_normalmap_enabled((*matIter)->contains(nsmaterial::normal));
			currentShader->set_heightmap_enabled((*matIter)->contains(nsmaterial::height));
			currentShader->set_material_id(mat_shader_id);
			currentShader->set_color_mode((*matIter)->color_mode());
			currentShader->set_frag_color_out((*matIter)->color());
			currentShader->set_force_alpha((*matIter)->using_alpha_from_color());
			
			nsmaterial::texmap_map_const_iter cIter = (*matIter)->begin();
			while (cIter != (*matIter)->end())
			{
				nstexture * t = nse.resource<nstexture>(cIter->second);
				if (t != NULL)
				{
					set_active_texture_unit(cIter->first);
					t->bind();
				}
				++cIter;
			}

			// If this material is translucent then be sure to set the right shader stuff
			if ((*matIter)->alpha_blend())
			{
				// bind the translucency buffers to the right spots
				m_tbuffers.bind_buffers();
				currentShader->set_uniform("window_size", fvec2(float(m_final_buf->size().x), float(m_final_buf->size().y)));
			}
			// Finally get the draw calls that go with this material and iterate through all of them
			const drawcall_set & currentSet = mdc_map.find(*matIter)->second;
			drawcall_set::iterator  dcIter = currentSet.begin();
			while (dcIter != currentSet.end())
			{
				uint32 ent_id = dcIter->entity_id;
				if (dcIter->transparent_picking)
					ent_id = 0;
				
				currentShader->set_entity_id(ent_id);
				currentShader->set_plugin_id(dcIter->plugin_id);

				if (!dcIter->submesh->m_has_tex_coords)
					currentShader->set_color_mode(true);

				if (dcIter->submesh->m_node != NULL)
					currentShader->set_node_transform(dcIter->submesh->m_node->m_world_tform);
				else
					currentShader->set_node_transform(fmat4());

				if (dcIter->anim_transforms != NULL)
				{
					currentShader->set_has_bones(true);
					currentShader->set_bone_transforms(*dcIter->anim_transforms);
				}
				else
					currentShader->set_has_bones(false);

				currentShader->set_height_minmax(dcIter->height_minmax);

				_draw_call(&(*dcIter));
				++dcIter;
			}
			gl_err_check("nsrender_system::draw_geometry");

			if ((*matIter)->alpha_blend())
				m_tbuffers.unbind_buffers();

			++matIter;
		}
		++shaderIter;
	}
}

void nsrender_system::_draw_scene_to_depth(nsdepth_shader * pShader)
{

	mat_drawcall_map::iterator matIter = m_drawcall_map.begin();
	while (matIter != m_drawcall_map.end())
	{
		pShader->set_height_map_enabled(false);
		nstexture * tex = nse.resource<nstexture>(matIter->first->map_tex_id(nsmaterial::height));
		if (tex != NULL)
		{
			pShader->set_height_map_enabled(true);
			set_active_texture_unit(nsmaterial::height);
			tex->bind();
		}

		drawcall_set & currentSet = matIter->second;
		drawcall_set::iterator dcIter = currentSet.begin();
		while (dcIter != currentSet.end())
		{
			if (dcIter->submesh->m_prim_type != GL_TRIANGLES)
			{
				++dcIter;
				continue;
			}

			if (!dcIter->casts_shadows)
			{
				++dcIter;
				continue;
			}

			if (dcIter->submesh->m_node != NULL)
				pShader->set_node_transform(dcIter->submesh->m_node->m_world_tform);
			else
				pShader->set_node_transform(fmat4());

			if (dcIter->anim_transforms != NULL)
			{
				pShader->set_has_bones(true);
				pShader->set_bone_transform(*dcIter->anim_transforms);
			}
			else
				pShader->set_has_bones(false);

			pShader->set_height_minmax(dcIter->height_minmax);
			_draw_call(&(*dcIter));
			++dcIter;
		}
		++matIter;
	}
}

void nsrender_system::_stencil_point_light(nslight_comp * pLight)
{
	nsmesh * boundingMesh = nse.resource<nsmesh>(pLight->mesh_id());
	for (uint32 i = 0; i < boundingMesh->count(); ++i)
	{
		nsmesh::submesh * cSub = boundingMesh->sub(i);

		if (cSub->m_node != NULL)
			m_shaders.light_stencil->set_node_transform(cSub->m_node->m_world_tform);
		else
			m_shaders.light_stencil->set_node_transform(fmat4());

		cSub->m_vao.bind();
		glDrawElements(cSub->m_prim_type,
					   static_cast<GLsizei>(cSub->m_indices.size()),
					   GL_UNSIGNED_INT,
					   0);
		cSub->m_vao.unbind();
	}
}

void nsrender_system::_stencil_spot_light(nslight_comp * pLight)
{
	nsmesh * boundingMesh = nse.resource<nsmesh>(pLight->mesh_id());
	for (uint32 i = 0; i < boundingMesh->count(); ++i)
	{
		nsmesh::submesh * cSub = boundingMesh->sub(i);

		if (cSub->m_node != NULL)
			m_shaders.light_stencil->set_node_transform(cSub->m_node->m_world_tform);
		else
			m_shaders.light_stencil->set_node_transform(fmat4());

		cSub->m_vao.bind();
		glDrawElements(cSub->m_prim_type,
					   static_cast<GLsizei>(cSub->m_indices.size()),
					   GL_UNSIGNED_INT,
					   0);
		cSub->m_vao.unbind();
	}
}

int32 nsrender_system::draw_priority()
{
	return RENDER_SYS_DRAW_PR;
}

int32 nsrender_system::update_priority()
{
	return RENDER_SYS_UPDATE_PR;
}

void nsrender_system::_bind_gbuffer_textures()
{
	for (uint32 i = 0; i < nsgbuf_object::attrib_count; ++i)
	{
		nsfb_object::attachment * att = m_gbuffer->color(i);
		set_active_texture_unit(att->m_tex_unit);
		att->m_texture->bind();
	}
}
