/*!
\file nscam_comp.h_

\brief Header file for nscam_comp class

This file contains all of the neccessary declarations for the nscam_comp class.

\author Daniel Randle
\date December 17 2013
\copywrite Earth Banana Games 2013
*/

#ifndef NSCAMCOMP_H
#define NSCAMCOMP_H

#define DEFAULT_Z_NEAR 1.0f
#define DEFAULT_Z_FAR 1000.0f
#define DEFAULT_CAM_VIEW_Z -15.0f
#define DEFAULT_CAM_SPEED 10.0f
#define DEFAULT_CAM_ZOOM_FACTOR 3.0f
#define DEFAULT_CAM_TOP_VIEW_HEIGHT 80.0f
#define DEFAULT_FOV_ANGLE 45.0f

#include <nscomponent.h>

class nsentity;
class nstimer;

class nscam_comp : public nscomponent
{
public:
	friend class nscamera_system;

	enum dir_t {
		dir_none = 0,
		dir_pos = 1,
		dir_neg = -1
	};

	struct movement_t
	{
		movement_t() :direction(dir_none), animating(false){}
		int32 direction;
		bool animating;
	};

	enum view_t {
		view_normal,
		view_top,
		view_iso,
		view_front
	};

	enum projection_mode {
		proj_persp,
		proj_ortho
	};

	template <class PUPer>
	friend void pup(PUPer & p, nscam_comp & cc);

	nscam_comp();

	nscam_comp(const nscam_comp & copy);

	~nscam_comp();

	void change_speed(float amount_);

	void compute_focus_transform();

	const movement_t & elevate() const;

	const movement_t & fly() const;

	const fvec3 & focus_point();

	fmat4 focus_transform();

	const fquat & focus_orientation();

	float speed() const;

	void toggle_projection_mode();

	const movement_t & strafe() const;

	void init();

	float aspect_ratio();

	const fvec2 & ortho_tb_clip();

	const fvec2 & ortho_lr_clip();

	const fvec2 & ortho_nf_clip();

	const fvec2 & persp_nf_clip();

	projection_mode proj_mode();

	const ivec2 & screen_size();
	
	virtual void pup(nsfile_pupper * p);

	void set_elevate(dir_t dir_, bool animate_);

	void set_ortho_tb_clip(const fvec2 & tb_);

	void set_ortho_lr_clip(const fvec2 & lr_);

	void set_ortho_nf_clip(const fvec2 & nf_);

	void set_persp_nf_clip(const fvec2 & nf_);

	void set_proj_mode(projection_mode mode_);

	void set_fly(dir_t dir_, bool animate_);

	void set_fov(float angle_deg_);

	void set_focus_point(const fvec3 & foc_point_);

	void set_focus_orientation(const fquat & foc_orient_);

	const fmat4 & proj();

	const fmat4 & proj_cam();

	const fmat4 & inv_proj_cam();

	const fmat4 & inv_proj();

	float fov();

	void set_speed(float units_per_sec_);

	void set_strafe(dir_t dir_, bool animate_);

	void resize_screen(int32 w_, int32 h_);

	void resize_screen(const ivec2 & dimen_);

	nscam_comp & operator=(nscam_comp rhs_);

private:
	void _update_proj();

	movement_t m_flying;
	movement_t m_strafing;
	movement_t m_elevating;
	float m_speed;
	fvec3 m_focus_point;
	fquat m_focus_orientation;
	float m_fov_angle;
	fvec2 m_persp_nf_clip;
	ivec2 m_screen_size;
	fvec2 m_tb_clip;
	fvec2 m_lr_clip;
	fvec2 m_nf_clip;
	projection_mode m_proj_mode;
	fmat4 m_proj_mat;
	fmat4 m_inv_proj_mat;
	fmat4 m_proj_cam;
	fmat4 m_inv_proj_cam;
	fmat4 m_focus_transform;
};

template <class PUPer>
void pup(PUPer & p, nscam_comp & cc)
{
	pup(p, cc.m_flying, "flying");
	pup(p, cc.m_strafing, "strafing");
	pup(p, cc.m_elevating, "elevating");
	pup(p, cc.m_speed, "speed");
	pup(p, cc.m_focus_point, "focus_point");
	pup(p, cc.m_focus_orientation, "focus_orientation");
	pup(p, cc.m_focus_transform, "focus_transform");

	pup(p, cc.m_fov_angle, "fov_angle");
	pup(p, cc.m_persp_nf_clip, "persp_nf_clip");
	pup(p, cc.m_screen_size, "screen_size");

	pup(p, cc.m_tb_clip, "tb_clip");
	pup(p, cc.m_lr_clip, "lr_clip");
	pup(p, cc.m_nf_clip, "nf_clip");
	pup(p, cc.m_proj_mode, "proj_mode");

	cc._update_proj();
}

template <class PUPer>
void pup(PUPer & p, nscam_comp::movement_t & cc, const nsstring & var_name_)
{
	pup(p, cc.direction, "dir");
	pup(p, cc.animating, "animating");
}

template<class PUPer>
void pup(PUPer & p, nscam_comp::projection_mode & en, const nsstring & var_name_)
{
	uint32 in = static_cast<uint32>(en);
	pup(p, in, var_name_);
	en = static_cast<nscam_comp::projection_mode>(in);
}

#endif
