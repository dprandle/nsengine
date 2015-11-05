#ifndef NSMESH_H
#define NSMESH_H

#define JOINTS_PER_VERTEX 4

#include <nsresource.h>
#include <nsbuffer_object.h>
#include <nsvertex_array_object.h>


class nsmesh : public nsresource
{
public:
	template<class PUPer>
	friend void pup(PUPer & p, nsmesh & mesh);

	struct joint
	{
		joint();
		uint32 m_joint_id;
		fmat4 m_offset_tform;
	};

	struct node
	{
		node(const nsstring & pName="", node * pParentNode=NULL);
		~node();
		node * create_child(const nsstring & pName);
		node * find_node(const nsstring & pNodeName);

		nsstring m_name;
		uint32 m_node_id;
		fmat4 m_node_tform;
		fmat4 m_world_tform;
		node * m_parent_node;
		std::vector<node*> m_child_nodes;
	};

	struct node_tree
	{
		node_tree();
		~node_tree();
		node * create_root_node(const nsstring & pName);
		node * find_node(const nsstring & pNodeName);

		node * m_root;
		std::map<nsstring,nsmesh::joint> m_name_joint_map;
	};

	struct submesh
	{
		struct connected_joints
		{
			connected_joints();
			void add_joint(uint32 bone_id, float weight);

			uint32 m_joint_ids[JOINTS_PER_VERTEX];
			float m_weights[JOINTS_PER_VERTEX];
		};

		submesh(nsmesh * pParentMesh=NULL);
		~submesh();
		void allocate_buffers();
		void calc_aabb();
		void init_gl();
		void resize(uint32 pNewSize);
		void update_vao();


		nsbuffer_object m_vert_buf;
		nsbuffer_object m_tex_buf;
		nsbuffer_object m_norm_buf;
		nsbuffer_object m_tang_buf;
		nsbuffer_object m_indice_buf;
		nsbuffer_object m_joint_buf;
		nsvertex_array_object m_vao;

		fvec3_vector m_verts;
		fvec2_vector m_tex_coords;
		fvec3_vector m_normals;
		fvec3_vector m_tangents;
		ui_vector m_indices;
		uivec3_vector m_triangles;
		uivec2_vector m_lines;
		std::vector<connected_joints> m_joints;

		GLenum m_prim_type;
		node * m_node;
		nsstring m_name;
		nsmesh * m_parent_mesh;
		nsbounding_box m_bounding_box;
		bool m_has_tex_coords;
	};

	typedef std::vector<submesh*>::iterator submesh_iter;

	nsmesh();
	virtual ~nsmesh();

	const nsbounding_box & aabb();

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

	virtual void init();

	node_tree * tree();

	virtual void pup(nsfile_pupper * p);

	submesh * remove(submesh * submesh);

	submesh * remove(uint32 subindex);

	submesh * remove(const nsstring & subname);

	void transform_node(node * pParentNode, const fmat4 & pTransform);

	uint32 vert_count();

	uint32 vert_count(uint32 pIndex);

	uint32 indice_count();

	uint32 indice_count(uint32 pIndex);

	uint32 node_count();

	uint32 joint_count();

	/*!
	Calculate volume using divergence theorem method
	*/
	float volume();

private:
	void _node_count(node * child, uint32 & cnt);
	void _propagate_world_transform(node * node_);
	std::vector<submesh*> m_submeshes;
	node_tree* m_node_tree;
	nsbounding_box m_bounding_box;
};


// This is a special structure to hold a set of bone ids that are each weighted to show how much the bone
// affects the vertex - I have a BONES_PER_JOINT limit set to 4 bones - I dont think I would ever need
// more than 4 bones affecting a single vertex in any sort of animation
template <class PUPer>
void pup(PUPer & p, nsmesh::submesh::connected_joints & bwid, const nsstring & var_name)
{
	for (uint32 i = 0; i < JOINTS_PER_VERTEX; ++i)
	{
		pup(p, bwid.m_joint_ids[i], var_name + ".joint_ids[" + std::to_string(i) + "]");
		pup(p, bwid.m_weights[i], var_name + ".weights[" + std::to_string(i) + "]");
	}
}

// This simply pups the submesh data - not much to say here - most of these attributes are std::vectors of vec3 or vec2 or uint32s
// The prim type is just GL_TRIANGLES or GL_LINES etc - and the BoneInfo is a std::vector of BoneWeightIDs
template <class PUPer>
void pup(PUPer & p, nsmesh::submesh & sm, const nsstring & var_name)
{
	pup(p, sm.m_verts, var_name + ".verts");
	pup(p, sm.m_tex_coords, var_name + ".tex_coords");
	pup(p, sm.m_normals, var_name + ".normals");
	pup(p, sm.m_tangents, var_name + ".tangents");
	pup(p, sm.m_indices, var_name + ".indices");
	pup(p, sm.m_triangles, var_name + ".triangles");
	pup(p, sm.m_lines, var_name + ".lines");
	pup(p, sm.m_joints, var_name + ".joints");
	pup(p, sm.m_prim_type, var_name + ".prim_type");
	pup(p, sm.m_has_tex_coords, var_name + ".has_tex_coords");
	pup(p, sm.m_name, var_name + ".name");
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
void pup(PUPer & p, nsmesh::joint & bone, const nsstring & var_name)
{
	pup(p, bone.m_joint_id, var_name + ".joint_id");
	pup(p, bone.m_offset_tform, var_name + ".offset_tform");
}

// Pup a node and all of its child nodes
template <class PUPer>
void pup(PUPer & p, nsmesh::node & node, const nsstring & var_name)
{
	pup(p, node.m_name, var_name + ".name");
	pup(p, node.m_node_id, var_name + ".node_id");
	pup(p, node.m_node_tform, var_name + ".node_tform");
	pup(p, node.m_world_tform, var_name + ".world_tform");
	pup(p, node.m_child_nodes, var_name + ".child_nodes");
	for (uint32 i = 0; i < node.m_child_nodes.size(); ++i)
		node.m_child_nodes[i]->m_parent_node = &node;
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
	pup(p, node_tree.m_name_joint_map, var_name + ".name_joint__map");
	bool has_root = (node_tree.m_root != NULL);
	pup(p, has_root, var_name + ".has_root"); // has_root will be saved or overwritten depending on saving or loading
	if (has_root)
		pup(p, node_tree.m_root, var_name);
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
		if (mesh.m_submeshes[i]->m_node != NULL)
			node_name = mesh.m_submeshes[i]->m_node->m_name;
		pup(p, node_name, "node_name");
		mesh.m_submeshes[i]->m_node = mesh.m_node_tree->find_node(node_name);
		mesh.m_submeshes[i]->m_parent_mesh = &mesh;
	}
	mesh.calc_aabb();
	mesh.init_gl();
	mesh.allocate();
}


class nsmesh_plane : public nsmesh
{
  public:	
	nsmesh_plane();
	~nsmesh_plane();

	void init();
	void set_dim(const fvec2 & dimensions_);
};


#endif
