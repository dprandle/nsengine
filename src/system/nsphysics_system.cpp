#include <system/nsphysics_system.h>
#include <nsengine.h>
#include <asset/nsmesh.h>
#include <asset/nsmaterial.h>
#include <component/nsrender_comp.h>
#include <asset/nsplugin.h>
#include <asset/nsentity_manager.h>
#include <asset/nsentity.h>
#include <asset/nsscene.h>
#include <component/nssel_comp.h>
#include <nscube_grid.h>
#include <component/nsphysic_comp.h>
#include <nstimer.h>

nsphysics_system::nsphysics_system():
	nssystem(type_to_hash(nsphysics_system)),
	gravity(0.0f, -GRAVITY_ACCEL, 0.0f),
	cam_grid_cube(300.0f,300.0f,300.0f),
	draw_grid(false)
{}
	
nsphysics_system::~nsphysics_system()
{
		
}

void nsphysics_system::init()
{
	nsplugin * cplg = nse.core();

	nsmesh_plane * mp = cplg->create<nsmesh_plane>("mesh_plane");
		
	bb_shower = cplg->create<nsentity>("bbshower");
	nsmaterial * red_mat = cplg->create<nsmaterial>("red_mat");
	red_mat->set_color_mode(true);
	red_mat->set_color(fvec4(1.0f,0.0f,0.0f,1.0f));
	nsrender_comp * rc = bb_shower->create<nsrender_comp>();
	rc->set_mesh_id(mp->full_id());
	rc->set_material(0, red_mat->full_id());

	register_action_handler(nsphysics_system::handle_toggle_draw_grid, "toggle_draw_grid");
}

void nsphysics_system::setup_input_map(nsinput_map * im, const nsstring & gctxt)
{
	nsinput_map::trigger toggle_draw_grid(
        "toggle_draw_grid",
        nsinput_map::t_pressed);
    toggle_draw_grid.add_mouse_mod(nsinput_map::any_button);
    im->add_key_trigger(gctxt, nsinput_map::key_c, toggle_draw_grid);

}

bool nsphysics_system::handle_toggle_draw_grid(nsaction_event * evnt)
{
	m_active_scene->remove(bb_shower, false);
	draw_grid = !draw_grid;
	return true;
}

void nsphysics_system::release()
{
	
}

void nsphysics_system::draw_tile_grid()
{
	if (!draw_grid)
		return;
	
	ibox wb = m_active_scene->cube_grid->grid_bounds();

	nsentity * cam = nse.video_driver()->current_context()->focused_vp->camera;
	if (cam != nullptr)
	{
		nstform_comp * tc = cam->get<nstform_comp>();
		if (tc != nullptr)
		{
			tform_per_scene_info * psi = tc->per_scene_info(m_active_scene);
			if (psi != nullptr)
			{
				fquat ornt = psi->m_tforms[0].world_orientation();
				fvec3 cwpos = psi->m_tforms[0].world_position();

				fvec3 far_pos = cwpos + ornt.target() * cam_grid_cube.z/2.0f;
				far_pos += ornt.right() * cam_grid_cube.x / 2.0f;
				far_pos += ornt.up() * cam_grid_cube.y / 2.0f;
				
				cwpos += ornt.up() * -cam_grid_cube.y / 2.0f;
				cwpos += ornt.right() * -cam_grid_cube.x / 2.0f;
				
				fbox bb(cwpos, far_pos);
				ibox cam_bb = nscube_grid::grid_from(bb);
				wb.min.maximize(cam_bb.min);
				wb.max.minimize(cam_bb.max);
			}
		}
	}
	m_active_scene->remove(bb_shower, false);
	for (int z = wb.min.z; z < wb.max.z; ++z)
	{
		for (int y = wb.min.y; y < wb.max.y; ++y)
		{
			for (int x = wb.min.x; x < wb.max.x; ++x)
			{
				auto items = m_active_scene->cube_grid->items_at(ivec3(x,y,z));
				if (items != nullptr && !items->empty())
				{
					m_active_scene->add(
						bb_shower,
						nullptr,
						false,
						nscube_grid::world_from(ivec3(x,y,-1)),
						fquat(),
						fvec3(CUBE_X_GRID/2.3f,CUBE_Y_GRID/2.30f,1.0f));
				}
			}
		}
	}		
}

