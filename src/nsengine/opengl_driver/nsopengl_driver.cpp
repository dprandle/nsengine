/*!
  \file nsopengl_driver.cpp

  \brief Definition file for nsopengl_driver class

  This file contains all of the neccessary definitions for the nsopengl_driver class.

  \author Daniel Randle
  \date Feb 23 2016
  \copywrite Earth Banana Games 2013
*/

#include <nsfont.h>
#include <nsanim_comp.h>
#include <nsterrain_comp.h>
#include <nssel_comp.h>
#include <nsplugin.h>
#include <nsrender_system.h>
#include <nsopengl_driver.h>
#include <nsshader.h>
#include <nsshadowbuf_object.h>
#include <nsgbuf_object.h>
#include <nsmaterial.h>
#include <nsscene.h>
#include <nscam_comp.h>
#include <nsshader_manager.h>
#include <nslight_comp.h>
#include <nsrender_comp.h>
#include <nsgl_texture.h>

translucent_buffers::translucent_buffers():
	atomic_counter(new nsbuffer_object(nsbuffer_object::atomic_counter, nsbuffer_object::storage_mutable)),
	header(new nsbuffer_object(nsbuffer_object::shader_storage, nsbuffer_object::storage_mutable)),
	fragments(new nsbuffer_object(nsbuffer_object::shader_storage, nsbuffer_object::storage_mutable)),
	header_clr_data(DEFAULT_ACCUM_BUFFER_RES_X*DEFAULT_ACCUM_BUFFER_RES_Y, -1)
{
	atomic_counter->video_init();
	header->video_init();
	fragments->video_init();
}

translucent_buffers::~translucent_buffers()
{
	atomic_counter->video_release();
	header->video_release();
	fragments->video_release();
	delete atomic_counter;
	delete header;
	delete fragments;
}

void translucent_buffers::bind_buffers()
{
	atomic_counter->bind(0);
	header->bind(0);
	fragments->bind(1);
}

void translucent_buffers::unbind_buffers()
{
	atomic_counter->unbind();
	header->unbind();	
}

void translucent_buffers::reset_atomic_counter()
{
	uint32 data = 0;
	atomic_counter->bind();
	uint32 * ptr = atomic_counter->map<uint32>(0, 1, nsbuffer_object::access_map_range(nsbuffer_object::map_write));
	ptr[0] = 0;
	atomic_counter->unmap();
	atomic_counter->unbind();
}

void render_pass::setup_pass()
{
	nsgl_framebuffer * tgt = (nsgl_framebuffer*)ren_target;
	tgt->set_target(nsgl_framebuffer::fb_draw);
	tgt->bind();

	if (use_vp_size && vp != nullptr)
		gl_state.current_viewport = ivec4(vp->bounds.xy(), vp->bounds.zw() - vp->bounds.xy());
	else
		gl_state.current_viewport = ivec4(0,0,tgt->size());
	
	driver->set_gl_state(gl_state);
}

struct font_layout
{
	uint32 line;
	uint32 xpos;
};
void ui_render_pass::render()
{
	ren_target->bind();
	ren_target->update_draw_buffers();
	const ivec4 & viewp = gl_state.current_viewport;
	
	for (uint32 i = 0; i < draw_calls->size(); ++i)
	{
		ui_draw_call * uidc = (ui_draw_call*)(*draw_calls)[i];		

		// render the border
		if (uidc->border_color.a >= 0.03f && uidc->border_shader != nullptr)
		{
			uidc->border_shader->bind();
			uidc->border_shader->set_uniform("viewport", fvec4(0,0,viewp.z,viewp.w));
			uidc->border_shader->set_uniform("border_pix", uidc->border_pix);
			uidc->border_shader->set_uniform("content_wscale", uidc->content_wscale);
			uidc->border_shader->set_uniform("content_tform", uidc->content_tform);
			uidc->border_shader->set_uniform("frag_color_out", uidc->border_color);
			uidc->border_shader->set_uniform("entity_id", uidc->entity_id);
			driver->render_ui_dc(uidc);
		}

		// render the material part of the ui-element
		if (uidc->shdr != nullptr)
		{
			uidc->shdr->bind();
			uidc->shdr->set_uniform("uitexture", DIFFUSE_TEX_UNIT);
			uidc->shdr->set_uniform("entity_id", uidc->entity_id);
			uidc->shdr->set_uniform("viewport", fvec4(0,0,viewp.z,viewp.w));
			uidc->shdr->set_uniform("wscale", uidc->content_wscale);
			uidc->shdr->set_uniform("content_tform", uidc->content_tform);
			uidc->shdr->set_uniform("tex_coord_rect", uidc->content_tex_coord_rect);
			uidc->shdr->set_uniform("color_mult", uidc->color_multiplier);
		
			if (uidc->mat != nullptr)
			{
				uidc->shdr->set_uniform("frag_color_out", uidc->mat->color());
				uidc->shdr->set_uniform("color_mode", uidc->mat->color_mode());
				driver->enable_culling(uidc->mat->culling());
				driver->set_cull_face(uidc->mat->cull_mode());
				driver->bind_textures(uidc->mat);
			}
			else
			{
				uidc->shdr->set_uniform("frag_color_out", fvec4(1,0,0,1));
				uidc->shdr->set_uniform("color_mode", true);
			}
			driver->render_ui_dc(uidc);
		}
		
		// render the text part of the ui-element
		if (uidc->fnt != nullptr)
		{
			font_info & fi = uidc->fnt->get_font_info();

			uidc->text_shader->bind();
			uidc->text_shader->set_uniform("uitexture", DIFFUSE_TEX_UNIT);
			uidc->text_shader->set_uniform("entity_id", uidc->entity_id);
			uidc->text_shader->set_uniform("viewport", fvec4(0,0,viewp.z,viewp.w));
			uidc->text_shader->set_uniform("content_tform", uidc->content_tform);
			uidc->text_shader->set_uniform("frag_color_out", uidc->fnt->render_color);
			uidc->text_shader->set_uniform("color_mult", uidc->color_multiplier);

			fvec2 rect_wh = fvec2(viewp.z, viewp.w) % uidc->content_wscale;
            fvec2 cursor_pos(0,0);
			int32 cur_line = 0;
			int32 line_index = 0;
			std::vector<float> x_offsets;

			// figure out the vertical starting cursor pos
			if (uidc->alignment < 3) // top alignment
			{
				cursor_pos.y = rect_wh.h - fi.line_height - uidc->margins.w;
			}
			else if (uidc->alignment < 6) // middle alignment
			{
				cursor_pos.y = (rect_wh.h - uidc->margins.y - uidc->margins.w - fi.line_height) / 2.0f;
				float v_offset_factor = float(uidc->text_line_sizes.size()-1) / 2.0f;
				cursor_pos.y += v_offset_factor * float(fi.line_height);
			}
			else // bottom alignment
			{
				cursor_pos.y = uidc->margins.y + (uidc->text_line_sizes.size() -1) * fi.line_height;
			}

			// figure out the first line's horizontal pos
			if (uidc->alignment % 3 == 0) // left
			{
				x_offsets.resize(uidc->text_line_sizes.size(), uidc->margins.x);
			}
			else if (uidc->alignment % 3 == 1) // center
			{
				x_offsets.resize(uidc->text_line_sizes.size(), 0);
				uint32 ti = 0;
				for (uint32 i = 0; i < x_offsets.size(); ++i)
				{
					float xoff = 0.0f;
					for (uint32 j = 0; j < uidc->text_line_sizes[i]; ++j)
					{
						char_info & ci = uidc->fnt->get_char_info(uidc->text[ti]);
						xoff += ci.xadvance;
						++ti;
					}
					++ti; // newline char
					x_offsets[i] = (rect_wh.w - uidc->margins.x - uidc->margins.z - xoff) / 2.0f;
				}
			}
			else // right
			{
				x_offsets.resize(uidc->text_line_sizes.size(), 0);
				uint32 ti = 0;
				for (uint32 i = 0; i < x_offsets.size(); ++i)
				{
					float xoff = 0.0f;
					for (uint32 j = 0; j < uidc->text_line_sizes[i]; ++j)
					{
						char_info & ci = uidc->fnt->get_char_info(uidc->text[ti]);
						xoff += ci.xadvance;
						++ti;
					}
					++ti;
					x_offsets[i] = (rect_wh.w - uidc->margins.z - xoff);
				}
			}
			cursor_pos.x = x_offsets[0];
			fvec2 cursor_xy(x_offsets[uidc->cursor_offset.y],cursor_pos.y);
			for (uint32 i = 0; i < uidc->text.size(); ++i)
			{
                if (uidc->text[i] == '\n')
                {
                    cursor_pos.y -= fi.line_height;
					line_index = 0;
					++cur_line;
                    cursor_pos.x = x_offsets[cur_line];
					if (cur_line == uidc->cursor_offset.y)
						cursor_xy.y = cursor_pos.y;
                    continue;
                }

				char_info & ci = uidc->fnt->get_char_info(uidc->text[i]);
				nstex2d * tex = get_resource<nstex2d>(uidc->fnt->texture_id(ci.page_index));
				if (tex == nullptr)
					continue;

				driver->set_active_texture_unit(nsmaterial::diffuse);
				tex->bind();

				ivec2 tsz = tex->size();
				fvec2 tex_size(tsz.x,tsz.y);

                fvec4 rect(ci.rect.x / tex_size.x,
                           ((tex_size.y - ci.rect.y) - ci.rect.w) / tex_size.y,
                           (ci.rect.x + ci.rect.z) / tex_size.x,
                           (tex_size.y - ci.rect.y) / tex_size.y);

				uidc->text_shader->set_uniform("pixel_wh", fvec2(ci.rect.z,ci.rect.w));
				uidc->text_shader->set_uniform("drawing_cursor", false);
                uidc->text_shader->set_uniform("offset_xy", fvec2(cursor_pos.x + ci.offset.x, cursor_pos.y + fi.line_height - ci.offset.y - ci.rect.w));
				uidc->text_shader->set_uniform("tex_coord_rect", rect);
				uidc->text_shader->set_uniform("rect_bounds", rect_wh);
				uidc->text_shader->set_uniform("margins", uidc->margins);

				cursor_pos.x += ci.xadvance;
				++line_index;

				if (cur_line == uidc->cursor_offset.y && line_index == uidc->cursor_offset.x)
					cursor_xy.x = cursor_pos.x;				

				driver->render_ui_dc(uidc);
			}

			// lets draw the cursor if the text is editable - ie has a text input component
			if (uidc->text_editable)
			{
				uidc->text_shader->set_uniform("drawing_cursor", true);
				uidc->text_shader->set_uniform("frag_color_out", uidc->cursor_color);
				uidc->text_shader->set_uniform("pixel_wh", fvec2(uidc->cursor_pixel_width,fi.line_height));
				uidc->text_shader->set_uniform("offset_xy",cursor_xy);
				driver->render_ui_dc(uidc);
			}
		}
	}
}

