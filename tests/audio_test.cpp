// core
#include <nstimer.h>
#include <nsengine.h>

#include <opengl/nsgl_driver.h>
#include <opengl/nsgl_window.h>

#include <nscube_grid.h>

// asset managers
#include <asset/nsplugin_manager.h>

// assets
#include <asset/nsscene.h>
#include <asset/nsinput_map.h>
#include <asset/nsplugin.h>
#include <asset/nstexture.h>
#include <asset/nsmaterial.h>
#include <asset/nsaudio_clip.h>

// systems
#include <system/nsaudio_system.h>
#include <system/nsselection_system.h>
#include <system/nsui_system.h>
#include <system/nsinput_system.h>
#include <system/nscamera_system.h>
#include <system/nstform_system.h>
#include <system/nsphysics_system.h>

// components
#include <component/nsaudio_source_comp.h>
#include <component/nscam_comp.h>
#include <component/nssel_comp.h>
#include <component/nsrender_comp.h>
#include <component/nssprite_comp.h>
#include <component/nsphysic_comp.h>

#define UNITS_PER_METER 40

class main_character_system : public nssystem
{
  public:
	
	main_character_system():
		nssystem(type_to_hash(main_character_system))
	{
		
	}
	
	~main_character_system()
	{
		
	}

	void init()
	{
		register_action_handler(main_character_system::handle_jump, "zombic_jump");
		register_action_handler(main_character_system::handle_walk_right, "zombic_right");
		register_action_handler(main_character_system::handle_walk_left, "zombic_left");
	}

	void setup_input_map(nsinput_map * im, const nsstring & gctxt)
	{
		
		nsinput_map::trigger zjump("zombic_jump", nsinput_map::t_pressed);
		zjump.add_key_mod(nsinput_map::key_any);
		im->add_key_trigger(gctxt, nsinput_map::key_space, zjump);

		im->remove_key_triggers(gctxt, NSCAM_LEFT);
		im->remove_key_triggers(gctxt, NSCAM_RIGHT);
		
        nsinput_map::trigger zright("zombic_right", nsinput_map::t_both);
        zright.add_key_mod(nsinput_map::key_any);
        im->add_key_trigger(gctxt, nsinput_map::key_d, zright);

		nsinput_map::trigger zleft("zombic_left", nsinput_map::t_both);
        zleft.add_key_mod(nsinput_map::key_any);
        im->add_key_trigger(gctxt, nsinput_map::key_a, zleft);
	}

	void release()
	{
		
	}

	void update()
	{
		nstform_comp * tc = zombic->get<nstform_comp>();
		instance_tform & itf = tc->per_scene_info(m_active_scene)->m_tforms[0];
		nssprite_sheet_comp * sc = zombic->get<nssprite_sheet_comp>();
		if (walk_right)
		{
			itf.phys.velocity.x = 50.0f;
			sc->current_anim = 1;
		}
		else if (walk_left)
		{
			itf.phys.velocity.x = -50.0f;
			sc->current_anim = 2;
		}
		else
		{
			itf.phys.velocity.x = 0.0f;
			sc->current_anim = 0;
		}

        nssel_comp * selc = zombic->get<nssel_comp>();
		if (!selc->selected(m_active_scene))
		{
			nsentity * cam = get_entity(uivec2(m_active_scene->plugin_id(),hash_id("zombie_editor_cam")));
			nstform_comp * tcc = cam->get<nstform_comp>();
		
			nscam_comp * cc = cam->get<nscam_comp>();
			tform_per_scene_info * psi = tcc->per_scene_info(m_active_scene);
			cc->post_update(true);
			tcc->post_update(true);
			fvec3 wpos = psi->m_tforms[0].world_position();
			fvec3 wpos2 = itf.world_position();
			wpos2.y = wpos.y;
			wpos2.z = wpos.z;
			psi->m_tforms[0].set_world_position(wpos2);
		}
	}

	int32 update_priority()
	{
		return 45000;
	}

