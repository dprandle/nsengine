///*--------------------------------------------- Noble Steed Engine--------------------------------------*
//Authors : Daniel Randle, Alex Bourne
//Date Created : Mar 6 2013
//
//File:
//	nsmesh.cpp
//
//Description:
//	This file contains the definition for the nsmesh class functions along with any other helper
//	functions that will aid in loading/using meshes
//*-----------------------------------------------------------------------------------------------
#include <nsmesh.h>
#include <nsengine.h>
#include <nsshader.h>

nsmesh::nsmesh(): m_node_tree(new node_tree())
{
	set_ext(DEFAULT_MESH_EXTENSION);
}

nsmesh::~nsmesh()
{
	while (m_submeshes.begin() != m_submeshes.end())
	{
		delete m_submeshes.back();
		m_submeshes.pop_back();
	}
	delete m_node_tree;
}

void nsmesh::allocate()
{
	for (uint32 i = 0; i < count(); ++i)
		allocate(i);
}

void nsmesh::allocate(uint32 subindex)
{
	sub(subindex)->allocate_buffers();
}

void nsmesh::bake_rotation(uint32 subindex, const fquat & pRot)
{
	submesh * sb = sub(subindex);
	if (sb == NULL)
		return;
	for (uint32 i = 0; i < sb->positions.size(); ++i)
	{
		sb->positions[i] = rotation_mat3(pRot) * sb->positions[i];
		if (sb->normals.size() == sb->positions.size())
			sb->normals[i] = rotation_mat3(pRot) * sb->normals[i];
		if (sb->tangents.size() == sb->positions.size())
			sb->tangents[i] = rotation_mat3(pRot) * sb->tangents[i];
	}
	sb->allocate_buffers();
	calc_aabb();
}

void nsmesh::bake_scaling(uint32 subindex, const fvec3 & pScaling)
{
	submesh * sb = sub(subindex);
	if (sb == NULL)
		return;
	for (uint32 i = 0; i < sb->positions.size(); ++i)
	{
		sb->positions[i] %= pScaling;
		if (sb->normals.size() == sb->positions.size())
			sb->normals[i] %= pScaling;
		if (sb->tangents.size() == sb->positions.size())
			sb->tangents[i] %= pScaling;
	}
	sb->allocate_buffers();
	calc_aabb();
}

void nsmesh::bake_translation(uint32 subindex, const fvec3 & pTrans)
{
	submesh * sb = sub(subindex);
	if (sb == NULL)
		return;
	for (uint32 i = 0; i < sb->positions.size(); ++i)
	{
		sb->positions[i] += pTrans;
		if (sb->normals.size() == sb->positions.size())
			sb->normals[i] += pTrans;
		if (sb->tangents.size() == sb->positions.size())
			sb->tangents[i] += pTrans;
	}
	sb->allocate_buffers();
	calc_aabb();
}

void nsmesh::bake()
{
	for (uint32 subindex = 0; subindex < count(); ++subindex)
	{
		submesh * sb = sub(subindex);
		if (sb == NULL || sb->node_ == NULL)
			return;
		for (uint32 i = 0; i < sb->positions.size(); ++i)
		{
			sb->positions[i] = (sb->node_->world_transform * sb->positions[i]).xyz();
			if (sb->normals.size() == sb->positions.size())
				sb->normals[i] = (sb->node_->world_transform * sb->normals[i]).xyz();
			if (sb->tangents.size() == sb->positions.size())
				sb->tangents[i] = (sb->node_->world_transform * sb->tangents[i]).xyz();
		}
		sb->node_ = NULL;
		sb->allocate_buffers();
	}
}

void nsmesh::bake_rotation(const fquat & pRot)
{
	for (uint32 subindex = 0; subindex < count(); ++subindex)
	{
		submesh * sb = sub(subindex);
		if (sb == NULL)
			return;
		for (uint32 i = 0; i < sb->positions.size(); ++i)
		{
			sb->positions[i] = rotation_mat3(pRot) * sb->positions[i];
			if (sb->normals.size() == sb->positions.size())
				sb->normals[i] = rotation_mat3(pRot) * sb->normals[i];
			if (sb->tangents.size() == sb->positions.size())
				sb->tangents[i] = rotation_mat3(pRot) * sb->tangents[i];
		}
		sb->allocate_buffers();
	}
	calc_aabb();
}