void selection_render_pass::render()
{
	for (uint32 i = 0; i < draw_calls->size(); ++i)
	{
		instanced_draw_call * dc = (instanced_draw_call*)(*draw_calls)[i];
		ren_target->set_draw_buffer(nsgl_framebuffer::att_none);		
		dc->shdr->bind();
		dc->shdr->set_uniform("heightMap", HEIGHT_TEX_UNIT);
		if (vp->camera != nullptr)
			dc->shdr->set_uniform("projCamMat",vp->camera->get<nscam_comp>()->proj_cam());
		
		if (dc->submesh->m_node != nullptr)
			dc->shdr->set_uniform("nodeTransform", dc->submesh->m_node->m_world_tform);
		else
			dc->shdr->set_uniform("nodeTransform", fmat4());

		if (dc->anim_transforms != nullptr)
		{
			for (uint32 i = 0; i < dc->anim_transforms->size(); ++i)
				dc->shdr->set_uniform("boneTransforms[" + std::to_string(i) + "]", (*dc->anim_transforms)[i]);
			dc->shdr->set_uniform("hasBones", true);
		}
		else
		{
			dc->shdr->set_uniform("hasBones", false);
		}
		dc->shdr->set_uniform("hasHeightMap", dc->mat->contains(nsmaterial::height));
		dc->shdr->set_uniform("hminmax", dc->height_minmax);
		driver->render_instanced_dc(dc);
		
		glPolygonMode(GL_FRONT, GL_LINE);
		glLineWidth(4.0f);
		driver->set_stencil_func(GL_NOTEQUAL, 1, -1);
		ren_target->set_draw_buffer(nsgl_framebuffer::att_color);
		dc->shdr->set_uniform("fragColOut", fvec4(dc->sel_color.rgb(),1.0f));	
		driver->render_instanced_dc(dc);
		
		glLineWidth(1.0f);
		glPolygonMode(GL_FRONT, GL_FILL);
		driver->enable_depth_test(false);
		driver->set_stencil_func(GL_EQUAL, 1, 0);
		dc->shdr->set_uniform("fragColOut", dc->sel_color);
		driver->render_instanced_dc(dc);
	}	
}

void gbuffer_render_pass::render()
{
	for (uint32 i = 0; i < draw_calls->size(); ++i)
	{
		instanced_draw_call * dc = (instanced_draw_call*)(*draw_calls)[i];
		ivec4 & viewp = gl_state.current_viewport;
		dc->shdr->bind();

		dc->shdr->set_uniform("diffuseMap", DIFFUSE_TEX_UNIT);
		dc->shdr->set_uniform("normalMap", NORMAL_TEX_UNIT);
		dc->shdr->set_uniform("opacityMap", OPACITY_TEX_UNIT);
		dc->shdr->set_uniform("heightMap", HEIGHT_TEX_UNIT);
		dc->shdr->set_uniform("viewport", fvec4(viewp.x, viewp.y, viewp.z, viewp.w));
        fmat4 proj_cam = vp->camera->get<nscam_comp>()->proj_cam();
		if (vp->camera != nullptr)
            dc->shdr->set_uniform("projCamMat", proj_cam);

		dc->shdr->set_uniform("hasHeightMap", dc->mat->contains(nsmaterial::height));
		dc->shdr->set_uniform("hasDiffuseMap", dc->mat->contains(nsmaterial::diffuse));
		dc->shdr->set_uniform("hasOpacityMap", dc->mat->contains(nsmaterial::opacity));
		dc->shdr->set_uniform("hasNormalMap", dc->mat->contains(nsmaterial::normal));
		dc->shdr->set_uniform("colorMode", dc->mat->color_mode());
		dc->shdr->set_uniform("fragColOut", dc->mat->color());
		dc->shdr->set_uniform("force_alpha", dc->mat->using_alpha_from_color());
		dc->shdr->set_uniform("material_id",  dc->mat_index);

		uint32 ent_id = dc->entity_id;
		if (dc->transparent_picking)
			ent_id = 0;

		dc->shdr->set_uniform("hminmax", dc->height_minmax);
		dc->shdr->set_uniform("entityID", ent_id);
		dc->shdr->set_uniform("pluginID", dc->plugin_id);

		if (dc->submesh->m_node != NULL)
			dc->shdr->set_uniform("nodeTransform", dc->submesh->m_node->m_world_tform);
		else
			dc->shdr->set_uniform("nodeTransform", fmat4());

		if (!dc->submesh->m_has_tex_coords)
			dc->shdr->set_uniform("colorMode", true);

		if (dc->anim_transforms != NULL)
		{
			dc->shdr->set_uniform("hasBones", true);
			for (uint32 i = 0; i < dc->anim_transforms->size(); ++i)
				dc->shdr->set_uniform("boneTransforms[" + std::to_string(i) + "]", (*dc->anim_transforms)[i]);
		}
		else
			dc->shdr->set_uniform("hasBones", false);

		driver->enable_culling(dc->mat->culling());
		driver->set_cull_face(dc->mat->cull_mode());
		driver->bind_textures(dc->mat);
		driver->render_instanced_dc(dc);
	}
	driver->bind_gbuffer_textures((nsgl_framebuffer*)ren_target);
}

