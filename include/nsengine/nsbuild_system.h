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

#include <nssystem.h>

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

	void enable(const bool & pEnable, const fvec2 & pMousePos);

	void enable_overwrite(bool pEnable);

	void enable_mirror(bool pEnable);

	void erase();

	const fvec4 active_brush_color() const;

	const brush_t & brush_type();

	nsentity * build_ent();

	const fvec3 & center() const;

	nsentity * tile_brush();

	nsentity * object_brush();

	const int32 & layer() const;

	const mode_t & mode() const;

	virtual void init();

	bool enabled() const;

	bool overwrite() const;

	bool mirror() const;

	void to_cursor(const fvec2 & pCursorPos, bool pUpdateCamFirst=false);

	void paint();

	void set_brush_type(const brush_t & pBrushMode);

	void set_active_brush_color(const fvec4 & pColor);

	void set_mode(const mode_t & pMode);

	void set_object_brush(nsentity * pBrush);

	void set_tile_brush(nsentity * pBrush);

	void set_build_ent(nsentity * pBuildEnt);

	void set_layer(const int32 & pLayer);

	void setCenter(const fvec3 & pMirrorCenter);

	void toggle(const fvec2 & pMousePos);

	virtual void update();

	virtual int32 update_priority();

private:
	nsentity * m_tile_brush;
	nsentity * m_object_brush;
	nsentity * m_mirror_brush;
	nsentity * m_build_ent;
	fvec3 m_mirror_center;
	uint32 m_tile_brush_center_tform_id;
	int32 m_layer;
	mode_t m_current_mode;
	brush_t m_current_brush_type;
	bool m_overwrite;
	bool m_enabled;
	bool m_mirror_mode;
};


#endif
