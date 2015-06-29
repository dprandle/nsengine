/* ----------------------------- Noble Steed Engine----------------------------*
Authors : Daniel Randle, Alex Bourne
Date Created : Mar 5 2013

File:
	nsshader.h

Description:
	This file contains the shader class which can initialize/load/bind shaders
	from file or c style string
*---------------------------------------------------------------------------*/

#ifndef NSSHADER_H
#define NSSHADER_H


#include <myGL\glew.h>
#include <nsglobal.h>
#include <nsglobject.h>
#include <nsmath.h>
#include <vector>
#include <map>
#include <nspupper.h>

#define MAX_BONE_TFORMS 100
#define PARTICLE_MAX_VISUAL_KEYS 50
#define PARTICLE_MAX_MOTION_KEYS 50

class NSLightComp;
class NSTFormComp;

#include <nsresource.h>

class NSShader : public NSResource, public NSGLObject
{
public:
	enum Error { None, Program, Compile, Link, Validate };

	enum ShaderType 
	{
		Vertex = GL_VERTEX_SHADER, 
		Geometry = GL_GEOMETRY_SHADER, 
		Fragment = GL_FRAGMENT_SHADER
	};

	enum TransformFeedbackMode {
		Interleaved = GL_INTERLEAVED_ATTRIBS,
		Separate = GL_SEPARATE_ATTRIBS
	};

	enum Attrib {
		Position = POS_ATT,
		TexCoords = TEX_ATT,
		Normal = NORM_ATT,
		Tangent = TANG_ATT,
		BoneID = BONEID_ATT,
		BoneWeight = BONEWEIGHT_ATT,
		InstTrans = TRANS_ATT,
		RefID = REFID_ATT
	};

	typedef std::unordered_map<nsuint,nsuint> UniformLocMap;
	
	
	template <class PUPer>
	friend void pup(PUPer & p, NSShader & shader);

	NSShader();
	~NSShader();

	bool compile();

	bool compile(ShaderType type);

	void unbind();

	void bind();

	void setxfb(TransformFeedbackMode pMode, nsstringarray * pOutLocs);

	virtual void init() {}

	virtual void initUniforms() {}

	nsuint attrib(const nsstring & pVariableName) const;

	Error error() const;

	void initGL();

	nsuint initUniformLoc(const nsstring & pVariable);

	bool compiled(ShaderType type);

	bool linked();

	bool link();

	virtual void pup(NSFilePUPer * p);

	virtual void pup(NSFilePUPer * p, ShaderType type);

	void release();

	void resetError();

	const nsstring & source(ShaderType type);

	void setName(ShaderType type, const nsstring & name);

	void setSource(ShaderType type, const nsstring & source);

	void setUniform(const nsstring & pVariableName, const fmat4 & pData);

	void setUniform(const nsstring & pVariableName, const fmat3 & pData);

	void setUniform(const nsstring & pVariableName, const fvec4 & pData);

	void setUniform(const nsstring & pVariableName, const fvec3 & pData);

	void setUniform(const nsstring & pVariableName, const fvec2 & pData);

	void setUniform(const nsstring & pVariableName, float pData);

	void setUniform(const nsstring & pVariableName, int pData);

	void setUniform(const nsstring & pVariableName, bool val);

	void setUniform(const nsstring & pVariableName, unsigned int pData);

	nsuint glid(ShaderType type);

	nsstring stagename(ShaderType type);

	TransformFeedbackMode xfb();

	virtual nsstring typeString() { return getTypeString();}

	nsstring managerTypeString() { return getManagerTypeString(); }

	static nsstring getTypeString() { return SHADER_TYPESTRING; }

	static nsstring getManagerTypeString() { return SHADER_MANAGER_TYPESTRING; }

protected:
	struct ShaderStage
	{
		ShaderStage(ShaderType pType);
		nsstring mSource;
		nsuint mStageID;
		ShaderType mType;
	};

	template <class PUPer>
	friend void pup(PUPer & p, NSShader::ShaderStage & stage, const nsstring & varName);

	nsuint _getHashedString(const nsstring & pString);
	void _setupTransformFeedback();
	bool _validate();
	ShaderStage & _stage(ShaderType type);