void oit_render_pass::render()
{
	ivec4 viewp = gl_state.current_viewport;
	ren_target->set_draw_buffer(nsgl_framebuffer::att_none);
	for (uint32 i = 0; i < draw_calls->size(); ++i)
	{
		instanced_draw_call * dc = (instanced_draw_call*)(*draw_calls)[i];

		dc->shdr->bind();
		dc->shdr->set_uniform("diffuseMap", DIFFUSE_TEX_UNIT);
		dc->shdr->set_uniform("normalMap", NORMAL_TEX_UNIT);
		dc->shdr->set_uniform("opacityMap", OPACITY_TEX_UNIT);
		dc->shdr->set_uniform("heightMap", HEIGHT_TEX_UNIT);
		dc->shdr->set_uniform("viewport", fvec4(viewp.x, viewp.y, viewp.z, viewp.w));
		if (vp->camera != nullptr)
			dc->shdr->set_uniform("projCamMat", vp->camera->get<nscam_comp>()->proj_cam());	

		dc->shdr->set_uniform("hasHeightMap", dc->mat->contains(nsmaterial::height));
		dc->shdr->set_uniform("hasDiffuseMap", dc->mat->contains(nsmaterial::diffuse));
		dc->shdr->set_uniform("hasOpacityMap", dc->mat->contains(nsmaterial::opacity));
		dc->shdr->set_uniform("hasNormalMap", dc->mat->contains(nsmaterial::normal));
		dc->shdr->set_uniform("colorMode", dc->mat->color_mode());
		dc->shdr->set_uniform("fragColOut", dc->mat->color());
		dc->shdr->set_uniform("force_alpha", dc->mat->using_alpha_from_color());
		dc->shdr->set_uniform("material_id",  dc->mat_index);

		uint32 ent_id = dc->entity_id;
		if (dc->transparent_picking)
			ent_id = 0;

		dc->shdr->set_uniform("hminmax", dc->height_minmax);
		dc->shdr->set_uniform("entityID", ent_id);
		dc->shdr->set_uniform("pluginID", dc->plugin_id);

		if (dc->submesh->m_node != NULL)
			dc->shdr->set_uniform("nodeTransform", dc->submesh->m_node->m_world_tform);
		else
			dc->shdr->set_uniform("nodeTransform", fmat4());

		if (!dc->submesh->m_has_tex_coords)
			dc->shdr->set_uniform("colorMode", true);

		if (dc->anim_transforms != NULL)
		{
			dc->shdr->set_uniform("hasBones", true);
			for (uint32 i = 0; i < dc->anim_transforms->size(); ++i)
				dc->shdr->set_uniform("boneTransforms[" + std::to_string(i) + "]", (*dc->anim_transforms)[i]);
		}
		else
			dc->shdr->set_uniform("hasBones", false);

		if (dc->mat != nullptr)
		{
			driver->enable_culling(dc->mat->culling());
			driver->set_cull_face(dc->mat->cull_mode());
			driver->bind_textures(dc->mat);
		}
		tbuffers->bind_buffers();
		driver->render_instanced_dc(dc);
	}
	ren_target->update_draw_buffers();
	driver->enable_depth_test(false);
	driver->enable_culling(true);
	driver->set_cull_face(GL_BACK);
	nsshader * fsort = nse.system<nsrender_system>()->get_render_shaders().frag_sort;
	
	fsort->bind();
	fsort->set_uniform("gMatMap", int32(G_PICKING_TEX_UNIT));
	fsort->set_uniform("viewport", fvec4(viewp.x, viewp.y, viewp.z, viewp.w));
	driver->current_context()->m_single_point->bind();
	tbuffers->bind_buffers();
	glDrawArrays(GL_POINTS, 0, 1);	
	tbuffers->unbind_buffers();
}

void light_shadow_pass::render()
{
	nsshader * cur_shdr = nullptr;
	ivec4 & viewp = gl_state.current_viewport;
	if (ldc->light_type == nslight_comp::l_point)
	{
		cur_shdr = nse.system<nsrender_system>()->get_render_shaders().shadow_cube;
		cur_shdr->bind();
		cur_shdr->set_uniform("lightPos", ldc->light_pos);
		cur_shdr->set_uniform("maxDepth", ldc->max_depth);
	}
	else
	{
		cur_shdr = nse.system<nsrender_system>()->get_render_shaders().shadow_2d;
		cur_shdr->bind();
	}
	cur_shdr->set_uniform("heightMap", HEIGHT_TEX_UNIT);
	cur_shdr->set_uniform("projMat", ldc->proj_light_mat);
	cur_shdr->set_uniform("viewport", fvec4(viewp.x,viewp.y,viewp.z,viewp.w));
	
	for (uint32 i = 0; i < draw_calls->size(); ++i)
	{
		instanced_draw_call * idc = (instanced_draw_call*)(*draw_calls)[i];
		if (idc->casts_shadows)
		{
			if (idc->submesh->m_node != nullptr)
				cur_shdr->set_uniform("nodeTransform", idc->submesh->m_node->m_world_tform);
			else
				cur_shdr->set_uniform("nodeTransform", fmat4());

			if (idc->anim_transforms != nullptr)
			{
				for (uint32 i = 0; i < idc->anim_transforms->size(); ++i)
					cur_shdr->set_uniform(
						"boneTransforms[" + std::to_string(i) + "]", (*idc->anim_transforms)[i]);
				cur_shdr->set_uniform("hasBones", true);
			}
			else
			{
				cur_shdr->set_uniform("hasBones", false);
			}
			cur_shdr->set_uniform("hasHeightMap", idc->mat->contains(nsmaterial::height));
			cur_shdr->set_uniform("hminmax", idc->height_minmax);

			driver->enable_culling(idc->mat->culling());
			driver->set_cull_face(idc->mat->cull_mode());
			driver->render_instanced_dc(idc);
		}
	}
}

void light_pass::render()
{
	for (uint32 i = 0; i < draw_calls->size(); ++i)
	{
		light_draw_call * dc = (light_draw_call*)(*draw_calls)[i];
		ivec4 & viewp = gl_state.current_viewport;
		if (dc->cast_shadows && vp->dir_light_shadows)
		{
			rpass->ldc = dc;
			rpass->setup_pass();
			rpass->render();
			nsgl_framebuffer::attachment * attch = rpass->ren_target->att(nsgl_framebuffer::att_depth);
			driver->set_active_texture_unit(attch->m_tex_unit);
			attch->m_texture->bind();
			render_pass::setup_pass();
		}

		ren_target->set_draw_buffer(nsgl_framebuffer::att_color);
		dc->shdr->bind();
		dc->shdr->set_uniform("shadowMap", SHADOW_TEX_UNIT);
		dc->shdr->set_uniform("gDiffuseMap", G_DIFFUSE_TEX_UNIT);
		dc->shdr->set_uniform("gNormalMap", G_NORMAL_TEX_UNIT);
		dc->shdr->set_uniform("gWorldPosMap", G_WORLD_POS_TEX_UNIT);
		dc->shdr->set_uniform("gMatMap", G_PICKING_TEX_UNIT);
		dc->shdr->set_uniform("epsilon", DEFAULT_SHADOW_EPSILON);
		dc->shdr->set_uniform("projCamMat", vp->camera->get<nscam_comp>()->proj_cam());
		dc->shdr->set_uniform("viewport", fvec4(viewp.x, viewp.y, viewp.z, viewp.w));
		dc->shdr->set_uniform("light.diffuseIntensity", dc->diffuse_intensity);
		dc->shdr->set_uniform("light.ambientIntensity", dc->ambient_intensity);
		dc->shdr->set_uniform("castShadows", dc->cast_shadows);
		dc->shdr->set_uniform("light.color", dc->light_color);
		dc->shdr->set_uniform("shadowSamples", dc->shadow_samples);
		dc->shdr->set_uniform("light.shadowDarkness", dc->shadow_darkness);
		const ivec2 & ss = driver->render_target(DIR_SHADOW2D_TARGET)->size();
		dc->shdr->set_uniform("shadowTexSize", fvec2(ss.x, ss.y));
		
		nsstring id;
		id.reserve(64);
		auto iter = dc->material_ids->begin();
		while (iter != dc->material_ids->end())
		{
			id = "materials[" + std::to_string(iter->second) + "].spec_";
			dc->shdr->set_uniform(id+"color",iter->first->specular_color());
			dc->shdr->set_uniform(id+"power",iter->first->specular_power());
			dc->shdr->set_uniform(id+"intensity",iter->first->specular_intensity());
			++iter;
		}

		dc->shdr->set_uniform("projLightMat", dc->proj_light_mat);
		dc->shdr->set_uniform("bgColor", dc->bg_color);
		dc->shdr->set_uniform("light.direction", dc->direction);
        dc->shdr->set_uniform("camWorldPos", vp->camera->get<nstform_comp>()->instance_transform(nse.system<nsrender_system>()->active_scene(), 0)->world_position());
		dc->shdr->set_uniform("fog_factor", vp->m_fog_nf);
		dc->shdr->set_uniform("fog_color", vp->m_fog_color);
		dc->shdr->set_uniform("lightingEnabled", vp->dir_lights);
		
		tbuffers->bind_buffers();
		driver->render_light_dc(dc);
		tbuffers->unbind_buffers();
	}
}

