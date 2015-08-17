/* ----------------------------- Noble Steed Engine----------------------------*
Authors : Daniel Randle, Alex Bourne
Date Created : Mar 7 2013

File:
	nsmesh.h

Description:
	This file contains the NSMesh class which can be used to load meshes in from file
	and draw them using their attached shader
*---------------------------------------------------------------------------*/

#ifndef NSMESH_H
#define NSMESH_H


#include <myGL/glew.h>
#include <nsglobal.h>
#include <vector>
#include <nsmaterial.h>
#include <nsmath.h>

/* ---------------------------------Class NSMesh-----------------------------*

Description:
	NSMesh will load a mesh from file along with any materials that it may use.
	File formates supported are defined by the file formats in Assimp library
	as that is the library this class uses to load meshes from file.

*-------------------------------------------------------------------------------*/

// includes
#include <nspupper.h>
#include <nsshader.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <nsresource.h>
#include <map>
#include <nsbuffer_object.h>
#include <nsvertex_array_object.h>

class NSMesh : public NSResource
{
public:
	template<class PUPer>
	friend void pup(PUPer & p, NSMesh & mesh);

	struct Bone
	{
		Bone();
		uint32 boneID;
		fmat4 mOffsetTransform;
	};

	struct Node
	{
		Node(const nsstring & pName="", Node * pParentNode=NULL);
		~Node();
		Node * createChild(const nsstring & pName);
		Node * findNode(const nsstring & pNodeName);

		nsstring mName;
		uint32 mNodeID;
		fmat4 mNodeTransform;
		fmat4 mWorldTransform;
		Node * mParentNode;
		std::vector<Node*> mChildNodes;
	};

	struct NodeTree
	{
		NodeTree();
		~NodeTree();
		Node * createRootNode(const nsstring & pName);
		Node * findNode(const nsstring & pNodeName);

		Node * mRootNode;
		std::map<nsstring,NSMesh::Bone> boneNameMap;
	};

	struct SubMesh
	{
		struct BoneWeightIDs
		{
			BoneWeightIDs();
			void addBoneInfo(uint32 pBoneID, float pWeight);

			uint32 boneIDs[BONES_PER_VERTEX];
			float weights[BONES_PER_VERTEX];
		};

		SubMesh(NSMesh * pParentMesh=NULL);
		~SubMesh();
		void allocateBuffers();
		void calcaabb();
		void initGL();
		void resize(uint32 pNewSize);
		void updateVAO();


		NSBufferObject mPosBuf;
		NSBufferObject mTexBuf;
		NSBufferObject mNormBuf;
		NSBufferObject mTangBuf;
		NSBufferObject mIndiceBuf;
		NSBufferObject mBoneBuf;
		NSVertexArrayObject mVAO;

		fvec3array mPositions;
		fvec2array mTexCoords;
		fvec3array mNormals;
		fvec3array mTangents;
		uint32array mIndices;
		uivec3array mTriangles;
		uivec2array mLines;
		std::vector<BoneWeightIDs> mBoneInfo;

		GLenum mPrimType;
		Node * mNode;
		nsstring mName;
		NSMesh * mParentMesh;
		NSBoundingBox mSubBoundingBox;
		bool mHasTexCoords;
	};

	typedef std::vector<SubMesh*>::iterator SubMeshIter;

	NSMesh();
	~NSMesh();

	const NSBoundingBox & aabb();

	bool add(SubMesh * submesh);

	void allocate();

	void allocate(uint32 subindex);

	void bake();

	void bakeRotation(const fquat & rotation);

	void bakeScaling(const fvec3 & scale);

	void bakeTranslation(const fvec3 & tranlation);

	void bakeRotation(uint32 subindex, const fquat & rotation);

	void bakeScaling(uint32 subindex, const fvec3 & scale);

	void bakeTranslation(uint32 subindex, const fvec3 & tranlation);

	void bakeNodeRotation(const fquat & rotation);

	void bakeNodeScaling(const fvec3 & scale);

	void bakeNodeTranslation(const fvec3 & tranlation);

	void calcaabb();

	uint32 count();

	bool contains(SubMesh * submesh);

	bool contains(uint32 subindex);

	bool contains(const nsstring & subname);
	
	SubMesh * create();

	bool del();

	bool del(SubMesh * submesh);

	bool del(uint32 subindex);

	bool del(const nsstring & subname);

	uint32 find(SubMesh * sub);

	uint32 find(const nsstring & subname);

	void flipnorm();

	void flipnorm(uint32 subindex);

	void flipuv();

	void flipuv(uint32 pSubIndex);

	void initGL();

	void initGL(uint32 subindex);

	SubMesh * submesh(const nsstring & pName);

	SubMesh * submesh(uint32 pIndex);

	void init();

	NodeTree * nodetree();

	virtual void pup(NSFilePUPer * p);

	SubMesh * remove(SubMesh * submesh);

	SubMesh * remove(uint32 subindex);

	SubMesh * remove(const nsstring & subname);

	void transformNode(Node * pParentNode, const fmat4 & pTransform);

	uint32 vertcount();

	uint32 vertcount(uint32 pIndex);

	/*!
	Calculate volume using divergence theorem method
	*/
	float volume();

private:
	void _propagateWorldTransform(Node * pChildNode);
	std::vector<SubMesh*> subMeshes;
	NodeTree* mNodeTree;
	NSBoundingBox mBoundingBox;
};

