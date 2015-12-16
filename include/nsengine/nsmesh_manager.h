/*! 
	\file nsmesh_manager.h
	
	\brief Header file for nsmesh_manager class

	This file contains all of the neccessary declarations for the nsmesh_manager class.

	\author Daniel Randle
	\date November 23 2013
	\copywrite Earth Banana Games 2013
*/

#ifndef NSMESH_MANAGER_H
#define NSMESH_MANAGER_H

#include <nsres_manager.h>
#include <nsmesh.h>

struct aiScene;
struct aiNode;

class nsmesh_manager : public nsres_manager
{
public:
	nsmesh_manager();
	~nsmesh_manager();

	template <class res_type>
	res_type * create(const nsstring & res_name, nsresource * to_copy=nullptr)
	{
		return nsres_manager::create<res_type>(res_name, to_copy);
	}

	virtual nsmesh * create(const nsstring & res_name, nsresource * to_copy=nullptr)
	{
		return create<nsmesh>(res_name, to_copy);
	}

	template <class res_type, class T>
	res_type * get(const T & res_name)
	{
		return nsres_manager::get<res_type>(res_name);
	}
	
	template<class T>
	nsmesh * get(const T & res_name)
	{
		return get<nsmesh>(res_name);
	}

	template<class res_type>
	res_type * load(const nsstring & fname)
	{
		return nsres_manager::load<res_type>(fname);
	}

	nsmesh * load(const nsstring & fname)
	{
		return load<nsmesh>(fname);
	}
	
	template<class res_type, class T >
	res_type * remove(const T & res_name)
	{
		return nsres_manager::remove<res_type>(res_name);
	}

	template<class T >
	nsmesh * remove(const T & res_name)
	{
		return remove<nsmesh>(res_name);
	}

	nsmesh * assimp_load_mesh(const aiScene * scene, const nsstring & mesh_name);

private:

	void _assimp_load_submeshes(nsmesh* pMesh, const aiScene * pScene);
	void _assimp_load_node(nsmesh* pMesh, nsmesh::node * pMeshNode, const aiNode * pNode, uint32 & node_id);
	void _assimp_load_node_heirarchy(nsmesh* pMesh, const aiNode * pRootNode);
};

#endif
