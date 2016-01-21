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
#include <nsgl_context.h>
#include <nsshader.h>

nsmesh::nsmesh():
	nsresource(),
	m_node_tree(new node_tree())
{
	set_ext(DEFAULT_MESH_EXTENSION);
}

nsmesh::nsmesh(const nsmesh & copy_):
	nsresource(copy_),
	m_node_tree(new node_tree(*copy_.m_node_tree)),
	m_submeshes(copy_.m_submeshes.size(), nullptr),
	m_bounding_box(copy_.m_bounding_box)
{
	for (uint32 i = 0; i < m_submeshes.size(); ++i)
	{
		m_submeshes[i] = new submesh(*copy_.m_submeshes[i]);
		m_submeshes[i]->init_gl();
		m_submeshes[i]->allocate_buffers();
	}
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


nsmesh & nsmesh::operator=(nsmesh rhs)
{
	nsresource::operator=(rhs);
	std::swap(m_submeshes,rhs.m_submeshes);
	std::swap(m_node_tree, rhs.m_node_tree);
	std::swap(m_bounding_box, rhs.m_bounding_box);
	return *this;
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
	for (uint32 i = 0; i < sb->m_verts.size(); ++i)
	{
		sb->m_verts[i] = rotation_mat3(pRot) * sb->m_verts[i];
		if (sb->m_normals.size() == sb->m_verts.size())
			sb->m_normals[i] = rotation_mat3(pRot) * sb->m_normals[i];
		if (sb->m_tangents.size() == sb->m_verts.size())
			sb->m_tangents[i] = rotation_mat3(pRot) * sb->m_tangents[i];
	}
	sb->allocate_buffers();
	calc_aabb();
}

void nsmesh::bake_scaling(uint32 subindex, const fvec3 & pScaling)
{
	submesh * sb = sub(subindex);
	if (sb == NULL)
		return;
	for (uint32 i = 0; i < sb->m_verts.size(); ++i)
	{
		sb->m_verts[i] %= pScaling;
		if (sb->m_normals.size() == sb->m_verts.size())
			sb->m_normals[i] %= pScaling;
		if (sb->m_tangents.size() == sb->m_verts.size())
			sb->m_tangents[i] %= pScaling;
	}
	sb->allocate_buffers();
	calc_aabb();
}

void nsmesh::bake_translation(uint32 subindex, const fvec3 & pTrans)
{
	submesh * sb = sub(subindex);
	if (sb == NULL)
		return;
	for (uint32 i = 0; i < sb->m_verts.size(); ++i)
	{
		sb->m_verts[i] += pTrans;
		if (sb->m_normals.size() == sb->m_verts.size())
			sb->m_normals[i] += pTrans;
		if (sb->m_tangents.size() == sb->m_verts.size())
			sb->m_tangents[i] += pTrans;
	}
	sb->allocate_buffers();
	calc_aabb();
}

void nsmesh::bake()
{
	for (uint32 subindex = 0; subindex < count(); ++subindex)
	{
		submesh * sb = sub(subindex);
		if (sb == NULL || sb->m_node == NULL)
			return;
		for (uint32 i = 0; i < sb->m_verts.size(); ++i)
		{
			sb->m_verts[i] = (sb->m_node->m_world_tform * sb->m_verts[i]).xyz();
			if (sb->m_normals.size() == sb->m_verts.size())
				sb->m_normals[i] = (sb->m_node->m_world_tform * sb->m_normals[i]).xyz();
			if (sb->m_tangents.size() == sb->m_verts.size())
				sb->m_tangents[i] = (sb->m_node->m_world_tform * sb->m_tangents[i]).xyz();
		}
		sb->m_node = NULL;
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
		for (uint32 i = 0; i < sb->m_verts.size(); ++i)
		{
			sb->m_verts[i] = rotation_mat3(pRot) * sb->m_verts[i];
			if (sb->m_normals.size() == sb->m_verts.size())
				sb->m_normals[i] = rotation_mat3(pRot) * sb->m_normals[i];
			if (sb->m_tangents.size() == sb->m_verts.size())
				sb->m_tangents[i] = rotation_mat3(pRot) * sb->m_tangents[i];
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
		for (uint32 i = 0; i < sb->m_verts.size(); ++i)
		{
			sb->m_verts[i] %= pScaling;
			if (sb->m_normals.size() == sb->m_verts.size())
				sb->m_normals[i] %= pScaling;
			if (sb->m_tangents.size() == sb->m_verts.size())
				sb->m_tangents[i] %= pScaling;
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
		for (uint32 i = 0; i < sb->m_verts.size(); ++i)
		{
			sb->m_verts[i] += pTrans;
			if (sb->m_normals.size() == sb->m_verts.size())
				sb->m_normals[i] += pTrans;
			if (sb->m_tangents.size() == sb->m_verts.size())
				sb->m_tangents[i] += pTrans;
		}
		sb->allocate_buffers();
	}
	calc_aabb();
}

void nsmesh::bake_node_rotation(const fquat & pRot)
{
	transform_node(m_node_tree->m_root, fmat4(rotation_mat3(pRot)));
	calc_aabb();
}

void nsmesh::bake_node_scaling(const fvec3 & pScaling)
{
	transform_node(m_node_tree->m_root, scaling_mat4(pScaling));
	calc_aabb();
}

void nsmesh::bake_node_translation(const fvec3 & pTrans)
{
	transform_node(m_node_tree->m_root, translation_mat4(pTrans));
	calc_aabb();
}

void nsmesh::calc_aabb()
{
	m_bounding_box.clear();
	auto iter = m_submeshes.begin();
	while (iter != m_submeshes.end())
	{
		fmat4 transform;
		if ((*iter)->m_node != NULL)
			transform = (*iter)->m_node->m_world_tform;
		(*iter)->calc_aabb();
		m_bounding_box.extend((*iter)->m_verts, transform);
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
	for (uint32 i = 0; i < sb->m_tex_coords.size(); ++i)
		sb->m_tex_coords[i].v = 1 - sb->m_tex_coords[i].v;

	sb->m_tex_buf.bind();
	sb->m_tex_buf.set_data(sb->m_tex_coords,
						 0,
						 static_cast<uint32>(sb->m_tex_coords.size()));
	sb->m_tex_buf.unbind();
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
	for (uint32 i = 0; i < sb->m_normals.size(); ++i)
	{
		sb->m_normals[i] *= -1.0f;
		sb->m_tangents[i] *= -1.0f;
	}

	sb->m_norm_buf.bind();
	sb->m_norm_buf.set_data(sb->m_normals,
						  0,
						  static_cast<uint32>(sb->m_normals.size()));
	sb->m_tang_buf.bind();
	sb->m_tang_buf.set_data(sb->m_tangents,
						  0,
						  static_cast<uint32>(sb->m_tangents.size()));
	sb->m_norm_buf.unbind();
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
		if ((*iter)->m_name == pName)
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
		for (uint32 triI = 0; triI < sb->m_triangles.size(); ++triI)
		{
			fvec3 Ai = sb->m_verts[sb->m_triangles[triI][0]];
			fvec3 Bi = sb->m_verts[sb->m_triangles[triI][1]];
			fvec3 Ci = sb->m_verts[sb->m_triangles[triI][2]];
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
		total += static_cast<uint32>(m_submeshes[i]->m_verts.size());
	return total;
}

uint32 nsmesh::vert_count(uint32 pIndex)
{
	return static_cast<uint32>(m_submeshes[pIndex]->m_verts.size());
}

uint32 nsmesh::indice_count()
{
	uint32 total = 0;
	for (uint32 i = 0; i < m_submeshes.size(); ++i)
		total += static_cast<uint32>(m_submeshes[i]->m_indices.size());
	return total;	
}

uint32 nsmesh::indice_count(uint32 pIndex)
{
	return static_cast<uint32>(m_submeshes[pIndex]->m_indices.size());	
}

void nsmesh::transform_node(node * pNode, const fmat4 & pTransform)
{
	pNode->m_node_tform = pTransform * pNode->m_node_tform;

	if (pNode->m_parent_node != NULL)
		pNode->m_world_tform = pNode->m_parent_node->m_world_tform * pNode->m_node_tform;
	else
		pNode->m_world_tform = pNode->m_node_tform;

	for (uint32 i = 0; i < pNode->m_child_nodes.size(); ++i)
		_propagate_world_transform(pNode->m_child_nodes[i]);
}

void nsmesh::_propagate_world_transform(node * pChildNode)
{
	if (pChildNode->m_parent_node != NULL)
		pChildNode->m_world_tform = pChildNode->m_parent_node->m_world_tform * pChildNode->m_node_tform;
	else
		pChildNode->m_world_tform = pChildNode->m_node_tform;

	for (uint32 i = 0; i < pChildNode->m_child_nodes.size(); ++i)
		_propagate_world_transform(pChildNode->m_child_nodes[i]);
}

nsmesh::submesh::connected_joints::connected_joints()
{
	for (int32 i = 0; i < JOINTS_PER_VERTEX; ++i)
	{
		m_joint_ids[i] = 0;
		m_weights[i] = 0.0f;
	}
}

void nsmesh::submesh::connected_joints::add_joint(uint32 pBoneID, float pWeight)
{
	for (int32 i = 0; i < JOINTS_PER_VERTEX; ++i)
	{
		if (m_weights[i] == 0.0f)
		{
			m_joint_ids[i] = pBoneID;
			m_weights[i] = pWeight;
			return;
		}
	}
}

nsmesh::submesh::submesh(nsmesh * pParentMesh): 
	m_vert_buf(nsbuffer_object::array,nsbuffer_object::storage_mutable),
	m_tex_buf(nsbuffer_object::array,nsbuffer_object::storage_mutable),
	m_norm_buf(nsbuffer_object::array,nsbuffer_object::storage_mutable),
	m_tang_buf(nsbuffer_object::array,nsbuffer_object::storage_mutable),
	m_indice_buf(nsbuffer_object::element_array,nsbuffer_object::storage_mutable),
	m_joint_buf(nsbuffer_object::array,nsbuffer_object::storage_mutable),
	m_vao(),
	m_verts(),
	m_tex_coords(),
	m_normals(),
	m_tangents(),
	m_indices(),
	m_triangles(),
	m_lines(),
	m_joints(),
	m_prim_type(0),
	m_node(NULL),
	m_name(),
	m_parent_mesh(pParentMesh),
	m_bounding_box(),
	m_has_tex_coords(false)
{}

nsmesh::submesh::submesh(const submesh & copy_):
	m_vert_buf(nsbuffer_object::array,nsbuffer_object::storage_mutable),
	m_tex_buf(nsbuffer_object::array,nsbuffer_object::storage_mutable),
	m_norm_buf(nsbuffer_object::array,nsbuffer_object::storage_mutable),
	m_tang_buf(nsbuffer_object::array,nsbuffer_object::storage_mutable),
	m_indice_buf(nsbuffer_object::element_array,nsbuffer_object::storage_mutable),
	m_joint_buf(nsbuffer_object::array,nsbuffer_object::storage_mutable),
	m_vao(),
	m_verts(copy_.m_verts),
	m_tex_coords(copy_.m_tex_coords),
	m_normals(copy_.m_normals),
	m_tangents(copy_.m_tangents),
	m_indices(copy_.m_indices),
	m_triangles(copy_.m_triangles),
	m_lines(copy_.m_lines),
	m_joints(copy_.m_joints),
	m_prim_type(copy_.m_prim_type),
	m_node(nullptr),
	m_name(copy_.m_name),
	m_parent_mesh(copy_.m_parent_mesh),
	m_bounding_box(copy_.m_bounding_box),
	m_has_tex_coords(copy_.m_has_tex_coords)
{
	if (copy_.m_node != nullptr)
		m_node = m_parent_mesh->tree()->find_node(copy_.m_node->m_node_id);
}

nsmesh::submesh & nsmesh::submesh::operator=(submesh rhs_)
{
	std::swap(m_verts,rhs_.m_verts);
	std::swap(m_tex_coords,rhs_.m_tex_coords);
	std::swap(m_normals, rhs_.m_normals);
	std::swap(m_tangents, rhs_.m_tangents);
	std::swap(m_indices, rhs_.m_indices);
	std::swap(m_triangles, rhs_.m_triangles);
	std::swap(m_lines, rhs_.m_lines);
	std::swap(m_joints, rhs_.m_joints);
	std::swap(m_prim_type, rhs_.m_prim_type);
	std::swap(m_node, rhs_.m_node);
	std::swap(m_name, rhs_.m_name);
	std::swap(m_parent_mesh, rhs_.m_parent_mesh);
	std::swap(m_bounding_box, rhs_.m_bounding_box);
	std::swap(m_has_tex_coords, rhs_.m_has_tex_coords);
	return *this;
}

nsmesh::submesh::~submesh()
{
	m_vert_buf.release();
	m_tex_buf.release();
	m_norm_buf.release();
	m_tang_buf.release();
	m_joint_buf.release();
	m_indice_buf.release();
	m_vao.release();
}

void nsmesh::submesh::allocate_buffers()
{
	m_vert_buf.bind();
	m_vert_buf.allocate(m_verts,
					 nsbuffer_object::mutable_static_draw,
					 static_cast<uint32>(m_verts.size()));
	m_tex_buf.bind();
	m_tex_buf.allocate(m_tex_coords,
					 nsbuffer_object::mutable_static_draw,
					 static_cast<uint32>(m_tex_coords.size()));
	m_norm_buf.bind();
	m_norm_buf.allocate(m_normals,
					  nsbuffer_object::mutable_static_draw,
					  static_cast<uint32>(m_normals.size()));
	m_tang_buf.bind();
	m_tang_buf.allocate(m_tangents,
					  nsbuffer_object::mutable_static_draw,
					  static_cast<uint32>(m_tangents.size()));
	m_joint_buf.bind();
	m_joint_buf.allocate(m_joints,
					  nsbuffer_object::mutable_static_draw,
					  static_cast<uint32>(m_joints.size()));
	m_indice_buf.bind();
	m_indice_buf.allocate(m_indices,
						nsbuffer_object::mutable_static_draw,
						static_cast<uint32>(m_indices.size()));
	update_vao();
}

void nsmesh::submesh::init_gl()
{
	m_vert_buf.init_gl();
	m_tex_buf.init_gl();
	m_norm_buf.init_gl();
	m_tang_buf.init_gl();
	m_joint_buf.init_gl();
	m_indice_buf.init_gl();
	m_vao.init_gl();
}

void nsmesh::submesh::calc_aabb()
{
	if (m_node == NULL)
		return;
	m_bounding_box.calculate(m_verts,m_node->m_world_tform);
}

void nsmesh::submesh::resize(uint32 pNewSize)
{
	m_verts.resize(pNewSize);
	m_tex_coords.resize(pNewSize);
	m_normals.resize(pNewSize);
	m_tangents.resize(pNewSize);
	m_indices.resize(pNewSize);
}

void nsmesh::submesh::update_vao()
{
	m_vao.bind();

	m_vert_buf.bind();
	m_vao.add(&m_vert_buf, nsshader::loc_position);
	m_vao.vertex_attrib_ptr(nsshader::loc_position, 3, GL_FLOAT, GL_FALSE, sizeof(fvec3), 0);

	m_tex_buf.bind();
	m_vao.add(&m_tex_buf, nsshader::loc_tex_coords);
	m_vao.vertex_attrib_ptr(nsshader::loc_tex_coords, 2, GL_FLOAT, GL_FALSE, sizeof(fvec2), 0);

	m_norm_buf.bind();
	m_vao.add(&m_norm_buf, nsshader::loc_normal);
	m_vao.vertex_attrib_ptr(nsshader::loc_normal, 3, GL_FLOAT, GL_FALSE, sizeof(fvec3), 0);

	m_tang_buf.bind();
	m_vao.add(&m_tang_buf, nsshader::loc_tangent);
	m_vao.vertex_attrib_ptr(nsshader::loc_tangent, 3, GL_FLOAT, GL_FALSE, sizeof(fvec3), 0);

	m_joint_buf.bind();
	m_vao.add(&m_joint_buf, nsshader::loc_bone_id);
	m_vao.add(&m_joint_buf, nsshader::loc_joint);
	m_vao.vertex_attrib_I_ptr(nsshader::loc_bone_id, 4, GL_INT, sizeof(nsmesh::submesh::connected_joints), 0);
	m_vao.vertex_attrib_ptr(nsshader::loc_joint, 4, GL_FLOAT, GL_FALSE, sizeof(nsmesh::submesh::connected_joints), 4 * sizeof(uint32));

	m_indice_buf.bind();

	m_vao.unbind();
}

nsmesh::node::node(const nsstring & pName, node * pParentNode):m_name(pName),
	m_node_id(0),
	m_node_tform(),
	m_world_tform(),
	m_parent_node(pParentNode),
	m_child_nodes()
{}

nsmesh::node::node(const node & copy_):
	m_node_id(copy_.m_node_id),
	m_node_tform(copy_.m_node_tform),
	m_world_tform(copy_.m_world_tform),
	m_parent_node(nullptr),
	m_child_nodes(copy_.m_child_nodes.size())
{
	for (uint32 i = 0; i < copy_.m_child_nodes.size(); ++i)
	{
		m_child_nodes[i] = new node(*copy_.m_child_nodes[i]);
		m_child_nodes[i]->m_parent_node = this;
	}
}

nsmesh::node::~node()
{
	while (m_child_nodes.begin() != m_child_nodes.end())
	{
		delete m_child_nodes.back();
		m_child_nodes.pop_back();
	}
}

nsmesh::node * nsmesh::node::find_node(const nsstring & pNodeName)
{
	if (m_name == pNodeName)
		return this;

	auto iter = m_child_nodes.begin();
	while (iter != m_child_nodes.end())
	{
		node * ret = (*iter)->find_node(pNodeName);
		if (ret != NULL)
			return ret;
		++iter;
	}
	return NULL;
}

nsmesh::node * nsmesh::node::find_node(uint32 node_id_)
{
	if (m_node_id == node_id_)
		return this;

	auto iter = m_child_nodes.begin();
	while (iter != m_child_nodes.end())
	{
		node * ret = (*iter)->find_node(node_id_);
		if (ret != NULL)
			return ret;
		++iter;
	}
	return NULL;
}

nsmesh::node * nsmesh::node::create_child(const nsstring & pName)
{
	node * childNode = new node(pName, this);
	m_child_nodes.push_back(childNode);
	return childNode;
}

nsmesh::node & nsmesh::node::operator=(node rhs)
{
	std::swap(m_name, rhs.m_name);
	std::swap(m_node_id, rhs.m_node_id);
	std::swap(m_node_tform, rhs.m_node_tform);
	std::swap(m_world_tform, rhs.m_world_tform);
	std::swap(m_parent_node, rhs.m_parent_node);
	std::swap(m_child_nodes, rhs.m_child_nodes);
	return *this;
}

nsmesh::joint::joint(): m_joint_id(0),
	m_offset_tform()
{}

nsmesh::node_tree::node_tree(): m_name_joint_map(),
	m_root(NULL)
{}

nsmesh::node_tree::node_tree(const node_tree & copy_):
	m_root(new node(*copy_.m_root))
{
	if (copy_.m_root != nullptr)
		m_root = new node(*(copy_.m_root));
}

nsmesh::node_tree & nsmesh::node_tree::operator=(node_tree copy_)
{
	std::swap(m_root,copy_.m_root);
	std::swap(m_name_joint_map, copy_.m_name_joint_map);
	return *this;
}

nsmesh::node * nsmesh::node_tree::find_node(const nsstring & pNodeName)
{
	if (m_root == NULL)
		return NULL;
	return m_root->find_node(pNodeName);
}

nsmesh::node * nsmesh::node_tree::find_node(uint32 node_id_)
{
	if (m_root == NULL)
		return NULL;
	return m_root->find_node(node_id_);
}

nsmesh::node_tree::~node_tree()
{
	delete m_root;
}

nsmesh::node * nsmesh::node_tree::create_root_node(const nsstring & pName)
{
	m_root = new node(pName,NULL);
	m_root->m_name = pName;
	return m_root;
}

nsmesh_plane::nsmesh_plane():
	nsmesh()
{}

nsmesh_plane::~nsmesh_plane()
{}

void nsmesh_plane::init()
{
	submesh * smsh = create();
	smsh->m_prim_type = GL_TRIANGLE_STRIP;
	smsh->init_gl();
	
	smsh->m_verts.push_back(fvec3(-1,-1,0));
	smsh->m_verts.push_back(fvec3(-1,1,0));
	smsh->m_verts.push_back(fvec3(1,-1,0));
	smsh->m_verts.push_back(fvec3(1,1,0));

	smsh->m_indices.push_back(0);
	smsh->m_indices.push_back(1);
	smsh->m_indices.push_back(2);
	smsh->m_indices.push_back(3);

	smsh->m_tex_coords.push_back(fvec2(0,0));
	smsh->m_tex_coords.push_back(fvec2(0,1));
	smsh->m_tex_coords.push_back(fvec2(1,0));
	smsh->m_tex_coords.push_back(fvec2(1,1));

	smsh->m_normals.push_back(fvec3(0,0,-1));
	smsh->m_normals.push_back(fvec3(0,0,-1));
	smsh->m_normals.push_back(fvec3(0,0,-1));
	smsh->m_normals.push_back(fvec3(0,0,-1));

	smsh->m_tangents.push_back(fvec3(0,1,0));
	smsh->m_tangents.push_back(fvec3(0,1,0));
	smsh->m_tangents.push_back(fvec3(0,1,0));
	smsh->m_tangents.push_back(fvec3(0,1,0));

	smsh->m_joints.resize(4);
	smsh->m_has_tex_coords = true;

	smsh->allocate_buffers();
}

void nsmesh::regenerate_node_ids()
{
	uint32 id = 0;
	if (m_node_tree->m_root != NULL)
	{
		m_node_tree->m_root->m_node_id = id++;
		_regen_node_ids(m_node_tree->m_root, id);
	}
}

void nsmesh::_regen_node_ids(node * node_, uint32 & id_)
{
	for (uint32 i = 0; i < node_->m_child_nodes.size(); ++i)
	{
		node_->m_child_nodes[i]->m_node_id = id_++;
		_regen_node_ids(node_->m_child_nodes[i], id_);
	}
}

void nsmesh_plane::set_dim(const fvec2 & dim_)
{
	submesh * smsh = sub(0);
	fvec2 hdim = dim_/2.0f;
	
	smsh->m_verts[0] = fvec3(-hdim.x,-hdim.y,0);
	smsh->m_verts[1] = fvec3(-hdim.x,hdim.y,0);
	smsh->m_verts[2] = fvec3(hdim.x,-hdim.y,0);
	smsh->m_verts[3] = fvec3(hdim.x,hdim.y,0);
	smsh->allocate_buffers();
}

uint32 nsmesh::node_count()
{
	if (m_node_tree->m_root == NULL)
		return 0;
	uint32 cnt = 0;
	_node_count(m_node_tree->m_root, cnt);
	return cnt;
}

uint32 nsmesh::joint_count()
{
	return m_node_tree->m_name_joint_map.size();
}

void nsmesh::_node_count(node * node_, uint32 & cnt)
{
	cnt += node_->m_child_nodes.size();
	for (uint32 i = 0; i < node_->m_child_nodes.size(); ++i)
		_node_count(node_->m_child_nodes[i], cnt);
}
