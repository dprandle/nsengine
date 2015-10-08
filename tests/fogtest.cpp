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
    glfw_setup(ivec2(600,600), false, "Build And Battle 1.0.0");
	
    uint32 i = nse.create_context();
    nse.start();

    nsplugin * plg = setup_basic_plugin();

	nsinput_map * imap = plg->get<nsinput_map>("basic_input");

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
	nse.system<nsselection_system>()->set_mirror_tile_color(fvec4(1.0f,1.0f,0.0f,0.7f));
	
	nse.system<nsrender_system>()->set_fog_factor(uivec2(10,100));
	nse.system<nsrender_system>()->set_fog_color(fvec4(nse.current_scene()->bg_color(),1.0f));

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
	nse.set_current_scene(scn, true);
	
    nsentity * cam = plg->create_camera("scenecam", 60.0f, uivec2(600, 600), fvec2(DEFAULT_Z_NEAR, DEFAULT_Z_FAR));
	nsentity * dirl = plg->create_dir_light("dirlight", 0.8f, 0.6f);
	scn->set_camera(cam);
	scn->add(dirl, fvec3(20.0f, 20.0f, -20.0f));
    nsentity * tile = plg->create_tile("grasstile", nse.import_dir() + "diffuseGrass.png", nse.import_dir() + "normalGrass.png", fvec3(1.0, 1.0, 0.0), 16.0f, 0.5f, fvec3(1.0f), true);
    scn->add_gridded(tile, ivec3(32, 32, 2), fvec3(0.0f,0.0f,0.0f));

	nsinput_map * imap = plg->load<nsinput_map>("bb_toolkit.nsi");
	nse.system<nsinput_system>()->set_input_map(imap->full_id());
	nse.system<nsinput_system>()->push_context("Main");	
	return plg;
}