void culled_light_pass::render()
{
	for (uint32 i = 0; i < draw_calls->size(); ++i)
	{
		light_draw_call * dc = (light_draw_call*)(*draw_calls)[i];
		if (dc->cast_shadows && vp->dir_light_shadows)
		{
			rpass->ldc = dc;
			rpass->setup_pass();
			rpass->render();
			nsgl_framebuffer::attachment * attch = rpass->ren_target->att(nsgl_framebuffer::att_depth);
			driver->set_active_texture_unit(attch->m_tex_unit);
			attch->m_texture->bind();
			render_pass::setup_pass();
		}
		ren_target->set_draw_buffer(nsgl_framebuffer::att_none);
		nsshader * st_shdr = nse.system<nsrender_system>()->get_render_shaders().light_stencil;
		ivec4 & viewp = gl_state.current_viewport;
		st_shdr->bind();
		st_shdr->set_uniform("projCamMat", vp->camera->get<nscam_comp>()->proj_cam());
		st_shdr->set_uniform("viewport", fvec4(viewp.x, viewp.y, viewp.z, viewp.w));

		if (dc->submesh->m_node != nullptr)
			st_shdr->set_uniform("nodeTransform", dc->submesh->m_node->m_world_tform);
		else
			st_shdr->set_uniform("nodeTransform", fmat4());
		st_shdr->set_uniform("transform", dc->light_transform);

		driver->render_light_dc(dc);
		
		driver->enable_depth_test(false);
		driver->enable_culling(true);
		driver->set_cull_face(GL_FRONT);
		driver->set_stencil_func(GL_NOTEQUAL, 0, 0xFF);
		ren_target->set_draw_buffer(nsgl_framebuffer::att_color);

		dc->shdr->bind();

		dc->shdr->bind();
		dc->shdr->set_uniform("shadowMap", SHADOW_TEX_UNIT);
		dc->shdr->set_uniform("gDiffuseMap", G_DIFFUSE_TEX_UNIT);
		dc->shdr->set_uniform("gNormalMap", G_NORMAL_TEX_UNIT);
		dc->shdr->set_uniform("gWorldPosMap", G_WORLD_POS_TEX_UNIT);
		dc->shdr->set_uniform("gMatMap", G_PICKING_TEX_UNIT);
		dc->shdr->set_uniform("epsilon", DEFAULT_SHADOW_EPSILON);
		dc->shdr->set_uniform("projCamMat", vp->camera->get<nscam_comp>()->proj_cam());
		dc->shdr->set_uniform("viewport", fvec4(viewp.x, viewp.y, viewp.z, viewp.w));
		dc->shdr->set_uniform("light.diffuseIntensity", dc->diffuse_intensity);
		dc->shdr->set_uniform("light.ambientIntensity", dc->ambient_intensity);
		dc->shdr->set_uniform("castShadows", dc->cast_shadows);
		dc->shdr->set_uniform("light.color", dc->light_color);
		dc->shdr->set_uniform("shadowSamples", dc->shadow_samples);
		dc->shdr->set_uniform("light.shadowDarkness", dc->shadow_darkness);
		dc->shdr->set_uniform("fog_factor", vp->m_fog_nf);
		dc->shdr->set_uniform("fog_color", vp->m_fog_color);		
		
		nsstring id;
		id.reserve(64);
		auto iter = dc->material_ids->begin();
		while (iter != dc->material_ids->end())
		{
			id = "materials[" + std::to_string(iter->second) + "].spec_";
			dc->shdr->set_uniform(id+"color",iter->first->specular_color());
			dc->shdr->set_uniform(id+"power",iter->first->specular_power());
			dc->shdr->set_uniform(id+"intensity",iter->first->specular_intensity());
			++iter;
		}

		dc->shdr->set_uniform("light.attConstant", dc->spot_atten.x);
		dc->shdr->set_uniform("light.attLinear", dc->spot_atten.y);
		dc->shdr->set_uniform("light.attExp", dc->spot_atten.z);
		dc->shdr->set_uniform("light.position", dc->light_pos);
		dc->shdr->set_uniform("maxDepth", dc->max_depth);
		dc->shdr->set_uniform("transform", dc->light_transform);
		if (dc->submesh->m_node != nullptr)
			dc->shdr->set_uniform("nodeTransform", dc->submesh->m_node->m_world_tform);
		else
			dc->shdr->set_uniform("nodeTransform", fmat4());
			
		if (dc->light_type == nslight_comp::l_spot)
		{
			dc->shdr->set_uniform("projLightMat", dc->proj_light_mat);
			dc->shdr->set_uniform("light.direction", dc->direction);
			dc->shdr->set_uniform("light.cutoff", dc->cutoff);
			const ivec2 & ss = driver->render_target(SPOT_SHADOW2D_TARGET)->size();
			dc->shdr->set_uniform("shadowTexSize", fvec2(ss.x, ss.y));

		}
		else
		{
			const ivec2 & ss = driver->render_target(POINT_SHADOW_TARGET)->size();
			dc->shdr->set_uniform("shadowTexSize", fvec2(ss.x, ss.y));
		}

		tbuffers->bind_buffers();
		driver->render_light_dc(dc);
		tbuffers->unbind_buffers();
	}
}

void final_render_pass::render()
{
	read_buffer->set_target(nsgl_framebuffer::fb_read);
	read_buffer->bind();
	read_buffer->set_read_buffer(nsgl_framebuffer::att_color);

	glBlitFramebuffer(
		vp->bounds.x,
		vp->bounds.y,
		vp->bounds.z,
		vp->bounds.w,
		vp->bounds.x,
		vp->bounds.y,
		vp->bounds.z,
		vp->bounds.w,
		GL_COLOR_BUFFER_BIT,
		GL_NEAREST);

	read_buffer->set_read_buffer(nsgl_framebuffer::att_none);
	read_buffer->unbind();	
}

GLEWContext * glewGetContext()
{
	nsopengl_driver * driver = static_cast<nsopengl_driver*>(nse.video_driver());
	return driver->current_context()->glew_context;
}

gl_ctxt::gl_ctxt(uint32 id) :
	context_id(id),
	initialized(false),
	glew_context(new GLEWContext()),
	m_tbuffers(nullptr),
	m_default_target(nullptr)
{
}

gl_ctxt::~gl_ctxt()
{
	delete glew_context;
}

void gl_ctxt::init()
{
	glewExperimental = true;

    // Initialize the glew extensions - if this fails we want a crash because there is nothing
	// useful the program can do without these initialized
	GLenum cont = glewInit();
	if (cont != GLEW_OK)
	{
		dprint("GLEW extensions unable to initialize");
		return;
	}
	initialized = true;

	// GL default setup
	glFrontFace(GL_CW);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(false);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);

	// Create the default framebuffer
	m_default_target = new nsgl_framebuffer;
	m_default_target->set_gl_id(0);

	// Get single point from render system and initialize it
	m_single_point = new nsbuffer_object(nsbuffer_object::array, nsbuffer_object::storage_mutable);
	m_single_point->video_init();

	
	uint32 data_ = 0;
	uint32 sz = DEFAULT_ACCUM_BUFFER_RES_X*DEFAULT_ACCUM_BUFFER_RES_Y;
	i_vector header_data(sz, -1);
	m_tbuffers = new translucent_buffers;
	m_tbuffers->atomic_counter->bind();
	m_tbuffers->atomic_counter->allocate(data_, nsbuffer_object::mutable_dynamic_draw);
	m_tbuffers->atomic_counter->unbind();
	m_tbuffers->header->bind();
	m_tbuffers->header->allocate(header_data, nsbuffer_object::mutable_dynamic_draw, header_data.size());
	m_tbuffers->fragments->bind();
	m_tbuffers->fragments->allocate<packed_fragment_data>(nsbuffer_object::mutable_dynamic_draw, sz * 2);
	m_tbuffers->fragments->unbind();

	m_single_point->bind();
	fvec3 point;
	m_single_point->allocate(point, nsbuffer_object::mutable_dynamic_draw);
	m_single_point->unbind();
}

void gl_ctxt::release()
{
	m_single_point->video_release();
	delete m_single_point;
	delete m_default_target;
	delete m_tbuffers;
	m_default_target = nullptr;
	initialized = false;
}


