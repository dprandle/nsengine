#ifndef NSGBUF_OBJECT_H
#define NSGBUF_OBJECT_H

#include <nsfb_object.h>

class nsgbuf_object : public nsfb_object
{
public:
	enum color_attachment_t {
		col_diffuse,
		col_picking,
		col_position,
		col_normal,
		attrib_count };

	nsgbuf_object();
	~nsgbuf_object();

	void debug_blit(const ivec2 & scrn);

	nsfb_object::attachment * color(uint32 att_type);

	nsfb_object::attachment * depth();

	void init();

private:

	bool _create_texture_attachments();
	uint32 m_multisample_level;
};

#endif
