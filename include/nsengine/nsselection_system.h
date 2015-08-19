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

#include <nssystem.h>
#include <nsglobal.h>
#include <nsevent.h>
#include <nstform_comp.h>
#include <nsmath.h>

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

class nsscene;
class NSSelComp;
class nsselection_shader;
class nsaction_event;
class nsstate_event;

class nsselection_system : public nssystem
{
public:
	
	enum axis_t
	{
		axis_x=0x0001,
		axis_y=0x0010,
		axis_z=0x0100
	};

	nsselection_system();

	~nsselection_system();

	bool add(nsentity * ent, uint32 tformid);

	bool add_to_grid();

	void change_layer(int32 pChange);

	bool collision();

	void del();

	void clear();

	bool contains(const uivec3 & itemid);

	virtual void draw();

	bool empty();

	void enable_layer_mode(const bool & pMode);

	int32 layer() const;

	const uivec3 & center();

	virtual int32 draw_priority();

	virtual int32 update_priority();

	uivec3 pick(float mousex, float mousey);

	uivec3 pick(const fvec2 & mpos);

	virtual void init();

	bool layer_mode() const;

	bool valid_brush();

	bool valid_tile_swap();

	void remove(nsentity * ent, uint32 pTFormID);

	void remove_from_grid();

	void reset_color();

	void rotate(nsentity * ent, const fvec4 & axisangle);

	void rotate(const fvec4 & axisangle);

	void rotate(nsentity * ent, NSTFormComp::DirVec axis, float angle);

	void rotate(NSTFormComp::DirVec axis, float angle);

	void rotate(nsentity * ent, NSTFormComp::Axis axis, float angle);

	void rotate(NSTFormComp::Axis axis, float angle);

	void rotate(nsentity * ent, const fvec3 & euler);

	void rotate(const fvec3 & euler);

	void rotate(nsentity * ent, const fquat & orientation);

	void rotate(const fquat & orientation);

	void scale(nsentity * ent, const fvec3 & pAmount);

	void scale(const fvec3 & pAmount);

	void scale(nsentity * ent, float x, float y, float z);

	void scale(float x, float y, float z);

	bool set(nsentity * ent, uint32 tformid);

	void set_color(const fvec4 & pColor);

	void set_picking_fbo(uint32 fbo);

	void set_final_fbo(uint32 fbo);

	void set_hidden_state(NSTFormComp::HiddenState pState, bool pSet);

	void set_layer(int32 pLayer);

	void set_shader(nsselection_shader * selShader);

	void set_occupied_spaces(bool show);

	void snap(nsentity * ent);

	void snap();

	void snap_x(nsentity * ent);

	void snap_x();

	void snap_y(nsentity * ent);

	void snap_y();

	void snap_z(nsentity * ent);

	void snap_z();

	void tile_swap(nsentity * newtile);

	void translate(nsentity * ent, const fvec3 & pAmount);

	void translate(const fvec3 & pAmount);

	void translate(float x, float y, float z);

	void translate(nsentity * ent, float x, float y, float z);

	void translate(nsentity * ent, NSTFormComp::DirVec pDir, float pAmount);

	void translate(NSTFormComp::DirVec pDir, float pAmount);

	void translate(nsentity * ent, NSTFormComp::Axis pDir, float pAmount);

	void translate(NSTFormComp::Axis pDir, float pAmount);

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
   	
	bool _handle_action_event(nsaction_event * evnt);
	bool _handle_state_event(nsstate_event * evnt);

	void _reset_focus(const uivec3 & pickid);

	virtual void _on_rotate_x(
		nsentity * ent,
		bool pPressed
		);

	virtual void _on_rotate_y(
		nsentity * ent,
		bool pPressed
		);

	virtual void _on_rotate_z(
		nsentity * ent,
		bool pPressed
		);

	virtual void _on_select(
		nsentity * ent,
		bool pPressed,
		const uivec3 & pID,
		bool pSnapZOnly = false
		);

	virtual void _on_multi_select(
		nsentity * ent,
		bool pPressed,
		const uivec3 & pID
		);

	virtual void _on_paint_select(
		nsentity * ent,
		const fvec2 & pPos
		);

	virtual void _on_draw_object(
		nsentity * ent,
		const fvec2 & pDelta,
		uint16 _axis
		);

	void _draw_occ();
	void _draw_ent_occ(nsentity * ent);
	void _draw_hidden();

	uivec3 m_focus_ent; //!< The entity/tform ID that the selection is focused on (the center of rotation)
	nspentityset m_selected_ents;
	nsselection_shader * m_sel_shader;
	fvec3 m_total_frame_translation;
	bool m_moving;
	bool m_toggle_move;
	bool m_send_foc_event;
	
	fvec2 m_pick_pos;
	fvec3 m_cached_point;
	bool m_layer_mode;
	bool m_draw_occ;
	int32 m_layer;
	fvec2 m_cached_point_last;
	uint32 m_final_buf;
	uint32 m_picking_buf;
	fmat4 m_trans;
};

#endif
