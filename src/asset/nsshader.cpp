/*--------------------------------------------- Noble Steed Engine--------------------------------------*
Authors : Daniel Randle, Alex Bourne
Date Created : Mar 5 2013

File:
	nsshader.cpp

Description:
	This file contains the definition for the nsshader class and any associated functions that help
	the ol' girl out
*-----------------------------------------------------------------------------------------------------*/

#include <nsmath/nsmath.h>
#include <asset/nsshader.h>
#include <asset/nsmaterial.h>
#include <component/nstform_comp.h>
#include <component/nslight_comp.h>
#include <nsengine.h>
#include <nsengine.h>
#include <asset/nsmaterial.h>
#include <exception>
#include <string.h>
#include <system/nstform_system.h>
#include <opengl/nsgl_driver.h>

nsshader::nsshader():
	nsasset(type_to_hash(nsshader)),
	nsvideo_object("nsshader"),
	shader_stages()
{
	video_context_init();
	set_ext(DEFAULT_SHADER_EXTENSION);
}

nsshader::nsshader(const nsshader & copy_):
	nsasset(copy_),
	nsvideo_object("nsshader"),
	shader_stages(copy_.shader_stages)
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
	vid_ctxt * vc = nse.video_driver()->current_context();
	if (vc != nullptr)
	{
		if (ctxt_objs[vc->context_id] == nullptr)
		{
			ctxt_objs[vc->context_id] = nse.factory<nsvid_obj_factory>(SHADER_VID_OBJ_GUID)->create(this);
		}
		else
		{
			dprint("nsshader::video_context_init - Context has already been initialized for shader " + m_name + " in ctxtid " + std::to_string(vc->context_id));
		}
	}
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
