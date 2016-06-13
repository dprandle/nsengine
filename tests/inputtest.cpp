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
#include <nsplugin_manager.h>
#include <nsmaterial.h>
#include <nsui_system.h>
#include <nsrender_system.h>
#include <nsui_canvas_comp.h>

nsplugin * setup_basic_plugin();
void setup_input_map(nsplugin * plg);
void create_button(
	nsplugin * plg,
	nsentity * canvas,
	const nsstring & tex_name,
	const nsstring & tex_ext,
	const fvec4 & pix_offset,
	const fvec4 & anchors,
	int32 layer,
    float angle = 0.0f,
    const fvec4 & tex_coords = fvec4(0,0.75,1,1));

/*
  This test adds camera controls and movement controls to an input map from scratch
  The generated input map can be used in other tests
  Also made sure that saving and loading the input map works in both binary and text saving modes
*/
int main()
{
    glfw_setup(ivec2(400,400), false, "Build And Battle 1.0.0");


    nse.start();
    nse._create_factory_systems();

	nsgl_driver * gl_driver = nse.create_video_driver<nsgl_driver>();
	gl_driver->create_context();
    gl_driver->init(true);

	nse.setup_core_plug();


    nsplugin * plg = setup_basic_plugin();
	setup_input_map(plg);

    //auto tfc = plg->get<nsentity>("button_start-screen")->get<nsrect_tform_comp>();
    //auto tfc2 = plg->get<nsentity>("button_new-match")->get<nsrect_tform_comp>();
    //auto can = plg->get<nsentity>("canvas_ent")->get<nsui_canvas_comp>();
	
	// Save the input map
    //plg->save<nsinput_map>("basic_input");

    //plg->save<nsfont>("cosn");

    while (glfw_window_open())
    {
        nse.update();
        //nse.system<nsui_system>()->set_active_viewport(nse.system<nsrender_system>()->viewport("main_view"));
        //nse.system<nsui_system>()->push_draw_calls();
        nse.system<nsrender_system>()->render();

        //tfc->canvas_info(can)->angle += nse.timer()->dt() * 90.0f;
        //tfc2->canvas_info(can)->angle += nse.timer()->dt() * 90.0f;
        glfw_update();
    }

    nse.shutdown();
	glfw_shutdown();
    return 0;
}

nsplugin * setup_basic_plugin()
{
    nsplugin * plg = nsep.create("testplug");
    plg->bind();
    nsep.set_active(plg);

    nsscene * scn = plg->create<nsscene>("mainscene");
	scn->set_bg_color(fvec3(0.7f, 0.7f, 1.0f));
	nsep.active()->set_current_scene(scn, true, false);
	
    nsentity * cam = plg->create_camera("scenecam", 60.0f, uivec2(400, 400), fvec2(DEFAULT_Z_NEAR, DEFAULT_Z_FAR));
    nsentity * dirl = plg->create_dir_light("dirlight", 1.0f, 0.1f,fvec3(1.0f),true,0.5f,2);	
    scn->add(cam);
    scn->add(dirl, fvec3(5.0f, 5.0f, -20.0f), orientation(fvec4(1,0,0,20.0f)));
	
   // nsentity * alpha_tile = plg->create_tile("alpha_tile", fvec4(1.0f, 1.0f, 0.0f, 0.5f), 16.0f, 0.5f, fvec3(1.0f), true);
    nsentity * tile_grass = plg->create_tile("grasstile", nse.import_dir() + "diffuseGrass.png", nse.import_dir() + "normalGrass.png", fvec4(0.0, 0.0, 1.0, 0.5f), 16.0f, 0.5f, fvec3(0.5f,0.0f,0.0f), true);

   // nsentity * spot_light = plg->create_spot_light("spot_light", 1.0f, 0.2f, 30.0f, 20.0f);
    //nsentity * point_light = plg->create_point_light("point_light", 1.0f, 0.0f, 20.0f);
    //nsentity * canvas = plg->create<nsentity>("canvas_ent");

    //canvas->create<nsui_canvas_comp>();

//    float pix_offset = 0;
//    create_button(plg, canvas, "fun", ".jpg", fvec4(),
//                  fvec4(0,0,1,1), 0, 0.0f, fvec4(0,0,1,1));
//    create_button(plg, canvas,"start-screen", ".png",
//                  fvec4(-350/2, -227.5, 350/2, 227.5), fvec4(0.5f,0.5f,0.5f,0.5f), 1, 0.0f, fvec4(0,0,1,1));
//    pix_offset = 2*88;
//    create_button(plg, canvas,"new-match", ".png", fvec4(-321/2, -308/8 + pix_offset, 321/2, 308/8 + pix_offset), fvec4(0.5f,0.5f,0.5f,0.5f), 2, 0.0f);
//    pix_offset = 88;
//    create_button(plg, canvas,"join-match", ".png", fvec4(-321/2, -308/8 + pix_offset, 321/2, 308/8 + pix_offset), fvec4(0.5f,0.5f,0.5f,0.5f), 2);
//    pix_offset = 0;
//    create_button(plg, canvas,"plugins", ".png", fvec4(-321/2, -308/8 + pix_offset, 321/2, 308/8 + pix_offset), fvec4(0.5f,0.5f,0.5f,0.5f), 2);
//    pix_offset = -88;
//    create_button(plg, canvas,"options", ".png", fvec4(-321/2, -308/8 + pix_offset, 321/2, 308/8 + pix_offset), fvec4(0.5f,0.5f,0.5f,0.5f), 2);
//    pix_offset = -2*88;
//    create_button(plg, canvas,"exit-game", ".png", fvec4(-321/2, -308/8 + pix_offset, 321/2, 308/8 + pix_offset), fvec4(0.5f,0.5f,0.5f,0.5f), 2);


    //alpha_tile->get<nssel_comp>()->enable_transparent_picking(false);
    //scn->add(point_light);
    //scn->add(spot_light, fvec3(10,10,0.0f));
    scn->add_gridded(tile_grass, ivec3(16, 16, 1), fvec3(0.0f,0.0f,10.0f));
    //scn->add_gridded(alpha_tile, ivec3(4, 4, 1), fvec3(0.0f,0.0f,0.0f));

    nsrender::viewport vp(fvec4(0.0f,0.0f,1.0f,1.0f),cam);
    //vp.ui_canvases.push_back(canvas);
    nse.system<nsrender_system>()->insert_viewport("main_view",vp);

	return plg;
}