	Error mErrorState;
	TransformFeedbackMode mMode;
	ShaderStage mVertex;
	ShaderStage mGeometry;
	ShaderStage mFragment;
	nsstringarray mXFBLocs;
	UniformLocMap mUniformLocs;
};

template<class PUPer>
void pup(PUPer & p, NSShader::ShaderType & en, const nsstring & pString)
{
	nsuint in = static_cast<nsuint>(en);
	pup(p, in, pString);
	en = static_cast<NSShader::ShaderType>(in);
}

template <class PUPer>
void pup(PUPer & p, NSShader::ShaderStage & stage, const nsstring & varName)
{
	pup(p, stage.mType, varName + ".type");
	pup(p, stage.mSource, varName + ".source");
}

template <class PUPer>
void pup(PUPer & p, NSShader & shader)
{
	pup(p, shader.mVertex, "vertex");
	pup(p, shader.mGeometry, "geometry");
	pup(p, shader.mFragment, "fragment");

}

class NSLightShader : public NSShader
{
public:
	NSLightShader();
	~NSLightShader();

	virtual void initUniforms();

	void setAmbientIntensity(nsfloat intensity);

	void setCastShadows(bool cast);

	void setDiffuseIntensity(nsfloat intensity);

	void setLightColor(const fvec3 & col);

	void setShadowSamples(int samples);

	void setShadowDarkness(float darkness);

	void setScreenSize(const fvec2 & size);

	void setCamWorldPos(const fvec3 & camPos);

	void setEpsilon(float epsilon);

	void setShadowTexSize(const fvec2 & size);

	void setWorldPosSampler(nsint sampler);

	void setDiffuseSampler(nsint sampler);

	void setNormalSampler(nsint sampler);

	void setMaterialSampler(nsint sampler);

	void setShadowSampler(nsint sampler);

	static nsstring getManagerTypeString() { return SHADER_MANAGER_TYPESTRING; }
};

class NSDirLightShader : public NSLightShader
{
public:
	NSDirLightShader();
	~NSDirLightShader();

	virtual void initUniforms();

	virtual void init();

	void setProjLightMat(const fmat4 & projLightMat);

	void setLightingEnabled(bool enable);

	void setBackgroundColor(const fvec3 & col);

	void setDirection(const fvec3 & dir);

	virtual nsstring typeString() { return getTypeString(); }

	static nsstring getTypeString() { return DIRLIGHTSHADER_TYPESTRING; }

	static nsstring getManagerTypeString() { return SHADER_MANAGER_TYPESTRING; }
};

class NSPointLightShader : public NSLightShader
{
public:
	NSPointLightShader();
	~NSPointLightShader();

	virtual void initUniforms();

	virtual void init();

	void setConstAtten(float att);

	void setLinAtten(float lin);

	void setExpAtten(float exp);

	void setPosition(const fvec3 & pos);

	void setMaxDepth(float maxd);

	void setTransform(const fmat4 & mat) { setUniform("transform", mat); }

	void setProjCamMat(const fmat4 & mat) { setUniform("projCamMat", mat); }

	void setNodeTransform(const fmat4 & mat) { setUniform("nodeTransform", mat); }

	virtual nsstring typeString() { return getTypeString(); }

	static nsstring getTypeString() { return POINTLIGHTSHADER_TYPESTRING; }

	static nsstring getManagerTypeString() { return SHADER_MANAGER_TYPESTRING; }
};

class NSSpotLightShader : public NSPointLightShader
{
public:
	NSSpotLightShader();
	~NSSpotLightShader();

	virtual void initUniforms();

	void setProjLightMat(const fmat4 & projLightMat);

	void setDirection(const fvec3 & dir);

	void setCutoff(float cutoff);

	virtual nsstring typeString() { return getTypeString(); }

	static nsstring getTypeString() { return SPOTLIGHTSHADER_TYPESTRING; }

	static nsstring getManagerTypeString() { return SHADER_MANAGER_TYPESTRING; }
};

class NSMaterialShader : public NSShader
{
public:
	NSMaterialShader();
	~NSMaterialShader();

	virtual void initUniforms();

	virtual void init();

	void setDiffuseSampler(nsint sampler);

	void setOpacitySampler(nsint sampler);

