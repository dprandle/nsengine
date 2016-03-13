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

#define DEFAULT_PROCESS_PARTICLE_SHADER "xfbparticle"

#include <nssystem.h>

class nsscene;
class nssel_comp;
class nsparticle_process_shader;

class nsparticle_system : public nssystem
{
public:

	nsparticle_system();

	~nsparticle_system();

	virtual int32 update_priority();

	virtual void init();

	virtual void update();

	void set_process_shader(nsparticle_process_shader * shader) { m_process_shader = shader; }

private:
	nsparticle_process_shader * m_process_shader;
};

#endif
