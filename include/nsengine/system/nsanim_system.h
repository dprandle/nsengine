/*!
\file nsanimsystem.h

\brief Header file for nsanim_system class

This file contains all of the neccessary declarations for the nsanim_system class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#ifndef NSANIM_SYSTEM_H
#define NSANIM_SYSTEM_H

#include <system/nssystem.h>

class nsanim_system : public nssystem
{
public:

	nsanim_system();

	~nsanim_system();

	void init();

	void release();

	void update();

	int32 update_priority();

};


#endif
