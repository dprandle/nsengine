/*!
  \file nsgl_render_passes.cpp

  \brief Definition file for nsgl_render_passes class

  This file contains all of the neccessary definitions for the nsgl_render_passes class.

  \author Daniel Randle
  \date Feb 23 2016
  \copywrite Earth Banana Games 2013
*/

#include <nsgl_render_passes.h>
#include <nsgl_driver.h>
#include <nsgl_vid_objs.h>
#include <nsgl_shader.h>
#include <nsgl_buffer.h>
#include <nsgl_framebuffer.h>
#include <nsgl_draw_calls.h>
#include <nsgl_gbuffer.h>
#include <nsgl_texture.h>
#include <nsgl_vao.h>
#include <nsmaterial.h>
#include <nstexture.h>
#include <nsshader.h>
#include <nsmesh.h>
#include <nsfont.h>
#include <nsentity.h>
#include <nscam_comp.h>
#include <nslight_comp.h>

void gl_render_pass::setup()
{
	nsgl_framebuffer * tgt = ren_target;
	tgt->target = nsgl_framebuffer::fb_draw;
	tgt->bind();

	if (use_vp_size && vp != nullptr)
		gl_state.current_viewport = ivec4(vp->bounds.xy(), vp->bounds.zw() - vp->bounds.xy());
	else
		gl_state.current_viewport = ivec4(0, 0, tgt->size);
	
	driver_ctxt->set_gl_state(gl_state);
}

