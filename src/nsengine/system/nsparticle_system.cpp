/*!
\file nsparticle_system.h

\brief Definition file for nsparticle_system class

This file contains all of the neccessary definitions for the nsparticle_system class.

\author Daniel Randle
\date March 8 2014
\copywrite Earth Banana Games 2013
*/

#include <nsgl_driver.h>
#include <nsgl_vao.h>
#include <nsvideo_driver.h>
#include <nsevent_dispatcher.h>
#include <nsparticle_system.h>
#include <nsparticle_comp.h>
#include <nsscene.h>
#include <nsengine.h>
#include <nsgl_framebuffer.h>
#include <nsrender_system.h>
#include <nsshader.h>
#include <nsshader_manager.h>
#include <nsentity.h>
#include <nstimer.h>
#include <nsmat_manager.h>
#include <nstexture.h>
#include <nsplugin.h>
#include <nstex_manager.h>
#include <nsgl_xfb.h>
#include <nscam_comp.h>
#include <nsplugin_manager.h>

nsparticle_system::nsparticle_system() :
nssystem()
{}

nsparticle_system::~nsparticle_system()
{}

void nsparticle_system::init()
{
}

int32 nsparticle_system::update_priority()
{
	return PARTICLE_SYS_UPDATE_PR;
}

void nsparticle_system::update()
{
    return;
	if (scene_error_check())
		return;
	
	nsrender::viewport * vp = nse.system<nsrender_system>()->current_viewport();
	if (vp == nullptr)
		return;
	
	nsentity * cam = vp->camera;
	if (cam == NULL)
		return;

	nscam_comp * compc = cam->get<nscam_comp>();
	nstform_comp * camTComp = cam->get<nstform_comp>();

	static uint32 count = 1;
	auto ents = m_active_scene->entities_with_comp<nsparticle_comp>();
	if (ents == nullptr)
		return;
	
    glEnable(GL_RASTERIZER_DISCARD);
	auto entIter = ents->begin();
	while (entIter != ents->end())
	{
		nsparticle_comp * comp = (*entIter)->get<nsparticle_comp>();

		if (comp->simulating())
			comp->elapsed() += nse.timer()->fixed();

		if (comp->elapsed() * 1000 >= comp->lifetime())
		{
			comp->reset();
			comp->enable_simulation(comp->looping());
		}

		nsshader * pshdr = get_resource<nsshader>(comp->shader_id());
		if (pshdr == NULL)
		{
			dprint("nsparticle_system::update No processing shader set in component - skipping");
			++entIter;
			continue;
		}
		
		if (pshdr->error() != nsshader::error_none)
		{
			dprint("nsparticle_system::update XFBShader shader has an error set to " + std::to_string(pshdr->error()) + " in entity \"" + (*entIter)->name() + "\"");
			++entIter;
			continue;
		}

		pshdr->bind();
		nstexture * texRand = get_resource<nstexture>(comp->rand_tex_id());
		if (texRand != NULL)
		{
			nse.video_driver<nsgl_driver>()->set_active_texture_unit(RAND_TEX_UNIT);
			texRand->bind();
		}
		
		pshdr->set_uniform("randomTex", RAND_TEX_UNIT);
		pshdr->set_uniform("dt", float(nse.timer()->fixed()));
		pshdr->set_uniform("timeElapsed", comp->elapsed());
		pshdr->set_uniform("lifetime", comp->lifetime());
		pshdr->set_uniform("launchFrequency", float(comp->emission_rate()));
		pshdr->set_uniform("angVelocity", comp->angular_vel());
		pshdr->set_uniform("motionGlobal", comp->motion_global_time());
		pshdr->set_uniform("visualGlobal", comp->visual_global_time());
		pshdr->set_uniform("interpolateMotion", comp->motion_key_interpolation());
		pshdr->set_uniform("interpolateVisual", comp->visual_key_interpolation());
		pshdr->set_uniform("startingSize", comp->starting_size());
		pshdr->set_uniform("emitterSize", comp->emitter_size());
		pshdr->set_uniform("emitterShape", comp->emitter_shape());
		pshdr->set_uniform("initVelocityMult", comp->init_vel_mult());
		pshdr->set_uniform("motionKeyType", uint32(comp->motion_key_type()));

		uint32 index = 0;
		ui_fvec3_map::const_iterator keyIter = comp->m_motion_keys.begin();
		while (keyIter != comp->m_motion_keys.end())
		{
			pshdr->set_uniform(
				"forceKeys[" + std::to_string(index) + "].time",
				float(keyIter->first) / float(comp->m_max_motion_keys * 1000) * float(comp->m_lifetime));
			pshdr->set_uniform(
				"forceKeys[" + std::to_string(index) + "].force",
				keyIter->second);
			++index;
			++keyIter;
		}

		index = 0;
		keyIter = comp->m_visual_keys.begin();
		while (keyIter != comp->m_visual_keys.end())
		{
			pshdr->set_uniform(
				"sizeKeys[" + std::to_string(index) + "].time",
				float(keyIter->first) / float(comp->m_max_visual_keys * 1000) * float(comp->m_lifetime));
			pshdr->set_uniform(
				"sizeKeys[" + std::to_string(index) + "].sizeVel",
				fvec2(keyIter->second.x, keyIter->second.y));
			pshdr->set_uniform(
				"sizeKeys[" + std::to_string(index) + "].alpha",
				keyIter->second.z);
			++index;
			++keyIter;
		}
		
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
