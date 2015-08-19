/*!
\file nsmovement_system.h

\brief Header file for nsmovement_system class

This file contains all of the neccessary declarations for the nsmovement_system class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#ifndef NSMOVEMENT_SYSTEM_H
#define NSMOVEMENT_SYSTEM_H

#include <nssystem.h>

class nsmovement_system : public nssystem
{
public:

	nsmovement_system();

	~nsmovement_system();

	virtual void init();

	virtual void update();

	virtual int32 update_priority();
};


#endif
