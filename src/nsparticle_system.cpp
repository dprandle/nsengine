/*!
\file nsparticle_system.h

\brief Definition file for nsparticle_system class

This file contains all of the neccessary definitions for the nsparticle_system class.

\author Daniel Randle
\date March 8 2014
\copywrite Earth Banana Games 2013
*/

#include <nsevent_dispatcher.h>
#include <nsparticle_system.h>
#include <nsparticle_comp.h>
#include <nsscene.h>
#include <nsengine.h>
#include <nsfb_object.h>
#include <nsrender_system.h>
#include <nsshader.h>
#include <nsshader_manager.h>
#include <nsentity.h>
#include <nstimer.h>
#include <nsmat_manager.h>
#include <nstexture.h>
#include <nsplugin.h>
#include <nstex_manager.h>
#include <nsxfb_object.h>
#include <nscam_comp.h>

nsparticle_system::nsparticle_system() :
nssystem()
{}

nsparticle_system::~nsparticle_system()
{}

uint32 nsparticle_system::final_fbo() { return m_final_buf; }

void nsparticle_system::set_final_fbo(uint32 fbo) { m_final_buf = fbo; }

void nsparticle_system::draw()
{
	nsscene * scene = nse.current_scene();
	if (scene == NULL)
		return;
	nsentity * cam = scene->camera();
	if (cam == NULL)
		return;
	nscam_comp * compc = cam->get<nscam_comp>();
	nstform_comp * camTComp = cam->get<nstform_comp>();

	nsfb_object * finalBuf = nse.framebuffer(m_final_buf);
	finalBuf->set_target(nsfb_object::fb_draw);
	finalBuf->bind();

	auto comps = scene->entities<nsparticle_comp>();
	auto entIter = comps.begin();
	while (entIter != comps.end())
	{
		nsparticle_comp * comp = (*entIter)->get<nsparticle_comp>();

		if (comp->first())
		{
			++entIter;
			continue;
		}
		nsmaterial * mat = nse.resource<nsmaterial>(comp->material_id());
		if (mat == NULL)
			mat = nse.system<nsrender_system>()->default_mat();

		nsparticle_render_shader * renderShader = nse.resource<nsparticle_render_shader>(mat->shader_id());

		if (renderShader == NULL)
			renderShader = nse.core()->get<nsparticle_render_shader>(DEFAULT_RENDER_PARTICLE_SHADER);

		if (renderShader == NULL || comp->first())
		{
			dprint("nsparticle_system::update Render shader is not set for entity or ent has not been updated yet (mFirst == false) \"" + (*entIter)->name() + "\"");
			++entIter;
			continue;
		}

		if (renderShader->error() != nsshader::error_none)
		{
			dprint("nsparticle_system::update Render shader has an error set to " + std::to_string(renderShader->error()) + " in entity \"" + (*entIter)->name() + "\"");
			++entIter;
			continue;
		}

		if (!comp->simulating())
		{
			++entIter;
			continue;
		}

		nstform_comp * tComp = (*entIter)->get<nstform_comp>();
		nstform_comp * camTComp = scene->camera()->get<nstform_comp>();
		nstexture * tex = nse.resource<nstexture>(mat->map_tex_id(nsmaterial::diffuse));
		if (tex != NULL)
			tex->enable(nsmaterial::diffuse);
		else
			dprint("nsparticle_system::draw() - No random texture set - particles will be lame.");

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);

		if (comp->blend_mode() == nsparticle_comp::b_mix)
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		else
			glBlendFunc(GL_ADD, GL_ADD);

		glDepthMask(GL_FALSE);
		glDisable(GL_CULL_FACE);

		renderShader->bind();
		renderShader->set_proj_cam_mat(compc->proj_cam());
		renderShader->set_cam_right(camTComp->dvec(nstform_comp::dir_right));
		renderShader->set_cam_up(camTComp->dvec(nstform_comp::dir_up));
		renderShader->set_cam_target(camTComp->dvec(nstform_comp::dir_target));
		renderShader->set_world_up(fvec3(0.0f, 0.0f, 1.0f));
		renderShader->set_diffusemap_enabled(mat->contains(nsmaterial::diffuse));
		renderShader->set_color_mode(mat->color_mode());
		renderShader->set_frag_color_out(mat->color());
		renderShader->set_lifetime(float(comp->lifetime()) / 1000.0f);
		renderShader->set_blend_mode(uint32(comp->blend_mode()));

		
		comp->va_obj()->bind();


		tComp->transform_buffer()->bind();
		for (uint32 tfInd = 0; tfInd < 4; ++tfInd)
		{
			comp->va_obj()->add(tComp->transform_buffer(), 4 + tfInd);
			comp->va_obj()->vertex_attrib_ptr(4 + tfInd, 4, GL_FLOAT, GL_FALSE, sizeof(fmat4), sizeof(fvec4)*tfInd);
			comp->va_obj()->vertex_attrib_div(4 + tfInd, 1);
		}

		glDrawTransformFeedbackInstanced(GL_POINTS, comp->xfb_id(),tComp->count());
		GLError("nsparticle_system::draw in glDrawElementsInstanced");


		tComp->transform_buffer()->bind();
		comp->va_obj()->remove(tComp->transform_buffer());
		comp->va_obj()->unbind();

		renderShader->unbind();
		++entIter;
	}

	finalBuf->unbind();
}

