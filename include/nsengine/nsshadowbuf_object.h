#ifndef NSSHADOWBUF_OBJECT_H
#define NSSHADOWBUF_OBJECT_H

#include <nsmath.h>
#include <nstexture.h>

class nsfb_object;

class nsshadowbuf_object
{
public:
	enum map_t {
		Direction,
		Point,
		Spot
	};

	nsshadowbuf_object();
	~nsshadowbuf_object();

	void bind(const map_t & type_);

	void disable(const map_t & type_);

	void enable(const map_t & type_);

	nstexture * texture(const map_t & type_);

	const uivec2 & size(const map_t & type_);

	nsfb_object * fb(const map_t & type_);

	void set_fb(nsfb_object * fb_, const map_t & type_);

	void init();

	void resize(const map_t & type_, uint32 w_, uint32 h_);

	void resize(const map_t & type_, const uivec2 & size_);

	void resize(uint32 w_, uint32 h_);

	void resize(const uivec2 & size_);

	void set_point_face(nstex_cubemap::cube_face face_);

private:
	nsfb_object * m_spot_fb;
	nsfb_object * m_point_fb;
	nsfb_object * m_dir_fb;
};

#endif
