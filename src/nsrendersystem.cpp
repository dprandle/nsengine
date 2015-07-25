/*! 
	\file nsrendersystem.cpp
	
	\brief Definition file for NSRenderSystem class

	This file contains all of the neccessary definitions for the NSRenderSystem class.

	\author Daniel Randle
	\date November 23 2013
	\copywrite Earth Banana Games 2013
*/

#include <nsrendersystem.h>
#include <nslightcomp.h>
#include <nsselcomp.h>
#include <nsentity.h>
#include <nsengine.h>
#include <nsentitymanager.h>
#include <nsshadermanager.h>
#include <nstexmanager.h>
#include <nsmeshmanager.h>
#include <nstimer.h>
#include <nsmatmanager.h>
#include <nseventdispatcher.h>
#include <nsscene.h>
#include <nsterraincomp.h>

NSRenderSystem::NSRenderSystem() :
mDrawCallMap(),
mGBuffer(new NSGBuffer()),
mFinalBuf(NULL),
mShadowBuf(new NSShadowBuffer()),
mShaders(),
mDebugDraw(false),
mEarlyZEnabled(false),
mLightingEnabled(true),
mScreenfbo(0),
NSSystem()
{}

NSRenderSystem::~NSRenderSystem()
{
	delete mShadowBuf;
	delete mGBuffer;
}

void NSRenderSystem::setScreenfbo(nsuint fbo)
{
	mScreenfbo = fbo;
}

nsuint NSRenderSystem::screenfbo()
{
	return mScreenfbo;
}

