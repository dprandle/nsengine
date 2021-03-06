/*!
\file nsparticle_system.h

\brief Definition file for nsparticle_system class

This file contains all of the neccessary definitions for the nsparticle_system class.

\author Daniel Randle
\date March 8 2014
\copywrite Earth Banana Games 2013
*/

#include <opengl/nsgl_driver.h>
#include <opengl/nsgl_vao.h>
#include <nsvideo_driver.h>
#include <nsevent_dispatcher.h>
#include <system/nsparticle_system.h>
#include <component/nsparticle_comp.h>
#include <nsengine.h>
#include <opengl/nsgl_framebuffer.h>
#include <system/nstform_system.h>
#include <asset/nsshader.h>
#include <asset/nsshader_manager.h>
#include <nsentity.h>
#include <nstimer.h>
#include <asset/nsmat_manager.h>
#include <asset/nstexture.h>
#include <asset/nsplugin.h>
#include <asset/nstex_manager.h>
#include <nsworld_data.h>
#include <opengl/nsgl_xfb.h>
#include <component/nscam_comp.h>
#include <asset/nsplugin_manager.h>
#include <nstform_ent_chunk.h>

nsparticle_system::nsparticle_system() :
	nssystem(type_to_hash(nsparticle_system))
{}

nsparticle_system::~nsparticle_system()
{}

void nsparticle_system::init()
{
	
}

void nsparticle_system::release()
{
	
}

int32 nsparticle_system::update_priority()
{
	return PARTICLE_SYS_UPDATE_PR;
}

void nsparticle_system::update()
{
    return;
	if (chunk_error_check())
		return;
	
	viewport * vp = nse.video_driver()->current_context()->focused_vp;
	if (vp == nullptr)
		return;
	
	nsentity * cam = vp->camera;
	if (cam == NULL)
		return;

	nscam_comp * compc = cam->get<nscam_comp>();
	nstform_comp * camTComp = cam->get<nstform_comp>();

	auto ents = m_active_chunk->entities_with_comp<nsparticle_comp>();
	if (ents == nullptr)
		return;
	
	auto entIter = ents->begin();
	while (entIter != ents->end())
	{
		nsparticle_comp * comp = (*entIter)->get<nsparticle_comp>();
		comp->video_update();
		++entIter;
	}
}
