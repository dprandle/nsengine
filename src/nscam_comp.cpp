/*!
\file nscamcomp.cpp

\brief Definition file for nscam_comp class

This file contains all of the neccessary definitions for the nscam_comp class.

\author Daniel Randle
\date December 17 2013
\copywrite Earth Banana Games 2013
*/

#include <nscam_comp.h>
#include <nsentity.h>
#include <nstimer.h>
#include <nstform_comp.h>

nscam_comp::nscam_comp():
m_flying(),
m_strafing(),
m_elevating(),
m_focus_point(),
m_focus_orientation(),
m_speed(DEFAULT_CAM_SPEED),
m_fov_angle(0.0f),
m_persp_nf_clip(),
m_proj_mode(proj_persp),
m_screen_size(),
m_focus_transform(),
NSComponent()
{}

nscam_comp::~nscam_comp()
{}

void nscam_comp::change_speed(float pAmount)
{
	m_speed += pAmount;
	post_update(true);
}

nscam_comp* nscam_comp::copy(const NSComponent * pToCopy)
{
	if (pToCopy == NULL)
		return NULL;
	const nscam_comp * comp = (const nscam_comp*)pToCopy;
	(*this) = (*comp);
	return this;
}

const fvec2 & nscam_comp::ortho_tb_clip()
{
	return m_tb_clip;
}

const fvec2 & nscam_comp::ortho_lr_clip()
{
	return m_lr_clip;
}

const fvec2 & nscam_comp::ortho_nf_clip()
{
	return m_nf_clip;
}

const fvec2 & nscam_comp::persp_nf_clip()
{
	return m_persp_nf_clip;
}

void nscam_comp::set_persp_nf_clip(const fvec2 & nf)
{
	m_persp_nf_clip = nf;
	_update_proj();
}

const fmat4 & nscam_comp::proj()
{
	return m_proj_mat;
}

float nscam_comp::fov()
{
	return m_fov_angle;
}

void nscam_comp::set_fov(float angledeg)
{
	m_fov_angle = angledeg;
	_update_proj();
}

float nscam_comp::aspect_ratio()
{
	return float(m_screen_size.w) / float(m_screen_size.h);
}

const fmat4 & nscam_comp::inv_proj()
{
	return m_inv_proj_mat;
}

nscam_comp::projection_mode nscam_comp::proj_mode()
{
	return m_proj_mode;
}

void nscam_comp::set_ortho_tb_clip(const fvec2 & tb)
{
	m_tb_clip = tb;
	_update_proj();
}

void nscam_comp::set_ortho_lr_clip(const fvec2 & lr)
{
	m_lr_clip = lr;
	_update_proj();
}

void nscam_comp::set_ortho_nf_clip(const fvec2 & nf)
{
	m_nf_clip = nf;
	_update_proj();
}

void nscam_comp::set_proj_mode(projection_mode mode)
{
	m_proj_mode = mode;
	_update_proj();
}

const fquat & nscam_comp::focus_orientation()
{
	return m_focus_orientation;
}

const fvec3 & nscam_comp::focus_point()
{
	return m_focus_point;
}

void nscam_comp::toggle_projection_mode()
{
	if (m_proj_mode == proj_persp)
		set_proj_mode(proj_ortho);
	else
		set_proj_mode(proj_persp);
}

void nscam_comp::set_focus_orientation(const fquat & pFocRot)
{
	m_focus_orientation = pFocRot;
	compute_focus_transform();
}

fmat4 nscam_comp::focus_transform()
{
	return m_focus_transform;
}

void nscam_comp::resize_screen(int32 w, int32 h)
{
	m_screen_size.set(w,h);
	_update_proj();
}

void nscam_comp::resize_screen(const ivec2 & dim)
{
	m_screen_size = dim;
	_update_proj();
}

void nscam_comp::compute_focus_transform()
{
	m_focus_transform.set(rotation_mat3(m_focus_orientation));
	m_focus_transform.set_column(3, m_focus_point.x, m_focus_point.y, m_focus_point.z, 1);
	m_focus_transform[3].x = 0; m_focus_transform[3].y = 0; m_focus_transform[3].z = 0;
}

const ivec2 & nscam_comp::screen_size()
{
	return m_screen_size;
}

const nscam_comp::movement_t & nscam_comp::elevate() const
{
	return m_elevating;
}

const nscam_comp::movement_t & nscam_comp::fly() const
{
	return m_flying;
}

float nscam_comp::speed() const
{
	return m_speed;
}

const nscam_comp::movement_t & nscam_comp::strafe() const
{
	return m_strafing;
}

void nscam_comp::init()
{}

const fmat4 & nscam_comp::proj_cam()
{
	return m_proj_cam;
}

const fmat4 & nscam_comp::inv_proj_cam()
{
	return m_inv_proj_cam;
}

void nscam_comp::set_elevate(dir_t pDir, bool pAnimate)
{
	if (m_elevating.direction != pDir)
	{
		if (!pAnimate)
			return;
		m_elevating.direction = pDir;
		m_elevating.animating = pAnimate;
	}
	else
		m_elevating.animating = pAnimate;
	post_update(true);
}

void nscam_comp::pup(nsfile_pupper * p)
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

void nscam_comp::set_fly(dir_t pDir, bool pAnimate)
{
	if (m_flying.direction != pDir)
	{
		if (!pAnimate)
			return;
		m_flying.direction = pDir;
		m_flying.animating = pAnimate;
	}
	else
		m_flying.animating = pAnimate;
	post_update(true);
}

void nscam_comp::set_focus_point(const fvec3 & pFocPoint)
{
	m_focus_point = pFocPoint;
	compute_focus_transform();
	post_update(true);
}

void nscam_comp::set_speed(float pUnitsPerSecond)
{
	m_speed = pUnitsPerSecond;
	post_update(true);
}

void nscam_comp::set_strafe(dir_t pDir, bool pAnimate)
{
	if (m_strafing.direction != pDir)
	{
		if (!pAnimate)
			return;
		m_strafing.direction = pDir;
		m_strafing.animating = pAnimate;
	}
	else
		m_strafing.animating = pAnimate;

	post_update(true);
}

nscam_comp & nscam_comp::operator=(const nscam_comp & pRHSComp)
{
	m_flying = pRHSComp.m_flying;
	m_elevating = pRHSComp.m_elevating;
	m_strafing = pRHSComp.m_strafing;
	m_speed = pRHSComp.m_speed;
	m_focus_point = pRHSComp.m_focus_point;
	m_focus_orientation = pRHSComp.m_focus_orientation;
	post_update(true);
	return (*this);
}

void nscam_comp::_update_proj()
{
	if (m_proj_mode == proj_persp)
		m_proj_mat = perspective(m_fov_angle, aspect_ratio(), m_persp_nf_clip.x, m_persp_nf_clip.y);
	else
		m_proj_mat = ortho(m_lr_clip.x, m_lr_clip.y, m_tb_clip.x, m_tb_clip.y, m_nf_clip.x, m_nf_clip.y);
	m_inv_proj_mat = inverse(m_proj_mat);
	post_update(true);
}
