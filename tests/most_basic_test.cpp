#define INPUTTEST

#include <nstimer.h>
#include <nsshader.h>
#include <nsinput_system.h>
#include <nsinput_map.h>
#include <iostream>
#include <nsengine.h>
#include <nsscene.h>
#include <iostream>
#include <nsplugin.h>
#include <nsmesh_manager.h>
#include <nsselection_system.h>
#include <nscamera_system.h>
#include <nscam_comp.h>
#include <nsinput_map_manager.h>
#include <nsmesh_manager.h>
#include <nsmaterial.h>
#include <nssel_comp.h>
#include <nsmath.h>
#include <nsrender_comp.h>
#include <nsanim_comp.h>
#include <nsfont.h>
#include <nsgl_driver.h>
#include <nsui_comp.h>
#include <nsrect_tform_comp.h>
#include <nsui_button_comp.h>
#include <nsplugin_manager.h>
#include <nsmaterial.h>
#include <nsui_system.h>
#include <nstexture.h>
#include <nsrender_system.h>
#include <nsui_canvas_comp.h>
#include <nsrouter.h>
#include <nsgl_window.h>
#include <nsscene_manager.h>

void setup_input_map(nsplugin * plg);


struct button_funcs
{
    void on_new_game()
    {
        if (can != nullptr && can->is_enabled())
        {
//            plg->save(scn);
//            nse.set_active_scene(nullptr);
//            plg->destroy(scn);
//            scn = nullptr;
            can->enable(false);
        }
        else
        {
            can->enable(true);
//            scn = plg->load<nsscene>("new_scene.map",true);
//            nse.set_active_scene(scn);
        }
    }

    nsplugin * plg;
    nsscene * scn;
    nsrouter * m_router;
	nsui_button_comp * btn;
    nsui_canvas_comp * can;
};

