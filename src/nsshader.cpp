/*--------------------------------------------- Noble Steed Engine--------------------------------------*
Authors : Daniel Randle, Alex Bourne
Date Created : Mar 5 2013

File:
	nsshader.cpp

Description:
	This file contains the definition for the NSShader class and any associated functions that help
	the ol' girl out
*-----------------------------------------------------------------------------------------------------*/
#include <nsmath.h>
#include <nsshader.h>
#include <nsmaterial.h>
#include <nstformcomp.h>
#include <nslightcomp.h>
#include <nsengine.h>
#include <nsengine.h>

NSShader::NSShader():
	mErrorState(None),
	mFragment(Fragment),
	mVertex(Vertex),
	mGeometry(Geometry),
	mUniformLocs(),
	mMode(Interleaved)
{
	setExtension(DEFAULT_SHADER_EXTENSION);
}

NSShader::~NSShader()
{
	release();
}

bool NSShader::compile()
{
	bool ret = true;

	if (!mFragment.mSource.empty())
		ret = compile(Fragment) && ret;
	if (!mVertex.mSource.empty())
		ret = compile(Vertex) && ret;
	if (!mGeometry.mSource.empty())
		ret = compile(Geometry) && ret;

	if (ret)
		dprint("NSShader::compile - Succesfully compiled shader " + mName);
	return ret;
}

bool NSShader::compile(ShaderType type)
{
	if (_stage(type).mStageID != 0)
		glDeleteShader(_stage(type).mStageID);

	_stage(type).mStageID = glCreateShader(type);
	GLError("NSShader::compile error creating shader");
	// create arguements for glShaderSource
	const char* c_str = _stage(type).mSource.c_str();

	glShaderSource(_stage(type).mStageID, 1, &c_str, NULL); // Attach source to shader
	GLError("NSShader::compile error setting shader source");

	glCompileShader(_stage(type).mStageID);
	GLError("NSShader::compile error compiling shader");

	GLint worked;
	glGetShaderiv(_stage(type).mStageID, GL_COMPILE_STATUS, &worked);
	if (!worked)
	{
		char infoLog[1024];
		glGetShaderInfoLog(_stage(type).mStageID, sizeof(infoLog), NULL, (GLchar*)infoLog);
		nsstring info(infoLog);
		mErrorState = Compile;

#ifdef NSDEBUG
		nsstringstream ss;
		ss << "NSShader::compile : Error compiling shader stage " << stagename(type) << " from " << mName << "\n";
			ss << info;
		dprint(ss.str());
#endif

		glDeleteShader(_stage(type).mStageID);
		GLError("NSShader::compile error deleting shader");
		_stage(type).mStageID = 0;
		return false;
	}
	dprint("NSShader::compile - Succesfully compiled shader stage " + stagename(type) + " from " + mName);
	return true;
}

bool NSShader::compiled(ShaderType type)
{
	return (_stage(type).mStageID != 0);
}

bool NSShader::linked()
{
	return (mGLName != 0);
}

void NSShader::setSource(ShaderType type, const nsstring & source)
{
	_stage(type).mSource = source;
}

void NSShader::initGL()
{
	// initGL does nothing for shader - program object created (which mGLName is used for) at
	// time of building shader object
}

nsuint NSShader::glid(ShaderType type)
{
	return _stage(type).mStageID;
}

nsuint NSShader::initUniformLoc(const nsstring & pVariable)
{
	nsuint loc = glGetUniformLocation( mGLName, pVariable.c_str() );
	GLError("NSShader::initUniformLoc");
	mUniformLocs[_getHashedString(pVariable)] = loc;
	return loc;
}

