///*--------------------------------------------- Noble Steed Engine--------------------------------------*
//Authors : Daniel Randle, Alex Bourne
//Date Created : Mar 6 2013
//
//File:
//	nsmesh.cpp
//
//Description:
//	This file contains the definition for the NSMesh class functions along with any other helper
//	functions that will aid in loading/using meshes
//*-----------------------------------------------------------------------------------------------
#include <nsmesh.h>
#include <nsengine.h>

NSMesh::NSMesh(): mNodeTree(new NodeTree())
{
	setExtension(DEFAULT_MESH_EXTENSION);
}

NSMesh::~NSMesh()
{
	while (subMeshes.begin() != subMeshes.end())
	{
		delete subMeshes.back();
		subMeshes.pop_back();
	}
	delete mNodeTree;
}

void NSMesh::allocate()
{
	for (nsuint i = 0; i < count(); ++i)
		allocate(i);
}

void NSMesh::allocate(nsuint subindex)
{
	submesh(subindex)->allocateBuffers();
}

void NSMesh::bakeRotation(nsuint subindex, const fquat & pRot)
{
	SubMesh * sub = submesh(subindex);
	if (sub == NULL)
		return;
	for (nsuint i = 0; i < sub->mPositions.size(); ++i)
	{
		sub->mPositions[i] = rotationMat3(pRot) * sub->mPositions[i];
		if (sub->mNormals.size() == sub->mPositions.size())
			sub->mNormals[i] = rotationMat3(pRot) * sub->mNormals[i];
		if (sub->mTangents.size() == sub->mPositions.size())
			sub->mTangents[i] = rotationMat3(pRot) * sub->mTangents[i];
	}
	sub->allocateBuffers();
	calcaabb();
}

void NSMesh::bakeScaling(nsuint subindex, const fvec3 & pScaling)
{
	SubMesh * sub = submesh(subindex);
	if (sub == NULL)
		return;
	for (nsuint i = 0; i < sub->mPositions.size(); ++i)
	{
		sub->mPositions[i] %= pScaling;
		if (sub->mNormals.size() == sub->mPositions.size())
			sub->mNormals[i] %= pScaling;
		if (sub->mTangents.size() == sub->mPositions.size())
			sub->mTangents[i] %= pScaling;
	}
	sub->allocateBuffers();
	calcaabb();
}

void NSMesh::bakeTranslation(nsuint subindex, const fvec3 & pTrans)
{
	SubMesh * sub = submesh(subindex);
	if (sub == NULL)
		return;
	for (nsuint i = 0; i < sub->mPositions.size(); ++i)
	{
		sub->mPositions[i] += pTrans;
		if (sub->mNormals.size() == sub->mPositions.size())
			sub->mNormals[i] += pTrans;
		if (sub->mTangents.size() == sub->mPositions.size())
			sub->mTangents[i] += pTrans;
	}
	sub->allocateBuffers();
	calcaabb();
}

void NSMesh::bake()
{
	for (nsuint subindex = 0; subindex < count(); ++subindex)
	{
		SubMesh * sub = submesh(subindex);
		if (sub == NULL || sub->mNode == NULL)
			return;
		for (nsuint i = 0; i < sub->mPositions.size(); ++i)
		{
			sub->mPositions[i] = (sub->mNode->mWorldTransform * sub->mPositions[i]).xyz();
			if (sub->mNormals.size() == sub->mPositions.size())
				sub->mNormals[i] = (sub->mNode->mWorldTransform * sub->mNormals[i]).xyz();
			if (sub->mTangents.size() == sub->mPositions.size())
				sub->mTangents[i] = (sub->mNode->mWorldTransform * sub->mTangents[i]).xyz();
		}
		sub->mNode = NULL;
		sub->allocateBuffers();
	}
}

void NSMesh::bakeRotation(const fquat & pRot)
{
	for (nsuint subindex = 0; subindex < count(); ++subindex)
	{
		SubMesh * sub = submesh(subindex);
		if (sub == NULL)
			return;
		for (nsuint i = 0; i < sub->mPositions.size(); ++i)
		{
			sub->mPositions[i] = rotationMat3(pRot) * sub->mPositions[i];
			if (sub->mNormals.size() == sub->mPositions.size())
				sub->mNormals[i] = rotationMat3(pRot) * sub->mNormals[i];
			if (sub->mTangents.size() == sub->mPositions.size())
				sub->mTangents[i] = rotationMat3(pRot) * sub->mTangents[i];
		}
		sub->allocateBuffers();
	}
	calcaabb();
}

