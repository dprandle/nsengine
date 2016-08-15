#ifndef NSRENDERBUF_OBJECT_H
#define NSRENDERBUF_OBJECT_H

#include <opengl/nsgl_object.h>

struct nsgl_renderbuffer : public nsgl_obj
{
	nsgl_renderbuffer();

	~nsgl_renderbuffer();

	void allocate();

	void bind();

	void init();

	void release();

	void resize(uint32 w_, uint32 h_);

	void resize(const uivec2 & size_);

	void unbind();

	uint32 sample_num;
	int32 internal_format;
	uivec2 size;
	bool allocated;
};


#endif