nsopengl_driver::nsopengl_driver() :
	nsvideo_driver(),
	m_current_context(nullptr),
	m_auto_cleanup(true)
{
	for (uint8 i = 0; i < MAX_CONTEXT_COUNT; ++i)
		m_contexts[i] = nullptr;
}

nsopengl_driver::~nsopengl_driver()
{
	for (uint8 i = 0; i < MAX_CONTEXT_COUNT; ++i)
	{
		delete m_contexts[i];
		m_contexts[i] = nullptr;
	}
}

uint8 nsopengl_driver::create_context()
{
	static uint8 id = 0; // forever increasing id
	m_contexts[id] = new gl_ctxt(id);
	m_current_context = m_contexts[id];
	return id++;
}

bool nsopengl_driver::destroy_context(uint8 c_id)
{
	if (c_id >= MAX_CONTEXT_COUNT)
		return false;
	
	delete m_contexts[c_id];
	m_contexts[c_id] = nullptr;
	return true;
}

bool nsopengl_driver::make_context_current(uint8 c_id)
{
	if (c_id >= MAX_CONTEXT_COUNT)
		return false;
	m_current_context = m_contexts[c_id];
	if (m_auto_cleanup)
		cleanup_vid_objs();
	return true;
}

gl_ctxt * nsopengl_driver::current_context()
{
	return m_current_context;
}

void nsopengl_driver::create_default_render_targets()
{
	// Create all of the framebuffers
	nsgbuf_object * gbuffer = new nsgbuf_object;
	gbuffer->video_init();
	gbuffer->resize(DEFAULT_GBUFFER_RES_X, DEFAULT_GBUFFER_RES_Y);
	gbuffer->init();
	add_render_target(GBUFFER_TARGET, gbuffer);

	tex_params tp;
	tp.min_filter = tmin_linear;
	tp.mag_filter = tmag_linear;
	
	// Accumulation buffer
	nsgl_framebuffer * accum_buffer = new nsgl_framebuffer;
	accum_buffer->video_init();
	accum_buffer->resize(DEFAULT_ACCUM_BUFFER_RES_X, DEFAULT_ACCUM_BUFFER_RES_Y);
	accum_buffer->init();
	accum_buffer->set_target(nsgl_framebuffer::fb_draw);

	accum_buffer->bind();
	accum_buffer->create_texture_attachment<nstex2d>(
		"rendered_frame",
		nsgl_framebuffer::attach_point(nsgl_framebuffer::att_color),
		FINAL_TEX_UNIT,
		tex_rgba,
		tex_float,
		tp);
	accum_buffer->add(gbuffer->depth());
	
	tp.min_filter = tmin_nearest;
	tp.mag_filter = tmag_nearest;

	accum_buffer->create_texture_attachment<nstex2d>(
		"final_picking",
		nsgl_framebuffer::attach_point(nsgl_framebuffer::att_color + nsgbuf_object::col_picking),
		G_PICKING_TEX_UNIT,
		tex_irgb,
		tex_u32,
		tp);

	accum_buffer->update_draw_buffers();
	add_render_target(ACCUM_TARGET, accum_buffer);

	// 2d shadow map targets
	nsshadow_tex2d_target * dir_shadow_target = new nsshadow_tex2d_target;
	dir_shadow_target->video_init();
	dir_shadow_target->resize(DEFAULT_DIR_LIGHT_SHADOW_W, DEFAULT_DIR_LIGHT_SHADOW_H);
	dir_shadow_target->init("direction_light_shadow");
	add_render_target(DIR_SHADOW2D_TARGET, dir_shadow_target);
	
	nsshadow_tex2d_target * spot_shadow_target = new nsshadow_tex2d_target;
	spot_shadow_target->video_init();
	spot_shadow_target->resize(DEFAULT_SPOT_LIGHT_SHADOW_W, DEFAULT_SPOT_LIGHT_SHADOW_H);
	spot_shadow_target->init("spot_light_shadow");
	add_render_target(SPOT_SHADOW2D_TARGET, spot_shadow_target);
	
	nsshadow_tex_cubemap_target * point_shadow_target = new nsshadow_tex_cubemap_target;
	point_shadow_target->video_init();
	point_shadow_target->resize(DEFAULT_POINT_LIGHT_SHADOW_W, DEFAULT_POINT_LIGHT_SHADOW_H);
	point_shadow_target->init("point_light_shadow");
	add_render_target(POINT_SHADOW_TARGET, point_shadow_target);	
}

void nsopengl_driver::setup_default_rendering()
{
	if (m_current_context == nullptr)
	{
		dprint("nsopengl_driver::setup_default_rendering You need to create a context before setting up rendering");
		return;
	}
	create_default_render_targets();
	create_default_render_passes();
}

void nsopengl_driver::clear_render_targets()
{
	while (m_current_context->m_render_targets.begin() != m_current_context->m_render_targets.end())
		destroy_render_target(m_current_context->m_render_targets.begin()->first);
	m_current_context->m_render_targets.clear();
}

void nsopengl_driver::clear_render_passes()
{
	while (m_current_context->m_render_passes.begin() != m_current_context->m_render_passes.end())
	{
		delete m_current_context->m_render_passes.back();
		m_current_context->m_render_passes.pop_back();
	}	
}

render_pass_vector * nsopengl_driver::render_passes()
{
	return &m_current_context->m_render_passes;
}

bool nsopengl_driver::add_render_target(const nsstring & name, nsgl_framebuffer * rt)
{
	return m_current_context->m_render_targets.emplace(name, rt).second;
}

nsgl_framebuffer * nsopengl_driver::default_target()
{
	return m_current_context->m_default_target;
}

nsgl_framebuffer * nsopengl_driver::remove_render_target(const nsstring & name)
{
	nsgl_framebuffer * fb = nullptr;
	auto iter = m_current_context->m_render_targets.find(name);
	if (iter != m_current_context->m_render_targets.end())
	{
		fb = iter->second;
		m_current_context->m_render_targets.erase(iter);
	}
	return fb;
}

nsgl_framebuffer * nsopengl_driver::render_target(const nsstring & name)
{
	auto iter = m_current_context->m_render_targets.find(name);
	if (iter != m_current_context->m_render_targets.end())
		return iter->second;
	return nullptr;
}

void nsopengl_driver::destroy_render_target(const nsstring & name)
{
	nsgl_framebuffer * rt = remove_render_target(name);
	rt->video_release();
	delete rt;
}

