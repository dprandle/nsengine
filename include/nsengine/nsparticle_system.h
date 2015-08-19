/*!
\file nsparticle_system.h

\brief Header file for nsparticle_system class

This file contains all of the neccessary declarations for the nsparticle_system class.

\author Daniel Randle
\date March 8 2014
\copywrite Earth Banana Games 2013
*/

#ifndef NSPARTICLESYSTEM_H
#define NSPARTICLESYSTEM_H

#include <nssystem.h>
#include <nsglobal.h>
#include <nsevent.h>
#include <nstform_comp.h>

class nsscene;
class nssel_comp;
class nsparticle_process_shader;

class nsparticle_system : public nssystem
{
public:

	nsparticle_system();

	~nsparticle_system();

	virtual void draw();

	virtual int32 draw_priority();

	virtual int32 update_priority();

	virtual void init();

	uint32 final_fbo();

	void set_final_fbo(uint32 fbo);

	virtual void update();

	void set_process_shader(nsparticle_process_shader * shader) { m_process_shader = shader; }

private:
	nsparticle_process_shader * m_process_shader;
	uint32 m_final_buf;
};

#endif
