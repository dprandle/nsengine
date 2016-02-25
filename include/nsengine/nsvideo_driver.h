/*!
  \file nsvideo_driver.h

  \brief Definition file for nsvideo_driver class

  This file contains all of the neccessary definitions for the nsvideo_driver class.

  \author Daniel Randle
  \date Feb 23 2016
  \copywrite Earth Banana Games 2013
*/

#ifndef NSVIDEO_DRIVER
#define NSVIDEO_DRIVER

class nsscene;
namespace nsrender
{
struct viewport;
}

class nsvideo_driver
{
  public:

	virtual void register_viewport(nsrender::viewport * vp) = 0;

	virtual void deregister_viewport(nsrender::viewport * vp) = 0;
	
	virtual void push_scene(nsscene * scn, nsrender::viewport * vp) = 0;

	virtual void render() = 0;
};

#endif
