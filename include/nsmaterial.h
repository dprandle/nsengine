/*! 
	\file nsmaterial.h
	
	\brief Header file for NSMaterial class

	This file contains all of the neccessary declarations for the NSMaterial class.

	\author Daniel Randle
	\date November 23 2013
	\copywrite Earth Banana Games 2013
*/

#ifndef NSMATERIAL_H
#define NSMATERIAL_H

// Inlcudes
#include <vector>
#include <nsglobal.h>
#include <nstexture.h>
#include <nsresource.h>
#include <unordered_map>
#include <nsmath.h>
#include <nspupper.h>

class NSMaterial : public NSResource
{
public:
	enum MapType {
		Diffuse,
		Normal,
		Height,
		Specular,
		Displacement,
		Ambient,
		Emissive,
		Shininess,
		Opacity,
		Light,
		Reflection
	};

	typedef std::unordered_map<MapType, uivec2> TexMap;
	typedef TexMap::iterator MapIter;
	typedef TexMap::const_iterator ConstMapIter;

	template<class PUPer>
	friend void pup(PUPer & p, NSMaterial & mat);

	struct SpecularComp
	{
		SpecularComp(nsfloat pPower = 0.0f, nsfloat pIntensity = 0.0f, fvec3 pColor = fvec3(1.0f, 1.0f, 1.0f)) :mPower(pPower), mIntensity(pIntensity), mColor(pColor)
		{};
		nsfloat mPower;
		nsfloat mIntensity;
		fvec3 mColor;
	};

	NSMaterial();
	~NSMaterial();

	MapIter begin();
	ConstMapIter begin() const;

	void changeSpecular(nsfloat pPowAmount, nsfloat pIntensityAmount);

	void changeSpecularIntensity(nsfloat pAmount);

	void changeSpecularPower(nsfloat pAmount);

	void clear();

	bool contains(const MapType & pMType);

	void enableCulling(nsbool pEnable);

	void enableWireframe(nsbool pEnable);

	MapIter end();
	ConstMapIter end() const;

	const fvec4 & color();

	nsbool colorMode();

	const GLenum & cullMode() const;

	uivec2 mapTextureID(MapType pMapType);

	const uivec2 & shaderID();

	const TexMap & texMaps() const;

	virtual void pup(NSFilePUPer * p);

	const SpecularComp & specular() const;

	const fvec3 & specularColor() const;

	nsfloat specularPower() const;

	nsfloat specularIntensity() const;

	/*!
	Get the other resources that this Material uses. This includes all texture maps.
	\return Map of resource ID to resource type containing all other used texture map IDs and a shader ID
	*/
	virtual uivec2array resources();

	void init();

	bool alphaBlend();

	nsbool culling() const;

	nsbool wireframe() const;

	/*!
	This should be called if there was a name change to a resource - will check if the resource is used by this component and if is
	is then it will update the handle
	*/
	virtual void nameChange(const uivec2 & oldid, const uivec2 newid);

	bool removeTextureMap(MapType pMapType);

	void setAlphaBlend(nsbool pBlend);

	void setColor(const fvec4 & pColor);

	void setColorMode(nsbool pEnable);

	void setCullMode(GLenum pMode);

	bool setMapTextureID(MapType pMapType, const uivec2 & resID, nsbool pOverwrite = false);

	bool setMapTextureID(MapType pMapType, nsuint plugID, nsuint resID, nsbool pOverwrite = false)
	{
		return setMapTextureID(pMapType, uivec2(plugID,resID), pOverwrite);
	}

	void setShaderID(const uivec2 & resID);

	void setShaderID(nsuint plugID, nsuint resID)
	{
		mMatShaderID.set(plugID, resID);
	}

	void setSpecular(const SpecularComp & pSpecComp);

	void setSpecular(nsfloat pPower, nsfloat pIntensity, const fvec3 & pColor);

	void setSpecularColor(const fvec3 & pColor);

	void setSpecularPower(nsfloat pPower);

	void setSpecularIntensity(nsfloat pIntensity);

	virtual nsstring typeString() { return getTypeString(); }

	nsstring managerTypeString() { return getManagerTypeString(); }

	static nsstring getTypeString() { return MATERIAL_TYPESTRING; }

	static nsstring getManagerTypeString() { return MAT_MANAGER_TYPESTRING; }

private:
	nsbool mAlphaBlend;
	uivec2 mMatShaderID;
	fvec4 mColor;
	nsbool mColorMode;
	nsbool mCullEnabled;
	GLenum mCullMode;
	SpecularComp mSpecComp;
	TexMap mTextureMaps;
	nsbool mWireframe;
};


template<class PUPer>
void pup(PUPer & p, NSMaterial & mat)
{
	pup(p, mat.mAlphaBlend, "alphaBlend");
	pup(p, mat.mMatShaderID, "matShaderID");
	pup(p, mat.mColor, "color");
	pup(p, mat.mColorMode, "colorMode");
	pup(p, mat.mCullEnabled, "cullEnabled");
	pup(p, mat.mCullMode, "cullMode");
	pup(p, mat.mSpecComp, "specComp");
	pup(p, mat.mTextureMaps, "textureMaps");
	pup(p, mat.mWireframe, "wireframe");
}

template<class PUPer>
void pup(PUPer & p, NSMaterial::SpecularComp & mat, const nsstring & pString)
{
	pup(p, mat.mPower, pString + ".mPower");
	pup(p, mat.mIntensity, pString + ".mIntensity");
	pup(p, mat.mColor, pString + ".mColor");
}

template<class PUPer>
void pup(PUPer & p, NSMaterial::MapType & my, const nsstring & pString)
{
	nsuint mt = static_cast<nsuint>(my);
	pup(p, mt, pString);
	my = static_cast<NSMaterial::MapType>(mt);
}

#endif