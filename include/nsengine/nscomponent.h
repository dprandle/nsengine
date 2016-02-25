/*! 
  \file nscomponent.h
	
  \brief Header file for nscomponent class

  This file contains all of the neccessary declarations for the nscomponent class.

  \author Daniel Randle
  \date November 23 2013
  \copywrite Earth Banana Games 2013
*/

#ifndef NSCOMPONENT_H
#define NSCOMPONENT_H

#include <nspupper.h>
#include <nsrouter.h>

class nsentity;
class nsevent;
class nstimer;
class nscomponent_inst;

class nscomponent
{
  public:

	friend class nscomp_factory;

	nscomponent();

	nscomponent(const nscomponent & copy);

	virtual ~nscomponent();

	nsentity * owner();

	bool update_posted() const;

	virtual void init()=0;

	uint32 type();

	virtual void name_change(const uivec2 &, const uivec2);

	/*!
	  Get the resources that the component uses. If no resources are used then leave this unimplemented - will return an empty map.
	  /return Map of resource ID to resource type containing all used resources
	*/
	virtual uivec3_vector resources();

	virtual void pup(nsfile_pupper * p) = 0;

	virtual void post_update(bool update_);

	void set_owner(nsentity * owner_);

	nscomponent & operator=(nscomponent_inst rhs_);

  protected:
	nsentity * m_owner;
	uint32 m_hashed_type;
	bool m_update;
	nsrouter m_router;
};

class nscomponent_inst : public nscomponent
{
	nscomponent_inst(const nscomponent & rhs):nscomponent(rhs) {}
	void init() {std::terminate();}
 	void pup(nsfile_pupper *) {std::terminate();}
};


#endif
