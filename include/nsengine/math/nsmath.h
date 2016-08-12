#ifndef NSMATH_H
#define NSMATH_H

#define PI 3.14159265359f
#define EPS 0.0001f

#include <nstypes.h>
#include <cmath>
#include <stdexcept>
#include <nsstring.h>

float clampf(float val_, const float & min_, const float & max_);
double clamp(double val_, const double & min_, const double & max_);
float fractf(const float & num_);
double fract(const double & num_);

float lerp(int32 low_, int32 high_, int32 middle_);
float lerp(uint32 low_, uint32 high_, uint32 middle_);
float lerp(float low_, float high_, float middle_);
float lerp(float low_, float high_, float middle_);
double lerp(double low_, double high_, double middle_);

template<class T>
T degrees(const T & val_);

template<class T>
T radians(const T & val_);

float random_float(float high_ = 1.0f, float low_ = 0.0f);

#include <nsmat4.h>

// Math typedefs
typedef nsvec2<char> cvec2;
typedef nsvec2<char16> c16vec2;
typedef nsvec2<char32> c32vec2;
typedef nsvec2<wchar> wcvec2;
typedef nsvec2<int8> i8vec2;
typedef nsvec2<int16> i16vec2;
typedef nsvec2<int32> ivec2;
typedef nsvec2<int64> i64vec2;
typedef nsvec2<uint8> ui8vec2;
typedef nsvec2<uint16> ui16vec2;
typedef nsvec2<uint32> uivec2;
typedef nsvec2<uint64> ui64vec2;
typedef nsvec2<float> fvec2;
typedef nsvec2<double> vec2;
typedef nsvec2<ldouble> ldvec2;

typedef nsvec3<char> cvec3;
typedef nsvec3<char16> c16vec3;
typedef nsvec3<char32> c32vec3;
typedef nsvec3<wchar> wcvec3;
typedef nsvec3<int8> i8vec3;
typedef nsvec3<int16> i16vec3;
typedef nsvec3<int32> ivec3;
typedef nsvec3<int64> i64vec3;
typedef nsvec3<uint8> ui8vec3;
typedef nsvec3<uint16> ui16vec3;
typedef nsvec3<uint32> uivec3;
typedef nsvec3<uint64> ui64vec3;
typedef nsvec3<float> fvec3;
typedef nsvec3<double> vec3;
typedef nsvec3<ldouble> ldvec3;

typedef nsvec4<char> cvec4;
typedef nsvec4<char16> c16vec4;
typedef nsvec4<char32> c32vec4;
typedef nsvec4<wchar> wcvec4;
typedef nsvec4<int8> i8vec4;
typedef nsvec4<int16> i16vec4;
typedef nsvec4<int32> ivec4;
typedef nsvec4<int64> i64vec4;
typedef nsvec4<uint8> ui8vec4;
typedef nsvec4<uint16> ui16vec4;
typedef nsvec4<uint32> uivec4;
typedef nsvec4<uint64> ui64vec4;
typedef nsvec4<float> fvec4;
typedef nsvec4<double> vec4;
typedef nsvec4<ldouble> ldvec4;

typedef nsquat<char> cquat;
typedef nsquat<char16> c16quat;
typedef nsquat<char32> c32quat;
typedef nsquat<wchar> wcquat;
typedef nsquat<int8> i8quat;
typedef nsquat<int16> i16quat;
typedef nsquat<int32> iquat;
typedef nsquat<int64> i64quat;
typedef nsquat<uint8> ui8quat;
typedef nsquat<uint16> ui16quat;
typedef nsquat<uint32> uiquat;
typedef nsquat<uint64> ui64quat;
typedef nsquat<float> fquat;
typedef nsquat<double> quat;
typedef nsquat<ldouble> ldquat;

typedef nsmat2<char> cmat2;
typedef nsmat2<char16> c16mat2;
typedef nsmat2<char32> c32mat2;
typedef nsmat2<wchar> wcmat2;
typedef nsmat2<int8> i8mat2;
typedef nsmat2<int16> i16mat2;
typedef nsmat2<int32> imat2;
typedef nsmat2<int64> i64mat2;
typedef nsmat2<uint8> ui8mat2;
typedef nsmat2<uint16> ui16mat2;
typedef nsmat2<uint32> uimat2;
typedef nsmat2<uint64> ui64mat2;
typedef nsmat2<float> fmat2;
typedef nsmat2<double> mat2;
typedef nsmat2<ldouble> ldmat2;

