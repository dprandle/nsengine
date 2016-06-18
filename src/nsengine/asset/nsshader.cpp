/*--------------------------------------------- Noble Steed Engine--------------------------------------*
Authors : Daniel Randle, Alex Bourne
Date Created : Mar 5 2013

File:
	nsshader.cpp

Description:
	This file contains the definition for the nsshader class and any associated functions that help
	the ol' girl out
*-----------------------------------------------------------------------------------------------------*/

#include <nsmath.h>
#include <nsshader.h>
#include <nsmaterial.h>
#include <nstform_comp.h>
#include <nslight_comp.h>
#include <nsengine.h>
#include <nsengine.h>
#include <nsmaterial.h>
#include <exception>
#include <string.h>
#include <nsrender_system.h>
#include <nsgl_driver.h>

nsshader::nsshader():
	nsasset(type_to_hash(nsshader)),
	nsvideo_object(),
	shader_stages(),
	m_vertex(vertex_shader),
	m_geometry(geometry_shader),
	m_fragment(fragment_shader)
{
	video_context_init();
	set_ext(DEFAULT_SHADER_EXTENSION);
}

nsshader::nsshader(const nsshader & copy_):
	nsasset(copy_),
	nsvideo_object(),
	shader_stages(copy_.shader_stages),
	m_vertex(vertex_shader),
	m_geometry(geometry_shader),
	m_fragment(fragment_shader)

{
	video_context_init();
}

nsshader::~nsshader()
{}

nsshader & nsshader::operator=(nsshader rhs)
{
	nsasset::operator=(rhs);
	std::swap(shader_stages, rhs.shader_stages);
	return *this;
}

void nsshader::video_context_init()
{
	video_context_release();
	uint8 context_id = nse.video_driver()->current_context()->context_id;
	ctxt_objs[context_id] = nse.factory<nsvid_obj_factory>(SHADER_VID_OBJ_GUID)->create(this);
}

void nsshader::pup(nsfile_pupper * p)
{
	if (p->type() == nsfile_pupper::pup_binary)
	{
		nsbinary_file_pupper * bf = static_cast<nsbinary_file_pupper *>(p);
		::pup(*bf, *this);
	}
	else
	{		
		nstext_file_pupper * tf = static_cast<nstext_file_pupper *>(p);
		::pup(*tf, *this);
	}
}
