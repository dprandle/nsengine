/*------------------------------------------- Noble Steed Engine-------------------------------------*
Authors : Daniel Randle, Alex Bourne
Date Created : Mar 10 2013

File:
	nsmaterial.cpp

Description:
	This file contains the definition for the NSMaterial class along with any helper functions
	that the class may use
*--------------------------------------------------------------------------------------------------*/

#include <nsmaterial.h>
#include <nstexture.h>
#include <nslog_file.h>
#include <nsshader.h>
#include <nsengine.h>
#include <nsshader_manager.h>
#include <nstex_manager.h>

NSMaterial::NSMaterial():
	mAlphaBlend(false),
	mMatShaderID(0),
	mColor(1.0f,1.0f,1.0f,1.0f),
	mColorMode(false),
	mSpecComp(),
	mTextureMaps(),
	mCullEnabled(true),
	mCullMode(GL_BACK),
	mWireframe(false)
{
	setExtension(DEFAULT_MAT_EXTENSION);
}

NSMaterial::~NSMaterial()
{}

NSMaterial::MapIter NSMaterial::begin()
{
	return mTextureMaps.begin();
}

NSMaterial::ConstMapIter NSMaterial::begin() const
{
	return mTextureMaps.begin();
}

void NSMaterial::changeSpecular(float pPowAmount, float pIntensityAmount)
{
	changeSpecularPower(pPowAmount);
	changeSpecularIntensity(pIntensityAmount);
}

void NSMaterial::changeSpecularIntensity(float pAmount)
{
	mSpecComp.mIntensity += pAmount;
}

void NSMaterial::changeSpecularPower(float pAmount)
{
	mSpecComp.mPower += pAmount;
}

void NSMaterial::clear()
{
	mTextureMaps.clear();
}

bool NSMaterial::contains(const MapType & pMType)
{
	auto iter = mTextureMaps.find(pMType);
	return (iter != mTextureMaps.end());
}

void NSMaterial::enableCulling(bool pEnable)
{
	mCullEnabled = pEnable;
}

void NSMaterial::enableWireframe(bool pEnable)
{
	mWireframe = pEnable;
}

NSMaterial::MapIter NSMaterial::end()
{
	return mTextureMaps.end();
}

NSMaterial::ConstMapIter NSMaterial::end() const
{
	return mTextureMaps.end();
}

const fvec4 & NSMaterial::color()
{
	return mColor;
}

const GLenum & NSMaterial::cullMode() const
{
	return mCullMode;
}

bool NSMaterial::colorMode()
{
	return mColorMode;
}

void NSMaterial::pup(NSFilePUPer * p)
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

uivec2 NSMaterial::mapTextureID(MapType pMapType)
{
	TexMap::iterator iter = mTextureMaps.find(pMapType);
	if (iter != mTextureMaps.end())
		return iter->second;
	return uivec2();
}

const uivec2 & NSMaterial::shaderID()
{
	return mMatShaderID;
}

const NSMaterial::SpecularComp & NSMaterial::specular() const
{
	return mSpecComp;
}

const fvec3 & NSMaterial::specularColor() const
{
	return mSpecComp.mColor;
}

float NSMaterial::specularPower() const
{
	return mSpecComp.mPower;
}

float NSMaterial::specularIntensity() const
{
	return mSpecComp.mIntensity;
}

const NSMaterial::TexMap & NSMaterial::texMaps() const
{
	return mTextureMaps;
}

/*!
Get the other resources that this Material uses. This includes all texture maps.
*/
uivec2array NSMaterial::resources()
{
	uivec2array ret;

	// add all texture maps that are available
	auto iter = mTextureMaps.begin();
	while (iter != mTextureMaps.end())
	{
		if (iter->second != 0) // only add if not equal to 0
			ret.push_back(iter->second);
		++iter;
	}

	// Add the shader and all resources shader may use
	if (mMatShaderID != 0)
		ret.push_back(mMatShaderID);

	return ret;
}

void NSMaterial::init()
{
	// do nothing
}

bool NSMaterial::alphaBlend()
{
	return mAlphaBlend;
}

bool NSMaterial::culling() const
{
	return mCullEnabled;
}

bool NSMaterial::wireframe() const
{
	return mWireframe;
}

/*!
This should be called if there was a name change to a resource - will check if the resource is used by this component and if is
is then it will update the handle
*/
void NSMaterial::nameChange(const uivec2 & oldid, const uivec2 newid)
{
	TexMap::iterator iter = mTextureMaps.begin();
	while (iter != mTextureMaps.end())
	{
		if (iter->second.x == oldid.x)
		{
			iter->second.x = newid.x;
			if (iter->second.y == oldid.y)
				iter->second.y = newid.y;
		}
		++iter;
	}

	if (mMatShaderID.x == oldid.x)
	{
		mMatShaderID.x = newid.x;
		if (mMatShaderID.y == oldid.y)
			mMatShaderID.y = newid.y;
	}
}

bool NSMaterial::removeTextureMap(MapType pMapType)
{
	return mTextureMaps.erase(pMapType) && 1;
}

void NSMaterial::setAlphaBlend(bool pBlend)
{
	mAlphaBlend = pBlend;
}

void NSMaterial::setCullMode(GLenum pMode)
{
	mCullMode = pMode;
}

void NSMaterial::setColor(const fvec4 & pColor)
{
	mColor = pColor;
}

void NSMaterial::setColorMode(bool pColorMode)
{
	mColorMode = pColorMode;
}

void NSMaterial::setShaderID(const uivec2 & resID)
{
	mMatShaderID = resID;
}

void NSMaterial::setSpecular(const SpecularComp & pSpecComp)
{
	mSpecComp = pSpecComp;
}

void NSMaterial::setSpecular(float pPower, float pIntensity,const fvec3 & pColor)
{
	setSpecularPower(pPower);
	setSpecularIntensity(pIntensity);
	setSpecularColor(pColor);
}

void NSMaterial::setSpecularColor(const fvec3 & pColor)
{
	mSpecComp.mColor = pColor;
}

void NSMaterial::setSpecularPower(float pPower)
{
	mSpecComp.mPower = pPower;
}

void NSMaterial::setSpecularIntensity(float pIntensity)
{
	mSpecComp.mIntensity = pIntensity;
}

bool NSMaterial::setMapTextureID(MapType pMapType, const uivec2 & pID, bool pOverwrite)
{
	TexMap::iterator iter = mTextureMaps.find(pMapType);
	if (iter != mTextureMaps.end())
	{
		if (pOverwrite)
		{
			mTextureMaps.erase(iter);
			mTextureMaps.emplace(pMapType, pID);
			return true;
		}
		else
			return false;
	}

	mTextureMaps.emplace(pMapType, pID);
	return true;
}
