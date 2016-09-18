/*!
  \file nsgl_vid_objs.cpp

  \brief Definition file for nsgl_vid_objs class

  This file contains all of the neccessary definitions for the nsgl_vid_objs stuff.

  \author Daniel Randle
  \date Feb 23 2016
  \copywrite Earth Banana Games 2013
*/

#include <nstimer.h>
#include <asset/nsentity.h>
#include <opengl/nsgl_vid_objs.h>
#include <opengl/nsgl_shader.h>
#include <opengl/nsgl_buffer.h>
#include <opengl/nsgl_vao.h>
#include <opengl/nsgl_xfb.h>
#include <opengl/nsgl_texture.h>
#include <asset/nsshader.h>
#include <asset/nstexture.h>
#include <component/nstform_comp.h>
#include <component/nsparticle_comp.h>
#include <component/nssel_comp.h>
#include <nsengine.h>
#include <opengl/nsgl_driver.h>

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
	dprint("nsgl_shader_obj::update - updating video info for shader " + shdr->name());
	for (uint32 i = 0; i < shdr->shader_stages.size(); ++i)
	{
        if (!shdr->shader_stages[i].source.empty())
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
	}
	gl_shdr->link();
	needs_update = false;
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
	gl_tex->bind();
	if (tex->type() == type_to_hash(nstex1d))
	{
		nstex1d * tex1d = (nstex1d*)tex;
		gl_tex->allocate_1d(
			tex1d->data(),
			tex1d->format(),
			tex1d->component_data_type(),
			tex1d->size(),
			tex1d->compress_on_upload(),
			tex1d->compressed_size());
	}
	else if (tex->type() == type_to_hash(nstex2d))
	{
		nstex2d * tex2d = (nstex2d*)tex;
		gl_tex->allocate_2d(
			tex2d->data(),
			tex2d->format(),
			tex2d->component_data_type(),
            tex2d->size(),
			tex2d->compress_on_upload(),
			tex2d->compressed_size());
	}
	else if (tex->type() == type_to_hash(nstex3d))
	{
		nstex3d * tex3d = (nstex3d*)tex;
		gl_tex->allocate_3d(
			tex3d->data(),
			tex3d->format(),
			tex3d->component_data_type(),
			tex3d->size(),
			tex3d->compress_on_upload(),
			tex3d->compressed_size());
	}
	else if (tex->type() == type_to_hash(nstex_cubemap))
	{
		nstex_cubemap * texcube = (nstex_cubemap*)tex;
		for (uint8 i = 0; i < 6; ++i)
		{
			uint8 * data_ptr = texcube->data();
			if (data_ptr != nullptr)
				data_ptr += texcube->size().x*texcube->size().y*texcube->bytes_per_pixel()*i;

			gl_tex->allocate_2d(
				data_ptr,
				texcube->format(),
				texcube->component_data_type(),
				texcube->size(),
				texcube->compress_on_upload(),
				texcube->compressed_size(),
				i);	
		}
	}
	else
	{
		dprint("nsgl_texture_obj::nsgl_texture_obj - Unrecognized texture type");
	}

	if (tex->mipmap_autogen())
		gl_tex->generate_mipmaps();
    gl_tex->set_parameters(tex->parameters());
	gl_tex->unbind();
	needs_update = false;
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

    gl_indice_buf->target = nsgl_buffer::element_array;
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
	needs_update = false;
}

nsgl_tform_comp_obj::nsgl_tform_comp_obj(nsvideo_object * parent_):
	nsvid_obj(parent_),
	gl_tform_buffer(new nsgl_buffer()),
	gl_tform_id_buffer(new nsgl_buffer()),
	last_size(0)
{
	gl_tform_buffer->target = nsgl_buffer::array;
	gl_tform_id_buffer->target = nsgl_buffer::array;
	gl_tform_buffer->init();
	gl_tform_id_buffer->init();
}

nsgl_tform_comp_obj::~nsgl_tform_comp_obj()
{
	gl_tform_buffer->release();
	gl_tform_id_buffer->release();
	delete gl_tform_buffer;
	delete gl_tform_id_buffer;
}

