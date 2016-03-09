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
#include <nstexture.h>
#include <nsmaterial.h>
#include <nsevent.h>
#include <nstimer.h>

void setup_input_map(nsplugin * plg);

// Lets make our own system for moving the ship around with the keys!!

class nsship_movement_system : public nssystem
{
public:

	nsship_movement_system():
		drag(0.1f),
		ship_mass(1.0f)
	{}

	~nsship_movement_system()
	{}

	virtual void init()
	{
		register_handler_func(this, &nsship_movement_system::_handle_action_event);
		register_handler_func(this, &nsship_movement_system::_handle_state_event);

		add_trigger_hash(0, "ship_left");
		add_trigger_hash(1, "ship_right");
		add_trigger_hash(2, "ship_up");
		add_trigger_hash(3, "ship_down");
		add_trigger_hash(4, "ship_fire");
	}

	virtual void update()
	{
		nsentity * ship = nse.current_scene()->entity(nse.active(),"ship");

		nstform_comp * tf = ship->get<nstform_comp>();

		// Find the drag force
		float drag_mag = length(velocity % velocity * drag);
		fvec3 drag_force = normalize(velocity) * drag_mag;
		
		// Leapfrog integration
		fvec3 posAv = tf->wpos() + 0.5f * velocity * nse.timer()->fixed();
		velocity += ((force - drag_force) / ship_mass) * nse.timer()->fixed();
		tf->set_pos(posAv + 0.5f * velocity * nse.timer()->fixed());
	}

	virtual int32 update_priority()
	{
		return 45000;
	}

  private:

	float ship_mass;
	float drag;
	fvec3 force;
	fvec3 velocity;
	
	bool _handle_action_event(nsaction_event * evnt)
	{
		return true;
	}
	
	bool _handle_state_event(nsstate_event * evnt)
	{
		if (evnt->trigger_hash_name == trigger_hash(0))
		{
            if (evnt->toggle)
                force += fvec3(-10.0f, 0.0f, 0.0f);
            else
                force -= fvec3(-10.0f, 0.0f, 0.0f);
		}
		else if (evnt->trigger_hash_name == trigger_hash(1))
		{
            if (evnt->toggle)
                force += fvec3(10.0f, 0.0f, 0.0f);
            else
                force -= fvec3(10.0f, 0.0f, 0.0f);
		}			
		else if (evnt->trigger_hash_name == trigger_hash(2))
		{
            if (evnt->toggle)
                force += fvec3(0.0f, 10.0f, 0.0f);
            else
                force -= fvec3(0.0f, 10.0f, 0.0f);
		}			
		else if (evnt->trigger_hash_name == trigger_hash(3))
		{
            if (evnt->toggle)
                force += fvec3(0.0f, -10.0f, 0.0f);
            else
                force -= fvec3(0.0f, -10.0f, 0.0f);
		}
		return true;
	}

};



int main()
{
	ivec2 screen_size(800,600);
	
	// This creates a window, then an opengl context and assigns it to the current thread/window
    glfw_setup(screen_size, false, "Build And Battle 1.0.0");

	// Create GLEW context - this loads all of the opengl extensions for the current opengl context
	// This also creates all of the engine stuff
	nse.create_context();

	nse.register_system<nsship_movement_system>("nsship_movement_system");

	// This tells the engine to add and initialize all the main systems/components/factories 
    nse.start(screen_size);
//    nse.system<nsrender_system>()->setup_default_rendering();

	// create a plugin to allow us to load and save groups of stuff easily
    nsplugin * plg = nse.create_plugin("ship_resources.bbp");
	plg->bind();
	nse.set_active(plg);

	// create a mapping of keys to events
	setup_input_map(plg);

	// Lets create an empty scene to start putting stuff in
	nsscene * new_level = plg->create<nsscene>("level_one");
	new_level->set_bg_color(fvec4(0.7,0.7,0.9,1.0));
	nse.set_current_scene(new_level, true, false);

	// Now lets create a new camera, and a directional light to light up everything
	nsentity * cam = plg->create_camera("camera",
										60.0f,
										uivec2(screen_size.x,screen_size.y),
										fvec2(DEFAULT_Z_NEAR, DEFAULT_Z_FAR));
	nsentity * dirl = plg->create_dir_light("dirlight", 1.0f, 0.0f);
	new_level->add(dirl, fvec3(20.0f, 20.0f, -50.0f));

	// Now we will make a texture from an image on file, then create a new material
	// and assign the new texture to the material
	nstex2d * ship_tex = plg->load<nstex2d>("./import/my_ship.png");
	nsmaterial * ship_mat = plg->create<nsmaterial>("ship_mat");
	ship_mat->set_alpha_blend(true);
	ship_mat->set_map_tex_id(nsmaterial::diffuse, ship_tex->full_id());
	
	// lets create the 2d plane which we will draw the sprite on
	nsmesh_plane * ship_mesh = plg->create<nsmesh_plane>("ship_mesh");
	ship_mesh->set_dim(fvec2(1,1));

	// Now that all the resources necessary are made, lets make our ship entity
	nsentity * ship = plg->create<nsentity>("ship");

	// Now we can add a render component and use the ship mesh/material
	nsrender_comp * rc = ship->create<nsrender_comp>();
	rc->set_mesh_id(ship_mesh->full_id());
	rc->set_material(0, ship_mat->full_id());

	// Lets add to the map at location 0,0,0 - if you dont specify location this is default
	new_level->add(ship);


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
	nsinput_map * im = plg->create<nsinput_map>("main_game_controls");
	im->create_context("ship_controls");

	nsinput_map::trigger ship_left(
		"ship_left",
		nsinput_map::t_toggle);
	ship_left.add_mouse_mod(nsinput_map::any_button); // Doesnt matter if mouse buttons are pressed

	nsinput_map::trigger ship_right(
		"ship_right",
		nsinput_map::t_toggle);
	ship_right.add_mouse_mod(nsinput_map::any_button);

	nsinput_map::trigger ship_up(
		"ship_up",
		nsinput_map::t_toggle);
	ship_up.add_mouse_mod(nsinput_map::any_button);

	nsinput_map::trigger ship_down(
		"ship_down",
		nsinput_map::t_toggle);
	ship_down.add_mouse_mod(nsinput_map::any_button);

	nsinput_map::trigger ship_fire(
		"ship_fire",
		nsinput_map::t_pressed);
	ship_down.add_mouse_mod(nsinput_map::any_button);

	im->add_key_trigger("ship_controls", nsinput_map::key_a, ship_left);
	im->add_key_trigger("ship_controls", nsinput_map::key_d, ship_right);
	im->add_key_trigger("ship_controls", nsinput_map::key_w, ship_up);
	im->add_key_trigger("ship_controls", nsinput_map::key_s, ship_down);

	nse.system<nsinput_system>()->set_input_map(im->full_id());
	nse.system<nsinput_system>()->push_context("ship_controls");
}

