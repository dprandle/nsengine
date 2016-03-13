#include <nsrenderbuf_object.h>
#include <myGL/glew.h>

nsgl_renderbuffer::nsgl_renderbuffer() :
m_sample_number(0),
m_internal_format(GL_RGBA8),
m_size(0),
m_allocated(false)
{}

nsgl_renderbuffer::~nsgl_renderbuffer()
{}

void nsgl_renderbuffer::allocate()
{
	// Note that the render buffer must be bound for this to work
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, m_sample_number, m_internal_format, m_size.w, m_size.h);
	m_allocated = !gl_err_check("nsrenderbuf_object::allocate");
}

void nsgl_renderbuffer::bind() const
{
	glBindRenderbuffer(GL_RENDERBUFFER, m_gl_name);
	gl_err_check("nsrenderbuf_object::bind");
}

void nsgl_renderbuffer::init_gl()
{
	glGenRenderbuffers(1, &m_gl_name);
	gl_err_check("nsrenderbuf_object::initGL");
}

void nsgl_renderbuffer::release()
{
	glDeleteRenderbuffers(1, &m_gl_name);
	gl_err_check("nsrenderbuf_object::release");
	m_gl_name = 0;
	m_allocated = false;
}

int32 nsgl_renderbuffer::internal_format() const
{
	return m_internal_format;
}

const uivec2 & nsgl_renderbuffer::size()
{
	return m_size;
}

uint32 nsgl_renderbuffer::multisample() const
{
	return m_sample_number;
}

void nsgl_renderbuffer::set_multisample(uint32 numsamples)
{
	m_sample_number = numsamples;
}


void nsgl_renderbuffer::resize(const uivec2 & size_)
{
	resize(size_.w, size_.h);
}

void nsgl_renderbuffer::resize(uint32 w, uint32 h)
{
	if (!m_allocated)
		m_size.set(w, h);
	else
	{
		release();
		m_size.set(w, h);
		init_gl();
		bind();
		allocate();
		unbind();
	}
}

void nsgl_renderbuffer::set_internal_format(int32 pInternalFormat)
{
	if (m_allocated)
		return;

	m_internal_format = pInternalFormat;
}

void nsgl_renderbuffer::unbind() const
{
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	gl_err_check("nsfb_object::unbind");
}
