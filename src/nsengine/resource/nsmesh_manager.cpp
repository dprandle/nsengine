/*! 
	\file nsmeshmanager.cpp
	
	\brief Definitions file for nsmeshManager class

	This file contains all of the neccessary definitions for the nsmesh_manager class.

	\author Daniel Randle
	\date November 23 2013
	\copywrite Earth Banana Games 2013
*/

#include <nsmesh_manager.h>
#include <assimp/scene.h>
#include <nsengine.h>
#include <nsplugin_manager.h>

nsmesh * get_mesh(const uivec2 & id)
{
	nsplugin * plg = nsep.get(id.x);
	if (plg == nullptr)
		return nullptr;
	return plg->get<nsmesh>(id.y);
}

nsmesh_manager::nsmesh_manager(): nsres_manager()
{
	set_local_dir(LOCAL_MESH_DIR_DEFAULT);
}

nsmesh_manager::~nsmesh_manager()
{}

nsmesh* nsmesh_manager::assimp_load_mesh(const aiScene * scene, const nsstring & pMeshName)
{
	nsmesh * mesh = create(pMeshName);
	_assimp_load_submeshes(mesh, scene);
	_assimp_load_node_heirarchy(mesh,scene->mRootNode);
	mesh->calc_aabb();
	return mesh;
}

void nsmesh_manager::_assimp_load_node(nsmesh* pMesh, nsmesh::node * pMeshNode, const aiNode * node, uint32 & node_id)
{
	aiMatrix4x4 trans = node->mTransformation;
	fmat4 lTransform( fvec4(trans.a1, trans.a2, trans.a3, trans.a4),
							fvec4(trans.b1, trans.b2, trans.b3, trans.b4),
							fvec4(trans.c1, trans.c2, trans.c3, trans.c4),
							fvec4(trans.d1, trans.d2, trans.d3, trans.d4));

	pMeshNode->m_node_tform = lTransform;

	if (pMeshNode->m_parent_node != NULL)
		pMeshNode->m_world_tform = pMeshNode->m_parent_node->m_world_tform * pMeshNode->m_node_tform;
	else
		pMeshNode->m_world_tform = pMeshNode->m_node_tform;


	// Assign each subMesh that refers to this node in assimp to this node in our NSMesh
	for ( uint32 j = 0; j < node->mNumMeshes; ++j)
	{
		nsmesh::submesh * subMesh = pMesh->sub(node->mMeshes[j]);
		if (subMesh == NULL)
			continue;

		subMesh->m_node = pMeshNode;
		if (subMesh->m_joints.empty())
		{
			nsmesh::submesh::connected_joints id;
			id.m_joint_ids[0] = subMesh->m_node->m_node_id;
			id.m_weights[0] = 1.0f;
			subMesh->m_joints.assign(subMesh->m_verts.size(), id);
			subMesh->m_joint_buf.allocate(subMesh->m_joints,
										nsbuffer_object::mutable_static_draw,
									   static_cast<uint32>(subMesh->m_joints.size()));
		}
	}

	// For each child node go through and load the node struture recursively
	for ( uint32 i = 0; i < node->mNumChildren; ++i)
	{
		aiNode * childAINode = node->mChildren[i];
		nsmesh::node * childNode = pMeshNode->create_child(childAINode->mName.C_Str());
		childNode->m_node_id = node_id++;
		_assimp_load_node(pMesh, childNode, childAINode, node_id);
	}
}

void nsmesh_manager::_assimp_load_node_heirarchy(nsmesh* pMesh, const aiNode * pRootNode)
{
	nsmesh::node_tree * meshNodeTree = pMesh->tree();
	nsmesh::node * rootNode = meshNodeTree->create_root_node(pRootNode->mName.C_Str());

	uint32 node_id = 0;
	
	rootNode->m_node_id = node_id++;
	_assimp_load_node(pMesh, rootNode, pRootNode, node_id);
}

