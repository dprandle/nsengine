/*!
  \file nsgl_vid_objs.cpp

  \brief Definition file for nsgl_vid_objs class

  This file contains all of the neccessary definitions for the nsgl_vid_objs stuff.

  \author Daniel Randle
  \date Feb 23 2016
  \copywrite Earth Banana Games 2013
*/

#include <nsgl_vid_objs.h>
#include <nsgl_shader.h>
#include <nsgl_buffer.h>
#include <nsgl_vao.h>
#include <nsgl_xfb.h>
#include <nsgl_texture.h>
#include <nsshader.h>
#include <nstexture.h>
#include <nstform_comp.h>
#include <nsparticle_comp.h>
#include <nssel_comp.h>
#include <nsengine.h>

nsgl_shader_obj::nsgl_shader_obj(nsvideo_object * parent_):
	nsvid_obj(parent_),
	gl_shdr(new nsgl_shader())
{
	gl_shdr->init();
}

nsgl_shader_obj::~nsgl_shader_obj()
{
	gl_shdr->release();
	delete gl_shdr;
}

void nsgl_shader_obj::update()
{
	nsshader * shdr = (nsshader*)parent;
	for (uint32 i = 0; i < shdr->shader_stages.size(); ++i)
	{
		if (shdr->shader_stages[i].name == "fragment")
			gl_shdr->compile(nsgl_shader::fragment_shader, shdr->shader_stages[i].source);
		else if (shdr->shader_stages[i].name == "vertex")
			gl_shdr->compile(nsgl_shader::vertex_shader, shdr->shader_stages[i].source);
		else if (shdr->shader_stages[i].name == "geometry")
			gl_shdr->compile(nsgl_shader::geometry_shader, shdr->shader_stages[i].source);
		else if (shdr->shader_stages[i].name == "tess_control")
			gl_shdr->compile(nsgl_shader::tess_control_shader, shdr->shader_stages[i].source);
		else if (shdr->shader_stages[i].name == "tess_evaluation")
			gl_shdr->compile(nsgl_shader::tess_evaluation_shader, shdr->shader_stages[i].source);
		else if (shdr->shader_stages[i].name == "compute")
			gl_shdr->compile(nsgl_shader::compute_shader, shdr->shader_stages[i].source);
		else
		{
			dprint("nsgl_shader_obj::update Could not compile stage with name " + shdr->shader_stages[i].name);
		}
	}
	gl_shdr->link();
}

nsgl_texture_obj::nsgl_texture_obj(nsvideo_object * parent_):
	nsvid_obj(parent_),
	gl_tex(new nsgl_texture())
{
	nstexture * tex = (nstexture*)parent;	
	gl_tex->init();
	if (tex->type() == type_to_hash(nstex1d))
	{
		gl_tex->target = nsgl_texture::tex_1d;
	}
	else if (tex->type() == type_to_hash(nstex2d))
	{
		gl_tex->target = nsgl_texture::tex_2d;
		gl_tex->set_parameter_i(nsgl_texture::mag_filter, GL_LINEAR);
		gl_tex->set_parameter_i(nsgl_texture::min_filter, GL_LINEAR_MIPMAP_LINEAR);
	}
	else if (tex->type() == type_to_hash(nstex3d))
	{
		gl_tex->target = nsgl_texture::tex_3d;
	}
	else if (tex->type() == type_to_hash(nstex_cubemap))
	{
		gl_tex->target = nsgl_texture::tex_cubemap;
	}
	else
	{
		dprint("nsgl_texture_obj::nsgl_texture_obj - Unrecognized texture type");
	}
}

nsgl_texture_obj::~nsgl_texture_obj()
{
	gl_tex->release();
	delete gl_tex;
}

void nsgl_texture_obj::update()
{
	nstexture * tex = (nstexture*)parent;
	
}

nsgl_submesh_obj::nsgl_submesh_obj(nsvideo_object * parent_):
	nsvid_obj(parent_),
	gl_vao(new nsgl_vao()),
	gl_vert_buf(new nsgl_buffer()),
	gl_tex_buf(new nsgl_buffer()),
	gl_norm_buf(new nsgl_buffer()),
	gl_tang_buf(new nsgl_buffer()),
	gl_indice_buf(new nsgl_buffer()),
	gl_joint_buf(new nsgl_buffer())
{
	gl_vert_buf->target = nsgl_buffer::array;
	gl_vert_buf->init();

	gl_tex_buf->target = nsgl_buffer::array;
	gl_tex_buf->init();

	gl_norm_buf->target = nsgl_buffer::array;
	gl_norm_buf->init();

	gl_tang_buf->target = nsgl_buffer::array;
	gl_tang_buf->init();

	gl_joint_buf->target = nsgl_buffer::array;
	gl_joint_buf->init();

	gl_indice_buf->target = nsgl_buffer::array;
	gl_indice_buf->init();

	gl_vao->init();
}