bool NSShader::link()
{
	if (mGLName != 0)
		glDeleteProgram(mGLName);

	mGLName = glCreateProgram();  // create program instance
	GLError("NSShader::link Error creating program");
	if (mGLName == 0)
	{
		mErrorState = Program;
		return false;
	}

	if (mFragment.mStageID)
		glAttachShader(mGLName, mFragment.mStageID);
	if (mVertex.mStageID)
		glAttachShader(mGLName, mVertex.mStageID);
	if (mGeometry.mStageID)
		glAttachShader(mGLName, mGeometry.mStageID);

	GLError("NSShader::link Error attaching program");
	if (!mXFBLocs.empty())
		_setupTransformFeedback();

	glLinkProgram(mGLName);
	GLError("NSShader::link Error linking program");
	if (mFragment.mStageID)
		glDetachShader(mGLName, mFragment.mStageID);
	if (mVertex.mStageID)
		glDetachShader(mGLName, mVertex.mStageID);
	if (mGeometry.mStageID)
		glDetachShader(mGLName, mGeometry.mStageID);

	GLError("NSShader::link Error detaching program");
	GLint worked;
	glGetProgramiv(mGLName, GL_LINK_STATUS, &worked);
	if (!worked)
	{
		char infoLog[1024];
		glGetProgramInfoLog(mGLName, sizeof(infoLog), NULL,(GLchar*)infoLog);
		nsstring info(infoLog);
		mErrorState = Link;
		#ifdef NSDEBUG
		nsstringstream ss;
		ss << "Error linking shader with name " << mName << "\n";
		ss << info;
		dprint(ss.str());
		#endif
		glDeleteProgram(mGLName);
		GLError("NSShader::link Error deleting program");
		mGLName = 0;
		return false;
	}
	dprint("NSShader::link - Succesfully linked shader " + mName);
	/*Validate may fail on older GPUS - add exceptions here!!*/
	return _validate();
}

void NSShader::pup(NSFilePUPer * p)
{
	if (p->type() == NSFilePUPer::Binary)
	{
		NSBinFilePUPer * bf = static_cast<NSBinFilePUPer *>(p);
		::pup(*bf, *this);
	}
	else
	{
		
		NSTextFilePUPer * tf = static_cast<NSTextFilePUPer *>(p);
		::pup(*tf, *this);
	}
}

nsstring NSShader::stagename(ShaderType type)
{
	nsstring typestr;
	switch (type)
	{
	case(Vertex) :
		typestr = "vertex";
		break;
	case(Geometry) :
		typestr = "geometry";
		break;
	case(Fragment) :
		typestr = "fragment";
		break;
	}
	return typestr;
}

void NSShader::pup(NSFilePUPer * p, ShaderType type)
{
	nsstring typestr = stagename(type);

	if (p->type() == NSFilePUPer::Binary)
	{
		NSBinFilePUPer * bf = static_cast<NSBinFilePUPer *>(p);
		::pup(*bf, _stage(type), typestr);
	}
	else
	{
		NSTextFilePUPer * tf = static_cast<NSTextFilePUPer *>(p);
		::pup(*tf, _stage(type), typestr);
	}
}

const nsstring & NSShader::source(ShaderType type)
{
	return _stage(type).mSource;
}

NSShader::ShaderStage & NSShader::_stage(ShaderType type)
{
	switch (type)
	{
	case(Vertex) :
		return mVertex;
	case(Geometry) :
		return mGeometry;
	case(Fragment) :
		return mFragment;
	default:
		throw std::exception("Called non stage");
	}
}

void NSShader::_setupTransformFeedback()
{
	GLchar ** varyings = new GLchar*[mXFBLocs.size()];
	for (nsuint i = 0; i < mXFBLocs.size(); ++i)
	{
		GLchar * str = new GLchar[mXFBLocs[i].size() + 1];
		strcpy(str, mXFBLocs[i].c_str());
		varyings[i] = str;
	}

 	glTransformFeedbackVaryings(mGLName, 4, varyings, mMode);
	GLError("NSShader::_setupTransformFeedback()");

	for (nsuint i = 0; i < mXFBLocs.size(); ++i)
		delete[] varyings[i];
	delete[] varyings;
}

void NSShader::setxfb(TransformFeedbackMode pMode, nsstringarray * pOutLocs)
{
	// Need to link after doing this - if shader has been linked already then just re-link (ie call link again)
	mMode = pMode;
	mXFBLocs.clear();
	nsstringarray::iterator sIter = pOutLocs->begin();
	while (sIter != pOutLocs->end())
	{
		mXFBLocs.push_back(*sIter);
		++sIter;
	}
}

void NSShader::bind()
{
	glUseProgram(mGLName);
	GLError("NSShader::bind");
}

