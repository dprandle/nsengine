#ifndef NSSHADER_H
#define NSSHADER_H

#include "nsasset.h"
#include "nsvideo_driver.h"

struct shader_stage_info
{
	shader_stage_info(const nsstring & name_="", const nsstring & source_=""):
		name(name_),
		source(source_)
	{}
		
	nsstring name;
	nsstring source;
};

class nsshader : public nsasset, public nsvideo_object
{
public:

	template <class PUPer>
	friend void pup(PUPer & p, nsshader & shader);

	nsshader();

	nsshader(const nsshader & copy_);

	~nsshader();

	nsshader & operator=(nsshader rhs);

	void init() {}

	void video_context_init();

	virtual void pup(nsfile_pupper * p);

	std::vector<shader_stage_info> shader_stages;
};

template <class PUPer>
void pup(PUPer & p, shader_stage_info & stage, const nsstring & var_name)
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
