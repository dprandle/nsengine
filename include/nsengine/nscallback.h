/*!
  \file   nscallback.h
  \author  <DreamInColor@DAN-LAPTOP>
  \date   Tue Jul  7 00:46:14 2015
  
  \brief  Callback struct wrapper base class
  
*/

#ifndef NSCALLBACK_H
#define NSCALLBACK_H

#include <math/nsmath.h>

struct nscallback
{
	virtual void run() = 0;
};

struct nssave_resouces_callback : public nscallback
{
	nssave_resouces_callback():
		res_id(),
		saved(false)
	{}
	
	uivec2 res_id;
	bool saved;
};

#endif
