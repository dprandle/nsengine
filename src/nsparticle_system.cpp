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
	NSScene * scene = nsengine.currentScene();
	if (scene == NULL)
		return;
	NSEntity * cam = scene->camera();
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
		NSMaterial * mat = nsengine.resource<NSMaterial>(comp->materialID());
		if (mat == NULL)
			mat = nsengine.system<NSRenderSystem>()->defaultMat();

		NSParticleRenderShader * renderShader = nsengine.resource<NSParticleRenderShader>(mat->shaderID());

		if (renderShader == NULL)
			renderShader = nsengine.engplug()->get<NSParticleRenderShader>(DEFAULT_RENDER_PARTICLE_SHADER);

		if (renderShader == NULL || comp->first())
		{
			dprint("NSParticleSystem::update Render shader is not set for entity or ent has not been updated yet (mFirst == false) \"" + (*entIter)->name() + "\"");
			++entIter;
			continue;
		}

		if (renderShader->error() != NSShader::None)
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
		NSTexture * tex = nsengine.resource<NSTexture>(mat->mapTextureID(NSMaterial::Diffuse));
		if (tex != NULL)
			tex->enable(NSMaterial::Diffuse);
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
		renderShader->setProjCamMat(compc->projCam());
		renderShader->setCamRight(camTComp->dirVec(NSTFormComp::Right));
		renderShader->setCamUp(camTComp->dirVec(NSTFormComp::Up));
		renderShader->setCamTarget(camTComp->dirVec(NSTFormComp::Target));
		renderShader->setWorldUp(fvec3(0.0f, 0.0f, 1.0f));
		renderShader->setDiffuseMapEnabled(mat->contains(NSMaterial::Diffuse));
		renderShader->setColorMode(mat->colorMode());
		renderShader->setFragOutColor(mat->color());
		renderShader->setLifetime(float(comp->lifetime()) / 1000.0f);
		renderShader->setBlendMode(uint32(comp->blendMode()));

		
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

int32 NSParticleSystem::drawPriority()
{
	return PARTICLE_SYS_DRAW_PR;
}

int32 NSParticleSystem::updatePriority()
{
	return PARTICLE_SYS_UPDATE_PR;
}

void NSParticleSystem::update()
{
	NSScene * scene = nsengine.currentScene();
	if (scene == NULL)
		return;
	NSEntity * cam = scene->camera();
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

		NSParticleProcessShader * particleShader = nsengine.resource<NSParticleProcessShader>(comp->shaderID());
		if (particleShader == NULL)
			particleShader = mDefaultShader;

		if (particleShader->error() != NSShader::None)
		{
			dprint("NSParticleSystem::update XFBShader shader has an error set to " + std::to_string(particleShader->error()) + " in entity \"" + (*entIter)->name() + "\"");
			++entIter;
			continue;
		}

		particleShader->bind();
		NSTexture * texRand = nsengine.resource<NSTexture>(comp->randomTextureID());
		if (texRand != NULL)
			texRand->enable(RAND_TEX_UNIT);

		particleShader->setTimeElapsed(comp->elapsed());
		particleShader->setdt(nsengine.timer()->fixed());
		particleShader->setAngularVelocity(comp->angularVelocity());
		particleShader->setLifetime(comp->lifetime());
		particleShader->setLaunchFreq(float(comp->emissionRate())); // for now
		particleShader->setMotionKeyGlobal(comp->motionGlobalTime());
		particleShader->setInterpolateMotionKeys(comp->motionKeyInterpolation());
		particleShader->setUniform("motionKeyType", uint32(comp->motionKeyType()));
		particleShader->setStartingSize(comp->startingSize());
		particleShader->setEmitterSize(comp->emitterSize());
		particleShader->setEmitterShape(comp->emitterShape());
		particleShader->setInterpolateVisualKeys(comp->visualKeyInterpolation());
		particleShader->setVisualKeyGlobal(comp->visualGlobalTime());
		particleShader->setInitialVelocityMult(comp->initVelocityMult());
		particleShader->setMotionKeys(comp->mMotionKeys,comp->mMaxMotionKeys, comp->mLifetime);
		particleShader->setVisualKeys(comp->mVisualKeys, comp->mMaxVisualKeys, comp->mLifetime);
		
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