void nsmesh::bake_scaling(const fvec3 & pScaling)
{
	for (uint32 subindex = 0; subindex < count(); ++subindex)
	{
		submesh * sb = sub(subindex);
		if (sb == NULL)
			return;
		for (uint32 i = 0; i < sb->positions.size(); ++i)
		{
			sb->positions[i] %= pScaling;
			if (sb->normals.size() == sb->positions.size())
				sb->normals[i] %= pScaling;
			if (sb->tangents.size() == sb->positions.size())
				sb->tangents[i] %= pScaling;
		}
		sb->allocate_buffers();
	}
	calc_aabb();
}

void nsmesh::bake_translation(const fvec3 & pTrans)
{
	for (uint32 subindex = 0; subindex < count(); ++subindex)
	{
		submesh * sb = sub(subindex);
		if (sb == NULL)
			return;
		for (uint32 i = 0; i < sb->positions.size(); ++i)
		{
			sb->positions[i] += pTrans;
			if (sb->normals.size() == sb->positions.size())
				sb->normals[i] += pTrans;
			if (sb->tangents.size() == sb->positions.size())
				sb->tangents[i] += pTrans;
		}
		sb->allocate_buffers();
	}
	calc_aabb();
}

void nsmesh::bake_node_rotation(const fquat & pRot)
{
	transform_node(m_node_tree->root_node, fmat4(rotation_mat3(pRot)));
	calc_aabb();
}

void nsmesh::bake_node_scaling(const fvec3 & pScaling)
{
	transform_node(m_node_tree->root_node, scaling_mat4(pScaling));
	calc_aabb();
}

void nsmesh::bake_node_translation(const fvec3 & pTrans)
{
	transform_node(m_node_tree->root_node, translation_mat4(pTrans));
	calc_aabb();
}

void nsmesh::calc_aabb()
{
	m_bounding_box.clear();
	auto iter = m_submeshes.begin();
	while (iter != m_submeshes.end())
	{
		fmat4 transform;
		if ((*iter)->node_ != NULL)
			transform = (*iter)->node_->world_transform;
		(*iter)->calc_aabb();
		m_bounding_box.extend((*iter)->positions, transform);
		++iter;
	}
}

void nsmesh::flip_uv()
{
	for (uint32 i = 0; i < m_submeshes.size(); ++i)
		flip_uv(i);
}

void nsmesh::flip_uv(uint32 pSubIndex)
{
	if (pSubIndex >= m_submeshes.size())
	{
		dprint("nsmesh::flipSubUVs Trying to get subMesh with out of range index");
		return;
	}
	submesh * sb = sub(pSubIndex);
	for (uint32 i = 0; i < sb->tex_coords.size(); ++i)
		sb->tex_coords[i].v = 1 - sb->tex_coords[i].v;

	sb->tex_buf.bind();
	sb->tex_buf.set_data(sb->tex_coords,
						 0,
						 static_cast<uint32>(sb->tex_coords.size()));
	sb->tex_buf.unbind();
}

void nsmesh::pup(nsfile_pupper * p)
{
	if (p->type() == nsfile_pupper::pup_binary)
	{
		nsbinary_file_pupper * bf = static_cast<nsbinary_file_pupper *>(p);
		::pup(*bf, *this);
	}
	else
	{
		nstext_file_pupper * tf = static_cast<nstext_file_pupper *>(p);
		::pup(*tf, *this);
	}
}

bool nsmesh::contains(submesh * submesh)
{
	for (uint32 i = 0; i < m_submeshes.size(); ++i)
	{
		if (m_submeshes[i] == submesh)
			return true;
	}
	return false;
}

bool nsmesh::contains(uint32 subindex)
{
	return (sub(subindex) != NULL);
}

bool nsmesh::contains(const nsstring & subname)
{
	return (sub(subname) != NULL);
}

