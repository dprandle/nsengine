/*!
\file nstimer.cpp

\brief Definition file for NSTimer class

This file contains all of the neccessary definitions for the NSTimer class.

\author Daniel Randle
\date December 17 2013
\copywrite Earth Banana Games 2013
*/

#include <nstimer.h>

NSTimer::NSTimer() : mStart(), mCurrent(), mLast(), mLag(0.0f), mTimer(), mRunning(false)
{
	reset();
}

NSTimer::~NSTimer()
{}

float NSTimer::dt()
{
	std::chrono::duration<float> dt = mCurrent - mLast;
	return dt.count();
}

float NSTimer::fixed()
{
	return FIXED_TIME_STEP;
}

float NSTimer::elapsed()
{
	std::chrono::duration<float> elapsed = mCurrent - mStart;
	return elapsed.count();
}

float & NSTimer::lag()
{
	return mLag;
}

void NSTimer::pause(bool pPause)
{
	mRunning = !pPause;
	if (mRunning)
	{
		update();
		mStart += (mCurrent - mLast);
	}
}

void NSTimer::reset()
{
	mStart = mTimer.now();
	mCurrent = mTimer.now();
	mLast = mTimer.now();
}

void NSTimer::start()
{
	mStart = mTimer.now();
	mCurrent = mStart;
	mRunning = true;
}

void NSTimer::update()
{
	mLast = mCurrent;
	if (mRunning)
		mCurrent = mTimer.now();
	mLag += dt();
}
