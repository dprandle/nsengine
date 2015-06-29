/*! 
	\file nsshadermanager.cpp
	
	\brief Definition file for NSShaderManager class

	This file contains all of the neccessary definitions for the NSShaderManager class.

	\author Daniel Randle
	\date November 23 2013
	\copywrite Earth Banana Games 2013
*/

#include <nsshadermanager.h>
#include <nsengine.h>

NSShaderManager::NSShaderManager():NSResManager()
{
	setLocalDirectory(LOCAL_SHADER_DIR_DEFAULT);
	setSaveMode(Text);
}

NSShaderManager::~NSShaderManager()
{}

bool NSShaderManager::compile()
{
	bool ret = true;
	MapType::iterator iter = begin();
	while (iter != end())
	{
		ret = ((NSShader*)iter->second)->compile() && ret;
		++iter;
	}
	return ret;
}

void NSShaderManager::initUniforms()
{
	MapType::iterator iter = begin();
	while (iter != end())
	{
		NSShader * shader = static_cast<NSShader*>(iter->second);
		shader->bind();
		shader->initUniforms();
		shader->unbind();
		++iter;
	}
}

bool NSShaderManager::link()
{
	bool ret = true;
	MapType::iterator iter = begin();
	while (iter != end())
	{
		ret = ((NSShader*)iter->second)->link() && ret;
		++iter;
	}
	return ret;
}

nsstring NSShaderManager::getTypeString()
{
	return SHADER_MANAGER_TYPESTRING;
}