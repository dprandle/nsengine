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

nsplugin * setup_basic_plugin();

int main()
{
    glfw_setup(ivec2(600,600), false, "Build And Battle 1.0.0");
	
    uint32 i = nse.create_context();
    nse.start();

    nsplugin * plg = nse.load_plugin("testplug.bbp");
	plg->bind();
	nse.set_active(plg);

	// Setup build brush (simple one)
	nsentity * tilebrush = plg->get<nsentity>("simple_tile_brush");
	nsentity * object_to_build = plg->get<nsentity>("tree");
	nsentity * grasstile = plg->get<nsentity>("grasstile");
	nsinput_map * imap = plg->get<nsinput_map>("bb_toolkit");
    nsengine & eng = nse;

	nse.set_current_scene("mainscene", false, false);
    nse.system<nsbuild_system>()->set_tile_brush(tilebrush);
	nse.system<nsbuild_system>()->set_tile_build_ent(grasstile);
	nse.system<nsbuild_system>()->set_object_build_ent(object_to_build);	
	nse.system<nsinput_system>()->set_input_map(imap->full_id());
	nse.system<nsinput_system>()->push_context("Main");

	while (glfw_window_open())
    {
        nse.update();
        glfw_update();
    }

    nse.shutdown();
	glfw_shutdown();
    return 0;
}