void create_button(
	nsplugin * plg,
	nsentity * canvas,
	const nsstring & tex_name,
	const nsstring & tex_ext,
	const fvec4 & pix_offset,
	const fvec4 & anchors,
	int32 layer,
    float angle,
    const fvec4 & tex_coords)
{
    nsentity * ui_button = plg->create<nsentity>("button_" + tex_name);
    nsui_material_comp * uic = ui_button->create<nsui_material_comp>();
    nsui_canvas_comp * uicc = canvas->get<nsui_canvas_comp>();
	
    uicc->add(ui_button);
	nsrect_tform_comp * tuic = ui_button->get<nsrect_tform_comp>();

    nsmaterial * mat = plg->create<nsmaterial>("btn_contents_mat_" + tex_name);
    nsmaterial * pad_mat = plg->create<nsmaterial>("btn_padding_mat_" + tex_name);
    pad_mat->set_color_mode(true);
    pad_mat->set_color(fvec4(1.0f,1.0f,0.0f,0.0f));
    nstex2d * reg_tex = plg->load<nstex2d>(nse.import_dir() + tex_name + tex_ext, true);
    mat->add_tex_map(nsmaterial::diffuse, tex_map_info(reg_tex->full_id(),tex_coords), true);
    mat->set_color_mode(false);

    auto pic = tuic->canvas_info(uicc);
    pic->anchor_rect = anchors;
    pic->pixel_offset_rect = pix_offset;//fvec4(-321/2, -308/8 + pix_offset, 321/2, 308/8 + pix_offset);
    pic->pivot = fvec2(0.5f,0.5f);
    pic->layer = layer;
    pic->angle = angle;

    uic->content_properties.mat_id = mat->full_id();
    uic->outer_properties.border_color = fvec4(0.0f,0.0f,0.0f,1.0f);
    uic->outer_properties.border = fvec4(3,3,3,3);

    uic->mat_shader_id = nse.core()->get<nsshader>(UI_SHADER)->full_id();
    uic->border_shader_id = nse.core()->get<nsshader>(UI_BORDER_SHADER)->full_id();
    uic->show = true;

    if (tex_name != "fun" && tex_name != "start-screen")
    {
        tuic->set_parent(uicc, plg->get<nsentity>("button_start-screen")->get<nsrect_tform_comp>());
    }
}

