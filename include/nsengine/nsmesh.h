/* ----------------------------- Noble Steed Engine----------------------------*
Authors : Daniel Randle, Alex Bourne
Date Created : Mar 7 2013

File:
	nsmesh.h

Description:
	This file contains the nsmesh class which can be used to load meshes in from file
	and draw them using their attached shader
*---------------------------------------------------------------------------*/

#ifndef NSMESH_H
#define NSMESH_H


#include <myGL/glew.h>
#include <nsglobal.h>
#include <vector>
#include <nsmaterial.h>
#include <nsmath.h>

/* ---------------------------------Class nsmesh-----------------------------*

Description:
	nsmesh will load a mesh from file along with any materials that it may use.
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

class nsmesh : public nsresource
{
public:
	template<class PUPer>
	friend void pup(PUPer & p, nsmesh & mesh);

	struct bone
	{
		bone();
		uint32 boneID;
		fmat4 mOffsetTransform;
	};

	struct node
	{
		node(const nsstring & pName="", node * pParentNode=NULL);
		~node();
		node * create_child(const nsstring & pName);
		node * find_node(const nsstring & pNodeName);

		nsstring name;
		uint32 node_id;
		fmat4 node_transform;
		fmat4 world_transform;
		node * parent_node;
		std::vector<node*> child_nodes;
	};

	struct node_tree
	{
		node_tree();
		~node_tree();
		node * create_root_node(const nsstring & pName);
		node * find_node(const nsstring & pNodeName);

		node * root_node;
		std::map<nsstring,nsmesh::bone> bone_name_map;
	};

	struct submesh
	{
		struct joint
		{
			joint();
			void add_bone(uint32 bone_id, float weight);

			uint32 bone_ids[BONES_PER_VERTEX];
			float weights[BONES_PER_VERTEX];
		};

		submesh(nsmesh * pParentMesh=NULL);
		~submesh();
		void allocate_buffers();
		void calc_aabb();
		void init_gl();
		void resize(uint32 pNewSize);
		void update_VAO();


		nsbuffer_object pos_buf;
		nsbuffer_object tex_buf;
		nsbuffer_object norm_buf;
		nsbuffer_object tang_buf;
		nsbuffer_object indice_buf;
		nsbuffer_object joint_buf;
		nsvertex_array_object vao;

		fvec3array positions;
		fvec2array tex_coords;
		fvec3array normals;
		fvec3array tangents;
		uint32array indices;
		uivec3array triangles;
		uivec2array lines;
		std::vector<joint> joints;

		GLenum primitive_type;
		node * node_;
		nsstring name;
		nsmesh * parent_mesh;
		NSBoundingBox bounding_box;
		bool has_tex_coords;
	};

	typedef std::vector<submesh*>::iterator submesh_iter;

	nsmesh();
	~nsmesh();

	const NSBoundingBox & aabb();

	bool add(submesh * submesh);

	void allocate();

	void allocate(uint32 subindex);

	void bake();

	void bake_rotation(const fquat & rotation);

	void bake_scaling(const fvec3 & scale);

	void bake_translation(const fvec3 & tranlation);

	void bake_rotation(uint32 subindex, const fquat & rotation);

	void bake_scaling(uint32 subindex, const fvec3 & scale);

	void bake_translation(uint32 subindex, const fvec3 & tranlation);

	void bake_node_rotation(const fquat & rotation);

	void bake_node_scaling(const fvec3 & scale);

	void bake_node_translation(const fvec3 & tranlation);

	void calc_aabb();

	uint32 count();

	bool contains(submesh * submesh);

	bool contains(uint32 subindex);

	bool contains(const nsstring & subname);
	
	submesh * create();

	bool del();

	bool del(submesh * submesh);

	bool del(uint32 subindex);

	bool del(const nsstring & subname);

	uint32 find(submesh * sub);

	uint32 find(const nsstring & subname);

	void flip_normals();

	void flip_normals(uint32 subindex);

	void flip_uv();

	void flip_uv(uint32 pSubIndex);

	void init_gl();

	void init_gl(uint32 subindex);

	submesh * sub(const nsstring & pName);

	submesh * sub(uint32 pIndex);

	void init();

	node_tree * tree();

	virtual void pup(NSFilePUPer * p);

	submesh * remove(submesh * submesh);

	submesh * remove(uint32 subindex);

	submesh * remove(const nsstring & subname);

	void transform_node(node * pParentNode, const fmat4 & pTransform);

	uint32 vert_count();

	uint32 vert_count(uint32 pIndex);

	/*!
	Calculate volume using divergence theorem method
	*/
	float volume();

private:
	void _propagate_world_transform(node * child_node);
	std::vector<submesh*> m_submeshes;
	node_tree* m_node_tree;
	NSBoundingBox m_bounding_box;
};