void ui_render_pass::render()
{
	ren_target->bind();
	ren_target->update_draw_buffers();
	const ivec4 & viewp = gl_state.current_viewport;
	
	for (uint32 i = 0; i < rq->size(); ++i)
	{
		ui_draw_call * uidc = (ui_draw_call*)(*rq)[i];		

        if (uidc->shdr != nullptr)
		{
            nsgl_shader * gl_shdr = uidc->shdr->video_obj<nsgl_shader_obj>()->gl_shdr;
			gl_shdr->bind();
			gl_shdr->set_uniform("uitexture", DIFFUSE_TEX_UNIT);
			gl_shdr->set_uniform("entity_id", uidc->entity_id);
			gl_shdr->set_uniform("viewport", fvec4(0,0,viewp.z,viewp.w));
			gl_shdr->set_uniform("wscale", uidc->content_wscale);
			gl_shdr->set_uniform("content_tform", uidc->content_tform);
			gl_shdr->set_uniform("pixel_blend", 2.0f);

			driver_ctxt->enable_stencil_test(true);
			driver_ctxt->set_stencil_func(GL_ALWAYS, 1, 0xFF);
			driver_ctxt->set_stencil_op(GL_KEEP, GL_KEEP, GL_REPLACE);			
			
			gl_shdr->set_uniform("border_rad_top", uidc->top_border_radius);
			gl_shdr->set_uniform("border_rad_bottom", uidc->bottom_border_radius);
			gl_shdr->set_uniform("frag_color_out", uidc->mat->color());
			gl_shdr->set_uniform("color_mode", uidc->mat->color_mode());
			gl_shdr->set_uniform("border_pix", fvec4(0.0f));
			tex_map_info ti = uidc->mat->mat_tex_info(nsmaterial::diffuse);
			gl_shdr->set_uniform("tex_coord_rect", ti.coord_rect);
			gl_shdr->set_uniform("color_mult", ti.color_mult);
			gl_shdr->set_uniform("color_add", ti.color_add);
			driver_ctxt->enable_culling(uidc->mat->culling());
			driver_ctxt->set_cull_face(uidc->mat->cull_mode());
			driver_ctxt->bind_textures(uidc->mat);
            driver_ctxt->render_ui_dc(uidc, gl_shdr);

			driver_ctxt->set_stencil_func(GL_NOTEQUAL, 1, 0xFF);
			driver_ctxt->set_stencil_op(GL_KEEP, GL_KEEP, GL_ZERO);			

			if (uidc->top_border_radius != fvec4(0.0f))
			{
				gl_shdr->set_uniform("border_rad_top", uidc->top_border_radius +
																			   fvec4(uidc->border_pix.xw(),uidc->border_pix.zw()));
			}
			else
			{
				gl_shdr->set_uniform("border_rad_top", fvec4(0.0f));
			}

			if (uidc->bottom_border_radius != fvec4(0.0f))
			{
				gl_shdr->set_uniform("border_rad_bottom", uidc->bottom_border_radius +
																			   fvec4(uidc->border_pix.xy(),uidc->border_pix.zy()));
			}
			else
			{
				gl_shdr->set_uniform("border_rad_bottom", fvec4(0.0f));
			}
			
			gl_shdr->set_uniform("border_pix", uidc->border_pix);
			gl_shdr->set_uniform("color_mode", uidc->border_mat->color_mode());
			gl_shdr->set_uniform("frag_color_out", uidc->border_mat->color());
			ti = uidc->border_mat->mat_tex_info(nsmaterial::diffuse);
			gl_shdr->set_uniform("tex_coord_rect", ti.coord_rect);
			gl_shdr->set_uniform("color_mult", ti.color_mult);
			gl_shdr->set_uniform("color_add", ti.color_add);
			driver_ctxt->enable_culling(uidc->border_mat->culling());
			driver_ctxt->set_cull_face(uidc->border_mat->cull_mode());
            driver_ctxt->render_ui_dc(uidc, gl_shdr);
		}

		driver_ctxt->set_stencil_func(GL_EQUAL, 1, 0xFF);
        // render the text part of the ui-element
		if (uidc->fnt != nullptr)
		{
			uint8 face_index = 0;
			
			nstexture * atlas = uidc->fnt->get_atlas(face_index);
			if (atlas != nullptr)
			{
				nsgl_shader * gl_txt_shdr = uidc->text_shader->video_obj<nsgl_shader_obj>()->gl_shdr;
				gl_txt_shdr->bind();
				gl_txt_shdr->set_uniform("uitexture", DIFFUSE_TEX_UNIT);
				gl_txt_shdr->set_uniform("entity_id", uidc->entity_id);
				gl_txt_shdr->set_uniform("viewport", fvec4(0,0,viewp.z,viewp.w));
				gl_txt_shdr->set_uniform("content_tform", uidc->content_tform);
				gl_txt_shdr->set_uniform("frag_color_out", uidc->fnt_material->color());

				tex_map_info ti = uidc->fnt_material->mat_tex_info(nsmaterial::diffuse);
				gl_txt_shdr->set_uniform("color_mult", ti.color_mult);
				gl_txt_shdr->set_uniform("color_add", ti.color_add);

				fvec2 rect_wh = fvec2(viewp.z, viewp.w) % uidc->content_wscale;
				fvec2 cursor_pos(0,0);
				int32 cur_line = 0;
				int32 line_index = 0;
				float line_height = uidc->fnt->line_height(face_index);
				std::vector<float> x_offsets;

				// figure out the vertical starting cursor pos
				if (uidc->alignment < 3) // top alignment
				{
					cursor_pos.y = rect_wh.h - line_height*0.75f - uidc->text_margins.w;
				}
				else if (uidc->alignment < 6) // middle alignment
				{
					float cntr_pt = (rect_wh.h - (uidc->text_margins.y + uidc->text_margins.w + line_height/2.0f)) / 2.0f;
					float v_offset_factor = float(uidc->text_line_sizes.size()-1) / 2.0f;
					cursor_pos.y = cntr_pt + v_offset_factor * line_height;
				}
				else // bottom alignment
				{
					cursor_pos.y = uidc->text_margins.y + (uidc->text_line_sizes.size() -1) * line_height * 0.75f;
				}

				// figure out the first line's horizontal pos
				if (uidc->alignment % 3 == 0) // left
				{
					x_offsets.resize(uidc->text_line_sizes.size(), uidc->text_margins.x);
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
							char_info * ci = uidc->fnt->get_char_info(face_index, uidc->text[ti]);
							xoff += ci->adv.x;
							++ti;
						}
						++ti; // newline char
						x_offsets[i] = (rect_wh.w - uidc->text_margins.x - uidc->text_margins.z - xoff) / 2.0f;
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
							char_info * ci = uidc->fnt->get_char_info(face_index, uidc->text[ti]);
							xoff += ci->adv.x;
							++ti;
						}
						++ti;
						x_offsets[i] = (rect_wh.w - uidc->text_margins.z - xoff);
					}
				}
			
				cursor_pos.x = x_offsets[0];
				fvec2 cursor_xy(x_offsets[uidc->cursor_offset.y],cursor_pos.y);
				for (uint32 i = 0; i < uidc->text.size(); ++i)
				{
					if (uidc->text[i] == '\n')
					{
						cursor_pos.y -= line_height;
						line_index = 0;
						++cur_line;
						cursor_pos.x = x_offsets[cur_line];
						if (cur_line == uidc->cursor_offset.y)
							cursor_xy.y = cursor_pos.y;
						continue;
					}

					char_info * ci = uidc->fnt->get_char_info(face_index, uidc->text[i]);

					driver_ctxt->set_active_texture_unit(nsmaterial::diffuse);
					atlas->video_obj<nsgl_texture_obj>()->gl_tex->bind();

					ivec2 tsz(ci->bm_size);
					fvec2 tex_size(tsz.x,tsz.y);

					gl_txt_shdr->set_uniform("pixel_wh", ci->bm_size);
					gl_txt_shdr->set_uniform("drawing_cursor", false);

					gl_txt_shdr->set_uniform(
						"offset_xy",
						fvec2(cursor_pos.x + ci->bearing.x, cursor_pos.y - (ci->bm_size.y - ci->bearing.y)));
					
					gl_txt_shdr->set_uniform("tex_coord_rect", ci->tc);

					cursor_pos.x += ci->adv.x;
					++line_index;

					if (cur_line == uidc->cursor_offset.y && line_index == uidc->cursor_offset.x)
						cursor_xy.x = cursor_pos.x;				

					driver_ctxt->render_ui_dc(uidc, gl_txt_shdr);
				}

// lets draw the cursor if the text is editable - ie has a text input component
				if (uidc->text_editable)
				{
					gl_txt_shdr->set_uniform("drawing_cursor", true);
					gl_txt_shdr->set_uniform("frag_color_out", uidc->cursor_color);
					gl_txt_shdr->set_uniform("color_mult", fvec4(1.0f));
					gl_txt_shdr->set_uniform("color_add", fvec4(0.0f));
					gl_txt_shdr->set_uniform("pixel_wh", fvec2(uidc->cursor_pixel_width, line_height));
					gl_txt_shdr->set_uniform("offset_xy",fvec2(cursor_xy.x, cursor_xy.y - line_height/4.0f));
					driver_ctxt->render_ui_dc(uidc, gl_txt_shdr);
				}
			}		
		}
		driver_ctxt->enable_stencil_test(false);
	}
}

