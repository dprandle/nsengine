/*! 
	\file nsanim_set.h
	
	\brief Header file for nsanim_set class

	This file contains all of the neccessary declarations for the nsanim_set class.

	\author Daniel Randle
	\date December 11 2013
	\copywrite Earth Banana Games 2013
*/

#ifndef NSANIMSET_H
#define NSANIMSET_H

#define DEFAULT_TICKS_PER_SECOND 20

#include <nsresource.h>

class nsanim_set : public nsresource
{
public:
	struct animation_data
	{
		struct animation_node
		{
			animation_node();
			~animation_node();

			typedef f_fquat_map::iterator key_rotation_iter;
			typedef f_fvec3_map::iterator key_iter;

			
			f_fquat_map rotation_keys;
			f_fvec3_map translation_keys;
			f_fvec3_map scaling_keys;

			fmat4 transform(float time_, float ticks_per_second);

			nsstring name;
			fmat4 offset_matrix;
		};
		typedef std::map<nsstring, animation_node*>::iterator anim_nodemap_iter;

		animation_data();
		~animation_data();

		animation_node * create_node(const nsstring & node_name_);

		animation_node * anim_node(const nsstring & node_name_);
		fmat4 bone_transform(const nsstring & node_name_, float time_);

		std::map<nsstring,animation_node*> anim_node_map;
		nsstring animation_name;
		float duration;
		float ticks_per_second_;
	};

	typedef std::map<nsstring, animation_data*> animmap;
	typedef std::map<nsstring,animation_data*>::iterator animmap_iter;

	template<class PUPer>
	friend void pup(PUPer & p, nsanim_set & aset);

	nsanim_set();
	~nsanim_set();

	void clear();

	void init();

	animation_data * create_anim_data(const nsstring & anim_name_);

	animation_data * anim_data(const nsstring & anim_name_);

	fmat4 anim_bone_transform(const nsstring & anim_name_,
		const nsstring & node_name_, float time_);

	virtual void pup(nsfile_pupper * p);

private:
	animmap m_animmap;
};

template<class PUPer>
void pup(PUPer & p, nsanim_set::animation_data::animation_node & node, const nsstring & var_name)
{
	pup(p, node.name, var_name + ".name");
	pup(p, node.offset_matrix, var_name + ".offset_matrix");
	pup(p, node.scaling_keys, var_name + ".scaling_keys");
	pup(p, node.rotation_keys, var_name + ".rotation_keys");
	pup(p, node.translation_keys, var_name + ".translation_keys");
}

template<class PUPer>
void pup(PUPer & p, nsanim_set::animation_data::animation_node * & node, const nsstring & var_name)
{
	if (p.mode() == PUP_IN) // If reading in then we need to allocate memory for the node
		node = new nsanim_set::animation_data::animation_node();
	pup(p, *node, var_name);
}

template<class PUPer>
void pup(PUPer & p, nsanim_set::animation_data & data, const nsstring & var_name)
{
	pup(p, data.animation_name, var_name + ".animation_name");
	pup(p, data.ticks_per_second_, var_name + ".ticks_per_second");
	pup(p, data.duration, var_name + ".duration");
	pup(p, data.anim_node_map, var_name + ".anim_node_map");
}

template<class PUPer>
void pup(PUPer & p, nsanim_set::animation_data * & data, const nsstring & var_name)
{
	if (p.mode() == PUP_IN)
		data = new nsanim_set::animation_data();
	pup(p, *data, var_name);
}

template<class PUPer>
void pup(PUPer & p, nsanim_set & aset)
{
	pup(p, aset.m_animmap, "animmap");
}

#endif
