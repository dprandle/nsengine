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

#include <nsresmanager.h>
#include <nsmesh.h>

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
		return create<NSMesh>(resName); // Create 2d texture by default
	}

	template <class ResType, class T>
	ResType * get(const T & rname)
	{
		return NSResManager::get<ResType>(rname);
	}
	
	template<class T>
	NSMesh * get(const T & resname)
	{
		return get<NSMesh>(resname);
	}

	template<class ResType>
	ResType * load(const nsstring & fname)
	{
		return NSResManager::load<ResType>(fname);
	}

	NSMesh * load(const nsstring & fname)
	{
		return load<NSMesh>(fname);
	}
	
	template<class ResType, class T >
	ResType * remove(const T & rname)
	{
		return NSResManager::remove<ResType>(rname);
	}

	template<class T >
	NSMesh * remove(const T & rname)
	{
		return remove<NSMesh>(rname);
	}

	NSMesh* assimpLoadMeshFromScene(const aiScene * scene, const nsstring & pMeshName);

private:

	void _assimpLoadSubMeshes(NSMesh* pMesh, const aiScene * pScene);
	void _assimpLoadNode(NSMesh* pMesh, NSMesh::Node * pMeshNode, const aiNode * pNode);
	void _assimpLoadNodeHeirarchy(NSMesh* pMesh, const aiNode * pRootNode);
};

#endif