void nsphysics_system::update()
{
	if (scene_error_check())
		return;

	draw_tile_grid();

	// Lets do some physics!
	entity_set * ents = m_active_scene->entities_with_comp<nsphysic_comp>();		
	auto iter = ents->begin();
	while (iter != ents->end())
	{
		nsphysic_comp * pc = (*iter)->get<nsphysic_comp>();
		nstform_comp * tc = (*iter)->get<nstform_comp>();
		nssel_comp *sc = (*iter)->get<nssel_comp>();
		sel_per_scene_info * spi = nullptr;
		if (sc != nullptr)
			spi = sc->scene_info(m_active_scene);
			
		tform_per_scene_info * psi = tc->per_scene_info(m_active_scene);
		for (uint32 i = 0; i < psi->m_tforms.size(); ++i)
		{
			instance_tform & itf = psi->m_tforms[i];				

			if (pc->dynamic)
			{
				m_active_scene->cube_grid->remove(&itf);
				m_active_scene->cube_grid->search_and_remove(uivec3((*iter)->full_id(),i),
															 itf.phys.aabb);
				itf.in_cube_grid = false;

				fvec3 resultant = gravity;
				if (!pc->gravity || (spi != nullptr && spi->m_selected))
					resultant = fvec3();

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

				itf.phys.aabb = transform_obb_to_aabb(pc->obb, itf.world_tf());
				
				fvec3 pos_av = itf.world_position() + 0.5f*itf.phys.velocity * nse.timer()->fixed();
				itf.phys.velocity += resultant * nse.timer()->fixed();
				pos_av += 0.5f * itf.phys.velocity * nse.timer()->fixed();
				itf.set_world_position(pos_av);
			}

			if (!itf.in_cube_grid)
			{
				itf.recursive_compute();
				itf.phys.aabb = transform_obb_to_aabb(pc->obb, itf.world_tf());
				m_active_scene->cube_grid->insert(&itf);
				itf.in_cube_grid = true;
			}

		}			
		++iter;
	}

	// finally go through collisions!
	check_and_resolve_collisions();
}

void nsphysics_system::add_col_items_to_vec(std::vector<col_stuff> & vec, uivec3_vector * item_ptr, const fvec3 & norm)
{
	if (item_ptr != nullptr)
	{
		auto iter = item_ptr->begin();
		while (iter != item_ptr->end())
		{
			col_stuff cs;
			cs.space_ind = *iter;
			cs.norm = norm;
			vec.push_back(cs);
			++iter;
		}
	}
}

fbox nsphysics_system::calc_final_velocities(const fvec3 & init_vel_a,
											  const fvec3 & init_vel_b,
											  const fvec3 & normal,
											  const fvec2 & masses,
											  float restitution)
{
	fvec3 pln_a = project_plane(init_vel_a, normal);
	fvec3 pln_b = project_plane(init_vel_b, normal);

	fvec3 norm_a = project_plane(init_vel_a, pln_a);
	fvec3 norm_b = project_plane(init_vel_b, pln_b);

	float mag_v1a = norm_a.length();
	float mag_v1b = norm_b.length();

	float mag_v2b = (mag_v1a * (masses.x + restitution) + mag_v1b * (masses.y - restitution)) / (masses.y + 1);
	float mag_v2a = masses.x * mag_v1a + masses.y * (mag_v1b - mag_v2b);

	norm_a.normalize();
	norm_a *= mag_v2a;
	norm_b.normalize();
	norm_b *= mag_v2b;
	return fbox(pln_a + norm_a, pln_b + norm_b);
}