void selection_render_pass::render()
{
	for (uint32 i = 0; i < rq->size(); ++i)
	{
		instanced_draw_call * dc = (instanced_draw_call*)(*rq)[i];
		ren_target->set_draw_buffer(nsgl_framebuffer::att_none);

		nsgl_shader * gl_shdr = dc->shdr->video_obj<nsgl_shader_obj>()->gl_shdr;
		gl_shdr->bind();
		gl_shdr->set_uniform("heightMap", HEIGHT_TEX_UNIT);
		if (vp->camera != nullptr)
			gl_shdr->set_uniform("projCamMat",vp->camera->get<nscam_comp>()->proj_cam());
		
		if (dc->submesh->m_node != nullptr)
			gl_shdr->set_uniform("nodeTransform", dc->submesh->m_node->m_world_tform);
		else
			gl_shdr->set_uniform("nodeTransform", fmat4());

		if (dc->anim_transforms != nullptr)
		{
			for (uint32 i = 0; i < dc->anim_transforms->size(); ++i)
				gl_shdr->set_uniform("boneTransforms[" + std::to_string(i) + "]", (*dc->anim_transforms)[i]);
			gl_shdr->set_uniform("hasBones", true);
		}
		else
		{
			gl_shdr->set_uniform("hasBones", false);
		}
		gl_shdr->set_uniform("hasHeightMap", dc->mat->contains(nsmaterial::height));
		gl_shdr->set_uniform("hminmax", dc->height_minmax);
        driver_ctxt->render_instanced_dc(dc, gl_shdr);
		
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        gl_err_check("selection_render_pass::render - glPolygonMode error 1");

        glEnable(GL_LINE_SMOOTH);
        gl_err_check("selection_render_pass::render - glEnable(GL_LINE_SMOOTH) error 1");

        float lineWidth[2];
        glGetFloatv(GL_LINE_WIDTH_RANGE, lineWidth);
		if (lineWidth[1] > 4.0f)
			lineWidth[1] = 4.0f;
        gl_err_check("selection_render_pass::render - glGetFloatv(GL_LINE_WIDTH_RANGE, lineWidth)");

        glLineWidth(lineWidth[1]);
        gl_err_check("selection_render_pass::render - glLineWidth error 1");

		driver_ctxt->set_stencil_func(GL_NOTEQUAL, 1, -1);
		ren_target->set_draw_buffer(nsgl_framebuffer::att_color);
		gl_shdr->set_uniform("fragColOut", fvec4(dc->sel_color.rgb(),1.0f));	
        driver_ctxt->render_instanced_dc(dc, gl_shdr);
		
        glLineWidth(lineWidth[0]);
        gl_err_check("selection_render_pass::render - glLineWidth error 2");

        glDisable(GL_LINE_SMOOTH);
        gl_err_check("selection_render_pass::render - glEnable(GL_LINE_SMOOTH) error 2");

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        gl_err_check("selection_render_pass::render - glPolygonMode error 2");

		driver_ctxt->enable_depth_test(false);
		driver_ctxt->set_stencil_func(GL_EQUAL, 1, 0);
		gl_shdr->set_uniform("fragColOut", dc->sel_color);
        driver_ctxt->render_instanced_dc(dc, gl_shdr);
	}	
}