void nsopengl_driver::create_default_render_passes()
{
	// setup gbuffer geometry stage
	gbuffer_render_pass * gbuf_pass = new gbuffer_render_pass;
	gbuf_pass->draw_calls = nse.system<nsrender_system>()->queue(SCENE_OPAQUE_QUEUE);
	gbuf_pass->ren_target = render_target(GBUFFER_TARGET);
	gbuf_pass->gl_state.depth_test = true;
	gbuf_pass->gl_state.depth_write = true;
	gbuf_pass->gl_state.culling = true;
	gbuf_pass->gl_state.cull_face = GL_BACK;
	gbuf_pass->gl_state.blending = false;
	gbuf_pass->gl_state.stencil_test = false;
	gbuf_pass->use_vp_size = true;
	gbuf_pass->driver = this;
	gbuf_pass->gl_state.clear_mask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;

	oit_render_pass * oit_pass = new oit_render_pass;
	oit_pass->tbuffers = m_current_context->m_tbuffers;
	oit_pass->draw_calls = nse.system<nsrender_system>()->queue(SCENE_TRANSLUCENT_QUEUE);
	oit_pass->ren_target = render_target(ACCUM_TARGET);
	oit_pass->gl_state.depth_test = true;
	oit_pass->gl_state.depth_write = false;
	oit_pass->gl_state.culling = true;
	oit_pass->gl_state.cull_face = GL_BACK;
	oit_pass->gl_state.blending = false;
	oit_pass->gl_state.stencil_test = false;
	oit_pass->driver = this;

	// setup dir light shadow stage
	light_shadow_pass * dir_shadow_pass = new light_shadow_pass;
	dir_shadow_pass->draw_calls = nse.system<nsrender_system>()->queue(SCENE_OPAQUE_QUEUE);
	dir_shadow_pass->ren_target = render_target(DIR_SHADOW2D_TARGET);
	dir_shadow_pass->gl_state.depth_test = true;
	dir_shadow_pass->gl_state.depth_write = true;
	dir_shadow_pass->gl_state.culling = true;
	dir_shadow_pass->gl_state.cull_face = GL_BACK;
	dir_shadow_pass->gl_state.blending = false;
	dir_shadow_pass->gl_state.stencil_test = false;
	dir_shadow_pass->gl_state.clear_mask = GL_DEPTH_BUFFER_BIT;
	dir_shadow_pass->enabled = false;
	dir_shadow_pass->driver = this;

	// setup dir light stage
	light_pass * dir_pass = new light_pass;
	dir_pass->draw_calls = nse.system<nsrender_system>()->queue(DIR_LIGHT_QUEUE);
	dir_pass->ren_target = render_target(ACCUM_TARGET);
	dir_pass->gl_state.depth_test = false;
	dir_pass->gl_state.depth_write = false;
	dir_pass->gl_state.culling = true;
	dir_pass->gl_state.cull_face = GL_BACK;
	dir_pass->gl_state.blending = false;
	dir_pass->gl_state.stencil_test = false;
	dir_pass->tbuffers = m_current_context->m_tbuffers;
	dir_pass->rpass = dir_shadow_pass;
	dir_pass->driver = this;
	
	light_shadow_pass * spot_shadow_pass = new light_shadow_pass;
	spot_shadow_pass->draw_calls = nse.system<nsrender_system>()->queue(SCENE_OPAQUE_QUEUE);
	spot_shadow_pass->ren_target = render_target(SPOT_SHADOW2D_TARGET);
	spot_shadow_pass->enabled = false;
	spot_shadow_pass->gl_state.depth_test = true;
	spot_shadow_pass->gl_state.depth_write = true;
	spot_shadow_pass->gl_state.culling = true;
	spot_shadow_pass->gl_state.cull_face = GL_BACK;
	spot_shadow_pass->gl_state.blending = false;
	spot_shadow_pass->gl_state.stencil_test = false;
	spot_shadow_pass->gl_state.clear_mask = GL_DEPTH_BUFFER_BIT;
	spot_shadow_pass->driver = this;

	culled_light_pass * spot_pass = new culled_light_pass;
	spot_pass->draw_calls = nse.system<nsrender_system>()->queue(SPOT_LIGHT_QUEUE);
	spot_pass->ren_target = render_target(ACCUM_TARGET);
	spot_pass->tbuffers = m_current_context->m_tbuffers;
	spot_pass->rpass = spot_shadow_pass;
	spot_pass->gl_state.depth_test = true;
	spot_pass->gl_state.depth_write = false;
	spot_pass->gl_state.culling = false;
	spot_pass->gl_state.blending = true;
	spot_pass->gl_state.blend_eqn = GL_FUNC_ADD;
	spot_pass->gl_state.blend_func = ivec2(GL_ONE, GL_ONE);
	spot_pass->gl_state.stencil_test = true;
	spot_pass->gl_state.stencil_func = ivec3(GL_ALWAYS, 0, 0);
	spot_pass->gl_state.stencil_op_back = ivec3(GL_KEEP, GL_INCR_WRAP, GL_KEEP);
	spot_pass->gl_state.stencil_op_front = ivec3(GL_KEEP, GL_DECR_WRAP, GL_KEEP);
	spot_pass->gl_state.clear_mask = GL_STENCIL_BUFFER_BIT;
	spot_pass->driver = this;

	light_shadow_pass * point_shadow_pass = new light_shadow_pass;
	point_shadow_pass->draw_calls = nse.system<nsrender_system>()->queue(SCENE_OPAQUE_QUEUE);
	point_shadow_pass->ren_target = render_target(POINT_SHADOW_TARGET);
	point_shadow_pass->gl_state.depth_test = true;
	point_shadow_pass->gl_state.depth_write = true;
	point_shadow_pass->gl_state.culling = true;
	point_shadow_pass->gl_state.cull_face = GL_BACK;
	point_shadow_pass->gl_state.blending = false;
	point_shadow_pass->gl_state.stencil_test = false;
	point_shadow_pass->gl_state.clear_mask = GL_DEPTH_BUFFER_BIT;
	point_shadow_pass->enabled = false;
	point_shadow_pass->driver = this;

	culled_light_pass * point_pass = new culled_light_pass;
	point_pass->draw_calls = nse.system<nsrender_system>()->queue(POINT_LIGHT_QUEUE);
	point_pass->ren_target = render_target(ACCUM_TARGET);
	point_pass->tbuffers = m_current_context->m_tbuffers;
	point_pass->rpass = point_shadow_pass;
	point_pass->gl_state.depth_test = true;
	point_pass->gl_state.depth_write = false;
	point_pass->gl_state.culling = false;
	point_pass->gl_state.blending = true;
	point_pass->gl_state.blend_func = ivec2(GL_ONE, GL_ONE);
	point_pass->gl_state.stencil_test = true;
	point_pass->gl_state.stencil_func = ivec3(GL_ALWAYS, 0, 0);
	point_pass->gl_state.blend_eqn = GL_FUNC_ADD;
	point_pass->gl_state.stencil_op_back = ivec3(GL_KEEP, GL_INCR_WRAP, GL_KEEP);
	point_pass->gl_state.stencil_op_front = ivec3(GL_KEEP, GL_DECR_WRAP, GL_KEEP);
	point_pass->gl_state.clear_mask = GL_STENCIL_BUFFER_BIT;
	point_pass->driver = this;

	selection_render_pass * sel_pass_opaque = new selection_render_pass;
	sel_pass_opaque->draw_calls = nse.system<nsrender_system>()->queue(SCENE_SELECTION_QUEUE);
	sel_pass_opaque->ren_target = render_target(ACCUM_TARGET);
	sel_pass_opaque->gl_state.blending = true;
	sel_pass_opaque->gl_state.blend_func = ivec2(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	sel_pass_opaque->gl_state.blend_eqn = GL_FUNC_ADD;
	sel_pass_opaque->gl_state.culling = true;
	sel_pass_opaque->gl_state.cull_face = GL_BACK;
	sel_pass_opaque->gl_state.clear_mask = GL_STENCIL_BUFFER_BIT;
	sel_pass_opaque->gl_state.depth_test = false;
	sel_pass_opaque->gl_state.depth_write = true;
	sel_pass_opaque->gl_state.stencil_test = true;
	sel_pass_opaque->gl_state.stencil_op_back = ivec3(GL_KEEP, GL_KEEP, GL_REPLACE);
	sel_pass_opaque->gl_state.stencil_op_front = ivec3(GL_KEEP, GL_KEEP, GL_REPLACE);
	sel_pass_opaque->gl_state.stencil_func = ivec3(GL_ALWAYS, 1, -1);
	sel_pass_opaque->driver = this;

	ui_render_pass * ui_pass = new ui_render_pass;
	ui_pass->draw_calls = nse.system<nsrender_system>()->queue(UI_RENDER_QUEUE);
	ui_pass->ren_target = render_target(ACCUM_TARGET);
	ui_pass->gl_state.blending = true;
	ui_pass->gl_state.blend_func = ivec2(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	ui_pass->gl_state.blend_eqn = GL_FUNC_ADD;
	ui_pass->gl_state.culling = true;
	ui_pass->gl_state.cull_face = GL_BACK;
	ui_pass->gl_state.clear_mask = 0;
	ui_pass->gl_state.depth_test = false;
	ui_pass->gl_state.depth_write = false;
	ui_pass->gl_state.stencil_test = false;
	ui_pass->driver = this;

	final_render_pass * final_pass = new final_render_pass;
	final_pass->ren_target = m_current_context->m_default_target;
	final_pass->read_buffer = render_target(ACCUM_TARGET);
	final_pass->gl_state.depth_test = false;
	final_pass->gl_state.depth_write = false;
	final_pass->gl_state.culling = false;
	final_pass->gl_state.blending = false;
	final_pass->gl_state.stencil_test = false;
	final_pass->gl_state.cull_face = GL_BACK;
	final_pass->use_vp_size = true;
	final_pass->driver = this;

	m_current_context->m_render_passes.push_back(gbuf_pass);
    m_current_context->m_render_passes.push_back(oit_pass);
    m_current_context->m_render_passes.push_back(dir_shadow_pass);
	m_current_context->m_render_passes.push_back(dir_pass);
    m_current_context->m_render_passes.push_back(spot_shadow_pass);
    m_current_context->m_render_passes.push_back(spot_pass);
    m_current_context->m_render_passes.push_back(point_shadow_pass);
    m_current_context->m_render_passes.push_back(point_pass);
    m_current_context->m_render_passes.push_back(sel_pass_opaque);
    m_current_context->m_render_passes.push_back(ui_pass);
	m_current_context->m_render_passes.push_back(final_pass);
}

void nsopengl_driver::resize_screen(const ivec2 & new_size)
{
	m_current_context->m_default_target->resize(new_size);
}

void nsopengl_driver::release()
{
	clear_render_targets();
	clear_render_passes();
	m_current_context->release();
	destroy_context(m_current_context->context_id);
	nsvideo_driver::release();
}

void nsopengl_driver::render(nsrender::viewport * vp)
{
	if (!_valid_check())
		return;

    m_current_context->m_render_passes[oit]->enabled = vp->order_independent_transparency;
    //m_current_context->m_render_passes[dir_shadow]->enabled = vp->dir_light_shadows;
    m_current_context->m_render_passes[dir_light]->enabled = vp->dir_lights;
    //m_current_context->m_render_passes[spot_shadow]->enabled = vp->spot_light_shadows;
    m_current_context->m_render_passes[spot_light]->enabled = vp->spot_lights;
    //m_current_context->m_render_passes[point_shadow]->enabled = vp->point_light_shadows;
    m_current_context->m_render_passes[point_light]->enabled = vp->point_lights;
    m_current_context->m_render_passes[selection]->enabled = vp->picking_enabled;

	for (uint32 i = 0; i < m_current_context->m_render_passes.size(); ++i)
	{
		render_pass * rp = m_current_context->m_render_passes[i];
		if (rp == nullptr || rp->ren_target == nullptr)
		{
			dprint("nsvideo_driver::_render_pass - pass " + std::to_string(i) + " is not correctly setup");
		}
		else if (rp->enabled)
		{
			rp->vp = vp;
			rp->setup_pass();
			rp->render();
		}
	}

	m_current_context->m_tbuffers->reset_atomic_counter();
}

void nsopengl_driver::set_gl_state(const opengl_state & state)
{
	set_viewport(state.current_viewport);
	enable_depth_test(state.depth_test);
	enable_depth_write(state.depth_write);
	enable_stencil_test(state.stencil_test);
	enable_blending(state.blending);
	enable_culling(state.culling);

	if (state.stencil_test)
	{
		set_stencil_func(state.stencil_func);
		set_stencil_op_back(state.stencil_op_back);
		set_stencil_op_front(state.stencil_op_front);
	}

	if (state.culling)
		set_cull_face(state.cull_face);

	if (state.blending)
	{
		set_blend_func(state.blend_func);
		set_blend_eqn(state.blend_eqn);
	}

	if (state.clear_mask != 0)
		clear_framebuffer(state.clear_mask);
}

void nsopengl_driver::set_blend_eqn(int32 eqn)
{
	if (eqn == 0)
		return;
	
	glBlendEquation(eqn);
	m_current_context->m_gl_state.blend_eqn = eqn;
	gl_err_check("nsopengl_driver::set_blend_eqn");	
}

bool nsopengl_driver::_valid_check()
{
	static bool shadererr = false;
	static bool shadernull = false;

	if (!nse.system<nsrender_system>()->get_render_shaders().valid())
	{
		if (!shadernull)
		{
			dprint("nsopengl_driver::_validCheck: Error - one of the rendering shaders is nullptr ");
			shadernull = true;
		}
		return false;
	}
	shadernull = false;

	if (nse.system<nsrender_system>()->get_render_shaders().error())
	{
		if (!shadererr)
		{
			dprint("nsopengl_driver::_validCheck: Error in one of the rendering shaders - check compile log");
			shadererr = true;
		}
		return false;
	}
	shadererr = false;
	return true;
}

void nsopengl_driver::enable_depth_write(bool enable)
{
	glDepthMask(enable);
	m_current_context->m_gl_state.depth_write = enable;
	gl_err_check("nsopengl_driver::enable_depth_write");
}

void nsopengl_driver::enable_depth_test(bool enable)
{
	if (enable)
	{
		glEnable(GL_DEPTH_TEST);
	}
	else
	{
		glDisable(GL_DEPTH_TEST);
		m_current_context->m_gl_state.depth_write = enable;
	}
	gl_err_check("nsopengl_driver::enable_depth_test");
	m_current_context->m_gl_state.depth_test = enable;
}

void nsopengl_driver::enable_stencil_test(bool enable)
{
	if (enable)
		glEnable(GL_STENCIL_TEST);
	else
		glDisable(GL_STENCIL_TEST);
	gl_err_check("nsopengl_driver::enable_stencil_test");
	m_current_context->m_gl_state.stencil_test = enable;
}

void nsopengl_driver::enable_blending(bool enable)
{
	if (enable)
		glEnable(GL_BLEND);
	else
		glDisable(GL_BLEND);
	m_current_context->m_gl_state.blending = enable;
	gl_err_check("nsopengl_driver::enable_blending");
}

void nsopengl_driver::enable_culling(bool enable)
{
	if (enable)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);
	m_current_context->m_gl_state.culling = enable;
	gl_err_check("nsopengl_driver::enable_culling");
}

void nsopengl_driver::set_cull_face(int32 cull_face)
{
	if (cull_face != m_current_context->m_gl_state.cull_face)
	{
		glCullFace(cull_face);
		m_current_context->m_gl_state.cull_face = cull_face;
		gl_err_check("nsopengl_driver::set_cull_face");
	}	
}

opengl_state nsopengl_driver::current_gl_state()
{
	return m_current_context->m_gl_state;
}

void nsopengl_driver::set_blend_func(int32 sfactor, int32 dfactor)
{
	set_blend_func(ivec2(sfactor,dfactor));
}

void nsopengl_driver::set_blend_func(const ivec2 & blend_func)
{
	glBlendFunc(blend_func.x, blend_func.y);
	m_current_context->m_gl_state.blend_func = blend_func;
	gl_err_check("nsopengl_driver::set_blend_func");
}

void nsopengl_driver::set_stencil_func(int32 func, int32 ref, int32 mask)
{
	set_stencil_func(ivec3(func,ref,mask));
}

void nsopengl_driver::set_stencil_func(const ivec3 & stencil_func)
{
	glStencilFunc(stencil_func.x, stencil_func.y, stencil_func.z);
	m_current_context->m_gl_state.stencil_func = stencil_func;
	gl_err_check("nsopengl_driver::set_stencil_func");
}

void nsopengl_driver::set_stencil_op(int32 sfail, int32 dpfail, int32 dppass)
{
	set_stencil_op(ivec3( sfail, dpfail, dppass));
}

void nsopengl_driver::set_stencil_op(const ivec3 & stencil_op)
{
	glStencilOp(stencil_op.x, stencil_op.y, stencil_op.z);
	m_current_context->m_gl_state.stencil_op_back = stencil_op;
	m_current_context->m_gl_state.stencil_op_front = stencil_op;
	gl_err_check("nsopengl_driver::set_stencil_op");
}

void nsopengl_driver::set_stencil_op_back(int32 sfail, int32 dpfail, int32 dppass)
{
	set_stencil_op_back(ivec3(sfail, dpfail, dppass));	
}
	
void nsopengl_driver::set_stencil_op_back(const ivec3 & stencil_op)
{
	glStencilOpSeparate(GL_BACK, stencil_op.x, stencil_op.y, stencil_op.z);
	m_current_context->m_gl_state.stencil_op_back = stencil_op;
	gl_err_check("nsopengl_driver::set_stencil_op_back");
}

void nsopengl_driver::set_stencil_op_front(int32 sfail, int32 dpfail, int32 dppass)
{
	set_stencil_op_front(ivec3(sfail, dpfail, dppass));		
}
	
void nsopengl_driver::set_stencil_op_front(const ivec3 & stencil_op)
{
	glStencilOpSeparate(GL_FRONT, stencil_op.x, stencil_op.y, stencil_op.z);
	m_current_context->m_gl_state.stencil_op_front = stencil_op;
	gl_err_check("nsopengl_driver::set_stencil_op_front");
}

uint32 nsopengl_driver::bound_fbo()
{
	GLint params;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &params);
	return params;
}