	void setNormalSampler(nsint sampler);

	void setHeightSampler(nsint sampler) { setUniform("heightMap", sampler); }

	void setHeightMapEnabled(bool enabled) { setUniform("hasHeightMap", enabled); }

	void setHeightMinMax(const fvec2 & hu) { setUniform("hminmax", hu); }

	void setSpecularPower(float power);

	void setSpecularIntensity(float intensity);

	void setSpecularColor(const fvec3 & col);

	void setEntityID(nsuint id);

	void setPluginID(nsuint id);

	void setColorMode(bool enable);

	void setFragOutColor(const fvec4 & fragcol);

	void setDiffuseMapEnabled(bool enabled);

	void setOpacityMapEnabled(bool enabled);

	void setNormalMapEnabled(bool enabled);

	void setLightingEnabled(bool enabled);

	void setNodeTransform(const fmat4 & tform);

	void setProjCamMat(const fmat4 & projCam);

	void setBoneTransforms(const fmat4array & transforms);

	void setHasBones(bool hasthem);

	virtual nsstring typeString() { return getTypeString(); }

	static nsstring getTypeString() { return MATERIALSHADER_TYPESTRING; }

	static nsstring getManagerTypeString() { return SHADER_MANAGER_TYPESTRING; }

};

class NSParticleProcessShader : public NSShader
{
public:
	NSParticleProcessShader();
	~NSParticleProcessShader();

	virtual void initUniforms();

	void init();

	void setRandomSampler(nsint sampler);

	void setdt(float dt);

	void setTimeElapsed(float elapsed);

	void setLifetime(nsuint lifetime);

	void setLaunchFreq(float freq);

	void setAngularVelocity(int angVelocity);

	void setMotionKeyGlobal(bool global);

	void setVisualKeyGlobal(bool global);

	void setInterpolateMotionKeys(bool interp);

	void setInterpolateVisualKeys(bool interp);

	void setStartingSize(const fvec2 & size);

	void setEmitterSize(const fvec3 & size);

	void setEmitterShape(nsuint shape);

	void setInitialVelocityMult(const fvec3 & mult);

	void setMotionKeys(const fvec3uimap & keys, nsuint maxKeys, nsuint lifetime);

	void setVisualKeys(const fvec3uimap & keys, nsuint maxKeys, nsuint lifetime);

	virtual nsstring typeString() { return getTypeString(); }

	static nsstring getTypeString() { return PARTICLEPROCESSSHADER_TYPESTRING; }

	static nsstring getManagerTypeString() { return SHADER_MANAGER_TYPESTRING; }
};

class NSParticleRenderShader : public NSShader
{
public:
	NSParticleRenderShader();
	~NSParticleRenderShader();

	virtual void initUniforms();

	void init();

	void setDiffuseSampler(nsint sampler);

	void setColorMode(bool enable);

	void setFragOutColor(const fvec4 & col);

	void setDiffuseMapEnabled(bool enable);

	void setLifetime(float seconds);

	void setBlendMode(nsuint mode);

	void setProjCamMat(const fmat4 & mat);

	void setCamUp(const fvec3 & vec);

	void setCamRight(const fvec3 & vec);

	void setCamTarget(const fvec3 & vec);

	void setWorldUp(const fvec3 & vec);

	virtual nsstring typeString() { return getTypeString(); }

	static nsstring getTypeString() { return PARTICLERENDERSHADER_TYPESTRING; }

	static nsstring getManagerTypeString() { return SHADER_MANAGER_TYPESTRING; }
};

class NSDepthShader : public NSShader
{
public:
	NSDepthShader();
	~NSDepthShader();

	virtual void initUniforms();

	void setNodeTransform(const fmat4 & tform);

	void setBoneTransforms(const fmat4array & transforms);

	void setHasBones(bool hasthem);

	void setProjMat(const fmat4 & mat) { setUniform("projMat", mat); }

	void setHeightSampler(nsint sampler) { setUniform("heightMap", sampler); }

	void setHeightMapEnabled(bool enabled) { setUniform("hasHeightMap", enabled); }

	void setHeightMinMax(const fvec2 & hu) { setUniform("hminmax", hu); }

	static nsstring getManagerTypeString() { return SHADER_MANAGER_TYPESTRING; }
};

