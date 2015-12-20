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
  m_fog_nf(DEFAULT_FOG_FACTOR_NEAR, DEFAULT_FOG_FACTOR_FAR)
{}

nsrender_system::~nsrender_system()
{
	delete m_shadow_buf;
	delete m_gbuffer;
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

void nsrender_system::draw()
{
	bool dirlt = false;
	static bool dirlterr = false;

	if (!_valid_check())
		return;

	nsscene * scene = nse.current_scene();
	if (scene == NULL)
		return;

	nsentity * cam = scene->camera();
	if (cam == NULL)
		return;

	nscam_comp * camc = cam->get<nscam_comp>();

	m_gbuffer->bind();
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);;
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	
	// Now draw geometry
	_draw_geometry();
	m_gbuffer->unbind();
	_bind_gbuffer_textures();

	if (m_debug_draw)
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_screen_fbo);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		m_gbuffer->debug_blit(camc->screen_size());
		return;
	}

	m_final_buf->set_target(nsfb_object::fb_draw);
	m_final_buf->bind();
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	glEnable(GL_STENCIL_TEST);
	auto dLIter = scene->entities<nslight_comp>().begin();

	fmat4 projLightMat, proj;
	float ar;
	while (dLIter != scene->entities<nslight_comp>().end())
	{
		nslight_comp * lComp = (*dLIter)->get<nslight_comp>();
		nstform_comp * tComp = (*dLIter)->get<nstform_comp>();
		nstform_comp * camTComp = cam->get<nstform_comp>();
		if (tComp == NULL)
		{
			++dLIter;
			continue;
		}

		if (lComp->type() == nslight_comp::l_spot && m_lighting_enabled)
		{
			ar = float(m_shadow_buf->size(nsshadowbuf_object::Spot).w) / float(m_shadow_buf->size(nsshadowbuf_object::Spot).h);
			proj = perspective(lComp->angle()*2.0f, ar, lComp->shadow_clipping().x, lComp->shadow_clipping().y);
			for (uint32 i = 0; i < tComp->count(); ++i)
			{
				projLightMat = proj * lComp->pov(i);
				if (lComp->cast_shadows())
				{
					m_shadow_buf->bind(nsshadowbuf_object::Spot);
					glEnable(GL_DEPTH_TEST);
					glDepthMask(GL_TRUE);
					glClear(GL_DEPTH_BUFFER_BIT);
					glCullFace(GL_FRONT);

					glViewport(0, 0, m_shadow_buf->size(nsshadowbuf_object::Spot).w, m_shadow_buf->size(nsshadowbuf_object::Spot).h);

					m_shaders.spot_shadow->bind();
					m_shaders.spot_shadow->set_proj_mat(projLightMat);
					_draw_scene_to_depth(m_shaders.spot_shadow);
					glViewport(0, 0, camc->screen_size().w, camc->screen_size().h);
				}

				m_final_buf->bind();

				// Bind the spot light's attachment texture for use in the shader
				nsfb_object::attachment * attch = m_shadow_buf->fb(nsshadowbuf_object::Spot)->att(nsfb_object::att_depth);
				set_active_texture_unit(attch->m_tex_unit);
				attch->m_texture->bind();

				m_final_buf->set_draw_buffer(nsfb_object::att_none);
				glDepthMask(GL_FALSE);
				glClear(GL_STENCIL_BUFFER_BIT);
				glDisable(GL_CULL_FACE);
				glStencilFunc(GL_ALWAYS, 0, 0);
				glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
				glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);

				m_shaders.light_stencil->bind();
				m_shaders.light_stencil->set_proj_cam_mat(camc->proj_cam());
				m_shaders.light_stencil->set_transform(lComp->transform(i));
				_stencil_spot_light(lComp);
				m_final_buf->set_draw_buffer(nsfb_object::att_color);
				glDisable(GL_DEPTH_TEST);
				glEnable(GL_CULL_FACE);
				glCullFace(GL_FRONT);
				glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
				m_shaders.spot_light->bind();
				m_shaders.spot_light->set_cam_world_pos(camTComp->wpos());
				m_shaders.spot_light->set_position(tComp->wpos(i));
				m_shaders.spot_light->set_max_depth(lComp->shadow_clipping().y - lComp->shadow_clipping().x);
				m_shaders.spot_light->set_transform(lComp->transform(i));
				m_shaders.spot_light->set_proj_cam_mat(camc->proj_cam());
				m_shaders.spot_light->set_proj_light_mat(projLightMat);
				m_shaders.spot_light->set_direction(tComp->dvec(nstform_comp::dir_target, i));
				_blend_spot_light(lComp);
			}
		}
		else if (lComp->type() == nslight_comp::l_point && m_lighting_enabled)
		{
			ar = float(m_shadow_buf->size(nsshadowbuf_object::Point).w) / float(m_shadow_buf->size(nsshadowbuf_object::Point).h);
			proj = perspective(90.0f, ar, lComp->shadow_clipping().x, lComp->shadow_clipping().y);
			for (uint32 i = 0; i < tComp->count(); ++i)
			{
				projLightMat = proj * lComp->pov(i);
				glCullFace(GL_FRONT);
				if (lComp->cast_shadows())
				{
					m_shadow_buf->bind(nsshadowbuf_object::Point);
					glEnable(GL_DEPTH_TEST);
					glDepthMask(GL_TRUE);
					glClear(GL_DEPTH_BUFFER_BIT);

					glViewport(0, 0, m_shadow_buf->size(nsshadowbuf_object::Point).w, m_shadow_buf->size(nsshadowbuf_object::Point).h);

					m_shaders.point_shadow->bind();
					m_shaders.point_shadow->set_light_pos(tComp->wpos(i));
					m_shaders.point_shadow->set_max_depth(lComp->shadow_clipping().y - lComp->shadow_clipping().x);
					m_shaders.point_shadow->set_proj_mat(proj);
					fmat4 f;
					f.set_column(3,fvec4(tComp->wpos(i)*-1.0f,1.0f));
					m_shaders.point_shadow->set_inverse_trans_mat(f);
					_draw_scene_to_depth(m_shaders.point_shadow);
					glViewport(0, 0, camc->screen_size().w, camc->screen_size().h);
				}

				m_final_buf->bind();

				// bind the point light shadow fb's attachment for use in the shader
				nsfb_object::attachment * attch = m_shadow_buf->fb(nsshadowbuf_object::Point)->att(nsfb_object::att_depth);
				set_active_texture_unit(attch->m_tex_unit);
				attch->m_texture->bind();

				m_final_buf->set_draw_buffer(nsfb_object::att_none);
				glDepthMask(GL_FALSE);
				glClear(GL_STENCIL_BUFFER_BIT);
				glDisable(GL_CULL_FACE);
				glStencilFunc(GL_ALWAYS, 0, 0);
				glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
				glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);
				m_shaders.light_stencil->bind();
				m_shaders.light_stencil->set_proj_cam_mat(camc->proj_cam());
				m_shaders.light_stencil->set_transform(lComp->transform(i));
				_stencil_point_light(lComp);

				m_final_buf->set_draw_buffer(nsfb_object::att_color);
				glEnable(GL_CULL_FACE);
				glCullFace(GL_FRONT);
				glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
				glDisable(GL_DEPTH_TEST);
				m_shaders.point_light->bind();
				m_shaders.point_light->set_cam_world_pos(camTComp->wpos());
				m_shaders.point_light->set_position(tComp->wpos(i));
				m_shaders.point_light->set_max_depth(lComp->shadow_clipping().y - lComp->shadow_clipping().x);
				m_shaders.point_light->set_transform(lComp->transform(i));
				m_shaders.point_light->set_proj_cam_mat(camc->proj_cam());
				_blend_point_light(lComp);
			}
		}
		else if (lComp->type() == nslight_comp::l_dir)
		{
			ar = float(m_shadow_buf->size(nsshadowbuf_object::Direction).w) / float(m_shadow_buf->size(nsshadowbuf_object::Direction).h);
			proj = perspective(160.0f, ar, lComp->shadow_clipping().x, lComp->shadow_clipping().y);
			for (uint32 i = 0; i < tComp->count(); ++i)
			{
				projLightMat = proj * tComp->pov();
				glCullFace(GL_BACK);

				if (lComp->cast_shadows() && m_lighting_enabled)
				{
					m_shadow_buf->bind(nsshadowbuf_object::Direction);
					glEnable(GL_DEPTH_TEST);
					glDepthMask(GL_TRUE);
					glClear(GL_DEPTH_BUFFER_BIT);
					glViewport(0, 0, m_shadow_buf->size(nsshadowbuf_object::Direction).w, m_shadow_buf->size(nsshadowbuf_object::Direction).h);

					m_shaders.dir_shadow->bind();
					m_shaders.dir_shadow->set_proj_mat(projLightMat);
					_draw_scene_to_depth(m_shaders.dir_shadow);
					glViewport(0, 0, camc->screen_size().w, camc->screen_size().h);
				}


				m_final_buf->bind();

				// Bind the dir light's attachment texture for use in the shader
				nsfb_object::attachment * attch = m_shadow_buf->fb(nsshadowbuf_object::Direction)->att(nsfb_object::att_depth);
				set_active_texture_unit(attch->m_tex_unit);
				attch->m_texture->bind();

				m_final_buf->set_draw_buffer(nsfb_object::att_color);
				glClear(GL_STENCIL_BUFFER_BIT);
				glDepthMask(GL_FALSE);
				glDisable(GL_DEPTH_TEST);
				glStencilFunc(GL_EQUAL, 0, 0xFF);
				m_shaders.dir_light->bind();
				m_shaders.dir_light->init_uniforms();
				m_shaders.dir_light->set_proj_light_mat(projLightMat);
				m_shaders.dir_light->set_lighting_enabled(m_lighting_enabled);
				m_shaders.dir_light->set_bg_color(scene->bg_color());
				m_shaders.dir_light->set_direction(tComp->transform(i).target());
				m_shaders.dir_light->set_cam_world_pos(camTComp->wpos());
				m_shaders.dir_light->set_fog_factor(m_fog_nf);
				m_shaders.dir_light->set_fog_color(m_fog_color);
				_blend_dir_light(lComp);
				dirlt = true;
				dirlterr = false;
			}
		}

		++dLIter;
	}
	if (!dirlt && !dirlterr)
	{
		dprint("nsrender_system::draw WARNING No directional light - scene will not be drawn. Add a directional light to the scene.");
		dirlterr = true;
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
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glDepthFunc(GL_LEQUAL);
	glViewport(0, 0, DEFAULT_FB_RES_X, DEFAULT_FB_RES_Y);

	// Frambuffer setup
	set_gbuffer_fbo(nse.create_framebuffer());
	set_shadow_fbo(nse.create_framebuffer(), nse.create_framebuffer(), nse.create_framebuffer());
	set_final_fbo(nse.composite_framebuffer());

	// Load default shaders
	nsplugin * cplg = nse.core();	
	nsstring shext = nsstring(DEFAULT_SHADER_EXTENSION);
	m_shaders.deflt = cplg->load<nsmaterial_shader>(nsstring(DEFAULT_GBUFFER_SHADER) + shext);
	m_shaders.deflt_wireframe = cplg->load<nsmaterial_shader>(nsstring(DEFAULT_GBUFFER_WIREFRAME_SHADER) + shext);
	m_shaders.light_stencil = cplg->load<nslight_stencil_shader>(nsstring(DEFAULT_LIGHTSTENCIL_SHADER) + shext);
	m_shaders.dir_light = cplg->load<nsdir_light_shader>(nsstring(DEFAULT_DIRLIGHT_SHADER) + shext);
	m_shaders.point_light = cplg->load<nspoint_light_shader>(nsstring(DEFAULT_POINTLIGHT_SHADER) + shext);
	m_shaders.spot_light = cplg->load<nsspot_light_shader>(nsstring(DEFAULT_SPOTLIGHT_SHADER) + shext);
	m_shaders.point_shadow = cplg->load<nspoint_shadowmap_shader>(nsstring(DEFAULT_POINTSHADOWMAP_SHADER) + shext);
	m_shaders.spot_shadow = cplg->load<nsspot_shadowmap_shader>(nsstring(DEFAULT_SPOTSHADOWMAP_SHADER) + shext);
	m_shaders.dir_shadow = cplg->load<nsdir_shadowmap_shader>(nsstring(DEFAULT_DIRSHADOWMAP_SHADER) + shext);
	cplg->load<nsskybox_shader>(nsstring(DEFAULT_SKYBOX_SHADER) + shext);
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

	// update render components and the draw call list
	fvec2 terh;
	nsrender_comp * rComp = NULL;
	nstform_comp * tComp = NULL;
	nsanim_comp * animComp = NULL;
	nsterrain_comp * terComp = NULL;
	nsmesh * currentMesh = NULL;
	nslight_comp * lc = NULL;

	auto iter = scene->entities<nsrender_comp>().begin();
	while (iter != scene->entities<nsrender_comp>().end())
	{
		terh.set(0.0f, 1.0f);
		rComp = (*iter)->get<nsrender_comp>();
		tComp = (*iter)->get<nstform_comp>();
		lc = (*iter)->get<nslight_comp>();
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
				else
					shader = m_shaders.deflt;
			}

			if (animComp != NULL)
				fTForms = animComp->final_transforms();

			if (terComp != NULL)
				terh = terComp->height_bounds();

			if (tComp != NULL)
			{
				m_drawcall_map[mat].emplace(
					draw_call(
						mSMesh,
						fTForms,
						tComp->transform_buffer(),
						tComp->transform_id_buffer(),
						terh,
						(*iter)->id(),
						(*iter)->plugin_id(),
						tComp->visible_count(),
						rComp->cast_shadow()));
				m_shader_mat_map[shader].insert(mat);
			}
		}
		
		++iter;
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
	m_shaders.dir_light->set_screen_size(fvec2(static_cast<float>(m_final_buf->size().x), static_cast<float>(m_final_buf->size().y)));
	m_shaders.dir_light->set_shadow_tex_size(fvec2(static_cast<float>(m_shadow_buf->size(nsshadowbuf_object::Direction).w), static_cast<float>(m_shadow_buf->size(nsshadowbuf_object::Direction).y)));

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
	m_shaders.point_light->set_screen_size(fvec2(static_cast<float>(m_final_buf->size().x), static_cast<float>(m_final_buf->size().y)));
	m_shaders.point_light->set_shadow_tex_size(fvec2(static_cast<float>(m_shadow_buf->size(nsshadowbuf_object::Direction).w), static_cast<float>(m_shadow_buf->size(nsshadowbuf_object::Direction).y)));
	m_shaders.point_light->set_const_atten(pLight->atten().x);
	m_shaders.point_light->set_lin_atten(pLight->atten().y);
	m_shaders.point_light->set_exp_atten(pLight->atten().z);


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
	m_shaders.spot_light->set_screen_size(fvec2(static_cast<float>(m_final_buf->size().x),
												static_cast<float>(m_final_buf->size().y)));
	m_shaders.spot_light->set_shadow_tex_size(
		fvec2(static_cast<float>(m_shadow_buf->size(nsshadowbuf_object::Direction).w),
			  static_cast<float>(m_shadow_buf->size(nsshadowbuf_object::Direction).y)));
	m_shaders.spot_light->set_const_atten(pLight->atten().x);
	m_shaders.spot_light->set_lin_atten(pLight->atten().y);
	m_shaders.spot_light->set_exp_atten(pLight->atten().z);
	m_shaders.spot_light->set_cutoff(pLight->cutoff());

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
	m_final_buf->add(m_final_buf->create<nstex2d>("RenderedFrame", nsfb_object::att_color, FINAL_TEX_UNIT, GL_RGBA32F, GL_RGBA, GL_FLOAT));
	m_final_buf->add(m_gbuffer->depth());
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