typedef nsmat3<char> cmat3;
typedef nsmat3<char16> c16mat3;
typedef nsmat3<char32> c32mat3;
typedef nsmat3<wchar> wcmat3;
typedef nsmat3<int8> i8mat3;
typedef nsmat3<int16> i16mat3;
typedef nsmat3<int32> imat3;
typedef nsmat3<int64> i64mat3;
typedef nsmat3<uint8> ui8mat3;
typedef nsmat3<uint16> ui16mat3;
typedef nsmat3<uint32> uimat3;
typedef nsmat3<uint64> ui64mat3;
typedef nsmat3<float> fmat3;
typedef nsmat3<double> mat3;
typedef nsmat3<ldouble> ldmat3;

typedef nsmat4<char> cmat4;
typedef nsmat4<char16> c16mat4;
typedef nsmat4<char32> c32mat4;
typedef nsmat4<wchar> cwmat4;
typedef nsmat4<int8> i8mat4;
typedef nsmat4<int16> i16mat4;
typedef nsmat4<int32> imat4;
typedef nsmat4<int64> i64mat4;
typedef nsmat4<uint8> ui8mat4;
typedef nsmat4<uint16> ui16mat4;
typedef nsmat4<uint32> uimat4;
typedef nsmat4<uint64> ui64mat4;
typedef nsmat4<float> fmat4;
typedef nsmat4<double> mat4;
typedef nsmat4<ldouble> ldmat4;

template<class T>
T degrees(const T & val_)
{
	return (180 / PI) * val_;
}

template<class T>
T radians(const T & val_)
{
	return (PI / 180) * val_;
}

template<class T>
struct nsbox
{
	nsbox<T>(const nsvec3<T> & min_=nsvec3<T>(), const nsvec3<T> & max_=nsvec3<T>()):
	min(min_),
		max(max_)
	{}
	
	nsbox<T>(const nsbox<T> & copy):
		min(copy.min),
		max(copy.max)
	{}

	nsbox<T> & operator=(const nsbox<T> & rhs)
	{
		if (this != &rhs)
		min = rhs.min;
		max = rhs.max;
		return *this;
	}

	union
	{
		struct
		{
			nsvec3<T> min;
			nsvec3<T> max;
		};

		struct
		{
			nsvec3<T> a;
			nsvec3<T> b;
		};
	};
		
	nsbox<T> operator+(const nsvec3<T> & rhs)
	{
		return nsbox<T>(min+rhs,max+rhs);
	}

	nsbox<T> operator-(const nsvec3<T> & rhs)
	{
		return nsbox<T>(min-rhs,max-rhs);		
	}

	nsbox<T> & operator+=(const nsvec3<T> & rhs)
	{
		min+=rhs;
		max+=rhs;
		return *this;
	}
	
	nsbox<T> & operator-=(const nsvec3<T> & rhs)
	{
		min-=rhs;
		max-=rhs;
		return *this;		
	}
};

template<class PUPer, class T>
void pup(PUPer & p, nsbox<T> & box, const nsstring & var_name_)
{
	pup(p, box.min, var_name_ + ".min");
	pup(p, box.max, var_name_ + ".max");
}


typedef nsbox<char> cbox;
typedef nsbox<char16> c16box;
typedef nsbox<char32> c32box;
typedef nsbox<wchar> cwbox;
typedef nsbox<int8> i8box;
typedef nsbox<int16> i16box;
typedef nsbox<int32> ibox;
typedef nsbox<int64> i64box;
typedef nsbox<uint8> ui8box;
typedef nsbox<uint16> ui16box;
typedef nsbox<uint32> uibox;
typedef nsbox<uint64> ui64box;
typedef nsbox<float> fbox;
typedef nsbox<double> dbox;
typedef nsbox<ldouble> ldbox;

template<class T>
bool point_in_rect(const nsvec2<T> & point, nsvec2<T> rect[4])
{
    nsvec2<T> v = point - rect[0];
    nsvec2<T> v1 = rect[1] - rect[0];
    nsvec2<T> v2 = rect[2] - rect[0];
    T f1 = v * v1, f2 = v1 * v1, f3 = v * v2, f4 = v2 * v2;
    return (0 <= f1 && f1 <= f2) && (0 <= f3 && f3 <= f4);	
}

template<class T>
nsvec2<T> solve_quadratic(const nsvec3<T> abc)
{
	nsvec2<T> ret;
	T rhs = T(sqrt(abc.y*abc.y - 4*abc.x*abc.z)) / (2 * abc.x);
	T lhs = abc.y*T(-1) / (2 * abc.x);
	ret.x = lhs + rhs;
	ret.y = lhs - rhs;
}

template<class T>
int collision_plane_sphere(const nsvec4<T> & plane, const nsvec4<T> & sphere)
{
	T dotp = dot(plane.xyz(), sphere.xyz()) - plane.w; 
    if(dotp > sphere.w)
        return 1; // The sphere is in front of the plane
    else if(dotp < -sphere.w)
        return 2; // The sphere is behind the plane
    return 3; // The sphere collides/straddles with the plane
}

