/*! 
	\file nsshadermanager.cpp
	
	\brief Definition file for NSShaderManager class

	This file contains all of the neccessary definitions for the NSShaderManager class.

	\author Daniel Randle
	\date November 23 2013
	\copywrite Earth Banana Games 2013
*/

#include <nsshadermanager.h>
#include <nsfileos.h>
#include <nsengine.h>

NSShaderManager::NSShaderManager():NSResManager()
{
	setLocalDirectory(LOCAL_SHADER_DIR_DEFAULT);
	setSaveMode(Text);
}

NSShaderManager::~NSShaderManager()
{}

bool NSShaderManager::compile(NSShader * sh)
{
	return sh->compile();		
}

bool NSShaderManager::compileAll()
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

void NSShaderManager::initUniforms(NSShader * sh)
{
	sh->initUniforms();
}

void NSShaderManager::initUniformsAll()
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

bool NSShaderManager::loadStage(NSShader * sh, const nsstring & fname, NSShader::ShaderType stagetype)
{
	nsstring fName;
	bool shouldPrefix = false;
	nsstring prefixdirs = mResourceDirectory + mLocalDirectory;

	size_t pos = fname.find_last_of("/\\");
	if (pos != nsstring::npos)
	{
		if (fname[0] != '/' && fname[0] != '.' && fname.find(":") == nsstring::npos) // then subdir
			shouldPrefix = true;
	}
	else
		shouldPrefix = true;

	if (shouldPrefix)
		fName = prefixdirs + fname;
	else
		fName = fname;
	
	nsfstream file;
	NSFilePUPer * p;
	if (mSaveMode == Binary)
	{
		file.open(fName, nsfstream::in | nsfstream::binary);
		p = new NSBinFilePUPer(file, PUP_IN);
	}
	else
	{
		file.open(fName, nsfstream::in);
		p = new NSTextFilePUPer(file, PUP_IN);
	}

	if (!file.is_open())
	{
		dprint("NSShaderManager::loadStage : Error opening file with name - " + fName);
		delete p;
		return false;
	}

	nsstring rt;
	if (mSaveMode == Binary)
		pup(*(static_cast<NSBinFilePUPer*>(p)), rt, "type");
	else
		pup(*(static_cast<NSTextFilePUPer*>(p)), rt, "type");

	if (rt != nsengine.guid(sh->type()) + "_stage:" + sh->stagename(stagetype))
	{
		dprint("NSShaderManager::loadStage Attempted to load shader stage " + fName + " which is not of stage type " + std::to_string(stagetype));
		delete p;
		file.close();
		destroy(sh);
		return false;
	}

	sh->pup(p, stagetype);
	dprint("NSShaderManager::loadStage - Succesfully loaded stage " + sh->stagename(stagetype) + " from file " + fName);
	delete p;
	file.close();
	return true;
}

bool NSShaderManager::saveStage(NSShader * sh, const nsstring & filename, NSShader::ShaderType stagetype)
{
	nsstring fName;
	bool shouldPrefix = false;
	
	size_t pos = filename.find_last_of("/\\");
	if (pos != nsstring::npos)
	{
		if (filename[0] != '/' && filename[0] != '.' && filename.find(":") == nsstring::npos) // then subdir
			shouldPrefix = true;
	}
	else
		shouldPrefix = true;

	if (shouldPrefix)
		fName = mResourceDirectory + mLocalDirectory + filename;
	else
		fName = filename;

	bool fret = nsfileio::create_dir(fName);
	if (fret)
	{
		dprint("NSShaderManager::save Created directory " + fName);
	}

	nsfstream file;
	NSFilePUPer * p;
	if (mSaveMode == Binary)
	{
		file.open(fName, nsfstream::out | nsfstream::binary);
		p = new NSBinFilePUPer(file, PUP_OUT);
	}
	else
	{
		file.open(fName, nsfstream::out);
		p = new NSTextFilePUPer(file, PUP_OUT);
	}

	if (!file.is_open())
	{
		dprint("NSShaderManager::saveStage : Error opening stage " + sh->stagename(stagetype) + " file " + fName);
		delete p;
		return false;
	}

	nsstring guid_ = nsengine.guid(sh->type());
	if (mSaveMode == Binary)
		pup(*(static_cast<NSBinFilePUPer*>(p)), guid_ + "_stage:" + sh->stagename(stagetype), "type");
	else
		pup(*(static_cast<NSTextFilePUPer*>(p)), guid_ + "_stage:" + sh->stagename(stagetype), "type");

	sh->pup(p, stagetype);
	dprint("NSShaderManager::saveStage - Succesfully saved stage " + sh->stagename(stagetype) + " to file " + fName);
	delete p;
	file.close();
	return true;
}

bool NSShaderManager::link(NSShader * sh)
{
	return sh->link();
}

bool NSShaderManager::linkAll()
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