void nsrender_system::_draw_call(drawcall_set::iterator pDCIter)
{
	// Check to make sure each buffer is allocated before setting the shader attribute : un-allocated buffers
	// are fairly common because not every mesh has tangents for example.. or normals.. or whatever

	pDCIter->submesh->m_vao.bind();
	pDCIter->transform_buffer->bind();
	for (uint32 tfInd = 0; tfInd < 4; ++tfInd)
	{
		pDCIter->submesh->m_vao.add(pDCIter->transform_buffer, nsshader::loc_instance_tform + tfInd);
		pDCIter->submesh->m_vao.vertex_attrib_ptr(nsshader::loc_instance_tform + tfInd, 4, GL_FLOAT, GL_FALSE, sizeof(fmat4), sizeof(fvec4)*tfInd);
		pDCIter->submesh->m_vao.vertex_attrib_div(nsshader::loc_instance_tform + tfInd, 1);
	}

	pDCIter->transform_id_buffer->bind();
	pDCIter->submesh->m_vao.add(pDCIter->transform_id_buffer, nsshader::loc_ref_id);
	pDCIter->submesh->m_vao.vertex_attrib_I_ptr(nsshader::loc_ref_id, 1, GL_UNSIGNED_INT, sizeof(uint32), 0);
	pDCIter->submesh->m_vao.vertex_attrib_div(nsshader::loc_ref_id, 1);

	gl_err_check("nsrender_system::_draw_call pre");
	glDrawElementsInstanced(pDCIter->submesh->m_prim_type,
							static_cast<GLsizei>(pDCIter->submesh->m_indices.size()),
							GL_UNSIGNED_INT,
							0,
							pDCIter->transform_count);
	gl_err_check("nsrender_system::_draw_call post");

	pDCIter->transform_buffer->bind();
	pDCIter->submesh->m_vao.remove(pDCIter->transform_buffer);
	pDCIter->transform_id_buffer->bind();
	pDCIter->submesh->m_vao.remove(pDCIter->transform_id_buffer);
	pDCIter->submesh->m_vao.unbind();
}

