/*!
\file nstimer.cpp

\brief Definition file for nstimer class

This file contains all of the neccessary definitions for the nstimer class.

\author Daniel Randle
\date December 17 2013
\copywrite Earth Banana Games 2013
*/

#include <nstimer.h>
#include <nsengine.h>
#include <nsplatform.h>
#include <iostream>

nstimer::nstimer() :
	m_start(0.0),
	m_current(0.0),
	m_last(0.0),
	m_running(false)
{
}

nstimer::~nstimer()
{}

double nstimer::dt()
{
	return (m_current - m_last);
}

double nstimer::fixed()
{
    return FIXED_TIME_STEP;
}

double nstimer::elapsed()
{
	return (m_current - m_start);
}

void nstimer::pause()
{
	m_running = false;
}

void nstimer::resume()
{
	m_running = true;
	update();
	m_start += (m_current - m_last);
}

void nstimer::start()
{
	m_start = platform::system_time();
	m_current = m_start;
	m_last = m_start;
	m_running = true;
}

void nstimer::update()
{
	m_last = m_current;
	if (m_running)
		m_current = platform::system_time();
}
