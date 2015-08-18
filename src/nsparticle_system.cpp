/*!
\file nsparticle_system.h

\brief Definition file for NSParticleSystem class

This file contains all of the neccessary definitions for the NSParticleSystem class.

\author Daniel Randle
\date March 8 2014
\copywrite Earth Banana Games 2013
*/

#include <nsevent_dispatcher.h>
#include <nsparticle_system.h>
#include <nsparticle_comp.h>
#include <nsscene.h>
#include <nsengine.h>
#include <nsframebuffer.h>
#include <nsrender_system.h>
#include <nsshader.h>
#include <nsshader_manager.h>
#include <nsentity.h>
#include <nstimer.h>
#include <nsmat_manager.h>
#include <nstexture.h>
#include <nsplugin.h>
#include <nstex_manager.h>

NSParticleSystem::NSParticleSystem() :
NSSystem()
{}

NSParticleSystem::~NSParticleSystem()
{}

// bool NSParticleSystem::handleEvent(NSEvent * pEvent)
// {
// 	return false;
// }

uint32 NSParticleSystem::finalfbo() { return mFinalBuf; }

void NSParticleSystem::setFinalfbo(uint32 fbo) { mFinalBuf = fbo; }

void NSParticleSystem::draw()
{
	nsscene * scene = nsengine.currentScene();
	if (scene == NULL)
		return;
	nsentity * cam = scene->camera();
	if (cam == NULL)
		return;
	NSCamComp * compc = cam->get<NSCamComp>();
	NSTFormComp * camTComp = cam->get<NSTFormComp>();

	NSFrameBuffer * finalBuf = nsengine.framebuffer(mFinalBuf);
	finalBuf->setTarget(NSFrameBuffer::Draw);
	finalBuf->bind();

	auto comps = scene->entities<NSParticleComp>();
	auto entIter = comps.begin();
	while (entIter != comps.end())
	{
		NSParticleComp * comp = (*entIter)->get<NSParticleComp>();

		if (comp->first())
		{
			++entIter;
			continue;
		}
		nsmaterial * mat = nsengine.resource<nsmaterial>(comp->materialID());
		if (mat == NULL)
			mat = nsengine.system<nsrender_system>()->default_mat();

		nsparticle_render_shader * renderShader = nsengine.resource<nsparticle_render_shader>(mat->shader_id());

		if (renderShader == NULL)
			renderShader = nsengine.engplug()->get<nsparticle_render_shader>(DEFAULT_RENDER_PARTICLE_SHADER);

		if (renderShader == NULL || comp->first())
		{
			dprint("NSParticleSystem::update Render shader is not set for entity or ent has not been updated yet (mFirst == false) \"" + (*entIter)->name() + "\"");
			++entIter;
			continue;
		}

		if (renderShader->error() != nsshader::error_none)
		{
			dprint("NSParticleSystem::update Render shader has an error set to " + std::to_string(renderShader->error()) + " in entity \"" + (*entIter)->name() + "\"");
			++entIter;
			continue;
		}

		if (!comp->simulating())
		{
			++entIter;
			continue;
		}

		NSTFormComp * tComp = (*entIter)->get<NSTFormComp>();
		NSTFormComp * camTComp = scene->camera()->get<NSTFormComp>();
		nstexture * tex = nsengine.resource<nstexture>(mat->map_tex_id(nsmaterial::diffuse));
		if (tex != NULL)
			tex->enable(nsmaterial::diffuse);
		else
			dprint("NSParticleSystem::draw() - No random texture set - particles will be lame.");

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);

		if (comp->blendMode() == NSParticleComp::Mix)
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		else
			glBlendFunc(GL_ADD, GL_ADD);

		glDepthMask(GL_FALSE);
		glDisable(GL_CULL_FACE);

		renderShader->bind();
		renderShader->set_proj_cam_mat(compc->projCam());
		renderShader->set_cam_right(camTComp->dirVec(NSTFormComp::Right));
		renderShader->set_cam_up(camTComp->dirVec(NSTFormComp::Up));
		renderShader->set_cam_target(camTComp->dirVec(NSTFormComp::Target));
		renderShader->set_world_up(fvec3(0.0f, 0.0f, 1.0f));
		renderShader->set_diffusemap_enabled(mat->contains(nsmaterial::diffuse));
		renderShader->set_color_mode(mat->color_mode());
		renderShader->set_frag_color_out(mat->color());
		renderShader->set_lifetime(float(comp->lifetime()) / 1000.0f);
		renderShader->set_blend_mode(uint32(comp->blendMode()));

		
		comp->vertexArrayObject()->bind();


		tComp->transformBuffer()->bind();
		for (uint32 tfInd = 0; tfInd < 4; ++tfInd)
		{
			comp->vertexArrayObject()->add(tComp->transformBuffer(), 4 + tfInd);
			comp->vertexArrayObject()->vertexAttribPtr(4 + tfInd, 4, GL_FLOAT, GL_FALSE, sizeof(fmat4), sizeof(fvec4)*tfInd);
			comp->vertexArrayObject()->vertexAttribDiv(4 + tfInd, 1);
		}

		glDrawTransformFeedbackInstanced(GL_POINTS, comp->transformFeedbackID(),tComp->count());
		GLError("NSParticleSystem::draw in glDrawElementsInstanced");


		tComp->transformBuffer()->bind();
		comp->vertexArrayObject()->remove(tComp->transformBuffer());
		comp->vertexArrayObject()->unbind();

		renderShader->unbind();
		++entIter;
	}

	finalBuf->unbind();
}

