#ifndef NSSHADOWBUF_OBJECT_H
#define NSSHADOWBUF_OBJECT_H

#include <math/nsmath.h>
#include <asset/nstexture.h>
#include <opengl/nsgl_framebuffer.h>

struct nsgl_framebuffer;

struct nsshadow_tex2d_target : public nsgl_framebuffer
{
  public:
	nsshadow_tex2d_target();
	~nsshadow_tex2d_target();

	void init(const nsstring & tex_name);
};

struct nsshadow_tex_cubemap_target : public nsgl_framebuffer
{
  public:
	nsshadow_tex_cubemap_target();
	~nsshadow_tex_cubemap_target();

	void init(const nsstring & tex_name);
};
#endif
