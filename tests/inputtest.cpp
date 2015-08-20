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

nsplugin * setup_basic_plugin();
void setup_input_map(nsplugin * plg);

int main()
{
    glfw_setup(ivec2(400,400), false, "Build And Battle 1.0.0");
	
    uint32 i = nse.create_context();
    nse.start();

    nsplugin * plg = setup_basic_plugin();
    setup_input_map(plg);

    nse.core()->manager<nsmesh_manager>()->set_save_mode(nsres_manager::text);
    nse.core()->save_all<nsmesh>();
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
    nsplugin * plg = nse.create_plugin("sillyplug");
    nsscene * scn = plg->create<nsscene>("mainscene");
	scn->set_bg_color(fvec3(0.7f, 0.7f, 1.0f));
	nse.set_current_scene(scn, true);
	
    nsentity * cam = plg->create_camera("scenecam", 60.0f, uivec2(400, 400), fvec2(DEFAULT_Z_NEAR, DEFAULT_Z_FAR));
	nsentity * dirl = plg->create_dir_light("dirlight", 0.8f, 0.2f);
	scn->set_camera(cam);
	scn->add(dirl, fvec3(20.0f, 20.0f, -20.0f));
    nsentity * tile = plg->create_tile("grasstile", nse.import_dir() + "diffuseGrass.png", nse.import_dir() + "normalGrass.png", fvec3(1.0, 1.0, 0.0), 16.0f, 0.5f, fvec3(1.0f), true);
    scn->add_gridded(tile, ivec3(32, 32, 2), fvec3(0.0f,0.0f,0.0f));
	return plg;
}

void setup_input_map(nsplugin * plg)
{
	nsinput_map * im = plg->create<nsinput_map>("testinginput");
	im->create_context("Main");
		
	nsinput_map::trigger camforward(
		NSCAM_FORWARD,
		nsinput_map::t_toggle
		);
	camforward.add_mouse_mod(nsinput_map::any_button);
	im->add_key_trigger("Main", nsinput_map::key_w, camforward);

	nsinput_map::trigger cambackward(
		NSCAM_BACKWARD,
		nsinput_map::t_toggle
		);
	cambackward.add_mouse_mod(nsinput_map::any_button);
	im->add_key_trigger("Main", nsinput_map::key_t, cambackward);

	nsinput_map::trigger camleft(
		NSCAM_LEFT,
		nsinput_map::t_toggle
		);
	camleft.add_mouse_mod(nsinput_map::any_button);
	im->add_key_trigger("Main", nsinput_map::key_a, camleft);

	nsinput_map::trigger camright(
		NSCAM_RIGHT,
		nsinput_map::t_toggle
		);
	camright.add_mouse_mod(nsinput_map::any_button);
	im->add_key_trigger("Main", nsinput_map::key_d, camright);

	nsinput_map::trigger camtilt(
		NSCAM_TILTPAN,
		nsinput_map::t_pressed,
		nsinput_map::axis_mouse_xdelta | nsinput_map::axis_mouse_ydelta);
    camtilt.add_key_mod(nsinput_map::key_any);
    camtilt.add_mouse_mod(nsinput_map::right_button);
	im->add_mouse_trigger("Main", nsinput_map::movement, camtilt);

	nsinput_map::trigger cammove(
		NSCAM_MOVE,
		nsinput_map::t_pressed,
		nsinput_map::axis_mouse_xdelta | nsinput_map::axis_mouse_ydelta
		);
	cammove.add_key_mod(nsinput_map::key_any);
    cammove.add_mouse_mod(nsinput_map::left_button);
    cammove.add_mouse_mod(nsinput_map::right_button);
    im->add_mouse_trigger("Main", nsinput_map::movement,cammove);

	nsinput_map::trigger camzoom(
		NSCAM_ZOOM,
		nsinput_map::t_pressed,
		nsinput_map::axis_scroll_delta);
	camzoom.add_key_mod(nsinput_map::key_any);
    im->add_mouse_trigger("Main", nsinput_map::scrolling, camzoom);

	nsinput_map::trigger camtopview(
        NSCAM_TOPVIEW,
        nsinput_map::t_pressed
        );
    camtopview.add_mouse_mod(nsinput_map::any_button);
    im->add_key_trigger("Main", nsinput_map::key_keypad_8, camtopview);

    nsinput_map::trigger cameraiso(
        NSCAM_ISOVIEW,
        nsinput_map::t_pressed
        );
    cameraiso.add_mouse_mod(nsinput_map::any_button);
    im->add_key_trigger("Main", nsinput_map::key_keypad_5, cameraiso);

    nsinput_map::trigger camfrontview(
        NSCAM_FRONTVIEW,
        nsinput_map::t_pressed
        );
    camfrontview.add_mouse_mod(nsinput_map::any_button);
    im->add_key_trigger("Main", nsinput_map::key_keypad_2, camfrontview);

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
    //selectentity.addKeyModifier(NSInputMap::Key_Any);
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
