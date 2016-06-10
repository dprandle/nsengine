/*!
  \file nsvideo_driver.h

  \brief Definition file for nsvideo_driver class

  This file contains all of the neccessary definitions for the nsvideo_driver class.

  \author Daniel Randle
  \date Feb 23 2016
  \copywrite Earth Banana Games 2013
*/

#ifndef NSVIDEO_DRIVER_H
#define NSVIDEO_DRIVER_H

#include <nsstring.h>
#include <nsmath.h>
#include <nsunordered_map.h>
#include <nsmesh.h>

namespace nsrender
{
struct viewport;
}

class nsparticle_comp;
struct tform_per_scene_info;
struct sel_per_scene_info;
class nstexture;
class nsshader;

class nsvideo_driver
{
  public:

	nsvideo_driver();
	
	virtual ~nsvideo_driver();

	virtual void setup_default_rendering() = 0;

	virtual uivec3 pick(const fvec2 & mouse_pos) = 0;
	
	virtual void init(bool setup_default_rend);

	bool initialized();

	virtual void release();

	virtual void resize_screen(const ivec2 & new_size) = 0;

	virtual void render(nsrender::viewport * vp) = 0;

	virtual void register_texture(nstexture * tex) = 0;

	virtual void deregister_texture(nstexture * tex) = 0;

	virtual void register_shader(nsshader * shader) = 0;

	virtual void deregister_shader(nsshader * shader) = 0;

	virtual void register_submesh(nsmesh::submesh * submesh) = 0;

	virtual void deregister_submesh(nsmesh::submesh * submesh) = 0;

	virtual void register_tform_per_scene_info(tform_per_scene_info * tf) = 0;

	virtual void deregister_tform_per_scene_info(tform_per_scene_info * tf) = 0;

	virtual void register_sel_per_scene_info(sel_per_scene_info * si) = 0;

	virtual void deregister_sel_per_scene_info(sel_per_scene_info * si) = 0;

	virtual void register_particle_comp(nsparticle_comp * pcomp) = 0;

	virtual void deregister_particle_comp(nsparticle_comp * pcomp) = 0;

  protected:
	
	bool m_initialized;
};

struct nsvid_ctxt_obj
{	
	virtual ~nsvid_ctxt_obj();
};

struct nsvid_obj
{
	nsvid_obj():
		ctxt_objs()
	{}
	
	nsvid_ctxt_obj * ctxt_objs[16];
};

#endif
