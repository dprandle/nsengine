#ifndef NSSHADER_H
#define NSSHADER_H

#include <nsresource.h>
#include <nsvideo_driver.h>

class nsshader : public nsresource, public nsvideo_object
{
public:

	template <class PUPer>
	friend void pup(PUPer & p, nsshader & shader);

	struct shader_stage
	{
		nsstring name;
		nsstring source;
	};

	nsshader();

	nsshader(const nsshader & copy_);

	~nsshader();

	nsshader & operator=(nsshader rhs);

	void init() {}

	void video_context_init();

	virtual void pup(nsfile_pupper * p);

	std::vector<shader_stage> shader_stages;
};

template <class PUPer>
void pup(PUPer & p, nsshader::shader_stage & stage, const nsstring & var_name)
{
	pup(p, stage.name, var_name + ".name");
	pup(p, stage.source, var_name + ".source");
}

template <class PUPer>
void pup(PUPer & p, nsshader & shader)
{
	pup(p, shader.shader_stages, "stage");
}
#endif