// This is a special structure to hold a set of bone ids that are each weighted to show how much the bone
// affects the vertex - I have a BONES_PER_VERTEX limit set to 4 bones - I dont think I would ever need
// more than 4 bones affecting a single vertex in any sort of animation
template <class PUPer>
void pup(PUPer & p, nsmesh::submesh::joint & bwid, const nsstring & var_name)
{
	for (uint32 i = 0; i < BONES_PER_VERTEX; ++i)
	{
		pup(p, bwid.bone_ids[i], var_name + ".bone_ids[" + std::to_string(i) + "]");
		pup(p, bwid.weights[i], var_name + ".weights[" + std::to_string(i) + "]");
	}
}

// This simply pups the submesh data - not much to say here - most of these attributes are std::vectors of vec3 or vec2 or uint32s
// The prim type is just GL_TRIANGLES or GL_LINES etc - and the BoneInfo is a std::vector of BoneWeightIDs
template <class PUPer>
void pup(PUPer & p, nsmesh::submesh & sm, const nsstring & var_name)
{
	pup(p, sm.positions, var_name + ".positions");
	pup(p, sm.tex_coords, var_name + ".tex_coords");
	pup(p, sm.normals, var_name + ".normals");
	pup(p, sm.tangents, var_name + ".tangents");
	pup(p, sm.indices, var_name + ".indices");
	pup(p, sm.triangles, var_name + ".triangles");
	pup(p, sm.lines, var_name + ".lines");
	pup(p, sm.joints, var_name + ".joints");
	pup(p, sm.primitive_type, var_name + ".primitive_type");
	pup(p, sm.has_tex_coords, var_name + ".has_tex_coords");
	pup(p, sm.name, var_name + ".name");
}

// This handles a pointer to a submesh - if the pupper is PUP_IN (reading) then allocate memory for the submesh
// and then pup what it points to
template <class PUPer>
void pup(PUPer & p, nsmesh::submesh * & sm, const nsstring & var_name)
{
	if (p.mode() == PUP_IN)
		sm = new nsmesh::submesh();		
	pup(p, *sm, var_name);
}

// Pup a bone - a bone just has an id and an offset transform to mess with the vertices its connected to
template <class PUPer>
void pup(PUPer & p, nsmesh::bone & bone, const nsstring & var_name)
{
	pup(p, bone.boneID, var_name + ".bone_id");
	pup(p, bone.mOffsetTransform, var_name + ".mOffsetTransform");
}

// Pup a node and all of its child nodes
template <class PUPer>
void pup(PUPer & p, nsmesh::node & node, const nsstring & var_name)
{
	pup(p, node.name, var_name + ".name");
	pup(p, node.node_id, var_name + ".node_id");
	pup(p, node.node_transform, var_name + ".node_transform");
	pup(p, node.world_transform, var_name + ".world_transform");
	pup(p, node.child_nodes, var_name + ".child_nodes");
	for (uint32 i = 0; i < node.child_nodes.size(); ++i)
		node.child_nodes[i]->parent_node = &node;
}

// Allocate memory for a node if the pupper is set to PUP_IN
template <class PUPer>
void pup(PUPer & p, nsmesh::node * & node_ptr, const nsstring & var_name)
{
	if (p.mode() == PUP_IN)
		node_ptr = new nsmesh::node();
	pup(p, *node_ptr, var_name);
}

// Pup a node tree - if a mesh has a root node (meaning it has submeshes with transforms relative to the base node as each submesh is given a node)
// then pup the root node - the above pupper handles allocating the memory
template <class PUPer>
void pup(PUPer & p, nsmesh::node_tree & node_tree, const nsstring & var_name)
{
	pup(p, node_tree.bone_name_map, var_name + ".bone_name_map");
	bool has_root = (node_tree.root_node != NULL);
	pup(p, has_root, var_name + ".has_root"); // has_root will be saved or overwritten depending on saving or loading
	if (has_root)
		pup(p, node_tree.root_node, var_name);
}

// Pup a mesh - pup the node tree and all of the submeshes - then go through the submeshes
// and assign the "parent" pointer (which should be the owning mesh) and assign the correct node
// if the mesh has a node structure (simple meshes wont)
// Also assign a debug object to it if debug mode is enabled - the debug object logs stuff and sends stuff to screen displays etc
template <class PUPer>
void pup(PUPer & p, nsmesh & mesh)
{
	pup(p, *mesh.m_node_tree, "node_tree");
	pup(p, mesh.m_submeshes, "submeshes");
	for (uint32 i = 0; i < mesh.m_submeshes.size(); ++i)
	{
		nsstring node_name = "";
		if (mesh.m_submeshes[i]->node_ != NULL)
			node_name = mesh.m_submeshes[i]->node_->name;
		pup(p, node_name, "node_name");
		mesh.m_submeshes[i]->node_ = mesh.m_node_tree->find_node(node_name);
		mesh.m_submeshes[i]->parent_mesh = &mesh;
	}
	mesh.calc_aabb();
	mesh.init_gl();
	mesh.allocate();
}

#endif
