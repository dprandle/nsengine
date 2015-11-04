
#include <nsmath.h>

float clampf(float pVal, const float & pMin, const float & pMax)
{
	if (pVal < pMin)
		pVal = pMin;
	if (pVal > pMax)
		pVal = pMax;
	return pVal;
}

double clamp(double pVal, const double & pMin, const double & pMax)
{
	if (pVal < pMin)
		pVal = pMin;
	if (pVal > pMax)
		pVal = pMax;
	return pVal;
}

float fractf(const float & num)
{
	float flr;
	if (num >= 0)
		flr = std::floor(num);
	else
		flr = std::ceil(num);
	return num - flr;
}

double fract(const double & num)
{
	double flr;
	if (num >= 0)
		flr = std::floor(num);
	else
		flr = std::ceil(num);
	return num - flr;
}

float lerp(float low, float high, float middle)
{
	return (middle - low) / (high - low);
}


double lerp(double low, double high, double middle)
{
	return (middle - low) / (high - low);
}

float lerp(int32 low, int32 high, int32 middle)
{
	return float(middle - low) / float(high - low);
}

float lerp(uint32 low, uint32 high, uint32 middle)
{
	return float(middle - low) / float(high - low);
}

#define FLOAT_EPS 0.00001

/*!
Calculates the box given a set of vertices.. if no vertices are given then will set everything to zero.
*/
nsbounding_box::nsbounding_box(
	const std::vector<fvec3> & pVertices
	)
{
	calculate(pVertices);
}

/*!
Find the min and max of a set of vertices and use that to make the bounding box.
*/
void nsbounding_box::calculate(
	const std::vector<fvec3> & pVertices,
	const fmat4 & pTransform
	)
{
	clear();
	extend(pVertices, pTransform);
}

/*!
Returns the center of the box or the center of a face of the box if pFace is specified as something
other than None.
*/
fvec3 nsbounding_box::center(const box_face & pFace)
{
	fvec3 center;
	switch (pFace)
	{
	case (f_none) :
		center.set(dx() / 2.0f, dy() / 2.0f, dz() / 2.0f);
		break;
	case (f_bottom) :
		center.set(dx() / 2.0f, dy() / 2.0f, mMin.z);
		break;
	case (f_top) :
		center.set(dx() / 2.0f, dy() / 2.0f, mMax.z);
		break;
	case (f_left) :
		center.set(mMin.x, dy() / 2.0f, dz() / 2.0f);
		break;
	case (f_right) :
		center.set(mMax.x, dy() / 2.0f, dz() / 2.0f);
		break;
	case (f_back) :
		center.set(dx() / 2.0f, mMin.y, dz() / 2.0f);
		break;
	case (f_front) :
		center.set(dx() / 2.0f, mMax.y, dz() / 2.0f);
		break;
	}
	return center;
}

/*!
Clears the verts and the min/max to 0.
*/
void nsbounding_box::clear()
{
	mMin = fvec3();
	mMax = fvec3();
	for (uint32 i = 0; i < 8; ++i)
		mVerts[i] = fvec3();
}

/*!
Length of box in x direction.
*/
float nsbounding_box::dx()
{
	return mMax.x - mMin.x;
}

/*!
Length of box in y direction.
*/
float nsbounding_box::dy()
{
	return mMax.y - mMin.y;
}

/*!
Length of box in z direction.
*/
float nsbounding_box::dz()
{
	return mMax.z - mMin.z;
}

void nsbounding_box::extend(
	const std::vector<fvec3> & pVertices,
	const fmat4 & pTransform
	)
{
	for (uint32 i = 0; i < pVertices.size(); ++i)
	{
		fvec3 tVert = (pTransform * fvec4(pVertices[i], 1.0f)).xyz();
		// Find maximum of each dimension
		if (tVert.x > mMax.x)
			mMax.x = tVert.x;
		if (tVert.y > mMax.y)
			mMax.y = tVert.y;
		if (tVert.z > mMax.z)
			mMax.z = tVert.z;

		// Find minimum of each dimension
		if (tVert.x < mMin.x)
			mMin.x = tVert.x;
		if (tVert.y < mMin.y)
			mMin.y = tVert.y;
		if (tVert.z < mMin.z)
			mMin.z = tVert.z;
	}
	_update_verts();
}

/*!
Set the min and max - will update the verts based on this new min and max.
*/
void nsbounding_box::set(
	const fvec3 & pMin,
	const fvec3 pMax
	)
{
	mMin = pMin;
	mMax = pMax;
	_update_verts();
}

/*!
The volume in whatever units the world is represented in. The cartesian coordinate x = 1, y = 1, z = 1 would
represent a point that is 1 unit away from each axis and 1.41 units away from the origin.
*/
float nsbounding_box::volume()
{
	return (dx() * dy() * dz());
}

void nsbounding_box::_update_verts()
{
	mVerts[0] = mMin;
	mVerts[1] = fvec3(mMax.x, mMin.y, mMin.z);
	mVerts[2] = fvec3(mMin.x, mMax.y, mMin.z);
	mVerts[3] = fvec3(mMax.x, mMax.y, mMin.z);
	mVerts[4] = fvec3(mMin.x, mMin.y, mMax.z);
	mVerts[5] = fvec3(mMax.x, mMin.y, mMax.z);
	mVerts[6] = fvec3(mMin.x, mMax.y, mMax.z);
	mVerts[7] = mMax;
}

float random_float(float pHigh, float pLow)
{
	return pLow + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (pHigh - pLow)));
}