void nsgl_tform_comp_obj::update()
{
	tform_per_scene_info * psi = (tform_per_scene_info*)parent;
	bool did_resize = false;
    if (psi->shared_geom_tforms.size() != last_size)
    {
		did_resize = true;
		gl_tform_buffer->bind();
		gl_tform_buffer->allocate<fmat4>(psi->shared_geom_tforms.size(), nullptr, nsgl_buffer::mutable_dynamic_draw);
		gl_tform_id_buffer->bind();
		gl_tform_id_buffer->allocate<uint32>(psi->shared_geom_tforms.size(), nullptr, nsgl_buffer::mutable_dynamic_draw);
		last_size = psi->shared_geom_tforms.size();
    }

	gl_tform_buffer->bind();
    fmat4 * mappedT = gl_tform_buffer->map<fmat4>(nsgl_buffer::write_only);

	gl_tform_id_buffer->bind();
    uint32 * mappedI = gl_tform_id_buffer->map<uint32>(nsgl_buffer::write_only);
	gl_tform_id_buffer->unbind();

	psi->visible_count = 0;
	for (uint32 i = 0; i < psi->shared_geom_tforms.size(); ++i)
	{
		nstform_comp * itf = psi->shared_geom_tforms[i];
		int32 state = itf->hidden_state();
		bool layerBit = (state & nstform_comp::hide_layer) == nstform_comp::hide_layer;
		bool objectBit = (state & nstform_comp::hide_object) == nstform_comp::hide_object;
		bool showBit = (state & nstform_comp::hide_none) == nstform_comp::hide_none;
		bool hideBit = (state & nstform_comp::hide_all) == nstform_comp::hide_all;

        if (!hideBit && (!layerBit && (showBit || !objectBit)))
        {
			if (itf->render_update() || did_resize)
			{
				mappedT[psi->visible_count] = itf->world_tf();
				mappedI[psi->visible_count] = itf->owner()->id();
				itf->set_render_update(false);
			}
			++psi->visible_count;
        }
	}
	gl_tform_buffer->bind();
	gl_tform_buffer->unmap();
	gl_tform_id_buffer->bind();
	gl_tform_id_buffer->unmap();
	gl_tform_id_buffer->unbind();
	psi->needs_update = false;
	needs_update = false;
}

nsgl_particle_comp_obj::nsgl_particle_comp_obj(nsvideo_object * parent_):
	nsvid_obj(parent_),
	last_size(0),
	buffer_index(0),
	gl_front_buffer(new nsgl_buffer()),
	gl_back_buffer(new nsgl_buffer())
{
	nsparticle_comp * pcomp = (nsparticle_comp*)parent;
	
	gl_xfbs[0] = new nsgl_xfb();
	gl_xfbs[1] = new nsgl_xfb();
	gl_xfbs[0]->init();
	gl_xfbs[1]->init();

	gl_vaos[0] = new nsgl_vao();
	gl_vaos[1] = new nsgl_vao();
	gl_vaos[0]->init();
	gl_vaos[1]->init();
	
	gl_front_buffer->init();
	gl_back_buffer->init();

	last_size = pcomp->max_particles();
	// Create an array of particles to allocate the buffers
	std::vector<particle> particles;
	particles.resize(pcomp->max_particles());
	particles[0].age_type_reserved.y = 1.0f;

	gl_vaos[0]->bind();

	gl_back_buffer->target = nsgl_buffer::array;
	gl_back_buffer->bind();
	gl_back_buffer->allocate(particles,nsgl_buffer::mutable_dynamic_draw);
	
	gl_vaos[0]->enable(0);
	gl_vaos[0]->vertex_attrib_ptr(0, 4, GL_FLOAT, GL_FALSE, sizeof(particle), 0);
	gl_vaos[0]->enable(1);
	gl_vaos[0]->vertex_attrib_ptr(1, 4, GL_FLOAT, GL_FALSE, sizeof(particle), sizeof(fvec4));
	gl_vaos[0]->enable(2);
	gl_vaos[0]->vertex_attrib_ptr(2, 4, GL_FLOAT, GL_FALSE, sizeof(particle), sizeof(fvec4) * 2);
	gl_vaos[0]->enable(3);
	gl_vaos[0]->vertex_attrib_ptr(3, 4, GL_FLOAT, GL_FALSE, sizeof(particle), sizeof(fvec4) * 3);
	gl_vaos[0]->unbind();

	gl_vaos[1]->bind();
	gl_front_buffer->target = nsgl_buffer::array;
	gl_front_buffer->bind();
	gl_front_buffer->allocate(particles,nsgl_buffer::mutable_dynamic_draw);

	gl_vaos[1]->enable(0);
	gl_vaos[1]->vertex_attrib_ptr(0, 4, GL_FLOAT, GL_FALSE, sizeof(particle), 0);
	gl_vaos[1]->enable(1);
	gl_vaos[1]->vertex_attrib_ptr(1, 4, GL_FLOAT, GL_FALSE, sizeof(particle), sizeof(fvec4));
	gl_vaos[1]->enable(2);
	gl_vaos[1]->vertex_attrib_ptr(2, 4, GL_FLOAT, GL_FALSE, sizeof(particle), sizeof(fvec4) * 2);
	gl_vaos[1]->enable(3);
	gl_vaos[1]->vertex_attrib_ptr(3, 4, GL_FLOAT, GL_FALSE, sizeof(particle), sizeof(fvec4) * 3);
	gl_vaos[1]->unbind();

	gl_front_buffer->target = nsgl_buffer::transform_feedback;
	gl_front_buffer->target_index = 0;
	gl_back_buffer->target = nsgl_buffer::transform_feedback;
	gl_back_buffer->target_index = 0;

	gl_xfbs[0]->prim_mode = nsgl_xfb::gl_points;
	gl_xfbs[1]->prim_mode = nsgl_xfb::gl_points;

	gl_xfbs[0]->bind();
	gl_front_buffer->bind();
	gl_xfbs[0]->unbind();

	gl_xfbs[1]->bind();
	gl_back_buffer->bind();
	gl_xfbs[1]->unbind();	
}