void NSShader::unbind()
{
	glUseProgram(NULL);
	GLError("NSShader::bind");
}

void NSShader::setUniform(const nsstring & pVariableName, const fmat4 & pData)
{
	fmat4 mat = pData;
	UniformLocMap::iterator iter = mUniformLocs.find(_getHashedString(pVariableName));
	if (iter == mUniformLocs.end())
		glUniformMatrix4fv(initUniformLoc(pVariableName), 1, GL_TRUE, mat.dataPtr());
	else
		glUniformMatrix4fv(iter->second, 1, GL_TRUE, mat.dataPtr());
	GLError("NSShader::setUniform");
}

void NSShader::setUniform(const nsstring & pVariableName, const fmat3 & pData)
{
	fmat3 mat = pData;
	UniformLocMap::iterator iter = mUniformLocs.find(_getHashedString(pVariableName));
	if (iter == mUniformLocs.end())
		glUniformMatrix3fv(initUniformLoc(pVariableName), 1, GL_TRUE, mat.dataPtr());
	else
		glUniformMatrix3fv(iter->second, 1, GL_TRUE, mat.dataPtr());
	GLError("NSShader::setUniform");
}

void NSShader::setUniform(const nsstring & pVariableName, bool val)
{
	setUniform(pVariableName, int(val));
}

void NSShader::setUniform(const nsstring & pVariableName, const fvec4 & pData)
{
	GLfloat vec[4];
	vec[0] = pData.x; vec[1] = pData.y; vec[2] = pData.z; vec[3] = pData.w;
	UniformLocMap::iterator iter = mUniformLocs.find(_getHashedString(pVariableName));
	if (iter == mUniformLocs.end())
		glUniform4fv(initUniformLoc(pVariableName), 1, vec);
	else
		glUniform4fv(iter->second, 1, vec);
	GLError("NSShader::setUniform");
}

void NSShader::setUniform(const nsstring & pVariableName, const fvec3 & pData)
{
	GLfloat vec[3];
	vec[0] = pData.x; vec[1] = pData.y; vec[2] = pData.z;
	UniformLocMap::iterator iter = mUniformLocs.find(_getHashedString(pVariableName));
	if (iter == mUniformLocs.end())
		glUniform3fv(initUniformLoc(pVariableName), 1, vec);
	else
		glUniform3fv(iter->second, 1, vec);
	GLError("NSShader::setUniform");
}

void NSShader::setUniform(const nsstring & pVariableName, const fvec2 & pData)
{
	GLfloat vec[2];
	vec[0] = pData.u; vec[1] = pData.v;
	UniformLocMap::iterator iter = mUniformLocs.find(_getHashedString(pVariableName));
	if (iter == mUniformLocs.end())
		glUniform2fv(initUniformLoc(pVariableName), 1, vec);
	else
		glUniform2fv(iter->second, 1, vec);
	GLError("NSShader::setUniform");
}

void NSShader::setUniform(const nsstring & pVariableName, float pData)
{
	UniformLocMap::iterator iter = mUniformLocs.find(_getHashedString(pVariableName));
	if (iter == mUniformLocs.end())
		glUniform1f(initUniformLoc(pVariableName), pData);
	else
		glUniform1f(iter->second, pData);
	GLError("NSShader::setUniform");
}

void NSShader::setUniform(const nsstring & pVariableName, int pData)
{
	UniformLocMap::iterator iter = mUniformLocs.find(_getHashedString(pVariableName));
	if (iter == mUniformLocs.end())
		glUniform1i(initUniformLoc(pVariableName), pData);
	else
		glUniform1i(iter->second, pData);
	GLError("NSShader::setUniform");
}

void NSShader::setUniform(const nsstring & pVariableName, unsigned int pData)
{
	UniformLocMap::iterator iter = mUniformLocs.find(_getHashedString(pVariableName));
	if (iter == mUniformLocs.end())
		glUniform1ui(initUniformLoc(pVariableName), pData);
	else
		glUniform1ui(iter->second, pData);
	GLError("NSShader::setUniform");
}

