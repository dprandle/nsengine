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

#define NSCAM_TOPVIEW_0 "camera_top_view_0"
#define NSCAM_ISOVIEW_0 "camera_iso_view_0"
#define NSCAM_FRONTVIEW_0 "camera_front_view_0"
#define NSCAM_TOPVIEW_120 "camera_top_view_120"
#define NSCAM_ISOVIEW_120 "camera_iso_view_120"
#define NSCAM_FRONTVIEW_120 "camera_front_view_120"
#define NSCAM_TOPVIEW_240 "camera_top_view_240"
#define NSCAM_ISOVIEW_240 "camera_iso_view_240"
#define NSCAM_FRONTVIEW_240 "camera_front_view_240"

#define NSCAM_TOGGLEMODE "camera_toggle_mode"

#define CAMERA_INPUT_CTXT "camera_controls"

#define DEFAULT_CAM_PIVOT_SENSITIVITY 70.0f
#define DEFAULT_CAM_MOVE_SENSITIVITY 15.0f

#include <nssystem.h>

class nsentity;
class nscam_comp;
class nstform_comp;
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
	enum camera_view_t
	{
		view_top_0,
		view_top_120,
		view_top_240,
		view_iso_0,
		view_iso_120,
		view_iso_240,
		view_front_0,
		view_front_120,
		view_front_240
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

	void setup_input_map(nsinput_map * imap, const nsstring & global_imap_name);

	void set_active_scene(nsscene * active_scene);

	bool x_inverted(const camera_mode & mode_ = mode_free);

	bool y_inverted(const camera_mode & mode_ = mode_free);

	void invert_x(bool invert_, const camera_mode & mode_=mode_free);

	void invert_y(bool invert_, const camera_mode & mode_ = mode_free);

	void set_zoom(float zfactor_);

	void toggle_mode();

	void set_camera_focus_manipulator(nsentity * cam_manip);

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
		camera_top_view_0,
		camera_iso_view_0,
		camera_front_view_0,
		camera_top_view_120,
		camera_iso_view_120,
		camera_front_view_120,
		camera_top_view_240,
		camera_iso_view_240,
		camera_front_view_240,
		camera_toggle_mode
	};
	
    void _on_cam_turn(nscam_comp * pCam, nstform_comp * tComp, const fvec2 & pDelta);
	void _on_cam_move(nscam_comp * pCam, nstform_comp * tComp, const fvec2 & pDelta);
	void _on_cam_zoom(nscam_comp * pCam, nstform_comp * tComp, float pScroll);

	bool _handle_camera_tilt_pan(nsaction_event * evnt);
	bool _handle_camera_move(nsaction_event * evnt);
	bool _handle_camera_zoom(nsaction_event * evnt);
	bool _handle_camera_top_view_0(nsaction_event * evnt);
	bool _handle_camera_iso_view_0(nsaction_event * evnt);
	bool _handle_camera_front_view_0(nsaction_event * evnt);
	bool _handle_camera_top_view_120(nsaction_event * evnt);
	bool _handle_camera_iso_view_120(nsaction_event * evnt);
	bool _handle_camera_front_view_120(nsaction_event * evnt);
	bool _handle_camera_top_view_240(nsaction_event * evnt);
	bool _handle_camera_iso_view_240(nsaction_event * evnt);
	bool _handle_camera_front_view_240(nsaction_event * evnt);
	bool _handle_camera_toggle_mode(nsaction_event * evnt);
	bool _handle_camera_forward(nsaction_event * evnt);
	bool _handle_camera_backward(nsaction_event * evnt);
	bool _handle_camera_left(nsaction_event * evnt);
	bool _handle_camera_right(nsaction_event * evnt);	
	bool _handle_sel_focus_event(nssel_focus_event * evnt);
	
	float m_zoom_factor;
	float m_turn_sensitivity;
	float m_strafe_sensitivity;
	float m_anim_time;
	float m_anim_elapsed;

	fquat m_start_orient, m_start_local_orient, m_final_orient;
	fvec3 m_start_pos, m_final_pos;
	bool m_anim_view, m_switch_back;

	ivec2 m_free_mode_inverted;
	ivec2 m_focus_mode_inverted;
	camera_mode m_cam_mode;
	nsentity * m_cam_focus_manipulator;
};

#endif
