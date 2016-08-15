/*! 
	\file nsfont_manager.h
	
	\brief Header file for nsfont_manager class

	This file contains all of the neccessary declarations for the nsfont_manager class.

	\author Daniel Randle
	\date Feb 20 2016
	\copywrite Earth Banana Games 2013
*/

#ifndef NSFONT_MANAGER_H
#define NSFONT_MANAGER_H

#include <asset/nsasset_manager.h>
#include <asset/nsfont.h>

nsfont * get_font(const uivec2 & id);

struct FT_LibraryRec_;
extern FT_LibraryRec_ * ftlib;

class nsfont_manager : public nsasset_manager
{
public:

	MANAGER_TEMPLATES
	
	nsfont_manager();
	~nsfont_manager();

	virtual nsfont * create(uint32 res_type_id, const nsstring & res_name, nsasset * to_copy=nullptr);

	virtual nsfont * load(uint32 res_type_id, const nsstring & fname, bool finalize_);
	
	nsfont * import(const nsstring & fname);

  private:
	
	bool _load_font_faces(nsfont * fnt);
	
};

#endif