void NSRenderSystem::blitFinalFrame()
{
	if (mDebugDraw)
		return;

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mScreenfbo);
	if (nsengine.currentScene() != NULL)
	{
		mFinalBuf->setTarget(NSFrameBuffer::Read);
		mFinalBuf->bind();
		mFinalBuf->setReadBuffer(NSFrameBuffer::Color);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_STENCIL_TEST);
		glDisable(GL_BLEND);
		glBlitFramebuffer(0, 0, DEFAULT_FB_RES_X, DEFAULT_FB_RES_Y, 0, 0, DEFAULT_FB_RES_X, DEFAULT_FB_RES_Y, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		mFinalBuf->setReadBuffer(NSFrameBuffer::None);
		mFinalBuf->unbind();
	}
	else
	{
		glClearColor(0.11f, 0.11f, 0.11f, 0.11f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
}

void NSRenderSystem::enableEarlyZ(bool pEnable)
{
	mEarlyZEnabled = pEnable;
}

void NSRenderSystem::enableLighting(bool pEnable)
{
	mLightingEnabled = pEnable;
	NSFrameBuffer * mCur = nsengine.framebuffer(boundfbo());

	mGBuffer->bind();
	if (pEnable)
		mGBuffer->fb()->updateDrawBuffers();
	else
	{
		NSFrameBuffer::AttachmentPointArray ap;
		ap.push_back(NSFrameBuffer::Color + NSGBuffer::Diffuse);
		ap.push_back(NSFrameBuffer::Color + NSGBuffer::Picking);
		mGBuffer->fb()->setDrawBuffers(&ap);
	}
	
	mCur->bind();
}

bool NSRenderSystem::_validCheck()
{
	static bool shadererr = false;
	static bool shadernull = false;
	static bool defmaterr = false;

	if (!mShaders.valid())
	{
		if (!shadernull)
		{
			dprint("NSRenderSystem::_validCheck: Error - one of the rendering shaders is NULL ");
			shadernull = true;
		}
		return false;
	}
	shadernull = false;

	if (mDefaultMaterial == NULL)
	{
		if (!defmaterr)
		{
			dprint("NSRenderSystem::_validCheck Error - default material is NULL - assign default material to render system");
			defmaterr = true;
		}
		return false;
	}
	defmaterr = false;

	if (mShaders.error())
	{
		if (!shadererr)
		{
			dprint("NSRenderSystem::_validCheck: Error in one of the rendering shaders - check compile log");
			shadererr = true;
		}
		return false;
	}
	shadererr = false;
	return true;
}

void NSRenderSystem::draw()
{
	bool dirlt = false;
	static bool dirlterr = false;

	if (!_validCheck())
		return;

	NSScene * scene = nsengine.currentScene();
	if (scene == NULL)
		return;

	NSEntity * cam = scene->camera();
	if (cam == NULL)
		return;

	NSCamComp * camc = cam->get<NSCamComp>();

	mGBuffer->bind();
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);;
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);;
	if (mEarlyZEnabled)
	{
		glDepthFunc(GL_LESS);
		
		mGBuffer->enableColorWrite(false);

		mShaders.mEarlyZShader->bind();
		mShaders.mEarlyZShader->setProjMat(camc->projCam());
		_drawSceneToDepth(mShaders.mEarlyZShader);

		mShaders.mXFBEarlyZ->bind();
		mShaders.mXFBEarlyZ->setProjCamMat(camc->projCam());
		_drawSceneToDepthXFB(mShaders.mXFBEarlyZ);

		mGBuffer->enableColorWrite(true);
		glDepthMask(GL_FALSE);
		glDepthFunc(GL_LEQUAL);
	}
	
	// Now draw geometry
	GLError("NSTexture::_drawTransformFeedbacks");
	//_drawTransformFeedbacks();
	GLError("NSTexture::_drawTransformFeedbacks");
	_drawGeometry();
	GLError("NSTexture::_drawTransformFeedbacks");
	mGBuffer->unbind();
	mGBuffer->enableTextures();


	if (mDebugDraw)
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mScreenfbo);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		mGBuffer->debugBlit(camc->dim());
		return;
	}


	mFinalBuf->setTarget(NSFrameBuffer::Draw);
	mFinalBuf->bind();
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	glEnable(GL_STENCIL_TEST);
	auto dLIter = scene->entities<NSLightComp>().begin();

	fmat4 projLightMat, proj;
	nsfloat ar;
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

		if (lComp->type() == NSLightComp::Spot && mLightingEnabled)
		{
			ar = float(mShadowBuf->dim(NSShadowBuffer::Spot).w) / float(mShadowBuf->dim(NSShadowBuffer::Spot).h);
			proj = perspective(lComp->angle()*2.0f, ar, lComp->shadowClipping().x, lComp->shadowClipping().y);
			for (nsuint i = 0; i < tComp->count(); ++i)
			{
				projLightMat = proj * lComp->pov(i);
				if (lComp->castShadows())
				{
					mShadowBuf->bind(NSShadowBuffer::Spot);
					glEnable(GL_DEPTH_TEST);
					glDepthMask(GL_TRUE);
					glClear(GL_DEPTH_BUFFER_BIT);
					glCullFace(GL_FRONT);

					glViewport(0, 0, mShadowBuf->dim(NSShadowBuffer::Spot).w, mShadowBuf->dim(NSShadowBuffer::Spot).h);

					mShaders.mSpotShadowShader->bind();
					mShaders.mSpotShadowShader->setProjMat(projLightMat);
					_drawSceneToDepth(mShaders.mSpotShadowShader);

					//mShaders.mXFBSpotShadowMap->bind();
					//mShaders.mXFBSpotShadowMap->setProjLightMat(projLightMat);
					//_drawSceneToDepthXFB(mShaders.mXFBSpotShadowMap);

					glViewport(0, 0, camc->dim().w, camc->dim().h);
				}

				mFinalBuf->bind();
				mShadowBuf->enable(NSShadowBuffer::Spot);
				mFinalBuf->setDrawBuffer(NSFrameBuffer::None);
				glDepthMask(GL_FALSE);
				glClear(GL_STENCIL_BUFFER_BIT);
				glDisable(GL_CULL_FACE);
				glStencilFunc(GL_ALWAYS, 0, 0);
				glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
				glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);

				mShaders.mLightStencilShader->bind();
				mShaders.mLightStencilShader->setProjCamMat(camc->projCam());
				mShaders.mLightStencilShader->setTransform(lComp->transform(i));
				_stencilSpotLight(lComp);
				mFinalBuf->setDrawBuffer(NSFrameBuffer::Color);
				glDisable(GL_DEPTH_TEST);
				glEnable(GL_CULL_FACE);
				glCullFace(GL_FRONT);
				glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
				mShaders.mSpotLightShader->bind();
				mShaders.mSpotLightShader->setCamWorldPos(camTComp->wpos());
				mShaders.mSpotLightShader->setPosition(tComp->wpos(i));
				mShaders.mSpotLightShader->setMaxDepth(lComp->shadowClipping().y - lComp->shadowClipping().x);
				mShaders.mSpotLightShader->setTransform(lComp->transform(i));
				mShaders.mSpotLightShader->setProjCamMat(camc->projCam());
				mShaders.mSpotLightShader->setProjLightMat(projLightMat);
				mShaders.mSpotLightShader->setDirection(tComp->dirVec(NSTFormComp::Target, i));
				_blendSpotLight(lComp);
			}
		}
		else if (lComp->type() == NSLightComp::Point && mLightingEnabled)
		{
			ar = float(mShadowBuf->dim(NSShadowBuffer::Point).w) / float(mShadowBuf->dim(NSShadowBuffer::Point).h);
			proj = perspective(90.0f, ar, lComp->shadowClipping().x, lComp->shadowClipping().y);
			for (nsuint i = 0; i < tComp->count(); ++i)
			{
				projLightMat = proj * lComp->pov(i);
				glCullFace(GL_FRONT);
				if (lComp->castShadows())
				{
					mShadowBuf->bind(NSShadowBuffer::Point);
					glEnable(GL_DEPTH_TEST);
					glDepthMask(GL_TRUE);
					glClear(GL_DEPTH_BUFFER_BIT);

					glViewport(0, 0, mShadowBuf->dim(NSShadowBuffer::Point).w, mShadowBuf->dim(NSShadowBuffer::Point).h);

					mShaders.mPointShadowShader->bind();
					mShaders.mPointShadowShader->setLightPos(tComp->wpos(i));
					mShaders.mPointShadowShader->setMaxDepth(lComp->shadowClipping().y - lComp->shadowClipping().x);
					mShaders.mPointShadowShader->setProjMat(proj);
					fmat4 f;
					f.setColumn(3,fvec4(tComp->wpos(i)*-1.0f,1.0f));
					mShaders.mPointShadowShader->setInverseTMat(f);
					_drawSceneToDepth(mShaders.mPointShadowShader);

					//mShaders.mXFBPointShadowMap->bind();
					//mShaders.mXFBPointShadowMap->setLightPos(tComp->wpos(i));
					//mShaders.mXFBPointShadowMap->setMaxDepth(lComp->shadowClipping().y - lComp->shadowClipping().x);
					//mShaders.mXFBPointShadowMap->setProjMat(proj);
					//mShaders.mXFBPointShadowMap->setInverseTMat(translationMat4(tComp->wpos(i)*-1));
					//_drawSceneToDepthXFB(mShaders.mXFBPointShadowMap);

					glViewport(0, 0, camc->dim().w, camc->dim().h);
				}

				mFinalBuf->bind();
				mShadowBuf->enable(NSShadowBuffer::Point);
				mFinalBuf->setDrawBuffer(NSFrameBuffer::None);
				glDepthMask(GL_FALSE);
				glClear(GL_STENCIL_BUFFER_BIT);
				glDisable(GL_CULL_FACE);
				glStencilFunc(GL_ALWAYS, 0, 0);
				glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
				glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);
				mShaders.mLightStencilShader->bind();
				mShaders.mLightStencilShader->setProjCamMat(camc->projCam());
				mShaders.mLightStencilShader->setTransform(lComp->transform(i));
				_stencilPointLight(lComp);

				mFinalBuf->setDrawBuffer(NSFrameBuffer::Color);
				glEnable(GL_CULL_FACE);
				glCullFace(GL_FRONT);
				glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
				glDisable(GL_DEPTH_TEST);
				mShaders.mPointLightShader->bind();
				mShaders.mPointLightShader->setCamWorldPos(camTComp->wpos());
				mShaders.mPointLightShader->setPosition(tComp->wpos(i));
				mShaders.mPointLightShader->setMaxDepth(lComp->shadowClipping().y - lComp->shadowClipping().x);
				mShaders.mPointLightShader->setTransform(lComp->transform(i));
				mShaders.mPointLightShader->setProjCamMat(camc->projCam());
				_blendPointLight(lComp);
			}
		}
		else if (lComp->type() == NSLightComp::Directional)
		{
			ar = float(mShadowBuf->dim(NSShadowBuffer::Direction).w) / float(mShadowBuf->dim(NSShadowBuffer::Direction).h);
			proj = perspective(160.0f, ar, lComp->shadowClipping().x, lComp->shadowClipping().y);
			for (nsuint i = 0; i < tComp->count(); ++i)
			{
				projLightMat = proj * tComp->pov();
				glCullFace(GL_BACK);

				if (lComp->castShadows() && mLightingEnabled)
				{
					mShadowBuf->bind(NSShadowBuffer::Direction);
					glEnable(GL_DEPTH_TEST);
					glDepthMask(GL_TRUE);
					glClear(GL_DEPTH_BUFFER_BIT);
					glViewport(0, 0, mShadowBuf->dim(NSShadowBuffer::Direction).w, mShadowBuf->dim(NSShadowBuffer::Direction).h);

					mShaders.mDirShadowShader->bind();
					mShaders.mDirShadowShader->setProjMat(projLightMat);
					_drawSceneToDepth(mShaders.mDirShadowShader);

					mShaders.mXFBDirShadowMap->bind();
					mShaders.mXFBDirShadowMap->setProjLightMat(projLightMat);
					_drawSceneToDepthXFB(mShaders.mXFBDirShadowMap);

					glViewport(0, 0, camc->dim().w, camc->dim().h);
				}


				mFinalBuf->bind();
				mShadowBuf->enable(NSShadowBuffer::Direction);
				mFinalBuf->setDrawBuffer(NSFrameBuffer::Color);
				glClear(GL_STENCIL_BUFFER_BIT);
				glDepthMask(GL_FALSE);
				glDisable(GL_DEPTH_TEST);
				glStencilFunc(GL_EQUAL, 0, 0xFF);
				mShaders.mDirLightShader->bind();
				mShaders.mDirLightShader->initUniforms();
				mShaders.mDirLightShader->setProjLightMat(projLightMat);
				mShaders.mDirLightShader->setLightingEnabled(mLightingEnabled);
				mShaders.mDirLightShader->setBackgroundColor(scene->backgroundColor());
				mShaders.mDirLightShader->setDirection(tComp->dirVec(NSTFormComp::Target, i));
				mShaders.mDirLightShader->setCamWorldPos(camTComp->wpos());
				_blendDirectionLight(lComp);
				dirlt = true;
				dirlterr = false;
			}
		}

		++dLIter;
	}
	if (!dirlt && !dirlterr)
	{
		dprint("NSRenderSystem::draw WARNING No directional light - scene will not be drawn. Add a directional light to the scene.");
		dirlterr = true;
	}
}

