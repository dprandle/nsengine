#define INPUTTEST

#include <opengl/nsgl_driver.h>
#include <opengl/nsgl_window.h>

#include <asset/nsfont.h>
#include <asset/nsfont_manager.h>
#include <asset/nsshader.h>
#include <asset/nsinput_map.h>
#include <asset/nsplugin.h>
#include <asset/nsmesh_manager.h>
#include <asset/nsinput_map_manager.h>
#include <asset/nsmesh_manager.h>
#include <asset/nsmaterial.h>
#include <asset/nsplugin_manager.h>
#include <asset/nstexture.h>
#include <asset/nsaudio_clip.h>

#include <component/nscam_comp.h>
#include <component/nssel_comp.h>
#include <component/nsrender_comp.h>
#include <component/nsanim_comp.h>
#include <component/nsrect_tform_comp.h>
#include <component/nsui_comp.h>
#include <component/nsui_button_comp.h>
#include <component/nsui_canvas_comp.h>
#include <component/nssprite_comp.h>

#include <system/nsinput_system.h>
#include <system/nsselection_system.h>
#include <system/nscamera_system.h>
#include <system/nsui_system.h>
#include <system/nstform_system.h>

#include <nstimer.h>
#include <nsrouter.h>
#include <nstile_grid.h>

#include <iostream>
#include <AL/al.h>


void setup_input_map(nsplugin * plg);

struct button_funcs
{	
    void on_new_game()
    {
    }

	void on_add_tile()
	{
	}

	void on_remove_selection()
	{
	}

	void on_toggle_ac()
	{
	}

	void on_toggle_sc()
	{
	}
};

struct button_style
{
	button_style():
		fnt(nullptr),
		text_alignment(nsui_text_comp::middle_center),
		text_margins()
	{
		for (uint8 i = 0; i < 4; ++i)
		{
			reg_states[i].top_border_radius = fvec4(5,5,5,5);
			reg_states[i].bottom_border_radius = fvec4(5,5,5,5);
			reg_states[i].border_size = fvec4(2,2,2,2);
			toggle_states[i].top_border_radius = fvec4(5,5,5,5);
			toggle_states[i].bottom_border_radius = fvec4(5,5,5,5);
			toggle_states[i].border_size = fvec4(2,2,2,2);
		}
	}
	nsfont * fnt;
	int text_alignment;
	uivec4 text_margins;
	button_state reg_states[4];
	button_state toggle_states[4];
};

// nsui_button_comp * create_button(
// 	nsentity * canvas,
// 	const nsstring & button_name,
// 	button_style style,
// 	const fvec2 & norm_center_pos,
// 	const fvec4 & offset,
// 	const nsstring & text
// 	);


