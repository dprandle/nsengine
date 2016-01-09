#define INPUTTEST

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
nsplugin * setup_basic_plugin();
void setup_input_map(nsplugin * plg);


/*
  This test adds camera controls and movement controls to an input map from scratch
  The generated input map can be used in other tests
  Also made sure that saving and loading the input map works in both binary and text saving modes
*/
int main()
{
    glfw_setup(ivec2(400,400), false, "Build And Battle 1.0.0");
	
    uint32 i = nse.create_context();
    nse.start();

    nsplugin * plg = setup_basic_plugin();
	setup_input_map(plg);

	// Save the input map
	plg->save<nsinput_map>("basic_input");

    while (glfw_window_open())
    {
        nse.update();
        glfw_update();
    }

    nse.shutdown();
	glfw_shutdown();
    return 0;
}

nsplugin * setup_basic_plugin()
{
    nsplugin * plg = nse.create_plugin("testplug");
    nsscene * scn = plg->create<nsscene>("mainscene");
	scn->set_bg_color(fvec3(0.7f, 0.7f, 1.0f));
	nse.system<nsrender_system>()->set_fog_color(scn->bg_color());
	nse.set_current_scene(scn, true, false);
	
    nsentity * cam = plg->create_camera("scenecam", 60.0f, uivec2(400, 400), fvec2(DEFAULT_Z_NEAR, DEFAULT_Z_FAR));
	nsentity * dirl = plg->create_dir_light("dirlight", 0.8f, 0.2f);
	scn->set_camera(cam);
	scn->add(dirl, fvec3(20.0f, 20.0f, -50.0f));
	
    nsentity * alpha_tile = plg->create_tile("alpha_tile", fvec4(1.0f, 0.0f, 0.0f, 0.5f), 16.0f, 0.5f, fvec3(1.0f), true);
	nsentity * tile_grass = plg->create_tile("grasstile", nse.import_dir() + "diffuseGrass.png", nse.import_dir() + "normalGrass.png", fvec4(0.0, 0.0, 1.0, 0.5f), 16.0f, 0.5f, fvec3(0.5f,0.0f,0.0f), true);

	nsentity * point_light = plg->create_point_light("point_light", 1.0f, 0.0f, 100.0f);
	//alpha_tile->get<nssel_comp>()->enable_transparent_picking(true);
	
	//plg->get<nsmaterial>("grasstile")->set_alpha_blend(true);
	//plg->get<nsmaterial>("grasstile")->use_alpha_from_color(true);
	scn->add(point_light);
    scn->add_gridded(tile_grass, ivec3(16, 16, 1), fvec3(0.0f,0.0f,10.0f));
	scn->add_gridded(alpha_tile, ivec3(4, 4, 1), fvec3(0.0f,0.0f,0.0f));
	return plg;
}

