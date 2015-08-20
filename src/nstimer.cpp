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

nstimer::nstimer() : m_start(), m_current(), m_last(), m_lag(0.0f), m_timer(), m_running(false)
{
	reset();
}

nstimer::~nstimer()
{}

float nstimer::dt()
{
	std::chrono::duration<float> dt = m_current - m_last;
	return dt.count();
}

float nstimer::fixed()
{
	return FIXED_TIME_STEP;
}

float nstimer::elapsed()
{
	std::chrono::duration<float> elapsed = m_current - m_start;
	return elapsed.count();
}

float & nstimer::lag()
{
	return m_lag;
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

void nstimer::reset()
{
	m_start = m_timer.now();
	m_current = m_timer.now();
	m_last = m_timer.now();
}

void nstimer::start()
{
	m_start = m_timer.now();
	m_current = m_start;
	m_running = true;
}

void nstimer::update()
{
	m_last = m_current;
	if (m_running)
		m_current = m_timer.now();
	m_lag += dt();
}