nsgl_particle_comp_obj::~nsgl_particle_comp_obj()
{
	gl_front_buffer->release();
	gl_back_buffer->release();
	delete gl_front_buffer;
	delete gl_back_buffer;
	for (uint8 i = 0; i < 2; ++i)
	{
		gl_xfbs[i]->release();
		delete gl_xfbs[i];
		gl_vaos[i]->release();
		delete gl_vaos[i];
	}
}

void nsgl_particle_comp_obj::update()
{
	nsparticle_comp * comp = (nsparticle_comp *)parent;
	if (comp->simulating())
        comp->elapsed() += double(nse.timer()->fixed());

	if (comp->elapsed() * 1000 >= comp->lifetime())
	{
		comp->reset();
		comp->enable_simulation(comp->looping());
	}

	nsshader * pshader = get_asset<nsshader>(comp->shader_id());
	if (pshader == nullptr)
	{
		dprint("nsgl_particle_comp_obj::update No processing shader set in component - skipping");
		return;
	}
	nsgl_shader * pshdr = pshader->video_obj<nsgl_shader_obj>()->gl_shdr;
	
	if (pshdr->error_state != nsgl_shader::error_none)
	{
		dprint("nsgl_particle_comp_obj::update Comp shader error set to " + std::to_string(pshdr->error_state) + " in entity \"" + comp->owner()->name() + "\"");
		return;
	}

	pshdr->bind();
	nstexture * texRand = get_asset<nstexture>(comp->rand_tex_id());
	if (texRand != nullptr)
	{
		nsgl_texture * gltex = texRand->video_obj<nsgl_texture_obj>()->gl_tex;
		nse.video_driver<nsgl_driver>()->current_context()->set_active_texture_unit(RAND_TEX_UNIT);
		gltex->bind();
	}
		
	pshdr->set_uniform("randomTex", RAND_TEX_UNIT);
	pshdr->set_uniform("dt", float(nse.timer()->fixed()));
	pshdr->set_uniform("timeElapsed", comp->elapsed());
	pshdr->set_uniform("lifetime", comp->lifetime());
	pshdr->set_uniform("launchFrequency", float(comp->emission_rate()));
	pshdr->set_uniform("angVelocity", comp->angular_vel());
	pshdr->set_uniform("motionGlobal", comp->motion_global_time());
	pshdr->set_uniform("visualGlobal", comp->visual_global_time());
	pshdr->set_uniform("interpolateMotion", comp->motion_key_interpolation());
	pshdr->set_uniform("interpolateVisual", comp->visual_key_interpolation());
	pshdr->set_uniform("startingSize", comp->starting_size());
	pshdr->set_uniform("emitterSize", comp->emitter_size());
	pshdr->set_uniform("emitterShape", comp->emitter_shape());
	pshdr->set_uniform("initVelocityMult", comp->init_vel_mult());
	pshdr->set_uniform("motionKeyType", uint32(comp->motion_key_type()));

	uint32 index = 0;
	ui_fvec3_map::const_iterator keyIter = comp->motion_keys().begin();
	while (keyIter != comp->motion_keys().end())
	{
		pshdr->set_uniform(
			"forceKeys[" + std::to_string(index) + "].time",
			float(keyIter->first) / float(comp->max_motion_keys() * 1000) * float(comp->lifetime()));
		pshdr->set_uniform(
			"forceKeys[" + std::to_string(index) + "].force",
			keyIter->second);
		++index;
		++keyIter;
	}

	index = 0;
	keyIter = comp->visual_keys().begin();
	while (keyIter != comp->visual_keys().end())
	{
		pshdr->set_uniform(
			"sizeKeys[" + std::to_string(index) + "].time",
			float(keyIter->first) / float(comp->max_visual_keys() * 1000) * float(comp->lifetime()));
		pshdr->set_uniform(
			"sizeKeys[" + std::to_string(index) + "].sizeVel",
			fvec2(keyIter->second.x, keyIter->second.y));
		pshdr->set_uniform(
			"sizeKeys[" + std::to_string(index) + "].alpha",
			keyIter->second.z);
		++index;
		++keyIter;
	}
		
	gl_xfbs[buffer_index]->bind();
	gl_vaos[buffer_index]->bind();
	gl_xfbs[buffer_index]->begin();
	
    glEnable(GL_RASTERIZER_DISCARD);
	if (comp->first())
	{
		glDrawArrays(GL_POINTS, 0, 1);
		gl_err_check("nsparticle_system::update in glDrawArrays");
		comp->set_first(false);
	}
	else
	{
		glDrawTransformFeedback(GL_POINTS, gl_xfbs[1-buffer_index]->gl_id);
		gl_err_check("nsparticle_system::update in glDrawTransformFeedback");
	}
	glDisable(GL_RASTERIZER_DISCARD);
	
	gl_xfbs[buffer_index]->end();
	gl_vaos[buffer_index]->unbind();
	gl_xfbs[buffer_index]->unbind();
	buffer_index = 1 - buffer_index;
	needs_update = false;
}