void nsrender_system::_draw_geometry()
{
	nsscene * scene = nse.current_scene();
	if (scene == NULL)
		return;
	nsentity * cam = scene->camera();
	if (cam == NULL)
		return;
	nstform_comp * camTComp = cam->get<nstform_comp>();
	nscam_comp * camc = cam->get<nscam_comp>();
	
	// Go through each shader in the shader material map.. because each submesh corresponds to exactly one
	// material we can use this material as the key in to the draw call map minimizing shader switches
	shader_mat_map::iterator shaderIter = m_shader_mat_map.begin();
	while (shaderIter != m_shader_mat_map.end())
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

			glCullFace((*matIter)->cull_mode());

			if ((*matIter)->culling())
				glEnable(GL_CULL_FACE);
			else
				glDisable(GL_CULL_FACE);

			currentShader->set_diffusemap_enabled((*matIter)->contains(nsmaterial::diffuse));
			currentShader->set_opacitymap_enabled((*matIter)->contains(nsmaterial::opacity));
			currentShader->set_normalmap_enabled((*matIter)->contains(nsmaterial::normal));
			currentShader->set_heightmap_enabled((*matIter)->contains(nsmaterial::height));

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

			currentShader->set_color_mode((*matIter)->color_mode());
			currentShader->set_frag_color_out((*matIter)->color());
			currentShader->set_specular_power((*matIter)->specular_power());
			currentShader->set_specular_color((*matIter)->specular_color());
			currentShader->set_specular_intensity((*matIter)->specular_intensity());

			// Finally get the draw calls that go with this material and iterate through all of them
			drawcall_set & currentSet = m_drawcall_map[*matIter];
			drawcall_set::iterator  dcIter = currentSet.begin();
			while (dcIter != currentSet.end())
			{
				currentShader->set_entity_id(dcIter->entity_id);
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

				_draw_call(dcIter);
				++dcIter;
			}
			gl_err_check("nsrender_system::draw_geometry");
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
		drawcall_set::iterator  dcIter = currentSet.begin();
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
			_draw_call(dcIter);
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

nsrender_system::draw_call::draw_call(nsmesh::submesh * pSubMesh, 
									  fmat4_vector * pAnimTransforms,
									  nsbuffer_object * pTransformBuffer,
									  nsbuffer_object * pTransformIDBuffer,
									  const fvec2 & heightMinMax,
									  uint32 pEntID,
									  uint32 plugID,
									  uint32 pNumTransforms, 
									  bool pCastShadows) :
	submesh(pSubMesh),
	anim_transforms(pAnimTransforms),
	transform_buffer(pTransformBuffer),
	transform_id_buffer(pTransformIDBuffer),
	height_minmax(heightMinMax),
	entity_id(pEntID),
	plugin_id(plugID),
	transform_count(pNumTransforms),
	casts_shadows(pCastShadows)
{}

nsrender_system::draw_call::~draw_call()
{}

bool nsrender_system::draw_call::operator<(const draw_call & rhs) const
{
	if (entity_id < rhs.entity_id)
		return true;
	else if (entity_id == rhs.entity_id)
		return submesh < rhs.submesh;
	return false;
}

bool nsrender_system::draw_call::operator<=(const draw_call & rhs) const
{
	return (*this < rhs || *this == rhs);
}

bool nsrender_system::draw_call::operator>(const draw_call & rhs) const
{
	return !(*this <= rhs);
}

bool nsrender_system::draw_call::operator>=(const draw_call & rhs) const
{
	return !(*this < rhs);
}

bool nsrender_system::draw_call::operator==(const draw_call & rhs) const
{
	return (entity_id == rhs.entity_id) && (submesh == rhs.submesh);
}

bool nsrender_system::draw_call::operator!=(const draw_call & rhs) const
{
	return !(*this == rhs);
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

bool nsrender_system::render_shaders::error()
{
	return (
		deflt->error() != nsshader::error_none ||
		deflt_wireframe->error() != nsshader::error_none ||
		light_stencil->error() != nsshader::error_none ||
		dir_light->error() != nsshader::error_none ||
		point_light->error() != nsshader::error_none ||
		spot_light->error() != nsshader::error_none ||
		point_shadow->error() != nsshader::error_none ||
		spot_shadow->error() != nsshader::error_none ||
		dir_shadow->error() != nsshader::error_none );
}

bool nsrender_system::render_shaders::valid()
{
	return (
		deflt != NULL &&
		deflt_wireframe != NULL &&
		light_stencil != NULL &&
		dir_light != NULL &&
		point_light != NULL &&
		spot_light != NULL &&
		point_shadow != NULL &&
		spot_shadow != NULL &&
		dir_shadow != NULL );
}