nsuint NSRenderSystem::boundfbo()
{
	GLint params;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &params);
	return params;
}

bool NSRenderSystem::debugDraw()
{
	return mDebugDraw;
}

NSMaterial * NSRenderSystem::defaultMat()
{
	return mDefaultMaterial;
}


// bool NSRenderSystem::handleEvent(NSEvent * pEvent)
// {
// 	NSScene * scene = nsengine.currentScene();
// 	if (scene == NULL)
// 		return false;
	
// 	if (pEvent == NULL)
// 	{
// 		dprint("NSRenderSystem::handleEvent Event is NULL - bad bad bad");
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

void NSRenderSystem::init()
{
	//nsengine.events()->addListener(this, NSEvent::InputKey);
	glFrontFace(GL_CW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glDepthFunc(GL_LEQUAL);
	glViewport(0, 0, DEFAULT_FB_RES_X, DEFAULT_FB_RES_Y);
	setGBufferfbo(nsengine.createFramebuffer());
	setShadowfbo(nsengine.createFramebuffer(), nsengine.createFramebuffer(), nsengine.createFramebuffer());
}

void NSRenderSystem::enableDebugDraw(bool pDebDraw)
{
	mDebugDraw = pDebDraw;
}

void NSRenderSystem::setDefaultMat(NSMaterial * pDefMat)
{
	mDefaultMaterial = pDefMat;
}

void NSRenderSystem::setShaders(const RenderShaders & pShaders)
{
	mShaders = pShaders;
}

void NSRenderSystem::toggleDebugDraw()
{
	enableDebugDraw(!mDebugDraw);
}

void NSRenderSystem::update()
{
	static bool sceneerr = false;
	static bool camerr = false;

	//nsengine.events()->process(this);

	// Warning message switches (so they dont appear every frame)
	NSScene * scene = nsengine.currentScene();
	if (scene == NULL)
	{
		if (!sceneerr)
		{
			dprint("NSRenderSystem::update WARNING No current scene set");
			sceneerr = true;
		}
		return;
	}
	else
		sceneerr = false;

	NSEntity * cam = scene->camera();
	if (cam == NULL)
	{
		if (!camerr)
		{
			dprint("NSRenderSystem::update WARNING No scene camera set");
			camerr = true;
		}
		return;
	}
	else
		camerr = false;

	 mDrawCallMap.clear();
	 mShaderMatMap.clear();
	 mXFBDraws.clear();

	// update render components and the draw call list
	 fvec2 terh;
	 NSRenderComp * rComp = NULL;
	 NSTFormComp * tComp = NULL;
	 NSAnimComp * animComp = NULL;
	 NSTerrainComp * terComp = NULL;
	 NSMesh * currentMesh = NULL;
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
		currentMesh = nsengine.resource<NSMesh>(rComp->meshID());

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
			mXFBDraws.emplace((*iter));
		else
		{
			NSMesh::SubMesh * mSMesh = NULL;
			NSMaterial * mat = NULL;
			fmat4array * fTForms = NULL;
			NSMaterialShader * shader = NULL;
			for (nsuint i = 0; i < currentMesh->count(); ++i)
			{
				mSMesh = currentMesh->submesh(i);
				mat = nsengine.resource<NSMaterial>(rComp->materialID(i));

				if (mat == NULL)
					mat = mDefaultMaterial;

				shader = nsengine.resource<NSMaterialShader>(mat->shaderID());
				fTForms = NULL;

				if (shader == NULL)
					shader = mShaders.mDefaultShader;

				if (animComp != NULL)
					fTForms = animComp->finalTransforms();

				if (terComp != NULL)
					terh = terComp->heightBounds();

				if (tComp != NULL)
				{
					mDrawCallMap[mat].emplace(DrawCall(mSMesh,
						fTForms,
						tComp->transformBuffer(),
						tComp->transformIDBuffer(),
						terh,
						(*iter)->id(),
						(*iter)->plugid(),
						tComp->count(),
						rComp->castShadow()));
					mShaderMatMap[shader].insert(mat);
				}
			}
		}
		++iter;
	}
}