void NSMesh::bakeScaling(const fvec3 & pScaling)
{
	for (nsuint subindex = 0; subindex < count(); ++subindex)
	{
		SubMesh * sub = submesh(subindex);
		if (sub == NULL)
			return;
		for (nsuint i = 0; i < sub->mPositions.size(); ++i)
		{
			sub->mPositions[i] %= pScaling;
			if (sub->mNormals.size() == sub->mPositions.size())
				sub->mNormals[i] %= pScaling;
			if (sub->mTangents.size() == sub->mPositions.size())
				sub->mTangents[i] %= pScaling;
		}
		sub->allocateBuffers();
	}
	calcaabb();
}

void NSMesh::bakeTranslation(const fvec3 & pTrans)
{
	for (nsuint subindex = 0; subindex < count(); ++subindex)
	{
		SubMesh * sub = submesh(subindex);
		if (sub == NULL)
			return;
		for (nsuint i = 0; i < sub->mPositions.size(); ++i)
		{
			sub->mPositions[i] += pTrans;
			if (sub->mNormals.size() == sub->mPositions.size())
				sub->mNormals[i] += pTrans;
			if (sub->mTangents.size() == sub->mPositions.size())
				sub->mTangents[i] += pTrans;
		}
		sub->allocateBuffers();
	}
	calcaabb();
}

void NSMesh::bakeNodeRotation(const fquat & pRot)
{
	transformNode(mNodeTree->mRootNode, fmat4(rotationMat3(pRot)));
	calcaabb();
}

void NSMesh::bakeNodeScaling(const fvec3 & pScaling)
{
	transformNode(mNodeTree->mRootNode, scalingMat4(pScaling));
	calcaabb();
}

void NSMesh::bakeNodeTranslation(const fvec3 & pTrans)
{
	transformNode(mNodeTree->mRootNode, translationMat4(pTrans));
	calcaabb();
}

void NSMesh::calcaabb()
{
	mBoundingBox.clear();
	auto iter = subMeshes.begin();
	while (iter != subMeshes.end())
	{
		fmat4 transform;
		if ((*iter)->mNode != NULL)
			transform = (*iter)->mNode->mWorldTransform;
		(*iter)->calcaabb();
		mBoundingBox.extend((*iter)->mPositions, transform);
		++iter;
	}
}

void NSMesh::flipuv()
{
	for (nsuint i = 0; i < subMeshes.size(); ++i)
		flipuv(i);
}

void NSMesh::flipuv(nsuint pSubIndex)
{
	if (pSubIndex >= subMeshes.size())
	{
		dprint("NSMesh::flipSubUVs Trying to get subMesh with out of range index");
		return;
	}
	SubMesh * sub = submesh(pSubIndex);
	for (nsuint i = 0; i < sub->mTexCoords.size(); ++i)
		sub->mTexCoords[i].v = 1 - sub->mTexCoords[i].v;

	sub->mTexBuf.bind();
	sub->mTexBuf.setData(sub->mTexCoords,
						 0,
						 static_cast<nsuint>(sub->mTexCoords.size()));
	sub->mTexBuf.unbind();
}

void NSMesh::pup(NSFilePUPer * p)
{
	if (p->type() == NSFilePUPer::Binary)
	{
		NSBinFilePUPer * bf = static_cast<NSBinFilePUPer *>(p);
		::pup(*bf, *this);
	}
	else
	{
		NSTextFilePUPer * tf = static_cast<NSTextFilePUPer *>(p);
		::pup(*tf, *this);
	}
}

bool NSMesh::contains(SubMesh * submesh)
{
	for (nsuint i = 0; i < subMeshes.size(); ++i)
	{
		if (subMeshes[i] == submesh)
			return true;
	}
	return false;
}

bool NSMesh::contains(nsuint subindex)
{
	return (submesh(subindex) != NULL);
}

bool NSMesh::contains(const nsstring & subname)
{
	return (submesh(subname) != NULL);
}

bool NSMesh::del()
{
	bool ret = false;
	while (subMeshes.begin() != subMeshes.end())
	{
		delete subMeshes.back();
		subMeshes.pop_back();
		ret = true;
	}
	delete mNodeTree;
	mNodeTree = new NodeTree();
	return ret;
}