int main()
{
    // nsengine e;
    // button_funcs bf;
	
    // bf.m_router = new nsrouter;
    // e.init(new nsgl_driver);
	
    // nsgl_window wind(ivec2(800,600), "Basic Test");
    // viewport * vp = wind.vid_context()->insert_viewport("main_view",fvec4(0.0f,0.0f,1.0f,1.0f));
	
    // nsplugin * plg = nsep.create<nsplugin>("most_basic_test");
    // plg->enable(true);
	// bf.plg = plg;
	
    // vp->camera = plg->create_camera("scenecam", 60.0f, uivec2(400, 400), fvec2(DEFAULT_Z_NEAR, DEFAULT_Z_FAR));
    // setup_input_map(plg);

    // nsmap_area * new_scene = plg->create<nsmap_area>("new_scene");
    // new_scene->enable(true);
	// bf.scn = new_scene;
	
    // new_scene->set_bg_color(fvec3(0.7f, 0.7f, 1.0f));

	// // create grass mat
	// bf.grass_mat = plg->create<nsmaterial>("grass_mat");
	// nstexture * dif = plg->load<nstex2d>(nse.import_dir() + "diffuseGrass.png", false);
	// nstexture * nrm = plg->load<nstex2d>(nse.import_dir() + "normalGrass.png", false);
	// tex_map_info tmi;
	// tmi.tex_id = dif->full_id();
	// bf.grass_mat->add_tex_map(nsmaterial::diffuse, tmi, true);
	// tmi.tex_id = nrm->full_id();
	// bf.grass_mat->add_tex_map(nsmaterial::normal, tmi, true);

    // nsentity * point_light = plg->create_point_light("point_light", 1.0f, 0.0f, 30.0f);
    // nsentity * spot_light = plg->create_spot_light("spot_light", 1.0f, 0.0f, 100.0f, 10.0f, fvec3(0.0f,0.0f,1.0f));
    // nsentity * dirl = plg->create_dir_light("dirlight", 1.0f, 0.0f,fvec3(1.0f,1.0f,1.0f),true,0.5f,2);
    // nsentity * canvas = plg->create<nsentity>("canvas");
	
	// nsui_canvas_comp * cc = canvas->create<nsui_canvas_comp>();
    // cc->enable(true);

	// button_style bs;
	
	// bs.fnt = plg->load<nsfont>(nse.import_dir() + "sample.otf",true);
    // bs.fnt->set_point_size(18);

    // bs.reg_states[0].border_color = fvec4(0.0f,0.0f,0.0f,0.7);
    // bs.reg_states[0].mat_color = fvec4(0.3, 0.3, 0.3, 1.0);
    // bs.reg_states[0].text_color = fvec4(1.0,1.0,1.0,1.0);

    // bs.reg_states[1].border_color = fvec4(0,0,0,0.7);
    // bs.reg_states[1].mat_color = fvec4(1.0, 0.0, 0.0, 0.5);
    // bs.reg_states[1].text_color = fvec4(1.0,1.0,1.0,1.0);

    // bs.reg_states[2].border_color = fvec4(0,0,1,1);
    // bs.reg_states[2].mat_color = fvec4(0.0,0.0,0.0,1);
    // bs.reg_states[2].text_color = fvec4(1.0,1.0,0.0,1.0);
	
    // bs.reg_states[3].border_color = fvec4(0.7,0.7,0.7,1);
    // bs.reg_states[3].mat_color = fvec4(0.4,0.4,0.4,0.6);
	// bs.reg_states[3].text_color = fvec4(1.0,1.0,1.0,1.0);
   

	// // Add stuff to scene
	// tform_info tf(uivec2(),fvec3(0,0,-20));
	// new_scene->add_entity(vp->camera, &tf);

	// tf.m_position = fvec3(5.0f, 5.0f, -20.0f);
	// tf.m_orient = orientation(fvec4(1,0,0,20.0f));
    // new_scene->add_entity(dirl, &tf);

	// tf.m_position = fvec3(5.0f, 20.0f, -20.0f);
	// tf.m_orient = orientation(fvec4(1,0,0,20.0f));
    // new_scene->add_entity(point_light, &tf);

	// //tf.m_parent = vp->camera->full_id();
	// tf.m_position = fvec3(20.0f, 5.0f, -20.0f);
	// tf.m_children.push_back(point_light->full_id());
	// tf.m_orient = orientation(fvec4(1,0,0,20.0f));
	// new_scene->add_entity(spot_light, &tf);

	// fvec2 button_size(200,60);
	// fvec2 start_pos(200,800);
	
	// // Add stuff to canvas
	// nsui_button_comp * btn = create_button(
	// 	canvas,
	// 	"add_tile_btn",
	// 	bs,
	// 	fvec2(0.0f),
	// 	fvec4(start_pos - button_size / 2.0f,start_pos + button_size / 2.0f),
	// 	"Add Tile"
	// 	);
	// bf.m_router->connect(&bf, &button_funcs::on_add_tile, btn->pressed);
	// start_pos.y -= 100.0f;
	// btn = create_button(
	// 	canvas,
	// 	"remove_selection_btn",
	// 	bs,
	// 	fvec2(0.0f),
	// 	fvec4(start_pos - button_size / 2.0f,start_pos + button_size / 2.0f),
	// 	"Delete Selection"
	// 	);
	// bf.m_router->connect(&bf, &button_funcs::on_remove_selection, btn->pressed);
	// start_pos.y -= 100;
	// btn = create_button(
	// 	canvas,
	// 	"toggle_anim_comp_to_selection_btn",
	// 	bs,
	// 	fvec2(0.0f),
	// 	fvec4(start_pos - button_size / 2.0f,start_pos + button_size / 2.0f),
	// 	"Toggle AC Selection"
	// 	);	
	// bf.m_router->connect(&bf, &button_funcs::on_toggle_ac, btn->pressed);
	// start_pos.y -= 100;
	// btn = create_button(
	// 	canvas,
	// 	"toggle_sprite_comp_to_selection_btn",
	// 	bs,
	// 	fvec2(0.0f),
	// 	fvec4(start_pos - button_size / 2.0f,start_pos + button_size / 2.0f),
	// 	"Toggle SC Selection"
	// 	);	
	// bf.m_router->connect(&bf, &button_funcs::on_toggle_sc, btn->pressed);

	// // set the canvas and scene
	// vp->ui_canvases.push_back(canvas);
    // nse.set_active_scene(plg->get<nsmap_area>("new_scene"));

	// plg->save(bs.fnt->get_atlas(0));
	
    // e.start();
    // while (e.running())
    // {
    //     nse.update();
    //     wind.vid_context()->render(nse.active_scene());
    //     wind.update();
	// 	window_poll_input();

    //     if (!wind.is_open())
    //         e.stop();
    // }

    // nse.release();
    // return 0;
}