void NSRenderSystem::_blendDirectionLight(NSLightComp * pLight)
{
	mShaders.mDirLightShader->setAmbientIntensity(pLight->intensity().y);
	mShaders.mDirLightShader->setCastShadows(pLight->castShadows());
	mShaders.mDirLightShader->setDiffuseIntensity(pLight->intensity().x);
	mShaders.mDirLightShader->setLightColor(pLight->color());
	mShaders.mDirLightShader->setShadowSamples(pLight->shadowSamples());
	mShaders.mDirLightShader->setShadowDarkness(pLight->shadowDarkness());
	mShaders.mDirLightShader->setScreenSize(fvec2(static_cast<nsfloat>(mFinalBuf->dim().x), static_cast<nsfloat>(mFinalBuf->dim().y)));
	mShaders.mDirLightShader->setShadowTexSize(fvec2(static_cast<nsfloat>(mShadowBuf->dim(NSShadowBuffer::Direction).w), static_cast<nsfloat>(mShadowBuf->dim(NSShadowBuffer::Direction).y)));

	NSMesh * boundingMesh = nsengine.resource<NSMesh>(pLight->meshid());
	for (nsuint i = 0; i < boundingMesh->count(); ++i)
	{
		NSMesh::SubMesh * cSub = boundingMesh->submesh(i);
		cSub->mVAO.bind();
		glDrawElements(cSub->mPrimType,
					   static_cast<GLsizei>(cSub->mIndices.size()),
					   GL_UNSIGNED_INT,
					   0);
		cSub->mVAO.unbind();
	}
}

void NSRenderSystem::_blendPointLight(NSLightComp * pLight)
{
	mShaders.mPointLightShader->setAmbientIntensity(pLight->intensity().y);
	mShaders.mPointLightShader->setCastShadows(pLight->castShadows());
	mShaders.mPointLightShader->setDiffuseIntensity(pLight->intensity().x);
	mShaders.mPointLightShader->setLightColor(pLight->color());
	mShaders.mPointLightShader->setShadowSamples(pLight->shadowSamples());
	mShaders.mPointLightShader->setShadowDarkness(pLight->shadowDarkness());
	mShaders.mPointLightShader->setScreenSize(fvec2(static_cast<nsfloat>(mFinalBuf->dim().x), static_cast<nsfloat>(mFinalBuf->dim().y)));
	mShaders.mPointLightShader->setShadowTexSize(fvec2(static_cast<nsfloat>(mShadowBuf->dim(NSShadowBuffer::Direction).w), static_cast<nsfloat>(mShadowBuf->dim(NSShadowBuffer::Direction).y)));
	mShaders.mPointLightShader->setConstAtten(pLight->atten().x);
	mShaders.mPointLightShader->setLinAtten(pLight->atten().y);
	mShaders.mPointLightShader->setExpAtten(pLight->atten().z);


	NSMesh * boundingMesh = nsengine.resource<NSMesh>(pLight->meshid());
	for (nsuint i = 0; i < boundingMesh->count(); ++i)
	{
		NSMesh::SubMesh * cSub = boundingMesh->submesh(i);

		if (cSub->mNode != NULL)
			mShaders.mPointLightShader->setUniform("nodeTransform", cSub->mNode->mWorldTransform);
		else
			mShaders.mPointLightShader->setUniform("nodeTransform", fmat4());

		cSub->mVAO.bind();
		glDrawElements(cSub->mPrimType,
					   static_cast<GLsizei>(cSub->mIndices.size()),
					   GL_UNSIGNED_INT,
					   0);
		cSub->mVAO.unbind();
	}
}

