#include <nsrenderbuf_object.h>
#include <nsgl_context.h>
#include <myGL/glew.h>

nsrenderbuf_object::nsrenderbuf_object() :
m_sample_number(0),
m_internal_format(GL_RGBA8),
m_size(0),
m_allocated(false)
{}

nsrenderbuf_object::~nsrenderbuf_object()
{}

void nsrenderbuf_object::allocate()
{
	// Note that the render buffer must be bound for this to work
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, m_sample_number, m_internal_format, m_size.w, m_size.h);
	m_allocated = !GLError("nsrenderbuf_object::allocate");
}

void nsrenderbuf_object::bind()
{
	glBindRenderbuffer(GL_RENDERBUFFER, m_gl_name);
	GLError("nsrenderbuf_object::bind");
}

void nsrenderbuf_object::init_gl()
{
	glGenRenderbuffers(1, &m_gl_name);
	GLError("nsrenderbuf_object::initGL");
}

void nsrenderbuf_object::release()
{
	glDeleteRenderbuffers(1, &m_gl_name);
	GLError("nsrenderbuf_object::release");
	m_gl_name = 0;
	m_allocated = false;
}

int32 nsrenderbuf_object::internal_format() const
{
	return m_internal_format;
}

const uivec2 & nsrenderbuf_object::size()
{
	return m_size;
}

uint32 nsrenderbuf_object::multisample() const
{
	return m_sample_number;
}

void nsrenderbuf_object::set_multisample(uint32 numsamples)
{
	m_sample_number = numsamples;
}


void nsrenderbuf_object::resize(const uivec2 & size_)
{
	resize(size_.w, size_.h);
}

void nsrenderbuf_object::resize(uint32 w, uint32 h)
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

void nsrenderbuf_object::set_internal_format(int32 pInternalFormat)
{
	if (m_allocated)
		return;

	m_internal_format = pInternalFormat;
}

void nsrenderbuf_object::unbind()
{
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	GLError("nsfb_object::unbind");
}