void nsmesh_manager::_assimp_load_submeshes(nsmesh * pMesh, const aiScene * pScene)
{
	for (uint32 meshIndex = 0; meshIndex < pScene->mNumMeshes; ++meshIndex)
	{
		const aiMesh * currentSubMesh = pScene->mMeshes[meshIndex];
		bool generateTangents = false;

		if (currentSubMesh != NULL)
		{
			nsmesh::submesh * subMesh = pMesh->create();
			subMesh->m_name = currentSubMesh->mName.C_Str();

			// Make sure the primitive type is correct
			if (currentSubMesh->mPrimitiveTypes == aiPrimitiveType_TRIANGLE)
				subMesh->m_prim_type = GL_TRIANGLES;
			else if (currentSubMesh->mPrimitiveTypes == aiPrimitiveType_POINT)
				subMesh->m_prim_type = GL_POINTS;
			else if (currentSubMesh->mPrimitiveTypes == aiPrimitiveType_LINE)
				subMesh->m_prim_type = GL_LINES;
			else
				throw std::exception();

			// Load all of the vertices per vertex information in to the submesh buffers
			// Should have normals, tangents, bit-tangents, and positions always because of processing
			// steps - which is good for the shaders also
			subMesh->m_verts.resize(currentSubMesh->mNumVertices);
			subMesh->m_normals.resize(currentSubMesh->mNumVertices);
			subMesh->m_tangents.resize(currentSubMesh->mNumVertices);
			subMesh->m_tex_coords.resize(currentSubMesh->mNumVertices);
			subMesh->m_has_tex_coords = currentSubMesh->HasTextureCoords(0);

			fvec3 maxNoNormals, maxNormals;
			fvec3 minNoNormals, minNormals;

			for (uint32 verticeIndex = 0; verticeIndex < currentSubMesh->mNumVertices; ++verticeIndex)
			{
				subMesh->m_verts[verticeIndex].x = currentSubMesh->mVertices[verticeIndex].x;
				subMesh->m_verts[verticeIndex].y = currentSubMesh->mVertices[verticeIndex].y;
				subMesh->m_verts[verticeIndex].z = currentSubMesh->mVertices[verticeIndex].z;

				if (currentSubMesh->HasNormals())
				{
					subMesh->m_normals[verticeIndex].x = currentSubMesh->mNormals[verticeIndex].x;
					subMesh->m_normals[verticeIndex].y = currentSubMesh->mNormals[verticeIndex].y;
					subMesh->m_normals[verticeIndex].z = currentSubMesh->mNormals[verticeIndex].z;
					subMesh->m_normals[verticeIndex].normalize();
				}

				if (currentSubMesh->HasTangentsAndBitangents())
				{
					subMesh->m_tangents[verticeIndex].x = currentSubMesh->mTangents[verticeIndex].x;
					subMesh->m_tangents[verticeIndex].y = currentSubMesh->mTangents[verticeIndex].y;
					subMesh->m_tangents[verticeIndex].z = currentSubMesh->mTangents[verticeIndex].z;
					subMesh->m_tangents[verticeIndex].normalize();
				}
				else
					generateTangents = true;

				// If there are tex coords stored in assimp then load them - otherwise just leave them as
				// the default constructed NSVec2D(0,0)
				if (subMesh->m_has_tex_coords)
				{
					subMesh->m_tex_coords[verticeIndex].u = currentSubMesh->mTextureCoords[0][verticeIndex].x;
					subMesh->m_tex_coords[verticeIndex].v = currentSubMesh->mTextureCoords[0][verticeIndex].y;
					subMesh->m_has_tex_coords = true;
				}
			}


			// Need to load bones no matter what - for shader purposes
			subMesh->m_joints.resize(subMesh->m_verts.size());
			// load the bone indexes in to a bone index list that can be used
			// in the shader to get bone transformations
			if (currentSubMesh->HasBones())
			{
				for (uint32 k = 0; k < currentSubMesh->mNumBones; ++k)
				{
					const aiBone * currentBone = currentSubMesh->mBones[k];
					nsstring boneName = currentBone->mName.C_Str();
					uint32 boneIndex = 0;
					nsmesh::node_tree * nodeTree = pMesh->tree();

					if (nodeTree->m_name_joint_map.find(boneName) == nodeTree->m_name_joint_map.end() )
					{
						boneIndex = static_cast<uint32>(pMesh->tree()->m_name_joint_map.size());
						nsmesh::joint boneInfo;
						const aiMatrix4x4 * mat = &currentBone->mOffsetMatrix;
						boneInfo.m_joint_id = boneIndex;
						boneInfo.m_offset_tform.set(
							mat->a1, mat->a2, mat->a3, mat->a4,
							mat->b1, mat->b2, mat->b3, mat->b4,
							mat->c1, mat->c2, mat->c3, mat->c4,
							mat->d1, mat->d2, mat->d3, mat->d4);
						nodeTree->m_name_joint_map[boneName] = boneInfo;
					}
					else
						boneIndex = nodeTree->m_name_joint_map[boneName].m_joint_id;

					for (uint32 l = 0; l < currentSubMesh->mBones[k]->mNumWeights; ++l)
					{
						uint32 vertexIndex = currentBone->mWeights[l].mVertexId;
						float weight = currentBone->mWeights[l].mWeight;
						subMesh->m_joints[vertexIndex].add_joint(boneIndex, weight);
					}
				}
			}

			// Go through the submesh depending on which type of primitive, storing the indexes
			// This will allow to draw other types of primitives besides triangles (well, allows lines and points)
			if (subMesh->m_prim_type == GL_TRIANGLES)
			{
				subMesh->m_indices.resize(currentSubMesh->mNumFaces * 3);
				subMesh->m_triangles.resize(currentSubMesh->mNumFaces);
				for (uint32 m = 0; m < currentSubMesh->mNumFaces; ++m)
				{
					const aiFace * currentFace = &currentSubMesh->mFaces[m];
					uint32 offset = m * 3;

					subMesh->m_triangles[m][0] = currentFace->mIndices[0];
					subMesh->m_indices[offset] = currentFace->mIndices[0];

					subMesh->m_triangles[m][1] = currentFace->mIndices[1];
					subMesh->m_indices[offset + 1] = currentFace->mIndices[1];

					subMesh->m_triangles[m][2] = currentFace->mIndices[2];
					subMesh->m_indices[offset + 2] = currentFace->mIndices[2];

					if (generateTangents)
					{
						fvec3 & posv0 = subMesh->m_verts[currentFace->mIndices[0]];
						fvec3 & posv1 = subMesh->m_verts[currentFace->mIndices[1]];
						fvec3 & posv2 = subMesh->m_verts[currentFace->mIndices[2]];

						fvec2 & texv0 = subMesh->m_tex_coords[currentFace->mIndices[0]];
						fvec2 & texv1 = subMesh->m_tex_coords[currentFace->mIndices[1]];
						fvec2 & texv2 = subMesh->m_tex_coords[currentFace->mIndices[2]];

						fvec3 edge1 = posv1 - posv0;
						fvec3 edge2 = posv2 - posv0;

						fvec2 delta1 = texv1 - texv0;
						fvec2 delta2 = texv2 - texv0;

						float f = 1.0f / (delta1.u * delta2.v - delta2.u * delta1.v);

						fvec3 tangent;

						tangent.set(
							f * (delta2.v * edge1.x - delta1.v * edge2.x),
							f * (delta2.v * edge1.y - delta1.v * edge2.y),
							f * (delta2.v * edge1.z - delta1.v * edge2.z)
							);

						for (uint32 i = 0; i < 3; ++i)
						{
							subMesh->m_tangents[currentFace->mIndices[i]] = tangent;
							subMesh->m_tangents[currentFace->mIndices[i]].normalize();
						}
					}
				}
			}
			else if (subMesh->m_prim_type == GL_LINES)
			{
				subMesh->m_indices.resize(currentSubMesh->mNumFaces * 2);
				subMesh->m_lines.resize(currentSubMesh->mNumFaces);
				for (uint32 m = 0; m < currentSubMesh->mNumFaces; ++m)
				{
					const aiFace * currentFace = &currentSubMesh->mFaces[m];
					uint32 offset = m * 2;

					subMesh->m_lines[m][0] = currentFace->mIndices[0];
					subMesh->m_indices[offset] = currentFace->mIndices[0];

					subMesh->m_lines[m][1] = currentFace->mIndices[1];
					subMesh->m_indices[offset + 1] = currentFace->mIndices[1];
				}
			}
			else if (subMesh->m_prim_type == GL_POINTS)
			{
				subMesh->m_indices.resize(currentSubMesh->mNumFaces);
				for (uint32 m = 0; m < currentSubMesh->mNumFaces; ++m)
				{
					const aiFace * currentFace = &currentSubMesh->mFaces[m];
					subMesh->m_indices[m] = currentFace->mIndices[0];
				}
			}
			else
				throw std::exception();

			subMesh->allocate_buffers();
		}
	}
}
