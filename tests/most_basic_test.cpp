#define INPUTTEST

#include <nstimer.h>
#include <nsshader.h>
#include <nsinput_system.h>
#include <nsinput_map.h>
#include <iostream>
#include <nsengine.h>
#include <nsglfw.h>
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
#include <nsopengl_driver.h>
#include <nsui_comp.h>
#include <nsrect_tform_comp.h>
#include <nsui_button_comp.h>
#include <nsplugin_manager.h>
#include <nsmaterial.h>
#include <nsui_system.h>
#include <nsrender_system.h>
#include <nsui_canvas_comp.h>
#include <nsrouter.h>

void setup_input_map(nsplugin * plg);


struct button_funcs
{
    void on_new_game()
    {
        nse.system<nsselection_system>()->delete_selection();
    }

    nsplugin * plg;
    nsscene * scn;
    nsrouter * m_router;
};

/*
  This test adds camera controls and movement controls to an input map from scratch
  The generated input map can be used in other tests
  Also made sure that saving and loading the input map works in both binary and text saving modes
*/
int main()
{
    glfw_setup(ivec2(400,400), false, "Build And Battle 1.0.0");
    button_funcs bf;
    bf.m_router = new nsrouter;
	
    nse.start();
    nse.create_default_systems();

	nsopengl_driver * gl_driver = nse.create_video_driver<nsopengl_driver>();
	gl_driver->create_context();
    gl_driver->init(true);

	nse.setup_core_plug();

    nsplugin * plg = nsep.create("most_basic_test");
    plg->bind();
	setup_input_map(plg);

    nsscene * new_scene = plg->create<nsscene>("new_scene");
    bf.scn = new_scene;
    bf.plg = plg;

    new_scene->set_bg_color(fvec3(0.7f, 0.7f, 1.0f));
    plg->set_current_scene(new_scene, true, false);

    nsentity * grass_tile = plg->create_tile("grass_tile", nse.import_dir() + "diffuseGrass.png", nse.import_dir() + "normalGrass.png", fvec4(1,0,0,1), 16.0, 0.6, fvec3(1,1,1), true);
    nsentity * point_light = plg->create_point_light("point_light", 1.0f, 0.0f, 30.0f);
    nsentity * spot_light = plg->create_spot_light("spot_light", 1.0f, 0.0f, 100.0f, 10.0f, fvec3(0.0f,0.0f,1.0f));
    nsentity * cam = plg->create_camera("scenecam", 60.0f, uivec2(400, 400), fvec2(DEFAULT_Z_NEAR, DEFAULT_Z_FAR));
    nsentity * dirl = plg->create_dir_light("dirlight", 1.0f, 0.0f,fvec3(1.0f,1.0f,1.0f),true,0.5f,2);

    nsfont * fnt = plg->load<nsfont>("./import/cosn.fnt",true);
    fnt->render_color = fvec4(1,1,1,1);

    new_scene->add(cam);
    new_scene->add(dirl, nullptr, false, fvec3(5.0f, 5.0f, -20.0f), orientation(fvec4(1,0,0,20.0f)));
    new_scene->add(point_light, nullptr, false, fvec3(5.0f, 20.0f, -20.0f), orientation(fvec4(1,0,0,20.0f)));
    new_scene->add(spot_light, point_light->get<nstform_comp>()->instance_transform(new_scene,0), false, fvec3(20.0f, 5.0f, -20.0f), orientation(fvec4(1,0,0,20.0f)));
    new_scene->add_gridded(grass_tile,ivec3(32,32,1));


    nsentity * canvas = plg->create<nsentity>("canvas");
    nsui_canvas_comp * cc = canvas->create<nsui_canvas_comp>();

    nsentity * ui_button = plg->create<nsentity>("button_new_match");
    nsui_comp * uic = ui_button->create<nsui_comp>();
	nsui_text_comp * uitxt = ui_button->create<nsui_text_comp>();
    uitxt->font_id = fnt->full_id();
    uitxt->text = ">>> ";
    uitxt->text_shader_id = nse.core()->get<nsshader>(UI_TEXT_SHADER)->full_id();
	uitxt->text_editable = true;
    uitxt->cursor_color = fvec4(1,1,1,0.8);
    uitxt->text_alignment = nsui_text_comp::bottom_left;
    uitxt->cursor_pixel_width = 2;
	uitxt->cursor_blink_rate_ms = 430.0f;
    uitxt->margins = uivec4(80,15,20,25);
	
//    nsui_button_comp * uibtn = ui_button->create<nsui_button_comp>();
//    bf.m_router->connect(&bf,&button_funcs::on_new_game,uibtn->pressed);
//    uibtn->change_button_using = colo;
//    uibtn->colors[0] = fvec4(1,1,0,0.5);
//    uibtn->colors[1] = fvec4(0.5,0.5,0.5,0.7);
//    uibtn->tex_coord_rects[0] = fvec4(0,0.5,1,0.75);
//    uibtn->tex_coord_rects[1] = fvec4(0,0.25,1,0.5);

    cc->add(ui_button);
    nsrect_tform_comp * tuic = ui_button->get<nsrect_tform_comp>();

    nsmaterial * mat = plg->create<nsmaterial>("btn_contents_mat");
    nstex2d * reg_tex = plg->load<nstex2d>(nse.import_dir() + "new-match.png", true);
    mat->add_tex_map(nsmaterial::diffuse, tex_map_info(reg_tex->full_id(),fvec4(0.0f,0.75f,1.0f,1.0f)), true);
    mat->set_color_mode(true);
    mat->set_color(fvec4(0.0f,0.0f,0.0f,0.8f));

    auto pic = tuic->canvas_info(cc);
    pic->anchor_rect = fvec4(0.0f,0.0f,0.0f,0.0f);
    pic->pixel_offset_rect = fvec4(0, 0, 800, 200);
    pic->pivot = fvec2(0.5f,0.5f);
    pic->layer = 0;
    pic->angle = 0.0f;

    uic->mat_id = mat->full_id();
    uic->border_color = fvec4(0.0f,0.0f,0.0f,1.0f);
    uic->border = fvec4(1,1,1,1);

    uic->content_shader_id = nse.core()->get<nsshader>(UI_SHADER)->full_id();
    uic->border_shader_id = nse.core()->get<nsshader>(UI_BORDER_SHADER)->full_id();
    uic->show = true;

    nsrender::viewport vp(fvec4(0.0f,0.0f,1.0f,1.0f), cam);
    vp.ui_canvases.push_back(canvas);
    nse.system<nsrender_system>()->insert_viewport("main_view",vp);

    nse.set_active_scene(new_scene);

	
    while (glfw_window_open())
    {
        nse.update();

        nse.system<nsui_system>()->set_active_viewport(&vp);
        nse.system<nsui_system>()->push_draw_calls();
        nse.system<nsrender_system>()->render();

        glfw_update();
    }

    nse.shutdown();
	glfw_shutdown();
    return 0;
}

void setup_input_map(nsplugin * plg)
{
	nsinput_map * im = plg->create<nsinput_map>("basic_input");
	im->create_context("main_global_ctxt");
	
    nsinput_map::trigger change_vp("mouse_pressed_in_viewport", nsinput_map::t_pressed);
    change_vp.add_key_mod(nsinput_map::key_any);
    im->add_mouse_trigger("main_global_ctxt", nsinput_map::left_button, change_vp);

	nse.system<nscamera_system>()->setup_input_map(im, "main_global_ctxt");
	nse.system<nsselection_system>()->setup_input_map(im, "main_global_ctxt");
	nse.system<nsui_system>()->setup_input_map(im, "main_global_ctxt");
	nse.system<nsinput_system>()->set_input_map(im->full_id());
	nse.system<nsinput_system>()->push_context("main_global_ctxt");	
}