void setup_input_map(nsplugin * plg)
{
    // nsinput_map * im = plg->create<nsinput_map>("basic_input");
    // im->create_context("main_global_ctxt");

	// nse.system<nscamera_system>()->setup_input_map(im, "main_global_ctxt");
    // nse.system<nsselection_system>()->setup_input_map(im, "main_global_ctxt");
    // nse.system<nsui_system>()->setup_input_map(im, "main_global_ctxt");
    // nse.system<nsinput_system>()->set_input_map(im->full_id());
    // nse.system<nsinput_system>()->push_context("main_global_ctxt");

	// nsinput_map::trigger change_vp("mouse_pressed_in_viewport", nsinput_map::t_pressed);
    // change_vp.add_key_mod(nsinput_map::key_any);
    // im->add_mouse_trigger("main_global_ctxt", nsinput_map::left_button, change_vp);
}


nsui_button_comp * create_button(
	nsentity * canvas,
	const nsstring & button_name,
	button_style style,
	const fvec2 & norm_center_pos,
	const fvec4 & offset,
	const nsstring & text
	)
{
	// nsplugin * plg = nsep.get<nsplugin>(canvas->plugin_id());
	// nsentity * bent = plg->create<nsentity>(button_name);

	//     // Create material for the button bg
    // nsmaterial * mat = plg->create<nsmaterial>(button_name);
    // mat->set_color_mode(true);
	
    // // Create material for the button border
    // nsmaterial * border_mat = plg->create<nsmaterial>(button_name + "_border");
    // border_mat->set_color_mode(true);
	
	// nsmaterial * fnt_mat = plg->create<nsmaterial>(button_name + "_fnt");
    // fnt_mat->set_color_mode(true);

    // nsui_material_comp * uic = bent->create<nsui_material_comp>();
    // uic->mat_id = mat->full_id();
    // uic->border_mat_id = border_mat->full_id();
    // uic->mat_shader_id = nse.core()->get<nsshader>(UI_SHADER)->full_id();

    // nsui_text_comp * uitxt = bent->create<nsui_text_comp>();
    // uitxt->font_material_id = fnt_mat->full_id();
    // uitxt->font_id = style.fnt->full_id();
    // uitxt->text = text;
    // uitxt->text_shader_id = nse.core()->get<nsshader>(UI_TEXT_SHADER)->full_id();
    // uitxt->text_alignment = nsui_text_comp::t_alignment(style.text_alignment);
    // uitxt->margins = style.text_margins;

    // nsui_button_comp * uibtn = bent->create<nsui_button_comp>();
	// for (uint8 i = 0; i < 4; ++i)
	// {
	// 	uibtn->button_states[i] = style.reg_states[i];
	// 	uibtn->toggled_button_states[i] = style.toggle_states[i];
	// }
	
    // nsui_canvas_comp * cc = canvas->get<nsui_canvas_comp>();
    // cc->add(bent);

    // nsrect_tform_comp * tuic = bent->get<nsrect_tform_comp>();
    // auto pic = tuic->canvas_info(cc);
    // pic->anchor_rect = fvec4(norm_center_pos,norm_center_pos);
    // pic->pixel_offset_rect = offset;
    // pic->pivot = fvec2(0.5f,0.5f);
    // pic->layer = 0;
    // pic->angle = 0.0f;
	// return uibtn;
}
