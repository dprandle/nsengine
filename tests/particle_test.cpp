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

int main()
{
    glfw_setup(ivec2(600,600), false, "Build And Battle 1.0.0");
	
    uint32 i = nse.create_context();
    nse.start();

    nsplugin * plg = nse.load_plugin("testplug.bbp");
	plg->bind();
	nse.set_active(plg);

	nsinput_map * imap = plg->get<nsinput_map>("bb_toolkit");
	nse.set_current_scene("mainscene", false, false);
	nse.system<nsrender_system>()->set_fog_factor(uivec2(60,110));
	nse.system<nsrender_system>()->set_fog_color(fvec4(nse.current_scene()->bg_color(),1.0f));
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
