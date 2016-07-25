#include <nsgl_renderbuffer.h>
#include <nsengine.h>
#include <nsgl_driver.h>

nsgl_renderbuffer::nsgl_renderbuffer() :
	sample_num(0),
	internal_format(GL_RGBA8),
	size(0),
	allocated(false)
{}

nsgl_renderbuffer::~nsgl_renderbuffer()
{}

void nsgl_renderbuffer::allocate()
{
	// Note that the render buffer must be bound for this to work
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, sample_num, internal_format, size.w, size.h);
	allocated = !gl_err_check("nsgl_renderbuffer::allocate");
}

void nsgl_renderbuffer::bind()
{
	glBindRenderbuffer(GL_RENDERBUFFER, gl_id);
	gl_err_check("nsgl_renderbuffer::bind");
}

void nsgl_renderbuffer::init()
{
	if (gl_id != 0)
	{
		dprint("nsgl_renderbuffer::init Error trying to initialize already initialized render buffer");
		return;
	}
	glGenRenderbuffers(1, &gl_id);
	gl_err_check("nsgl_renderbuffer::init");
}

void nsgl_renderbuffer::release()
{
	glDeleteRenderbuffers(1, &gl_id);
	gl_id = 0;
	gl_err_check("nsgl_renderbuffer::release");
	allocated = false;
}

void nsgl_renderbuffer::resize(const uivec2 & size_)
{
	resize(size_.w, size_.h);
}

void nsgl_renderbuffer::resize(uint32 w, uint32 h)
{
	if (!allocated)
		size.set(w, h);
	else
	{
		release();
		size.set(w, h);
		init();
		bind();
		allocate();
		unbind();
	}
}

void nsgl_renderbuffer::unbind()
{
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	gl_err_check("nsgl_renderbuffer::unbind");
}
