/*!
  \file nsgl_draw_calls.h

  \brief Definition file for nsgl_draw_calls class

  This file contains all of the neccessary definitions for the nsgl_draw_calls class.

  \author Daniel Randle
  \date Feb 23 2016
  \copywrite Earth Banana Games 2013
*/

#ifndef NSGL_DRAW_CALLS_H
#define NSGL_DRAW_CALLS_H

#include <nsvector.h>
#include <asset/nsmesh.h>
#include <nsvideo_driver.h>

class nsshader;
class nsmaterial;
class nsfont;

struct nsgl_buffer;

struct gl_draw_call : public draw_call
{
	gl_draw_call():
		draw_call(),
		shdr(nullptr),
		mat(nullptr),
		mat_index(0)
	{}
	virtual ~gl_draw_call() {}
	
	nsshader * shdr;
	nsmaterial * mat;
	uint32 mat_index;
};

struct single_geom_info
{
	single_geom_info():
		transform(),
		entity_id(0)
	{}
	~single_geom_info() {}	
	uint32 entity_id;
	fmat4 transform;	
};

struct instanced_geom_info
{
	instanced_geom_info():
		tform_buffer(nullptr),
		tform_id_buffer(nullptr),
		transform_count(0)
	{}
	~instanced_geom_info() {}
	
	nsgl_buffer * tform_buffer;
	nsgl_buffer * tform_id_buffer;
	uint32 transform_count;
};

struct geometry_draw_call : public gl_draw_call
{
	geometry_draw_call():
		gl_draw_call(),
		scn(nullptr),
		submesh(nullptr),
		anim_transforms(nullptr),
		height_minmax(),
		plugin_id(0),
		casts_shadows(false),
		transparent_picking(false),
		sel_color(),
		instanced(false),
		single_dci(),
		instanced_dci()
	{}
	
	~geometry_draw_call() {}

	nsscene * scn;
	nsmesh::submesh * submesh;
	fmat4_vector * anim_transforms;
	fvec2 height_minmax;
	uint32 plugin_id;
	bool transparent_picking;
	bool casts_shadows;
	fvec4 sel_color;
	bool instanced;

	single_geom_info single_dci;
	instanced_geom_info instanced_dci;
};

typedef std::map<nsmaterial*, uint32> mat_id_map;

struct light_draw_call : public gl_draw_call
{
	light_draw_call():
		gl_draw_call(),
		scn(nullptr),
		proj_light_mat(),
		light_transform(),
		light_pos(),
		bg_color(),
		direction(),
		cast_shadows(false),
		light_color(),
		spot_atten(),
		shadow_samples(0),
		shadow_darkness(0.0f),
		diffuse_intensity(0.0f),
		ambient_intensity(0.0f),
		material_ids(nullptr),
		max_depth(0.0f),
		cutoff(0.0f),
		light_type(0),
		submesh(nullptr)
	{}
	
	~light_draw_call() {}

	nsscene * scn;
	fmat4 proj_light_mat;
	fmat4 light_transform;
	fvec3 light_pos;
	fvec4 bg_color;
	fvec3 direction;
	bool cast_shadows;
	fvec3 light_color;
	fvec3 spot_atten;
	int32 shadow_samples;
	float shadow_darkness;
	float diffuse_intensity;
	float ambient_intensity;
	mat_id_map * material_ids;
	float max_depth;
	float cutoff;
	uint32 light_type;
	nsmesh::submesh * submesh;
};

struct ui_draw_call : public gl_draw_call
{
	ui_draw_call():
		gl_draw_call(),
		text_shader(nullptr),
		entity_id(),
		content_tform(),
		content_wscale(1.0f),
		border_pix(0.0f),
		text(),
		fnt(nullptr),
		text_editable(false),
		cursor_pixel_width(0),
		cursor_color(1.0f,0.0f,0.0f,1.0f),
		cursor_offset(),
		text_line_sizes(),
		text_margins(),
		alignment(0)
	{}

	~ui_draw_call() {}

	// ui_rect_tform info
	uivec3 entity_id;	
	fmat3 content_tform;
	fvec2 content_wscale;

	// ui_material_comp along with shdr and mat
	nsmaterial * border_mat;
	fvec4 border_pix;
	fvec4 top_border_radius;
	fvec4 bottom_border_radius;

	// ui_text_comp info
	nsshader * text_shader;
	nsstring text;
	nsfont * fnt;
	nsmaterial * fnt_material;
	std::vector<uint32> text_line_sizes;
	uivec4 text_margins;
	uint8 alignment;

	// ui_text_input_comp info
	bool text_editable;
	uint32 cursor_pixel_width;
	fvec4 cursor_color;
	uivec2 cursor_offset;
};


#endif