void gbuffer_render_pass::render()
{
	for (uint32 i = 0; i < rq->size(); ++i)
	{
		instanced_draw_call * dc = (instanced_draw_call*)(*rq)[i];
		ivec4 & viewp = gl_state.current_viewport;
		nsgl_shader * gl_shdr = dc->shdr->video_obj<nsgl_shader_obj>()->gl_shdr;
		
		gl_shdr->bind();
		gl_shdr->set_uniform("diffuseMap", DIFFUSE_TEX_UNIT);
		gl_shdr->set_uniform("normalMap", NORMAL_TEX_UNIT);
		gl_shdr->set_uniform("opacityMap", OPACITY_TEX_UNIT);
		gl_shdr->set_uniform("heightMap", HEIGHT_TEX_UNIT);
		gl_shdr->set_uniform("viewport", fvec4(viewp.x, viewp.y, viewp.z, viewp.w));
		fmat4 proj_cam = vp->camera->get<nscam_comp>()->proj_cam();
		if (vp->camera != nullptr)
			gl_shdr->set_uniform("projCamMat", proj_cam);

		gl_shdr->set_uniform("hasHeightMap", dc->mat->contains(nsmaterial::height));
		gl_shdr->set_uniform("hasDiffuseMap", dc->mat->contains(nsmaterial::diffuse));
		gl_shdr->set_uniform("hasOpacityMap", dc->mat->contains(nsmaterial::opacity));
		gl_shdr->set_uniform("hasNormalMap", dc->mat->contains(nsmaterial::normal));
		gl_shdr->set_uniform("colorMode", dc->mat->color_mode());
		gl_shdr->set_uniform("fragColOut", dc->mat->color());
		gl_shdr->set_uniform("force_alpha", dc->mat->using_alpha_from_color());
		gl_shdr->set_uniform("material_id",  dc->mat_index);

		uint32 ent_id = dc->entity_id;
		if (dc->transparent_picking)
			ent_id = 0;

		gl_shdr->set_uniform("hminmax", dc->height_minmax);
		gl_shdr->set_uniform("entityID", ent_id);
		gl_shdr->set_uniform("pluginID", dc->plugin_id);

		if (dc->submesh->m_node != NULL)
			gl_shdr->set_uniform("nodeTransform", dc->submesh->m_node->m_world_tform);
		else
			gl_shdr->set_uniform("nodeTransform", fmat4());

		if (!dc->submesh->m_has_tex_coords)
			gl_shdr->set_uniform("colorMode", true);

		if (dc->anim_transforms != NULL)
		{
			gl_shdr->set_uniform("hasBones", true);
			for (uint32 i = 0; i < dc->anim_transforms->size(); ++i)
				gl_shdr->set_uniform("boneTransforms[" + std::to_string(i) + "]", (*dc->anim_transforms)[i]);
		}
		else
			gl_shdr->set_uniform("hasBones", false);

		driver_ctxt->enable_culling(dc->mat->culling());
		driver_ctxt->set_cull_face(dc->mat->cull_mode());
		driver_ctxt->bind_textures(dc->mat);
        driver_ctxt->render_instanced_dc(dc, gl_shdr);
	}
    driver_ctxt->bind_gbuffer_textures(ren_target);
}