void NSRenderSystem::_blendSpotLight(NSLightComp * pLight)
{
	mShaders.mSpotLightShader->setAmbientIntensity(pLight->intensity().y);
	mShaders.mSpotLightShader->setCastShadows(pLight->castShadows());
	mShaders.mSpotLightShader->setDiffuseIntensity(pLight->intensity().x);
	mShaders.mSpotLightShader->setLightColor(pLight->color());
	mShaders.mSpotLightShader->setShadowSamples(pLight->shadowSamples());
	mShaders.mSpotLightShader->setShadowDarkness(pLight->shadowDarkness());
	mShaders.mSpotLightShader->setScreenSize(fvec2(static_cast<nsfloat>(mFinalBuf->dim().x),
												   static_cast<nsfloat>(mFinalBuf->dim().y)));
	mShaders.mSpotLightShader->setShadowTexSize(
		fvec2(static_cast<nsfloat>(mShadowBuf->dim(NSShadowBuffer::Direction).w),
			  static_cast<nsfloat>(mShadowBuf->dim(NSShadowBuffer::Direction).y)));
	mShaders.mSpotLightShader->setConstAtten(pLight->atten().x);
	mShaders.mSpotLightShader->setLinAtten(pLight->atten().y);
	mShaders.mSpotLightShader->setExpAtten(pLight->atten().z);
	mShaders.mSpotLightShader->setCutoff(pLight->cutoff());

	NSMesh * boundingMesh = nsengine.resource<NSMesh>(pLight->meshid());
	for (nsuint i = 0; i < boundingMesh->count(); ++i)
	{
		NSMesh::SubMesh * cSub = boundingMesh->submesh(i);

		if (cSub->mNode != NULL)
			mShaders.mSpotLightShader->setUniform("nodeTransform", cSub->mNode->mWorldTransform);
		else
			mShaders.mSpotLightShader->setUniform("nodeTransform", fmat4());

		cSub->mVAO.bind();
		glDrawElements(cSub->mPrimType,
					   static_cast<GLsizei>(cSub->mIndices.size()),
					   GL_UNSIGNED_INT,
					   0);
		cSub->mVAO.unbind();
	}
}

uivec3 NSRenderSystem::shadowfbo()
{
	NSFrameBuffer * d = mShadowBuf->fb(NSShadowBuffer::Direction);
	NSFrameBuffer * s = mShadowBuf->fb(NSShadowBuffer::Spot);
	NSFrameBuffer * p = mShadowBuf->fb(NSShadowBuffer::Point);
	uivec3 ret;
	if (d != NULL)
		ret.x = d->glid();
	if (s != NULL)
		ret.y = s->glid();
	if (p != NULL)
		ret.z = s->glid();
	return ret;
}

nsuint NSRenderSystem::finalfbo()
{
	if (mFinalBuf != NULL)
		return mFinalBuf->glid();
	return 0;
}

nsuint NSRenderSystem::gbufferfbo()
{
	NSFrameBuffer * fb = mGBuffer->fb();
	if (fb != NULL)
		return fb->glid();
	return 0;
}

void NSRenderSystem::setShadowfbo(nsuint fbodir, nsuint fbospot, nsuint fbopoint)
{
	mShadowBuf->setfb(nsengine.framebuffer(fbodir), NSShadowBuffer::Direction);
	mShadowBuf->setfb(nsengine.framebuffer(fbospot), NSShadowBuffer::Spot);
	mShadowBuf->setfb(nsengine.framebuffer(fbopoint), NSShadowBuffer::Point);
	mShadowBuf->setdim(NSShadowBuffer::Direction, DEFAULT_DIRLIGHT_SHADOW_W, DEFAULT_DIRLIGHT_SHADOW_H);
	mShadowBuf->setdim(NSShadowBuffer::Spot, DEFAULT_SPOTLIGHT_SHADOW_W, DEFAULT_SPOTLIGHT_SHADOW_H);
	mShadowBuf->setdim(NSShadowBuffer::Point, DEFAULT_POINTLIGHT_SHADOW_W, DEFAULT_POINTLIGHT_SHADOW_H);
	mShadowBuf->init();
}

void NSRenderSystem::setGBufferfbo(nsuint fbo)
{
	mGBuffer->setfb(nsengine.framebuffer(fbo));
	mGBuffer->setfbdim(uivec2(DEFAULT_FB_RES_X, DEFAULT_FB_RES_Y));
	mGBuffer->init();
}

void NSRenderSystem::setFinalfbo(nsuint fbo)
{
	mFinalBuf = nsengine.framebuffer(fbo);
	if (mFinalBuf == NULL)
		return;

	mFinalBuf->setdim(DEFAULT_FB_RES_X, DEFAULT_FB_RES_Y);
	mFinalBuf->setTarget(NSFrameBuffer::ReadAndDraw);
	mFinalBuf->bind();
	mFinalBuf->add(mFinalBuf->create<NSTex2D>("RenderedFrame", NSFrameBuffer::Color, FINAL_TEX_UNIT, GL_RGBA32F, GL_RGBA, GL_FLOAT));
	mFinalBuf->add(mGBuffer->depth());
	mFinalBuf->updateDrawBuffers();
}