void nsparticle_system::init()
{
	set_final_fbo(nse.composite_framebuffer());

	nsplugin * cplg = nse.core();
	nsstring shext = nsstring(DEFAULT_SHADER_EXTENSION);
	
	m_process_shader = cplg->load<nsparticle_process_shader>(nsstring(DEFAULT_PROCESS_PARTICLE_SHADER) + shext);
	nsparticle_render_shader * renderparticle = cplg->load<nsparticle_render_shader>(nsstring(DEFAULT_RENDER_PARTICLE_SHADER) + shext);
}

int32 nsparticle_system::draw_priority()
{
	return PARTICLE_SYS_DRAW_PR;
}

int32 nsparticle_system::update_priority()
{
	return PARTICLE_SYS_UPDATE_PR;
}

void nsparticle_system::update()
{
	nsscene * scene = nse.current_scene();
	if (scene == NULL)
		return;
	nsentity * cam = scene->camera();
	if (cam == NULL)
		return;
	nscam_comp * compc = cam->get<nscam_comp>();
	nstform_comp * camTComp = cam->get<nstform_comp>();

	//nse.events()->process(this); // process any events first

	if (scene == NULL) // if scene is null return
		return;

	glEnable(GL_RASTERIZER_DISCARD);
	static uint32 count = 1;
	auto comps = scene->entities<nsparticle_comp>();
	auto entIter = comps.begin();
	while (entIter != comps.end())
	{
		nsparticle_comp * comp = (*entIter)->get<nsparticle_comp>();

		if (comp->simulating())
			comp->elapsed() += nse.timer()->fixed();

		if (comp->elapsed() * 1000 >= comp->lifetime())
		{
			comp->reset();
			comp->enable_simulation(comp->looping());
		}

		nsparticle_process_shader * particleShader = nse.resource<nsparticle_process_shader>(comp->shader_id());
		if (particleShader == NULL)
			particleShader = m_process_shader;

		if (particleShader->error() != nsshader::error_none)
		{
			dprint("nsparticle_system::update XFBShader shader has an error set to " + std::to_string(particleShader->error()) + " in entity \"" + (*entIter)->name() + "\"");
			++entIter;
			continue;
		}

		particleShader->bind();
		nstexture * texRand = nse.resource<nstexture>(comp->rand_tex_id());
		if (texRand != NULL)
			texRand->enable(RAND_TEX_UNIT);

		particleShader->set_elapsed(comp->elapsed());
		particleShader->set_dt(nse.timer()->fixed());
		particleShader->set_angular_vel(comp->angular_vel());
		particleShader->set_lifetime(comp->lifetime());
		particleShader->set_launch_freq(float(comp->emission_rate())); // for now
		particleShader->set_motion_key_global(comp->motion_global_time());
		particleShader->set_interpolated_motion_keys(comp->motion_key_interpolation());
		particleShader->set_uniform("motionKeyType", uint32(comp->motion_key_type()));
		particleShader->set_starting_size(comp->starting_size());
		particleShader->set_emitter_size(comp->emitter_size());
		particleShader->set_emitter_shape(comp->emitter_shape());
		particleShader->set_interpolated_visual_keys(comp->visual_key_interpolation());
		particleShader->set_visual_key_global(comp->visual_global_time());
		particleShader->set_initial_vel_mult(comp->init_vel_mult());
		particleShader->set_motion_keys(comp->m_motion_keys,comp->m_max_motion_keys, comp->m_lifetime);
		particleShader->set_visual_keys(comp->m_visual_keys, comp->m_max_visual_keys, comp->m_lifetime);
		
		comp->xfb_obj()->bind();
		comp->va_obj()->bind();
		comp->xfb_obj()->begin();
		if (comp->first())
		{
			glDrawArrays(GL_POINTS, 0, 1);
			GLError("nsparticle_system::update in glDrawArrays");
			comp->set_first(false);
		}
		else
		{
			glDrawTransformFeedback(GL_POINTS, comp->xfb_id());
			GLError("nsparticle_system::update in glDrawTransformFeedback");
		}
		comp->xfb_obj()->end();
		comp->va_obj()->unbind();
		comp->xfb_obj()->unbind();
		comp->swap();

		std::vector<float> f;
		f.resize(10 * count);

		++entIter;
		++count;
	}

	glDisable(GL_RASTERIZER_DISCARD);	
}
