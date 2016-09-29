/*!
\file nsselection_system.h

\brief Header file for nsselection_system class

This file contains all of the neccessary declarations for the nsselection_system class.

\author Daniel Randle
\date March 8 2014
\copywrite Earth Banana Games 2013
*/

#ifndef NSSELECTION_SYSTEM_H
#define NSSELECTION_SYSTEM_H

#include <system/nssystem.h>
#include <asset/nsentity_manager.h>

#define NSSEL_SELECT "select_entity"
#define NSSEL_SHIFTSELECT "shift_select"
#define NSSEL_MULTISELECT "multi_select"
#define NSSEL_MOVE "move_selection"
#define NSSEL_MOVE_XY "move_selection_xy"
#define NSSEL_MOVE_ZY "move_selection_zy"
#define NSSEL_MOVE_ZX "move_selection_zx"
#define NSSEL_MOVE_X "move_selection_x"
#define NSSEL_MOVE_Y "move_selection_y"
#define NSSEL_MOVE_Z "move_selection_z"
#define NSSEL_MOVE_TOGGLE "move_selection_toggle"
#define SELECTION_INPUT_CTXT "selection_controls"

class nsscene;
class nssel_comp;
class nsselection_shader;
struct nsaction_event;
struct nsstate_event;

class nsselection_system : public nssystem
{
public:

	SLOT_OBJECT
	
	enum axis_t
	{
        axis_x=0x01,
        axis_y=0x02,
        axis_z=0x04
	};

	nsselection_system();

	~nsselection_system();

	bool add_to_selection(nsentity * ent);

	bool add_selection_to_grid(nsscene * scn);

	void change_layer(int32 pChange);

	bool selection_collides_with_tilegrid();

	void delete_selection();

	void clear_selection();

	bool selection_contains(const uivec2 & itemid);

	entity_ptr_set & current_selection();

	bool empty();

	const uivec2 & selection_center_entity_id();

	void enable_mirror_selection(bool enable_);

	void enable_draw_occupied_grid(bool enable_);

	bool draw_occupied_grid();

	bool mirror_selection_enabled();

	int32 update_priority();

	uivec3 pick(float mousex, float mousey);

	uivec3 pick(const fvec2 & mpos);

	void init();

	void release();

	bool valid_tile_swap();

	bool selection_being_dragged();

	void remove_from_selection(nsentity * ent);

	void remove_from_grid();

	void reset_color();

	bool set_selection(nsentity * ent);

	void set_selection_color(const fvec4 & pColor);

	void setup_input_map(nsinput_map * imap, const nsstring & global_ctxt_name);

	void set_focus_entity(const uivec2 & focus_ent);

	void set_occupied_spaces(bool show);

	void set_mirror_tile_color(const fvec4 & color);

	void translate_selection(const fvec3 & amount);

	void rotate_selection(const fquat & rotation);

	void snap_selection_to_grid();

	const fvec4 & mirror_tile_color();

	virtual void update();
	
protected:

	enum input_triggers
	{
		selected_entity,
		multi_select,
		shift_select,
		move_select,
		move_selection_xy,
		move_selection_zy,
		move_selection_zx,
		move_selection_x,
		move_selection_y,
		move_selection_z,
		move_selection_toggle
	};
   	
	bool _handle_selected_entity(nsaction_event * evnt);
	bool _handle_multi_select(nsaction_event * evnt);
	bool _handle_shift_select(nsaction_event * evnt);
	bool _handle_move_select(nsaction_event * evnt);
	bool _handle_move_selection_xy(nsaction_event * evnt);
	bool _handle_move_selection_zy(nsaction_event * evnt);
	bool _handle_move_selection_zx(nsaction_event * evnt);
	bool _handle_move_selection_x(nsaction_event * evnt);
	bool _handle_move_selection_y(nsaction_event * evnt);
	bool _handle_move_selection_z(nsaction_event * evnt);
	bool _handle_move_selection_toggle(nsaction_event * evnt);
	bool _handle_rotate_selection(nsaction_event * evnt);
	bool _handle_scene_ent_removed(scene_ent_removed * evnt);

	void on_ent_destroyed(uint32 type_id, uivec2 ent_id);
	
	void _reset_focus(const uivec2 & pickid);

	virtual void _on_draw_object(
		nsentity * ent,
		const fvec2 & pDelta,
		uint16 _axis,
        const fvec4 & norm_vp
		);

	void _draw_occ();
	void _draw_ent_occ(nsentity * ent);
	void _draw_hidden();

	uivec2 m_focus_ent; //!< The entity/tform ID that the selection is focused on (the center of rotation)
	entity_ptr_set m_selected_ents;
	fvec3 m_total_frame_translation;
	bool m_moving;
	bool m_toggle_move;
	bool m_send_foc_event;
	bool m_mirror_selection;
    bool m_started_drag_over_ui;
	
	fvec2 m_pick_pos;
	fvec3 m_cached_point;
	fvec4 m_mirror_tile_color;

	bool m_draw_occ;
	fvec2 m_cached_point_last;
	fmat4 m_trans;
};

#endif

