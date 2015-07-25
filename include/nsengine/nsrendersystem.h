/*! 
	\file nsrendersystem.h
	
	\brief Header file for NSRenderSystem class

	This file contains all of the neccessary declarations for the NSRenderSystem class.

	\author Daniel Randle
	\date November 23 2013
	\copywrite Earth Banana Games 2013
*/

#ifndef NSRENDERSYSTEM_H
#define NSRENDERSYSTEM_H
#include <nsglobal.h>
#include <nssystem.h>
#include <nsrendercomp.h>
#include <nsanimcomp.h>
#include <nstformcomp.h>
#include <nsbufferobject.h>
#include <map>

#include <nsframebuffer.h>


class NSRenderSystem : public NSSystem
{
public:
	struct DrawCall
	{
		DrawCall(NSMesh::SubMesh * pSubMesh,
			fmat4array * pAnimTransforms,
			NSBufferObject * pTransformBuffer,
			NSBufferObject * pTransformIDBuffer,
			const fvec2 & heightMinMax,
			nsuint pEntID,
			nsuint plugID,
			nsuint pNumTransforms,
			nsbool pCastShadows);
		~DrawCall();

		NSMesh::SubMesh * mSubMesh;
		fmat4array * mAnimTransforms;
		NSBufferObject * mTransformBuffer;
		NSBufferObject * mTransformIDBuffer;
		fvec2 mHeightMinMax;
		nsuint mEntID;
		nsuint mPlugID;
		nsuint mNumTransforms;
		nsbool mCastShadows;

		bool operator<(const DrawCall & rhs) const;
		bool operator<=(const DrawCall & rhs) const;
		bool operator>(const DrawCall & rhs) const;
		bool operator>=(const DrawCall & rhs) const;
		bool operator!=(const DrawCall & rhs) const;
		bool operator==(const DrawCall & rhs) const;
	};

	struct RenderShaders
	{
		RenderShaders() :
			mDefaultShader(NULL),
			mEarlyZShader(NULL),
			mLightStencilShader(NULL),
			mDirLightShader(NULL),
			mPointLightShader(NULL),
			mSpotLightShader(NULL),
			mPointShadowShader(NULL),
			mSpotShadowShader(NULL),
			mDirShadowShader(NULL),
			mGBufDefaultXFB(NULL),
			mGBufDefaultXBFRender(NULL),
			mXFBEarlyZ(NULL),
			mXFBDirShadowMap(NULL),
			mXFBPointShadowMap(NULL),
			mXFBSpotShadowMap(NULL)
		{}

		NSMaterialShader * mDefaultShader;
		NSEarlyZShader * mEarlyZShader;
		NSLightStencilShader * mLightStencilShader;
		NSDirLightShader * mDirLightShader;
		NSPointLightShader * mPointLightShader;
		NSSpotLightShader * mSpotLightShader;
		NSPointShadowMapShader * mPointShadowShader;
		NSSpotShadowMapShader * mSpotShadowShader;
		NSDirShadowMapShader * mDirShadowShader;
		NSXFBShader * mGBufDefaultXFB;
		NSRenderXFBShader * mGBufDefaultXBFRender;
		NSEarlyZXFBShader * mXFBEarlyZ;
		NSDirShadowMapXFBShader * mXFBDirShadowMap;
		NSPointShadowMapXFBShader * mXFBPointShadowMap;
		NSSpotShadowMapXFBShader * mXFBSpotShadowMap;

		bool error()
		{
			return (
				mDefaultShader->error() != NSShader::None ||
				mEarlyZShader->error() != NSShader::None ||
				mLightStencilShader->error() != NSShader::None ||
				mDirLightShader->error() != NSShader::None ||
				mPointLightShader->error() != NSShader::None ||
				mSpotLightShader->error() != NSShader::None ||
				mPointShadowShader->error() != NSShader::None ||
				mSpotShadowShader->error() != NSShader::None ||
				mDirShadowShader->error() != NSShader::None ||
				mGBufDefaultXFB->error() != NSShader::None ||
				mXFBEarlyZ->error() != NSShader::None ||
				mXFBDirShadowMap->error() != NSShader::None ||
				mXFBPointShadowMap->error() != NSShader::None ||
				mXFBSpotShadowMap->error() != NSShader::None ||
				mGBufDefaultXBFRender->error() != NSShader::None);
		}

		bool valid()
		{
			return (
				mDefaultShader != NULL &&
				mEarlyZShader != NULL &&
				mLightStencilShader != NULL &&
				mDirLightShader != NULL &&
				mPointLightShader != NULL &&
				mSpotLightShader != NULL &&
				mPointShadowShader != NULL &&
				mSpotShadowShader != NULL &&
				mDirShadowShader != NULL &&
				mGBufDefaultXFB != NULL &&
				mXFBEarlyZ != NULL &&
				mXFBDirShadowMap != NULL &&
				mXFBPointShadowMap != NULL &&
				mXFBSpotShadowMap != NULL &&
				mGBufDefaultXBFRender != NULL);
		}
	};

	typedef std::set<DrawCall> DCSet;
	typedef std::map<NSMaterial*, DCSet> MatDCMap;
	typedef std::map<NSMaterialShader*, nspmatset> ShaderMaterialMap;
	typedef std::set<NSEntity*> XFBDrawSet;

	NSRenderSystem();
	~NSRenderSystem();

	void blitFinalFrame();

	void draw();

	void enableEarlyZ(bool pEnable);

	void enableDebugDraw(bool pDebDraw);

	void enableLighting(bool pEnable);

	NSMaterial * defaultMat();

	uivec3 pick(nsfloat mousex, nsfloat mousey);

	//virtual bool handleEvent(NSEvent * pEvent);

	virtual void init();

	uivec3 shadowfbo();

	nsuint finalfbo();

	nsuint gbufferfbo();

	nsuint boundfbo();

	nsuint screenfbo();

	void setScreenfbo(nsuint fbo);

	bool debugDraw();

	void setDefaultMat(NSMaterial * pDefMaterial);

	void setShaders(const RenderShaders & pShaders);

	void setGBufferfbo(nsuint fbo);

	void setFinalfbo(nsuint fbo);

	void setShadowfbo(nsuint fbo1, nsuint fbo2, nsuint fbo3);

	void toggleDebugDraw();

	void update();

	virtual nsint drawPriority();

	virtual nsint updatePriority();

private:
	void _blendDirectionLight(NSLightComp * pLight);
	void _blendPointLight(NSLightComp * pLight);
	void _blendSpotLight(NSLightComp * pLight);
	void _stencilPointLight(NSLightComp * pLight);
	void _stencilSpotLight(NSLightComp * pLight);
	void _drawGeometry();
	void _drawTransformFeedbacks();
	void _drawSceneToDepth(NSDepthShader * pShader);
	void _drawSceneToDepthXFB(NSShader * pShader);
	void _drawCall(DCSet::iterator pDCIter);
	bool _validCheck();

	bool mDebugDraw;
	bool mEarlyZEnabled;
	bool mLightingEnabled;


	MatDCMap mDrawCallMap;
	//MatDCMap mTransparentDrawCallMap;
	ShaderMaterialMap mShaderMatMap;
	//ShaderMaterialMap mTransparentShaderMatMap;
	XFBDrawSet mXFBDraws;
	nsuint mScreenfbo;

	NSGBuffer * mGBuffer;
	NSShadowBuffer * mShadowBuf;
	NSFrameBuffer * mFinalBuf;
	NSMaterial * mDefaultMaterial;
	RenderShaders mShaders;
};

#endif
