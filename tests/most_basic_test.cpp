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
#include <AL/al.h>


void setup_input_map(nsplugin * plg);

uint32 source;

struct button_funcs
{
    void on_new_game()
    {
		alSourcePlay(source);
    }

    nsplugin * plg;
	
    nsscene * scn;
    nsrouter * m_router;
    nsui_button_comp * btn;
    nsui_canvas_comp * can;
};


int main()
{
    nsengine e;
    button_funcs bf;
	
    bf.m_router = new nsrouter;
    e.init(new nsgl_driver);

        alGenSources(1, &source);
	
    nsgl_window wind(ivec2(800,600), "Basic Test");
    viewport * vp = wind.vid_context()->insert_viewport("main_view",fvec4(0.0f,0.0f,1.0f,1.0f));
	
    nsplugin * plg = nsep.create<nsplugin>("most_basic_test");
    plg->manager<nsscene_manager>()->set_save_mode(nsasset_manager::text);
    plg->enable(true);
    bf.plg = plg;

    setup_input_map(plg);

    nsaudio_clip * ac = plg->load<nsaudio_clip>("./import/example2.ogg", false);
	ac->read_data();
	plg->save(ac);
	
	alSourcei(source, AL_BUFFER, ac->al_id());

    nsinput_map * im = plg->get<nsinput_map>("basic_input");

    nsinput_map::trigger change_vp("mouse_pressed_in_viewport", nsinput_map::t_pressed);
    change_vp.add_key_mod(nsinput_map::key_any);
    im->add_mouse_trigger("main_global_ctxt", nsinput_map::left_button, change_vp);

    nsscene * new_scene = plg->create<nsscene>("new_scene");
    new_scene->enable(true);
	bf.scn = new_scene;
	
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
    //nsentity * skbox = plg->create_skydome("sky_box", nse.import_dir() + "skydomes/space.cube", "jpg", "skydomes");
    //new_scene->set_skydome(skbox);

    new_scene->add(cam,nullptr,true,fvec3(0,0,-20));
    new_scene->add(dirl, nullptr, false, fvec3(5.0f, 5.0f, -20.0f), orientation(fvec4(1,0,0,20.0f)));
    uint32 plid = new_scene->add(point_light, nullptr, false, fvec3(5.0f, 20.0f, -20.0f), orientation(fvec4(1,0,0,20.0f)));

	instance_tform * itf =
		&point_light->get<nstform_comp>()->per_scene_info(new_scene)->m_tforms[plid];

	new_scene->add(spot_light, itf, false, fvec3(20.0f, 5.0f, -20.0f), orientation(fvec4(1,0,0,20.0f)));

    new_scene->add_gridded(grass_tile,ivec3(8,8,1));

	new_scene->add(stone_tile,
				   &grass_tile->get<nstform_comp>()->per_scene_info(new_scene)->m_tforms[3],
				   true,
				   fvec3(0,0,-20));
	
	grass_tile->get<nstform_comp>()->per_scene_info(
	   new_scene)->m_tforms[3].set_parent(
		   &spot_light->get<nstform_comp>()->per_scene_info(new_scene)->m_tforms[0], true);

	
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
    nsfont * fnt = plg->load<nsfont>(nse.import_dir() + "sample.otf",true);
    fnt->set_point_size(30);

    uitxt->font_material_id = fnt_mat->full_id();
    uitxt->font_id = fnt->full_id();
    uitxt->text = ">>>";
    uitxt->text_shader_id = nse.core()->get<nsshader>(UI_TEXT_SHADER)->full_id();
    uitxt->text_alignment = nsui_text_comp::middle_left;
    uitxt->margins = uivec4(3,3,3,3);

    nsui_text_input_comp * uitxt_input = ui_button->create<nsui_text_input_comp>();
    uitxt_input->cursor_color = fvec4(1,1,1,0.8);
    uitxt_input->cursor_pixel_width = 2;
    uitxt_input->cursor_blink_rate_ms = 430.0f;
	
    nsui_button_comp * uibtn = ui_button->create<nsui_button_comp>();
    bf.m_router->connect(&bf,&button_funcs::on_new_game,uibtn->pressed);
    bf.btn = uibtn;
    uibtn->button_states[0].border_size = fvec4(10,10,10,10);
    uibtn->button_states[0].border_color = fvec4(0,0,0,0.7);
    uibtn->button_states[0].mat_color = fvec4(0.3, 0.3, 0.3, 0.5);
    uibtn->button_states[0].text_color = fvec4(1.0,1.0,1.0,1.0);

    uibtn->button_states[1].border_size = fvec4(0,0,10,0);
    uibtn->button_states[1].border_color = fvec4(0,0,0,0.7);
    uibtn->button_states[1].mat_color = fvec4(0.0, 0.0, 0.0, 0.5);
    uibtn->button_states[1].text_color = fvec4(1.0,1.0,1.0,1.0);

    uibtn->button_states[2].border_color = fvec4(0,0,1,1);
    uibtn->button_states[2].border_size = fvec4(5,5,5,5);
    uibtn->button_states[2].mat_color = fvec4(1,0,0,1);
    uibtn->button_states[2].mat_color_mult = fvec4(0.7,0,0,1);

    uibtn->button_states[3].border_color = fvec4(0.7,0.7,0.7,1);
    uibtn->button_states[3].border_size = fvec4(5,5,5,5);
    uibtn->button_states[3].mat_color = fvec4(0.4,0.4,0.4,0.6);
	
    nsui_canvas_comp * cc = canvas->create<nsui_canvas_comp>();
    cc->enable(true);
    cc->add(ui_button);

    nsrect_tform_comp * tuic = ui_button->get<nsrect_tform_comp>();
    auto pic = tuic->canvas_info(cc);
    pic->anchor_rect = fvec4(0.1f,0.9f,0.1f,0.9f);
    float h = 40;
    float w = 100;
    pic->pixel_offset_rect = fvec4(-w/2, -h/2, w/2, h/2);
    pic->pivot = fvec2(0.5f,0.5f);
    pic->layer = 0;
    pic->angle = 0.0f;

//    plg->save_all_in_plugin();
//    nsep.save(plg);

    vp->ui_canvases.push_back(canvas);
    vp->camera = plg->get<nsentity>("scenecam");
    nse.set_active_scene(plg->get<nsscene>("new_scene"));


    nse.system<nscamera_system>()->setup_input_map(im, "main_global_ctxt");
    nse.system<nsselection_system>()->setup_input_map(im, "main_global_ctxt");
    nse.system<nsui_system>()->setup_input_map(im, "main_global_ctxt");
    nse.system<nsinput_system>()->set_input_map(im->full_id());
    nse.system<nsinput_system>()->push_context("main_global_ctxt");
	
    e.start();
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
    nsinput_map * im = plg->create<nsinput_map>("basic_input");
    im->create_context("main_global_ctxt");

    //nsinput_map::trigger change_vp("mouse_pressed_in_viewport", nsinput_map::t_pressed);
    //change_vp.add_key_mod(nsinput_map::key_any);
    //im->add_mouse_trigger("main_global_ctxt", nsinput_map::left_button, change_vp);
}
