/*!
\file nsparticlesystem.h

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
#include <nstformcomp.h>

class NSScene;
class NSSelComp;
class NSParticleProcessShader;

class NSParticleSystem : public NSSystem
{
public:

	NSParticleSystem();

	~NSParticleSystem();

	virtual void draw();

	virtual float drawPriority();

	virtual float updatePriority();

	virtual bool handleEvent(NSEvent * pEvent);

	virtual void init();

	nsuint finalfbo();

	void setFinalfbo(nsuint fbo);

	virtual void update();

	void setShader(NSParticleProcessShader * shader) { mDefaultShader = shader; }

	virtual nsstring typeString() { return getTypeString(); }

	static nsstring getTypeString();

private:
	NSParticleProcessShader * mDefaultShader;
	nsuint mFinalBuf;
};

#endif