class NSDirShadowMapShader : public NSDepthShader
{
public:
	NSDirShadowMapShader();
	~NSDirShadowMapShader();

	virtual void initUniforms();

	void init();

	virtual nsstring typeString() { return getTypeString(); }

	static nsstring getTypeString() { return DIR_SHADOWMAP_SHADER_TYPESTRING; }

	static nsstring getManagerTypeString() { return SHADER_MANAGER_TYPESTRING; }
};

class NSPointShadowMapShader : public NSDepthShader
{
public:
	NSPointShadowMapShader();
	~NSPointShadowMapShader();

	virtual void initUniforms();

	void init() {}

	void setLightPos(const fvec3 & pos);

	void setMaxDepth(float maxd);

	void setInverseTMat(const fmat4 & invt);

	virtual nsstring typeString() { return getTypeString(); }

	static nsstring getTypeString() { return POINT_SHADOWMAP_SHADER_TYPESTRING; }

	static nsstring getManagerTypeString() { return SHADER_MANAGER_TYPESTRING; }
};

class NSSpotShadowMapShader : public NSDepthShader
{
public:
	NSSpotShadowMapShader();
	~NSSpotShadowMapShader();

	virtual void initUniforms();

	void init();

	virtual nsstring typeString() { return getTypeString(); }

	static nsstring getTypeString() { return SPOT_SHADOWMAP_SHADER_TYPESTRING; }

	static nsstring getManagerTypeString() { return SHADER_MANAGER_TYPESTRING; }
};

class NSEarlyZShader : public NSDepthShader
{
public:
	NSEarlyZShader();
	~NSEarlyZShader();

	virtual void initUniforms();

	void init();

	virtual nsstring typeString() { return getTypeString(); }

	static nsstring getTypeString() { return EARLYZ_SHADER_TYPESTRING; }

	static nsstring getManagerTypeString() { return SHADER_MANAGER_TYPESTRING; }
};

class NSDirShadowMapXFBShader : public NSShader
{
public:
	NSDirShadowMapXFBShader(){}
	~NSDirShadowMapXFBShader(){}

	virtual void initUniforms() { setProjLightMat(fmat4()); }

	void init(){ NSShader::init(); }

	void setProjLightMat(const fmat4 & mat){ setUniform("projLightMat", mat); }

	virtual nsstring typeString() { return getTypeString(); }

	static nsstring getTypeString() { return DIR_SHADOWMAPXFB_SHADER_TYPESTRING; }

	static nsstring getManagerTypeString() { return SHADER_MANAGER_TYPESTRING; }
};

class NSPointShadowMapXFBShader : public NSShader
{
public:
	NSPointShadowMapXFBShader(){}
	~NSPointShadowMapXFBShader(){}

	void init(){ NSShader::init(); }

	virtual void initUniforms() {
		setProjMat(fmat4());
		setInverseTMat(fmat4());
		setProjLightMat(fmat4());
		setLightPos(fvec3());
		setMaxDepth(0.0f);
	}

	void setProjMat(const fmat4 & mat){ setUniform("projMat", mat); }

	void setInverseTMat(const fmat4 & mat){ setUniform("inverseTMat", mat); }

	void setProjLightMat(const fmat4 & mat){ setUniform("projLightMat", mat); }

	void setLightPos(fvec3 pos) { setUniform("lightPos", pos); }

	void setMaxDepth(nsfloat d) { setUniform("maxDepth", d); }

	virtual nsstring typeString() { return getTypeString(); }

	static nsstring getTypeString() { return POINT_SHADOWMAPXFB_SHADER_TYPESTRING; }

	static nsstring getManagerTypeString() { return SHADER_MANAGER_TYPESTRING; }
};

class NSSpotShadowMapXFBShader : public NSShader
{
public:
	NSSpotShadowMapXFBShader() {}
	~NSSpotShadowMapXFBShader() {}

	virtual void initUniforms() { setProjLightMat(fmat4()); }

	void init(){ NSShader::init(); }

	void setProjLightMat(const fmat4 & mat){ setUniform("projLightMat", mat); }

	virtual nsstring typeString() { return getTypeString(); }

	static nsstring getTypeString() { return SPOT_SHADOWMAPXFB_SHADER_TYPESTRING; }

