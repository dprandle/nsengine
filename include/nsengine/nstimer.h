/*!
\file nstimer.h

\brief Header file for nstimer class

This file contains all of the neccessary declarations for the nstimer class.

\author Daniel Randle
\date December 17 2013
\copywrite Earth Banana Games 2013
*/

#ifndef NSTIMER_H
#define NSTIMER_H

#define FIXED_TIME_STEP 0.01667f

#include <chrono>

class nstimer 
{
public:
	nstimer();
	~nstimer();

	float dt();

	float elapsed();

	float fixed();

	float & lag();

	void pause();

	void resume();

	void reset();

	void start();

	void update();

private:
	std::chrono::time_point<std::chrono::high_resolution_clock> m_start;
	std::chrono::time_point<std::chrono::high_resolution_clock> m_current;
	std::chrono::time_point<std::chrono::high_resolution_clock> m_last;
	std::chrono::high_resolution_clock m_timer;
	bool m_running;
	float m_lag;
};
#endif
