/*! 
	\file nssprite_comp.h
	
	\brief Header file for nssprite_comp class

	This file contains all of the neccessary declarations for the nssprite_comp class.

	\author Daniel Randle
	\date December 11 2013
	\copywrite Earth Banana Games 2013
*/

#ifndef NSSPRITE_COMP_H
#define NSSPRITE_COMP_H

#include <component/nscomponent.h>
#include <signal.h>

class nstex2d;

struct anim_frame
{
	anim_frame():
		tc(),
		audio_source_ind(0),
		emit_signal(false)
	{}
	
	fvec4 tc;
	uint32 audio_source_ind;
	bool emit_signal;
};

struct sprite_animation;

struct anim_frame_sound_info
{
	anim_frame_sound_info(nsentity * ent_=nullptr, sprite_animation * anim_=nullptr, uint32 frame_ind_=0):
		ent(ent_),
		anim(anim_),
		frame_ind(frame_ind_)
	{}
	
	nsentity * ent;
	sprite_animation * anim;
	uint32 frame_ind;
};

struct sprite_animation
{
	sprite_animation();
	
	nsstring name;
	uint32 hashed_name;
	nstex2d * tex;
	std::vector<anim_frame> frames;
	float duration;
	uint32 current_frame;
	ns::signal<const anim_frame_sound_info &> next_frame;
};

class nssprite_sheet_comp : public nscomponent
{
public:

	template <class PUPer>
	friend void pup(PUPer & p, nssprite_sheet_comp & tc);

	nssprite_sheet_comp();
	nssprite_sheet_comp(const nssprite_sheet_comp & rhs);

	virtual ~nssprite_sheet_comp();

	virtual nssprite_sheet_comp * copy(const nscomponent * comp_);

	virtual void init();

	virtual void pup(nsfile_pupper * p);

	nssprite_sheet_comp & operator=(nssprite_sheet_comp rhs_);

	float elapsed;

	std::vector<sprite_animation*> animations;

	uint32 current_anim;
	bool loop;
	bool playing;
};

template <class PUPer>
void pup(PUPer & p, nssprite_sheet_comp & tc)
{
	// do nothing for now
}

#endif