void NSShader::release()
{
	if (mGLName)
		glDeleteProgram(mGLName);
	if (mFragment.mStageID)
		glDeleteShader(mFragment.mStageID);
	if (mVertex.mStageID)
		glDeleteShader(mVertex.mStageID);
	if (mGeometry.mStageID)
		glDeleteShader(mGeometry.mStageID);
	GLError("NSShader::release");
}

void NSShader::resetError()
{
	mErrorState = None;
}

NSShader::Error NSShader::error() const
{
	return mErrorState;
}

unsigned int NSShader::attrib(const nsstring & pVariableName) const
{
	return glGetAttribLocation(mGLName, pVariableName.c_str());
	GLError("NSShader::attrib");
}

nsuint NSShader::_getHashedString(const nsstring & pString)
{
    nsuint hash = 5381;
    nsint c;
	const char * str = pString.c_str();
	while (c = *str++)
        hash = ((hash << 5) + hash) + c;

    return hash;
}

bool NSShader::_validate()
{
	glValidateProgram(mGLName);
	GLint worked;
    glGetProgramiv(mGLName, GL_VALIDATE_STATUS, &worked);
    if (!worked) 
	{
		char infoLog[1024];
        glGetProgramInfoLog(mGLName, sizeof(infoLog), NULL, infoLog);
		nsstring info(infoLog);
		mErrorState = Validate;

		#ifdef NSDEBUG
		nsstringstream ss;
		ss << "NSShader::_validate : Error validating shader with name " << mName << "\n";
		ss << info;
		dprint(ss.str());
		#endif
        return false;
    }
	return true;
}

NSShader::ShaderStage::ShaderStage(ShaderType pType):
	mSource(),
	mStageID(0),
	mType(pType)
{}


NSLightShader::NSLightShader():NSShader() {}

NSLightShader::~NSLightShader() {}

void NSLightShader::initUniforms()
{
	setShadowSampler(SHADOW_TEX_UNIT);
	setDiffuseSampler(G_DIFFUSE_TEX_UNIT);
	setWorldPosSampler(G_WORLD_POS_TEX_UNIT);
	setNormalSampler(G_NORMAL_TEX_UNIT);
	setMaterialSampler(G_MATERIAL_TEX_UNIT);
	setEpsilon(DEFAULT_SHADOW_EPSILON);
	setAmbientIntensity(0);
	setCastShadows(false);
	setDiffuseIntensity(0);
	setLightColor(fvec3());
	setShadowSamples(0);
	setShadowDarkness(0);
	setScreenSize(fvec2());
	setCamWorldPos(fvec3());
}

void NSLightShader::setAmbientIntensity(nsfloat intensity)
{
	setUniform("light.ambientIntensity", intensity);
}

void NSLightShader::setDiffuseIntensity(nsfloat intensity)
{
	setUniform("light.diffuseIntensity", intensity);
}

void NSLightShader::setCastShadows(bool cast)
{
	setUniform("castShadows", cast);
}

void NSLightShader::setLightColor(const fvec3 & col)
{
	setUniform("light.color", col);
}

void NSLightShader::setShadowSamples(int samples)
{
	setUniform("shadowSamples", samples);
}

void NSLightShader::setShadowDarkness(float darkness)
{
	setUniform("light.shadowDarkness", darkness);
}

void NSLightShader::setShadowTexSize(const fvec2 & size)
{
	setUniform("shadowTexSize", size);
}

void NSLightShader::setScreenSize(const fvec2 & size)
{
	setUniform("screenSize", size);
}

void NSLightShader::setCamWorldPos(const fvec3 & camPos)
{
	setUniform("camWorldPos", camPos);
}

void NSLightShader::setEpsilon(float epsilon)
{
	setUniform("epsilon", epsilon);
}

void NSLightShader::setWorldPosSampler(nsint sampler)
{
	setUniform("gWorldPosMap", sampler);
}

void NSLightShader::setDiffuseSampler(nsint sampler)
{
	setUniform("gDiffuseMap", sampler);
}

void NSLightShader::setNormalSampler(nsint sampler)
{
	setUniform("gNormalMap", sampler);
}

void NSLightShader::setMaterialSampler(nsint sampler)
{
	setUniform("gMatMap", sampler);
}

void NSLightShader::setShadowSampler(nsint sampler)
{
	setUniform("shadowMap", sampler);
}