nsgl_submesh_obj::~nsgl_submesh_obj()
{
	gl_vert_buf->release();
	gl_tex_buf->release();
	gl_norm_buf->release();
	gl_tang_buf->release();
	gl_joint_buf->release();
	gl_indice_buf->release();

	delete gl_vao;
	delete gl_vert_buf;
	delete gl_tex_buf;
	delete gl_norm_buf;
	delete gl_tang_buf;
	delete gl_joint_buf;
	delete gl_indice_buf;
	delete gl_vao;	
}

void nsgl_submesh_obj::update()
{
	nsmesh::submesh * subm = static_cast<nsmesh::submesh*>(parent);
	
	gl_vert_buf->bind();
	gl_vert_buf->allocate(subm->m_verts, nsgl_buffer::mutable_static_draw);
	
	gl_tex_buf->bind();
	gl_tex_buf->allocate(subm->m_tex_coords,nsgl_buffer::mutable_static_draw);

	gl_norm_buf->bind();
	gl_norm_buf->allocate(subm->m_normals,nsgl_buffer::mutable_static_draw);

	gl_tang_buf->bind();
	gl_tang_buf->allocate(subm->m_tangents,nsgl_buffer::mutable_static_draw);

	gl_joint_buf->bind();
	gl_joint_buf->allocate(subm->m_joints,nsgl_buffer::mutable_static_draw);

	gl_indice_buf->bind();
	gl_indice_buf->allocate(subm->m_indices,nsgl_buffer::mutable_static_draw);

	gl_vao->bind();
	gl_vert_buf->bind();
	gl_vao->add(gl_vert_buf, nsgl_shader::loc_position);
	gl_vao->vertex_attrib_ptr(nsgl_shader::loc_position, 3, GL_FLOAT, GL_FALSE, sizeof(fvec3), 0);

	gl_tex_buf->bind();
	gl_vao->add(gl_tex_buf, nsgl_shader::loc_tex_coords);
	gl_vao->vertex_attrib_ptr(nsgl_shader::loc_tex_coords, 2, GL_FLOAT, GL_FALSE, sizeof(fvec2), 0);

	gl_norm_buf->bind();
	gl_vao->add(gl_norm_buf, nsgl_shader::loc_normal);
	gl_vao->vertex_attrib_ptr(nsgl_shader::loc_normal, 3, GL_FLOAT, GL_FALSE, sizeof(fvec3), 0);

	gl_tang_buf->bind();
	gl_vao->add(gl_tang_buf, nsgl_shader::loc_tangent);
	gl_vao->vertex_attrib_ptr(nsgl_shader::loc_tangent, 3, GL_FLOAT, GL_FALSE, sizeof(fvec3), 0);

	gl_joint_buf->bind();
	gl_vao->add(gl_joint_buf, nsgl_shader::loc_bone_id);
	gl_vao->add(gl_joint_buf, nsgl_shader::loc_joint);
	gl_vao->vertex_attrib_I_ptr(
		nsgl_shader::loc_bone_id,
		4,
		GL_INT,
		sizeof(nsmesh::submesh::connected_joints),
		0);
	gl_vao->vertex_attrib_ptr(
		nsgl_shader::loc_joint,
		4,
		GL_FLOAT,
		GL_FALSE,
		sizeof(nsmesh::submesh::connected_joints),
		4 * sizeof(uint32));
	gl_indice_buf->bind();
	gl_vao->unbind();
}

nsgl_tform_comp_obj::nsgl_tform_comp_obj(nsvideo_object * parent_):
	nsvid_obj(parent_)
{
	
}

nsgl_tform_comp_obj::~nsgl_tform_comp_obj()
{
	
}

void nsgl_tform_comp_obj::update()
{
	
}

nsgl_sel_comp_obj::nsgl_sel_comp_obj(nsvideo_object * parent_):
	nsvid_obj(parent_)
{
	
}

nsgl_sel_comp_obj::~nsgl_sel_comp_obj()
{
	
}

void nsgl_sel_comp_obj::update()
{
	
}

nsgl_particle_comp_obj::nsgl_particle_comp_obj(nsvideo_object * parent_):
	nsvid_obj(parent_)
{
	
}

nsgl_particle_comp_obj::~nsgl_particle_comp_obj()
{
	
}

void nsgl_particle_comp_obj::update()
{
	
}
