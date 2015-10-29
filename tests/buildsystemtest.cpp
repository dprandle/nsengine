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
#include <nsbuild_system.h>
#include <nsrender_comp.h>
#include <nssel_comp.h>
#include <nstile_brush_comp.h>
#include <nsrender_system.h>
#include <nsmesh.h>

nsplugin * setup_basic_plugin();

int main()
{
    glfw_setup(ivec2(400,400), false, "Build And Battle 1.0.0");
	
    uint32 i = nse.create_context();
    nse.start();

    nsplugin * plg = setup_basic_plugin();

	nsinput_map * imap = plg->get<nsinput_map>("basic_input");

	imap->create_context(BUILD_MODE_CTXT);

	nsinput_map::trigger insert_ent(
		NSINSERT_ENTITY,
		nsinput_map::t_toggle,
		nsinput_map::axis_mouse_xpos | nsinput_map::axis_mouse_ypos
		);
	imap->add_mouse_trigger(BUILD_MODE_CTXT, nsinput_map::left_button, insert_ent);

	nsinput_map::trigger toggle_build(
		NSTOGGLE_BUILD,
		nsinput_map::t_pressed
		);
	imap->add_key_trigger("Main", nsinput_map::key_b, toggle_build);
	
	nsinput_map::trigger toggle_build_erase_mode(
		NSTOGGLE_BUILD_ERASE_MODE,
		nsinput_map::t_pressed
		);
	imap->add_key_trigger("Main", nsinput_map::key_e, toggle_build_erase_mode);

	nsinput_map::trigger toggle_tile_build_mode(
		NSTOGGLE_TILE_BUILD_MODE,
		nsinput_map::t_pressed
		);
	imap->add_key_trigger("Main", nsinput_map::key_t, toggle_tile_build_mode);

	nsinput_map::trigger move_brush_xy(
		NSSEL_MOVE_XY,
		nsinput_map::t_pressed,
		nsinput_map::axis_mouse_xdelta | nsinput_map::axis_mouse_ydelta
		);
    imap->add_mouse_trigger(BUILD_MODE_CTXT, nsinput_map::movement, move_brush_xy);

	nsinput_map::trigger move_brush_click_xy(
		NSSEL_MOVE_XY,
		nsinput_map::t_pressed,
		nsinput_map::axis_mouse_xdelta | nsinput_map::axis_mouse_ydelta
		);
	move_brush_click_xy.add_mouse_mod(nsinput_map::left_button);
    imap->add_mouse_trigger(BUILD_MODE_CTXT, nsinput_map::movement, move_brush_click_xy);

	nsinput_map::trigger move_brush_z(
        NSSEL_MOVE_Z,
		nsinput_map::t_pressed,
		nsinput_map::axis_mouse_xdelta | nsinput_map::axis_mouse_ydelta
		);
	move_brush_z.add_key_mod(nsinput_map::key_z);
    imap->add_mouse_trigger(BUILD_MODE_CTXT, nsinput_map::movement,move_brush_z);

	nsinput_map::trigger initial_snap_brush_z(
        NSINITIAL_SNAP_BRUSH_Z,
		nsinput_map::t_pressed,
		false
		);
    imap->add_key_trigger(BUILD_MODE_CTXT, nsinput_map::key_z,initial_snap_brush_z);

	nsinput_map::trigger snap_brush_z(
        NSSNAP_BRUSH_Z,
		nsinput_map::t_released,
		false
		);
    imap->add_key_trigger(BUILD_MODE_CTXT, nsinput_map::key_z,snap_brush_z);

	nsinput_map::trigger move_brush_click_z(
        NSINSERT_ENTITY,
		nsinput_map::t_toggle,
		nsinput_map::axis_mouse_xpos | nsinput_map::axis_mouse_ypos
		);
	move_brush_click_z.add_key_mod(nsinput_map::key_z);
    imap->add_mouse_trigger(BUILD_MODE_CTXT, nsinput_map::left_button, move_brush_click_z);

	nsinput_map::trigger save_map(
        "save_map",
		nsinput_map::t_pressed
		);
	save_map.add_key_mod(nsinput_map::key_lctrl);
    imap->add_key_trigger("Main", nsinput_map::key_s, save_map);

	// Setup build brush (simple one)
	nsentity * tilebrush = plg->create<nsentity>("simple_tile_brush");
	nstile_brush_comp * tc = tilebrush->create<nstile_brush_comp>();
	nsrender_comp * rc = tilebrush->create<nsrender_comp>();
	nssel_comp * sc = tilebrush->create<nssel_comp>();
	rc->set_mesh_id(nse.core()->get<nsmesh>(MESH_FULL_TILE)->full_id());
	sc->set_default_sel_color(fvec4(0.0f, 1.0f, 0.0f, 0.8f));
	tc->add(0, 0);
	tc->add(0, 1);
	tc->add(1, 1);
	
	nsentity * object_to_build = plg->load_model("tree", "DAEpinetree.dae", true);	
	nsmesh * msh = plg->get<nsmesh>(object_to_build->get<nsrender_comp>()->mesh_id().y);
	msh->bake_node_rotation(orientation(fvec4(1.0,0.0,0.0,-90.0f)));
	nsoccupy_comp * oc = object_to_build->get<nsoccupy_comp>();
	oc->build(msh->aabb());
    nse.system<nsbuild_system>()->set_tile_brush(tilebrush);
	nse.system<nsbuild_system>()->set_tile_build_ent(plg->get<nsentity>("grasstile"));
	nse.system<nsbuild_system>()->set_object_build_ent(object_to_build);
    nse.system<nsbuild_system>()->enable_mirror(true);
    nse.system<nsselection_system>()->enable_mirror_selection(true);

	nsentity * skybox = plg->create_skydome("skydome", "skydomes/sun5deg.cube", ".png", "skydome/");
	nse.current_scene()->set_skydome(skybox);

	imap->rename("bb_toolkit");
    nse.system<nsinput_system>()->set_input_map(imap->full_id());
    nse.system<nsinput_system>()->push_context("Main");
	nse.save_plugins(true);
	
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
	nse.set_current_scene(scn, true, false);
	
    nsentity * cam = plg->create_camera("scenecam", 60.0f, uivec2(400, 400), fvec2(DEFAULT_Z_NEAR, DEFAULT_Z_FAR));
	nsentity * dirl = plg->create_dir_light("dirlight", 0.8f, 0.6f);
	scn->set_camera(cam);
	scn->add(dirl, fvec3(20.0f, 20.0f, -20.0f));
    nsentity * tile = plg->create_tile("grasstile", nse.import_dir() + "diffuseGrass.png", nse.import_dir() + "normalGrass.png", fvec3(1.0, 1.0, 0.0), 16.0f, 0.5f, fvec3(1.0f), true);
    scn->add_gridded(tile, ivec3(32, 32, 2), fvec3(0.0f,0.0f,0.0f));

	nsinput_map * imap = plg->load<nsinput_map>("basic_input.nsi");
	return plg;
}