#ifdef ORDER_INDEPENDENT_TRANSLUCENCY
void oit_render_pass::render()
{
	ivec4 viewp = gl_state.current_viewport;
	ren_target->set_draw_buffer(nsgl_framebuffer::att_none);
	for (uint32 i = 0; i < rq->size(); ++i)
	{
		instanced_draw_call * dc = (instanced_draw_call*)(*rq)[i];
		nsgl_shader * gl_shdr = dc->shdr->video_obj<nsgl_shader_obj>()->gl_shdr;
		
		gl_shdr->bind();
		gl_shdr->set_uniform("diffuseMap", DIFFUSE_TEX_UNIT);
		gl_shdr->set_uniform("normalMap", NORMAL_TEX_UNIT);
		gl_shdr->set_uniform("opacityMap", OPACITY_TEX_UNIT);
		gl_shdr->set_uniform("heightMap", HEIGHT_TEX_UNIT);
		gl_shdr->set_uniform("viewport", fvec4(viewp.x, viewp.y, viewp.z, viewp.w));
		if (vp->camera != nullptr)
			gl_shdr->set_uniform("projCamMat", vp->camera->get<nscam_comp>()->proj_cam());	

		gl_shdr->set_uniform("hasHeightMap", dc->mat->contains(nsmaterial::height));
		gl_shdr->set_uniform("hasDiffuseMap", dc->mat->contains(nsmaterial::diffuse));
		gl_shdr->set_uniform("hasOpacityMap", dc->mat->contains(nsmaterial::opacity));
		gl_shdr->set_uniform("hasNormalMap", dc->mat->contains(nsmaterial::normal));
		gl_shdr->set_uniform("colorMode", dc->mat->color_mode());
		gl_shdr->set_uniform("fragColOut", dc->mat->color());
		gl_shdr->set_uniform("force_alpha", dc->mat->using_alpha_from_color());
		gl_shdr->set_uniform("material_id",  dc->mat_index);

		uint32 ent_id = dc->entity_id;
		if (dc->transparent_picking)
			ent_id = 0;

		gl_shdr->set_uniform("hminmax", dc->height_minmax);
		gl_shdr->set_uniform("entityID", ent_id);
		gl_shdr->set_uniform("pluginID", dc->plugin_id);

		if (dc->submesh->m_node != NULL)
			gl_shdr->set_uniform("nodeTransform", dc->submesh->m_node->m_world_tform);
		else
			gl_shdr->set_uniform("nodeTransform", fmat4());

		if (!dc->submesh->m_has_tex_coords)
			gl_shdr->set_uniform("colorMode", true);

		if (dc->anim_transforms != NULL)
		{
			gl_shdr->set_uniform("hasBones", true);
			for (uint32 i = 0; i < dc->anim_transforms->size(); ++i)
				gl_shdr->set_uniform("boneTransforms[" + std::to_string(i) + "]", (*dc->anim_transforms)[i]);
		}
		else
			gl_shdr->set_uniform("hasBones", false);

		if (dc->mat != nullptr)
		{
			driver_ctxt->enable_culling(dc->mat->culling());
			driver_ctxt->set_cull_face(dc->mat->cull_mode());
			driver_ctxt->bind_textures(dc->mat);
		}
	    tbuffers->bind_buffers();
        driver_ctxt->render_instanced_dc(dc, gl_shdr);
	}
	ren_target->update_draw_buffers();
	driver_ctxt->enable_depth_test(false);
	driver_ctxt->enable_culling(true);
	driver_ctxt->set_cull_face(GL_BACK);

	nsgl_shader * fsort = nse.video_driver<nsgl_driver>()->rshaders.frag_sort->video_obj<nsgl_shader_obj>()->gl_shdr;
	fsort->bind();
	fsort->set_uniform("gMatMap", int32(G_PICKING_TEX_UNIT));
	fsort->set_uniform("viewport", fvec4(viewp.x, viewp.y, viewp.z, viewp.w));
    fsort->set_uniform("gbuf_tex_size", fvec2(ren_target->size.x,ren_target->size.y));

    nsmesh::submesh * sub = nse.core()->get<nsmesh>(MESH_DIRLIGHT_BOUNDS)->sub(0);
    nsgl_submesh_obj * so = sub->video_obj<nsgl_submesh_obj>();
    so->gl_vao->bind();
    tbuffers->bind_buffers();
    gl_err_check("pre oit_render_pass::render pre");
    glDrawElements(get_gl_prim_type(sub->m_prim_type),
                   static_cast<GLsizei>(sub->m_indices.size()),
                   GL_UNSIGNED_INT,
                   0);
    gl_err_check("post oit_render_pass::render post");
    tbuffers->unbind_buffers();
    so->gl_vao->unbind();
}
#endif

