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

#include <asset/nsasset_manager.h>
#include <asset/nsmesh.h>

nsmesh * get_mesh(const uivec2 & id);

struct aiScene;
struct aiNode;

class nsmesh_manager : public nsasset_manager
{
public:

	MANAGER_TEMPLATES
	
	nsmesh_manager();
	~nsmesh_manager();

	nsmesh * load(uint32 res_type_id, const nsstring & fname, bool finalize);
	
	nsmesh * assimp_load_mesh(const aiScene * scene, const nsstring & mesh_name);

	bool vid_update_on_load;

private:

	void _assimp_load_submeshes(nsmesh* pMesh, const aiScene * pScene);
	void _assimp_load_node(nsmesh* pMesh, nsmesh::node * pMeshNode, const aiNode * pNode, uint32 & node_id);
	void _assimp_load_node_heirarchy(nsmesh* pMesh, const aiNode * pRootNode);
};

#endif
