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

#define FIXED_TIME_STEP 1/60.0

class nstimer 
{
public:
	nstimer();
	~nstimer();

	double dt();

	double elapsed();

	double fixed();

	float & lag();

	void pause();

	void resume();

	void start();

	void update();

private:
	double m_start;
	double m_current;
	double m_last;
	bool m_running;
};
#endif