/*
  This test adds camera controls and movement controls to an input map from scratch
  The generated input map can be used in other tests
  Also made sure that saving and loading the input map works in both binary and text saving modes
*/
int main()
{
//    glfw_setup(ivec2(1920,1080) / 2, false, "Build And Battle 1.0.0");
    button_funcs bf;
    bf.m_router = new nsrouter;
	nsengine e;
    e.init(new nsgl_driver);
	
    nsgl_window wind(ivec2(800,600), "Basic Test");
	
    nsplugin * plg = nsep.create("most_basic_test");
    plg->enable(true);
    plg->manager<nsscene_manager>()->set_save_mode(nsasset_manager::text);

    nsinput_map * im = plg->create<nsinput_map>("basic_input");
    im->create_context("main_global_ctxt");

    nsinput_map::trigger change_vp("mouse_pressed_in_viewport", nsinput_map::t_pressed);
    change_vp.add_key_mod(nsinput_map::key_any);
    im->add_mouse_trigger("main_global_ctxt", nsinput_map::left_button, change_vp);

    nsscene * new_scene = plg->create<nsscene>("new_scene");
	new_scene->enable(true);
    bf.scn = new_scene;
    bf.plg = plg;
	
    new_scene->set_bg_color(fvec3(0.7f, 0.7f, 1.0f));
    nsentity * grass_tile = plg->create_tile("grass_tile",
											 nse.import_dir() + "diffuseGrass.png",
                                             nse.import_dir() + "normalGrass.png",
                                             fvec4(1,0,0,0.5f), 16.0, 0.6, fvec3(1,1,1), true);

    nsentity * stone_tile = plg->create_tile("stone_tile",
                                             nse.import_dir() + "diffuseStone.png",
                                             nse.import_dir() + "normalStone.png",
                                             fvec4(1,0,0,0.5f), 16.0, 0.6, fvec3(1,1,1), true);

    nsentity * point_light = plg->create_point_light("point_light", 1.0f, 0.0f, 30.0f);
    nsentity * spot_light = plg->create_spot_light("spot_light", 1.0f, 0.0f, 100.0f, 10.0f, fvec3(0.0f,0.0f,1.0f));
    nsentity * cam = plg->create_camera("scenecam", 60.0f, uivec2(400, 400), fvec2(DEFAULT_Z_NEAR, DEFAULT_Z_FAR));
    nsentity * dirl = plg->create_dir_light("dirlight", 1.0f, 0.0f,fvec3(1.0f,1.0f,1.0f),true,0.5f,2);
    nsentity * canvas = plg->create<nsentity>("canvas");
    nsentity * ui_button = plg->create<nsentity>("button_new_match");

    new_scene->add(cam,nullptr,true,fvec3(0,0,-20));
    new_scene->add(dirl, nullptr, false, fvec3(5.0f, 5.0f, -20.0f), orientation(fvec4(1,0,0,20.0f)));
    new_scene->add(point_light, nullptr, false, fvec3(5.0f, 20.0f, -20.0f), orientation(fvec4(1,0,0,20.0f)));
    new_scene->add(spot_light, point_light->get<nstform_comp>()->instance_transform(new_scene,0), false, fvec3(20.0f, 5.0f, -20.0f), orientation(fvec4(1,0,0,20.0f)));
    new_scene->add_gridded(grass_tile,ivec3(3,3,1));
    new_scene->add(stone_tile, grass_tile->get<nstform_comp>()->instance_transform(new_scene,3),true,fvec3(0,0,-20));
    grass_tile->get<nstform_comp>()->instance_transform(new_scene,3)->set_parent(spot_light->get<nstform_comp>()->instance_transform(new_scene,0), true);

	
	// Create material for the button bg
    nsmaterial * mat = plg->create<nsmaterial>("btn_contents_mat");
    //nstex2d * reg_tex = plg->load<nstex2d>(nse.import_dir() + "boona.jpg", true);
    //mat->add_tex_map(nsmaterial::diffuse, tex_map_info(reg_tex->full_id(),fvec4(0.0f,0.0f,1.0f,1.0f)), true);
    mat->set_color_mode(true);
	mat->set_color(fvec4(0.0f,0.0f,0.0f,0.8f));
	
	// Create material for the button border
	nsmaterial * border_mat = plg->create<nsmaterial>("border_mat");
	border_mat->set_color_mode(true);
	border_mat->set_color(fvec4(0.1f,0.1f,0.1f,1.0f));
	
	// Create material for the fnt rendering
	nsmaterial * fnt_mat = plg->create<nsmaterial>("my_font");
	fnt_mat->set_color_mode(true);
    fnt_mat->set_color(fvec4(1.0f,0.0f,0.0f,0.8f));
	
	nsui_material_comp * uic = ui_button->create<nsui_material_comp>();
	uic->mat_id = mat->full_id();
	uic->border_mat_id = border_mat->full_id();
	uic->border_size = fvec4(1,1,1,1);
	uic->mat_shader_id = nse.core()->get<nsshader>(UI_SHADER)->full_id();

	nsui_text_comp * uitxt = ui_button->create<nsui_text_comp>();
    nsfont * fnt = plg->load<nsfont>("./import/cosn.fnt",true);
	fnt->material_id = fnt_mat->full_id();
    uitxt->font_id = fnt->full_id();
    uitxt->text = ">>> ";
    uitxt->text_shader_id = nse.core()->get<nsshader>(UI_TEXT_SHADER)->full_id();
    uitxt->text_alignment = nsui_text_comp::bottom_left;
    uitxt->margins = uivec4(3,3,3,3);

	nsui_text_input_comp * uitxt_input = ui_button->create<nsui_text_input_comp>();
	uitxt_input->cursor_color = fvec4(1,1,1,0.8);
	uitxt_input->cursor_pixel_width = 2;
	uitxt_input->cursor_blink_rate_ms = 430.0f;
	
	nsui_button_comp * uibtn = ui_button->create<nsui_button_comp>();
	bf.m_router->connect(&bf,&button_funcs::on_new_game,uibtn->pressed);
    bf.btn = uibtn;
    uibtn->button_states[0].border_size = fvec4(5,5,5,5);
	uibtn->button_states[0].border_color = fvec4(1,0,0,0.7);
    uibtn->button_states[0].top_border_radius = fvec4(50,50,50,50);
	uibtn->button_states[0].bottom_border_radius = fvec4(50,50,50,50);
    uibtn->button_states[0].mat_color = fvec4(0.7,0,0,0.5);
    uibtn->button_states[0].text_color = fvec4(0.0,1.0,1.0,1.0);

	uibtn->button_states[1].border_color = fvec4(0,1.0,0,1);
    uibtn->button_states[1].top_border_radius = fvec4(50,50,50,50);
	uibtn->button_states[1].bottom_border_radius = fvec4(50,50,50,50);
    uibtn->button_states[1].border_size = fvec4(3,3,5,5);
    uibtn->button_states[1].mat_color = fvec4(0.0,0.7,0,1);
    uibtn->button_states[1].mat_color_mult = fvec4(1.0,1.0,1.0,0.5);

	uibtn->button_states[2].border_color = fvec4(0,0,1,1);
	uibtn->button_states[2].border_size = fvec4(30,30,30,30);
    uibtn->button_states[2].mat_color = fvec4(1,0,0,1);
    uibtn->button_states[2].mat_color_mult = fvec4(0.7,0,0,1);

	uibtn->button_states[3].border_color = fvec4(0.7,0.7,0.7,1);
	uibtn->button_states[3].border_size = fvec4(5,5,5,5);
    uibtn->button_states[3].mat_color = fvec4(0.4,0.4,0.4,0.6);
	
    nsui_canvas_comp * cc = canvas->create<nsui_canvas_comp>();
    cc->enable(true);
    cc->add(ui_button);
    bf.can = cc;

    nsrect_tform_comp * tuic = ui_button->get<nsrect_tform_comp>();
    auto pic = tuic->canvas_info(cc);
    pic->anchor_rect = fvec4(0.2f,0.2f,0.2f,0.2f);
    pic->pixel_offset_rect = fvec4(-410/8.0f, -729/8.0f, 410/8.0f, 729/8.0f);
    pic->pivot = fvec2(0.5f,0.5f);
    pic->layer = 0;
    pic->angle = 0.0f;

    plg->save_all_in_plugin();
    nsep.save(plg);



    viewport vp(fvec4(0.0f,0.0f,1.0f,1.0f), cam);
    vp.ui_canvases.push_back(canvas);

    wind.vid_context()->insert_viewport("main_view",vp);
    nse.set_active_scene(new_scene);


    nse.system<nscamera_system>()->setup_input_map(im, "main_global_ctxt");
    nse.system<nsselection_system>()->setup_input_map(im, "main_global_ctxt");
    nse.system<nsui_system>()->setup_input_map(im, "main_global_ctxt");
    nse.system<nsinput_system>()->set_input_map(im->full_id());
    nse.system<nsinput_system>()->push_context("main_global_ctxt");
	
    e.start();

    cc->enable(false);
    cc->enable(true);
    cc->enable(false);
    cc->enable(true);
    cc->enable(false);
    cc->enable(true);
    cc->enable(false);
    cc->enable(true);
    cc->enable(false);
    cc->enable(true);
    while (e.running())
    {
        nse.update();
        wind.vid_context()->render(nse.active_scene());
        wind.update();
		window_poll_input();

        if (!wind.is_open())
            e.stop();
    }

    nse.release();
    return 0;
}

void setup_input_map(nsplugin * plg)
{

}