NSDirLightShader::NSDirLightShader() :NSLightShader() {}

NSDirLightShader::~NSDirLightShader() {}

void NSDirLightShader::initUniforms()
{
	NSLightShader::initUniforms();
	setProjLightMat(fmat4());
	setLightingEnabled(false);
	setBackgroundColor(fvec3());
	setDirection(fvec3());
}

void NSDirLightShader::init() {}

void NSDirLightShader::setProjLightMat(const fmat4 & projLightMat)
{
	setUniform("projLightMat", projLightMat);
}

void NSDirLightShader::setLightingEnabled(bool enable)
{
	setUniform("lightingEnabled", enable);
}

void NSDirLightShader::setBackgroundColor(const fvec3 & col)
{
	setUniform("bgColor", col);
}

void NSDirLightShader::setDirection(const fvec3 & dir)
{
	setUniform("light.direction", dir);
}

NSPointLightShader::NSPointLightShader() :NSLightShader() {}

NSPointLightShader::~NSPointLightShader() {}

void NSPointLightShader::initUniforms()
{
	NSLightShader::initUniforms();
	setConstAtten(0);
	setLinAtten(0);
	setExpAtten(0);
	setPosition(0);
}

void NSPointLightShader::init()
{}

void NSPointLightShader::setConstAtten(float att)
{
	setUniform("light.attConstant", att);
}

void NSPointLightShader::setLinAtten(float lin)
{
	setUniform("light.attLinear", lin);
}

void NSPointLightShader::setExpAtten(float exp)
{
	setUniform("light.attExp", exp);
}

void NSPointLightShader::setPosition(const fvec3 & pos)
{
	setUniform("light.position", pos);
}

void NSPointLightShader::setMaxDepth(float maxd)
{
	setUniform("maxDepth", maxd);
}

NSSpotLightShader::NSSpotLightShader() :NSPointLightShader() {}

NSSpotLightShader::~NSSpotLightShader() {}

void NSSpotLightShader::initUniforms()
{
	NSLightShader::initUniforms();
	setCutoff(0);
	setDirection(fvec3());
	setProjLightMat(fmat4());
}

void NSSpotLightShader::setProjLightMat(const fmat4 & projLightMat)
{
	setUniform("projLightMat", projLightMat);
}

void NSSpotLightShader::setDirection(const fvec3 & dir)
{
	setUniform("light.direction", dir);
}

void NSSpotLightShader::setCutoff(float cutoff)
{
	setUniform("light.cutoff", cutoff);
}

NSMaterialShader::NSMaterialShader() : NSShader() {}
NSMaterialShader::~NSMaterialShader(){}

void NSMaterialShader::initUniforms()
{
	fmat4array b;
	b.resize(MAX_BONE_TFORMS);
	setDiffuseSampler(DIFFUSE_TEX_UNIT);
	setNormalSampler(NORMAL_TEX_UNIT);
	setOpacitySampler(OPACITY_TEX_UNIT);
	setHeightSampler(HEIGHT_TEX_UNIT);
	setHeightMapEnabled(false);
	setHeightMinMax(fvec2(0.0f, 1.0f));
	setSpecularPower(0);
	setSpecularIntensity(0);
	setSpecularColor(fvec3());
	setEntityID(0);
	setPluginID(0);
	setColorMode(false);
	setFragOutColor(fvec4());
	setDiffuseMapEnabled(false);
	setOpacityMapEnabled(false);
	setNormalMapEnabled(false);
	setLightingEnabled(false);
	setNodeTransform(fmat4());
	setProjCamMat(fmat4());
	setBoneTransforms(b);
	setHasBones(false);
}

void NSMaterialShader::init()
{
	
}

void NSMaterialShader::setDiffuseSampler(nsint sampler)
{
	setUniform("diffuseMap", sampler);
}

void NSMaterialShader::setOpacitySampler(nsint sampler)
{
	setUniform("opacityMap", sampler);
}

void NSMaterialShader::setNormalSampler(nsint sampler)
{
	setUniform("normalMap", sampler);
}

void NSMaterialShader::setSpecularPower(float power)
{
	setUniform("specPower", power);
}

void NSMaterialShader::setSpecularIntensity(float intensity)
{
	setUniform("specIntensity", intensity);
}

