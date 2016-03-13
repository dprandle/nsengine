#ifndef NSSHADOWBUF_OBJECT_H
#define NSSHADOWBUF_OBJECT_H

#include <nsmath.h>
#include <nstexture.h>
#include <nsfb_object.h>

class nsgl_framebuffer;

class nsshadow_tex2d_target : public nsgl_framebuffer
{
  public:
	nsshadow_tex2d_target();
	~nsshadow_tex2d_target();

	void init();
};

class nsshadow_tex_cubemap_target : public nsgl_framebuffer
{
  public:
	nsshadow_tex_cubemap_target();
	~nsshadow_tex_cubemap_target();

	void init();
};
#endif
