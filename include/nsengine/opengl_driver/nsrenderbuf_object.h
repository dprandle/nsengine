#ifndef NSRENDERBUF_OBJECT_H
#define NSRENDERBUF_OBJECT_H

#include <nsgl_object.h>

class nsgl_renderbuffer : public nsgl_object
{
  public:

	nsgl_renderbuffer();

	~nsgl_renderbuffer();

	void allocate();

	void bind() const;

	int32 internal_format() const;

	const uivec2 & size();

	uint32 multisample() const;

	void video_init();

	void video_release();

	void resize(uint32 w_, uint32 h_);

	void resize(const uivec2 & size_);

	void set_multisample(uint32 numsamples);

	void set_internal_format(int32 internal_format_);

	void unbind() const;

  private:
	uint32 m_sample_number;
	int32 m_internal_format;
	uivec2 m_size;
	bool m_allocated;
};


#endif