void light_shadow_pass::render()
{
	nsgl_shader * cur_shdr = nullptr;
	ivec4 & viewp = gl_state.current_viewport;
	if (ldc->light_type == nslight_comp::l_point)
	{
		cur_shdr = driver_ctxt->driver->rshaders.shadow_cube->video_obj<nsgl_shader_obj>()->gl_shdr;
		cur_shdr->bind();
		cur_shdr->set_uniform("lightPos", ldc->light_pos);
		cur_shdr->set_uniform("maxDepth", ldc->max_depth);
	}
	else
	{
		cur_shdr = driver_ctxt->driver->rshaders.shadow_2d->video_obj<nsgl_shader_obj>()->gl_shdr;
		cur_shdr->bind();
	}
	cur_shdr->set_uniform("heightMap", HEIGHT_TEX_UNIT);
	cur_shdr->set_uniform("projMat", ldc->proj_light_mat);
	cur_shdr->set_uniform("viewport", fvec4(viewp.x,viewp.y,viewp.z,viewp.w));
	
	for (uint32 i = 0; i < rq->size(); ++i)
	{
		instanced_draw_call * idc = (instanced_draw_call*)(*rq)[i];
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

			driver_ctxt->enable_culling(idc->mat->culling());
			driver_ctxt->set_cull_face(idc->mat->cull_mode());
            driver_ctxt->render_instanced_dc(idc, cur_shdr);
		}
	}
}

