/*!
\file nscamera_system.h

\brief Header file for nscamera_system class

This file contains all of the neccessary declarations for the nscamera_system class.

\author Daniel Randle
\date December 17 2013
\copywrite Earth Banana Games 2013
*/

#ifndef NSCAMERA_SYSTEM_H
#define NSCAMERA_SYSTEM_H

#define NSCAM_FORWARD "camera_forward"
#define NSCAM_BACKWARD "camera_backward"
#define NSCAM_LEFT "camera_left"
#define NSCAM_RIGHT "camera_right"
#define NSCAM_TILTPAN "camera_tilt_pan"
#define NSCAM_MOVE "camera_move"
#define NSCAM_ZOOM "camera_zoom"
#define NSCAM_TOPVIEW "camera_top_view"
#define NSCAM_ISOVIEW "camera_iso_view"
#define NSCAM_FRONTVIEW "camera_front_view"
#define NSCAM_TOGGLEMODE "camera_toggle_mode"

#include <nssystem.h>
#include <nsglobal.h>
#include <nsmath.h>

class NSCamComp;
class NSTFormComp;
class nsaction_event;
class nsstate_event;

class nscamera_system : public nssystem
{
public:

	enum camera_mode {
		mode_free, /*!< Free mode - camera moves as fps */
		mode_focus /*!< Focus mode - camera focuses on single object */
	};

	enum sensitivity_t
	{
		sens_turn,
		sens_strafe
	};

	/*!
	Enum holds 3 different pre-set camera view identifiers
	*/
	enum camera_view_t {
		view_top, /*!< Top down camera view */
		view_iso, /*!< Isometric camera view */
		view_front /*!< Front on camera view */
	};

	nscamera_system();
	~nscamera_system();

	void change_sensitivity(float amount_, const sensitivity_t & which_);

	void init();

	virtual void update();

	const camera_mode & mode() const;

	const float & sensitivity(const sensitivity_t & which_) const;

	float zoom() const;

	void set_sensitivity(float sensitivity_, const sensitivity_t & which_);

	void set_mode(camera_mode mode_);

	void set_view(camera_view_t view_);

	bool x_inverted(const camera_mode & mode_ = mode_free);

	bool y_inverted(const camera_mode & mode_ = mode_free);

	void invert_x(bool invert_, const camera_mode & mode_=mode_free);

	void invert_y(bool invert_, const camera_mode & mode_ = mode_free);

	void set_zoom(float zfactor_);

	void toggle_mode();

	virtual int32 update_priority();

private:

	enum input_trigger_t
	{
		camera_forward,
		camera_backward,
		camera_left,
		camera_right,
		camera_tilt_pan,
		camera_move,
		camera_zoom,
		camera_top_view,
		camera_iso_view,
		camera_front_view,
		camera_toggle_mode
	};

    void _on_cam_turn(NSCamComp * pCam, NSTFormComp * tComp, const fvec2 & pDelta);
	void _on_cam_move(NSCamComp * pCam, NSTFormComp * tComp, const fvec2 & pDelta);
	void _on_cam_zoom(NSCamComp * pCam, NSTFormComp * tComp, float pScroll);
	
	bool _handle_action_event(nsaction_event * evnt);
	bool _handle_state_event(nsstate_event * evnt);
	bool _handle_sel_focus_event(nssel_focus_event * evnt);
	
	float m_zoom_factor;
	float m_turn_sensitivity;
	float m_strafe_sensitivity;
	float m_anim_time;
	float m_anim_elapsed;
	fquat m_start_orient, m_final_orient;
	fvec3 m_start_pos, m_final_pos;
	bool m_anim_view, m_switch_back;
	ivec2 m_free_mode_inverted;
	ivec2 m_focus_mode_inverted;
	uivec3 m_focus_ent;
	camera_mode m_cam_mode;
};

#endif
