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

#define WORLD_MULT 700.0
#define GRAVITY_ACCEL -50.0

class nsphysic_comp : public nscomponent
{
  public:
	
	nsphysic_comp();

	nsphysic_comp(const nsphysic_comp & copy);

	~nsphysic_comp();

	void init();

	void release();

	void finalize();

	void name_change(const uivec2 &, const uivec2);

	uivec3_vector resources();

	void pup(nsfile_pupper * p);

	fbox aabb;
	float mass;
	bool gravity;
};

nsphysic_comp::nsphysic_comp():
	nscomponent(type_to_hash(nsphysic_comp)),
    mass(80.0f),
	gravity(true)
{
		
}

nsphysic_comp::nsphysic_comp(const nsphysic_comp & copy):
	nscomponent(copy.m_hashed_type)
{
		
}

nsphysic_comp::~nsphysic_comp()
{
		
}

void nsphysic_comp::init()
{
}

void nsphysic_comp::release()
{
		
}

void nsphysic_comp::finalize()
{
		
}

void nsphysic_comp::name_change(const uivec2 &, const uivec2)
{
		
}

uivec3_vector nsphysic_comp::resources()
{
	return uivec3_vector();
}

void nsphysic_comp::pup(nsfile_pupper * p)
{
	
}

class nsphysic2d_system : public nssystem
{
  public:
	nsphysic2d_system():
		nssystem(type_to_hash(nsphysic2d_system)),
        gravity(0.0f,GRAVITY_ACCEL * WORLD_MULT,0.0f),
		bottom_plane(0.0f,1.0f,0.0f,-60.0f)
	{
		
	}
	
	~nsphysic2d_system()
	{
		
	}

	void init()
	{
		
	}

	void setup_input_map(nsinput_map * im, const nsstring & gctxt)
	{
	}

	void release()
	{
		
	}

	void update()
	{
		entity_set * ents = m_active_scene->entities_with_comp<nsphysic_comp>();
		auto iter = ents->begin();
		while (iter != ents->end())
		{
			nsphysic_comp * pc = (*iter)->get<nsphysic_comp>();
			nstform_comp * tc = (*iter)->get<nstform_comp>();
			nssel_comp *sc = (*iter)->get<nssel_comp>();
			sel_per_scene_info * spi = sc->scene_info(m_active_scene);
			
			tform_per_scene_info * psi = tc->per_scene_info(m_active_scene);
			for (uint32 i = 0; i < psi->m_tforms.size(); ++i)
			{
				instance_tform & itf = psi->m_tforms[i];

				fvec3 resultant;
				if (pc->gravity && !spi->m_selected)
					resultant = gravity;

				auto accel_iter = itf.phys.accels.begin();			
				while (accel_iter != itf.phys.accels.end())
				{
					resultant += accel_iter->accel;
					accel_iter->elapsed += nse.timer()->fixed();
					if (accel_iter->elapsed >= accel_iter->duration)
						accel_iter = itf.phys.accels.erase(accel_iter);
					else
						++accel_iter;
				}


				fvec3 pos_av = itf.world_position() + 0.5f*itf.phys.velocity * nse.timer()->fixed();
				itf.phys.velocity += (resultant / pc->mass) * nse.timer()->fixed();
				pos_av += 0.5f * itf.phys.velocity * nse.timer()->fixed();

				fbox updated_bx = pc->aabb + pos_av;
				int ret = collision_plane_aabb(bottom_plane, updated_bx);
				if (ret == 3)
				{
					pos_av.y = bottom_plane.w + 1.0f;
					itf.phys.velocity.y = 0.0f;
				}
				
				itf.set_world_position(pos_av);
				
			}
			
			++iter;


			
		}
	}

	int32 update_priority()
	{
		return PHYSIC_2D_SYS_UPDATE_PR;
	}

	fvec3 gravity;
	fvec4 bottom_plane;
};

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
			fvec3(0.0f,200.0f*WORLD_MULT,0.0f), 0.1f);
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

class nssprite_anim_system : public nssystem
{
  public:
	
	nssprite_anim_system():
		nssystem(type_to_hash(nssprite_anim_system))
	{
		
	}
	
	~nssprite_anim_system()
	{
		
	}

	void init()
	{
		
	}

	void release()
	{
		
	}