// This is a special structure to hold a set of bone ids that are each weighted to show how much the bone
// affects the vertex - I have a BONES_PER_VERTEX limit set to 4 bones - I dont think I would ever need
// more than 4 bones affecting a single vertex in any sort of animation
template <class PUPer>
void pup(PUPer & p, NSMesh::SubMesh::BoneWeightIDs & bwid, const nsstring & varName)
{
	for (uint32 i = 0; i < BONES_PER_VERTEX; ++i)
	{
		pup(p, bwid.boneIDs[i], varName + ".boneID[" + std::to_string(i) + "]");
		pup(p, bwid.weights[i], varName + ".weights[" + std::to_string(i) + "]");
	}
}

// This simply pups the submesh data - not much to say here - most of these attributes are std::vectors of vec3 or vec2 or uint32s
// The prim type is just GL_TRIANGLES or GL_LINES etc - and the BoneInfo is a std::vector of BoneWeightIDs
template <class PUPer>
void pup(PUPer & p, NSMesh::SubMesh & sm, const nsstring & varName)
{
	pup(p, sm.mPositions, varName + ".mPositions");
	pup(p, sm.mTexCoords, varName + ".mTexCoords");
	pup(p, sm.mNormals, varName + ".mNormals");
	pup(p, sm.mTangents, varName + ".mTangents");
	pup(p, sm.mIndices, varName + ".mIndices");
	pup(p, sm.mTriangles, varName + ".mTriangles");
	pup(p, sm.mLines, varName + ".mLines");
	pup(p, sm.mBoneInfo, varName + ".mBoneInfo");
	pup(p, sm.mPrimType, varName + ".mPrimType");
	pup(p, sm.mHasTexCoords, varName + ".mHasTexCoords");
	pup(p, sm.mName, varName + ".mName");
}

// This handles a pointer to a submesh - if the pupper is PUP_IN (reading) then allocate memory for the submesh
// and then pup what it points to
template <class PUPer>
void pup(PUPer & p, NSMesh::SubMesh * & sm, const nsstring & varName)
{
	if (p.mode() == PUP_IN)
		sm = new NSMesh::SubMesh();
		
	pup(p, *sm, varName);
}

// Pup a bone - a bone just has an id and an offset transform to mess with the vertices its connected to
template <class PUPer>
void pup(PUPer & p, NSMesh::Bone & bone, const nsstring & varName)
{
	pup(p, bone.boneID, varName + ".boneID");
	pup(p, bone.mOffsetTransform, varName + ".mOffsetTransform");
}

// Pup a node and all of its child nodes
template <class PUPer>
void pup(PUPer & p, NSMesh::Node & node, const nsstring & varName)
{
	pup(p, node.mName, varName + ".mName");
	pup(p, node.mNodeID, varName + ".mNodeID");
	pup(p, node.mNodeTransform, varName + ".mNodeTransform");
	pup(p, node.mWorldTransform, varName + ".mWorldTransform");
	pup(p, node.mChildNodes, varName + ".mChildNodes");
	for (uint32 i = 0; i < node.mChildNodes.size(); ++i)
		node.mChildNodes[i]->mParentNode = &node;
}

// Allocate memory for a node if the pupper is set to PUP_IN
template <class PUPer>
void pup(PUPer & p, NSMesh::Node * & nodePtr, const nsstring & varName)
{
	if (p.mode() == PUP_IN)
		nodePtr = new NSMesh::Node();
	pup(p, *nodePtr, varName);
}

// Pup a node tree - if a mesh has a root node (meaning it has submeshes with transforms relative to the base node as each submesh is given a node)
// then pup the root node - the above pupper handles allocating the memory
template <class PUPer>
void pup(PUPer & p, NSMesh::NodeTree & nodeTree, const nsstring & varName)
{
	pup(p, nodeTree.boneNameMap, varName + ".boneNameMap");
	bool hasRoot = (nodeTree.mRootNode != NULL);
	pup(p, hasRoot, varName + ".hasRoot"); // hasRoot will be saved or overwritten depending on saving or loading
	if (hasRoot)
		pup(p, nodeTree.mRootNode, varName);
}

// Pup a mesh - pup the node tree and all of the submeshes - then go through the submeshes
// and assign the "parent" pointer (which should be the owning mesh) and assign the correct node
// if the mesh has a node structure (simple meshes wont)
// Also assign a debug object to it if debug mode is enabled - the debug object logs stuff and sends stuff to screen displays etc
template <class PUPer>
void pup(PUPer & p, NSMesh & mesh)
{
	pup(p, *mesh.mNodeTree, "nodeTree");
	pup(p, mesh.subMeshes, "subMeshes");
	for (uint32 i = 0; i < mesh.subMeshes.size(); ++i)
	{
		nsstring nodeName = "";
		if (mesh.subMeshes[i]->mNode != NULL)
			nodeName = mesh.subMeshes[i]->mNode->mName;
		pup(p, nodeName, "nodeName");
		mesh.subMeshes[i]->mNode = mesh.mNodeTree->findNode(nodeName);
		mesh.subMeshes[i]->mParentMesh = &mesh;
	}
	mesh.calcaabb();
	mesh.initGL();
	mesh.allocate();
}

#endif
