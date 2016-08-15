/*!
\file nstile_brush_comp.h

\brief Header file for nstile_brush_comp class

This file contains all of the neccessary declarations for the nstile_brush_comp class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#ifndef NSTILE_BRUSH_COMP_H
#define NSTILE_BRUSH_COMP_H

#include <component/nscomponent.h>

class nstile_brush_comp : public nscomponent
{
public:
	template <class PUPer>
	friend void pup(PUPer & p, nstile_brush_comp & tbc);

	nstile_brush_comp();

	nstile_brush_comp(const nstile_brush_comp & copy);

	virtual ~nstile_brush_comp();

	bool add(int32 x_, int32 y_);

	bool add(const ivec2 & grid_space_);

	ivec2_vector::iterator begin();

	void change_height(const int32 & amount_);

	bool contains(int32 x_, int32 y_);

	bool contains(const ivec2 & grid_space_);

	virtual void pup(nsfile_pupper * p);

	ivec2_vector::iterator end();

	int32 height() const;

	virtual void init();

	bool remove(int32 x_, int32 y_);

	bool remove(const ivec2 & grid_pos_);

	void set_height(const int32 & height_);

	nstile_brush_comp & operator=(nstile_brush_comp rhs_);

private:
	ivec2_vector m_brush;
	int32 m_height;
};

template <class PUPer>
void pup(PUPer & p, nstile_brush_comp & tbc)
{
	pup(p, tbc.m_brush, "brush");
	pup(p, tbc.m_height, "height");
}

#endif