template<class T>
int collision_plane_aabb(const nsvec4<T> & plane, const nsbox<T> & aabb)
{
    // Get the Extense vector
	
    nsvec3<T> ext = (aabb.max - aabb.min)/2.0f;
    
    // Get the center of the Box
    nsvec3<T> center = aabb.min + ext;

	nsvec3<T> norm = plane.xyz();
	
    // Dot Product between the plane normal and the center of the Axis Aligned Box
    // using absolute values
    float rad = abs(norm.x*ext.x) + abs(norm.y*ext.y) + abs(norm.z*ext.z);
 
    nsvec4<T> sphere(center,rad);
    return collision_plane_sphere(plane, sphere);
}

template<class T>
bool collision_aabb_aabb(const nsbox<T> & aabb1, const nsbox<T> & aabb2, int8 mask=0)
{
    //Check if Box1's max is greater than Box2's min and Box1's min is less than Box2's max
    return(
        ((aabb1.max.x >= aabb2.min.x) && (aabb1.min.x <= aabb2.max.x) || (mask & 0x01)) &&
        ((aabb1.max.y >= aabb2.min.y) && (aabb1.min.y <= aabb2.max.y) || (mask & 0x02)) &&
		((aabb1.max.z >= aabb2.min.z) && (aabb1.min.z <= aabb2.max.z) || (mask & 0x04)));
}

template<class T>
bool collision_sphere_sphere(const nsvec4<T> & sphere1, const nsvec4<T> & sphere2)
{
	float dist = (sphere2 - sphere1).xyz().length_sq();
	return dist <= sphere2.w*sphere2.w + sphere1.w*sphere1.w;
}

template<class T>
nsbox<T> transform_obb_to_aabb(const nsbox<T> & obb, const nsmat4<T> & tform)
{
	nsvec3<T> verts[8];
	nsbox<T> ret;

	verts[0] = obb.min;
	verts[1] = nsvec3<T>(obb.max.x, obb.min.y, obb.min.z);
	verts[2] = nsvec3<T>(obb.min.x, obb.max.y, obb.min.z);
	verts[3] = nsvec3<T>(obb.max.x, obb.max.y, obb.min.z);
	verts[4] = nsvec3<T>(obb.min.x, obb.min.y, obb.max.z);
	verts[5] = nsvec3<T>(obb.max.x, obb.min.y, obb.max.z);
	verts[6] = nsvec3<T>(obb.min.x, obb.max.y, obb.max.z);
	verts[7] = obb.max;

	for (uint8 i = 0; i < 8; ++i)
	{
		verts[i] = (tform * verts[i]).xyz();

		if (i == 0)
		{
			ret.min = verts[i];
			ret.max = verts[i];
		}
		
		if (verts[i].x > ret.max.x)
			ret.max.x = verts[i].x;
		if (verts[i].y > ret.max.y)
			ret.max.y = verts[i].y;
		if (verts[i].z > ret.max.z)
			ret.max.z = verts[i].z;

		if (verts[i].x < ret.min.x)
			ret.min.x = verts[i].x;
		if (verts[i].y < ret.min.y)
			ret.min.y = verts[i].y;
		if (verts[i].z < ret.min.z)
			ret.min.z = verts[i].z;
	}
	return ret;
}

#include <vector>
struct nsbounding_box
{
	nsbounding_box(
		const std::vector<fvec3> & verts_ = std::vector<fvec3>()
		);

	~nsbounding_box();

	enum box_face {
		f_none,
		f_bottom,
		f_top,
		f_left,
		f_right,
		f_back,
		f_front
	};

	void calculate(
		const std::vector<fvec3> & verts_,
		const fmat4 & tform_ = fmat4()
		);

	fvec3 center(const box_face & pFace = f_none);

	void clear();

	float dx();
	float dy();
	float dz();

	void extend(
		const std::vector<fvec3> & verts_,
		const fmat4 & tform_ = fmat4()
		);

	void set(
		const fvec3 & min_,
		const fvec3 max_
		);

	float volume();

	fvec3 mMin;
	fvec3 mMax;
	fvec3 mVerts[8];

private:
	void _update_verts();
};

template<class PUPer>
void pup(PUPer & p, nsbounding_box & box, const nsstring & var_name_)
{
	pup(p, box.mMin, var_name_ + ".mMin");
	pup(p, box.mMax, var_name_ + ".mMax");
	for (uint32 i = 0; i < 8; ++i)
		pup(p, box.mVerts[i], var_name_ + "vert[" + std::to_string(i) + "]");
}

#endif
