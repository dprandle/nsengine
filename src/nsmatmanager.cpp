/*! 
	\file nsmatmanager.cpp
	
	\brief Header file for NSMatManager class

	This file contains all of the neccessary definitions for the NSMatManager class.

	\author Daniel Randle
	\date November 23 2013
	\copywrite Earth Banana Games 2013
*/

#include <nsmatmanager.h>
#include <assimp\material.h>
#include <nstexmanager.h>
#include <nsengine.h>

NSMatManager::NSMatManager():
	NSResManager()
{
	setLocalDirectory(LOCAL_MATERIAL_DIR_DEFAULT);
	setSaveMode(NSResManager::Text);
}

NSMatManager::~NSMatManager()
{}

NSMaterial* NSMatManager::assimpLoadMaterial(const nsstring & pMaterialName, const aiMaterial * pAIMat, const nsstring & pTexDir)
{
	NSMaterial * mat = create(pMaterialName);
	NSTexManager * texmanager = nsengine.manager<NSTexManager>(mPlugID);
	
	nsbool ret = false;

	aiColor3D colorDif(0.0f, 0.0f, 0.0f);
	pAIMat->Get(AI_MATKEY_COLOR_DIFFUSE, colorDif);
	if (!(colorDif.r == 0.0f && colorDif.g == 0.0f && colorDif.b == 0.0f))
		mat->setColor(fvec4(colorDif.r, colorDif.g, colorDif.b, mat->color().w));

	aiColor3D colorSpec(0.0f, 0.0f, 0.0f);
	pAIMat->Get(AI_MATKEY_COLOR_SPECULAR, colorSpec);
	if (!(colorSpec.r == 0.0f && colorSpec.g == 0.0f && colorSpec.b == 0.0f))
		mat->setSpecularColor(fvec3(colorSpec.r, colorSpec.g, colorSpec.b));

	int wf = 0;
	pAIMat->Get(AI_MATKEY_ENABLE_WIREFRAME, wf);
	mat->enableWireframe(wf && 1);

	int ts = 0;
	pAIMat->Get(AI_MATKEY_TWOSIDED, ts);
	mat->enableCulling(ts && 1);

	float shininess = 0.0f;
	float shininessStr = 0.0f;
	pAIMat->Get(AI_MATKEY_SHININESS, shininess);
	pAIMat->Get(AI_MATKEY_SHININESS_STRENGTH, shininessStr);
	mat->setSpecularPower(shininess);
	mat->setSpecularIntensity(shininessStr);

	if (pAIMat->GetTextureCount(aiTextureType_DIFFUSE) > 0)
	{
		aiString path;
		if (pAIMat->GetTexture(aiTextureType_DIFFUSE, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
		{
			nsstring filename = path.C_Str();
			nsstring texName = filename.substr(filename.find_last_of("/\\") + 1);
			nsstring lookName = "";
			size_t extPos = texName.find_last_of(".");

			if (extPos != nsstring::npos)
			{
				lookName = texName.substr(0, extPos);
				NSTexture * tex = texmanager->loadImage(pTexDir + texName);
				if (tex == NULL)
					tex = texmanager->get(lookName);

				if (tex != NULL)
				{
					mat->setMapTextureID(NSMaterial::Diffuse, uivec2(tex->plugid(), tex->id()));
					ret = true;
				}
			}
		}
	}
	if (pAIMat->GetTextureCount(aiTextureType_SPECULAR) > 0)
	{
		aiString path;
		if (pAIMat->GetTexture(aiTextureType_SPECULAR, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
		{
			nsstring filename = path.C_Str();
			nsstring texName = filename.substr(filename.find_last_of("/\\") + 1);
			nsstring lookName = "";
			size_t extPos = texName.find_last_of(".");

			if (extPos != nsstring::npos)
			{
				lookName = texName.substr(0, extPos);
				NSTexture * tex = texmanager->loadImage(pTexDir + texName);
				if (tex == NULL)
					tex = texmanager->get(lookName);

				if (tex != NULL)
				{
					mat->setMapTextureID(NSMaterial::Specular, uivec2(tex->plugid(),tex->id()));
					ret = true;
				}
			}
		}
	}
	if (pAIMat->GetTextureCount(aiTextureType_AMBIENT) > 0)
	{
		aiString path;
		if (pAIMat->GetTexture(aiTextureType_AMBIENT, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
		{
			nsstring filename = path.C_Str();
			nsstring texName = filename.substr(filename.find_last_of("/\\") + 1);
			nsstring lookName = "";
			size_t extPos = texName.find_last_of(".");

			if (extPos != nsstring::npos)
			{
				lookName = texName.substr(0, extPos);
				NSTexture * tex = texmanager->loadImage(pTexDir + texName);
				if (tex == NULL)
					tex = texmanager->get(lookName);

				if (tex != NULL)
				{
					mat->setMapTextureID(NSMaterial::Ambient, uivec2(tex->plugid(),tex->id()));
					ret = true;
				}
			}
		}
	}
	if (pAIMat->GetTextureCount(aiTextureType_EMISSIVE) > 0)
	{
		aiString path;
		if (pAIMat->GetTexture(aiTextureType_EMISSIVE, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
		{
			nsstring filename = path.C_Str();
			nsstring texName = filename.substr(filename.find_last_of("/\\") + 1);
			nsstring lookName = "";
			size_t extPos = texName.find_last_of(".");

			if (extPos != nsstring::npos)
			{
				lookName = texName.substr(0, extPos);
				NSTexture * tex = texmanager->loadImage(pTexDir + texName);
				if (tex == NULL)
					tex = texmanager->get(lookName);

				if (tex != NULL)
				{
					mat->setMapTextureID(NSMaterial::Emissive, uivec2(tex->plugid(),tex->id()));
					ret = true;
				}
			}
		}
	}
	if (pAIMat->GetTextureCount(aiTextureType_HEIGHT) > 0)
	{
		aiString path;
		if (pAIMat->GetTexture(aiTextureType_HEIGHT, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
		{
			nsstring filename = path.C_Str();
			nsstring texName = filename.substr(filename.find_last_of("/\\") + 1);
			nsstring lookName = "";
			size_t extPos = texName.find_last_of(".");

			if (extPos != nsstring::npos)
			{
				lookName = texName.substr(0, extPos);
				NSTexture * tex = texmanager->loadImage(pTexDir + texName);
				if (tex == NULL)
					tex = texmanager->get(lookName);

				if (tex != NULL)
				{
					mat->setMapTextureID(NSMaterial::Height, uivec2(tex->plugid(),tex->id()));
					ret = true;
				}
			}
		}
	}
	if (pAIMat->GetTextureCount(aiTextureType_NORMALS) > 0)
	{
		aiString path;
		if (pAIMat->GetTexture(aiTextureType_NORMALS, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
		{
			nsstring filename = path.C_Str();
			nsstring texName = filename.substr(filename.find_last_of("/\\") + 1);
			nsstring lookName = "";
			size_t extPos = texName.find_last_of(".");

			if (extPos != nsstring::npos)
			{
				lookName = texName.substr(0, extPos);
				NSTexture * tex = texmanager->loadImage(pTexDir + texName);
				if (tex == NULL)
					tex = texmanager->get(lookName);

				if (tex != NULL)
				{
					mat->setMapTextureID(NSMaterial::Normal, uivec2(tex->plugid(),tex->id()));
					ret = true;
				}
			}
		}
	}
	if (pAIMat->GetTextureCount(aiTextureType_SHININESS) > 0)
	{
		aiString path;
		if (pAIMat->GetTexture(aiTextureType_SHININESS, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
		{
			nsstring filename = path.C_Str();
			nsstring texName = filename.substr(filename.find_last_of("/\\") + 1);
			nsstring lookName = "";
			size_t extPos = texName.find_last_of(".");

			if (extPos != nsstring::npos)
			{
				lookName = texName.substr(0, extPos);
				NSTexture * tex = texmanager->loadImage(pTexDir + texName);
				if (tex == NULL)
					tex = texmanager->get(lookName);

				if (tex != NULL)
				{
					mat->setMapTextureID(NSMaterial::Shininess, uivec2(tex->plugid(),tex->id()));
					ret = true;
				}
			}
		}
	}
	if (pAIMat->GetTextureCount(aiTextureType_OPACITY) > 0)
	{
		aiString path;
		if (pAIMat->GetTexture(aiTextureType_OPACITY, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
		{
			nsstring filename = path.C_Str();
			nsstring texName = filename.substr(filename.find_last_of("/\\") + 1);
			nsstring lookName = "";
			size_t extPos = texName.find_last_of(".");

			if (extPos != nsstring::npos)
			{
				lookName = texName.substr(0, extPos);
				NSTexture * tex = texmanager->loadImage(pTexDir + texName);
				if (tex == NULL)
					tex = texmanager->get(lookName);

				if (tex != NULL)
				{
					mat->setMapTextureID(NSMaterial::Opacity, uivec2(tex->plugid(),tex->id()));
					ret = true;
				}
			}
		}
	}
	if (pAIMat->GetTextureCount(aiTextureType_DISPLACEMENT) > 0)
	{
		aiString path;
		if (pAIMat->GetTexture(aiTextureType_DISPLACEMENT, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
		{
			nsstring filename = path.C_Str();
			nsstring texName = filename.substr(filename.find_last_of("/\\") + 1);
			nsstring lookName = "";
			size_t extPos = texName.find_last_of(".");

			if (extPos != nsstring::npos)
			{
				lookName = texName.substr(0, extPos);
				NSTexture * tex = texmanager->loadImage(pTexDir + texName);
				if (tex == NULL)
					tex = texmanager->get(lookName);

				if (tex != NULL)
				{
					mat->setMapTextureID(NSMaterial::Displacement, uivec2(tex->plugid(),tex->id()));
					ret = true;
				}
			}
		}
	}
	if (pAIMat->GetTextureCount(aiTextureType_LIGHTMAP) > 0)
	{
		aiString path;
		if (pAIMat->GetTexture(aiTextureType_LIGHTMAP, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
		{
			nsstring filename = path.C_Str();
			nsstring texName = filename.substr(filename.find_last_of("/\\") + 1);
			nsstring lookName = "";
			size_t extPos = texName.find_last_of(".");

			if (extPos != nsstring::npos)
			{
				lookName = texName.substr(0, extPos);
				NSTexture * tex = texmanager->loadImage(pTexDir + texName);
				if (tex == NULL)
					tex = texmanager->get(lookName);

				if (tex != NULL)
				{
					mat->setMapTextureID(NSMaterial::Light, uivec2(tex->plugid(),tex->id()));
					ret = true;
				}
			}
		}
	}
	if (pAIMat->GetTextureCount(aiTextureType_REFLECTION) > 0)
	{
		aiString path;
		if (pAIMat->GetTexture(aiTextureType_REFLECTION, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
		{
			nsstring filename = path.C_Str();
			nsstring texName = filename.substr(filename.find_last_of("/\\") + 1);
			nsstring lookName = "";
			size_t extPos = texName.find_last_of(".");

			if (extPos != nsstring::npos)
			{
				lookName = texName.substr(0, extPos);
				NSTexture * tex = texmanager->loadImage(pTexDir + texName);
				if (tex == NULL)
					tex = texmanager->get(lookName);

				if (tex != NULL)
				{
					mat->setMapTextureID(NSMaterial::Reflection, uivec2(tex->plugid(),tex->id()));
					ret = true;
				}
			}
		}
	}
	if (!ret)
		mat->setColorMode(true);

	return mat;
}
