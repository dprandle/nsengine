#ifndef NSGBUF_OBJECT_H
#define NSGBUF_OBJECT_H

#include <nsfb_object.h>

class nsgbuf_object
{
public:
	enum color_attachment_t {
		col_diffuse,
		col_picking,
		col_position,
		col_normal,
		col_material,
		attrib_count };

	nsgbuf_object();
	~nsgbuf_object();

	void bind();

	void debug_blit(const ivec2 & scrn);

	void disable_textures();

	void enable_textures();

	void enabled_color_write(bool pEnable);

	nsfb_object::attachment * color(color_attachment_t pType);

	nsfb_object::attachment * depth();

	const uivec2 & fb_size();

	nsfb_object * fb();

	uivec3 pick(float mousex, float mousey);

	const uivec2 & screen_size();

	void init();

	void resize_fb(uint32 w, uint32 h);

	void resize_fb(const uivec2 & size_);

	void resize_screen(uint32 w, uint32 h);

	void resize_screen(const uivec2 & size_);

	void set_fb(nsfb_object * fb);

	void unbind();

private:

	bool _create_texture_attachments();

	nsfb_object * m_tex_fb;
	uivec2 m_screen_size;
	uint32 m_multisample_level;
};

#endif
