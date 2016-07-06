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

#include <nsasset_manager.h>
#include <nsmesh.h>

nsmesh * get_mesh(const uivec2 & id);

struct aiScene;
struct aiNode;

class nsmesh_manager : public nsasset_manager
{
public:
	nsmesh_manager();
	~nsmesh_manager();

	template <class res_type>
	res_type * create(const nsstring & res_name, nsasset * to_copy=nullptr)
	{
		return nsasset_manager::create<res_type>(res_name, to_copy);
	}

	virtual nsmesh * create(const nsstring & res_name, nsasset * to_copy=nullptr)
	{
		return create<nsmesh>(res_name, to_copy);
	}

	template <class res_type, class T>
	res_type * get(const T & res_name)
	{
		return nsasset_manager::get<res_type>(res_name);
	}
	
	template<class T>
	nsmesh * get(const T & res_name)
	{
		return get<nsmesh>(res_name);
	}

	template<class res_type>
	res_type * load(const nsstring & fname, bool finalize_)
	{
		return nsasset_manager::load<res_type>(fname, finalize_);
	}

	nsmesh * load(const nsstring & fname, bool finalize_);

	nsmesh * load(uint32 res_type_id, const nsstring & fname, bool finalize);
	
	template<class res_type, class T >
	res_type * remove(const T & res_name)
	{
		return nsasset_manager::remove<res_type>(res_name);
	}

	template<class T >
	nsmesh * remove(const T & res_name)
	{
		return remove<nsmesh>(res_name);
	}

	nsmesh * assimp_load_mesh(const aiScene * scene, const nsstring & mesh_name);

	bool vid_update_on_load;

private:

	void _assimp_load_submeshes(nsmesh* pMesh, const aiScene * pScene);
	void _assimp_load_node(nsmesh* pMesh, nsmesh::node * pMeshNode, const aiNode * pNode, uint32 & node_id);
	void _assimp_load_node_heirarchy(nsmesh* pMesh, const aiNode * pRootNode);
};

#endif