void NSMaterialShader::setSpecularColor(const fvec3 & col)
{
	setUniform("specColor", col);
}

void NSMaterialShader::setEntityID(nsuint id)
{
	setUniform("entityID", id);
}

void NSMaterialShader::setPluginID(nsuint id)
{
	setUniform("pluginID", id);
}

void NSMaterialShader::setColorMode(bool enable)
{
	setUniform("colorMode", enable);
}

void NSMaterialShader::setFragOutColor(const fvec4 & fragcol)
{
	setUniform("fragColOut", fragcol);
}

void NSMaterialShader::setDiffuseMapEnabled(bool enabled)
{
	setUniform("hasDiffuseMap", enabled);
}

void NSMaterialShader::setOpacityMapEnabled(bool enabled)
{
	setUniform("hasOpacityMap", enabled);
}

void NSMaterialShader::setNormalMapEnabled(bool enabled)
{
	setUniform("hasNormalMap", enabled);
}

void NSMaterialShader::setLightingEnabled(bool enabled)
{
	setUniform("lightingEnabled", enabled);
}

void NSMaterialShader::setNodeTransform(const fmat4 & tform)
{
	setUniform("nodeTransform", tform);
}

void NSMaterialShader::setProjCamMat(const fmat4 & projCam)
{
	setUniform("projCamMat", projCam);
}


void NSMaterialShader::setBoneTransforms(const fmat4array & transforms)
{
	for (nsuint i = 0; i < transforms.size(); ++i)
		setUniform("boneTransforms[" + std::to_string(i) + "]", transforms[i]);
}

void NSMaterialShader::setHasBones(bool hasthem)
{
	setUniform("hasBones", hasthem);
}

NSParticleProcessShader::NSParticleProcessShader() : NSShader() {}
NSParticleProcessShader::~NSParticleProcessShader() {}

void NSParticleProcessShader::initUniforms()
{
	setRandomSampler(RAND_TEX_UNIT);
	setdt(0);
	setTimeElapsed(0);
	setLifetime(0);
	setLaunchFreq(0);
	setAngularVelocity(0);
	setMotionKeyGlobal(false);
	setVisualKeyGlobal(false);
	setInterpolateMotionKeys(false);
	setInterpolateVisualKeys(false);
	setStartingSize(fvec2());
	setEmitterSize(fvec3());
	setEmitterShape(0);
	setInitialVelocityMult(fvec3());
}

void NSParticleProcessShader::init() 
{
	std::vector<nsstring> outLocs2;
	outLocs2.push_back("gPosOut");
	outLocs2.push_back("gVelOut");
	outLocs2.push_back("gScaleAndAngleOut");
	outLocs2.push_back("gAgeOut");
	setxfb(NSShader::Interleaved, &outLocs2);
}

void NSParticleProcessShader::setRandomSampler(nsint sampler)
{
	setUniform("randomTex", sampler);
}

void NSParticleProcessShader::setdt(float dt)
{
	setUniform("dt", dt);
}

void NSParticleProcessShader::setTimeElapsed(float elapsed)
{
	setUniform("timeElapsed", elapsed);
}

void NSParticleProcessShader::setLifetime(nsuint lifetime)
{
	setUniform("lifetime", lifetime);
}

void NSParticleProcessShader::setLaunchFreq(float freq)
{
	setUniform("launchFrequency", freq);
}

void NSParticleProcessShader::setAngularVelocity(int angVelocity)
{
	setUniform("angVelocity", angVelocity);
}

void NSParticleProcessShader::setMotionKeyGlobal(bool global)
{
	setUniform("motionGlobal", global);
}

void NSParticleProcessShader::setVisualKeyGlobal(bool global)
{
	setUniform("visualGlobal", global);
}

void NSParticleProcessShader::setInterpolateMotionKeys(bool interp)
{
	setUniform("interpolateMotion", interp);
}

void NSParticleProcessShader::setInterpolateVisualKeys(bool interp)
{
	setUniform("interpolateVisual", interp);
}

void NSParticleProcessShader::setStartingSize(const fvec2 & size)
{
	setUniform("startingSize", size);
}

void NSParticleProcessShader::setEmitterSize(const fvec3 & size)
{
	setUniform("emitterSize", size);
}

