/*! 
	\file nsrendersystem.cpp
	
	\brief Definition file for nsrender_system class

	This file contains all of the neccessary definitions for the nsrender_system class.

	\author Daniel Randle
	\date November 23 2013
	\copywrite Earth Banana Games 2013
*/

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

nsrender_system::nsrender_system() :
m_drawcall_map(),
m_gbuffer(new NSGBuffer()),
m_final_buf(NULL),
m_shadow_buf(new NSShadowBuffer()),
m_shaders(),
m_debug_draw(false),
m_earlyz_enabled(false),
m_lighting_enabled(true),
m_screen_fbo(0),
NSSystem()
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
	if (nsengine.currentScene() != NULL)
	{
		m_final_buf->setTarget(NSFrameBuffer::Read);
		m_final_buf->bind();
		m_final_buf->setReadBuffer(NSFrameBuffer::Color);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_STENCIL_TEST);
		glDisable(GL_BLEND);
		glBlitFramebuffer(0, 0, DEFAULT_FB_RES_X, DEFAULT_FB_RES_Y, 0, 0, DEFAULT_FB_RES_X, DEFAULT_FB_RES_Y, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		m_final_buf->setReadBuffer(NSFrameBuffer::None);
		m_final_buf->unbind();
	}
	else
	{
		glClearColor(0.11f, 0.11f, 0.11f, 0.11f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
}

void nsrender_system::enable_earlyz(bool pEnable)
{
	m_earlyz_enabled = pEnable;
}

void nsrender_system::enable_lighting(bool pEnable)
{
	m_lighting_enabled = pEnable;
	NSFrameBuffer * mCur = nsengine.framebuffer(bound_fbo());

	m_gbuffer->bind();
	if (pEnable)
		m_gbuffer->fb()->updateDrawBuffers();
	else
	{
		NSFrameBuffer::AttachmentPointArray ap;
		ap.push_back(NSFrameBuffer::Color + NSGBuffer::Diffuse);
		ap.push_back(NSFrameBuffer::Color + NSGBuffer::Picking);
		m_gbuffer->fb()->setDrawBuffers(&ap);
	}
	
	mCur->bind();
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

	nsscene * scene = nsengine.currentScene();
	if (scene == NULL)
		return;

	nsentity * cam = scene->camera();
	if (cam == NULL)
		return;

	NSCamComp * camc = cam->get<NSCamComp>();

	m_gbuffer->bind();
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);;
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);;
	if (m_earlyz_enabled)
	{
		glDepthFunc(GL_LESS);
		
		m_gbuffer->enableColorWrite(false);

		m_shaders.early_z->bind();
		m_shaders.early_z->set_proj_mat(camc->projCam());
		_draw_scene_to_depth(m_shaders.early_z);

		m_shaders.xfb_earlyz->bind();
		m_shaders.xfb_earlyz->set_proj_cam_mat(camc->projCam());
		_draw_scene_to_depth_xfb(m_shaders.xfb_earlyz);

		m_gbuffer->enableColorWrite(true);
		glDepthMask(GL_FALSE);
		glDepthFunc(GL_LEQUAL);
	}
	
	// Now draw geometry
	GLError("nstexture::_drawTransformFeedbacks");
	//_drawTransformFeedbacks();
	GLError("nstexture::_drawTransformFeedbacks");
	_draw_geometry();
	GLError("nstexture::_drawTransformFeedbacks");
	m_gbuffer->unbind();
	m_gbuffer->enableTextures();


	if (m_debug_draw)
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_screen_fbo);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		m_gbuffer->debugBlit(camc->dim());
		return;
	}


	m_final_buf->setTarget(NSFrameBuffer::Draw);
	m_final_buf->bind();
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	glEnable(GL_STENCIL_TEST);
	auto dLIter = scene->entities<NSLightComp>().begin();

	fmat4 projLightMat, proj;
	float ar;
	while (dLIter != scene->entities<NSLightComp>().end())
	{
		NSLightComp * lComp = (*dLIter)->get<NSLightComp>();
		NSTFormComp * tComp = (*dLIter)->get<NSTFormComp>();
		NSTFormComp * camTComp = cam->get<NSTFormComp>();
		if (tComp == NULL)
		{
			++dLIter;
			continue;
		}

		if (lComp->type() == NSLightComp::Spot && m_lighting_enabled)
		{
			ar = float(m_shadow_buf->dim(NSShadowBuffer::Spot).w) / float(m_shadow_buf->dim(NSShadowBuffer::Spot).h);
			proj = perspective(lComp->angle()*2.0f, ar, lComp->shadowClipping().x, lComp->shadowClipping().y);
			for (uint32 i = 0; i < tComp->count(); ++i)
			{
				projLightMat = proj * lComp->pov(i);
				if (lComp->castShadows())
				{
					m_shadow_buf->bind(NSShadowBuffer::Spot);
					glEnable(GL_DEPTH_TEST);
					glDepthMask(GL_TRUE);
					glClear(GL_DEPTH_BUFFER_BIT);
					glCullFace(GL_FRONT);

					glViewport(0, 0, m_shadow_buf->dim(NSShadowBuffer::Spot).w, m_shadow_buf->dim(NSShadowBuffer::Spot).h);

					m_shaders.spot_shadow->bind();
					m_shaders.spot_shadow->set_proj_mat(projLightMat);
					_draw_scene_to_depth(m_shaders.spot_shadow);

					//mShaders.mXFBSpotShadowMap->bind();
					//mShaders.mXFBSpotShadowMap->setProjLightMat(projLightMat);
					//_drawSceneToDepthXFB(mShaders.mXFBSpotShadowMap);

					glViewport(0, 0, camc->dim().w, camc->dim().h);
				}

				m_final_buf->bind();
				m_shadow_buf->enable(NSShadowBuffer::Spot);
				m_final_buf->setDrawBuffer(NSFrameBuffer::None);
				glDepthMask(GL_FALSE);
				glClear(GL_STENCIL_BUFFER_BIT);
				glDisable(GL_CULL_FACE);
				glStencilFunc(GL_ALWAYS, 0, 0);
				glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
				glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);

				m_shaders.light_stencil->bind();
				m_shaders.light_stencil->set_proj_cam_mat(camc->projCam());
				m_shaders.light_stencil->set_transform(lComp->transform(i));
				_stencil_spot_light(lComp);
				m_final_buf->setDrawBuffer(NSFrameBuffer::Color);
				glDisable(GL_DEPTH_TEST);
				glEnable(GL_CULL_FACE);
				glCullFace(GL_FRONT);
				glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
				m_shaders.spot_light->bind();
				m_shaders.spot_light->set_cam_world_pos(camTComp->wpos());
				m_shaders.spot_light->set_position(tComp->wpos(i));
				m_shaders.spot_light->set_max_depth(lComp->shadowClipping().y - lComp->shadowClipping().x);
				m_shaders.spot_light->set_transform(lComp->transform(i));
				m_shaders.spot_light->set_proj_cam_mat(camc->projCam());
				m_shaders.spot_light->set_proj_light_mat(projLightMat);
				m_shaders.spot_light->set_direction(tComp->dirVec(NSTFormComp::Target, i));
				_blend_spot_light(lComp);
			}
		}
		else if (lComp->type() == NSLightComp::Point && m_lighting_enabled)
		{
			ar = float(m_shadow_buf->dim(NSShadowBuffer::Point).w) / float(m_shadow_buf->dim(NSShadowBuffer::Point).h);
			proj = perspective(90.0f, ar, lComp->shadowClipping().x, lComp->shadowClipping().y);
			for (uint32 i = 0; i < tComp->count(); ++i)
			{
				projLightMat = proj * lComp->pov(i);
				glCullFace(GL_FRONT);
				if (lComp->castShadows())
				{
					m_shadow_buf->bind(NSShadowBuffer::Point);
					glEnable(GL_DEPTH_TEST);
					glDepthMask(GL_TRUE);
					glClear(GL_DEPTH_BUFFER_BIT);

					glViewport(0, 0, m_shadow_buf->dim(NSShadowBuffer::Point).w, m_shadow_buf->dim(NSShadowBuffer::Point).h);

					m_shaders.point_shadow->bind();
					m_shaders.point_shadow->set_light_pos(tComp->wpos(i));
					m_shaders.point_shadow->set_max_depth(lComp->shadowClipping().y - lComp->shadowClipping().x);
					m_shaders.point_shadow->set_proj_mat(proj);
					fmat4 f;
					f.setColumn(3,fvec4(tComp->wpos(i)*-1.0f,1.0f));
					m_shaders.point_shadow->set_inverse_trans_mat(f);
					_draw_scene_to_depth(m_shaders.point_shadow);

					//mShaders.mXFBPointShadowMap->bind();
					//mShaders.mXFBPointShadowMap->setLightPos(tComp->wpos(i));
					//mShaders.mXFBPointShadowMap->setMaxDepth(lComp->shadowClipping().y - lComp->shadowClipping().x);
					//mShaders.mXFBPointShadowMap->setProjMat(proj);
					//mShaders.mXFBPointShadowMap->setInverseTMat(translationMat4(tComp->wpos(i)*-1));
					//_drawSceneToDepthXFB(mShaders.mXFBPointShadowMap);

					glViewport(0, 0, camc->dim().w, camc->dim().h);
				}

				m_final_buf->bind();
				m_shadow_buf->enable(NSShadowBuffer::Point);
				m_final_buf->setDrawBuffer(NSFrameBuffer::None);
				glDepthMask(GL_FALSE);
				glClear(GL_STENCIL_BUFFER_BIT);
				glDisable(GL_CULL_FACE);
				glStencilFunc(GL_ALWAYS, 0, 0);
				glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
				glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);
				m_shaders.light_stencil->bind();
				m_shaders.light_stencil->set_proj_cam_mat(camc->projCam());
				m_shaders.light_stencil->set_transform(lComp->transform(i));
				_stencil_point_light(lComp);

				m_final_buf->setDrawBuffer(NSFrameBuffer::Color);
				glEnable(GL_CULL_FACE);
				glCullFace(GL_FRONT);
				glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
				glDisable(GL_DEPTH_TEST);
				m_shaders.point_light->bind();
				m_shaders.point_light->set_cam_world_pos(camTComp->wpos());
				m_shaders.point_light->set_position(tComp->wpos(i));
				m_shaders.point_light->set_max_depth(lComp->shadowClipping().y - lComp->shadowClipping().x);
				m_shaders.point_light->set_transform(lComp->transform(i));
				m_shaders.point_light->set_proj_cam_mat(camc->projCam());
				_blend_point_light(lComp);
			}
		}
		else if (lComp->type() == NSLightComp::Directional)
		{
			ar = float(m_shadow_buf->dim(NSShadowBuffer::Direction).w) / float(m_shadow_buf->dim(NSShadowBuffer::Direction).h);
			proj = perspective(160.0f, ar, lComp->shadowClipping().x, lComp->shadowClipping().y);
			for (uint32 i = 0; i < tComp->count(); ++i)
			{
				projLightMat = proj * tComp->pov();
				glCullFace(GL_BACK);

				if (lComp->castShadows() && m_lighting_enabled)
				{
					m_shadow_buf->bind(NSShadowBuffer::Direction);
					glEnable(GL_DEPTH_TEST);
					glDepthMask(GL_TRUE);
					glClear(GL_DEPTH_BUFFER_BIT);
					glViewport(0, 0, m_shadow_buf->dim(NSShadowBuffer::Direction).w, m_shadow_buf->dim(NSShadowBuffer::Direction).h);

					m_shaders.dir_shadow->bind();
					m_shaders.dir_shadow->set_proj_mat(projLightMat);
					_draw_scene_to_depth(m_shaders.dir_shadow);

					m_shaders.xfb_dir_shadow->bind();
					m_shaders.xfb_dir_shadow->set_proj_light_mat(projLightMat);
					_draw_scene_to_depth_xfb(m_shaders.xfb_dir_shadow);

					glViewport(0, 0, camc->dim().w, camc->dim().h);
				}


				m_final_buf->bind();
				m_shadow_buf->enable(NSShadowBuffer::Direction);
				m_final_buf->setDrawBuffer(NSFrameBuffer::Color);
				glClear(GL_STENCIL_BUFFER_BIT);
				glDepthMask(GL_FALSE);
				glDisable(GL_DEPTH_TEST);
				glStencilFunc(GL_EQUAL, 0, 0xFF);
				m_shaders.dir_light->bind();
				m_shaders.dir_light->init_uniforms();
				m_shaders.dir_light->set_proj_light_mat(projLightMat);
				m_shaders.dir_light->set_lighting_enabled(m_lighting_enabled);
				m_shaders.dir_light->set_bg_color(scene->bg_color());
				m_shaders.dir_light->set_direction(tComp->dirVec(NSTFormComp::Target, i));
				m_shaders.dir_light->set_cam_world_pos(camTComp->wpos());
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
	nsscene * scn = nsengine.currentScene();
	if (scn != NULL)
	{
		nsentity * cam = scn->camera();
		if (cam != NULL)
		{
			NSCamComp * cc = cam->get<NSCamComp>();
			cc->resize(m_screen_size);
		}
	}
}

const ivec2 & nsrender_system::screen_size()
{
	return m_screen_size;	
}

bool nsrender_system::debug_draw()
{
	return m_debug_draw;
}

nsmaterial * nsrender_system::default_mat()
{
	return m_default_mat;
}


// bool nsrender_system::handleEvent(NSEvent * pEvent)
// {
// 	nsscene * scene = nsengine.currentScene();
// 	if (scene == NULL)
// 		return false;
	
// 	if (pEvent == NULL)
// 	{
// 		dprint("nsrender_system::handleEvent Event is NULL - bad bad bad");
// 		return false;
// 	}

// 	bool handled = false;

// 	if (pEvent->mName == RENDER_DEBUG)
// 	{
// 		NSInputKeyEvent * kEvent = static_cast<NSInputKeyEvent*>(pEvent);

// 		// a lot of nonsense to get to this point - just put in debug mode if the debug
// 		// mode event is received
// 		if (kEvent->mPorR == 1)
// 		{
// 			toggleDebugDraw();
// 			handled = true;
// 		}
// 	}
// 	return handled;
// }

void nsrender_system::init()
{
	//nsengine.events()->addListener(this, NSEvent::InputKey);
	glFrontFace(GL_CW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glDepthFunc(GL_LEQUAL);
	glViewport(0, 0, DEFAULT_FB_RES_X, DEFAULT_FB_RES_Y);
	set_gbuffer_fbo(nsengine.createFramebuffer());
	set_shadow_fbo(nsengine.createFramebuffer(), nsengine.createFramebuffer(), nsengine.createFramebuffer());
}

void nsrender_system::enable_debug_draw(bool pDebDraw)
{
	m_debug_draw = pDebDraw;
}

void nsrender_system::set_default_mat(nsmaterial * pDefMat)
{
	m_default_mat = pDefMat;
}

void nsrender_system::set_shaders(const RenderShaders & pShaders)
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

	//nsengine.events()->process(this);

	// Warning message switches (so they dont appear every frame)
	nsscene * scene = nsengine.currentScene();
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
	 m_xfb_draws.clear();

	// update render components and the draw call list
	 fvec2 terh;
	 NSRenderComp * rComp = NULL;
	 NSTFormComp * tComp = NULL;
	 NSAnimComp * animComp = NULL;
	 NSTerrainComp * terComp = NULL;
	 nsmesh * currentMesh = NULL;
	 NSLightComp * lc = NULL;

	auto iter = scene->entities<NSRenderComp>().begin();
	while (iter != scene->entities<NSRenderComp>().end())
	{
		terh.set(0.0f, 1.0f);
		rComp = (*iter)->get<NSRenderComp>();
		tComp = (*iter)->get<NSTFormComp>();
		lc = (*iter)->get<NSLightComp>();
		animComp = (*iter)->get<NSAnimComp>();
		terComp = (*iter)->get<NSTerrainComp>();
		currentMesh = nsengine.resource<nsmesh>(rComp->meshID());

		if (lc != NULL)
		{
			if (lc->updatePosted())
			{
				//tComp->setScale(lc->scaling());
				lc->postUpdate(false);
			}
		}

		if (rComp->updatePosted())
			rComp->postUpdate(false);

		if (currentMesh == NULL)
		{
			++iter;
			continue;
		}

		if (tComp->transformFeedback())
			m_xfb_draws.emplace((*iter));
		else
		{
			nsmesh::submesh * mSMesh = NULL;
			nsmaterial * mat = NULL;
			fmat4array * fTForms = NULL;
			nsmaterial_shader * shader = NULL;
			for (uint32 i = 0; i < currentMesh->count(); ++i)
			{
				mSMesh = currentMesh->sub(i);
				mat = nsengine.resource<nsmaterial>(rComp->materialID(i));

				if (mat == NULL)
					mat = m_default_mat;

				shader = nsengine.resource<nsmaterial_shader>(mat->shader_id());
				fTForms = NULL;

				if (shader == NULL)
					shader = m_shaders.deflt;

				if (animComp != NULL)
					fTForms = animComp->finalTransforms();

				if (terComp != NULL)
					terh = terComp->heightBounds();

				if (tComp != NULL)
				{
					m_drawcall_map[mat].emplace(draw_call(mSMesh,
						fTForms,
						tComp->transformBuffer(),
						tComp->transformIDBuffer(),
						terh,
						(*iter)->id(),
						(*iter)->plugin_id(),
						tComp->count(),
						rComp->castShadow()));
					m_shader_mat_map[shader].insert(mat);
				}
			}
		}
		++iter;
	}
}

void nsrender_system::_blend_dir_light(NSLightComp * pLight)
{
	m_shaders.dir_light->set_ambient_intensity(pLight->intensity().y);
	m_shaders.dir_light->set_cast_shadows(pLight->castShadows());
	m_shaders.dir_light->set_diffuse_intensity(pLight->intensity().x);
	m_shaders.dir_light->set_light_color(pLight->color());
	m_shaders.dir_light->set_shadow_samples(pLight->shadowSamples());
	m_shaders.dir_light->set_shadow_darkness(pLight->shadowDarkness());
	m_shaders.dir_light->set_screen_size(fvec2(static_cast<float>(m_final_buf->dim().x), static_cast<float>(m_final_buf->dim().y)));
	m_shaders.dir_light->set_shadow_tex_size(fvec2(static_cast<float>(m_shadow_buf->dim(NSShadowBuffer::Direction).w), static_cast<float>(m_shadow_buf->dim(NSShadowBuffer::Direction).y)));

	nsmesh * boundingMesh = nsengine.resource<nsmesh>(pLight->meshid());
	for (uint32 i = 0; i < boundingMesh->count(); ++i)
	{
		nsmesh::submesh * cSub = boundingMesh->sub(i);
		cSub->vao.bind();
		GLsizei sz = static_cast<GLsizei>(cSub->indices.size());
		glDrawElements(cSub->primitive_type,
					   sz,
					   GL_UNSIGNED_INT,
					   nullptr);
        GLError("_blendDirectionLight: Draw Error");
		cSub->vao.unbind();
	}
}

void nsrender_system::_blend_point_light(NSLightComp * pLight)
{
	m_shaders.point_light->set_ambient_intensity(pLight->intensity().y);
	m_shaders.point_light->set_cast_shadows(pLight->castShadows());
	m_shaders.point_light->set_diffuse_intensity(pLight->intensity().x);
	m_shaders.point_light->set_light_color(pLight->color());
	m_shaders.point_light->set_shadow_samples(pLight->shadowSamples());
	m_shaders.point_light->set_shadow_darkness(pLight->shadowDarkness());
	m_shaders.point_light->set_screen_size(fvec2(static_cast<float>(m_final_buf->dim().x), static_cast<float>(m_final_buf->dim().y)));
	m_shaders.point_light->set_shadow_tex_size(fvec2(static_cast<float>(m_shadow_buf->dim(NSShadowBuffer::Direction).w), static_cast<float>(m_shadow_buf->dim(NSShadowBuffer::Direction).y)));
	m_shaders.point_light->set_const_atten(pLight->atten().x);
	m_shaders.point_light->set_lin_atten(pLight->atten().y);
	m_shaders.point_light->set_exp_atten(pLight->atten().z);


	nsmesh * boundingMesh = nsengine.resource<nsmesh>(pLight->meshid());
	for (uint32 i = 0; i < boundingMesh->count(); ++i)
	{
		nsmesh::submesh * cSub = boundingMesh->sub(i);

		if (cSub->node_ != NULL)
			m_shaders.point_light->set_uniform("nodeTransform", cSub->node_->world_transform);
		else
			m_shaders.point_light->set_uniform("nodeTransform", fmat4());

		cSub->vao.bind();
		glDrawElements(cSub->primitive_type,
					   static_cast<GLsizei>(cSub->indices.size()),
					   GL_UNSIGNED_INT,
					   0);
		cSub->vao.unbind();
	}
}

void nsrender_system::_blend_spot_light(NSLightComp * pLight)
{
	m_shaders.spot_light->set_ambient_intensity(pLight->intensity().y);
	m_shaders.spot_light->set_cast_shadows(pLight->castShadows());
	m_shaders.spot_light->set_diffuse_intensity(pLight->intensity().x);
	m_shaders.spot_light->set_light_color(pLight->color());
	m_shaders.spot_light->set_shadow_samples(pLight->shadowSamples());
	m_shaders.spot_light->set_shadow_darkness(pLight->shadowDarkness());
	m_shaders.spot_light->set_screen_size(fvec2(static_cast<float>(m_final_buf->dim().x),
												   static_cast<float>(m_final_buf->dim().y)));
	m_shaders.spot_light->set_shadow_tex_size(
		fvec2(static_cast<float>(m_shadow_buf->dim(NSShadowBuffer::Direction).w),
			  static_cast<float>(m_shadow_buf->dim(NSShadowBuffer::Direction).y)));
	m_shaders.spot_light->set_const_atten(pLight->atten().x);
	m_shaders.spot_light->set_lin_atten(pLight->atten().y);
	m_shaders.spot_light->set_exp_atten(pLight->atten().z);
	m_shaders.spot_light->set_cutoff(pLight->cutoff());

	nsmesh * boundingMesh = nsengine.resource<nsmesh>(pLight->meshid());
	for (uint32 i = 0; i < boundingMesh->count(); ++i)
	{
		nsmesh::submesh * cSub = boundingMesh->sub(i);

		if (cSub->node_ != NULL)
			m_shaders.spot_light->set_uniform("nodeTransform", cSub->node_->world_transform);
		else
			m_shaders.spot_light->set_uniform("nodeTransform", fmat4());

		cSub->vao.bind();
		glDrawElements(cSub->primitive_type,
					   static_cast<GLsizei>(cSub->indices.size()),
					   GL_UNSIGNED_INT,
					   0);
		cSub->vao.unbind();
	}
}

uivec3 nsrender_system::shadow_fbo()
{
	NSFrameBuffer * d = m_shadow_buf->fb(NSShadowBuffer::Direction);
	NSFrameBuffer * s = m_shadow_buf->fb(NSShadowBuffer::Spot);
	NSFrameBuffer * p = m_shadow_buf->fb(NSShadowBuffer::Point);
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
	NSFrameBuffer * fb = m_gbuffer->fb();
	if (fb != NULL)
		return fb->gl_id();
	return 0;
}

void nsrender_system::set_shadow_fbo(uint32 fbodir, uint32 fbospot, uint32 fbopoint)
{
	m_shadow_buf->setfb(nsengine.framebuffer(fbodir), NSShadowBuffer::Direction);
	m_shadow_buf->setfb(nsengine.framebuffer(fbospot), NSShadowBuffer::Spot);
	m_shadow_buf->setfb(nsengine.framebuffer(fbopoint), NSShadowBuffer::Point);
	m_shadow_buf->setdim(NSShadowBuffer::Direction, DEFAULT_DIRLIGHT_SHADOW_W, DEFAULT_DIRLIGHT_SHADOW_H);
	m_shadow_buf->setdim(NSShadowBuffer::Spot, DEFAULT_SPOTLIGHT_SHADOW_W, DEFAULT_SPOTLIGHT_SHADOW_H);
	m_shadow_buf->setdim(NSShadowBuffer::Point, DEFAULT_POINTLIGHT_SHADOW_W, DEFAULT_POINTLIGHT_SHADOW_H);
	m_shadow_buf->init();
}

void nsrender_system::set_gbuffer_fbo(uint32 fbo)
{
	m_gbuffer->setfb(nsengine.framebuffer(fbo));
	m_gbuffer->setfbdim(uivec2(DEFAULT_FB_RES_X, DEFAULT_FB_RES_Y));
	m_gbuffer->init();
}

void nsrender_system::set_final_fbo(uint32 fbo)
{
	m_final_buf = nsengine.framebuffer(fbo);
	if (m_final_buf == NULL)
		return;

	m_final_buf->setdim(DEFAULT_FB_RES_X, DEFAULT_FB_RES_Y);
	m_final_buf->setTarget(NSFrameBuffer::ReadAndDraw);
	m_final_buf->bind();
	m_final_buf->add(m_final_buf->create<nstex2d>("RenderedFrame", NSFrameBuffer::Color, FINAL_TEX_UNIT, GL_RGBA32F, GL_RGBA, GL_FLOAT));
	m_final_buf->add(m_gbuffer->depth());
	m_final_buf->updateDrawBuffers();
}

void nsrender_system::_draw_xfbs()
{
	nsscene * scene = nsengine.currentScene();
	if (scene == NULL)
		return;
	nsentity * cam = scene->camera();
	if (cam == NULL)
		return;
	NSTFormComp * camTComp = cam->get<NSTFormComp>();
	NSCamComp * camc = cam->get<NSCamComp>();

	xfb_draw_set::iterator drawIter = m_xfb_draws.begin();
	while (drawIter != m_xfb_draws.end())
	{
		NSRenderComp * rComp = (*drawIter)->get<NSRenderComp>();
		NSTFormComp * tComp = (*drawIter)->get<NSTFormComp>();
		nsmesh * mesh = nsengine.resource<nsmesh>(rComp->meshID());
		if (mesh == NULL)
			return;

		NSTFormComp::XFBData * xbdat = tComp->xfbData();

		auto bufIter = xbdat->mXFBBuffers.begin();
		uint32 tFormByteOffset = 0;
		uint32 indexByteOffset = 0;
		while (bufIter != xbdat->mXFBBuffers.end())
		{
			if (xbdat->mUpdate)
			{
				m_shaders.xfb_default->bind();

				bufIter->mTFFeedbackObj->bind();
				glEnable(GL_RASTERIZER_DISCARD);
				bufIter->mTFFeedbackObj->begin();
				for (uint32 subI = 0; subI < mesh->count(); ++subI)
				{
					nsmesh::submesh * subMesh = mesh->sub(subI);

					if (subMesh->node_ != NULL)
						m_shaders.xfb_default->set_uniform("nodeTransform", subMesh->node_->world_transform);
					else
						m_shaders.xfb_default->set_uniform("nodeTransform", fmat4());

					// Set up all of the uniform inputs
					subMesh->vao.bind();
					tComp->transformBuffer()->bind();
					for (uint32 tfInd = 0; tfInd < 4; ++tfInd)
					{
						subMesh->vao.add(tComp->transformBuffer(), nsshader::loc_instance_tform + tfInd);
						subMesh->vao.vertexAttribPtr(nsshader::loc_instance_tform + tfInd, 4, GL_FLOAT, GL_FALSE, sizeof(fmat4), sizeof(fvec4)*tfInd + tFormByteOffset);
						subMesh->vao.vertexAttribDiv(nsshader::loc_instance_tform + tfInd, 1);
					}

					tComp->transformIDBuffer()->bind();
					subMesh->vao.add(tComp->transformIDBuffer(), nsshader::loc_ref_id);
					subMesh->vao.vertexAttribIPtr(nsshader::loc_ref_id, 1, GL_UNSIGNED_INT, sizeof(uint32), indexByteOffset);
					subMesh->vao.vertexAttribDiv(nsshader::loc_ref_id, 1);

					// Draw the stuff without sending the stuff to be rasterized
					glDrawElementsInstanced(subMesh->primitive_type,
											static_cast<GLsizei>(subMesh->indices.size()),
											GL_UNSIGNED_INT,
											0,
											bufIter->mInstanceCount);
					
					GLError("nsrender_system::_drawTransformFeedbacks glDrawElementsInstanced");

					tComp->transformBuffer()->bind();
					subMesh->vao.remove(tComp->transformBuffer());

					tComp->transformIDBuffer()->bind();
					subMesh->vao.remove(tComp->transformIDBuffer());

					subMesh->vao.unbind();
				}

				bufIter->mTFFeedbackObj->end();
				glDisable(GL_RASTERIZER_DISCARD);
				bufIter->mTFFeedbackObj->unbind();
			}

			for (uint32 subI = 0; subI < mesh->count(); ++subI)
			{
				nsmesh::submesh * subMesh = mesh->sub(subI);

				nsmaterial * mat = nsengine.resource<nsmaterial>(rComp->materialID(subI));
				if (mat == NULL)
					mat = m_default_mat;

				nsmaterial_shader * shader = nsengine.resource<nsmaterial_shader>(mat->shader_id());
				if (shader == NULL)
					shader = m_shaders.xfb_render;

				shader->bind();
				shader->set_proj_cam_mat(camc->projCam());
				shader->set_lighting_enabled(m_lighting_enabled);

				glCullFace(mat->cull_mode());

				if (mat->culling())
					glEnable(GL_CULL_FACE);
				else
					glDisable(GL_CULL_FACE);

				if (mat->wireframe())
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				else
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

				shader->set_diffusemap_enabled(mat->contains(nsmaterial::diffuse));
				shader->set_opacitymap_enabled(mat->contains(nsmaterial::opacity));
				shader->set_normalmap_enabled(mat->contains(nsmaterial::normal));
				shader->set_heightmap_enabled(mat->contains(nsmaterial::height));

				nsmaterial::texmap_map_const_iter cIter = mat->begin();
				while (cIter != mat->end())
				{
					nstexture * t = nsengine.resource<nstexture>(cIter->second);
					if (t != NULL)
						t->enable(cIter->first);
					else
						dprint("nsrender_system::_drawGeometry Texture id in material " + mat->name() + " does not refer to any valid texture");
					++cIter;
				}

				shader->set_color_mode(mat->color_mode());
				shader->set_frag_color_out(mat->color());
				shader->set_specular_power(mat->specular_power());
				shader->set_specular_color(mat->specular_color());
				shader->set_specular_intensity(mat->specular_intensity());
				shader->set_entity_id((*drawIter)->id());
				shader->set_plugin_id((*drawIter)->plugin_id());

				if (!subMesh->has_tex_coords)
					shader->set_color_mode(true);

				bufIter->mXFBVAO->bind();
				glDrawTransformFeedback(GL_TRIANGLES, bufIter->mTFFeedbackObj->gl_id());
				GLError("nsrender_system::_drawTransformFeedbacks glDrawTransformFeedback");
				bufIter->mXFBVAO->unbind();


				nsmaterial::texmap_map_const_iter eIter = mat->begin();
				while (eIter != mat->end())
				{
					nstexture * t = nsengine.resource<nstexture>(eIter->second);
					if (t != NULL)
						t->enable(eIter->first);
					else
						dprint("nsrender_system::_drawGeometry Texture id in material " + mat->name() + " does not refer to any valid texture");
					++eIter;
				}

			}
			tFormByteOffset += bufIter->mInstanceCount * sizeof(fmat4);
			indexByteOffset += bufIter->mInstanceCount * sizeof(uint32);
			++bufIter;
		}
		xbdat->mUpdate = false;
		++drawIter;
	}
}

void nsrender_system::_draw_call(drawcall_set::iterator pDCIter)
{
	// Check to make sure each buffer is allocated before setting the shader attribute : un-allocated buffers
	// are fairly common because not every mesh has tangents for example.. or normals.. or whatever

	pDCIter->submesh->vao.bind();
	pDCIter->transform_buffer->bind();
	for (uint32 tfInd = 0; tfInd < 4; ++tfInd)
	{
		pDCIter->submesh->vao.add(pDCIter->transform_buffer, nsshader::loc_instance_tform + tfInd);
		pDCIter->submesh->vao.vertexAttribPtr(nsshader::loc_instance_tform + tfInd, 4, GL_FLOAT, GL_FALSE, sizeof(fmat4), sizeof(fvec4)*tfInd);
		pDCIter->submesh->vao.vertexAttribDiv(nsshader::loc_instance_tform + tfInd, 1);
	}

	pDCIter->transform_id_buffer->bind();
	pDCIter->submesh->vao.add(pDCIter->transform_id_buffer, nsshader::loc_ref_id);
	pDCIter->submesh->vao.vertexAttribIPtr(nsshader::loc_ref_id, 1, GL_UNSIGNED_INT, sizeof(uint32), 0);
	pDCIter->submesh->vao.vertexAttribDiv(nsshader::loc_ref_id, 1);
	GLError("nsrender_system::_drawCall 1");
	
	glDrawElementsInstanced(pDCIter->submesh->primitive_type,
							static_cast<GLsizei>(pDCIter->submesh->indices.size()),
							GL_UNSIGNED_INT,
							0,
							pDCIter->transform_count);
	
	GLError("nsrender_system::_drawCall 2");

	pDCIter->transform_buffer->bind();
	pDCIter->submesh->vao.remove(pDCIter->transform_buffer);
	pDCIter->transform_id_buffer->bind();
	pDCIter->submesh->vao.remove(pDCIter->transform_id_buffer);
	pDCIter->submesh->vao.unbind();
}

void nsrender_system::_draw_geometry()
{
	nsscene * scene = nsengine.currentScene();
	if (scene == NULL)
		return;
	nsentity * cam = scene->camera();
	if (cam == NULL)
		return;
	NSTFormComp * camTComp = cam->get<NSTFormComp>();
	NSCamComp * camc = cam->get<NSCamComp>();
	
	// Go through each shader in the shader material map.. because each submesh corresponds to exactly one
	// material we can use this material as the key in to the draw call map minimizing shader switches
	shader_mat_map::iterator shaderIter = m_shader_mat_map.begin();
	while (shaderIter != m_shader_mat_map.end())
	{
		nsmaterial_shader * currentShader = shaderIter->first;
		currentShader->bind();
		currentShader->set_proj_cam_mat(camc->projCam());
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

			if ((*matIter)->wireframe())
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			else
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			currentShader->set_diffusemap_enabled((*matIter)->contains(nsmaterial::diffuse));
			currentShader->set_opacitymap_enabled((*matIter)->contains(nsmaterial::opacity));
			currentShader->set_normalmap_enabled((*matIter)->contains(nsmaterial::normal));
			currentShader->set_heightmap_enabled((*matIter)->contains(nsmaterial::height));

			nsmaterial::texmap_map_const_iter cIter = (*matIter)->begin();
			while (cIter != (*matIter)->end())
			{
				nstexture * t = nsengine.resource<nstexture>(cIter->second);
				if (t != NULL)
					t->enable(cIter->first);
				else
					dprint("nsrender_system::_drawGeometry Texture id in material " + (*matIter)->name() + " does not refer to any valid texture");
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

				if (!dcIter->submesh->has_tex_coords)
					currentShader->set_color_mode(true);

				if (dcIter->submesh->node_ != NULL)
					currentShader->set_node_transform(dcIter->submesh->node_->world_transform);
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
			GLError("nsrender_system::df");
			nsmaterial::texmap_map_const_iter eIter = (*matIter)->begin();
			while (eIter != (*matIter)->end())
			{
				nstexture * t = nsengine.resource<nstexture>(eIter->second);
				if (t != NULL)
					t->disable(eIter->first);
				else
					dprint("nsrender_system::_drawGeometry Texture id in material " + (*matIter)->name() + " does not refer to any valid texture");
				++eIter;
			}
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
		nstexture * tex = nsengine.resource<nstexture>(matIter->first->map_tex_id(nsmaterial::height));
		if (tex != NULL)
		{
			pShader->set_height_map_enabled(true);
			tex->enable(nsmaterial::height);
		}

		drawcall_set & currentSet = matIter->second;
		drawcall_set::iterator  dcIter = currentSet.begin();
		while (dcIter != currentSet.end())
		{
			if (dcIter->submesh->primitive_type != GL_TRIANGLES)
			{
				++dcIter;
				continue;
			}

			if (!dcIter->casts_shadows)
			{
				++dcIter;
				continue;
			}

			if (dcIter->submesh->node_ != NULL)
				pShader->set_node_transform(dcIter->submesh->node_->world_transform);
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


void nsrender_system::_draw_scene_to_depth_xfb(nsshader * pShader)
{
	xfb_draw_set::iterator drawIter = m_xfb_draws.begin();
	while (drawIter != m_xfb_draws.end())
	{
		NSRenderComp * rComp = (*drawIter)->get<NSRenderComp>();
		NSTFormComp * tComp = (*drawIter)->get<NSTFormComp>();
		nsmesh * mesh = nsengine.resource<nsmesh>(rComp->meshID());
		if (mesh == NULL)
			return;

		NSTFormComp::XFBData * xbdat = tComp->xfbData();

		auto bufIter = xbdat->mXFBBuffers.begin();
		uint32 tFormByteOffset = 0;
		uint32 indexByteOffset = 0;
		if (!xbdat->mUpdate)
		{
			while (bufIter != xbdat->mXFBBuffers.end())
			{
				for (uint32 subI = 0; subI < mesh->count(); ++subI)
				{
					bufIter->mXFBVAO->bind();
					glDrawTransformFeedback(GL_TRIANGLES, bufIter->mTFFeedbackObj->gl_id());
					GLError("nsrender_system::_drawSceneToDepthXFB");
					bufIter->mXFBVAO->unbind();
				}
				++bufIter;
			}
		}
		++drawIter;
	}
}

void nsrender_system::_stencil_point_light(NSLightComp * pLight)
{
	nsmesh * boundingMesh = nsengine.resource<nsmesh>(pLight->meshid());
	for (uint32 i = 0; i < boundingMesh->count(); ++i)
	{
		nsmesh::submesh * cSub = boundingMesh->sub(i);

		if (cSub->node_ != NULL)
			m_shaders.light_stencil->set_node_transform(cSub->node_->world_transform);
		else
			m_shaders.light_stencil->set_node_transform(fmat4());

		cSub->vao.bind();
		glDrawElements(cSub->primitive_type,
					   static_cast<GLsizei>(cSub->indices.size()),
					   GL_UNSIGNED_INT,
					   0);
		cSub->vao.unbind();
	}
}

void nsrender_system::_stencil_spot_light(NSLightComp * pLight)
{
	nsmesh * boundingMesh = nsengine.resource<nsmesh>(pLight->meshid());
	for (uint32 i = 0; i < boundingMesh->count(); ++i)
	{
		nsmesh::submesh * cSub = boundingMesh->sub(i);

		if (cSub->node_ != NULL)
			m_shaders.light_stencil->set_node_transform(cSub->node_->world_transform);
		else
			m_shaders.light_stencil->set_node_transform(fmat4());

		cSub->vao.bind();
		glDrawElements(cSub->primitive_type,
					   static_cast<GLsizei>(cSub->indices.size()),
					   GL_UNSIGNED_INT,
					   0);
		cSub->vao.unbind();
	}
}

nsrender_system::draw_call::draw_call(nsmesh::submesh * pSubMesh, 
	fmat4array * pAnimTransforms,
	NSBufferObject * pTransformBuffer,
	NSBufferObject * pTransformIDBuffer,
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