bool NSMesh::add(SubMesh * submesh)
{
	bool ret = !contains(submesh);
	if (ret)
		subMeshes.push_back(submesh);
	return ret;
}

NSMesh::SubMesh * NSMesh::create()
{
	SubMesh * subMesh = new SubMesh(this);
	subMesh->initGL();
	if (!add(subMesh))
	{
		delete subMesh;
		return NULL;
	}
	return subMesh;
}

void NSMesh::flipnorm()
{
	for (nsuint i = 0; i < subMeshes.size(); ++i)
		flipnorm(i);
}

void NSMesh::flipnorm(nsuint pSubIndex)
{
	if (pSubIndex >= subMeshes.size())
	{
		dprint("NSMesh::flipSubNormals Trying to get subMesh with out of range index");
		return;
	}

	SubMesh * sub = submesh(pSubIndex);
	for (nsuint i = 0; i < sub->mNormals.size(); ++i)
	{
		sub->mNormals[i] *= -1.0f;
		sub->mTangents[i] *= -1.0f;
	}

	sub->mNormBuf.bind();
	sub->mNormBuf.setData(sub->mNormals,
						  0,
						  static_cast<nsuint>(sub->mNormals.size()));
	sub->mTangBuf.bind();
	sub->mTangBuf.setData(sub->mTangents,
						  0,
						  static_cast<nsuint>(sub->mTangents.size()));
	sub->mNormBuf.unbind();
}

void NSMesh::initGL()
{
	for (nsuint i = 0; i < count(); ++i)
		initGL(i);
}

void NSMesh::initGL(nsuint subindex)
{
	submesh(subindex)->initGL();
}

const NSBoundingBox & NSMesh::aabb()
{
	return mBoundingBox;
}

NSMesh::SubMesh * NSMesh::submesh(const nsstring & pName)
{
	SubMeshIter iter = subMeshes.begin();
	while (iter != subMeshes.end())
	{
		if ((*iter)->mName == pName)
			return *iter;
		++iter;
	}
	return NULL;
}

NSMesh::SubMesh * NSMesh::submesh(nsuint pIndex)
{
	if (pIndex >= subMeshes.size())
		return NULL;

	return subMeshes[pIndex];
}

void NSMesh::init()
{
}

nsfloat NSMesh::volume()
{
	float K1 = 1.0f;
	float K2 = 1.0f;
	float K3 = 1.0f;

	float mult = 1 / (2 * (K1 + K2 + K3));

	float sum = 0.0f;

	for (nsuint subI = 0; subI < subMeshes.size(); ++subI)
	{
		NSMesh::SubMesh * sub = submesh(subI);
		for (nsuint triI = 0; triI < sub->mTriangles.size(); ++triI)
		{
			fvec3 Ai = sub->mPositions[sub->mTriangles[triI][0]];
			fvec3 Bi = sub->mPositions[sub->mTriangles[triI][1]];
			fvec3 Ci = sub->mPositions[sub->mTriangles[triI][2]];
			fvec3 Fr = (Ai + Bi + Ci) / 3.0f;
			Fr.x *= K1; Fr.y *= K2; Fr.z *= K3;

			fvec3 AtoB = Bi - Ai;
			fvec3 AtoC = Ci - Ai;
			fvec3 crs = AtoB.cross(AtoC);

			sum += Fr * crs;
		}
	}
	sum *= mult;
	return sum;
}

nsuint NSMesh::count()
{
	return static_cast<nsuint>(subMeshes.size());
}

NSMesh::NodeTree * NSMesh::nodetree()
{
	return mNodeTree;
}

nsbool NSMesh::del(SubMesh * submesh)
{
	SubMesh * sub = remove(submesh);
	if (sub == NULL)
		return false;
	delete sub;
	return true;
}

nsuint NSMesh::find(SubMesh * sub)
{
	for (nsuint i = 0; i < subMeshes.size(); ++i)
	{
		if (subMeshes[i] == sub)
			return i;
	}
	return -1;
}

nsuint NSMesh::find(const nsstring & subname)
{
	return find(submesh(subname));
}

nsbool NSMesh::del(nsuint pIndex)
{
	return del(submesh(pIndex));
}

nsbool NSMesh::del(const nsstring & subname)
{
	return del(submesh(subname));
}

NSMesh::SubMesh * NSMesh::remove(SubMesh * sub)
{
	nsuint cnt = find(sub);
	if (cnt == -1)
		return NULL;
	auto iter = subMeshes.begin() + cnt;
	subMeshes.erase(iter);
	return sub;
}

