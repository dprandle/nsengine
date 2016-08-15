/*!
\file nsbuild_system.h

\brief Header file for nsbuild_system class

This file contains all of the neccessary declarations for the nsbuild_system class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#ifndef NSBUILDSYSTEM_H
#define NSBUILDSYSTEM_H

#define ENT_OBJECT_BRUSH "objectbrush"
#define ENT_MIRROR_BRUSH "mirrorbrush"
#define NSINSERT_ENTITY "insert_entity"
#define NSINITIAL_SNAP_BRUSH_Z "initial_snap_brush_z"
#define NSSNAP_BRUSH_Z "snap_brush_z"
#define BUILD_MODE_CTXT "build_mode"
#define NSTOGGLE_BUILD "toggle_build"
#define NSTOGGLE_BUILD_ERASE_MODE "toggle_build_erase_mode"
#define NSTOGGLE_TILE_BUILD_MODE "toggle_tile_build_mode"

#include <system/nssystem.h>

class nsentity;

class nsbuild_system : public nssystem
{
public:

	enum mode_t {
		build_mode,
		erase_mode
	};

	enum brush_t {
		brush_none,
		brush_tile,
		brush_object
	};

	nsbuild_system();

	~nsbuild_system();

	void change_layer(const int32 & pAmount);

	void enable(const bool & pEnable);

	void enable_overwrite(bool pEnable);

	void enable_mirror(bool pEnable);

	void enable_stamp_mode(bool enable);

	void erase();

	const fvec4 active_brush_color() const;

	const brush_t & brush_type();

	nsentity * tile_build_ent();

	nsentity * object_build_ent();

	const fvec3 & center() const;

	nsentity * tile_brush();

	nsentity * object_brush();

	const int32 & layer() const;

	const mode_t & mode() const;

	void init();

	void release();

	bool enabled() const;

	bool overwrite() const;

	bool mirror() const;

	void to_cursor();

	void paint();

	void set_brush_type(const brush_t & pBrushMode);

	void set_active_brush_color(const fvec4 & pColor);

	void set_mode(const mode_t & pMode);

	void set_object_brush(nsentity * pBrush);

	void set_tile_brush(nsentity * pBrush);

	void set_object_build_ent(nsentity * pBuildEnt);

	void set_tile_build_ent(nsentity * pBuildEnt);

	void set_layer(const int32 & pLayer);

	void set_center(const fvec3 & pMirrorCenter);

	bool stamp_mode();

	void toggle();

	virtual void update();

	virtual int32 update_priority();

private:

	enum input_trigger_t
	{
		insert_entity,
		initial_snap_brush_z,
		snap_brush_z,
		toggle_build,
		toggle_build_erase_mode,
		toggle_tile_build_mode,
		select_move_toggle // this is for snapping after z movement and continuing move mode
	};

	bool _handle_initial_snap_brush_z(nsaction_event * evnt);
	bool _handle_snap_z(nsaction_event * evnt);
	bool _handle_toggle_build_erase_mode(nsaction_event * evnt);
	bool _handle_toggle_build(nsaction_event * evnt);
	bool _handle_toggle_tile_build_mode(nsaction_event * evnt);
	bool _handle_insert_entity(nsaction_event * evnt);
	bool _handle_select_move_toggle(nsaction_event * evnt);
	bool _handle_cam_change_event(nscam_change_event * evnt);
	bool _handle_mouse_move_event(nsmouse_move_event * mevnt);
	
	nsentity * m_tile_brush;
	nsentity * m_object_brush;
	nsentity * m_mirror_brush;

	nsentity * m_tile_build_ent;
	nsentity * m_object_build_ent;

	fvec3 m_mirror_center;
	uint32 m_tile_brush_center_tform_id;
	int32 m_layer;

	mode_t m_current_mode;
	brush_t m_current_brush_type;
	bool m_overwrite;
	bool m_enabled;

	bool m_mirror_mode;

	bool m_painting;
	bool m_erasing;
	bool m_stamp_mode;
	fvec2 m_norm_mpos;
};


#endif
