/*!
\file nstile_grid.h

\brief Header file for NSTileGrid class

This file contains all of the neccessary declarations for the NSTileGrid class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#ifndef NSTILEGRID_H
#define NSTILEGRID_H

#include <unordered_map>
#include <unordered_set>
#include <nsglobal.h>
#include <nsmath.h>

class NSTileGrid
{
public:
	typedef std::vector<uivec3array> Layer;
	typedef std::vector<Layer> Quadrant;
	typedef std::vector<Quadrant> Map;

	enum QuadrantIndex
	{
		TopRightFront,
		TopLeftFront,
		TopRightBack,
		TopLeftBack,
		BottomRightFront,
		BottomLeftFront,
		BottomRightBack,
		BottomLeftBack
	};

	struct GridBounds
	{
		ivec3 minSpace;
		ivec3 maxSpace;
	};

	struct Index
	{
		Index(uint32 pQuad = 0, uint32 pX = 0, uint32 pY = 0, uint32 pZ = 0) :
		mQuad(QuadrantIndex(pQuad)),
		mIndex(pX, pY, pZ)
		{}

		Index(uint32 pQuad, const uivec3 & pInd) :
			mQuad(QuadrantIndex(pQuad)),
			mIndex(pInd)
		{}


		QuadrantIndex mQuad;
		uivec3 mIndex;
	};

	NSTileGrid();
	~NSTileGrid();

	bool add(const uivec3 & pItem, const fvec3 & pPos);

	bool add(const uivec3 & pItem, const ivec3 & pSpace, const fvec3 & pOrigin = fvec3());

	bool add(const uivec3 pItem, const ivec3array & pSpaces, const fvec3 & pOrigin = fvec3());

	const uivec3 & at(const Index & pSpace) const;

	uivec3 get(const fvec3 & pPos) const;

	uivec3 get(const ivec3 & pSpace, const fvec3 & pOrigin = fvec3()) const;

	uivec3array boundedSet(const fvec3 & pPoint1, const fvec3 & pPoint2);

	GridBounds occupiedGridBounds();

	int32 minLayer();

	int32 maxLayer();

	int32 minY();

	int32 maxY();

	int32 minX();

	int32 maxX();

	bool occupied(const fvec3 & pPos) const;

	bool occupied(const ivec3 & pSpace, const fvec3 & pOrigin = fvec3()) const;

	bool occupied(const ivec3array & pSpaces, const fvec3 & pOrigin = fvec3()) const;

	bool remove(const fvec3 & pPos);

	bool remove(const ivec3 & pSpace, const fvec3 & pOrigin = fvec3());

	/*!
	Go through entire grid and remove any occurances that have this transform ID
	*/
	void remove(const uivec3 & id);

	/*!
	Go through entire grid and remove any occurances that have this entity ID
	*/
	void remove(const uivec2 & id);

	/*!
	Go through entire grid and remove any occurances that have this plugin ID
	*/
	void remove(uint32 plugid);

	bool remove(const ivec3array & pSpaces, const fvec3 & pOrigin = fvec3());

	void nameChange(const uivec2 & oldid, const uivec2 newid);

	static int32 indexX(float pX, bool pOffset);
	static int32 indexY(float pY);
	static int32 indexZ(float pZ);

	static ivec3 grid(const fvec3 & world);
	static ivec3 grid(const Index & index);

	static Index index(const ivec3 & grid);
	static Index index(const fvec3 & world);

	static fvec3 world(const Index & index);
	static fvec3 world(const ivec3 & grid, const fvec3 & pOrigin = fvec3());

	static void snap(fvec3 & world);

private:
	bool _checkBounds(const Index & pIndex) const;
	void _resizeForSpace(const Index & pIndex);
	const uivec3 & _getID(const Index & pIndex);

	Map mMap;
};


#endif