uint32 nsopengl_driver::active_tex_unit()
{
	GLint act_un;
	glGetIntegerv(GL_ACTIVE_TEXTURE, &act_un);
	return act_un - BASE_TEX_UNIT;
}

void nsopengl_driver::init(bool setup_default_rend)
{
	if (m_current_context == nullptr)
	{
		dprint("nsopengl_driver::init You must first create a context before initializing");
		return;
	}
	m_current_context->init();
	nsvideo_driver::init(setup_default_rend);
}

void nsopengl_driver::set_active_texture_unit(uint32 tex_unit)
{
	glActiveTexture(BASE_TEX_UNIT + tex_unit);
	gl_err_check("nsopengl_driver::set_active_texture_unit");
}

void nsopengl_driver::set_viewport(const ivec4 & val)
{
	if (m_current_context->m_gl_state.current_viewport != val)
	{
		glViewport(val.x, val.y, val.z, val.w);
		m_current_context->m_gl_state.current_viewport = val;
		gl_err_check("nsopengl_driver::set_viewport");
	}
}

void nsopengl_driver::clear_framebuffer(uint32 clear_mask)
{
	glClear(clear_mask);
	gl_err_check("nsopengl_driver::clear_framebuffer");
}

void nsopengl_driver::bind_textures(nsmaterial * material)
{
	nsmaterial::texmap_map_const_iter cIter = material->begin();
	while (cIter != material->end())
	{
		nstexture * t = get_resource<nstexture>(cIter->second.tex_id);
		if (t != nullptr)
		{
			set_active_texture_unit(cIter->first);
			t->bind();
		}
		++cIter;
	}	
}