void light_pass::render()
{
	for (uint32 i = 0; i < rq->size(); ++i)
	{
		light_draw_call * dc = (light_draw_call*)(*rq)[i];
		ivec4 & viewp = gl_state.current_viewport;
		if (dc->cast_shadows && vp->dir_light_shadows)
		{
			rpass->ldc = dc;
			rpass->setup();
			rpass->render();
			nsgl_framebuffer::attachment * attch = rpass->ren_target->att(nsgl_framebuffer::att_depth);
			driver_ctxt->set_active_texture_unit(attch->m_tex_unit);
			attch->m_texture->video_obj<nsgl_texture_obj>()->gl_tex->bind();
			gl_render_pass::setup();
		}

#ifdef ORDER_INDEPENDENT_TRANSLUCENCY
		ren_target->set_draw_buffer(nsgl_framebuffer::att_color);
#else
		ren_target->update_draw_buffers();
#endif

		nsgl_shader * gl_shdr = dc->shdr->video_obj<nsgl_shader_obj>()->gl_shdr;
		gl_shdr->bind();
		gl_shdr->set_uniform("shadowMap", SHADOW_TEX_UNIT);
		gl_shdr->set_uniform("gDiffuseMap", G_DIFFUSE_TEX_UNIT);
		gl_shdr->set_uniform("gNormalMap", G_NORMAL_TEX_UNIT);
		gl_shdr->set_uniform("gWorldPosMap", G_WORLD_POS_TEX_UNIT);
		gl_shdr->set_uniform("gMatMap", G_PICKING_TEX_UNIT);
		gl_shdr->set_uniform("epsilon", DEFAULT_SHADOW_EPSILON);
		gl_shdr->set_uniform("projCamMat", vp->camera->get<nscam_comp>()->proj_cam());
		gl_shdr->set_uniform("viewport", fvec4(viewp.x, viewp.y, viewp.z, viewp.w));
		gl_shdr->set_uniform("gbuf_tex_size", fvec2(ren_target->size.x,ren_target->size.y));
		gl_shdr->set_uniform("light.diffuseIntensity", dc->diffuse_intensity);
		gl_shdr->set_uniform("light.ambientIntensity", dc->ambient_intensity);
		gl_shdr->set_uniform("castShadows", dc->cast_shadows);
		gl_shdr->set_uniform("light.color", dc->light_color);
		gl_shdr->set_uniform("shadowSamples", dc->shadow_samples);
		gl_shdr->set_uniform("light.shadowDarkness", dc->shadow_darkness);
		const ivec2 & ss = driver_ctxt->render_target(DIR_SHADOW2D_TARGET)->size;
		gl_shdr->set_uniform("shadowTexSize", fvec2(ss.x, ss.y));
		
		nsstring id;
		id.reserve(64);
		auto iter = dc->material_ids->begin();
		while (iter != dc->material_ids->end())
		{
			id = "materials[" + std::to_string(iter->second) + "].spec_";
			gl_shdr->set_uniform(id+"color",iter->first->specular_color());
			gl_shdr->set_uniform(id+"power",iter->first->specular_power());
			gl_shdr->set_uniform(id+"intensity",iter->first->specular_intensity());
			++iter;
		}

		gl_shdr->set_uniform("projLightMat", dc->proj_light_mat);
		gl_shdr->set_uniform("bgColor", dc->bg_color);
		gl_shdr->set_uniform("light.direction", dc->direction);
		gl_shdr->set_uniform("camWorldPos",
							 vp->camera->get<nstform_comp>()->instance_transform(dc->scn, 0)->world_position());
		gl_shdr->set_uniform("fog_factor", vp->m_fog_nf);
		gl_shdr->set_uniform("fog_color", vp->m_fog_color);
		gl_shdr->set_uniform("lightingEnabled", vp->dir_lights);

#ifdef ORDER_INDEPENDENT_TRANSLUCENCY
		tbuffers->bind_buffers();
#endif
        driver_ctxt->render_light_dc(dc, gl_shdr);
#ifdef ORDER_INDEPENDENT_TRANSLUCENCY
		tbuffers->unbind_buffers();
#endif
	}
}