	static nsstring getManagerTypeString() { return SHADER_MANAGER_TYPESTRING; }
};

class NSEarlyZXFBShader : public NSSpotShadowMapXFBShader
{
public:
	NSEarlyZXFBShader(){}
	~NSEarlyZXFBShader(){}

	virtual void initUniforms() { setProjCamMat(fmat4()); }

	void init(){ NSShader::init(); }

	void setProjCamMat(const fmat4 & mat){ setUniform("projCamMat", mat); }

	virtual nsstring typeString() { return getTypeString(); }

	static nsstring getTypeString() { return EARLYZXFB_SHADER_TYPESTRING; }

	static nsstring getManagerTypeString() { return SHADER_MANAGER_TYPESTRING; }
};

class NSRenderXFBShader : public NSMaterialShader
{
public:
	NSRenderXFBShader();
	~NSRenderXFBShader();

	virtual nsstring typeString() { return getTypeString(); }

	static nsstring getTypeString() { return RENDERXFB_SHADER_TYPESTRING; }

	static nsstring getManagerTypeString() { return SHADER_MANAGER_TYPESTRING; }
};

class NSXFBShader : public NSShader
{
public:
	NSXFBShader();
	~NSXFBShader();

	virtual void initUniforms();

	void init();

	void setNodeTransform(const fmat4 & mat);

	virtual nsstring typeString() { return getTypeString(); }

	static nsstring getTypeString() { return XFB_SHADER_TYPESTRING; }

	static nsstring getManagerTypeString() { return SHADER_MANAGER_TYPESTRING; }

};

class NSLightStencilShader : public NSShader
{
public:
	NSLightStencilShader() {}
	~NSLightStencilShader() {}

	virtual void initUniforms() {
		setTransform(fmat4());
		setProjCamMat(fmat4());
	}

	void init() {}

	void setNodeTransform(const fmat4 & mat) { setUniform("nodeTransform", mat); }

	void setTransform(const fmat4 & mat) { setUniform("transform", mat); }

	void setProjCamMat(const fmat4 & mat){ setUniform("projCamMat", mat); }

	virtual nsstring typeString() { return getTypeString(); }

	static nsstring getTypeString() { return LIGHTSTENCIL_SHADER_TYPESTRING; }

	static nsstring getManagerTypeString() { return SHADER_MANAGER_TYPESTRING; }
};

class NSSkyboxShader : public NSMaterialShader
{
public:
	NSSkyboxShader() : NSMaterialShader() {}
	~NSSkyboxShader() {}

	virtual nsstring typeString() { return getTypeString(); }

	static nsstring getTypeString() { return SKYBOX_SHADER_TYPESTRING; }

	static nsstring getManagerTypeString() { return SHADER_MANAGER_TYPESTRING; }
};

class NSTransparencyShader : public NSMaterialShader
{
public:
	NSTransparencyShader() : NSMaterialShader() {}
	~NSTransparencyShader() {}

	virtual nsstring typeString() { return getTypeString(); }

	static nsstring getTypeString() { return TRANSPARENCY_SHADER_TYPESTRING; }

	static nsstring getManagerTypeString() { return SHADER_MANAGER_TYPESTRING; }
};

class NSSelectionShader : public NSShader
{
public:
	NSSelectionShader();
	~NSSelectionShader();

	virtual void initUniforms();

	void init();

	void setFragOutColor(const fvec4 & col);

	void setNodeTransform(const fmat4 & tform);

	void setProjCamMat(const fmat4 & projCam);

	void setBoneTransforms(const fmat4array & transforms);

	void setHasBones(bool hasthem);

	void setTransform(const fmat4 & mat);

	void setHeightSampler(nsint sampler) { setUniform("heightMap", sampler); }

	void setHeightMapEnabled(bool enabled) { setUniform("hasHeightMap", enabled); }

	void setHeightMinMax(const fvec2 & hu) { setUniform("hminmax", hu); }

	virtual nsstring typeString() { return getTypeString(); }

	static nsstring getTypeString() { return SELECTION_SHADER_TYPESTRING; }

	static nsstring getManagerTypeString() { return SHADER_MANAGER_TYPESTRING; }
};

#endif