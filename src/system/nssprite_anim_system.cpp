#include <system/nssprite_anim_system.h>
#include <nsengine.h>
#include <asset/nsscene.h>
#include <component/nssprite_comp.h>
#include <asset/nsmaterial.h>
#include <component/nsrender_comp.h>
#include <nstimer.h>
#include <asset/nstexture.h>

nssprite_anim_system::nssprite_anim_system():
	nssystem(type_to_hash(nssprite_anim_system))
{
		
}
	
nssprite_anim_system::~nssprite_anim_system()
{
		
}

void nssprite_anim_system::init()
{
		
}

void nssprite_anim_system::release()
{}

void nssprite_anim_system::update()
{
	if (scene_error_check())
		return;
	
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
			sprite_animation * cur_anim = scc->animations[scc->current_anim];
			int index = int(scc->elapsed / cur_anim->duration * 16.0f);
			if (index != cur_anim->current_frame)
			{
				fvec4 tex_rect = cur_anim->frames[index].tc;
				get_asset<nsmaterial>(rc->material_id(0))->set_map_tex_coord_rect(
					nsmaterial::diffuse,
					tex_rect);
				get_asset<nsmaterial>(rc->material_id(0))->set_map_tex_id(nsmaterial::diffuse, cur_anim->tex->full_id());
				if (cur_anim->frames[index].emit_signal)
				{
					cur_anim->next_frame(
						anim_frame_sound_info((*scn_iter),scc->animations[scc->current_anim], index));
				}
				cur_anim->current_frame = index;
			}
			scc->elapsed += nse.timer()->fixed();
			if (scc->elapsed >= cur_anim->duration)
			{
				scc->elapsed = 0.0f;
				if (!scc->loop)
					scc->playing = false;
			}
		}

		++scn_iter;
	}
		
}

int32 nssprite_anim_system::update_priority()
{
	return SPRITE_ANIM_SYS_UPDATE;
}