void NSRenderSystem::_drawTransformFeedbacks()
{
	NSScene * scene = nsengine.currentScene();
	if (scene == NULL)
		return;
	NSEntity * cam = scene->camera();
	if (cam == NULL)
		return;
	NSTFormComp * camTComp = cam->get<NSTFormComp>();
	NSCamComp * camc = cam->get<NSCamComp>();

	XFBDrawSet::iterator drawIter = mXFBDraws.begin();
	while (drawIter != mXFBDraws.end())
	{
		NSRenderComp * rComp = (*drawIter)->get<NSRenderComp>();
		NSTFormComp * tComp = (*drawIter)->get<NSTFormComp>();
		NSMesh * mesh = nsengine.resource<NSMesh>(rComp->meshID());
		if (mesh == NULL)
			return;

		NSTFormComp::XFBData * xbdat = tComp->xfbData();

		auto bufIter = xbdat->mXFBBuffers.begin();
		nsuint tFormByteOffset = 0;
		nsuint indexByteOffset = 0;
		while (bufIter != xbdat->mXFBBuffers.end())
		{
			if (xbdat->mUpdate)
			{
				mShaders.mGBufDefaultXFB->bind();

				bufIter->mTFFeedbackObj->bind();
				glEnable(GL_RASTERIZER_DISCARD);
				bufIter->mTFFeedbackObj->begin();
				for (nsuint subI = 0; subI < mesh->count(); ++subI)
				{
					NSMesh::SubMesh * subMesh = mesh->submesh(subI);

					if (subMesh->mNode != NULL)
						mShaders.mGBufDefaultXFB->setUniform("nodeTransform", subMesh->mNode->mWorldTransform);
					else
						mShaders.mGBufDefaultXFB->setUniform("nodeTransform", fmat4());

					// Set up all of the uniform inputs
					subMesh->mVAO.bind();
					tComp->transformBuffer()->bind();
					for (nsuint tfInd = 0; tfInd < 4; ++tfInd)
					{
						subMesh->mVAO.add(tComp->transformBuffer(), NSShader::InstTrans + tfInd);
						subMesh->mVAO.vertexAttribPtr(NSShader::InstTrans + tfInd, 4, GL_FLOAT, GL_FALSE, sizeof(fmat4), sizeof(fvec4)*tfInd + tFormByteOffset);
						subMesh->mVAO.vertexAttribDiv(NSShader::InstTrans + tfInd, 1);
					}

					tComp->transformIDBuffer()->bind();
					subMesh->mVAO.add(tComp->transformIDBuffer(), NSShader::RefID);
					subMesh->mVAO.vertexAttribIPtr(NSShader::RefID, 1, GL_UNSIGNED_INT, sizeof(nsuint), indexByteOffset);
					subMesh->mVAO.vertexAttribDiv(NSShader::RefID, 1);

					// Draw the stuff without sending the stuff to be rasterized
					glDrawElementsInstanced(subMesh->mPrimType,
											static_cast<GLsizei>(subMesh->mIndices.size()),
											GL_UNSIGNED_INT,
											0,
											bufIter->mInstanceCount);
					
					GLError("NSRenderSystem::_drawTransformFeedbacks glDrawElementsInstanced");

					tComp->transformBuffer()->bind();
					subMesh->mVAO.remove(tComp->transformBuffer());

					tComp->transformIDBuffer()->bind();
					subMesh->mVAO.remove(tComp->transformIDBuffer());

					subMesh->mVAO.unbind();
				}

				bufIter->mTFFeedbackObj->end();
				glDisable(GL_RASTERIZER_DISCARD);
				bufIter->mTFFeedbackObj->unbind();
			}

			for (nsuint subI = 0; subI < mesh->count(); ++subI)
			{
				NSMesh::SubMesh * subMesh = mesh->submesh(subI);

				NSMaterial * mat = nsengine.resource<NSMaterial>(rComp->materialID(subI));
				if (mat == NULL)
					mat = mDefaultMaterial;

				NSMaterialShader * shader = nsengine.resource<NSMaterialShader>(mat->shaderID());
				if (shader == NULL)
					shader = mShaders.mGBufDefaultXBFRender;

				shader->bind();
				shader->setProjCamMat(camc->projCam());
				shader->setLightingEnabled(mLightingEnabled);

				glCullFace(mat->cullMode());

				if (mat->culling())
					glEnable(GL_CULL_FACE);
				else
					glDisable(GL_CULL_FACE);

				if (mat->wireframe())
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				else
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

				shader->setDiffuseMapEnabled(mat->contains(NSMaterial::Diffuse));
				shader->setOpacityMapEnabled(mat->contains(NSMaterial::Opacity));
				shader->setNormalMapEnabled(mat->contains(NSMaterial::Normal));
				shader->setHeightMapEnabled(mat->contains(NSMaterial::Height));

				NSMaterial::ConstMapIter cIter = mat->begin();
				while (cIter != mat->end())
				{
					NSTexture * t = nsengine.resource<NSTexture>(cIter->second);
					if (t != NULL)
						t->enable(cIter->first);
					else
						dprint("NSRenderSystem::_drawGeometry Texture id in material " + mat->name() + " does not refer to any valid texture");
					++cIter;
				}

				shader->setColorMode(mat->colorMode());
				shader->setFragOutColor(mat->color());
				shader->setSpecularPower(mat->specularPower());
				shader->setSpecularColor(mat->specularColor());
				shader->setSpecularIntensity(mat->specularIntensity());
				shader->setEntityID((*drawIter)->id());
				shader->setPluginID((*drawIter)->plugid());

				if (!subMesh->mHasTexCoords)
					shader->setColorMode(true);

				bufIter->mXFBVAO->bind();
				glDrawTransformFeedback(GL_TRIANGLES, bufIter->mTFFeedbackObj->glid());
				GLError("NSRenderSystem::_drawTransformFeedbacks glDrawTransformFeedback");
				bufIter->mXFBVAO->unbind();


				NSMaterial::ConstMapIter eIter = mat->begin();
				while (eIter != mat->end())
				{
					NSTexture * t = nsengine.resource<NSTexture>(eIter->second);
					if (t != NULL)
						t->enable(eIter->first);
					else
						dprint("NSRenderSystem::_drawGeometry Texture id in material " + mat->name() + " does not refer to any valid texture");
					++eIter;
				}

			}
			tFormByteOffset += bufIter->mInstanceCount * sizeof(fmat4);
			indexByteOffset += bufIter->mInstanceCount * sizeof(nsuint);
			++bufIter;
		}
		xbdat->mUpdate = false;
		++drawIter;
	}
}

