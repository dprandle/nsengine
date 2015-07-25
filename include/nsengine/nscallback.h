/*!
  \file   nscallback.h
  \author  <DreamInColor@DAN-LAPTOP>
  \date   Tue Jul  7 00:46:14 2015
  
  \brief  Callback struct wrapper base class
  
*/

#ifndef NSCALLBACK_H
#define NSCALLBACK_H

#include <nsglobal.h>

struct NSCallback
{
	virtual void run() = 0;
};

struct NSSaveResCallback : public NSCallback
{
	NSSaveResCallback():
		resid(),
		saved(false)
	{}
	
	uivec2 resid;
	bool saved;
};

#endif
