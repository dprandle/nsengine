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

#define FIXED_TIME_STEP 1/100.0

#include <nstimer.h>
#include <nscallback.h>

class nstimer;

double system_time();

struct nstimer_callback
{
    nstimer_callback();
    virtual ~nstimer_callback();
    virtual void run() = 0;
    nstimer * timer;
};

class nstimer
{
public:

    enum cb_mode {
        no_shot,
        single_shot,
        continous_shot
    };

    nstimer();
    ~nstimer();

    double dt();
    double elapsed();
    double elapsed_since_callback();
    bool running();

	double fixed();

    void set_callback(nstimer_callback * cb);
    void set_callback_mode(cb_mode mode);
    void set_callback_delay(double ms);

    nstimer_callback * callback();
    cb_mode callback_mode();
    double callback_delay();

    void stop();
    void start();
    void update();

private:
    double m_start;
    double m_current;
    double m_last;
    double m_last_cb;
    double m_cb_delay;
    bool m_running;
    nstimer_callback * m_cb;
    cb_mode m_cmode;
};
#endif
