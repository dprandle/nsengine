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
#include <nsmaterial.h>
#include <nstexture.h>
#include <nsshader.h>
#include <nstex_manager.h>

nsplugin * setup_basic_plugin();

int main()
{
    glfw_setup(ivec2(1024,1024), false, "Build And Battle 1.0.0");
	
    uint32 i = nse.create_context();
    nse.start();

    nsplugin * plg = setup_basic_plugin();
	nsinput_map * imap = plg->get<nsinput_map>("basic_input");

	nsentity * skybox = plg->create<nsentity>("skybox");

	nstexture * sky_box = plg->manager<nstex_manager>()->load_cubemap("./import/skydomes/space.cube",".jpg");
	nsmaterial * sb_mat = plg->create<nsmaterial>("skybox");
	sb_mat->set_map_tex_id(nsmaterial::diffuse, sky_box->full_id());
	sb_mat->set_shader_id(nse.core()->get<nsshader>(DEFAULT_SKYBOX_SHADER)->full_id());
	sb_mat->set_cull_mode(GL_FRONT);
	nsrender_comp * rc = skybox->create<nsrender_comp>();
	rc->set_cast_shadow(false);
	rc->set_mesh_id(nse.core()->get<nsmesh>(MESH_SKYDOME)->full_id());
	rc->set_material(0, sb_mat->full_id());
	
	nse.current_scene()->set_skydome(skybox);
	
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
	
    nsentity * cam = plg->create_camera("scenecam", 60.0f, uivec2(400, 400), fvec2(DEFAULT_Z_NEAR, DEFAULT_Z_FAR));
	nsentity * dirl = plg->create_dir_light("dirlight", 0.8f, 0.6f);
	scn->set_camera(cam);
	scn->add(dirl, fvec3(20.0f, 20.0f, -20.0f));
    nsentity * tile = plg->create_tile("grasstile", nse.import_dir() + "diffuseGrass.png", nse.import_dir() + "normalGrass.png", fvec3(1.0, 1.0, 0.0), 16.0f, 0.5f, fvec3(1.0f), true);
    scn->add_gridded(tile, ivec3(32, 32, 2), fvec3(0.0f,0.0f,0.0f));

	nsinput_map * imap = plg->load<nsinput_map>("basic_input.nsi");
	nse.system<nsinput_system>()->set_input_map(imap->full_id());
	nse.system<nsinput_system>()->push_context("Main");	
	return plg;
}
