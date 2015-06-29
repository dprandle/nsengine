/*! 
	\file nsmeshmanager.h
	
	\brief Header file for NSMeshManager class

	This file contains all of the neccessary declarations for the NSMeshManager class.

	\author Daniel Randle
	\date November 23 2013
	\copywrite Earth Banana Games 2013
*/

#ifndef NSMESHMANAGER_H
#define NSMESHMANAGER_H

#include <nsmesh.h>
#include <nsresmanager.h>

struct aiScene;

class NSMeshManager : public NSResManager
{
public:
	NSMeshManager();
	~NSMeshManager();


	template <class ResType>
	ResType * create(const nsstring & resName)
	{
		return NSResManager::create<ResType>(resName);
	}

	virtual NSMesh * create(const nsstring & resName)
	{
		return NSResManager::create<NSMesh>(resName);
	}

	virtual NSMesh * create(const nsstring & resType, const nsstring & resName)
	{
		return static_cast<NSMesh*>(NSResManager::create(resType, resName));
	}

	template <class ResType, class T>
	ResType * get(const T & rname)
	{
		return NSResManager::get<ResType>(rname);
	}

	virtual NSMesh * get(nsuint resid)
	{
		return static_cast<NSMesh*>(NSResManager::get(resid));
	}

	virtual NSMesh * get(const nsstring & resName)
	{
		return static_cast<NSMesh*>(NSResManager::get(resName));
	}

	template<class ResType>
	ResType * load(const nsstring & pFileName, bool pAppendDirectories = true)
	{
		return NSResManager::load<ResType>(pFileName, pAppendDirectories);
	}

	virtual NSMesh * load(const nsstring & resType, const nsstring & pFileName, bool pAppendDirectories = true)
	{
		return static_cast<NSMesh*>(NSResManager::load(resType, pFileName, pAppendDirectories));
	}

	template<class ResType, class T >
	ResType * remove(const T & rname)
	{
		return NSResManager::remove<ResType>(rname);
	}

	virtual NSMesh * remove(const nsstring & name)
	{
		return static_cast<NSMesh*>(NSResManager::remove(name));
	}

	virtual NSMesh * remove(nsuint id)
	{
		return static_cast<NSMesh*>(NSResManager::remove(id));
	}

	virtual NSMesh * remove(NSResource * res)
	{
		return static_cast<NSMesh*>(NSResManager::remove(res));
	}

	NSMesh* assimpLoadMeshFromScene(const aiScene * scene, const nsstring & pMeshName);

	virtual nsstring typeString() { return getTypeString(); }

	static nsstring getTypeString();

private:

	void _assimpLoadSubMeshes(NSMesh* pMesh, const aiScene * pScene);
	void _assimpLoadNode(NSMesh* pMesh, NSMesh::Node * pMeshNode, const aiNode * pNode);
	void _assimpLoadNodeHeirarchy(NSMesh* pMesh, const aiNode * pRootNode);
};

#endif