NSMesh::SubMesh * NSMesh::remove(nsuint index)
{
	return remove(submesh(index));
}

NSMesh::SubMesh * NSMesh::remove(const nsstring & name)
{
	return remove(submesh(name));
}

nsuint NSMesh::vertcount()
{
	nsuint total = 0;
	for (nsuint i = 0; i < subMeshes.size(); ++i)
		total += static_cast<nsuint>(subMeshes[i]->mIndices.size());
	return total;
}

nsuint NSMesh::vertcount(nsuint pIndex)
{
	return static_cast<nsuint>(subMeshes[pIndex]->mIndices.size());
}

void NSMesh::transformNode(Node * pNode, const fmat4 & pTransform)
{
	pNode->mNodeTransform = pTransform * pNode->mNodeTransform;

	if (pNode->mParentNode != NULL)
		pNode->mWorldTransform = pNode->mParentNode->mWorldTransform * pNode->mNodeTransform;
	else
		pNode->mWorldTransform = pNode->mNodeTransform;

	for (nsuint i = 0; i < pNode->mChildNodes.size(); ++i)
		_propagateWorldTransform(pNode->mChildNodes[i]);
}

void NSMesh::_propagateWorldTransform(Node * pChildNode)
{
	if (pChildNode->mParentNode != NULL)
		pChildNode->mWorldTransform = pChildNode->mParentNode->mWorldTransform * pChildNode->mNodeTransform;
	else
		pChildNode->mWorldTransform = pChildNode->mNodeTransform;

	for (nsuint i = 0; i < pChildNode->mChildNodes.size(); ++i)
		_propagateWorldTransform(pChildNode->mChildNodes[i]);
}

NSMesh::SubMesh::BoneWeightIDs::BoneWeightIDs()
{
	for (int i = 0; i < BONES_PER_VERTEX; ++i)
	{
		boneIDs[i] = 0;
		weights[i] = 0.0f;
	}
}

void NSMesh::SubMesh::BoneWeightIDs::addBoneInfo(nsuint pBoneID, nsfloat pWeight)
{
	for (int i = 0; i < BONES_PER_VERTEX; ++i)
	{
		if (weights[i] == 0.0f)
		{
			boneIDs[i] = pBoneID;
			weights[i] = pWeight;
			return;
		}
	}
}

NSMesh::SubMesh::SubMesh(NSMesh * pParentMesh): 
	mPosBuf(NSBufferObject::Array,NSBufferObject::Mutable),
	mTexBuf(NSBufferObject::Array,NSBufferObject::Mutable),
	mNormBuf(NSBufferObject::Array,NSBufferObject::Mutable),
	mTangBuf(NSBufferObject::Array,NSBufferObject::Mutable),
	mBoneBuf(NSBufferObject::Array,NSBufferObject::Mutable),
	mIndiceBuf(NSBufferObject::ElementArray,NSBufferObject::Mutable),
	mPositions(),
	mTexCoords(),
	mNormals(),
	mTangents(),
	mBoneInfo(),
	mIndices(),
	mNode(NULL),
	mName(),
	mParentMesh(pParentMesh),
	mHasTexCoords(false),
	mSubBoundingBox()
{}

NSMesh::SubMesh::~SubMesh()
{
	mPosBuf.release();
	mTexBuf.release();
	mNormBuf.release();
	mTangBuf.release();
	mBoneBuf.release();
	mIndiceBuf.release();
	mVAO.release();
}

void NSMesh::SubMesh::allocateBuffers()
{
	mPosBuf.bind();
	mPosBuf.allocate(mPositions,
					 NSBufferObject::MutableStaticDraw,
					 static_cast<nsuint>(mPositions.size()));
	mTexBuf.bind();
	mTexBuf.allocate(mTexCoords,
					 NSBufferObject::MutableStaticDraw,
					 static_cast<nsuint>(mTexCoords.size()));
	mNormBuf.bind();
	mNormBuf.allocate(mNormals,
					  NSBufferObject::MutableStaticDraw,
					  static_cast<nsuint>(mNormals.size()));
	mTangBuf.bind();
	mTangBuf.allocate(mTangents,
					  NSBufferObject::MutableStaticDraw,
					  static_cast<nsuint>(mTangents.size()));
	mBoneBuf.bind();
	mBoneBuf.allocate(mBoneInfo,
					  NSBufferObject::MutableStaticDraw,
					  static_cast<nsuint>(mBoneInfo.size()));
	mIndiceBuf.bind();
	mIndiceBuf.allocate(mIndices,
						NSBufferObject::MutableStaticDraw,
						static_cast<nsuint>(mIndices.size()));
	updateVAO();
}

