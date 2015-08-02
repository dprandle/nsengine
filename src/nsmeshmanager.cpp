/*! 
	\file nsmeshmanager.cpp
	
	\brief Definitions file for NSMeshManager class

	This file contains all of the neccessary definitions for the NSMeshManager class.

	\author Daniel Randle
	\date November 23 2013
	\copywrite Earth Banana Games 2013
*/

#include <nsmeshmanager.h>
#include <assimp/scene.h>
#include <nsengine.h>

NSMeshManager::NSMeshManager(): NSResManager()
{
	setLocalDirectory(LOCAL_MESH_DIR_DEFAULT);
}

NSMeshManager::~NSMeshManager()
{}

NSMesh* NSMeshManager::assimpLoadMeshFromScene(const aiScene * scene, const nsstring & pMeshName)
{
	NSMesh * mesh = create(pMeshName);
	_assimpLoadSubMeshes(mesh, scene);
	_assimpLoadNodeHeirarchy(mesh,scene->mRootNode);
	mesh->calcaabb();
	return mesh;
}

void NSMeshManager::_assimpLoadNode(NSMesh* pMesh, NSMesh::Node * pMeshNode, const aiNode * node)
{
	aiMatrix4x4 trans = node->mTransformation;
	fmat4 lTransform( fvec4(trans.a1, trans.a2, trans.a3, trans.a4),
							fvec4(trans.b1, trans.b2, trans.b3, trans.b4),
							fvec4(trans.c1, trans.c2, trans.c3, trans.c4),
							fvec4(trans.d1, trans.d2, trans.d3, trans.d4));

	pMeshNode->mNodeTransform = lTransform;

	if (pMeshNode->mParentNode != NULL)
		pMeshNode->mWorldTransform = pMeshNode->mParentNode->mWorldTransform * pMeshNode->mNodeTransform;
	else
		pMeshNode->mWorldTransform = pMeshNode->mNodeTransform;


	// Assign each subMesh that refers to this node in assimp to this node in our NSMesh
	for ( nsuint j = 0; j < node->mNumMeshes; ++j)
	{
		NSMesh::SubMesh * subMesh = pMesh->submesh(node->mMeshes[j]);
		if (subMesh == NULL)
			continue;

		subMesh->mNode = pMeshNode;
		if (subMesh->mBoneInfo.empty())
		{
			NSMesh::SubMesh::BoneWeightIDs id;
			id.boneIDs[0] = subMesh->mNode->mNodeID;
			id.weights[0] = 1.0f;
			subMesh->mBoneInfo.assign(subMesh->mPositions.size(), id);
			subMesh->mBoneBuf.allocate(subMesh->mBoneInfo,
									   NSBufferObject::MutableStaticDraw,
									   static_cast<nsuint>(subMesh->mBoneInfo.size()));
		}
	}

	// For each child node go through and load the node struture recursively
	for ( nsuint i = 0; i < node->mNumChildren; ++i)
	{
		aiNode * childAINode = node->mChildren[i];
		NSMesh::Node * childNode = pMeshNode->createChild(childAINode->mName.C_Str());
		_assimpLoadNode(pMesh, childNode, childAINode);
	}
}

void NSMeshManager::_assimpLoadNodeHeirarchy(NSMesh* pMesh, const aiNode * pRootNode)
{
	NSMesh::NodeTree * meshNodeTree = pMesh->nodetree();
	NSMesh::Node * rootNode = meshNodeTree->createRootNode(pRootNode->mName.C_Str());
	_assimpLoadNode(pMesh, rootNode, pRootNode);
}