void setup_input_map(nsplugin * plg)
{
	nsinput_map * im = plg->create<nsinput_map>("basic_input");
	im->create_context("Main");
		
	nsinput_map::trigger camforward(
		NSCAM_FORWARD,
		nsinput_map::t_toggle,
		nsinput_map::axis_mouse_xpos | nsinput_map::axis_mouse_ypos
		);
	camforward.add_mouse_mod(nsinput_map::any_button);
	im->add_key_trigger("Main", nsinput_map::key_w, camforward);

	nsinput_map::trigger cambackward(
		NSCAM_BACKWARD,
		nsinput_map::t_toggle,
		nsinput_map::axis_mouse_xpos | nsinput_map::axis_mouse_ypos
		);
	cambackward.add_mouse_mod(nsinput_map::any_button);
	im->add_key_trigger("Main", nsinput_map::key_s, cambackward);

	nsinput_map::trigger camleft(
		NSCAM_LEFT,
		nsinput_map::t_toggle,
		nsinput_map::axis_mouse_xpos | nsinput_map::axis_mouse_ypos
		);
	camleft.add_mouse_mod(nsinput_map::any_button);
	im->add_key_trigger("Main", nsinput_map::key_a, camleft);

	nsinput_map::trigger camright(
		NSCAM_RIGHT,
		nsinput_map::t_toggle,
		nsinput_map::axis_mouse_xpos | nsinput_map::axis_mouse_ypos
		);
	camright.add_mouse_mod(nsinput_map::any_button);
	im->add_key_trigger("Main", nsinput_map::key_d, camright);

	nsinput_map::trigger camtilt(
		NSCAM_TILTPAN,
		nsinput_map::t_pressed,
		nsinput_map::axis_mouse_xdelta | nsinput_map::axis_mouse_ydelta | nsinput_map::axis_mouse_xpos | nsinput_map::axis_mouse_ypos);
    camtilt.add_key_mod(nsinput_map::key_any);
    camtilt.add_mouse_mod(nsinput_map::right_button);
	im->add_mouse_trigger("Main", nsinput_map::movement, camtilt);

	nsinput_map::trigger cammove(
		NSCAM_MOVE,
		nsinput_map::t_pressed,
		nsinput_map::axis_mouse_xdelta | nsinput_map::axis_mouse_ydelta | nsinput_map::axis_mouse_xpos | nsinput_map::axis_mouse_ypos
		);
	cammove.add_key_mod(nsinput_map::key_any);
    cammove.add_mouse_mod(nsinput_map::left_button);
    cammove.add_mouse_mod(nsinput_map::right_button);
    im->add_mouse_trigger("Main", nsinput_map::movement,cammove);

	nsinput_map::trigger camzoom(
		NSCAM_ZOOM,
		nsinput_map::t_pressed,
		nsinput_map::axis_scroll_delta | nsinput_map::axis_mouse_xpos | nsinput_map::axis_mouse_ypos);
	camzoom.add_key_mod(nsinput_map::key_any);
    im->add_mouse_trigger("Main", nsinput_map::scrolling, camzoom);

	nsinput_map::trigger camtopview_0(
        NSCAM_TOPVIEW_0,
        nsinput_map::t_pressed,
		nsinput_map::axis_mouse_xpos | nsinput_map::axis_mouse_ypos
        );
    camtopview_0.add_mouse_mod(nsinput_map::any_button);
    im->add_key_trigger("Main", nsinput_map::key_keypad_8, camtopview_0);

    nsinput_map::trigger cameraiso_0(
        NSCAM_ISOVIEW_0,
        nsinput_map::t_pressed,
		nsinput_map::axis_mouse_xpos | nsinput_map::axis_mouse_ypos
        );
    cameraiso_0.add_mouse_mod(nsinput_map::any_button);
    im->add_key_trigger("Main", nsinput_map::key_keypad_5, cameraiso_0);

    nsinput_map::trigger camfrontview_0(
        NSCAM_FRONTVIEW_0,
        nsinput_map::t_pressed,
		nsinput_map::axis_mouse_xpos | nsinput_map::axis_mouse_ypos
        );
    camfrontview_0.add_mouse_mod(nsinput_map::any_button);
    im->add_key_trigger("Main", nsinput_map::key_keypad_2, camfrontview_0);

	nsinput_map::trigger camtopview_120(
		NSCAM_TOPVIEW_120,
        nsinput_map::t_pressed,
		nsinput_map::axis_mouse_xpos | nsinput_map::axis_mouse_ypos
        );
	camtopview_120.add_mouse_mod(nsinput_map::any_button);
    im->add_key_trigger("Main", nsinput_map::key_keypad_7, camtopview_120);

    nsinput_map::trigger cameraiso_120(
        NSCAM_ISOVIEW_120,
        nsinput_map::t_pressed,
		nsinput_map::axis_mouse_xpos | nsinput_map::axis_mouse_ypos
        );
    cameraiso_120.add_mouse_mod(nsinput_map::any_button);
    im->add_key_trigger("Main", nsinput_map::key_keypad_4, cameraiso_120);

    nsinput_map::trigger camfrontview_120(
        NSCAM_FRONTVIEW_120,
        nsinput_map::t_pressed,
		nsinput_map::axis_mouse_xpos | nsinput_map::axis_mouse_ypos
        );
    camfrontview_120.add_mouse_mod(nsinput_map::any_button);
    im->add_key_trigger("Main", nsinput_map::key_keypad_1, camfrontview_120);

	nsinput_map::trigger camtopview_240(
        NSCAM_TOPVIEW_240,
        nsinput_map::t_pressed,
		nsinput_map::axis_mouse_xpos | nsinput_map::axis_mouse_ypos
        );
    camtopview_240.add_mouse_mod(nsinput_map::any_button);
    im->add_key_trigger("Main", nsinput_map::key_keypad_9, camtopview_240);

    nsinput_map::trigger cameraiso_240(
        NSCAM_ISOVIEW_240,
        nsinput_map::t_pressed,
		nsinput_map::axis_mouse_xpos | nsinput_map::axis_mouse_ypos
        );
    cameraiso_240.add_mouse_mod(nsinput_map::any_button);
    im->add_key_trigger("Main", nsinput_map::key_keypad_6, cameraiso_240);

    nsinput_map::trigger camfrontview_240(
        NSCAM_FRONTVIEW_240,
        nsinput_map::t_pressed,
		nsinput_map::axis_mouse_xpos | nsinput_map::axis_mouse_ypos
        );
    camfrontview_240.add_mouse_mod(nsinput_map::any_button);
    im->add_key_trigger("Main", nsinput_map::key_keypad_3, camfrontview_240);
	
	nsinput_map::trigger camtogglemode(
        NSCAM_TOGGLEMODE,
        nsinput_map::t_pressed
        );
    camtogglemode.add_mouse_mod(nsinput_map::any_button);
    im->add_key_trigger("Main", nsinput_map::key_f, camtogglemode);

	nsinput_map::trigger selectentity(
		NSSEL_SELECT,
		nsinput_map::t_pressed,
		nsinput_map::axis_mouse_xpos | nsinput_map::axis_mouse_ypos
		);
    im->add_mouse_trigger("Main", nsinput_map::left_button,selectentity);

	nsinput_map::trigger shiftselect(
		NSSEL_SHIFTSELECT,
		nsinput_map::t_pressed,
		nsinput_map::axis_mouse_xpos | nsinput_map::axis_mouse_ypos
		);
	shiftselect.add_key_mod(nsinput_map::key_lshift);
    im->add_mouse_trigger("Main", nsinput_map::movement,shiftselect);

	nsinput_map::trigger multiselect(
		NSSEL_MULTISELECT,
		nsinput_map::t_pressed,
		nsinput_map::axis_mouse_xpos | nsinput_map::axis_mouse_ypos
		);
	multiselect.add_key_mod(nsinput_map::key_lctrl);
    im->add_mouse_trigger("Main", nsinput_map::left_button,multiselect);

    nsinput_map::trigger selectmove(
        NSSEL_MOVE,
        nsinput_map::t_pressed,
        nsinput_map::axis_mouse_xdelta | nsinput_map::axis_mouse_ydelta
        );
    selectmove.add_mouse_mod(nsinput_map::left_button);
    im->add_mouse_trigger("Main", nsinput_map::movement,selectmove);
	
	nsinput_map::trigger selectmovexy(
		NSSEL_MOVE_XY,
		nsinput_map::t_pressed,
		nsinput_map::axis_mouse_xdelta | nsinput_map::axis_mouse_ydelta
		);
    selectmovexy.add_key_mod(nsinput_map::key_x);
    selectmovexy.add_key_mod(nsinput_map::key_y);
	selectmovexy.add_mouse_mod(nsinput_map::left_button);
    im->add_mouse_trigger("Main", nsinput_map::movement,selectmovexy);

	nsinput_map::trigger selectmovezy(
		NSSEL_MOVE_ZY,
		nsinput_map::t_pressed,
		nsinput_map::axis_mouse_xdelta | nsinput_map::axis_mouse_ydelta
		);
	selectmovezy.add_key_mod(nsinput_map::key_z);
	selectmovezy.add_key_mod(nsinput_map::key_y);
	selectmovezy.add_mouse_mod(nsinput_map::left_button);
    im->add_mouse_trigger("Main", nsinput_map::movement,selectmovezy);

	nsinput_map::trigger selectmovezx(
        NSSEL_MOVE_ZX,
		nsinput_map::t_pressed,
		nsinput_map::axis_mouse_xdelta | nsinput_map::axis_mouse_ydelta
		);
	selectmovezx.add_key_mod(nsinput_map::key_z);
	selectmovezx.add_key_mod(nsinput_map::key_x);
    selectmovezx.add_mouse_mod(nsinput_map::left_button);
    im->add_mouse_trigger("Main", nsinput_map::movement,selectmovezx);

	nsinput_map::trigger selectmovex(
		NSSEL_MOVE_X,
		nsinput_map::t_pressed,
		nsinput_map::axis_mouse_xdelta | nsinput_map::axis_mouse_ydelta
		);
	selectmovex.add_mouse_mod(nsinput_map::left_button);
	selectmovex.add_key_mod(nsinput_map::key_x);
    im->add_mouse_trigger("Main", nsinput_map::movement,selectmovex);

	nsinput_map::trigger selectmovey(
		NSSEL_MOVE_Y,
		nsinput_map::t_pressed,
		nsinput_map::axis_mouse_xdelta | nsinput_map::axis_mouse_ydelta
		);
	selectmovey.add_key_mod(nsinput_map::key_y);
	selectmovey.add_mouse_mod(nsinput_map::left_button);
    im->add_mouse_trigger("Main", nsinput_map::movement,selectmovey);

	nsinput_map::trigger selectmovez(
        NSSEL_MOVE_Z,
		nsinput_map::t_pressed,
		nsinput_map::axis_mouse_xdelta | nsinput_map::axis_mouse_ydelta
		);
	selectmovez.add_key_mod(nsinput_map::key_z);
    selectmovez.add_mouse_mod(nsinput_map::left_button);
    im->add_mouse_trigger("Main", nsinput_map::movement,selectmovez);

	nsinput_map::trigger selectmovetoggle(
        NSSEL_MOVE_TOGGLE,
		nsinput_map::t_toggle
		);
	selectmovetoggle.add_key_mod(nsinput_map::key_any);
    im->add_mouse_trigger("Main", nsinput_map::left_button,selectmovetoggle);

	nse.system<nsinput_system>()->set_input_map(im->full_id());
	nse.system<nsinput_system>()->push_context("Main");
	
}