void NSMesh::SubMesh::initGL()
{
	mPosBuf.initGL();
	mTexBuf.initGL();
	mNormBuf.initGL();
	mTangBuf.initGL();
	mBoneBuf.initGL();
	mIndiceBuf.initGL();
	mVAO.initGL();
}

void NSMesh::SubMesh::calcaabb()
{
	if (mNode == NULL)
		return;
	mSubBoundingBox.calculate(mPositions,mNode->mWorldTransform);
}

void NSMesh::SubMesh::resize(nsuint pNewSize)
{
	mPositions.resize(pNewSize);
	mTexCoords.resize(pNewSize);
	mNormals.resize(pNewSize);
	mTangents.resize(pNewSize);
	mIndices.resize(pNewSize);
}

void NSMesh::SubMesh::updateVAO()
{
	mVAO.bind();

	mPosBuf.bind();
	mVAO.add(&mPosBuf, NSShader::Position);
	mVAO.vertexAttribPtr(NSShader::Position, 3, GL_FLOAT, GL_FALSE, sizeof(fvec3), 0);

	mTexBuf.bind();
	mVAO.add(&mTexBuf, NSShader::TexCoords);
	mVAO.vertexAttribPtr(NSShader::TexCoords, 2, GL_FLOAT, GL_FALSE, sizeof(fvec2), 0);

	mNormBuf.bind();
	mVAO.add(&mNormBuf, NSShader::Normal);
	mVAO.vertexAttribPtr(NSShader::Normal, 3, GL_FLOAT, GL_FALSE, sizeof(fvec3), 0);

	mTangBuf.bind();
	mVAO.add(&mTangBuf, NSShader::Tangent);
	mVAO.vertexAttribPtr(NSShader::Tangent, 3, GL_FLOAT, GL_FALSE, sizeof(fvec3), 0);

	mBoneBuf.bind();
	mVAO.add(&mBoneBuf, NSShader::BoneID);
	mVAO.add(&mBoneBuf, NSShader::BoneWeight);
	mVAO.vertexAttribIPtr(NSShader::BoneID, 4, GL_INT, sizeof(NSMesh::SubMesh::BoneWeightIDs), 0);
	mVAO.vertexAttribPtr(NSShader::BoneWeight, 4, GL_FLOAT, GL_FALSE, sizeof(NSMesh::SubMesh::BoneWeightIDs), 4 * sizeof(nsuint));

	mIndiceBuf.bind();

	mVAO.unbind();
}

NSMesh::Node::Node(const nsstring & pName, Node * pParentNode):mName(pName),
	mNodeID(0),
	mNodeTransform(),
	mWorldTransform(),
	mParentNode(pParentNode),
	mChildNodes()
{}

NSMesh::Node::~Node()
{
	while (mChildNodes.begin() != mChildNodes.end())
	{
		delete mChildNodes.back();
		mChildNodes.pop_back();
	}
}

NSMesh::Node * NSMesh::Node::findNode(const nsstring & pNodeName)
{
	if (mName == pNodeName)
		return this;

	auto iter = mChildNodes.begin();
	while (iter != mChildNodes.end())
	{
		Node * ret = (*iter)->findNode(pNodeName);
		if (ret != NULL)
			return ret;
		++iter;
	}
	return NULL;
}

NSMesh::Node * NSMesh::Node::createChild(const nsstring & pName)
{
	Node * childNode = new Node(pName, this);
	mChildNodes.push_back(childNode);
	return childNode;
}

NSMesh::Bone::Bone(): boneID(0),
	mOffsetTransform()
{}

NSMesh::NodeTree::NodeTree(): boneNameMap(),
	mRootNode(NULL)
{}

NSMesh::Node * NSMesh::NodeTree::findNode(const nsstring & pNodeName)
{
	if (mRootNode == NULL)
		return NULL;
	return mRootNode->findNode(pNodeName);
}

NSMesh::NodeTree::~NodeTree()
{
	delete mRootNode;
}

NSMesh::Node * NSMesh::NodeTree::createRootNode(const nsstring & pName)
{
	mRootNode = new Node(pName,NULL);
	mRootNode->mName = pName;
	return mRootNode;
}
