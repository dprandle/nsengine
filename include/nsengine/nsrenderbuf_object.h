#ifndef NSRENDERBUF_OBJECT_H
#define NSRENDERBUF_OBJECT_H

#include <nsgl_object.h>
#include <nsmath.h>

class nsrenderbuf_object : public nsgl_object
{
  public:

	nsrenderbuf_object();

	~nsrenderbuf_object();

	void allocate();

	void bind();

	int32 internal_format() const;

	const uivec2 & size();

	uint32 multisample() const;

	void init_gl();

	void release();

	void resize(uint32 w_, uint32 h_);

	void resize(const uivec2 & size_);

	void set_multisample(uint32 numsamples);

	void set_internal_format(int32 internal_format_);

	void unbind();

  private:
	uint32 m_sample_number;
	int32 m_internal_format;
	uivec2 m_size;
	bool m_allocated;
};


#endif