void NSMeshManager::_assimpLoadSubMeshes(NSMesh * pMesh, const aiScene * pScene)
{
	for (nsuint meshIndex = 0; meshIndex < pScene->mNumMeshes; ++meshIndex)
	{
		const aiMesh * currentSubMesh = pScene->mMeshes[meshIndex];
		bool generateTangents = false;

		if (currentSubMesh != NULL)
		{
			NSMesh::SubMesh * subMesh = pMesh->create();
			subMesh->mName = currentSubMesh->mName.C_Str();

			// Make sure the primitive type is correct
			if (currentSubMesh->mPrimitiveTypes == aiPrimitiveType_TRIANGLE)
				subMesh->mPrimType = GL_TRIANGLES;
			else if (currentSubMesh->mPrimitiveTypes == aiPrimitiveType_POINT)
				subMesh->mPrimType = GL_POINTS;
			else if (currentSubMesh->mPrimitiveTypes == aiPrimitiveType_LINE)
				subMesh->mPrimType = GL_LINES;
			else
				throw std::exception();

			// Load all of the vertices per vertex information in to the submesh buffers
			// Should have normals, tangents, bit-tangents, and positions always because of processing
			// steps - which is good for the shaders also
			subMesh->mPositions.resize(currentSubMesh->mNumVertices);
			subMesh->mNormals.resize(currentSubMesh->mNumVertices);
			subMesh->mTangents.resize(currentSubMesh->mNumVertices);
			subMesh->mTexCoords.resize(currentSubMesh->mNumVertices);
			subMesh->mHasTexCoords = currentSubMesh->HasTextureCoords(0);

			fvec3 maxNoNormals, maxNormals;
			fvec3 minNoNormals, minNormals;

			for (nsuint verticeIndex = 0; verticeIndex < currentSubMesh->mNumVertices; ++verticeIndex)
			{
				subMesh->mPositions[verticeIndex].x = currentSubMesh->mVertices[verticeIndex].x;
				subMesh->mPositions[verticeIndex].y = currentSubMesh->mVertices[verticeIndex].y;
				subMesh->mPositions[verticeIndex].z = currentSubMesh->mVertices[verticeIndex].z;

				if (currentSubMesh->HasNormals())
				{
					subMesh->mNormals[verticeIndex].x = currentSubMesh->mNormals[verticeIndex].x;
					subMesh->mNormals[verticeIndex].y = currentSubMesh->mNormals[verticeIndex].y;
					subMesh->mNormals[verticeIndex].z = currentSubMesh->mNormals[verticeIndex].z;
					subMesh->mNormals[verticeIndex].normalize();
				}

				if (currentSubMesh->HasTangentsAndBitangents())
				{
					subMesh->mTangents[verticeIndex].x = currentSubMesh->mTangents[verticeIndex].x;
					subMesh->mTangents[verticeIndex].y = currentSubMesh->mTangents[verticeIndex].y;
					subMesh->mTangents[verticeIndex].z = currentSubMesh->mTangents[verticeIndex].z;
					subMesh->mTangents[verticeIndex].normalize();
				}
				else
					generateTangents = true;

				// If there are tex coords stored in assimp then load them - otherwise just leave them as
				// the default constructed NSVec2D(0,0)
				if (subMesh->mHasTexCoords)
				{
					subMesh->mTexCoords[verticeIndex].u = currentSubMesh->mTextureCoords[0][verticeIndex].x;
					subMesh->mTexCoords[verticeIndex].v = currentSubMesh->mTextureCoords[0][verticeIndex].y;
					subMesh->mHasTexCoords = true;
				}
			}


			// Need to load bones no matter what - for shader purposes
			subMesh->mBoneInfo.resize(subMesh->mPositions.size());
			// load the bone indexes in to a bone index list that can be used
			// in the shader to get bone transformations
			if (currentSubMesh->HasBones())
			{
				for (nsuint k = 0; k < currentSubMesh->mNumBones; ++k)
				{
					const aiBone * currentBone = currentSubMesh->mBones[k];
					nsstring boneName = currentBone->mName.C_Str();
					nsuint boneIndex = 0;
					NSMesh::NodeTree * nodeTree = pMesh->nodetree();

					if (nodeTree->boneNameMap.find(boneName) == nodeTree->boneNameMap.end() )
					{
						boneIndex = static_cast<nsuint>(pMesh->nodetree()->boneNameMap.size());
						NSMesh::Bone boneInfo;
						const aiMatrix4x4 * mat = &currentBone->mOffsetMatrix;
						boneInfo.boneID = boneIndex;
						boneInfo.mOffsetTransform.set(
							mat->a1, mat->a2, mat->a3, mat->a4,
							mat->b1, mat->b2, mat->b3, mat->b4,
							mat->c1, mat->c2, mat->c3, mat->c4,
							mat->d1, mat->d2, mat->d3, mat->d4);
						nodeTree->boneNameMap[boneName] = boneInfo;
					}
					else
						boneIndex = nodeTree->boneNameMap[boneName].boneID;

					for (nsuint l = 0; l < currentSubMesh->mBones[k]->mNumWeights; ++l)
					{
						nsuint vertexIndex = currentBone->mWeights[l].mVertexId;
						nsfloat weight = currentBone->mWeights[l].mWeight;
						subMesh->mBoneInfo[vertexIndex].addBoneInfo(boneIndex, weight);
					}
				}
			}

			// Go through the submesh depending on which type of primitive, storing the indexes
			// This will allow to draw other types of primitives besides triangles (well, allows lines and points)
			if (subMesh->mPrimType == GL_TRIANGLES)
			{
				subMesh->mIndices.resize(currentSubMesh->mNumFaces * 3);
				subMesh->mTriangles.resize(currentSubMesh->mNumFaces);
				for (nsuint m = 0; m < currentSubMesh->mNumFaces; ++m)
				{
					const aiFace * currentFace = &currentSubMesh->mFaces[m];
					nsuint offset = m * 3;

					subMesh->mTriangles[m][0] = currentFace->mIndices[0];
					subMesh->mIndices[offset] = currentFace->mIndices[0];

					subMesh->mTriangles[m][1] = currentFace->mIndices[1];
					subMesh->mIndices[offset + 1] = currentFace->mIndices[1];

					subMesh->mTriangles[m][2] = currentFace->mIndices[2];
					subMesh->mIndices[offset + 2] = currentFace->mIndices[2];

					if (generateTangents)
					{
						fvec3 & posv0 = subMesh->mPositions[currentFace->mIndices[0]];
						fvec3 & posv1 = subMesh->mPositions[currentFace->mIndices[1]];
						fvec3 & posv2 = subMesh->mPositions[currentFace->mIndices[2]];

						fvec2 & texv0 = subMesh->mTexCoords[currentFace->mIndices[0]];
						fvec2 & texv1 = subMesh->mTexCoords[currentFace->mIndices[1]];
						fvec2 & texv2 = subMesh->mTexCoords[currentFace->mIndices[2]];

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

						for (nsuint i = 0; i < 3; ++i)
						{
							subMesh->mTangents[currentFace->mIndices[i]] = tangent;
							subMesh->mTangents[currentFace->mIndices[i]].normalize();
						}
					}
				}
			}
			else if (subMesh->mPrimType == GL_LINES)
			{
				subMesh->mIndices.resize(currentSubMesh->mNumFaces * 2);
				subMesh->mLines.resize(currentSubMesh->mNumFaces);
				for (nsuint m = 0; m < currentSubMesh->mNumFaces; ++m)
				{
					const aiFace * currentFace = &currentSubMesh->mFaces[m];
					nsuint offset = m * 2;

					subMesh->mLines[m][0] = currentFace->mIndices[0];
					subMesh->mIndices[offset] = currentFace->mIndices[0];

					subMesh->mLines[m][1] = currentFace->mIndices[1];
					subMesh->mIndices[offset + 1] = currentFace->mIndices[1];
				}
			}
			else if (subMesh->mPrimType == GL_POINTS)
			{
				subMesh->mIndices.resize(currentSubMesh->mNumFaces);
				for (nsuint m = 0; m < currentSubMesh->mNumFaces; ++m)
				{
					const aiFace * currentFace = &currentSubMesh->mFaces[m];
					subMesh->mIndices[m] = currentFace->mIndices[0];
				}
			}
			else
				throw std::exception();

			subMesh->allocateBuffers();
		}
	}
}
