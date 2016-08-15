/*! 
	\file nsanim_comp.h
	
	\brief Header file for nsanim_comp class

	This file contains all of the neccessary declarations for the nsanim_comp class.

	\author Daniel Randle
	\date November 23 2013
	\copywrite Earth Banana Games 2013
*/

#ifndef NSANIM_COMP_H
#define NSANIM_COMP_H

#include <component/nscomponent.h>
#include <asset/nsanim_set.h>
#include <asset/nsmesh.h>

class nstimer;


class nsanim_comp : public nscomponent
{
public:
	nsanim_comp();

	nsanim_comp(const nsanim_comp & cpy);	

	~nsanim_comp();

	template <class PUPer>
	friend void pup(PUPer & p, nsanim_comp & anim);

	float & elapsed();

	void fill_joints(nsmesh::node_tree * node_tree_, animation_data * current_anim_);

	const uivec2 & anim_set_id();

	const nsstring & current_anim_name();

	fmat4_vector * final_transforms();

	virtual void name_change(const uivec2 & old_id_, const uivec2 new_id_);

	/*!
	Get the resources that the component uses. For the animation component that is simply an AnimSet
	/return Map of resource ID to resource type containing AnimSet used
	*/
	virtual uivec3_vector resources();

	void init();

	bool animating() const;

	bool looping() const;

	virtual void pup(nsfile_pupper * p);

	void set_anim_set_id(uint32 plug_id_, uint32 res_id_)
	{
		m_anim_set_id.x = plug_id_; m_anim_set_id.y = res_id_;
		post_update(true);
	}

	void set_anim_set_id(const uivec2 & anim_set_id_);

	void set_animate(bool animate_);

	void set_loop(bool loop_);

	void set_current_animation(const nsstring & anim_name_);

	nsanim_comp & operator=(nsanim_comp rhs_);

private:
	void _fill_joint_transforms(nsmesh::node_tree * node_tree_, nsmesh::node * node_, animation_data * current_anim_, fmat4 & parent_tform_);

	float m_elapsed_time;
	
	bool m_animation;
	bool m_looping;

	uivec2 m_anim_set_id; //! AnimSet resource used
	nsstring m_current_anim; //! Current animation within the animation set
	fmat4_vector m_final_transforms;
};

template <class PUPer>
void pup(PUPer & p, nsanim_comp & anim)
{
	pup(p, anim.m_elapsed_time, "elapsed_time");
	pup(p, anim.m_animation, "animating");
	pup(p, anim.m_looping, "looping");
	pup(p, anim.m_anim_set_id, "anim_set_id");
	pup(p, anim.m_current_anim, "current_anim");
}

#endif