bool nsmesh::del()
{
	bool ret = false;
	while (m_submeshes.begin() != m_submeshes.end())
	{
		delete m_submeshes.back();
		m_submeshes.pop_back();
		ret = true;
	}
	delete m_node_tree;
	m_node_tree = new node_tree();
	return ret;
}

bool nsmesh::add(submesh * submesh)
{
	bool ret = !contains(submesh);
	if (ret)
		m_submeshes.push_back(submesh);
	return ret;
}

nsmesh::submesh * nsmesh::create()
{
	submesh * subMesh = new submesh(this);
	subMesh->init_gl();
	if (!add(subMesh))
	{
		delete subMesh;
		return NULL;
	}
	return subMesh;
}

void nsmesh::flip_normals()
{
	for (uint32 i = 0; i < m_submeshes.size(); ++i)
		flip_normals(i);
}

void nsmesh::flip_normals(uint32 pSubIndex)
{
	if (pSubIndex >= m_submeshes.size())
	{
		dprint("nsmesh::flipSubNormals Trying to get subMesh with out of range index");
		return;
	}

	submesh * sb = sub(pSubIndex);
	for (uint32 i = 0; i < sb->normals.size(); ++i)
	{
		sb->normals[i] *= -1.0f;
		sb->tangents[i] *= -1.0f;
	}

	sb->norm_buf.bind();
	sb->norm_buf.set_data(sb->normals,
						  0,
						  static_cast<uint32>(sb->normals.size()));
	sb->tang_buf.bind();
	sb->tang_buf.set_data(sb->tangents,
						  0,
						  static_cast<uint32>(sb->tangents.size()));
	sb->norm_buf.unbind();
}

void nsmesh::init_gl()
{
	for (uint32 i = 0; i < count(); ++i)
		init_gl(i);
}

void nsmesh::init_gl(uint32 subindex)
{
	sub(subindex)->init_gl();
}

const nsbounding_box & nsmesh::aabb()
{
	return m_bounding_box;
}

nsmesh::submesh * nsmesh::sub(const nsstring & pName)
{
	submesh_iter iter = m_submeshes.begin();
	while (iter != m_submeshes.end())
	{
		if ((*iter)->name == pName)
			return *iter;
		++iter;
	}
	return NULL;
}

nsmesh::submesh * nsmesh::sub(uint32 pIndex)
{
	if (pIndex >= m_submeshes.size())
		return NULL;

	return m_submeshes[pIndex];
}

void nsmesh::init()
{
}

