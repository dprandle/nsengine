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

class NSShaderManager : public NSResManager
{
public:

	NSShaderManager();
	~NSShaderManager();

	template <class ResType>
	ResType * create(const nsstring & resName)
	{
		return NSResManager::create<ResType>(resName);
	}

	virtual NSShader * create(const nsstring & resName)
	{
		return create<NSShader>(resName); // Create 2d texture by default
	}

	template <class ResType, class T>
	ResType * get(const T & rname)
	{
		return NSResManager::get<ResType>(rname);
	}
	
	template<class T>
	NSShader * get(const T & resname)
	{
		return get<NSShader>(resname);
	}

	template<class ResType>
	ResType * load(const nsstring & fname)
	{
		return NSResManager::load<ResType>(fname);
	}

	NSShader * load(const nsstring & fname)
	{
		return load<NSShader>(fname);
	}
	
	template<class ResType, class T >
	ResType * remove(const T & rname)
	{
		return NSResManager::remove<ResType>(rname);
	}

	template<class T >
	NSShader * remove(const T & rname)
	{
		return remove<NSShader>(rname);
	}

	bool compileAll();

	template<class T>
	bool compile(const T & shader)
	{
		NSShader * sh = get(shader);
		return compile(sh);
	}

	bool compile(NSShader * sh);

	void initUniformsAll();

	template<class T>
	void initUniforms(const T & shader)
	{
		NSShader * sh = get(shader);
		initUniforms(sh);
	}

	void initUniforms(NSShader * sh);

	bool linkAll();

	template<class T>
	bool link(const T & shader)
	{
		NSShader * sh = get(shader);
		return link(sh);
	}

	bool link(NSShader * sh);
	
	template<class T>
	bool loadStage(const T & shader, const nsstring & filename, NSShader::ShaderType stagetype)
	{
		NSShader * sh = get(shader);
		return loadStage(shader, filename, stagetype, pAppendDirectories);
	}

	bool loadStage(NSShader * shader, const nsstring & fname, NSShader::ShaderType stagetype);

	template<class T>
	bool saveStage(const T & shader, const nsstring & filename, NSShader::ShaderType stagetype)
	{
		NSShader * sh = get(shader);
		return saveStage(sh, filename, stagetype, pAppendDirectories);
	}

	bool saveStage(NSShader * sh, const nsstring & filename, NSShader::ShaderType stagetype);
};

#endif
