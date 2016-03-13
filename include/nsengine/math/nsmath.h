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

#include "nsmat4.h"

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