void NSRenderSystem::_drawCall(DCSet::iterator pDCIter)
{
	// Check to make sure each buffer is allocated before setting the shader attribute : un-allocated buffers
	// are fairly common because not every mesh has tangents for example.. or normals.. or whatever

	pDCIter->mSubMesh->mVAO.bind();
	pDCIter->mTransformBuffer->bind();
	for (nsuint tfInd = 0; tfInd < 4; ++tfInd)
	{
		pDCIter->mSubMesh->mVAO.add(pDCIter->mTransformBuffer, NSShader::InstTrans + tfInd);
		pDCIter->mSubMesh->mVAO.vertexAttribPtr(NSShader::InstTrans + tfInd, 4, GL_FLOAT, GL_FALSE, sizeof(fmat4), sizeof(fvec4)*tfInd);
		pDCIter->mSubMesh->mVAO.vertexAttribDiv(NSShader::InstTrans + tfInd, 1);
	}

	pDCIter->mTransformIDBuffer->bind();
	pDCIter->mSubMesh->mVAO.add(pDCIter->mTransformIDBuffer, NSShader::RefID);
	pDCIter->mSubMesh->mVAO.vertexAttribIPtr(NSShader::RefID, 1, GL_UNSIGNED_INT, sizeof(nsuint), 0);
	pDCIter->mSubMesh->mVAO.vertexAttribDiv(NSShader::RefID, 1);
	GLError("NSRenderSystem::_drawCall 1");
	
	glDrawElementsInstanced(pDCIter->mSubMesh->mPrimType,
							static_cast<GLsizei>(pDCIter->mSubMesh->mIndices.size()),
							GL_UNSIGNED_INT,
							0,
							pDCIter->mNumTransforms);
	
	GLError("NSRenderSystem::_drawCall 2");

	pDCIter->mTransformBuffer->bind();
	pDCIter->mSubMesh->mVAO.remove(pDCIter->mTransformBuffer);
	pDCIter->mTransformIDBuffer->bind();
	pDCIter->mSubMesh->mVAO.remove(pDCIter->mTransformIDBuffer);
	pDCIter->mSubMesh->mVAO.unbind();
}

void NSRenderSystem::_drawGeometry()
{
	NSScene * scene = nsengine.currentScene();
	if (scene == NULL)
		return;
	NSEntity * cam = scene->camera();
	if (cam == NULL)
		return;
	NSTFormComp * camTComp = cam->get<NSTFormComp>();
	NSCamComp * camc = cam->get<NSCamComp>();
	
	// Go through each shader in the shader material map.. because each submesh corresponds to exactly one
	// material we can use this material as the key in to the draw call map minimizing shader switches
	ShaderMaterialMap::iterator shaderIter = mShaderMatMap.begin();
	while (shaderIter != mShaderMatMap.end())
	{
		NSMaterialShader * currentShader = shaderIter->first;
		currentShader->bind();
		currentShader->setProjCamMat(camc->projCam());
		currentShader->setLightingEnabled(mLightingEnabled);
		// Now go through each material that is under the current shader, use the material as a key in the draw call map
		// to get all the draw calls associated with that material, and then draw everything. If a material for some reason
		// is not in the shader map then some items wont draw
		auto matIter = shaderIter->second.begin();
		while (matIter != shaderIter->second.end())
		{

			glCullFace((*matIter)->cullMode());

			if ((*matIter)->culling())
				glEnable(GL_CULL_FACE);
			else
				glDisable(GL_CULL_FACE);

			if ((*matIter)->wireframe())
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			else
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			currentShader->setDiffuseMapEnabled((*matIter)->contains(NSMaterial::Diffuse));
			currentShader->setOpacityMapEnabled((*matIter)->contains(NSMaterial::Opacity));
			currentShader->setNormalMapEnabled((*matIter)->contains(NSMaterial::Normal));
			currentShader->setHeightMapEnabled((*matIter)->contains(NSMaterial::Height));

			NSMaterial::ConstMapIter cIter = (*matIter)->begin();
			while (cIter != (*matIter)->end())
			{
				NSTexture * t = nsengine.resource<NSTexture>(cIter->second);
				if (t != NULL)
					t->enable(cIter->first);
				else
					dprint("NSRenderSystem::_drawGeometry Texture id in material " + (*matIter)->name() + " does not refer to any valid texture");
				++cIter;
			}

			currentShader->setColorMode((*matIter)->colorMode());
			currentShader->setFragOutColor((*matIter)->color());
			currentShader->setSpecularPower((*matIter)->specularPower());
			currentShader->setSpecularColor((*matIter)->specularColor());
			currentShader->setSpecularIntensity((*matIter)->specularIntensity());

			// Finally get the draw calls that go with this material and iterate through all of them
			DCSet & currentSet = mDrawCallMap[*matIter];
			DCSet::iterator  dcIter = currentSet.begin();
			while (dcIter != currentSet.end())
			{
				currentShader->setEntityID(dcIter->mEntID);
				currentShader->setPluginID(dcIter->mPlugID);

				if (!dcIter->mSubMesh->mHasTexCoords)
					currentShader->setColorMode(true);

				if (dcIter->mSubMesh->mNode != NULL)
					currentShader->setNodeTransform(dcIter->mSubMesh->mNode->mWorldTransform);
				else
					currentShader->setNodeTransform(fmat4());

				if (dcIter->mAnimTransforms != NULL)
				{
					currentShader->setHasBones(true);
					currentShader->setBoneTransforms(*dcIter->mAnimTransforms);
				}
				else
					currentShader->setHasBones(false);

				currentShader->setHeightMinMax(dcIter->mHeightMinMax);

				_drawCall(dcIter);
				++dcIter;
			}
			GLError("NSRenderSystem::df");
			NSMaterial::ConstMapIter eIter = (*matIter)->begin();
			while (eIter != (*matIter)->end())
			{
				NSTexture * t = nsengine.resource<NSTexture>(eIter->second);
				if (t != NULL)
					t->disable(eIter->first);
				else
					dprint("NSRenderSystem::_drawGeometry Texture id in material " + (*matIter)->name() + " does not refer to any valid texture");
				++eIter;
			}
			++matIter;
		}
		++shaderIter;
	}
}

