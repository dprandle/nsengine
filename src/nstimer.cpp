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

nstimer_callback::nstimer_callback():
    timer(nullptr)
{}

nstimer_callback::~nstimer_callback() {}

nstimer::nstimer() :
    m_start(0.0),
    m_current(0.0),
    m_last(0.0),
    m_last_cb(0.0),
    m_cb_delay(0.0),
    m_running(false),
    m_cb(nullptr),
    m_cmode(no_shot)
{}

nstimer::~nstimer()
{
    if (m_cb != nullptr)
        delete m_cb;
}

double nstimer::dt()
{
    return (m_current - m_last);
}

double nstimer::elapsed()
{
    return (m_current - m_start);
}

void nstimer::stop()
{
    update();
    m_running = false;
}

void nstimer::start()
{
    m_start = platform::system_time();
    m_current = m_start;
    m_last = m_start;
    m_last_cb = m_start;
    m_running = true;
}

void nstimer::update()
{
    if (!m_running)
        return;

    m_last = m_current;
    m_current = platform::system_time();

    double cb_elapsed = elapsed_since_callback();
    if (cb_elapsed >= m_cb_delay)
    {
        if ((m_cmode == single_shot && m_last_cb == m_start) || (m_cmode == continous_shot))
        {
            if (m_cb != nullptr)
                m_cb->run();
            else
                dprint("nstimer::udpate - callback function set to null");
            m_last_cb = m_current;
        }
    }
}

double nstimer::elapsed_since_callback()
{
    return m_current - m_last_cb;
}

nstimer_callback * nstimer::callback()
{
    return m_cb;
}

nstimer::cb_mode nstimer::callback_mode()
{
    return m_cmode;
}

void nstimer::set_callback(nstimer_callback * cb)
{
    if (m_cb != nullptr)
        delete m_cb;
    m_cb = cb;
    if (m_cb != nullptr)
        m_cb->timer = this;
}

double nstimer::callback_delay()
{
    return m_cb_delay;
}

void nstimer::set_callback_mode(cb_mode mode)
{
    m_cmode = mode;
    m_last_cb = m_last;
}

void nstimer::set_callback_delay(double secs)
{
    m_cb_delay = secs;
}

bool nstimer::running()
{
    return m_running;
}

double nstimer::fixed()
{
    return FIXED_TIME_STEP;
}
