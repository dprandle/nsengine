/*!
\file nstimer.h

\brief Header file for NSTimer class

This file contains all of the neccessary declarations for the NSTimer class.

\author Daniel Randle
\date December 17 2013
\copywrite Earth Banana Games 2013
*/

#ifndef NSTIMER_H
#define NSTIMER_H
#include <chrono>
#include <nsglobal.h>

class NSTimer 
{
public:
	NSTimer();
	~NSTimer();

	nsfloat dt();

	nsfloat elapsed();

	nsfloat fixed();

	nsfloat & lag();

	void pause(bool pPause);

	void reset();

	void start();

	void update();

private:
	std::chrono::time_point<std::chrono::high_resolution_clock> mStart;
	std::chrono::time_point<std::chrono::high_resolution_clock> mCurrent;
	std::chrono::time_point<std::chrono::high_resolution_clock> mLast;
	std::chrono::high_resolution_clock mTimer;
	bool mRunning;
	nsfloat mLag;
};
#endif