	void update()
	{
		entity_set * ents = m_active_scene->entities_with_comp<nssprite_sheet_comp>();

		entity_set::iterator scn_iter = ents->begin();
		while (scn_iter != ents->end())
		{
			nssprite_sheet_comp * scc = (*scn_iter)->get<nssprite_sheet_comp>();
			nsrender_comp * rc = (*scn_iter)->get<nsrender_comp>();

			if (rc == nullptr)
			{
				++scn_iter;
				continue;
			}
			
			if (scc->playing)
			{
				int index = int(scc->elapsed / scc->animations[scc->current_anim].duration * 16.0f);
				if (index != scc->animations[scc->current_anim].current_frame)
				{
					fvec4 tex_rect = scc->animations[scc->current_anim].frames[index];
					get_asset<nsmaterial>(rc->material_id(0))->set_map_tex_coord_rect(
						nsmaterial::diffuse,
						tex_rect);
					get_asset<nsmaterial>(rc->material_id(0))->set_map_tex_id(nsmaterial::diffuse, scc->animations[scc->current_anim].tex->full_id());
					scc->animations[scc->current_anim].current_frame = index;
				}
				scc->elapsed += nse.timer()->fixed();
				if (scc->elapsed >= scc->animations[scc->current_anim].duration)
				{
					scc->elapsed = 0.0f;
					if (!scc->loop)
						scc->playing = false;
				}
			}

			++scn_iter;
		}
		
	}

	int32 update_priority()
	{
		return SPRITE_ANIM_SYS_UPDATE;
	}

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
	pc->aabb = fbox(pln->aabb().mMin,pln->aabb().mMax);

	nssprite_sheet_comp * ssc = zombie->create<nssprite_sheet_comp>();
	ssc->animations.resize(3);

	ssc->animations[0].name = "idle";
	ssc->animations[1].name = "walk_left";
	ssc->animations[2].name = "walk_right";
    ssc->current_anim = 0;
	ivec4 padding(1,1,1,2);
	for (int32 i = 0; i < 3; ++i)
	{
		int inc = tex[i]->size().w / 16;
		ssc->animations[i].tex = tex[i];
		ssc->animations[i].hashed_name = hash_id(ssc->animations[i].name);
		ssc->animations[i].frames.resize(16);
		ssc->animations[i].duration = 0.3f;
		for (int32 j = 0; j < 16; ++j)
		{
			
			ssc->animations[i].frames[j].x = float(j*inc + padding.x) / float(tex[i]->size().w);
			ssc->animations[i].frames[j].y = float(padding.y) / float(tex[i]->size().h);
			ssc->animations[i].frames[j].z = float((j+1)*inc - padding.z) / float(tex[i]->size().w);
			ssc->animations[i].frames[j].w = float(tex[i]->size().h - padding.w) / float(tex[i]->size().h);
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

	e.register_system<nssprite_anim_system>("nssprite_anim_system");
	e.register_system<main_character_system>("main_character_system");
	e.register_system<nsphysic2d_system>("nsphysic2d_system");
	e.register_component<nsphysic_comp>("nsphysic_comp");
	
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
                                    fvec2(-100*resf,100*resf),
                                    fvec2(100,-100),
                                    fvec2(-1,-1000));

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
	
	nsentity * layer1 = plg->create<nsentity>("layer1");
	nsentity * layer2 = plg->create<nsentity>("layer2");

	nsrender_comp * rc = layer1->create<nsrender_comp>();
	rc->set_material(0, mat1->full_id());
	rc->set_mesh_id(bgmsh->full_id());

	rc = layer2->create<nsrender_comp>();
	rc->set_material(0, mat2->full_id());
	rc->set_mesh_id(bgmsh->full_id());

	new_scene->add(layer1,nullptr,false,fvec3(0,0,0),
		orientation(fvec4(0,0,1,180)),fvec3(3072/10,1536/15,1));
	
	new_scene->add(layer2,nullptr,false,fvec3(0,0,-2.0),
		orientation(fvec4(0,0,1,180)),fvec3(3072/10,1536/15,1));
	
	
	new_scene->add(
		zombie,
		nullptr,
		false,
		fvec3(0,0,-5),
		orientation(fvec4(0,0,1,180)),
		fvec3(10.0f,10*resf,10.0f));

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
