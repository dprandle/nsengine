// core
#include <nstimer.h>
#include <nsengine.h>
#include <nsgl_driver.h>
#include <nsgl_window.h>

// asset managers
#include <nsplugin_manager.h>

// assets
#include <nsscene.h>
#include <nsinput_map.h>
#include <nsplugin.h>
#include <nstexture.h>
#include <nsmaterial.h>

// systems
#include <nsselection_system.h>
#include <nsui_system.h>
#include <nsinput_system.h>
#include <nscamera_system.h>
#include <nsrender_system.h>

// components
#include <nscam_comp.h>
#include <nssel_comp.h>
#include <nsrender_comp.h>
#include <nssprite_comp.h>

nsentity * import_sprite_from_file(nsplugin * plg)
{
	nstex2d * tex[3] = {};
	
	nsmesh_plane * pln = plg->create<nsmesh_plane>("main_zombie");
	tex[0] = plg->load<nstex2d>("./import/idle.png",false);
    tex[1] = plg->load<nstex2d>("./import/left_walk.png",false);
    tex[2] = plg->load<nstex2d>("./import/right_walk.png",false);
	nsentity * zombie = plg->create<nsentity>("main_zombie");
	nsmaterial * zmat = plg->create<nsmaterial>("zombie_mat");

	nssel_comp * sc = zombie->create<nssel_comp>();
	sc->set_default_sel_color(fvec4(1.0f,1.0f,0.0f,1.0f));

	nsrender_comp * rc = zombie->create<nsrender_comp>();
	rc->set_mesh_id(pln->full_id());
	rc->set_material(0, zmat->full_id());

	nssprite_sheet_comp * ssc = zombie->create<nssprite_sheet_comp>();
	ssc->animations.resize(3);

	ssc->animations[0].name = "idle";
	ssc->animations[1].name = "walk_left";
	ssc->animations[2].name = "walk_right";
    ssc->current_anim = 0;
	for (int32 i = 0; i < 3; ++i)
	{
		int inc = tex[i]->size().w / 16;
		ssc->animations[i].tex = tex[i];
		ssc->animations[i].hashed_name = hash_id(ssc->animations[i].name);
		ssc->animations[i].frames.resize(16);
		ssc->animations[i].duration = 0.5f;
		for (int32 j = 0; j < 16; ++j)
		{
			ssc->animations[i].frames[j].x = float(j*inc) / float(tex[i]->size().w);
			ssc->animations[i].frames[j].z = float((j+1)*inc) / float(tex[i]->size().w);
			ssc->animations[i].frames[j].w = 1.0f;
		}
	}

	tex_map_info ti;
	ti.tex_id = tex[0]->full_id();
	ti.coord_rect = fvec4(0.0f,0.0f,float(tex[0]->size().w / 16) / float(tex[0]->size().w), 1.0f);
	zmat->add_tex_map(nsmaterial::diffuse, ti, true);

	ssc->loop = true;
	ssc->playing = true;	
	return zombie;
}

int main()
{
    nsengine e;
    e.init(new nsgl_driver);

    ivec2 res(1920.0f*0.8f,1080.0f*0.8f);
    nsgl_window wind(res, "Basic Test");
    viewport * vp = wind.vid_context()->insert_viewport("main_view",fvec4(0.0f,0.0f,1.0f,1.0f));

	float resf = float(res.x) / float(res.y);
	
	// Main pluging
    nsplugin * plg = nsep.create<nsplugin>("most_basic_test");
    plg->enable(true);

	// Setup input map
	nsinput_map * im = plg->create<nsinput_map>("basic_input");
	nsstring gctxt("main_globale_ctxt");
    im->create_context(gctxt);
	nse.system<nstform_system>()->setup_input_map(im, gctxt);
    nse.system<nscamera_system>()->setup_input_map(im, gctxt);
    nse.system<nsselection_system>()->setup_input_map(im, gctxt);
    nse.system<nsui_system>()->setup_input_map(im, gctxt);
    nse.system<nsinput_system>()->set_input_map(im->full_id());
    nse.system<nsinput_system>()->push_context(gctxt);

    nsscene * new_scene = plg->create<nsscene>("new_scene");
    new_scene->enable(true);
    new_scene->set_bg_color(fvec3(0.7,0.7,0.7));

    nsentity * dir_light = plg->create_dir_light("zombie_dir_light", 0.0f, 1.0f);
    vp->camera = plg->create_camera("zombie_editor_cam",
                                    fvec2(-100*resf,100*resf),
                                    fvec2(100,-100),
                                    fvec2(-1,-1000));

	new_scene->add(dir_light, nullptr, false, fvec3(20,20,-20), orientation(fvec4(1,0,0,30.0f)));
	new_scene->add(vp->camera);

	nsentity * grass_tile = plg->create_tile("grass_tile",
                                             nse.import_dir() + "diffuseGrass.png",
                                             nse.import_dir() + "normalGrass.png",
                                             fvec4(1,0,0,0.5f), 16.0, 0.6, fvec3(1,1,1), true);
    new_scene->add_gridded(grass_tile,ivec3(8,8,1), fvec3(10,10,0.0f));

	nsentity * zombie = import_sprite_from_file(plg);
	new_scene->add(zombie,nullptr,false,fvec3(0,0,0),orientation(fvec4(0,0,1,90)),fvec3(1.0f,resf,1.0f));

	nsrender_comp * rc = zombie->get<nsrender_comp>();
	nssprite_sheet_comp * scc = zombie->get<nssprite_sheet_comp>();


    nse.set_active_scene(new_scene);

	e.start();
	int last_index = 0;
    while (e.running())
	{
		
		if (scc->playing)
		{
			int index = int(scc->elapsed / scc->animations[scc->current_anim].duration * 16.0f);
			if (index != last_index)
			{
				fvec4 tex_rect = scc->animations[scc->current_anim].frames[index];
				std::cout << "tex_rect " << tex_rect.to_string() << std::endl;
				get_asset<nsmaterial>(rc->material_id(0))->set_map_tex_coord_rect(
					nsmaterial::diffuse,
					tex_rect);
				get_asset<nsmaterial>(rc->material_id(0))->set_map_tex_id(nsmaterial::diffuse, scc->animations[scc->current_anim].tex->full_id());
				last_index = index;
			}
			scc->elapsed += nse.timer()->dt();
			if (scc->elapsed >= scc->animations[scc->current_anim].duration)
			{
				scc->elapsed = 0.0f;
				if (!scc->loop)
					scc->playing = false;
			}
		}
				
        nse.update();
        wind.vid_context()->render(nse.active_scene());
        wind.update();
		window_poll_input();

        if (!wind.is_open())
            e.stop();
    }

    nse.release();

	
}
