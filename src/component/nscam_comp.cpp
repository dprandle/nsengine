/*!
\file nscamcomp.cpp

\brief Definition file for nscam_comp class

This file contains all of the neccessary definitions for the nscam_comp class.

\author Daniel Randle
\date December 17 2013
\copywrite Earth Banana Games 2013
*/

#include <component/nscam_comp.h>
#include <asset/nsentity.h>
#include <nstimer.h>
#include <component/nstform_comp.h>
#include <algorithm>

nscam_comp::nscam_comp():
	nscomponent(type_to_hash(nscam_comp)),
	m_flying(),
	m_strafing(),
	m_elevating(),
	m_speed(DEFAULT_CAM_SPEED),
	m_fov_angle(0.0f),
	m_persp_nf_clip(),
	m_proj_mode(proj_persp),
    m_screen_size()
{}

nscam_comp::nscam_comp(const nscam_comp & copy):
	nscomponent(copy),
	m_flying(),
	m_strafing(),
	m_elevating(),
	m_speed(copy.m_speed),
	m_fov_angle(copy.m_fov_angle),
	m_persp_nf_clip(copy.m_persp_nf_clip),
	m_screen_size(copy.m_screen_size),
	m_tb_clip(copy.m_tb_clip),
	m_lr_clip(copy.m_lr_clip),
	m_nf_clip(copy.m_nf_clip),
	m_proj_mode(copy.m_proj_mode),
	m_proj_mat(copy.m_proj_mat),
	m_inv_proj_mat(copy.m_inv_proj_mat),
	m_proj_cam(copy.m_proj_cam),
    m_inv_proj_cam(copy.m_inv_proj_cam)
{}

nscam_comp::~nscam_comp()
{}

void nscam_comp::change_speed(float pAmount)
{
	m_speed += pAmount;
	post_update(true);
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

void nscam_comp::toggle_projection_mode()
{
	if (m_proj_mode == proj_persp)
		set_proj_mode(proj_ortho);
	else
		set_proj_mode(proj_persp);
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

nscam_comp & nscam_comp::operator=(nscam_comp rhs_)
{
	nscomponent::operator=(rhs_);
	std::swap(m_flying,rhs_.m_flying);
	std::swap(m_elevating,rhs_.m_elevating);
	std::swap(m_strafing,rhs_.m_strafing);
	std::swap(m_speed,rhs_.m_speed);
	std::swap(m_fov_angle, rhs_.m_fov_angle);
	std::swap(m_persp_nf_clip, rhs_.m_persp_nf_clip);
	std::swap(m_screen_size, rhs_.m_screen_size);
	std::swap(m_tb_clip, rhs_.m_tb_clip);
	std::swap(m_lr_clip, rhs_.m_lr_clip);
	std::swap(m_nf_clip, rhs_.m_nf_clip);
	std::swap(m_proj_mode, rhs_.m_proj_mode);
	std::swap(m_inv_proj_mat, rhs_.m_inv_proj_mat);
	std::swap(m_proj_cam, rhs_.m_proj_cam);
	std::swap(m_inv_proj_cam, rhs_.m_inv_proj_cam);
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
