/*!
\file nsoccupycomp.h

\brief Header file for NSOccupyComp class

This file contains all of the neccessary declarations for the NSOccupyComp class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#ifndef NSOCCUPYCOMP_H
#define NSOCCUPYCOMP_H

#include <nscomponent.h>
#include <nsmath.h>

class NSOccupyComp : public NSComponent
{
public:

	template <class PUPer>
	friend void pup(PUPer & p, NSOccupyComp & oc);

	NSOccupyComp();

	virtual ~NSOccupyComp();

	bool add(int x, int y, int z);

	bool add(const ivec3 & pGridPos);

	ivec3array::iterator begin();

	void build(const NSBoundingBox & pBox);

	bool contains(int x, int y, int z);

	bool contains(const ivec3 & pGridPos);

	void clear();

	virtual NSOccupyComp * copy(const NSComponent* pComp);

	void enableDraw(bool pEnable);

	ivec3array::iterator end();

	const ivec3array & spaces() const;

	virtual void init();

	bool drawEnabled();

	const uivec2 & matid();

	const uivec2 & meshid();

	virtual void nameChange(nsuint plugID, nsuint oldID, nsuint newID);

	virtual void pup(NSFilePUPer * p);

	bool remove(int x, int y, int z);

	bool remove(const ivec3 & pGridPos);

	virtual uivec2array resources();

	void setMeshID(const uivec2 & mesh);

	void setMatID(const uivec2 & mat);

	NSOccupyComp & operator=(const NSOccupyComp & pRHSComp);

	static nsstring getTypeString();

	virtual nsstring typeString() { return getTypeString(); }

private:
	ivec3array mSpaces;
	uivec2 mMeshID;
	uivec2 mMatID;
	bool mDrawEnabled;
};

template <class PUPer>
void pup(PUPer & p, NSOccupyComp & oc)
{
	pup(p, oc.mSpaces, "spaces");
	pup(p, oc.mMeshID, "meshID");
	pup(p, oc.mMatID, "matID");
	pup(p, oc.mDrawEnabled, "drawEnabled");
}

#endif