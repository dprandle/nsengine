/*!
\file nsparticle_system.h

\brief Header file for NSParticleSystem class

This file contains all of the neccessary declarations for the NSParticleSystem class.

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
class NSSelComp;
class nsparticle_process_shader;

class NSParticleSystem : public NSSystem
{
public:

	NSParticleSystem();

	~NSParticleSystem();

	virtual void draw();

	virtual int32 draw_priority();

	virtual int32 update_priority();

//	virtual bool handleEvent(NSEvent * pEvent);

	virtual void init();

	uint32 finalfbo();

	void setFinalfbo(uint32 fbo);

	virtual void update();

	void setShader(nsparticle_process_shader * shader) { mDefaultShader = shader; }

private:
	nsparticle_process_shader * mDefaultShader;
	uint32 mFinalBuf;
};

#endif
