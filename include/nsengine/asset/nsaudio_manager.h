/*! 
	\file nsaudio_manager.h
	
	\brief Header file for nsaudio_manager class

	This file contains all of the neccessary declarations for the nsaudio_manager class.

	\author Daniel Randle
	\date November 23 2013
	\copywrite Earth Banana Games 2013
*/

#ifndef NSAUDIO_MANAGER_H
#define NSAUDIO_MANAGER_H

#include <nsasset_manager.h>
#include <nsaudio_source.h>

class nsaudio_manager : public nsasset_manager
{
public:

	MANAGER_TEMPLATES
	
	nsaudio_manager();
	~nsaudio_manager();
	
	nsaudio_clip * load(uint32 res_type_id, const nsstring & fname, bool finalize_);
	
	bool save(nsasset * clip, const nsstring & path);

};

#endif