void nsphysics_system::check_and_resolve_collisions()
{
	nscube_grid * cg = m_active_scene->cube_grid; // shortcut
		
	entity_set * ents = m_active_scene->entities_with_comp<nsphysic_comp>();		
	auto iter = ents->begin();
	while (iter != ents->end())
	{
		nsphysic_comp * pc = (*iter)->get<nsphysic_comp>();

		if (!pc->dynamic) // no using checking collisions of static stuff
		{
			++iter;
			continue;
		}
			
		nstform_comp * tc = (*iter)->get<nstform_comp>();
		
		tform_per_scene_info * psi = tc->per_scene_info(m_active_scene);
		for (uint32 i = 0; i < psi->m_tforms.size(); ++i)
		{
			instance_tform & itf = psi->m_tforms[i];

			// lets get our bounding box in terms of grid spaces
			ibox bb_grid = nscube_grid::grid_from(itf.phys.aabb);

			// now put together a list of items in the cells surrounding me
			std::vector<col_stuff> items;
			items.reserve(128);

			// go through top and bottom row
			for (int x = bb_grid.min.x - 1; x < bb_grid.max.x + 1; ++x)
			{
				uivec3_vector * iptr = cg->items_at( ivec3(x, bb_grid.min.y-1, bb_grid.min.z));
				add_col_items_to_vec(items, iptr, fvec3(0,0.4f,0));
				iptr = cg->items_at(ivec3(x, bb_grid.max.y,bb_grid.min.z));
				add_col_items_to_vec(items, iptr, fvec3(0,-0.4f,0));
			}

			// go left and right
			for (int y = bb_grid.min.y; y < bb_grid.max.y + 1; ++y)
			{
				uivec3_vector * iptr = cg->items_at( ivec3(bb_grid.min.x - 1, y, bb_grid.min.z));
				add_col_items_to_vec(items, iptr, fvec3(1,0,0)*3.0f);
				iptr = cg->items_at( ivec3(bb_grid.max.x, y, bb_grid.min.z));
				add_col_items_to_vec(items, iptr, fvec3(-1,0,0)*3.0f);
			}

			// Now we have our list of possible things we could collide with - check it
			auto item_iter = items.begin();
			while (item_iter != items.end())
			{
				nsentity * ent_col = get_entity(item_iter->space_ind.xy());
				nsphysic_comp * phy_col = ent_col->get<nsphysic_comp>();

				
				float bounciness = phy_col->bounciness + pc->bounciness;
				clampf(bounciness, 0.0f, 1.0f);

				tform_per_scene_info * psi_col =
					ent_col->get<nstform_comp>()->per_scene_info(m_active_scene);

				instance_tform & tfi_col = psi_col->m_tforms[item_iter->space_ind.z];

				bool collision = false;

				if (pc->col_type == col_cube)
				{
					if (phy_col->col_type == col_cube)
					{
						if (collision_aabb_aabb(itf.phys.aabb, tfi_col.phys.aabb, 0x04))
						{
							collision = true;
							itf.translate(item_iter->norm*COLLISION_RECOIL);
						}
										
					}
					else if (phy_col->col_type == col_sphere)
					{
						float col_dept;
						if (collision_aabb_sphere(itf.phys.aabb, phy_col->sphere + fvec4(tfi_col.world_position(),0.0f), item_iter->norm, col_dept))
						{
							collision = true;
							itf.translate(item_iter->norm*COLLISION_RECOIL);
						}
					}
				}
				else if (pc->col_type == col_sphere)
				{
					if (phy_col->col_type == col_cube)
					{
						float col_dept;
						if (collision_aabb_sphere(tfi_col.phys.aabb, pc->sphere + fvec4(itf.world_position(), 0.0f), item_iter->norm, col_dept))
						{
							itf.translate(item_iter->norm*COLLISION_RECOIL);
							collision = true;
						}
					}
					else if (phy_col->col_type == col_sphere)
					{
						if (collision_sphere_sphere(pc->sphere + fvec4(itf.world_position(),0.0f), phy_col->sphere + fvec4(tfi_col.world_position(),0.0f)))
						{
							collision = true;
							itf.translate(item_iter->norm*COLLISION_RECOIL);
						}
					}
				}

				if (collision)
				{
					fbox final_vels = calc_final_velocities(
						itf.phys.velocity,
						tfi_col.phys.velocity,
						item_iter->norm,
						fvec2(pc->mass, phy_col->mass),
						bounciness);
					itf.phys.velocity = final_vels.a;
					nse.event_dispatch()->push<collision_event>(
						uivec3((*iter)->full_id(),i), item_iter->space_ind);						
				}
					
				++item_iter;
			}

				
		}			
		++iter;
	}		
}

int32 nsphysics_system::update_priority()
{
	return PHYSIC_2D_SYS_UPDATE_PR;
}