void setup_input_map(nsplugin * plg)
{
	nsinput_map * im = plg->create<nsinput_map>("basic_input");
	im->create_context("Main");

    nsinput_map::trigger toggle_cam(
        "ToggleCam",
        nsinput_map::t_pressed);
    toggle_cam.add_mouse_mod(nsinput_map::any_button);
    im->add_key_trigger("Main", nsinput_map::key_g, toggle_cam);

	nsinput_map::trigger camforward(
		NSCAM_FORWARD,
		nsinput_map::t_both
		);
	camforward.add_mouse_mod(nsinput_map::any_button);
	im->add_key_trigger("Main", nsinput_map::key_w, camforward);

	nsinput_map::trigger cambackward(
		NSCAM_BACKWARD,
		nsinput_map::t_both
		);
	cambackward.add_mouse_mod(nsinput_map::any_button);
	im->add_key_trigger("Main", nsinput_map::key_s, cambackward);

	nsinput_map::trigger camleft(
		NSCAM_LEFT,
		nsinput_map::t_both
		);
	camleft.add_mouse_mod(nsinput_map::any_button);
	im->add_key_trigger("Main", nsinput_map::key_a, camleft);

	nsinput_map::trigger camright(
		NSCAM_RIGHT,
		nsinput_map::t_both
		);
	camright.add_mouse_mod(nsinput_map::any_button);
	im->add_key_trigger("Main", nsinput_map::key_d, camright);

	nsinput_map::trigger camtilt(
		NSCAM_TILTPAN,
		nsinput_map::t_pressed);
    camtilt.add_key_mod(nsinput_map::key_any);
    camtilt.add_mouse_mod(nsinput_map::right_button);
	im->add_mouse_trigger("Main", nsinput_map::movement, camtilt);

	nsinput_map::trigger cammove(
		NSCAM_MOVE,
		nsinput_map::t_pressed);
	cammove.add_key_mod(nsinput_map::key_any);
    cammove.add_mouse_mod(nsinput_map::left_button);
    cammove.add_mouse_mod(nsinput_map::right_button);
    im->add_mouse_trigger("Main", nsinput_map::movement,cammove);

	nsinput_map::trigger camzoom(
		NSCAM_ZOOM,
		nsinput_map::t_pressed);
	camzoom.add_key_mod(nsinput_map::key_any);
    im->add_mouse_trigger("Main", nsinput_map::scrolling, camzoom);

	nsinput_map::trigger camtopview_0(
        NSCAM_TOPVIEW_0,
        nsinput_map::t_pressed);
    camtopview_0.add_mouse_mod(nsinput_map::any_button);
    im->add_key_trigger("Main", nsinput_map::key_keypad_8, camtopview_0);

    nsinput_map::trigger cameraiso_0(
        NSCAM_ISOVIEW_0,
        nsinput_map::t_pressed);
    cameraiso_0.add_mouse_mod(nsinput_map::any_button);
    im->add_key_trigger("Main", nsinput_map::key_keypad_5, cameraiso_0);

    nsinput_map::trigger camfrontview_0(
        NSCAM_FRONTVIEW_0,
        nsinput_map::t_pressed);
    camfrontview_0.add_mouse_mod(nsinput_map::any_button);
    im->add_key_trigger("Main", nsinput_map::key_keypad_2, camfrontview_0);

	nsinput_map::trigger camtopview_120(
		NSCAM_TOPVIEW_120,
        nsinput_map::t_pressed);
	camtopview_120.add_mouse_mod(nsinput_map::any_button);
    im->add_key_trigger("Main", nsinput_map::key_keypad_7, camtopview_120);

    nsinput_map::trigger cameraiso_120(
        NSCAM_ISOVIEW_120,
        nsinput_map::t_pressed);
    cameraiso_120.add_mouse_mod(nsinput_map::any_button);
    im->add_key_trigger("Main", nsinput_map::key_keypad_4, cameraiso_120);

    nsinput_map::trigger camfrontview_120(
        NSCAM_FRONTVIEW_120,
        nsinput_map::t_pressed);
    camfrontview_120.add_mouse_mod(nsinput_map::any_button);
    im->add_key_trigger("Main", nsinput_map::key_keypad_1, camfrontview_120);

	nsinput_map::trigger camtopview_240(
        NSCAM_TOPVIEW_240,
        nsinput_map::t_pressed);
    camtopview_240.add_mouse_mod(nsinput_map::any_button);
    im->add_key_trigger("Main", nsinput_map::key_keypad_9, camtopview_240);

    nsinput_map::trigger cameraiso_240(
        NSCAM_ISOVIEW_240,
        nsinput_map::t_pressed);
    cameraiso_240.add_mouse_mod(nsinput_map::any_button);
    im->add_key_trigger("Main", nsinput_map::key_keypad_6, cameraiso_240);

    nsinput_map::trigger camfrontview_240(
        NSCAM_FRONTVIEW_240,
        nsinput_map::t_pressed);
    camfrontview_240.add_mouse_mod(nsinput_map::any_button);
    im->add_key_trigger("Main", nsinput_map::key_keypad_3, camfrontview_240);
	
	nsinput_map::trigger camtogglemode(
        NSCAM_TOGGLEMODE,
        nsinput_map::t_pressed);
    camtogglemode.add_mouse_mod(nsinput_map::any_button);
    im->add_key_trigger("Main", nsinput_map::key_f, camtogglemode);

	nsinput_map::trigger selectentity(
		NSSEL_SELECT,
		nsinput_map::t_pressed);
    im->add_mouse_trigger("Main", nsinput_map::left_button,selectentity);

	nsinput_map::trigger shiftselect(
		NSSEL_SHIFTSELECT,
		nsinput_map::t_pressed);
	shiftselect.add_key_mod(nsinput_map::key_lshift);
    im->add_mouse_trigger("Main", nsinput_map::movement,shiftselect);
	nsinput_map::trigger multiselect(
		NSSEL_MULTISELECT,
		nsinput_map::t_pressed);
	multiselect.add_key_mod(nsinput_map::key_lctrl);
    im->add_mouse_trigger("Main", nsinput_map::left_button,multiselect);

    nsinput_map::trigger selectmove(
        NSSEL_MOVE,
        nsinput_map::t_pressed);
    selectmove.add_mouse_mod(nsinput_map::left_button);
    im->add_mouse_trigger("Main", nsinput_map::movement,selectmove);
	
	nsinput_map::trigger selectmovexy(
		NSSEL_MOVE_XY,
		nsinput_map::t_pressed);
    selectmovexy.add_key_mod(nsinput_map::key_x);
    selectmovexy.add_key_mod(nsinput_map::key_y);
	selectmovexy.add_mouse_mod(nsinput_map::left_button);
    im->add_mouse_trigger("Main", nsinput_map::movement,selectmovexy);

	nsinput_map::trigger selectmovezy(
		NSSEL_MOVE_ZY,
		nsinput_map::t_pressed);
	selectmovezy.add_key_mod(nsinput_map::key_z);
	selectmovezy.add_key_mod(nsinput_map::key_y);
	selectmovezy.add_mouse_mod(nsinput_map::left_button);
    im->add_mouse_trigger("Main", nsinput_map::movement,selectmovezy);

	nsinput_map::trigger selectmovezx(
        NSSEL_MOVE_ZX,
		nsinput_map::t_pressed);
	selectmovezx.add_key_mod(nsinput_map::key_z);
	selectmovezx.add_key_mod(nsinput_map::key_x);
    selectmovezx.add_mouse_mod(nsinput_map::left_button);
    im->add_mouse_trigger("Main", nsinput_map::movement,selectmovezx);

	nsinput_map::trigger selectmovex(
		NSSEL_MOVE_X,
		nsinput_map::t_pressed);
	selectmovex.add_mouse_mod(nsinput_map::left_button);
	selectmovex.add_key_mod(nsinput_map::key_x);
    im->add_mouse_trigger("Main", nsinput_map::movement,selectmovex);

	nsinput_map::trigger selectmovey(
		NSSEL_MOVE_Y,
		nsinput_map::t_pressed);
	selectmovey.add_key_mod(nsinput_map::key_y);
	selectmovey.add_mouse_mod(nsinput_map::left_button);
    im->add_mouse_trigger("Main", nsinput_map::movement,selectmovey);

	nsinput_map::trigger selectmovez(
        NSSEL_MOVE_Z,
		nsinput_map::t_pressed);
	selectmovez.add_key_mod(nsinput_map::key_z);
    selectmovez.add_mouse_mod(nsinput_map::left_button);
    im->add_mouse_trigger("Main", nsinput_map::movement,selectmovez);

	nsinput_map::trigger selectmovetoggle(
        NSSEL_MOVE_TOGGLE,
		nsinput_map::t_both
		);
	selectmovetoggle.add_key_mod(nsinput_map::key_any);
    im->add_mouse_trigger("Main", nsinput_map::left_button,selectmovetoggle);

    nsinput_map::trigger change_vp("mouse_pressed_in_viewport", nsinput_map::t_pressed);
    change_vp.add_key_mod(nsinput_map::key_any);
    im->add_mouse_trigger("Main", nsinput_map::left_button, change_vp);

	nse.system<nsinput_system>()->set_input_map(im->full_id());
	nse.system<nsinput_system>()->push_context("Main");
	
}