void NSParticleProcessShader::setEmitterShape(nsuint shape)
{
	setUniform("emitterShape", shape);
}

void NSParticleProcessShader::setInitialVelocityMult(const fvec3 & mult)
{
	setUniform("initVelocityMult", mult);
}

void NSParticleProcessShader::setMotionKeys(const fvec3uimap & keys, nsuint maxKeys, nsuint lifetime)
{
	nsuint index = 0;
	fvec3uimap::const_iterator keyIter = keys.begin();
	while (keyIter != keys.end())
	{
		setUniform("forceKeys[" + std::to_string(index) + "].time", float(keyIter->first) / float(maxKeys * 1000) * float(lifetime));
		setUniform("forceKeys[" + std::to_string(index) + "].force", keyIter->second);
		++index;
		++keyIter;
	}
}

void NSParticleProcessShader::setVisualKeys(const fvec3uimap & keys, nsuint maxKeys, nsuint lifetime)
{
	nsuint index = 0;
	fvec3uimap::const_iterator keyIter = keys.begin();
	while (keyIter != keys.end())
	{
		setUniform("sizeKeys[" + std::to_string(index) + "].time", float(keyIter->first) / float(maxKeys * 1000) * float(lifetime));
		setUniform("sizeKeys[" + std::to_string(index) + "].sizeVel", fvec2(keyIter->second.x, keyIter->second.y));
		setUniform("sizeKeys[" + std::to_string(index) + "].alpha", keyIter->second.z);
		++index;
		++keyIter;
	}
}

NSParticleRenderShader::NSParticleRenderShader(): NSShader() {}
NSParticleRenderShader::~NSParticleRenderShader() {}

void NSParticleRenderShader::initUniforms()
{
	setDiffuseSampler(DIFFUSE_TEX_UNIT);
	setColorMode(false);
	setFragOutColor(fvec4());
	setDiffuseMapEnabled(false);
	setLifetime(0.0f);
	setBlendMode(0);
	setProjCamMat(fmat4());
	setCamUp(fvec3());
	setCamRight(fvec3());
	setCamTarget(fvec3());
	setWorldUp(fvec3());
}

void NSParticleRenderShader::init() {}

void NSParticleRenderShader::setDiffuseSampler(nsint sampler)
{
	setUniform("diffuseMap", sampler);
}

void NSParticleRenderShader::setColorMode(bool enable)
{
	setUniform("colorMode", enable);
}

void NSParticleRenderShader::setFragOutColor(const fvec4 & col)
{
	setUniform("fragColOut", col);
}

void NSParticleRenderShader::setDiffuseMapEnabled(bool enable)
{
	setUniform("hasDiffuseMap", enable);
}

void NSParticleRenderShader::setLifetime(float seconds)
{
	setUniform("lifetimeSecs", seconds);
}

void NSParticleRenderShader::setBlendMode(nsuint mode)
{
	setUniform("blendMode", mode);
}

void NSParticleRenderShader::setProjCamMat(const fmat4 & mat)
{
	setUniform("projCamMat", mat);
}

void NSParticleRenderShader::setCamUp(const fvec3 & vec)
{
	setUniform("camUp", vec);
}

void NSParticleRenderShader::setCamRight(const fvec3 & vec)
{
	setUniform("rightVec", vec);
}

void NSParticleRenderShader::setCamTarget(const fvec3 & vec)
{
	setUniform("camTarget", vec);
}

void NSParticleRenderShader::setWorldUp(const fvec3 & vec)
{
	setUniform("worldUp", vec);
}

NSDepthShader::NSDepthShader() : NSShader(){}
NSDepthShader::~NSDepthShader() {}

void NSDepthShader::initUniforms()
{
	fmat4array b;
	b.resize(MAX_BONE_TFORMS);
	setHeightSampler(HEIGHT_TEX_UNIT);
	setHeightMapEnabled(false);
	setHeightMinMax(fvec2(0.0f, 1.0f));
	setNodeTransform(fmat4());
	setBoneTransforms(b);
	setProjMat(fmat4());
	setHasBones(false);
}

void NSDepthShader::setNodeTransform(const fmat4 & tform)
{
	setUniform("nodeTransform", tform);
}