void culled_light_pass::render()
{
	for (uint32 i = 0; i < rq->size(); ++i)
	{
		light_draw_call * dc = (light_draw_call*)(*rq)[i];
		if (dc->cast_shadows && vp->dir_light_shadows)
		{
			rpass->ldc = dc;
			rpass->setup();
			rpass->render();
			nsgl_framebuffer::attachment * attch = rpass->ren_target->att(nsgl_framebuffer::att_depth);
			driver_ctxt->set_active_texture_unit(attch->m_tex_unit);
			attch->m_texture->video_obj<nsgl_texture_obj>()->gl_tex->bind();
			gl_render_pass::setup();
		}
		ren_target->set_draw_buffer(nsgl_framebuffer::att_none);
		nsgl_shader * st_shdr = driver_ctxt->driver->rshaders.light_stencil->video_obj<nsgl_shader_obj>()->gl_shdr;
		ivec4 & viewp = gl_state.current_viewport;
		st_shdr->bind();
		st_shdr->set_uniform("projCamMat", vp->camera->get<nscam_comp>()->proj_cam());
		st_shdr->set_uniform("viewport", fvec4(viewp.x, viewp.y, viewp.z, viewp.w));

		if (dc->submesh->m_node != nullptr)
			st_shdr->set_uniform("nodeTransform", dc->submesh->m_node->m_world_tform);
		else
			st_shdr->set_uniform("nodeTransform", fmat4());
		st_shdr->set_uniform("transform", dc->light_transform);
        driver_ctxt->render_light_dc(dc, st_shdr);
		
		driver_ctxt->enable_depth_test(false);
		driver_ctxt->enable_culling(true);
		driver_ctxt->set_cull_face(GL_FRONT);
		driver_ctxt->set_stencil_func(GL_NOTEQUAL, 0, 0xFF);
		ren_target->set_draw_buffer(nsgl_framebuffer::att_color);

		nsgl_shader * gl_shdr = dc->shdr->video_obj<nsgl_shader_obj>()->gl_shdr;
		gl_shdr->bind();
		gl_shdr->set_uniform("shadowMap", SHADOW_TEX_UNIT);
		gl_shdr->set_uniform("gDiffuseMap", G_DIFFUSE_TEX_UNIT);
		gl_shdr->set_uniform("gNormalMap", G_NORMAL_TEX_UNIT);
		gl_shdr->set_uniform("gWorldPosMap", G_WORLD_POS_TEX_UNIT);
		gl_shdr->set_uniform("gMatMap", G_PICKING_TEX_UNIT);
		gl_shdr->set_uniform("epsilon", DEFAULT_SHADOW_EPSILON);
		gl_shdr->set_uniform("projCamMat",
							 vp->camera->get<nscam_comp>()->proj_cam());
		gl_shdr->set_uniform("viewport", fvec4(viewp.x, viewp.y, viewp.z, viewp.w));
		gl_shdr->set_uniform("gbuf_tex_size", fvec2(ren_target->size.x,ren_target->size.y));
		gl_shdr->set_uniform("light.diffuseIntensity", dc->diffuse_intensity);
		gl_shdr->set_uniform("light.ambientIntensity", dc->ambient_intensity);
		gl_shdr->set_uniform("castShadows", dc->cast_shadows);
		gl_shdr->set_uniform("light.color", dc->light_color);
		gl_shdr->set_uniform("shadowSamples", dc->shadow_samples);
		gl_shdr->set_uniform("light.shadowDarkness", dc->shadow_darkness);
		gl_shdr->set_uniform("fog_factor", vp->m_fog_nf);
		gl_shdr->set_uniform("fog_color", vp->m_fog_color);		
		
		nsstring id;
		id.reserve(64);
		auto iter = dc->material_ids->begin();
		while (iter != dc->material_ids->end())
		{
			id = "materials[" + std::to_string(iter->second) + "].spec_";
			gl_shdr->set_uniform(id+"color",iter->first->specular_color());
			gl_shdr->set_uniform(id+"power",iter->first->specular_power());
			gl_shdr->set_uniform(id+"intensity",iter->first->specular_intensity());
			++iter;
		}

		gl_shdr->set_uniform("light.attConstant", dc->spot_atten.x);
		gl_shdr->set_uniform("light.attLinear", dc->spot_atten.y);
		gl_shdr->set_uniform("light.attExp", dc->spot_atten.z);
		gl_shdr->set_uniform("light.position", dc->light_pos);
		gl_shdr->set_uniform("maxDepth", dc->max_depth);
		gl_shdr->set_uniform("transform", dc->light_transform);
		if (dc->submesh->m_node != nullptr)
			gl_shdr->set_uniform("nodeTransform", dc->submesh->m_node->m_world_tform);
		else
			gl_shdr->set_uniform("nodeTransform", fmat4());
			
		if (dc->light_type == nslight_comp::l_spot)
		{
			gl_shdr->set_uniform("projLightMat", dc->proj_light_mat);
			gl_shdr->set_uniform("light.direction", dc->direction);
			gl_shdr->set_uniform("light.cutoff", dc->cutoff);
			const ivec2 & ss = driver_ctxt->render_target(SPOT_SHADOW2D_TARGET)->size;
			gl_shdr->set_uniform("shadowTexSize", fvec2(ss.x, ss.y));

		}
		else
		{
			const ivec2 & ss = driver_ctxt->render_target(POINT_SHADOW_TARGET)->size;
			gl_shdr->set_uniform("shadowTexSize", fvec2(ss.x, ss.y));
		}

#ifdef ORDER_INDEPENDENT_TRANSLUCENCY		
		tbuffers->bind_buffers();
#endif
        driver_ctxt->render_light_dc(dc, gl_shdr);
#ifdef ORDER_INDEPENDENT_TRANSLUCENCY
		tbuffers->unbind_buffers();
#endif
	}
}

void final_render_pass::render()
{
	read_buffer->target = nsgl_framebuffer::fb_read;
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