uivec3 nsopengl_driver::pick(const fvec2 & mouse_pos)
{
	nsgl_framebuffer * pck = (nsgl_framebuffer*)render_target(ACCUM_TARGET);
	if (pck == NULL)
		return uivec3();

	fvec2 screen_size = fvec2(m_current_context->m_default_target->size().x, m_current_context->m_default_target->size().y);
	fvec2 accum_size = fvec2(pck->size().x,pck->size().y);
	fvec2 rmpos = mouse_pos % (screen_size / accum_size);
	uivec3 index = pck->pick(rmpos.x, rmpos.y, 1);
	return index;
}

void nsopengl_driver::bind_gbuffer_textures(nsgl_framebuffer * fb)
{
	nsgbuf_object * obj = dynamic_cast<nsgbuf_object*>(fb);

	if (obj == nullptr)
		return;

	for (uint32 i = 0; i < nsgbuf_object::attrib_count; ++i)
	{
		nsgl_framebuffer::attachment * att = obj->color(i);
		set_active_texture_unit(att->m_tex_unit);
		att->m_texture->bind();
	}
}

void nsopengl_driver::init_texture(nstexture * tex)
{
	if (tex->video_texture() != nullptr)
	{
		dprint("nsopengl_driver::init_texture Trying to initialize already initialized gl_texture");
		return;
	}
	nsgl_texture * gltex = new nsgl_texture;
	gltex->video_init();
	if (tex->type() == type_to_hash(nstex1d))
	{
		gltex->m_target = nsgl_texture::tex_1d;
	}
	else if (tex->type() == type_to_hash(nstex2d))
	{
		gltex->m_target = nsgl_texture::tex_2d;
		gltex->set_parameter_i(nsgl_texture::mag_filter, GL_LINEAR);
		gltex->set_parameter_i(nsgl_texture::min_filter, GL_LINEAR_MIPMAP_LINEAR);
	}
	else if (tex->type() == type_to_hash(nstex3d))
	{
		gltex->m_target = nsgl_texture::tex_3d;
	}
	else if (tex->type() == type_to_hash(nstex_cubemap))
	{
		gltex->m_target = nsgl_texture::tex_cubemap;
	}
	else
	{
		dprint("nsopengl_driver::init_texture - Unrecognized texture type");
	}
	tex->set_video_texture(gltex);
}

gl_ctxt * nsopengl_driver::context(uint8 id)
{
	if (id >= MAX_CONTEXT_COUNT)
		return nullptr;
	return m_contexts[id];
}

void nsopengl_driver::release_texture(nstexture * tex)
{
	nsgl_texture * gltex = tex->video_texture<nsgl_texture>();
	if (gltex == nullptr)
	{
		dprint("nsopengl_driver::release_texture Trying to release uninitialized gl_texture");
		return;
	}
	gltex->video_release();
	if (gltex->gl_obj.all_ids_released())
	{
		delete gltex;
		return;
	}
	for (uint8 i = 0; i < MAX_CONTEXT_COUNT; ++i)
	{
		if (gltex->gl_obj.m_gl_name[i] != 0)
		{
			vid_obj_rel vobj;
			vobj.vo = gltex;
			vobj.gl_obj = &gltex->gl_obj;
			
			gl_ctxt * ctx = m_contexts[i];
			if (ctx == nullptr)
			{
				dprint("nsopengl_texture::release_texture Crash: non zero gl id when context is null");
			}
			ctx->need_release.push_back(vobj);
		}
	}
	tex->set_video_texture(nullptr);
}

void nsopengl_driver::cleanup_vid_objs()
{
	for (uint32 i = 0; i < current_context()->need_release.size(); ++i)
	{
		current_context()->need_release[i].vo->video_release();
		if (current_context()->need_release[i].gl_obj->all_ids_released())
			delete current_context()->need_release[i].vo;
	}
	current_context()->need_release.clear();
}

void nsopengl_driver::enable_auto_cleanup(bool enable)
{
	m_auto_cleanup = enable;
}

bool nsopengl_driver::auto_cleanup()
{
	return m_auto_cleanup;
}

void nsopengl_driver::render_instanced_dc(instanced_draw_call * idc)
{
	idc->submesh->m_vao.bind();

	if (idc->transform_buffer != nullptr)
	{
		idc->transform_buffer->bind();
		for (uint32 tfInd = 0; tfInd < 4; ++tfInd)
		{
			idc->submesh->m_vao.add(idc->transform_buffer, nsshader::loc_instance_tform + tfInd);
			idc->submesh->m_vao.vertex_attrib_ptr(nsshader::loc_instance_tform + tfInd, 4, GL_FLOAT, GL_FALSE, sizeof(fmat4), sizeof(fvec4)*tfInd);
			idc->submesh->m_vao.vertex_attrib_div(nsshader::loc_instance_tform + tfInd, 1);
		}
	}

	if (idc->transform_id_buffer != nullptr)
	{
		idc->transform_id_buffer->bind();
		idc->submesh->m_vao.add(idc->transform_id_buffer, nsshader::loc_ref_id);
		idc->submesh->m_vao.vertex_attrib_I_ptr(nsshader::loc_ref_id, 1, GL_UNSIGNED_INT, sizeof(uint32), 0);
		idc->submesh->m_vao.vertex_attrib_div(nsshader::loc_ref_id, 1);	
	}
	
	gl_err_check("instanced_geometry_draw_call::render pre");
	glDrawElementsInstanced(idc->submesh->m_prim_type,
							static_cast<GLsizei>(idc->submesh->m_indices.size()),
							GL_UNSIGNED_INT,
							0,
							idc->transform_count);
	gl_err_check("instanced_geometry_draw_call::render post");	

	if (idc->transform_buffer != nullptr)
	{
		idc->transform_buffer->bind();
		idc->submesh->m_vao.remove(idc->transform_buffer);
	}

	if (idc->transform_id_buffer != nullptr)
	{
		idc->transform_id_buffer->bind();
		idc->submesh->m_vao.remove(idc->transform_id_buffer);
	}
	idc->submesh->m_vao.unbind();
}

void nsopengl_driver::render_light_dc(light_draw_call * idc)
{
	gl_err_check("pre dir_light_pass::render");
	if (idc->submesh == nullptr)
	{
		m_current_context->m_single_point->bind();
		glDrawArrays(GL_POINTS, 0, 1);
		m_current_context->m_single_point->unbind();
	}
	else
	{
		idc->submesh->m_vao.bind();
		glDrawElements(idc->submesh->m_prim_type,
					   static_cast<GLsizei>(idc->submesh->m_indices.size()),
					   GL_UNSIGNED_INT,
					   0);
		idc->submesh->m_vao.unbind();
	}
	gl_err_check("post dir_light_pass::render");	
}

void nsopengl_driver::render_ui_dc(ui_draw_call * idc)
{
	gl_err_check("pre ui_draw_call::render");
	if (m_current_context->m_single_point != nullptr)
	{
		m_current_context->m_single_point->bind();
		glDrawArrays(GL_POINTS, 0, 1);
		m_current_context->m_single_point->unbind();
	}
    gl_err_check("post ui_draw_call::render");	
}
