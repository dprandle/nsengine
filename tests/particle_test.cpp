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
#include <nsbuild_system.h>
#include <nsrender_comp.h>
#include <nssel_comp.h>
#include <nstile_brush_comp.h>
#include <nsrender_system.h>
#include <nsmesh.h>
#include <nsanim_comp.h>

void setup_input_map(nsplugin * plg);

int main()
{
	ivec2 screen_size(800,600);
	
	// This creates a window, then an opengl context and assigns it to the current thread/window
    glfw_setup(screen_size, false, "Build And Battle 1.0.0");

	// Create GLEW context - this loads all of the opengl extensions for the current opengl context
	// This also creates all of the engine stuff
	nse.create_context();

	// This tells the engine to add and initialize all the main systems/components/factories 
    nse.start(screen_size);

	// create a plugin to allow us to load and save groups of stuff easily
    nsplugin * plg = nse.create_plugin("ship_resources.bbp");
	plg->bind();
	nse.set_active(plg);

	// create a mapping of keys to events
	setup_input_map(plg);

	// Lets create an empty scene to start putting stuff in
	nsscene * new_level = plg->create<nsscene>("level_one");
	nse.set_current_scene(new_level, true, false);

	// Now lets create a new camera, and a directional light to light up everything
	nsentity * cam = plg->create_camera("camera",
										60.0f,
										uivec2(400, 400),
										fvec2(DEFAULT_Z_NEAR, DEFAULT_Z_FAR));
//	nsentity * dirl = plg->create_dir_light("dirlight", 0.8f, 0.2f);
//	scn->set_camera(cam);
//	scn->add(dirl, fvec3(20.0f, 20.0f, -50.0f));


	// while the window is open update all engine stuff, then do the OS message update stuff (glfw)
	while (glfw_window_open())
    {
        nse.update();
        glfw_update();
    }

    nse.shutdown();
	glfw_shutdown();
    return 0;
}

void setup_input_map(nsplugin * plg)
{
	nsinput_map * im = plg->create<nsinput_map>("ship_controls");
	im->create_context("main_game_controls");
		
	nsinput_map::trigger ship_left(
		"ship_left",
		nsinput_map::t_toggle);
	ship_left.add_mouse_mod(nsinput_map::any_button); // Doesnt matter if mouse buttons are pressed

	nsinput_map::trigger ship_right(
		"ship_right",
		nsinput_map::t_toggle);
	ship_right.add_mouse_mod(nsinput_map::any_button); // Doesnt matter if mouse buttons are pressed

	nsinput_map::trigger ship_up(
		"ship_up",
		nsinput_map::t_toggle);
	ship_up.add_mouse_mod(nsinput_map::any_button); // Doesnt matter if mouse buttons are pressed

	nsinput_map::trigger ship_down(
		"ship_down",
		nsinput_map::t_toggle);
	ship_down.add_mouse_mod(nsinput_map::any_button); // Doesnt matter if mouse buttons are pressed

	nsinput_map::trigger ship_fire(
		"ship_fire",
		nsinput_map::t_pressed);
	ship_down.add_mouse_mod(nsinput_map::any_button); // Doesnt matter if mouse buttons are pressed

	im->add_key_trigger("main_game_controls", nsinput_map::key_a, ship_left);
	im->add_key_trigger("main_game_controls", nsinput_map::key_d, ship_right);
	im->add_key_trigger("main_game_controls", nsinput_map::key_w, ship_up);
	im->add_key_trigger("main_game_controls", nsinput_map::key_s, ship_down);

	nse.system<nsinput_system>()->set_input_map(im->full_id());
	nse.system<nsinput_system>()->push_context("main_game_controls");
}

