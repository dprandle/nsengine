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

void nsparticle_system::init()
{
	nsplugin * cplg = nse.core();
	nsstring shext = nsstring(DEFAULT_SHADER_EXTENSION);
	
	m_process_shader = cplg->load<nsparticle_process_shader>(
		nsstring(DEFAULT_PROCESS_PARTICLE_SHADER) + shext);
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

		nsparticle_process_shader * pshdr = nse.resource<nsparticle_process_shader>(comp->shader_id());
		if (pshdr == NULL)
			pshdr = m_process_shader;

		if (pshdr->error() != nsshader::error_none)
		{
			dprint("nsparticle_system::update XFBShader shader has an error set to " + std::to_string(pshdr->error()) + " in entity \"" + (*entIter)->name() + "\"");
			++entIter;
			continue;
		}

		pshdr->bind();
		nstexture * texRand = nse.resource<nstexture>(comp->rand_tex_id());
		if (texRand != NULL)
		{
			nse.system<nsrender_system>()->set_active_texture_unit(RAND_TEX_UNIT);
			texRand->bind();
		}

		pshdr->set_elapsed(comp->elapsed());
		pshdr->set_dt(nse.timer()->fixed());
		pshdr->set_angular_vel(comp->angular_vel());
		pshdr->set_lifetime(comp->lifetime());
		pshdr->set_launch_freq(float(comp->emission_rate())); // for now
		pshdr->set_motion_key_global(comp->motion_global_time());
		pshdr->set_interpolated_motion_keys(comp->motion_key_interpolation());
		pshdr->set_uniform("motionKeyType", uint32(comp->motion_key_type()));
		pshdr->set_starting_size(comp->starting_size());
		pshdr->set_emitter_size(comp->emitter_size());
		pshdr->set_emitter_shape(comp->emitter_shape());
		pshdr->set_interpolated_visual_keys(comp->visual_key_interpolation());
		pshdr->set_visual_key_global(comp->visual_global_time());
		pshdr->set_initial_vel_mult(comp->init_vel_mult());
		pshdr->set_motion_keys(comp->m_motion_keys,comp->m_max_motion_keys, comp->m_lifetime);
		pshdr->set_visual_keys(comp->m_visual_keys, comp->m_max_visual_keys, comp->m_lifetime);
		
		comp->xfb_obj()->bind();
		comp->va_obj()->bind();
		comp->xfb_obj()->begin();
		if (comp->first())
		{
			glDrawArrays(GL_POINTS, 0, 1);
			gl_err_check("nsparticle_system::update in glDrawArrays");
			comp->set_first(false);
		}
		else
		{
			glDrawTransformFeedback(GL_POINTS, comp->xfb_id());
			gl_err_check("nsparticle_system::update in glDrawTransformFeedback");
		}
		comp->xfb_obj()->end();
		comp->va_obj()->unbind();
		comp->xfb_obj()->unbind();
		comp->swap();
		++entIter;
		++count;
	}

	glDisable(GL_RASTERIZER_DISCARD);	
}