float nsmesh::volume()
{
	float K1 = 1.0f;
	float K2 = 1.0f;
	float K3 = 1.0f;

	float mult = 1 / (2 * (K1 + K2 + K3));

	float sum = 0.0f;

	for (uint32 subI = 0; subI < m_submeshes.size(); ++subI)
	{
		nsmesh::submesh * sb = sub(subI);
		for (uint32 triI = 0; triI < sb->triangles.size(); ++triI)
		{
			fvec3 Ai = sb->positions[sb->triangles[triI][0]];
			fvec3 Bi = sb->positions[sb->triangles[triI][1]];
			fvec3 Ci = sb->positions[sb->triangles[triI][2]];
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

uint32 nsmesh::count()
{
	return static_cast<uint32>(m_submeshes.size());
}

nsmesh::node_tree * nsmesh::tree()
{
	return m_node_tree;
}

bool nsmesh::del(submesh * sb)
{
	sb = remove(sb);
	if (sb == NULL)
		return false;
	delete sb;
	return true;
}

uint32 nsmesh::find(submesh * sb)
{
	for (uint32 i = 0; i < m_submeshes.size(); ++i)
	{
		if (m_submeshes[i] == sb)
			return i;
	}
	return -1;
}

uint32 nsmesh::find(const nsstring & subname)
{
	return find(sub(subname));
}

bool nsmesh::del(uint32 pIndex)
{
	return del(sub(pIndex));
}

bool nsmesh::del(const nsstring & subname)
{
	return del(sub(subname));
}

nsmesh::submesh * nsmesh::remove(submesh * sb)
{
	uint32 cnt = find(sb);
	if (cnt == -1)
		return NULL;
	auto iter = m_submeshes.begin() + cnt;
	m_submeshes.erase(iter);
	return sb;
}

nsmesh::submesh * nsmesh::remove(uint32 index)
{
	return remove(sub(index));
}

nsmesh::submesh * nsmesh::remove(const nsstring & name)
{
	return remove(sub(name));
}

uint32 nsmesh::vert_count()
{
	uint32 total = 0;
	for (uint32 i = 0; i < m_submeshes.size(); ++i)
		total += static_cast<uint32>(m_submeshes[i]->indices.size());
	return total;
}

uint32 nsmesh::vert_count(uint32 pIndex)
{
	return static_cast<uint32>(m_submeshes[pIndex]->indices.size());
}

void nsmesh::transform_node(node * pNode, const fmat4 & pTransform)
{
	pNode->node_transform = pTransform * pNode->node_transform;

	if (pNode->parent_node != NULL)
		pNode->world_transform = pNode->parent_node->world_transform * pNode->node_transform;
	else
		pNode->world_transform = pNode->node_transform;

	for (uint32 i = 0; i < pNode->child_nodes.size(); ++i)
		_propagate_world_transform(pNode->child_nodes[i]);
}

void nsmesh::_propagate_world_transform(node * pChildNode)
{
	if (pChildNode->parent_node != NULL)
		pChildNode->world_transform = pChildNode->parent_node->world_transform * pChildNode->node_transform;
	else
		pChildNode->world_transform = pChildNode->node_transform;

	for (uint32 i = 0; i < pChildNode->child_nodes.size(); ++i)
		_propagate_world_transform(pChildNode->child_nodes[i]);
}

nsmesh::submesh::joint::joint()
{
	for (int32 i = 0; i < BONES_PER_JOINT; ++i)
	{
		bone_ids[i] = 0;
		weights[i] = 0.0f;
	}
}

void nsmesh::submesh::joint::add_bone(uint32 pBoneID, float pWeight)
{
	for (int32 i = 0; i < BONES_PER_JOINT; ++i)
	{
		if (weights[i] == 0.0f)
		{
			bone_ids[i] = pBoneID;
			weights[i] = pWeight;
			return;
		}
	}
}

nsmesh::submesh::submesh(nsmesh * pParentMesh): 
	pos_buf(nsbuffer_object::array,nsbuffer_object::storage_mutable),
	tex_buf(nsbuffer_object::array,nsbuffer_object::storage_mutable),
	norm_buf(nsbuffer_object::array,nsbuffer_object::storage_mutable),
	tang_buf(nsbuffer_object::array,nsbuffer_object::storage_mutable),
	joint_buf(nsbuffer_object::array,nsbuffer_object::storage_mutable),
	indice_buf(nsbuffer_object::element_array,nsbuffer_object::storage_mutable),
	positions(),
	tex_coords(),
	normals(),
	tangents(),
	joints(),
	indices(),
	node_(NULL),
	name(),
	parent_mesh(pParentMesh),
	has_tex_coords(false),
	bounding_box()
{}

nsmesh::submesh::~submesh()
{
	pos_buf.release();
	tex_buf.release();
	norm_buf.release();
	tang_buf.release();
	joint_buf.release();
	indice_buf.release();
	vao.release();
}

void nsmesh::submesh::allocate_buffers()
{
	pos_buf.bind();
	pos_buf.allocate(positions,
					 nsbuffer_object::mutable_static_draw,
					 static_cast<uint32>(positions.size()));
	tex_buf.bind();
	tex_buf.allocate(tex_coords,
					 nsbuffer_object::mutable_static_draw,
					 static_cast<uint32>(tex_coords.size()));
	norm_buf.bind();
	norm_buf.allocate(normals,
					  nsbuffer_object::mutable_static_draw,
					  static_cast<uint32>(normals.size()));
	tang_buf.bind();
	tang_buf.allocate(tangents,
					  nsbuffer_object::mutable_static_draw,
					  static_cast<uint32>(tangents.size()));
	joint_buf.bind();
	joint_buf.allocate(joints,
					  nsbuffer_object::mutable_static_draw,
					  static_cast<uint32>(joints.size()));
	indice_buf.bind();
	indice_buf.allocate(indices,
						nsbuffer_object::mutable_static_draw,
						static_cast<uint32>(indices.size()));
	update_VAO();
}

void nsmesh::submesh::init_gl()
{
	pos_buf.init_gl();
	tex_buf.init_gl();
	norm_buf.init_gl();
	tang_buf.init_gl();
	joint_buf.init_gl();
	indice_buf.init_gl();
	vao.init_gl();
}

void nsmesh::submesh::calc_aabb()
{
	if (node_ == NULL)
		return;
	bounding_box.calculate(positions,node_->world_transform);
}

void nsmesh::submesh::resize(uint32 pNewSize)
{
	positions.resize(pNewSize);
	tex_coords.resize(pNewSize);
	normals.resize(pNewSize);
	tangents.resize(pNewSize);
	indices.resize(pNewSize);
}

void nsmesh::submesh::update_VAO()
{
	vao.bind();

	pos_buf.bind();
	vao.add(&pos_buf, nsshader::loc_position);
	vao.vertex_attrib_ptr(nsshader::loc_position, 3, GL_FLOAT, GL_FALSE, sizeof(fvec3), 0);

	tex_buf.bind();
	vao.add(&tex_buf, nsshader::loc_tex_coords);
	vao.vertex_attrib_ptr(nsshader::loc_tex_coords, 2, GL_FLOAT, GL_FALSE, sizeof(fvec2), 0);

	norm_buf.bind();
	vao.add(&norm_buf, nsshader::loc_normal);
	vao.vertex_attrib_ptr(nsshader::loc_normal, 3, GL_FLOAT, GL_FALSE, sizeof(fvec3), 0);

	tang_buf.bind();
	vao.add(&tang_buf, nsshader::loc_tangent);
	vao.vertex_attrib_ptr(nsshader::loc_tangent, 3, GL_FLOAT, GL_FALSE, sizeof(fvec3), 0);

	joint_buf.bind();
	vao.add(&joint_buf, nsshader::loc_bone_id);
	vao.add(&joint_buf, nsshader::loc_joint);
	vao.vertex_attrib_I_ptr(nsshader::loc_bone_id, 4, GL_INT, sizeof(nsmesh::submesh::joint), 0);
	vao.vertex_attrib_ptr(nsshader::loc_joint, 4, GL_FLOAT, GL_FALSE, sizeof(nsmesh::submesh::joint), 4 * sizeof(uint32));

	indice_buf.bind();

	vao.unbind();
}

nsmesh::node::node(const nsstring & pName, node * pParentNode):name(pName),
	node_id(0),
	node_transform(),
	world_transform(),
	parent_node(pParentNode),
	child_nodes()
{}

nsmesh::node::~node()
{
	while (child_nodes.begin() != child_nodes.end())
	{
		delete child_nodes.back();
		child_nodes.pop_back();
	}
}

nsmesh::node * nsmesh::node::find_node(const nsstring & pNodeName)
{
	if (name == pNodeName)
		return this;

	auto iter = child_nodes.begin();
	while (iter != child_nodes.end())
	{
		node * ret = (*iter)->find_node(pNodeName);
		if (ret != NULL)
			return ret;
		++iter;
	}
	return NULL;
}

nsmesh::node * nsmesh::node::create_child(const nsstring & pName)
{
	node * childNode = new node(pName, this);
	child_nodes.push_back(childNode);
	return childNode;
}

nsmesh::bone::bone(): boneID(0),
	mOffsetTransform()
{}

nsmesh::node_tree::node_tree(): bone_name_map(),
	root_node(NULL)
{}

nsmesh::node * nsmesh::node_tree::find_node(const nsstring & pNodeName)
{
	if (root_node == NULL)
		return NULL;
	return root_node->find_node(pNodeName);
}

nsmesh::node_tree::~node_tree()
{
	delete root_node;
}

nsmesh::node * nsmesh::node_tree::create_root_node(const nsstring & pName)
{
	root_node = new node(pName,NULL);
	root_node->name = pName;
	return root_node;
}
