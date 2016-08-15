#define INPUTTEST

#include <opengl/nsgl_driver.h>
#include <opengl/nsgl_window.h>

#include <asset/nsfont.h>
#include <asset/nsfont_manager.h>
#include <asset/nsshader.h>
#include <asset/nsinput_map.h>
#include <asset/nsscene.h>
#include <asset/nsplugin.h>
#include <asset/nsmesh_manager.h>
#include <asset/nsinput_map_manager.h>
#include <asset/nsmesh_manager.h>
#include <asset/nsmaterial.h>
#include <asset/nsplugin_manager.h>
#include <asset/nstexture.h>
#include <asset/nsscene_manager.h>
#include <asset/nsaudio_clip.h>

#include <component/nscam_comp.h>
#include <component/nssel_comp.h>
#include <component/nsrender_comp.h>
#include <component/nsanim_comp.h>
#include <component/nsrect_tform_comp.h>
#include <component/nsui_comp.h>
#include <component/nsui_button_comp.h>
#include <component/nsui_canvas_comp.h>

#include <system/nsinput_system.h>
#include <system/nsselection_system.h>
#include <system/nscamera_system.h>
#include <system/nsui_system.h>
#include <system/nstform_system.h>

#include <nstimer.h>
#include <nsrouter.h>

#include <iostream>

nsscene * setup_basic_scene(nsplugin * plg);
void setup_input_map(nsplugin * plg);


struct button_funcs
{
    void on_new_game()
    {
		
    }

	nsgl_window * wind;
	nsgl_window * wind2;
	
    nsplugin * plg;
    nsscene * scn;
    nsrouter * m_router;
	nsui_button_comp * btn;
};

button_funcs bf;
nsengine e;
/*
  This test adds camera controls and movement controls to an input map from scratch
  The generated input map can be used in other tests
  Also made sure that saving and loading the input map works in both binary and text saving modes
*/
int main()
{
    bf.m_router = new nsrouter;
	e.init(new nsgl_driver);


    nsgl_window wind(ivec2(800,600), "Basic Test");
    nsgl_window wind2(ivec2(900,700), "Second Window Test");
	bf.wind = &wind;


	nsplugin * plg = nsep.create<nsplugin>("most_basic_test");
    plg->enable(true);

	nstex2d * tx = plg->load<nstex2d>("./import/bbicon.png", true);
    tx->flip_verticle();
    wind.set_icon(tx);
	
	setup_input_map(plg);
	nsscene * new_scene = setup_basic_scene(plg);

    viewport vp(fvec4(0.0f,0.0f,1.0f,1.0f), plg->get<nsentity>("scenecam"));
    vp.ui_canvases.push_back(plg->get<nsentity>("canvas"));

    wind.vid_context()->insert_viewport("main_view",vp);
    vp.camera = plg->get<nsentity>("scenecam2");
	wind2.vid_context()->insert_viewport("main_view",vp);
	
    e.start();

    while (e.running())
    {
        e.update();

        if (wind.is_current() && wind.is_open())
        {
            if (wind2.is_open())
            {
                wind2.make_current();
                wind2.vid_context()->render(new_scene);
                wind2.update();
            }

            wind.make_current();
            wind.vid_context()->render(new_scene);
            wind.update();
        }
        else if (wind2.is_current() && wind2.is_open())
        {
            if (wind.is_open())
            {
                wind.make_current();
                wind.vid_context()->render(new_scene);
                wind.update();
            }

            wind2.make_current();
            wind2.vid_context()->render(new_scene);
            wind2.update();
        }
        window_poll_input();

        if (!wind.is_open() && !wind2.is_open())
        {
            e.stop();
        }
    }

    e.release();
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

nsscene * setup_basic_scene(nsplugin * plg)
{

    nsscene * new_scene = plg->create<nsscene>("new_scene");
	new_scene->enable(true);
	
    new_scene->set_bg_color(fvec3(0.7f, 0.7f, 1.0f));
    nsentity * grass_tile = plg->create_tile("grass_tile",
                                             nse.import_dir() + "diffuseGrass.png",
                                             nse.import_dir() + "normalGrass.png",
                                             fvec4(1,0,0,0.5f), 16.0, 0.6, fvec3(1,1,1), true);

    nsentity * point_light = plg->create_point_light("point_light", 1.0f, 0.0f, 30.0f);
    nsentity * spot_light = plg->create_spot_light("spot_light", 1.0f, 0.0f, 100.0f, 10.0f, fvec3(0.0f,0.0f,1.0f));

    nsentity * cam = plg->create_camera("scenecam", 60.0f, uivec2(400, 400), fvec2(DEFAULT_Z_NEAR, DEFAULT_Z_FAR));
    nsentity * cam2 = plg->create_camera("scenecam2", 60.0f, uivec2(400, 400), fvec2(DEFAULT_Z_NEAR, DEFAULT_Z_FAR));

    nsentity * dirl = plg->create_dir_light("dirlight", 1.0f, 0.0f,fvec3(1.0f,1.0f,1.0f),true,0.5f,2);
    nsentity * canvas = plg->create<nsentity>("canvas");
    nsentity * ui_button = plg->create<nsentity>("button_new_match");

    new_scene->add(cam,nullptr,true,fvec3(0,0,-20));
    new_scene->add(cam2,nullptr,true,fvec3(0,0,-20));
    new_scene->add(dirl, nullptr, false, fvec3(5.0f, 5.0f, -20.0f), orientation(fvec4(1,0,0,20.0f)));
    new_scene->add(point_light, nullptr, false, fvec3(5.0f, 20.0f, -20.0f), orientation(fvec4(1,0,0,20.0f)));
    new_scene->add_gridded(grass_tile,ivec3(32,32,1));
	
	// Create material for the button bg
    nsmaterial * mat = plg->create<nsmaterial>("btn_contents_mat");
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
    nsfont * fnt = plg->load<nsfont>("/usr/share/fonts/truetype/arial.ttf",true);
	uitxt->font_material_id = fnt_mat->full_id();
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
    cc->add(ui_button);

    nsrect_tform_comp * tuic = ui_button->get<nsrect_tform_comp>();
    auto pic = tuic->canvas_info(cc);
    pic->anchor_rect = fvec4(0.2f,0.2f,0.2f,0.2f);
    pic->pixel_offset_rect = fvec4(-410/8.0f, -729/8.0f, 410/8.0f, 729/8.0f);
    pic->pivot = fvec2(0.5f,0.5f);
    pic->layer = 0;
    pic->angle = 0.0f;

    nse.set_active_scene(new_scene);
	return new_scene;
}