void NSParticleSystem::init()
{
}

int32 NSParticleSystem::draw_priority()
{
	return PARTICLE_SYS_DRAW_PR;
}

int32 NSParticleSystem::update_priority()
{
	return PARTICLE_SYS_UPDATE_PR;
}

void NSParticleSystem::update()
{
	nsscene * scene = nsengine.currentScene();
	if (scene == NULL)
		return;
	nsentity * cam = scene->camera();
	if (cam == NULL)
		return;
	NSCamComp * compc = cam->get<NSCamComp>();
	NSTFormComp * camTComp = cam->get<NSTFormComp>();

	//nsengine.events()->process(this); // process any events first

	if (scene == NULL) // if scene is null return
		return;

	glEnable(GL_RASTERIZER_DISCARD);
	static uint32 count = 1;
	auto comps = scene->entities<NSParticleComp>();
	auto entIter = comps.begin();
	while (entIter != comps.end())
	{
		NSParticleComp * comp = (*entIter)->get<NSParticleComp>();

		if (comp->simulating())
			comp->elapsed() += nsengine.timer()->fixed();

		if (comp->elapsed() * 1000 >= comp->lifetime())
		{
			comp->reset();
			comp->enableSimulation(comp->looping());
		}

		nsparticle_process_shader * particleShader = nsengine.resource<nsparticle_process_shader>(comp->shaderID());
		if (particleShader == NULL)
			particleShader = mDefaultShader;

		if (particleShader->error() != nsshader::error_none)
		{
			dprint("NSParticleSystem::update XFBShader shader has an error set to " + std::to_string(particleShader->error()) + " in entity \"" + (*entIter)->name() + "\"");
			++entIter;
			continue;
		}

		particleShader->bind();
		nstexture * texRand = nsengine.resource<nstexture>(comp->randomTextureID());
		if (texRand != NULL)
			texRand->enable(RAND_TEX_UNIT);

		particleShader->set_elapsed(comp->elapsed());
		particleShader->set_dt(nsengine.timer()->fixed());
		particleShader->set_angular_vel(comp->angularVelocity());
		particleShader->set_lifetime(comp->lifetime());
		particleShader->set_launch_freq(float(comp->emissionRate())); // for now
		particleShader->set_motion_key_global(comp->motionGlobalTime());
		particleShader->set_interpolated_motion_keys(comp->motionKeyInterpolation());
		particleShader->set_uniform("motionKeyType", uint32(comp->motionKeyType()));
		particleShader->set_starting_size(comp->startingSize());
		particleShader->set_emitter_size(comp->emitterSize());
		particleShader->set_emitter_shape(comp->emitterShape());
		particleShader->set_interpolated_visual_keys(comp->visualKeyInterpolation());
		particleShader->set_visual_key_global(comp->visualGlobalTime());
		particleShader->set_initial_vel_mult(comp->initVelocityMult());
		particleShader->set_motion_keys(comp->mMotionKeys,comp->mMaxMotionKeys, comp->mLifetime);
		particleShader->set_visual_keys(comp->mVisualKeys, comp->mMaxVisualKeys, comp->mLifetime);
		
		comp->transformFeedbackObject()->bind();
		comp->vertexArrayObject()->bind();
		comp->transformFeedbackObject()->begin();
		if (comp->first())
		{
			glDrawArrays(GL_POINTS, 0, 1);
			GLError("NSParticleSystem::update in glDrawArrays");
			comp->setFirst(false);
		}
		else
		{
			glDrawTransformFeedback(GL_POINTS, comp->transformFeedbackID());
			GLError("NSParticleSystem::update in glDrawTransformFeedback");
		}
		comp->transformFeedbackObject()->end();
		comp->vertexArrayObject()->unbind();
		comp->transformFeedbackObject()->unbind();
		comp->swap();

		std::vector<float> f;
		f.resize(10 * count);

		++entIter;
		++count;
	}

	glDisable(GL_RASTERIZER_DISCARD);	
}
