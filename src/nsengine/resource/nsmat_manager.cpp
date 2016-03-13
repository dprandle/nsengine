/*! 
	\file nsmatmanager.cpp
	
	\brief Header file for nsmat_manager class

	This file contains all of the neccessary definitions for the nsmat_manager class.

	\author Daniel Randle
	\date November 23 2013
	\copywrite Earth Banana Games 2013
*/

#include <nsmat_manager.h>
#include <assimp/material.h>
#include <nstex_manager.h>
#include <nsengine.h>
#include <nsplugin_manager.h>

nsmaterial * get_material(const uivec2 & id)
{
	nsplugin * plg = nsep.get(id.x);
	if (plg == nullptr)
		return nullptr;
	return plg->get<nsmaterial>(id.y);
}

nsmat_manager::nsmat_manager():
	nsres_manager()
{
	set_local_dir(LOCAL_MATERIAL_DIR_DEFAULT);
	set_save_mode(nsres_manager::text);
}

nsmat_manager::~nsmat_manager()
{}

nsmaterial* nsmat_manager::assimp_load_material(const nsstring & pMaterialName, const aiMaterial * pAIMat, const nsstring & pTexDir)
{
	nsmaterial * mat = create(pMaterialName);
	nsplugin * plg = nsep.get(m_plugin_id);
	nstex_manager * texmanager = plg->manager<nstex_manager>();
	
	bool ret = false;

	aiColor3D colorDif(0.0f, 0.0f, 0.0f);
	pAIMat->Get(AI_MATKEY_COLOR_DIFFUSE, colorDif);
	if (!(colorDif.r == 0.0f && colorDif.g == 0.0f && colorDif.b == 0.0f))
		mat->set_color(fvec4(colorDif.r, colorDif.g, colorDif.b, mat->color().w));

	aiColor3D colorSpec(0.0f, 0.0f, 0.0f);
	pAIMat->Get(AI_MATKEY_COLOR_SPECULAR, colorSpec);
	if (!(colorSpec.r == 0.0f && colorSpec.g == 0.0f && colorSpec.b == 0.0f))
		mat->set_specular_color(fvec3(colorSpec.r, colorSpec.g, colorSpec.b));

	int32 wf = 0;
	pAIMat->Get(AI_MATKEY_ENABLE_WIREFRAME, wf);
	mat->enable_wireframe(wf && 1);

	int32 ts = 0;
	pAIMat->Get(AI_MATKEY_TWOSIDED, ts);
	mat->enable_culling(ts && 1);

	float shininess = 0.0f;
	float shininessStr = 0.0f;
	pAIMat->Get(AI_MATKEY_SHININESS, shininess);
	pAIMat->Get(AI_MATKEY_SHININESS_STRENGTH, shininessStr);
	mat->set_specular_power(shininess);
	mat->set_specular_intensity(shininessStr);

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
				nstexture * tex = texmanager->load_image(pTexDir + texName);
				if (tex == NULL)
					tex = texmanager->get(lookName);

				if (tex != NULL)
				{
					mat->set_map_tex_id(nsmaterial::diffuse, uivec2(tex->plugin_id(), tex->id()));
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
				nstexture * tex = texmanager->load_image(pTexDir + texName);
				if (tex == NULL)
					tex = texmanager->get(lookName);

				if (tex != NULL)
				{
					mat->set_map_tex_id(nsmaterial::spec, uivec2(tex->plugin_id(),tex->id()));
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
				nstexture * tex = texmanager->load_image(pTexDir + texName);
				if (tex == NULL)
					tex = texmanager->get(lookName);

				if (tex != NULL)
				{
					mat->set_map_tex_id(nsmaterial::ambient, uivec2(tex->plugin_id(),tex->id()));
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
				nstexture * tex = texmanager->load_image(pTexDir + texName);
				if (tex == NULL)
					tex = texmanager->get(lookName);

				if (tex != NULL)
				{
					mat->set_map_tex_id(nsmaterial::emmisive, uivec2(tex->plugin_id(),tex->id()));
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
				nstexture * tex = texmanager->load_image(pTexDir + texName);
				if (tex == NULL)
					tex = texmanager->get(lookName);

				if (tex != NULL)
				{
					mat->set_map_tex_id(nsmaterial::height, uivec2(tex->plugin_id(),tex->id()));
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
				nstexture * tex = texmanager->load_image(pTexDir + texName);
				if (tex == NULL)
					tex = texmanager->get(lookName);

				if (tex != NULL)
				{
					mat->set_map_tex_id(nsmaterial::normal, uivec2(tex->plugin_id(),tex->id()));
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
				nstexture * tex = texmanager->load_image(pTexDir + texName);
				if (tex == NULL)
					tex = texmanager->get(lookName);

				if (tex != NULL)
				{
					mat->set_map_tex_id(nsmaterial::shininess, uivec2(tex->plugin_id(),tex->id()));
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
				nstexture * tex = texmanager->load_image(pTexDir + texName);
				if (tex == NULL)
					tex = texmanager->get(lookName);

				if (tex != NULL)
				{
					mat->set_map_tex_id(nsmaterial::opacity, uivec2(tex->plugin_id(),tex->id()));
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
				nstexture * tex = texmanager->load_image(pTexDir + texName);
				if (tex == NULL)
					tex = texmanager->get(lookName);

				if (tex != NULL)
				{
					mat->set_map_tex_id(nsmaterial::displacement, uivec2(tex->plugin_id(),tex->id()));
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
				nstexture * tex = texmanager->load_image(pTexDir + texName);
				if (tex == NULL)
					tex = texmanager->get(lookName);

				if (tex != NULL)
				{
					mat->set_map_tex_id(nsmaterial::light, uivec2(tex->plugin_id(),tex->id()));
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
				nstexture * tex = texmanager->load_image(pTexDir + texName);
				if (tex == NULL)
					tex = texmanager->get(lookName);

				if (tex != NULL)
				{
					mat->set_map_tex_id(nsmaterial::reflection, uivec2(tex->plugin_id(),tex->id()));
					ret = true;
				}
			}
		}
	}
	if (!ret)
		mat->set_color_mode(true);

	return mat;
}
