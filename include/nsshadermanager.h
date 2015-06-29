/*! 
	\file nsshadermanager.h
	
	\brief Header file for NSShaderManager class

	This file contains all of the neccessary declarations for the NSShaderManager class.

	\author Daniel Randle
	\date November 23 2013
	\copywrite Earth Banana Games 2013
*/

#ifndef NSSHADERMANAGER_H
#define NSSHADERMANAGER_H

#include <nsresmanager.h>
#include <nsshader.h>

class NSShader;

class NSShaderManager : public NSResManager
{
public:

	NSShaderManager();
	~NSShaderManager();

	bool compile();

	template<class T>
	bool compile(const T & shader)
	{
		NSShader * sh = get(shader);
		if (sh == NULL)
			return false;
		return sh->compile();
	}

	template <class ResType>
	ResType * create(const nsstring & resName)
	{
		return NSResManager::create<ResType>(resName);
	}

	virtual NSShader * create(const nsstring & resName)
	{
		return NSResManager::create<NSShader>(resName);
	}

	virtual NSShader * create(const nsstring & resType, const nsstring & resName)
	{
		return static_cast<NSShader*>(NSResManager::create(resType, resName));
	}

	template <class ResType, class T>
	ResType * get(const T & rname)
	{
		return NSResManager::get<ResType>(rname);
	}

	virtual NSShader * get(nsuint resid)
	{
		return static_cast<NSShader*>(NSResManager::get(resid));
	}

	virtual NSShader * get(const nsstring & resName)
	{
		return static_cast<NSShader*>(NSResManager::get(resName));
	}

	void initUniforms();

	template<class T>
	void initUniforms(const T & shader)
	{
		NSShader * sh = get(shader);
		if (sh == NULL)
			return;
		sh->initUniforms();
	}

	bool link();

	template<class T>
	bool link(const T & shader)
	{
		NSShader * sh = get(shader);
		if (sh == NULL)
			return false;
		return sh->link();
	}

	template<class ResType, class T>
	ResType * loadStage(const T & shader, const nsstring & filename, NSShader::ShaderType stagetype, bool pAppendDirectories = true)
	{
		ResType * sh = get<ResType>(shader);
		if (sh == NULL)
			return NULL;

		nsstring fName(filename);
		if (pAppendDirectories)
			fName = mResourceDirectory + mLocalDirectory + filename;

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
			return NULL;
		}

		nsstring rt;
		if (mSaveMode == Binary)
			pup(*(static_cast<NSBinFilePUPer*>(p)), rt, "type");
		else
			pup(*(static_cast<NSTextFilePUPer*>(p)), rt, "type");

		if (rt != sh->typeString() + "stage:" + std::to_string(stagetype))
		{
			dprint("NSShaderManager::loadStage Attempted to load shader stage " + fName + " which is not of stage type " + std::to_string(stagetype));
			delete p;
			file.close();
			unload(sc);
			return false;
		}

		sh->pup(p, stagetype);
		dprint("NSShaderManager::loadStage - Succesfully loaded " + sh->typeString() + " stage " + std::to_string(stagetype) + " from file " + fName);
		delete p;
		file.close();
		return sh;
	}

	template<class T>
	NSShader * loadStage(const T & shader, const nsstring & filename, NSShader::ShaderType stagetype, bool pAppendDirectories = true)
	{
		return loadStage<NSShader>(shader, filename, stagetype, pAppendDirectories);
	}

	template<class ResType>
	ResType * load(const nsstring & pFileName, bool pAppendDirectories = true)
	{
		return NSResManager::load<ResType>(pFileName, pAppendDirectories);
	}

	virtual NSShader * load(const nsstring & resType, const nsstring & pFileName, bool pAppendDirectories = true)
	{
		return static_cast<NSShader*>(NSResManager::load(resType, pFileName, pAppendDirectories));
	}

	template<class ResType, class T >
	ResType * remove(const T & rname)
	{
		return NSResManager::remove<ResType>(rname);
	}

	virtual NSShader * remove(const nsstring & name)
	{
		return static_cast<NSShader*>(NSResManager::remove(name));
	}

	virtual NSShader * remove(nsuint id)
	{
		return static_cast<NSShader*>(NSResManager::remove(id));
	}

	virtual NSShader * remove(NSResource * res)
	{
		return static_cast<NSShader*>(NSResManager::remove(res));
	}

	template<class ResType, class T>
	ResType * saveStage(const T & shader, const nsstring & filename, NSShader::ShaderType stagetype, bool pAppendDirectories = true)
	{
		ResType * sh = get<ResType>(shader);
		if (sh == NULL)
			return NULL;

		nsstring fName(filename);
		if (pAppendDirectories)
			fName = mResourceDirectory + mLocalDirectory + filename;

		bool fret = create_dir(fName);
		if (fret)
			dprint("NSShaderManager::save Created directory " + fName);

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
			dprint("NSShaderManager::saveStage : Error opening " + sh->typeString " stage " + std::to_string(stagetype) + " file " + fName);
			delete p;
			return NULL;
		}

		if (mSaveMode == Binary)
			pup(*(static_cast<NSBinFilePUPer*>(p)), sh->typeString() + "stage:" + std::to_string(stagetype), "type");
		else
			pup(*(static_cast<NSTextFilePUPer*>(p)), sh->typeString() + "stage:" + std::to_string(stagetype), "type");

		sh->pup(p, stagetype);
		dprint("NSShaderManager::saveStage - Succesfully saved " + sh->typeString " stage " + std::to_string(stagetype) + " to file " + fName);
		delete p;
		file.close();
		return sh;
	}

	template<class T>
	NSShader * saveStage(const T & shader, const nsstring & filename, NSShader::ShaderType stagetype, bool pAppendDirectories = true)
	{
		return saveStage<NSShader>(shader, filename, stagetype, pAppendDirectories);
	}

	static nsstring getTypeString();

	virtual nsstring typeString() { return getTypeString(); }
};

#endif