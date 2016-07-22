#ifndef NSGL_GBUFFER_H
#define NSGL_GBUFFER_H

#include "nsgl_framebuffer.h"

struct nsgl_gbuffer : public nsgl_framebuffer
{
	enum color_attachment_t {
		col_diffuse,
		col_picking,
		col_position,
		col_normal,
		attrib_count };

	nsgl_gbuffer();
	~nsgl_gbuffer();

	void debug_blit(const ivec2 & scrn);

	nsgl_framebuffer::attachment * color(uint32 att_type);

	nsgl_framebuffer::attachment * depth();

	void init();

	bool create_texture_attachments();
};

#endif