void NSDepthShader::setBoneTransforms(const fmat4array & transforms)
{
	for (nsuint i = 0; i < transforms.size(); ++i)
		setUniform("boneTransforms[" + std::to_string(i) + "]", transforms[i]);
}

void NSDepthShader::setHasBones(bool hasthem)
{
	setUniform("hasBones", hasthem);
}

NSDirShadowMapShader::NSDirShadowMapShader() :NSDepthShader() {}

NSDirShadowMapShader::~NSDirShadowMapShader() {}

void NSDirShadowMapShader::initUniforms()
{}

void NSDirShadowMapShader::init() {}

NSPointShadowMapShader::NSPointShadowMapShader() :NSDepthShader() {}

NSPointShadowMapShader::~NSPointShadowMapShader() {}

void NSPointShadowMapShader::initUniforms()
{
	setLightPos(fvec3());
	setMaxDepth(0.0f);
	setInverseTMat(fmat4());
	NSDepthShader::initUniforms();
}

void init()
{

}

void NSPointShadowMapShader::setLightPos(const fvec3 & pos)
{
	setUniform("lightPos", pos);
}

void NSPointShadowMapShader::setMaxDepth(float maxd)
{
	setUniform("maxDepth", maxd);
}

void NSPointShadowMapShader::setInverseTMat(const fmat4 & invt)
{
	setUniform("inverseTMat", invt);
}

NSSpotShadowMapShader::NSSpotShadowMapShader() :NSDepthShader() {}

NSSpotShadowMapShader::~NSSpotShadowMapShader() {}

void NSSpotShadowMapShader::initUniforms()
{}

void NSSpotShadowMapShader::init() {}

NSEarlyZShader::NSEarlyZShader() : NSDepthShader() {}

NSEarlyZShader::~NSEarlyZShader() {}

void NSEarlyZShader::initUniforms()
{}

void NSEarlyZShader::init()
{}

NSRenderXFBShader::NSRenderXFBShader() : NSMaterialShader() {}
NSRenderXFBShader::~NSRenderXFBShader() {}


NSXFBShader::NSXFBShader() : NSShader() {}
NSXFBShader::~NSXFBShader() {}

void NSXFBShader::initUniforms()
{
	setNodeTransform(fmat4());
}

void NSXFBShader::init()
{
	std::vector<nsstring> outLocs;
	outLocs.push_back("worldPos");
	outLocs.push_back("texCoords");
	outLocs.push_back("normal");
	outLocs.push_back("tangent");
	setxfb(NSShader::Separate, &outLocs);
}

void NSXFBShader::setNodeTransform(const fmat4 & mat)
{
	setUniform("nodeTransform", mat);
}

NSSelectionShader::NSSelectionShader() : NSShader() {}
NSSelectionShader::~NSSelectionShader() {}

void NSSelectionShader::initUniforms()
{
	fmat4array b;
	b.resize(MAX_BONE_TFORMS);
	setHeightSampler(HEIGHT_TEX_UNIT);
	setHeightMapEnabled(false);
	setHeightMinMax(fvec2(0.0f, 1.0f));
	setNodeTransform(fmat4());
	setBoneTransforms(b);
	setHasBones(false);
	setProjCamMat(fmat4());
	setFragOutColor(fvec4());
	setTransform(fmat4());
}

void NSSelectionShader::init() {}

void NSSelectionShader::setFragOutColor(const fvec4 & col)
{
	setUniform("fragColOut", col);
}

void NSSelectionShader::setNodeTransform(const fmat4 & tform)
{
	setUniform("nodeTransform", tform);
}

void NSSelectionShader::setProjCamMat(const fmat4 & projCam)
{
	setUniform("projCamMat", projCam);
}

void NSSelectionShader::setBoneTransforms(const fmat4array & transforms)
{
	for (nsuint i = 0; i < transforms.size(); ++i)
		setUniform("boneTransforms[" + std::to_string(i) + "]", transforms[i]);
}

void NSSelectionShader::setHasBones(bool hasthem)
{
	setUniform("hasBones", hasthem);
}

void NSSelectionShader::setTransform(const fmat4 & mat)
{
	setUniform("transform", mat);
}