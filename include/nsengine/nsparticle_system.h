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

class NSScene;
class NSSelComp;
class NSParticleProcessShader;

class NSParticleSystem : public NSSystem
{
public:

	NSParticleSystem();

	~NSParticleSystem();

	virtual void draw();

	virtual int32 drawPriority();

	virtual int32 updatePriority();

//	virtual bool handleEvent(NSEvent * pEvent);

	virtual void init();

	uint32 finalfbo();

	void setFinalfbo(uint32 fbo);

	virtual void update();

	void setShader(NSParticleProcessShader * shader) { mDefaultShader = shader; }

private:
	NSParticleProcessShader * mDefaultShader;
	uint32 mFinalBuf;
};

#endif