void NSRenderSystem::_drawSceneToDepth(NSDepthShader * pShader)
{

	MatDCMap::iterator matIter = mDrawCallMap.begin();
	while (matIter != mDrawCallMap.end())
	{
		pShader->setHeightMapEnabled(false);
		NSTexture * tex = nsengine.resource<NSTexture>(matIter->first->mapTextureID(NSMaterial::Height));
		if (tex != NULL)
		{
			pShader->setHeightMapEnabled(true);
			tex->enable(NSMaterial::Height);
		}

		DCSet & currentSet = matIter->second;
		DCSet::iterator  dcIter = currentSet.begin();
		while (dcIter != currentSet.end())
		{
			if (dcIter->mSubMesh->mPrimType != GL_TRIANGLES)
			{
				++dcIter;
				continue;
			}

			if (!dcIter->mCastShadows)
			{
				++dcIter;
				continue;
			}

			if (dcIter->mSubMesh->mNode != NULL)
				pShader->setNodeTransform(dcIter->mSubMesh->mNode->mWorldTransform);
			else
				pShader->setNodeTransform(fmat4());

			if (dcIter->mAnimTransforms != NULL)
			{
				pShader->setHasBones(true);
				pShader->setBoneTransforms(*dcIter->mAnimTransforms);
			}
			else
				pShader->setHasBones(false);

			pShader->setHeightMinMax(dcIter->mHeightMinMax);
			_drawCall(dcIter);
			++dcIter;
		}
		++matIter;
	}
}


void NSRenderSystem::_drawSceneToDepthXFB(NSShader * pShader)
{
	XFBDrawSet::iterator drawIter = mXFBDraws.begin();
	while (drawIter != mXFBDraws.end())
	{
		NSRenderComp * rComp = (*drawIter)->get<NSRenderComp>();
		NSTFormComp * tComp = (*drawIter)->get<NSTFormComp>();
		NSMesh * mesh = nsengine.resource<NSMesh>(rComp->meshID());
		if (mesh == NULL)
			return;

		NSTFormComp::XFBData * xbdat = tComp->xfbData();

		auto bufIter = xbdat->mXFBBuffers.begin();
		nsuint tFormByteOffset = 0;
		nsuint indexByteOffset = 0;
		if (!xbdat->mUpdate)
		{
			while (bufIter != xbdat->mXFBBuffers.end())
			{
				for (nsuint subI = 0; subI < mesh->count(); ++subI)
				{
					bufIter->mXFBVAO->bind();
					glDrawTransformFeedback(GL_TRIANGLES, bufIter->mTFFeedbackObj->glid());
					GLError("NSRenderSystem::_drawSceneToDepthXFB");
					bufIter->mXFBVAO->unbind();
				}
				++bufIter;
			}
		}
		++drawIter;
	}
}

void NSRenderSystem::_stencilPointLight(NSLightComp * pLight)
{
	NSMesh * boundingMesh = nsengine.resource<NSMesh>(pLight->meshid());
	for (nsuint i = 0; i < boundingMesh->count(); ++i)
	{
		NSMesh::SubMesh * cSub = boundingMesh->submesh(i);

		if (cSub->mNode != NULL)
			mShaders.mLightStencilShader->setNodeTransform(cSub->mNode->mWorldTransform);
		else
			mShaders.mLightStencilShader->setNodeTransform(fmat4());

		cSub->mVAO.bind();
		glDrawElements(cSub->mPrimType,
					   static_cast<GLsizei>(cSub->mIndices.size()),
					   GL_UNSIGNED_INT,
					   0);
		cSub->mVAO.unbind();
	}
}

void NSRenderSystem::_stencilSpotLight(NSLightComp * pLight)
{
	NSMesh * boundingMesh = nsengine.resource<NSMesh>(pLight->meshid());
	for (nsuint i = 0; i < boundingMesh->count(); ++i)
	{
		NSMesh::SubMesh * cSub = boundingMesh->submesh(i);

		if (cSub->mNode != NULL)
			mShaders.mLightStencilShader->setNodeTransform(cSub->mNode->mWorldTransform);
		else
			mShaders.mLightStencilShader->setNodeTransform(fmat4());

		cSub->mVAO.bind();
		glDrawElements(cSub->mPrimType,
					   static_cast<GLsizei>(cSub->mIndices.size()),
					   GL_UNSIGNED_INT,
					   0);
		cSub->mVAO.unbind();
	}
}

NSRenderSystem::DrawCall::DrawCall(NSMesh::SubMesh * pSubMesh, 
	fmat4array * pAnimTransforms,
	NSBufferObject * pTransformBuffer,
	NSBufferObject * pTransformIDBuffer,
	const fvec2 & heightMinMax,
	nsuint pEntID,
	nsuint plugID,
	nsuint pNumTransforms, 
	nsbool pCastShadows) :
mSubMesh(pSubMesh),
mAnimTransforms(pAnimTransforms),
mTransformBuffer(pTransformBuffer),
mTransformIDBuffer(pTransformIDBuffer),
mHeightMinMax(heightMinMax),
mEntID(pEntID),
mPlugID(plugID),
mNumTransforms(pNumTransforms),
mCastShadows(pCastShadows)
{}

NSRenderSystem::DrawCall::~DrawCall()
{}

bool NSRenderSystem::DrawCall::operator<(const DrawCall & rhs) const
{
	if (mEntID < rhs.mEntID)
		return true;
	else if (mEntID == rhs.mEntID)
		return mSubMesh < rhs.mSubMesh;
	return false;
}

bool NSRenderSystem::DrawCall::operator<=(const DrawCall & rhs) const
{
	return (*this < rhs || *this == rhs);
}

bool NSRenderSystem::DrawCall::operator>(const DrawCall & rhs) const
{
	return !(*this <= rhs);
}

bool NSRenderSystem::DrawCall::operator>=(const DrawCall & rhs) const
{
	return !(*this < rhs);
}

bool NSRenderSystem::DrawCall::operator==(const DrawCall & rhs) const
{
	return (mEntID == rhs.mEntID) && (mSubMesh == rhs.mSubMesh);
}

bool NSRenderSystem::DrawCall::operator!=(const DrawCall & rhs) const
{
	return !(*this == rhs);
}

nsint NSRenderSystem::drawPriority()
{
	return RENDER_SYS_DRAW_PR;
}

nsint NSRenderSystem::updatePriority()
{
	return RENDER_SYS_UPDATE_PR;
}