	bool handle_jump(nsaction_event * jump_event)
	{
		nstform_comp * tc = zombic->get<nstform_comp>();
		tc->per_scene_info(m_active_scene)->m_tforms[0].phys.accels.emplace_back(
			fvec3(0.0f, 5*GRAVITY_ACCEL*UNITS_PER_METER, 0.0f), 0.1f);
		
		nse.event_dispatch()->push<audio_play_event>(uivec3(zombic->full_id(),0),0);
		return true;
	}

	bool handle_walk_right(nsaction_event * jump_event)
	{
		walk_right = (jump_event->cur_state);
		return true;
	}

	bool handle_walk_left(nsaction_event * jump_event)
	{
		walk_left = (jump_event->cur_state);
		return true;
	}


	bool walk_right;
	bool walk_left;
	
	nsentity * zombic;
	float movement_speed;
	fvec2 movment_vector;
};


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
	sc->set_default_sel_color(fvec4(1.0f,1.0f,0.0f,0.4f));

	nsrender_comp * rc = zombie->create<nsrender_comp>();
	rc->set_mesh_id(pln->full_id());
	rc->set_material(0, zmat->full_id());

	nsphysic_comp * pc = zombie->create<nsphysic_comp>();
	pln->calc_aabb();
	pc->obb = fbox(pln->aabb().mMin,pln->aabb().mMax);
	pc->dynamic =true;

    nsaudio_clip * ac = plg->load<nsaudio_clip>("./import/jump.wav", false);
	ac->read_data();

	nsaudio_source_comp * asc = zombie->create<nsaudio_source_comp>();
	source_obj * src = new source_obj;
    src->buffer = ac;
	src->init();
	asc->sources.push_back(src);

	nssprite_sheet_comp * ssc = zombie->create<nssprite_sheet_comp>();
	ssc->animations.resize(3);

	for (uint8 i = 0; i < 3; ++i)
		ssc->animations[i] = new sprite_animation();
	
	ssc->animations[0]->name = "idle";
	ssc->animations[1]->name = "walk_left";
	ssc->animations[2]->name = "walk_right";
    ssc->current_anim = 0;
	ivec4 padding(1,1,1,2);
	for (int32 i = 0; i < 3; ++i)
	{
		int inc = tex[i]->size().w / 16;
		ssc->animations[i]->tex = tex[i];
		ssc->animations[i]->hashed_name = hash_id(ssc->animations[i]->name);
		ssc->animations[i]->frames.resize(16);
		ssc->animations[i]->duration = 0.3f;
		for (int32 j = 0; j < 16; ++j)
		{
			
			ssc->animations[i]->frames[j].tc.x = float(j*inc + padding.x) / float(tex[i]->size().w);
			ssc->animations[i]->frames[j].tc.y = float(padding.y) / float(tex[i]->size().h);
			ssc->animations[i]->frames[j].tc.z = float((j+1)*inc - padding.z) / float(tex[i]->size().w);
			ssc->animations[i]->frames[j].tc.w = float(tex[i]->size().h - padding.w) / float(tex[i]->size().h);
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
	e.register_system<main_character_system>("main_character_system");
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
	nse.system<main_character_system>()->setup_input_map(im, gctxt);

    nse.system<nsinput_system>()->set_input_map(im->full_id());
    nse.system<nsinput_system>()->push_context(gctxt);

    nsscene * new_scene = plg->create<nsscene>("new_scene");
    new_scene->enable(true);
    new_scene->set_bg_color(fvec3(0.7,0.7,0.7));

    nsentity * dir_light = plg->create_dir_light("zombie_dir_light", 0.0f, 1.0f);
    vp->camera = plg->create_camera("zombie_editor_cam",
                                    fvec2(-500*resf,500*resf),
                                    fvec2(500,-500),
                                    fvec2(-1,-500));
	nse.system<nsaudio_system>()->listener = uivec3(vp->camera->full_id(),0);

	new_scene->add(dir_light, nullptr, false, fvec3(20,20,-20), orientation(fvec4(1,0,0,30.0f)));
	new_scene->add(vp->camera);

	nsentity * zombie = import_sprite_from_file(plg);
	nse.system<main_character_system>()->zombic = zombie;

	nsmaterial * mat1 = plg->create<nsmaterial>("later1");
	nsmaterial * mat2 = plg->create<nsmaterial>("later2");
	nstex2d * tl1 = plg->load<nstex2d>("./import/layer-1.png", false);
	nstex2d * tl2 = plg->load<nstex2d>("./import/layer-2.png", false);

	tex_map_info ti;
	ti.tex_id = tl1->full_id();
	mat1->add_tex_map(nsmaterial::diffuse, ti, true);
	ti.tex_id = tl2->full_id();
	mat2->add_tex_map(nsmaterial::diffuse, ti, true);

	nsmesh_plane * bgmsh = plg->create<nsmesh_plane>("bgmesh");
	bgmsh->calc_aabb();
	
	nsentity * layer1 = plg->create<nsentity>("layer1");
	nsentity * layer2 = plg->create<nsentity>("layer2");
	nsentity * platform_1 = plg->create<nsentity>("platform_1");
	nsentity * platform_2 = plg->create<nsentity>("platform_2");
	

	nsrender_comp * rc = layer1->create<nsrender_comp>();
	rc->set_material(0, mat1->full_id());
	rc->set_mesh_id(bgmsh->full_id());

	rc = layer2->create<nsrender_comp>();
	rc->set_material(0, mat2->full_id());
	rc->set_mesh_id(bgmsh->full_id());	

	nsmaterial * platform_mat = plg->create<nsmaterial>("platform_mat");
	platform_mat->set_color(fvec4(0.0f,0.6f,0.3f,1.0f));
	platform_mat->set_color_mode(true);
	
	rc = platform_1->create<nsrender_comp>();
	rc->set_material(0, platform_mat->full_id());
	rc->set_mesh_id(bgmsh->full_id());

	nsphysic_comp * pcc = platform_1->create<nsphysic_comp>();
	pcc->obb = fbox(bgmsh->aabb().mMin,bgmsh->aabb().mMax);

	nse.system<nsphysics_system>()->gravity *= UNITS_PER_METER;

	
	nsmaterial * platform_mat2 = plg->create<nsmaterial>("platform_mat2");
	platform_mat2->set_color(fvec4(1.0f,0.6f,0.3f,1.0f));
	platform_mat2->set_color_mode(true);
	
	rc = platform_2->create<nsrender_comp>();
	rc->set_material(0, platform_mat2->full_id());
	rc->set_mesh_id(bgmsh->full_id());

	nsphysic_comp * pcc2 = platform_2->create<nsphysic_comp>();
	pcc2->obb = fbox(bgmsh->aabb().mMin,bgmsh->aabb().mMax);

	nse.system<nsaudio_system>()->units_per_meter = UNITS_PER_METER;

	new_scene->add(layer1,nullptr,false,fvec3(0,0,0),
		orientation(fvec4(0,0,1,180)),fvec3(3072/10,1536/15,1));
	
	new_scene->add(layer2,nullptr,false,fvec3(0,0,-2.0),
		orientation(fvec4(0,0,1,180)),fvec3(3072/10,1536/15,1));
	
	new_scene->add(
		platform_1,nullptr,false,fvec3(20.0f,50.0f,-5.0f),fquat(),fvec3(100.0f,20.0f,1.0f));

	new_scene->add(
		platform_2,nullptr,false,fvec3(200.0f,30.0f,-6.0f),fquat(),fvec3(30.0f,10.0f,1.0f));

	new_scene->add(
		zombie,
		nullptr,
		false,
		fvec3(0,0,-7.0f),
		orientation(fvec4(0,0,1,180)),
		fvec3(UNITS_PER_METER/4.0f,UNITS_PER_METER/4.0f*resf,1.0f));

    nse.set_active_scene(new_scene);

	e.start();
    while (e.running())
	{				
        nse.update();
        wind.vid_context()->render(nse.active_scene());
        wind.update();
		window_poll_input();

        if (!wind.is_open())
            e.stop();
    }

    nse.release();	
}
